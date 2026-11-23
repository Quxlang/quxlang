# `io::async_runtime`

Module `io` · source file `async_runtime.qxs`

`io::async_runtime` combines a native completion queue, the threads that
process its completions, a pool of file workers, and a coroutine scheduler in
one object. A coroutine launched with `ON(runtime) RUN` runs under an
[`io::async_controller`](async-controller.md), and the file and socket
operations that such a coroutine calls suspend the coroutine until the runtime
completes them. The runtime also returns a
[`std::future`](../std/future.md) for each operation that ordinary code submits
directly. [`io::async_context`](async-context.md) is the lower-level completion
queue with explicit callbacks, and
[`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md) is the
coroutine scheduler without I/O.

## Declaration

```quxlang
::async_runtime INCLUDE_IF(OS_LINUX || OS_MACOS || OS_WINDOWS) STRUCT ROOTED
```

`ROOTED` removes copy and move construction, so a runtime remains at its
address for its whole lifetime. Controllers and scheduled invocations hold a
pointer to the runtime. The structure is declared on Linux, macOS, and Windows.

A runtime owns three groups of threads:

| Thread group | Constructor parameter | Work that the group executes |
| --- | --- | --- |
| Scheduler workers | `workers` | Coroutine frames launched on the runtime, and coroutines and functions started with `GO` inside those coroutines. |
| Completion workers | `completion_workers` | Events of the native completion queue: the completion of positioned transfers and the `complete` member of [native operations](native-operation.md). |
| File workers | `file_workers` | Cursor-based transfers, each of which is one synchronous native read or write. |

A coroutine that waits for an operation occupies no thread. The completion of
the operation schedules the next activation of the coroutine on a scheduler
worker.

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates the completion queue and starts every thread group. |
| [`DESTRUCTOR`](#destructor) | Drains the runtime and joins every thread. |
| [`drain`](#drain) | Waits until all scheduled and submitted work has finished. |

**Coroutine launch**

| Member | Description |
| --- | --- |
| [`CO_RUN`](#co_run) | Hook of `ON(runtime) RUN`: selects a coroutine and schedules it. |
| [`launch`](#launch) | Schedules a selected coroutine specialization. |

**Direct submission**

| Member | Description |
| --- | --- |
| [`read`](#read) | Submits a cursor-based or positioned read and returns its future. |
| [`write`](#write) | Submits a cursor-based or positioned write and returns its future. |
| [`submit`](#submit) | Submits a native operation and returns its future. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@workers SZ DEFAULT(4), @completion_workers SZ DEFAULT(1), @file_workers SZ DEFAULT(2), @entries SZ DEFAULT(256))
```

Creates the native completion queue with capacity `entries`, starts
`file_workers` file workers and `workers` scheduler workers, and attaches an
[`io::thread_pool`](thread-pool.md) with `completion_workers` threads to the
completion queue.

**Parameters**:

- `workers` is the number of scheduler workers.
- `completion_workers` is the number of threads that process native
  completions.
- `file_workers` is the number of threads that execute cursor-based transfers.
- `entries` is the capacity of the completion queue, with the meaning that the
  constructor of [`io::async_context`](async-context.md#constructor) specifies.

**Checked failures**: `ASSERT` requires `workers`, `completion_workers`,
`file_workers`, and `entries` to be positive, requires `completion_workers` to
be at most `entries`, and requires `entries` to be at most 4,294,967,295. The
checks are evaluated only when the `ASSERT_ENABLED` policy is enabled.

**Exceptions**: a native completion queue that cannot be created throws
[`async_error::initialization_failed`](async-context.md#ioasync_error). An
exception that the creation of a thread throws propagates.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`drain`](#drain) and then joins the completion workers. Coroutine frames
therefore finish their cleanup before the completion workers stop. The
scheduler, the file workers, and the completion queue are destroyed afterwards
as members of the runtime.

**Preconditions**: no other thread submits work to the runtime during
destruction. The destructor does not run on a thread that the runtime owns.

**Exceptions**: the exceptions of
[`thread_pool::join`](thread-pool.md#join) and of the destructor of
[`io::async_context`](async-context.md#destructor) propagate.

### `drain`

```quxlang
.drain FUNCTION() MUT
```

Blocks the calling thread until the following three conditions have been
observed in order: every invocation on the scheduler has finished, including
its cleanup; every operation queued on the file workers has finished; and no
operation of the native completion queue is outstanding.

**Preconditions**: submissions from threads outside the runtime stop before the
call begins. The function is not called from a thread that the runtime owns. A
coroutine that waits for a result that never arrives, such as a read on a
connection whose peer sends no data, keeps the function blocked.

### `CO_RUN`

```quxlang
.CO_RUN FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @group MUT->std::task_group DEFAULT(NULL)) MUT: AUTO
```

`CO_RUN` is the hook that the expression `ON(runtime) RUN operation(arguments...)`
invokes. The function selects the coroutine overload of `operation` for the
controller type `io::async_controller` and the types of `arguments`, and
schedules the selected coroutine through [`launch`](#launch), forwarding the
group pointer. For a member coroutine, the receiver is part of the selection.

**Return value**: a [`std::future#T`](../std/future.md), where `T` is the
declared return type of the selected coroutine. The future becomes ready after
the coroutine frame has completed and has been cleaned up. An exception that
leaves the coroutine is delivered through the future.

Arguments are evaluated and captured in the invocation before `CO_RUN` returns.
An argument that is passed to a reference parameter is borrowed; the referenced
object remains alive until the coroutine finishes. An invocation declines
cancellation: the frame of a coroutine that waits for an I/O completion is
retained until the completion arrives.

Within the coroutine, every nested coroutine call, every `GO` expression, and
every coroutine constructor or destructor uses the same runtime, as
[`io::async_controller`](async-controller.md) specifies.
[Thread-pool coroutines](../../reference/thread-pool-coroutines.md) specifies
the `ON ... RUN` and `GO` expressions.

### `launch`

```quxlang
.launch FUNCTION(@COROUTINE:descriptor AUTO, @ARGUMENTS:arguments ..., @group MUT->std::task_group DEFAULT(NULL)) MUT: AUTO
```

Allocates one invocation that holds an `io::async_controller` and the frame of
the coroutine specialization `descriptor`, initializes the frame with
`arguments`, and schedules the invocation on the scheduler workers. `CO_RUN`
and the hooks of `io::async_controller` call the function; `descriptor` is a
coroutine specialization that those callers have already selected.

**Return value**: a `std::future` for the declared return type of `descriptor`.

**Exceptions**: an exception thrown while the invocation is allocated or
scheduled propagates, and the invocation is deallocated.

### `read`

```quxlang
.read FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref) MUT: std::future#transfer_result                 // (1)
.read FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref, @offset U64) MUT: std::future#transfer_result    // (2)
```

1. Queues a cursor-based read on a file worker. The file worker performs the
   synchronous [`file_ref::read`](file-ref.md#read) at the current file
   position of the descriptor; the operating system manages that position,
   which every user of the descriptor shares.
2. Submits a positioned read at byte offset `offset` to the native completion
   queue through [`async_context::read`](async-context.md#read). The file
   position of the descriptor is not used.

**Parameters**: `descriptor` is the borrowed
[`io::io_descriptor_ref`](io-descriptor-ref.md). `buffer` designates the
destination bytes. `offset` is the byte offset in the file.

**Return value**: a single-consumer future for the
[`io::transfer_result`](buffer-ref.md#iotransfer_result). Ordinary code obtains
the result with `get`, which blocks the calling thread. A coroutine obtains the
result with `AWAIT`. For overload (2), a submission that the native queue
rejects produces a ready future whose result has `succeeded` equal to `FALSE`
and `size` equal to zero. A failed transfer is reported through the result; the
future carries no exception for it.

**Preconditions**: the descriptor and the storage that `buffer` designates
remain valid until the future is ready.

**Target restrictions**: the Windows requirements of
[`async_context::read`](async-context.md#read) apply to overload (2), and the
Windows restriction of [`file_ref::read`](file-ref.md#read) applies to
overload (1).

### `write`

```quxlang
.write FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref) MUT: std::future#transfer_result                 // (1)
.write FUNCTION(@descriptor io_descriptor_ref, @buffer CONST& buffer_ref, @offset U64) MUT: std::future#transfer_result    // (2)
```

1. Queues a cursor-based write on a file worker, which performs the synchronous
   [`file_ref::write`](file-ref.md#write).
2. Submits a positioned write at byte offset `offset` to the native completion
   queue through [`async_context::write`](async-context.md#write).

The parameters, return value, preconditions, and target restrictions are those
of [`read`](#read). The referenced bytes are not modified. A successful result
can report fewer bytes than `buffer.size`.

### `submit`

```quxlang
.submit FUNCTION(@operation AUTO& AUTO(operation_type)) MUT: std::future#(operation_type::value_type)
```

Passes `operation` to [`async_context::submit`](async-context.md#submit) of the
completion queue of the runtime. The runtime takes ownership of the operation,
starts it on the calling thread, and processes its notifications on the
completion workers. [Native operations](native-operation.md) specifies the
members that `operation_type` supplies.

**Return value**: a future for the value of the operation. An exception that
the operation throws is delivered through the future.

**Preconditions**: every resource that the operation refers to outlives the
completion of the operation.

## Coroutine operations

The following operations select a suspending overload when a coroutine that
runs on an `io::async_runtime` calls them, and the corresponding synchronous
function in every other context:

| Type | Operations |
| --- | --- |
| [`io::file_ref`](file-ref.md#read-and-write-coroutines), [`io::file`](file.md#read-and-write-coroutines) | `read`, `write` (cursor-based and positioned) |
| [`net::tcp_connection`, `net::tcp_connection_ref`](../net/tcp-connection.md) | `connect`, `read`, `write` |
| [`net::tcp_listener`, `net::tcp_listener_ref`](../net/tcp-listener.md) | `accept` |
| [`net::udp_session`, `net::udp_session_ref`](../net/udp-session.md) | `send`, `receive` |

## Target restrictions

The structure is declared when `OS_LINUX || OS_MACOS || OS_WINDOWS` holds. The
completion queue is the [`io::async_context`](async-context.md) backend of the
target: io_uring or epoll on Linux, according to the option
[`io::linux_use_io_uring`](index.md#module-options); kqueue on macOS; and an I/O
completion port on Windows. On Linux with the epoll backend, the completion
queue owns an additional internal worker pool that executes positioned file
transfers, as [Platform implementations](async-context.md#platform-implementations)
describes.

## Example

The program reads from `/dev/zero` through both direct submission forms and
through a coroutine.

```quxlang
--8<-- "example-programs/reference-programs/runtime_transfers/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/runtime_transfers/expected-output.txt"
```

## See also

- [`io::async_controller`](async-controller.md)
- [`io::async_context`](async-context.md)
- [`io::thread_pool`](thread-pool.md)
- [Native operations](native-operation.md)
- [`std::future`](../std/future.md)
- [`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
- [Module `net`](../net/index.md)
