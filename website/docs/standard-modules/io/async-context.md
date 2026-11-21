# `io::async_context`

Module `io` · source files `async_io.qxs`, `async_io_linux.qxs`, `async_io_epoll.qxs`, `async_io_macos.qxs`, `async_io_windows.qxs`

`io::async_context` owns one native completion queue: an io_uring or an epoll
instance on Linux, a kqueue on macOS, or an I/O completion port on Windows. The
context accepts positioned file transfers with completion callbacks and
[native operations](native-operation.md) with futures. The context starts no
thread for callbacks: a callback runs on a thread that calls
[`wait_one`](#wait_one) or on a worker of an attached
[`io::thread_pool`](thread-pool.md). [`io::async_runtime`](async-runtime.md)
combines a context, a worker pool, and a coroutine scheduler, and is the
interface intended for coroutine code. `io::async_context` is the lower-level
interface with explicit callbacks.

## Declaration

```quxlang
::async_context INCLUDE_IF(OS_LINUX || OS_WINDOWS || OS_MACOS)
  STRUCT MOVE_ONLY

::async_error ENUM [initialization_failed, resource_release_failed, worker_stop_failed, submission_failed];
```

`MOVE_ONLY` removes the copy operations. The context keeps its native queue
state in a separate allocation with a stable address, so moving a context does
not move the queue state. The structure is declared on Linux, macOS, and
Windows.

The source states several requirements with `ASSERT`. An `ASSERT` is evaluated
only when the `ASSERT_ENABLED` policy is enabled; a violated requirement that is
not evaluated results in undefined behavior.
[Diagnostics and Failure](../../reference/diagnostics-and-failure.md) and
[Compilation Policies](../../reference/compilation-policies.md) specify the
policy.

### Backend aliases

```quxlang
::io_uring_async_runtime INCLUDE_IF(OS_LINUX && linux_use_io_uring) ALIAS async_context;
::epoll_async_runtime INCLUDE_IF(OS_LINUX && linux_use_io_uring == FALSE) ALIAS async_context;
::kqueue_async_runtime INCLUDE_IF(OS_MACOS) ALIAS async_context;
::overlapped_async_runtime INCLUDE_IF(OS_WINDOWS) ALIAS async_context;
```

Source file `async_runtime.qxs` declares one alias per backend. Exactly one
alias exists on each supported target, and the alias names the same type as
`io::async_context`. The alias name identifies the mechanism that the target
selects; [Platform implementations](#platform-implementations) describes each
mechanism.

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates the native queue or transfers ownership of a context. |
| [`DESTRUCTOR`](#destructor) | Releases the native queue. |

**Submission**

| Member | Description |
| --- | --- |
| [`read`](#read) | Submits a positioned read with a completion callback. |
| [`write`](#write) | Submits a positioned write with a completion callback. |
| [`submit`](#submit) | Submits an owned native operation and returns its future. |

**Completion processing**

| Member | Description |
| --- | --- |
| [`wait_one`](#wait_one) | Waits for one completion and runs its callback on the calling thread. |
| [`drain`](#drain) | Waits until no operation is outstanding. |

**Observers**

| Member | Description |
| --- | --- |
| [`valid`](#valid) | Reports whether the native queue exists. |
| [`outstanding`](#outstanding) | Returns the number of unfinished operations. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@entries SZ)                         // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& async_context)          // (2)
```

1. Allocates the stable queue state and creates the native completion queue.
   A failure to create the native queue does not throw; the context is then
   constructed with [`valid`](#valid) equal to `FALSE`.
2. Transfers the queue state from `OTHER`. `OTHER` is left without queue state:
   its `valid` function returns `FALSE` and its `outstanding` function returns
   zero. Objects bound to the source context, such as an
   [`io::async_file`](async-file.md) or an [`io::thread_pool`](thread-pool.md),
   are bound to the destination context after the move, because they refer to
   the queue state, which the move transfers without relocating it.

**Parameters**: `entries` is the capacity of the context. The value is the upper
bound for the worker count of an attached `io::thread_pool`. On Linux with
io_uring, the value is the number of submission slots requested from the kernel.
On Windows, the value is the concurrency value of the completion port. The
macOS and Linux epoll backends do not pass the value to the operating system.

**Checked failures** (1): `ASSERT` requires `entries` to be nonzero and at most
4,294,967,295.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Releases the native queue and the queue state. Destroying a moved-from context
performs no operation.

**Checked failures**: `ASSERT` requires that no operation is outstanding, that
no `io::thread_pool` is attached, and that no thread is inside `wait_one`.

**Exceptions**: a failure to release a native resource throws
`async_error::resource_release_failed`. The queue state is deallocated in every
case.

### `read`

```quxlang
.read FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref, @offset U64, @callback AUTO& AUTO(callback_type))
```

Submits a positioned read of at most `buffer.size` bytes at byte offset `offset`
of the borrowed descriptor. The file position of the descriptor is not used.

**Parameters**:

- `descriptor` is the [`io::io_descriptor_ref`](io-descriptor-ref.md) to read
  from. The context does not take ownership.
- `buffer` designates the destination bytes.
- `offset` is the byte offset in the file.
- `callback` is an object that is callable as
  `callback(@result transfer_result)`. The context takes ownership of the
  callable: a temporary is moved, and any other argument is copied.

**Completion**: the context invokes the callable exactly once with an
[`io::transfer_result`](buffer-ref.md#iotransfer_result) and then destroys it.

- When the native queue accepts the request, the invocation occurs on a thread
  that consumes completions: a caller of [`wait_one`](#wait_one) or a worker of
  an attached [`io::thread_pool`](thread-pool.md).
- When the native queue rejects the request, the invocation occurs before `read`
  returns, on the calling thread, with `succeeded` equal to `FALSE` and `size`
  equal to zero.

A successful result reports the number of bytes transferred, which can be less
than `buffer.size`; zero bytes into a nonempty buffer denotes end of file. A
failed native transfer reports `succeeded` equal to `FALSE` and `size` equal to
zero. One operation transfers at most 2,147,483,647 bytes.

**Preconditions**: the descriptor and the storage that `buffer` designates
remain valid until the callable returns. Objects that the callable refers to
remain alive until the callable returns. The callable does not throw.

**Checked failures**: `ASSERT` requires [`valid`](#valid) to return `TRUE`.

**Target restrictions**: on Windows, the handle supports overlapped I/O. The
first submission associates the handle with this context's completion port.
Subsequent submissions to this context reuse that binding. A handle that
cannot be associated, including a handle bound to another completion port,
produces an immediate unsuccessful completion. After all operations complete,
the handle is closed through `file_ref::close` or
[`io::close_windows_async_handle`](native-operation.md#close_windows_async_handle).
On macOS, an `offset` above 9,223,372,036,854,775,807 produces an immediate
unsuccessful completion.

### `write`

```quxlang
.write FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref, @offset U64, @callback AUTO& AUTO(callback_type))
```

Submits a positioned write of at most `buffer.size` bytes at byte offset
`offset` of the borrowed descriptor. The parameters, completion rules,
preconditions, checked failures, and target restrictions are those of
[`read`](#read). The referenced bytes are not modified. A successful result can
report fewer bytes than `buffer.size`.

### `submit`

```quxlang
.submit FUNCTION(@operation AUTO& AUTO(operation_type)): std::future#(operation_type::value_type)
```

Takes ownership of `operation`, places it at a stable address, starts it, and
returns a [`std::future`](../std/future.md) for its result. The operation type
supplies the members `value_type`, `start`, `complete`, and `result`;
[Native operations](native-operation.md) specifies the protocol.

**Return value**: a future that becomes ready when the operation completes. An
exception that the operation throws is delivered through the future.

**Checked failures**: `ASSERT` requires [`valid`](#valid) to return `TRUE`.

### `wait_one`

```quxlang
.wait_one FUNCTION(): BOOL
```

Blocks the calling thread until the native queue delivers one event, and then
processes that event on the calling thread. For a file transfer, processing
invokes the completion callback. For a native operation, processing invokes the
`complete` member of the operation and, when the operation finishes, publishes
its result.

**Return value**: `TRUE` when the function processed a completion or a readiness
notification. `FALSE` when the function consumed a worker stop event.

The function has no timeout. A call made while no operation is outstanding does
not return until another thread submits an operation. Several threads can call
`wait_one` on the same context.

**Checked failures**: `ASSERT` requires [`valid`](#valid) to return `TRUE` and
requires that no `io::thread_pool` is attached to the context. A failure of the
native wait call other than an interruption by a signal results in `PANIC`.

### `drain`

```quxlang
.drain FUNCTION()
```

Blocks the calling thread until the number of outstanding operations is zero.
The function does not process completions. Another thread, such as a worker of
an attached `io::thread_pool`, consumes the completions that allow the count to
reach zero.

**Preconditions**: the context is not moved-from. Submissions from other threads
stop before the call. The function is not called from a completion callback.

### `valid`

```quxlang
.valid FUNCTION(): BOOL
```

**Return value**: `TRUE` when the context holds queue state and the native queue
was created. `FALSE` for a moved-from context and for a context whose native
queue could not be created.

### `outstanding`

```quxlang
.outstanding FUNCTION(): SZ
```

**Return value**: the number of submitted operations that are not yet retired.
A file transfer is retired after its callback returns. A native operation is
retired after its final notification. A moved-from context returns zero.

## `io::async_error`

```quxlang
::async_error ENUM [initialization_failed, resource_release_failed, worker_stop_failed, submission_failed];
```

`io::async_error` is the exception type of the asynchronous facilities of the
`io` module. The enumeration is declared on every target.

| Enumerator | Meaning | Thrown by |
| --- | --- | --- |
| `initialization_failed` | The native completion queue is unavailable. | The constructor of [`io::async_runtime`](async-runtime.md). |
| `resource_release_failed` | A native queue close or unmap operation failed. | The destructor of `io::async_context`. |
| `worker_stop_failed` | A worker stop notification could not be posted. | [`io::thread_pool::join`](thread-pool.md#join) and the constructor of `io::thread_pool`. |
| `submission_failed` | The native queue rejected an operation registration. | The members of [`io::async_submission_ref`](native-operation.md#ioasync_submission_ref). |

File transfers do not throw `async_error`; they report a rejected submission
through an unsuccessful `transfer_result`.

## Platform implementations

The public interface is identical on every supported target. The mechanism
behind the interface is selected at compile time.

| Target | Alias | Native queue | Positioned file transfer | Worker stop event |
| --- | --- | --- | --- | --- |
| Linux, `linux_use_io_uring` equal to `TRUE` (default) | `io_uring_async_runtime` | One io_uring with mapped submission and completion rings | One read or write submission entry per transfer | One no-operation entry per worker |
| Linux, `linux_use_io_uring` equal to `FALSE` | `epoll_async_runtime` | One epoll instance and one eventfd counter | `pread64` or `pwrite64` on an internal worker thread; the result is queued and announced through the eventfd | One eventfd token per worker |
| macOS | `kqueue_async_runtime` | One kqueue | POSIX asynchronous I/O (`aio_read`, `aio_write`) whose completion is delivered to the kqueue | One user event per worker |
| Windows | `overlapped_async_runtime` | One I/O completion port | `ReadFile` or `WriteFile` with an `OVERLAPPED` structure | One completion packet without an `OVERLAPPED` structure per worker |

The Boolean option [`io::linux_use_io_uring`](index.md#module-options) selects
between the two Linux backends.

**Linux io_uring**. Construction calls `io_uring_setup` and maps the rings; a
failure of either step produces an invalid context. Each submission publishes
one entry and enters the kernel while holding a mutex, so several threads can
submit. A full submission ring rejects the request. Consumers serialize access
to the completion ring with a second mutex.

**Linux epoll**. Regular files do not provide epoll readiness notifications, so
the backend executes each positioned transfer synchronously on an internal
[`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md) that is
constructed with its default worker count, and retries a transfer that a signal
interrupts. The epoll instance receives the eventfd and the readiness
registrations of [native operations](native-operation.md). Destruction drains
the internal workers before closing both descriptors.

**macOS**. Construction calls `kqueue`. Each transfer initializes one
asynchronous I/O control block inside the request state.

**Windows**. Construction creates the completion port with `entries` as its
concurrency value. A handle has one completion-port binding for its open
lifetime. The port delivers completions for transfers that finish immediately
and for transfers that complete later.

**Other targets**. The structure is not declared. `io::async_error` is declared
on every target.

## Implementation types

```quxlang
::async_worker_entry INCLUDE_IF(OS_LINUX || OS_WINDOWS || OS_MACOS)
  STRUCT MOVE_ONLY
```

`io::async_worker_entry` is the thread entry callable of
[`io::thread_pool`](thread-pool.md). Its call operator processes completions of
one context until the context delivers a stop event. Its constructor accepts a
pointer to the module-private queue state, so code outside the `io` module
cannot construct the type.

## Example

The program submits one positioned read of `/dev/zero` with a callback and
consumes the completion on the main thread.

```quxlang
--8<-- "example-programs/reference-programs/completion_queue/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/completion_queue/expected-output.txt"
```

## See also

- [`io::async_file`](async-file.md)
- [`io::thread_pool`](thread-pool.md)
- [`io::async_runtime`](async-runtime.md)
- [Native operations](native-operation.md)
- [`io::io_uring_processor`](io-uring-processor.md)
- [`std::future`](../std/future.md)
- [Build Options](../../reference/build-options.md)
