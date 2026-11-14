# io_uring processor

`io::io_uring_processor` owns a Linux io_uring submission queue and completion
queue. The processor is independent of `io::async_runtime`, coroutine controllers,
and future producers. The interface is available on Linux regardless of the
`io::linux_use_io_uring` option used by the asynchronous runtime.

## Construction

```qxs
VAR processor io::io_uring_processor :(
  @submission_entries 256,
  @completion_entries 512,
  @single_issuer TRUE
);
```

The named arguments have the defaults shown above. The kernel validates queue
capacities and rounds supported capacities to its required sizes. Single-issuer
mode restricts submission to the creating thread. The processor requires `IORING_FEAT_SINGLE_MMAP` for a shared submission/completion
mapping and `IORING_FEAT_NODROP` for completion-overflow retention. Setup failure throws
`io::io_uring_error::initialization_failed`.

## Operation contract

`io::io_uring_operation_ref` borrows an object implementing these methods:

```qxs
.prepare FUNCTION(@entry MUT& io::io_uring_submission_entry) MUT;
.on_notify FUNCTION(@completion io::io_uring_completion_result) MUT;
```

`prepare` initializes a zero-initialized submission entry. The submission entry
uses the ABI layout defined in `syscall::linux::io_uring_submission_entry`.
The processor owns the `user_data` field and replaces its value after preparation.
`IOSQE_CQE_SKIP_SUCCESS` is rejected because omitted notifications prevent
registration retirement. External injection of completion records is prohibited.

The processor allocates a stable registration containing the borrowed interface.
The concrete operation retains a stable address through its terminal notification.
Each kernel-referenced buffer satisfies the lifetime requirements of its opcode
and completion flags. Selected buffers have ownership lifetimes independent of
the registration. `on_notify` receives the signed kernel result,
all completion flags, and `is_final`. A completion is final when
`IORING_CQE_F_MORE` is absent. Multishot operations retain the same registration
across intermediate notifications. A final notification can represent an error
or a partial transfer.

Callbacks execute on the thread calling `process_completions`. They do not block
or recursively invoke completion processing. A callback finishes accessing its
operation before publishing a continuation that can destroy the operation.
The processor releases its completion reference after a terminal callback returns
or throws. A queued cancellation retains the target registration until its own
terminal callback finishes, preventing identity reuse during cancellation. An exception from an intermediate callback retains its registration;
the caller must continue processing or cancel the operation.

## Submission and cancellation

`enqueue(@operation ...)` prepares an entry and returns an opaque
`io::io_uring_submission_id`. Queue exhaustion throws `queue_full` without
retaining the operation reference. Preparation may already have executed.
`flush()` submits queued entries and supports batching. Submission failure leaves
unconsumed entries queued for retry.

`enqueue_cancel(@submission ..., @notification ...)` queues cancellation and
returns the cancellation request's own identity. The notification object's
`prepare` method is not called. Its `on_notify` receives the cancellation result.
The original operation survives until its own terminal notification.

An identity is valid only in its originating processor and expires when its
terminal callback begins. Cancellation callers coordinate with completion
consumption to prevent use of expired identities. Identities are not durable
handles for completed operations.

## Completion processing and observation

`has_completions()` returns an advisory snapshot without consuming records.
`process_completions(@maximum ...)` dispatches a bounded number of records and
returns the number dispatched. Kernel overflow records are retrieved when the
mapped completion queue becomes empty. Callback exceptions propagate after the
current record has been consumed; subsequent records remain available.

`wait(@minimum 1, @timeout_nanoseconds -1)` waits without submitting entries or
invoking callbacks. Negative timeouts select an indefinite wait. Nonnegative
values use the extended kernel timeout ABI. The method returns false on timeout
or signal interruption and throws `wait_failed` for other kernel failures.
The minimum count must fit the completion queue. `pending_count()` includes
queued submissions and registrations whose terminal callbacks have not finished.

`register_completion_event(@descriptor ...)` registers a borrowed eventfd with
the kernel. Other threads can observe that eventfd without consuming completion
records. Notifications indicate that the completion queue should be checked;
the eventfd counter does not count completed operations. Registration uses the
ordinary eventfd notification mode, including completions produced during
submission. `unregister_completion_event()` removes the registration.

## Concurrency and destruction

Submission methods require one serialized producer. Completion processing and
waiting require one serialized consumer. These roles may run concurrently on
different threads. Configuration and destruction require exclusive access.
The processor contains no scheduler locks, timestamps, or worker policy. A
scheduler can serialize completion consumers to transfer ownership to a monitor.

All registrations must finish before destruction. Destruction with pending
registrations violates the lifetime contract and terminates execution. A shutdown
sequence stops new submissions, flushes queued work, requests cancellation where
appropriate, and consumes terminal notifications before destroying the processor.
