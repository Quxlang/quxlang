# Module `io`

Module `io` · source files `filesystem.qxs`, `io_descriptor_ref.qxs`, `async_io.qxs`, `async_io_linux.qxs`, `async_io_epoll.qxs`, `async_io_macos.qxs`, `async_io_windows.qxs`, `async_runtime.qxs`, `native_operation.qxs`, `io_uring_processor.qxs`, `epoll_processor.qxs`, `overlapped_processor.qxs`

The `io` module provides native file access and asynchronous I/O. Its
declarations form three layers. The first layer consists of value types that
refer to native resources and byte ranges, and of owners that close a native
file automatically. The second layer is a completion queue,
[`io::async_context`](async-context.md), that performs positioned file
transfers with callbacks on the native asynchronous mechanism of the target.
The third layer, [`io::async_runtime`](async-runtime.md), combines the
completion queue with worker threads and a coroutine scheduler, so that
coroutines suspend while a transfer is in progress. The [`net`](../net/index.md)
module builds its socket operations on the third layer.

The standalone Linux and Windows processors expose native event dispatch to
applications that provide their own scheduling.

A source file gains access to the module with `IMPORT io;` and names its
declarations with the prefix `io::`.

## Pages

**Resources and synchronous transfers**

| Page | Contents |
| --- | --- |
| [`io::buffer_ref` and `io::transfer_result`](buffer-ref.md) | Non-owning byte range and the outcome of one transfer. |
| [`io::io_descriptor_ref`](io-descriptor-ref.md) | Borrowed native descriptor or handle. |
| [`io::file_ref`](file-ref.md) | Non-owning file reference with read, write, and close operations. |
| [`io::file`](file.md) | Move-only owner that closes its file. |
| [Path-based file opening](filesystem.md) | `open_read`, `open_read_write`, `open_replace`, `create_temporary_path`. |

**Completion queue with callbacks**

| Page | Contents |
| --- | --- |
| [`io::async_context`](async-context.md) | Native completion queue, `io::async_error`, backend aliases, and platform implementations. |
| [`io::async_file`](async-file.md) | Owner of a file bound to one context. |
| [`io::thread_pool`](thread-pool.md) | Threads that execute the completions of one context. |

**Runtime for coroutines and futures**

| Page | Contents |
| --- | --- |
| [`io::async_runtime`](async-runtime.md) | Completion queue, worker threads, and coroutine scheduler in one object. |
| [`io::async_controller`](async-controller.md) | Controller of coroutines that run on a runtime. |
| [Native operations and `io::async_submission_ref`](native-operation.md) | Protocol for operations that the completion queue drives. |

**Standalone processors**

| Page | Contents |
| --- | --- |
| [`io::io_uring_processor`](io-uring-processor.md) | Standalone owner of an io_uring with batched submission and explicit completion processing. |
| [`io::epoll_processor`](epoll-processor.md) | Standalone owner of an epoll instance with shared per-descriptor readiness notifications and explicit event processing. |
| [`io::overlapped_processor`](overlapped-processor.md) | Windows I/O completion port with caller-owned requests and batched completion dispatch. |

## Module options

```quxlang
::linux_use_io_uring OPTION BOOL DEFAULT_VALUE(TRUE);
```

`io::linux_use_io_uring` selects the mechanism behind
[`io::async_context`](async-context.md) on Linux. The option is declared in
source file `async_runtime.qxs` and has the default value `TRUE`.

| Value | Completion queue on Linux | Backend alias |
| --- | --- | --- |
| `TRUE` (default) | io_uring | `io::io_uring_async_runtime` |
| `FALSE` | epoll, with positioned file transfers on internal worker threads | `io::epoll_async_runtime` |

The option has the following effects on Linux targets:

- The option selects which of the two implementations of the queue state and of
  the submission, waiting, and shutdown functions is compiled.
  [Platform implementations](async-context.md#platform-implementations)
  describes both.
- [`async_submission_ref::submit`](native-operation.md#submit) is declared only
  when the option is `TRUE`.
- [`async_submission_ref::wait`](native-operation.md#wait) registers readiness
  through an io_uring poll entry when the option is `TRUE` and through the
  epoll instance when the option is `FALSE`.
- The socket operations of the `net` module submit native io_uring requests
  when the option is `TRUE` and use nonblocking calls with readiness
  notifications when the option is `FALSE`, as
  [Socket operations](../net/socket-operations.md#platform-implementations)
  describes.

The option has no effect on macOS and Windows targets, on the synchronous file
operations, or on either standalone Linux processor.

An option is a compile-time value that the target configuration of the build
supplies. The value is set in the `options` mapping of the logical module that
maps the `io` sources:

```yaml
modules:
  io:
    source: io
    options:
      linux_use_io_uring: false
```

[Build Options](../../reference/build-options.md) specifies option declarations
and their configuration. The `net` module reads the option as
`io::linux_use_io_uring`, so both modules follow the single configured value.

## Target availability

Each declaration exists only on the targets for which its
[`INCLUDE_IF`](../../reference/availability-and-targets.md) condition holds.

| Declaration | Linux | macOS | Windows | JVM |
| --- | --- | --- | --- | --- |
| `buffer_ref`, `transfer_result` | Yes | Yes | Yes | Yes |
| `io_descriptor_ref` | Yes | Yes | Yes | No |
| `file_ref` | Yes | Yes | Yes | Yes, with a different member set |
| `file` | Yes | Yes | Yes | Yes, without coroutine overloads |
| `open_read`, `open_read_write`, `open_replace` | Yes | Yes | No | Yes |
| `create_temporary_path` | No | No | No | Yes |
| `windows_transfer_size` | No | No | Yes | No |
| `async_context`, `async_file`, `thread_pool`, `async_worker_entry` | Yes | Yes | Yes | No |
| `async_runtime`, `async_controller` | Yes | Yes | Yes | No |
| `io_uring_async_runtime` | When `linux_use_io_uring` is `TRUE` | No | No | No |
| `epoll_async_runtime` | When `linux_use_io_uring` is `FALSE` | No | No | No |
| `kqueue_async_runtime` | No | Yes | No | No |
| `overlapped_async_runtime` | No | No | Yes | No |
| `async_submission_ref::submit` | When `linux_use_io_uring` is `TRUE` | No | No | No |
| `async_submission_ref::wait` | Yes | Yes | No | No |
| `async_submission_ref::associate`, `async_submission_ref::overlapped` | No | No | Yes | No |
| `epoll_processor`, `epoll_operation_ref`, `epoll_events`, `epoll_registration_id`, `epoll_error` | Yes | No | No | No |
| `io_uring_processor`, `io_uring_operation_ref`, `io_uring_submission_entry` | Yes | No | No | No |
| `overlapped_processor`, `overlapped_handle_ref`, `overlapped_request`, `overlapped_operation_ref`, `overlapped_completion_result`, `overlapped_error` | No | No | Yes | No |

The declarations `async_error`, `async_submission_ref`, `io_uring_error`,
`io_uring_completion_result`, `io_uring_submission_id`, `operation_status`, and
the option `linux_use_io_uring` carry no condition and are declared on every
target.
`async_controller` carries no condition and is usable where `async_runtime` is
declared.

## Conventions

- A type whose name ends in `_ref` is a non-owning value. Copies refer to the
  same native resource, and destruction performs no native operation.
- An owner (`file`, `async_file`) is move-only and closes its resource in its
  destructor.
- A file transfer reports its outcome in an
  [`io::transfer_result`](buffer-ref.md#iotransfer_result) and does not throw
  for a native transfer failure. The asynchronous facilities throw
  [`io::async_error`](async-context.md#ioasync_error) for failures of the
  completion queue itself.
- Requirements that the source states with `ASSERT` are evaluated only when the
  `ASSERT_ENABLED` policy is enabled.
  [Compilation Policies](../../reference/compilation-policies.md) specifies the
  policy.

## See also

- [Module `net`](../net/index.md)
- [`std::future`](../std/future.md)
- [`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md)
- [`std::buffered_writer` and `std::buffered_reader`](../std/buffered-io.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
- [Build Options](../../reference/build-options.md)
