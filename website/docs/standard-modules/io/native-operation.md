# Native operations and `io::async_submission_ref`

Module `io` · source file `native_operation.qxs`

A native operation is an object that performs one asynchronous action against
the native completion queue of an [`io::async_context`](async-context.md) and
produces one value. The `submit` functions of
[`io::async_context`](async-context.md#submit),
[`io::async_runtime`](async-runtime.md#submit), and
[`io::async_controller`](async-controller.md#submit) take ownership of such an
object, drive it through the protocol that this page specifies, and return a
[`std::future`](../std/future.md) for its value. The operation communicates
with the completion queue through an `io::async_submission_ref`. The socket
operations of the [`net`](../net/socket-operations.md) module are native
operations. The positioned file transfers of `io::async_context` use completion
callbacks and are not native operations.

## Operation protocol

The module declares no interface type for native operations. The `submit`
functions are templates over the operation type and use the following four
members by name.

```quxlang
::value_type ALIAS SZ;    // The result type of the operation; SZ serves as the example.
.start FUNCTION(@submission CONST& io::async_submission_ref) MUT: BOOL
.complete FUNCTION(@submission CONST& io::async_submission_ref, @size SZ, @error U32) MUT: BOOL
.result FUNCTION() MUT: value_type
```

| Member | Called | Meaning of the result |
| --- | --- | --- |
| `value_type` | Not applicable | The type of the value that the future delivers. `VOID` is permitted. |
| `start` | Once, by `submit`, on the submitting thread, before `submit` returns | `TRUE`: the operation has completed. `FALSE`: the operation has registered one native notification. |
| `complete` | Once per native notification, on a thread that consumes completions | `TRUE`: the operation has completed. `FALSE`: the operation has registered one further native notification. |
| `result` | Once, after `start` or `complete` has returned `TRUE` | The value of the operation. |

### Ownership and lifetime

`submit` moves a temporary operation, and copies any other argument, into
storage that the context owns. The operation remains at one address from the
call of `start` until its completion, and the context destroys the operation
after the completion. Addresses of members of the operation are therefore valid
arguments for native calls that retain an address until the notification
arrives. Storage outside the operation, such as a buffer that the operation
refers to, remains alive until the future is ready.

### Completion

An operation completes in exactly one of the following ways:

- `start` or `complete` returns `TRUE`. The context calls `result` and stores
  the returned value in the future. For a `value_type` of `VOID`, the context
  calls `result` and completes the future without a value.
- `start`, `complete`, or `result` throws. The context stores the exception in
  the future, and `get` or `AWAIT` on the future rethrows it.

A `FALSE` return requires exactly one pending native notification that was
registered through the `submission` argument of the same call. An operation
that returns `FALSE` without a registration never completes, and
[`drain`](async-context.md#drain) does not return. An operation that throws or
returns `TRUE` has no notification pending at that point: the context releases
the request state immediately, and a later notification would refer to released
state.

### Notification arguments

The `complete` function receives the outcome of the native notification in
`size` and `error`. A value of zero in `error` denotes success; any other value
is the positive native error number of the target.

| Target | `size` | `error` |
| --- | --- | --- |
| Linux, io_uring backend | The completion result when it is nonnegative, otherwise zero | The negated completion result when it is negative, otherwise zero |
| Linux, epoll backend | Zero | Zero |
| macOS | Zero | The error number that the kernel event carries when the event has the error flag, otherwise zero |
| Windows | The number of bytes transferred | The value of `GetLastError` when the completion packet reports failure, otherwise zero |

On the readiness-based targets (the Linux epoll backend and macOS), a
notification states that the descriptor is ready and carries no transfer
result. The socket operations of the `net` module perform the nonblocking
native call in `complete` and register a further notification when the call
reports that it would block.

### Threads and serialization

`start` runs on the thread that calls `submit`. `complete` runs on a thread
that consumes completions: a caller of
[`wait_one`](async-context.md#wait_one), a worker of an
[`io::thread_pool`](thread-pool.md), or a completion worker of an
`io::async_runtime`. `result` runs on the thread that observed the completion.
The context serializes the calls for one operation: a `complete` call begins
only after `start` has returned, and two `complete` calls of one operation
never overlap. Calls for different operations can overlap when several threads
consume completions.

## `io::async_submission_ref`

```quxlang
::async_submission_ref STRUCT
```

`io::async_submission_ref` gives one operation access to the completion queue
that owns it. The context constructs the object and passes it to `start` and
`complete`; the constructor is private to the module. An instance is valid
until its operation completes. The member functions that exist depend on the
target.

| Member | Declared when | Description |
| --- | --- | --- |
| [`submit`](#submit) | `OS_LINUX && linux_use_io_uring` | Submits one io_uring entry. |
| [`wait`](#wait) | `OS_LINUX || OS_MACOS` | Registers one readiness notification for a descriptor. |
| [`associate`](#associate) | `OS_WINDOWS` | Associates a handle with the completion port. |
| [`overlapped`](#overlapped) | `OS_WINDOWS` | Returns the `OVERLAPPED` structure of the operation. |

### `submit`

```quxlang
.submit INCLUDE_IF(OS_LINUX && linux_use_io_uring)
  FUNCTION(@entry syscall::linux::io_uring_submission_entry) CONST
```

Stores the completion identity of the operation in the `user_data` field of
`entry` and publishes the entry to the io_uring of the context. The completion
of the entry is delivered to `complete` as one notification.

**Preconditions**: storage that the entry refers to remains alive until the
notification arrives.

**Exceptions**: a full submission ring, and an entry that the kernel does not
accept, throw [`async_error::submission_failed`](async-context.md#ioasync_error).

### `wait`

```quxlang
.wait INCLUDE_IF(OS_LINUX || OS_MACOS) FUNCTION(@descriptor I32, @writing BOOL) CONST
```

Registers one notification that is delivered when `descriptor` becomes readable
(`writing` equal to `FALSE`) or writable (`writing` equal to `TRUE`). The
registration delivers at most one notification.

**Preconditions**: the descriptor remains open until the notification arrives.
At most one read registration and one write registration are pending for one
descriptor at any time.

**Exceptions**: a rejected registration throws
[`async_error::submission_failed`](async-context.md#ioasync_error).

**Target restrictions**: the registration mechanism depends on the backend.

| Target | Mechanism |
| --- | --- |
| Linux, io_uring backend | One poll submission entry for the descriptor. |
| Linux, epoll backend | The function duplicates the descriptor and registers the duplicate with the epoll instance as a one-shot registration. The context removes the registration and closes the duplicate before it calls `complete`. |
| macOS | One kqueue read filter or write filter that is added as a one-shot event. The function repeats a registration call that a signal interrupts. |

### `associate`

```quxlang
.associate INCLUDE_IF(OS_WINDOWS) FUNCTION(@descriptor CONST& io_descriptor_ref) CONST
```

Associates the open handle identified by `descriptor` with the context's
completion port. The handle supports overlapped I/O. An operation obtains
a copy of the descriptor for any deferred start. Copies share the same
association. The caller keeps the native resource open until completion.
[Windows descriptor ownership and closure](io-descriptor-ref.md#close_windows_handle)

**Exceptions**: a failed association throws
[`async_error::submission_failed`](async-context.md#ioasync_error).

### `overlapped`

```quxlang
.overlapped INCLUDE_IF(OS_WINDOWS) FUNCTION() CONST: MUT->syscall::windows::overlapped
```

**Return value**: a pointer to the `OVERLAPPED` structure of the operation. The
structure has a stable address until the operation completes. An overlapped
native call that receives this pointer delivers its completion packet to
`complete` as one notification.

## Target restrictions

`io::async_submission_ref` is declared on every target, and each of its public
members is declared only under the condition listed above. A portable native
operation selects its native calls with `STATIC_IF` or declares one `start` and
one `complete` function per target with `INCLUDE_IF`. The option
[`io::linux_use_io_uring`](index.md#module-options) determines whether `submit`
exists on Linux.

## Example

The operation waits until a descriptor is writable. `start` registers one
readiness notification and returns `FALSE`; a completion worker of the runtime
calls `complete` when the notification arrives. The program uses `wait` and is
therefore restricted to Linux and macOS.

```quxlang
--8<-- "example-programs/reference-programs/readiness_probe/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/readiness_probe/expected-output.txt"
```

## See also

- [`io::async_context`](async-context.md)
- [`io::async_runtime`](async-runtime.md)
- [`io::async_controller`](async-controller.md)
- [Socket operations](../net/socket-operations.md)
- [`io::io_uring_processor`](io-uring-processor.md)
- [`std::future`](../std/future.md)
- [Module `io`](index.md)
