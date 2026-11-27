# `io::overlapped_processor`

Module `io` · source file `overlapped_processor.qxs` · Windows

`io::overlapped_processor` owns a Windows I/O completion port. An application
associates each open handle once, supplies storage for each simultaneous
request, and chooses the threads that dispatch completions. A file, socket,
or named pipe that supports overlapped I/O can use the same port.

## Submitting a file write

The following receiver records the outcome of one transfer. The submission
function takes a handle previously associated with the processor. Windows
may complete the write immediately or report `ERROR_IO_PENDING`. In both
cases the consumer receives the byte count and final error in a completion
packet.

```quxlang
LANGUAGE QUXLANG EN 0.0;

IMPORT io;
IMPORT syscall;

::write_result INCLUDE_IF(OS_WINDOWS) STRUCT
{
  .completion VAR io::overlapped_completion_result;

  .on_notify FUNCTION(@completion io::overlapped_completion_result) MUT NOEXCEPT
  { .completion := completion; }
}

::write_at INCLUDE_IF(OS_WINDOWS) FUNCTION(
  @file io::overlapped_handle_ref, @request MUT& io::overlapped_request,
  @data CONST=>>BYTE, @size U32, @offset U64)
{
  AUTO write := -< [=data, =size] (@handle ADDRESS, @overlapped MUT->syscall::windows::overlapped): U32
  {
    IF (syscall::windows::write_file(@handle handle, @buffer data,
      @count size, @transferred NULL, @overlapped overlapped) != 0) { RETURN 0; }
    RETURN syscall::windows::get_last_error();
  };
  file.submit(@request request, @start write, @offset offset);
}
```

For an idle port and an open file described by `descriptor`, the following
sequence writes one byte at offset 16. All storage referenced by Windows or
by the receiver lives through dispatch.

```quxlang
VAR processor io::overlapped_processor;
VAR file := processor.associate(@descriptor descriptor);
VAR result write_result;
VAR request io::overlapped_request :(@operation result);
VAR data [1]BYTE;
data[0] := 42;
write_at(@file file, @request request, @data data[&0], @size 1, @offset 16);
processor.process_completions(@maximum 1, @timeout_milliseconds -1);
```

## Processor members

| Member | Contract |
| --- | --- |
| `CONSTRUCTOR(@concurrency U32 DEFAULT(0))` | Creates the port. The concurrency value limits the number of active consumer threads. Zero selects the Windows default based on the system processor count. |
| `associate(@descriptor io_descriptor_ref): overlapped_handle_ref` | Establishes the native association and returns a copyable reference that borrows the handle and processor. |
| `post(@request MUT& overlapped_request, @transferred U32 DEFAULT(0))` | Queues one successful notification with the specified byte count. A producer can wake a waiting consumer with this method. |
| `process_completions(@maximum SZ, @timeout_milliseconds I32 DEFAULT(0)): SZ` | Dequeues and dispatches at most `maximum` packets on the calling thread. The result is the number of notifications delivered. |
| `pending_count(): SZ` | Returns an advisory count that includes submissions in progress and all requests in batches whose notifications have yet to finish. |
| `DESTRUCTOR()` | Closes the port. Destruction requires a drained port and exclusive access. Destruction with outstanding requests panics. |

An associated handle must support overlapped I/O. A file is opened with
`FILE_FLAG_OVERLAPPED`. The handle requires `FILE_SKIP_COMPLETION_PORT_ON_SUCCESS`
to be clear so the port receives a packet on immediate success. Each operation issued on the handle that can
generate a packet for this port must use a request submitted through its
binding. The native association lasts until the handle is closed. Changing
the associated port requires opening a new handle. The caller owns the handle
and coordinates its closure with outstanding I/O. [Windows handle association](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-createiocompletionport) · [Windows completion modes](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setfilecompletionnotificationmodes)

## Requests and cancellation

`overlapped_request(@operation overlapped_operation_ref)` binds a non-owning
receiver to rooted request storage. The receiver implements
`on_notify(@completion io::overlapped_completion_result) MUT NOEXCEPT`.
A request can be embedded in an application operation or placed in reusable
storage. The receiver and its request each occupy a stable address. At most
one submission or post may use a request at a time.

`overlapped_handle_ref::submit(@request MUT& overlapped_request, @start AUTO& AUTO,
@offset U64 DEFAULT(0))` clears the native `OVERLAPPED`, fills its 64-bit byte
offset, and calls `start(@handle ADDRESS, @overlapped MUT->syscall::windows::overlapped)`
on the submitting thread. The callable issues exactly one native operation
using that handle and pointer. It preserves the null event and enables
completion-port delivery. Its return value is a `U32` Windows error code.

| Submission outcome | Behavior |
| --- | --- |
| Zero or `ERROR_IO_PENDING` (997) | Windows delivers one terminal notification. The byte count comes from that packet. |
| Other Windows error with no packet | `submit` throws `io::overlapped_error` containing the error in `.code`. The request is immediately reusable. |
| Exception from `start` before issuance | `submit` rolls back the outstanding count and propagates the exception. |

`overlapped_handle_ref::cancel(@request MUT& overlapped_request): BOOL`
requests cancellation of one native operation issued on that handle.
`TRUE` means Windows accepted the cancellation request. `FALSE` means
Windows found no matching pending I/O. Other native failures throw
`io::overlapped_error`. A cancellation request can race with ordinary
completion. The terminal notification reports the actual outcome, including
`ERROR_OPERATION_ABORTED` (995) when cancellation succeeds. Cancellation
preserves the original request and its buffers until notification. [Windows cancellation semantics](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-cancelioex)

## Dispatch and concurrency

`process_completions` retrieves up to 32 packets per native call and invokes
each receiver directly. A larger `maximum` permits multiple batches. A zero
maximum returns immediately. A zero timeout polls, a positive timeout waits
that many milliseconds for the first packet, and a negative timeout waits
indefinitely. Once a batch arrives, further dequeues in the same call poll.
A timeout returns the count dispatched so far. A native wait failure throws
`io::overlapped_error`. [Windows batch dequeue](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getqueuedcompletionstatusex)

`overlapped_completion_result.transferred` is the packet's `U32` byte count.
`error` is zero for success or a Windows system error code for a failed
operation. Dispatch reads each request's native status and translates a
failure individually. A batch can carry both successful and failed
operations. [Windows status conversion](https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-rtlntstatustodoserror)

Multiple producers and consumers can use the same processor concurrently.
Each consumer uses its own stack buffer. Dispatch identifies the request
directly from the returned `OVERLAPPED` address. Submission and dispatch
require no processor-owned heap allocation or handle-table lookup. The
processor retires the outstanding count once per dispatched batch.

Completion can begin before the issuing call returns. The producer prepares
all state used by the receiver before issuing native I/O. After Windows
accepts the request, the submission callable can return only an accepted
status and must avoid further access to storage that the receiver can reuse.
The consumer finishes reading the request before entering `on_notify`.
A receiver may therefore resubmit the same request from its callback.
Applications with multiple consumers synchronize access to shared receiver
state and coordinate cancellation with request reuse.
