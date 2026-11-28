# `io::io_uring_processor`

Module `io` · source file `io_uring_processor.qxs`

`io::io_uring_processor` owns one Linux io_uring: the ring descriptor, the
shared submission and completion ring mapping, and the submission entry
mapping. The processor queues prepared submission entries, submits them in
batches, and dispatches completion records to borrowed operation objects on the
thread that asks for them. The processor is independent of
[`io::async_context`](async-context.md),
[`io::async_runtime`](async-runtime.md), coroutine controllers, and futures; it
starts no thread and performs no internal locking. The processor is available
on Linux for both values of the option
[`io::linux_use_io_uring`](index.md#module-options), which selects the
Linux backends of `io::async_context` and `io::async_runtime2`.

## Declarations

```quxlang
::io_uring_error ENUM
[
  initialization_failed, resource_release_failed, queue_full,
  submission_failed, registration_failed, wait_failed, invalid_submission
];
::io_uring_submission_entry INCLUDE_IF(OS_LINUX) ALIAS syscall::linux::io_uring_submission_entry;
::io_uring_completion_result STRUCT
::io_uring_operation_ref INCLUDE_IF(OS_LINUX) GENERIC_REF INCOMPARABLE
::io_uring_submission_id STRUCT
::io_uring_processor INCLUDE_IF(OS_LINUX) STRUCT ROOTED
```

| Entity | Description |
| --- | --- |
| [`io::io_uring_processor`](#member-summary) | Owner of the ring. |
| [`io::io_uring_operation_ref`](#ioio_uring_operation_ref) | Borrowed interface of an operation object. |
| [`io::io_uring_submission_entry`](#ioio_uring_submission_entry) | The native submission entry layout. |
| [`io::io_uring_completion_result`](#ioio_uring_completion_result) | One notification delivered to an operation. |
| [`io::io_uring_submission_id`](#ioio_uring_submission_id) | Identity of a live submission, used for cancellation. |
| [`io::io_uring_error`](#ioio_uring_error) | Exception type of the processor. |

`ROOTED` removes copy and move construction, so a processor remains at its
address for its whole lifetime.

## Concurrency contract

The processor defines three roles and requires the caller to serialize each
role:

| Role | Member functions | Requirement |
| --- | --- | --- |
| Producer | [`enqueue`](#enqueue), [`enqueue_cancel`](#enqueue_cancel), [`flush`](#flush), [`submit`](#submit) | One serialized producer. In single-issuer mode, the producer is the thread that constructed the processor. |
| Consumer | [`process_completions`](#process_completions), [`wait`](#wait) | One serialized consumer. |
| Exclusive | Construction, destruction, [`register_completion_event`](#register_completion_event), [`unregister_completion_event`](#unregister_completion_event) | No other access to the processor. |

The producer and the consumer can run concurrently on different threads. A
notification callback can enqueue work when the producer serialization is
satisfied. [`has_completions`](#has_completions) and
[`pending_count`](#pending_count) read atomic state and return advisory
snapshots.

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates and maps the ring. |
| [`DESTRUCTOR`](#destructor) | Releases a quiescent ring. |

**Submission**

| Member | Description |
| --- | --- |
| [`enqueue`](#enqueue) | Prepares one operation and queues its entry. |
| [`enqueue_cancel`](#enqueue_cancel) | Queues the cancellation of a live submission. |
| [`flush`](#flush) | Submits every queued entry to the kernel. |
| [`submit`](#submit) | Prepares one operation and submits it to the kernel in the same call. |

**Completion**

| Member | Description |
| --- | --- |
| [`wait`](#wait) | Waits for completion records without dispatching them. |
| [`process_completions`](#process_completions) | Dispatches a bounded number of completion records. |
| [`has_completions`](#has_completions) | Reports whether completion records are available. |
| [`pending_count`](#pending_count) | Returns the number of unfinished registrations. |

**Completion event**

| Member | Description |
| --- | --- |
| [`register_completion_event`](#register_completion_event) | Registers an eventfd that the kernel signals for completions. |
| [`unregister_completion_event`](#unregister_completion_event) | Removes the eventfd registration. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@submission_entries U32 DEFAULT(256), @completion_entries U32 DEFAULT(512), @single_issuer BOOL DEFAULT(TRUE))
```

Creates one io_uring with the requested queue capacities and maps its rings.
The constructor always requests an explicit completion queue size
(`IORING_SETUP_CQSIZE`). The processor uses the capacities that the kernel
reports after setup.

The processor requests `IORING_SETUP_NO_SQARRAY` so the kernel can consume
submission entries directly by slot number. If setup returns `EINVAL`, the
processor retries with the submission index array and initializes the indices
once during construction.

**Parameters**:

- `submission_entries` is the requested capacity of the submission queue.
- `completion_entries` is the requested capacity of the completion queue.
- `single_issuer` requests single-issuer mode (`IORING_SETUP_SINGLE_ISSUER`),
  which binds submission to the thread that creates the ring.

**Exceptions**: `io_uring_error::initialization_failed` is thrown when the
setup system call fails, when the kernel does not report both
`IORING_FEAT_SINGLE_MMAP` (one shared mapping for both rings) and
`IORING_FEAT_NODROP` (retention of completion records that overflow the
completion queue), or when a ring mapping fails. The constructor releases every
resource that it acquired before the exception propagates.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Unmaps both mappings and closes the ring descriptor.

**Preconditions**: every registration has received its final notification, so
that [`pending_count`](#pending_count) returns zero. The destructor evaluates
this requirement in every build configuration: a nonzero count results in
`PANIC`.

**Exceptions**: a failed unmap or close operation throws
`io_uring_error::resource_release_failed` after every release has been
attempted.

A shutdown sequence that satisfies the precondition stops new submissions,
flushes queued entries, queues cancellations for operations that do not
complete on their own, and processes completions until `pending_count` returns
zero.

### `enqueue`

```quxlang
.enqueue FUNCTION(@operation io_uring_operation_ref) MUT: io_uring_submission_id
```

Calls `operation.prepare` with a default-initialized submission entry, stores
the identity of a new registration in the `user_data` field of the entry, and
places the entry in the submission queue. The function does not enter the
kernel and dispatches no callback; [`flush`](#flush) submits the entry.

**Parameters**: `operation` borrows the operation object. The registration
keeps the borrowed reference until the final notification of the submission.

**Return value**: the identity of the new submission.

**Preconditions**: the operation object remains at one address, and alive,
until its final notification has been delivered. Storage that the prepared
entry refers to satisfies the lifetime requirements of its opcode.

**Exceptions**:

- `io_uring_error::invalid_submission` is thrown when the prepared entry sets
  the flag `IOSQE_CQE_SKIP_SUCCESS`. A submission without a completion record
  would never release its registration.
- `io_uring_error::queue_full` is thrown when the submission queue has no free
  slot.

In both cases the processor retains no reference to the operation, and
`pending_count` is unchanged. `prepare` has already been called when either
exception is thrown.

### `enqueue_cancel`

```quxlang
.enqueue_cancel FUNCTION(@submission io_uring_submission_id, @notification io_uring_operation_ref) MUT: io_uring_submission_id
```

Queues an asynchronous cancellation request for the submission that
`submission` identifies. The cancellation is a submission of its own with its
own registration.

**Parameters**: `submission` is a live identity that this processor returned.
`notification` borrows the object that receives the result of the cancellation
request; its `prepare` function is not called.

**Return value**: the identity of the cancellation request.

The original operation remains registered until its own final notification,
which reports the outcome of the cancelled operation. The cancellation
registration holds a reference to the target registration until the final
notification of the cancellation has been delivered, so the identity of the
target is not reused during that time.

**Preconditions**: `submission` was returned by this processor and has not
expired, as [`io::io_uring_submission_id`](#ioio_uring_submission_id)
specifies.

**Exceptions**: `io_uring_error::queue_full`, under the condition of
[`enqueue`](#enqueue).

### `flush`

```quxlang
.flush FUNCTION() MUT
```

Enters the kernel repeatedly until the kernel has consumed every queued entry.
The function returns immediately when the submission queue is empty. A system
call that a signal interrupts is repeated.

**Exceptions**: `io_uring_error::submission_failed` is thrown when the system
call fails or consumes no entry. Entries that the kernel has not consumed
remain queued, and a later call submits them.

### `submit`

```quxlang
.submit FUNCTION(@operation io_uring_operation_ref) MUT: io_uring_submission_id
```

Calls `operation.prepare`, registers the operation, and enters the kernel
before returning. The function combines [`enqueue`](#enqueue) and
[`flush`](#flush) for a producer that submits each operation as it is issued.
The notification of the operation can begin on the consumer thread before the
function returns.

**Preconditions**:

- The submission queue holds no entry queued by [`enqueue`](#enqueue) or
  [`enqueue_cancel`](#enqueue_cancel). A producer uses either `submit` or the
  `enqueue` and `flush` pair.
- The prepared entry does not set `IOSQE_CQE_SKIP_SUCCESS`.

**Return value**: the identity of the submission.

**Exceptions**: `io_uring_error::invalid_submission` under the condition of
[`enqueue`](#enqueue), and `io_uring_error::submission_failed` when the system
call fails. A failed call withdraws the entry and retains no reference to the
operation.

`io::async_runtime2` submits every io_uring operation through this function
while it holds its producer lock.

### `wait`

```quxlang
.wait FUNCTION(@minimum U32 DEFAULT(1), @timeout_nanoseconds I64 DEFAULT(-1)) MUT: BOOL
```

Blocks until at least `minimum` completion records are available or the timeout
expires. The function submits no entry and dispatches no callback.

**Parameters**: `minimum` is the number of completion records to wait for. A
negative `timeout_nanoseconds` waits without a time limit. A nonnegative value
is the time limit in nanoseconds; the function passes it through the extended
argument form of the enter system call.

**Return value**: `TRUE` when the wait succeeded. `FALSE` when the time limit
expired or a signal interrupted the wait.

**Exceptions**: any other failure of the system call throws
`io_uring_error::wait_failed`.

### `process_completions`

```quxlang
.process_completions FUNCTION(@maximum SZ) MUT: SZ
```

Removes at most `maximum` completion records from the completion queue and, for
each record, calls `on_notify` of the registered operation on the calling
thread. The function does not block. When the mapped completion queue is empty
and the kernel reports retained overflow records, the function enters the
kernel once to move those records into the queue.

**Return value**: the number of records dispatched.

A record is *final* when the completion flag `IORING_CQE_F_MORE` is absent.
After the callback for a final record returns, the processor releases the
registration and decrements [`pending_count`](#pending_count). A multishot
submission delivers several records through one registration; every record
except the last has `is_final` equal to `FALSE`.

**Exceptions**: an exception that a callback throws propagates to the caller
after the record has been consumed. Records that were not yet dispatched remain
in the queue. A registration whose final callback throws is released. A
registration whose intermediate callback throws is retained, and the caller
continues processing or cancels the submission. A failure of the system call
that retrieves retained overflow records throws `io_uring_error::wait_failed`.

**Preconditions**: every completion record of the ring originates from a
submission of this processor. Completion records injected from another ring
are prohibited.

### `has_completions`

```quxlang
.has_completions FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the mapped completion queue is nonempty or the
kernel reports retained overflow records. The value is an advisory snapshot;
the function consumes no record and dispatches no callback.

### `pending_count`

```quxlang
.pending_count FUNCTION() CONST: SZ
```

**Return value**: the number of registrations, queued or submitted, whose final
notification has not finished. Cancellation requests are included.

### `register_completion_event`

```quxlang
.register_completion_event FUNCTION(@descriptor I32) MUT
```

Registers the eventfd `descriptor` with the ring. The kernel signals the
eventfd when it posts completion records, so a thread other than the consumer
can observe the eventfd without consuming records. The processor borrows the
descriptor and does not close it.

**Exceptions**: a rejected registration throws
`io_uring_error::registration_failed`.

### `unregister_completion_event`

```quxlang
.unregister_completion_event FUNCTION() MUT
```

Removes the eventfd registration.

**Exceptions**: a rejected request throws
`io_uring_error::registration_failed`.

## `io::io_uring_operation_ref`

```quxlang
::io_uring_operation_ref INCLUDE_IF(OS_LINUX) GENERIC_REF INCOMPARABLE
{
  .prepare FUNCTION(@entry MUT& io_uring_submission_entry) MUT;
  .on_notify FUNCTION(@completion io_uring_completion_result) MUT;
}
```

`io::io_uring_operation_ref` is a non-owning generic reference to an object
that supplies the two member functions. An object of a type with both members
converts to the reference at the call of `enqueue` or `enqueue_cancel`.
[Generics](../../reference/generics.md) specifies `GENERIC_REF`.

- `prepare` initializes the submission entry of the operation: the opcode, the
  descriptor, and the opcode-specific fields. The processor overwrites
  `user_data` afterwards.
- `on_notify` receives one notification. The function does not block and does
  not call `process_completions`. A callback finishes every access to its own
  object before it publishes a continuation that can destroy the object.

## `io::io_uring_submission_entry`

```quxlang
::io_uring_submission_entry INCLUDE_IF(OS_LINUX) ALIAS syscall::linux::io_uring_submission_entry;
```

The alias names the 64-byte native submission entry layout that the `syscall`
module declares, with the following fields:

| Field | Type | Field | Type |
| --- | --- | --- | --- |
| `opcode` | `U8` | `operation_flags` | `U32` |
| `flags` | `U8` | `user_data` | `syscall::linux::io_uring_address` |
| `priority` | `U16` | `buffer_index` | `U16` |
| `descriptor` | `I32` | `personality` | `U16` |
| `offset` | `U64` | `splice_descriptor` | `I32` |
| `address` | `syscall::linux::io_uring_address` | `address_3` | `U64` |
| `length` | `U32` | `reserved` | `U64` |

The processor reserves `user_data`. A default-initialized entry has opcode
zero, which is the no-operation request.

## `io::io_uring_completion_result`

```quxlang
::io_uring_completion_result STRUCT
{
  .result VAR I32;
  .flags VAR U32;
  .is_final VAR BOOL;
}
```

| Member | Description |
| --- | --- |
| `result` | The signed result of the completion record. A negative value is a negated Linux error number. |
| `flags` | All completion flags of the record. |
| `is_final` | `TRUE` when the record is the last notification of its submission. |

A final notification can report an error or a partial transfer.

## `io::io_uring_submission_id`

```quxlang
::io_uring_submission_id STRUCT
```

An `io_uring_submission_id` identifies one live submission in the processor
that returned it. The structure has no public member; its only use is as the
`submission` argument of [`enqueue_cancel`](#enqueue_cancel). An identity
expires when the final callback of its submission begins. A cancellation that
was queued before that point remains valid, because the cancellation
registration keeps the target registration allocated. The program coordinates
cancellation with completion processing so that no expired identity is passed
to `enqueue_cancel`. An identity is not a durable handle for a completed
operation.

## `io::io_uring_error`

| Enumerator | Thrown by | Condition |
| --- | --- | --- |
| `initialization_failed` | [`CONSTRUCTOR`](#constructor) | Setup, feature, or mapping failure. |
| `resource_release_failed` | [`DESTRUCTOR`](#destructor), and the constructor when it releases resources after a failure | Failed unmap or close. |
| `queue_full` | [`enqueue`](#enqueue), [`enqueue_cancel`](#enqueue_cancel) | No free submission slot. |
| `submission_failed` | [`flush`](#flush), [`submit`](#submit) | The enter system call failed or consumed no entry. |
| `registration_failed` | [`register_completion_event`](#register_completion_event), [`unregister_completion_event`](#unregister_completion_event) | The register system call failed. |
| `wait_failed` | [`wait`](#wait), [`process_completions`](#process_completions) | The enter system call failed for a reason other than a timeout or a signal. |
| `invalid_submission` | [`enqueue`](#enqueue) | The prepared entry requests suppression of its completion record. |

## Target restrictions

| Declaration | Declared when |
| --- | --- |
| `io_uring_processor`, `io_uring_operation_ref`, `io_uring_submission_entry` | `OS_LINUX` |
| `io_uring_error`, `io_uring_completion_result`, `io_uring_submission_id` | Every target |

The processor requires a kernel that provides io_uring with the features
`IORING_FEAT_SINGLE_MMAP` and `IORING_FEAT_NODROP`. The constructor throws
`io_uring_error::initialization_failed` on a kernel that rejects the requested
setup flags or does not report both features.

## Example

The following fragment is taken from the unit tests of the processor
(`main_test_io_uring_processor.qxs`). The fragment requires Linux and was not
executed for this page. The operation type prepares a no-operation entry and
records each notification; the test queues two operations in a ring with two
submission slots, observes `queue_full` for a third, and processes the two
completions one at a time.

```quxlang
::operation STRUCT
{
  .descriptor VAR I32;
  .poll VAR BOOL;
  .fail_notification VAR BOOL;
  .skip_completion VAR BOOL;
  .notifications VAR SZ;
  .final_notifications VAR SZ;
  .last_result VAR I32;
  DOC <$ Prepares a no-op or a multishot readable-descriptor poll. $>
  .prepare FUNCTION(@entry MUT& io::io_uring_submission_entry) MUT
  {
    IF (.skip_completion) { entry.flags := 64; }
    IF (.poll)
    {
      entry.opcode := 6;
      entry.descriptor := .descriptor;
      entry.length := 1;
      entry.operation_flags := 1;
      STATIC_IF(ARCH_IS_Z_ARCH) { entry.operation_flags := entry.operation_flags #++ 16; }
    }
  }
  DOC <$ Records intermediate updates and terminal notifications separately. $>
  .on_notify FUNCTION(@completion io::io_uring_completion_result) MUT
  {
    .notifications := .notifications + 1;
    .last_result := completion.result;
    IF (completion.is_final) { .final_notifications := .final_notifications + 1; }
    IF (.fail_notification) { THROW (19 AS I32); }
  }
}

DOC <$ Queued operations submit in batches and completion processing respects its bound. $>
::batch UNIT_TEST
{
  VAR processor io::io_uring_processor :(@submission_entries 2, @completion_entries 4);
  VAR first operation;
  VAR second operation;
  VAR third operation;
  processor.enqueue(@operation first);
  processor.enqueue(@operation second);
  VAR exhausted BOOL;
  TRY { processor.enqueue(@operation third); }
  CATCH failure CONST& io::io_uring_error { exhausted := failure == io::io_uring_error::queue_full; }
  TEST_ASSERT(exhausted);
  TEST_ASSERT(processor.pending_count() == 2);
  processor.flush();
  TEST_ASSERT(processor.wait(@minimum 2, @timeout_nanoseconds 2000000000));
  TEST_ASSERT(processor.has_completions());
  TEST_ASSERT(processor.process_completions(@maximum 1) == 1);
  TEST_ASSERT(processor.pending_count() == 1);
  TEST_ASSERT(processor.process_completions(@maximum 1) == 1);
  TEST_ASSERT(first.final_notifications == 1 && second.final_notifications == 1);
  TEST_ASSERT(first.last_result == 0 && second.last_result == 0);
  TEST_ASSERT(processor.pending_count() == 0);
  TEST_ASSERT(processor.wait(@timeout_nanoseconds 1000000) == FALSE);
}
```

## See also

- [`io::async_context`](async-context.md)
- [Native operations](native-operation.md)
- [Module `io`](index.md)
- [Generics](../../reference/generics.md)
- [Tests](../../reference/tests.md)
- [Availability and Targets](../../reference/availability-and-targets.md)
