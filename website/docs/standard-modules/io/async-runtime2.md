# `io::async_runtime2`

`async_runtime2` runs one native completion consumer on Linux, macOS and
Windows. Multiple threads can submit file transfers and TCP or UDP operations
to it. A caller can supply a receiver, await completion in a coroutine frame, or
request a single-consumer future. A positioned transfer with a caller-owned
request requires no per-transfer heap allocation by the runtime.

## Positioned transfers

`runtime.bind(@descriptor descriptor)` borrows an open descriptor and returns
an `io::async_file_binding2`. On Windows, the handle must support overlapped
I/O and must produce completion-port packets for synchronous successes.
The first binding associates the open handle with this runtime's port. Copies
of the originating descriptor reuse the association. A binding borrows the
native file and the runtime, both of which must outlive transfers through it.
Closing the file invalidates every binding for that open file.
After the last transfer, the owner can close the file through `io::file`,
`io::file_ref.close()`, or the originating descriptor's
`close_windows_handle(@close)` member. A POSIX binding borrows the descriptor
directly. [Windows descriptor ownership](io-descriptor-ref.md#close_windows_handle)

The following coroutine issues a positioned read using a request stored in
its own frame. Its caller supplies the binding and the destination storage.

```quxlang
::read_header COROUTINE(@runtime MUT->io::async_runtime2,
  @file io::async_file_binding2, @buffer io::buffer_ref): io::transfer_result
{
  VAR completion io::transfer_waiter2;
  runtime->.read(@file file, @request completion.request, @buffer buffer, @offset 0);
  RETURN AWAIT(completion);
}
```

| Operation | Result |
| --- | --- |
| `read(@file, @request, @buffer, @offset)` | Issues a read and delivers one callback on acceptance. |
| `write(@file, @request, @buffer, @offset)` | Issues a write and delivers one callback on acceptance. |
| `read(@file, @buffer, @offset)` or `write(@file, @buffer, @offset)` | Returns a `std::future#io::transfer_result` and manages the request's lifetime. |
| `submit(@operations, @count)` | Submits a prefix of a contiguous batch and returns the number accepted. |
| `read(@descriptor, @buffer)` or `write(@descriptor, @buffer)` | Operates at the shared file cursor on a blocking worker and returns a future. Windows requires a synchronous handle for these overloads. |

Offsets are unsigned byte positions up to `2⁶³ − 1`. Positioned operations
preserve the shared file cursor. A successful read can return fewer bytes than
requested. At end of file it returns `succeeded == TRUE` and `size == 0`.
Submission errors throw synchronously and leave the rejected request available
for reuse. An accepted operation that fails completes with
`succeeded == FALSE`. Native binding and queue errors use the associated
platform processor's error type. An invalid descriptor passed to `bind` or an
out-of-range offset throws `io::async_error::submission_failed`.

### Request ownership

`io::transfer_request2(@completion receiver)` borrows an object with a
`.complete FUNCTION(@result io::transfer_result) MUT NOEXCEPT` member. The
request has a stable address. Each outstanding transfer needs its own request,
and the file, receiver and buffer must outlive native completion. The receiver
may release or resubmit its request from the callback. It must finish
accessing the previous transfer's buffer before allowing another operation
to use the same bytes. A completion may arrive before the submission call
returns. The consumer invokes callbacks serially, so a callback that blocks
waiting for another callback on that runtime prevents its delivery.

`io::transfer_waiter2` contains its request, result and one continuation
slot. It supports both completion before `AWAIT` and completion after
suspension. A waiter can be reused after its result has been consumed. The caller must
keep a submitted waiter alive until completion. Cancellation of a coroutine
suspended on the waiter preserves its frame until the completion arrives.

### Batches

Each `io::positioned_transfer2(@file, @request, @buffer, @offset, @writing)`
describes a read (`FALSE`) or a write (`TRUE`). The descriptions are borrowed
until `submit` returns. Each accepted operation follows the ordinary request
and buffer lifetime rules. The caller supplies distinct requests and arranges
any required ordering between operations on overlapping byte ranges.

The description constructor borrows a request by reference and validates the
offset once. An empty batch returns zero. On native rejection of the first
operation, the call throws.
A short return identifies the accepted prefix. The caller can submit the suffix
again. Each accepted request produces exactly one completion, and completion
order can differ from submission order.

## File member coroutines

In a coroutine running on `async_runtime2`, calls to `io::file.read`,
`io::file.write`, `io::file_ref.read` and `io::file_ref.write` use the
runtime supplied by `io::async_controller2`. The call accepts an output
`io::transfer_result`. For example, `ON(runtime) RUN read_record(...)`
selects the positioned read in this coroutine.

```quxlang
::read_record COROUTINE(@file io::file_ref, @buffer io::buffer_ref,
  @offset U64): io::transfer_result
{
  VAR result io::transfer_result;
  file.read(@buffer buffer, @offset offset, @result result);
  RETURN result;
}
```

With `@offset`, the member binds the descriptor to the runtime and awaits a
positioned transfer with an `io::transfer_waiter2` in the coroutine frame.
Omitting `@offset` schedules a synchronous cursor operation on the blocking
file pool and awaits its future. Both forms borrow the handle and buffer
through completion. A rejected positioned call or failed positioned transfer
sets `result.succeeded` to `FALSE` and `result.size` to zero. On Windows,
positioned operations require an overlapped handle associated with the
runtime's port. Cursor operations require a synchronous handle.
[File coroutine overloads](file-ref.md#read-and-write-coroutines)

## Native operations and sockets

The socket owners and references of the `net` module accept `async_runtime2`
in `async_accept`, `async_connect`, `async_read`, `async_write`, `async_send`
and `async_receive`. Calling the corresponding socket member from a coroutine
running on this runtime uses a frame-owned native waiter. A socket waiting for
I/O occupies no file worker or scheduler worker. [Socket operations](../net/socket-operations.md)

`submit(@operation operation)` owns a concrete native operation and returns
a future of `operation_type::value_type`. An operation provides
`start(@submission)`, `complete(@submission, @size, @error)` and `result()`.
Both `start` and `complete` return `TRUE` on terminal completion and `FALSE`
after registering one pending notification. `result()` supplies the terminal
value. A submission or completion failure is delivered through the future.
A native operation finishes accessing its submitted state before publishing a
request that another thread can complete.

`io::native_waiter2#operation_type(@runtime, @operation)` borrows the same
protocol in stable storage. Its first `AWAIT` starts the operation. The
waiter keeps its continuation and error state in the calling frame and delivers
the result on resumption. Each waiter serves one operation, and the operation
and its buffers outlive completion.

On Windows, `io::native_submission2.associate(@descriptor CONST& io_descriptor_ref)`
associates a socket with the runtime's port and its socket completion key.
`overlapped()` returns the request header for the native call. Subsequent
submissions through the originating descriptor reuse the port and key.
A different runtime or an incompatible completion key raises
`io::async_error::submission_failed`. A socket operation retains a descriptor copy, which keeps its association
record alive when the start runs later. The caller keeps the socket open
through completion. [Windows descriptor ownership](io-descriptor-ref.md)

On Linux with io_uring, `submit_native(@entry, @request)` submits a single-shot
SQE with a caller-owned `io::uring_request2(@completion)`. The runtime owns
the CQE identity. Each SQE must produce exactly one completion on this ring.
Multishot operations and sends that produce a separate zero-copy notification
violate this requirement. The runtime rejects `IOSQE_CQE_SKIP_SUCCESS` with
`io::io_uring_error::invalid_submission`. Every CQE must carry the identity
of a request submitted by this runtime. An `IORING_OP_MSG_RING` operation
that injects a message CQE into the runtime's ring violates that precondition.

On macOS and Linux with epoll, `wait_ready(@descriptor, @interest, @request)` arms a
one-shot readable or writable interest for an `io::readiness_request2`.
Closing a descriptor requires completion of its pending registrations.
Both request types borrow an `io::native_receiver2` implementing
`on_native_result(@size SZ, @error U32) MUT NOEXCEPT`. A successful readiness
notification reports zero in both fields and permits another nonblocking
transport attempt. Windows uses the native submission's `OVERLAPPED` header
and identifies socket packets by their completion key.

## Scheduling and shutdown

`async_runtime2(@entries, @file_workers, @workers)` defaults to 256 Linux
ring entries, two file workers per blocking pool and one coroutine scheduler
worker. Both worker counts must be positive. Construction starts the native
consumer. Cursor transfers and `ON(runtime) RUN` acquire their respective
worker pools on first use.

`ON(runtime) RUN` selects `io::async_controller2#T`. The scheduler stores the
controller with the coroutine frame in one invocation. The controller exposes
its runtime through `io_runtime()` and submits native operations through
`submit(@operation)`. Nested coroutine calls, grouped tasks, coroutine
construction and coroutine destruction use the same I/O runtime. A file
coroutine can call the file member overloads directly or reuse an explicit
binding and waiter across positioned transfers.

After external producers finish submitting, one external thread can call
`drain()`. It waits for accepted transfers, their callbacks, cursor operations
and scheduled coroutines, including work those operations submit to this
runtime. Callback and worker threads must return control before their own work
can drain. Destruction drains the runtime, wakes its native consumer and joins
its threads.

## Platform storage and dispatch

| Backend | Positioned transfers |
| --- | --- |
| Linux, `io::linux_use_io_uring == TRUE` | One serialized producer path writes submission entries to the ring. Each entry identifies its caller-owned request. The persistent request holds only the receiver. A batch shares one submission-tail publication and can enter the kernel once for multiple transfers. |
| Linux, `io::linux_use_io_uring == FALSE` | File workers run positioned system calls. The request carries the transfer arguments and a queue link. An eventfd signals epoll when a completed list becomes nonempty. |
| macOS | macOS 26 and later deliver native AIO results through kqueue. Earlier supported releases run positioned system calls on file workers and signal kqueue. One request storage slot holds the record for the selected mechanism. |
| Windows | The request stores an `OVERLAPPED`, its receiver and one atomic word. The consumer dequeues packets in batches. If a packet arrives during the initiating native call, the runtime saves its byte count and delivers it after that call returns. |
