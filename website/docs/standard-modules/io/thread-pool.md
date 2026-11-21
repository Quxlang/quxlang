# `io::thread_pool`

Module `io` · source file `async_io.qxs`

`io::thread_pool` owns a fixed number of native threads that wait on the
completion queue of one [`io::async_context`](async-context.md) and execute its
completion callbacks. A pool replaces manual calls of
[`async_context::wait_one`](async-context.md#wait_one): a context accepts at
most one attached pool, and a context with an attached pool accepts no manual
waiter. The pool executes completions only. Scheduling of coroutines and of
arbitrary callables is the function of
[`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md).
[`io::async_runtime`](async-runtime.md) owns one `io::thread_pool` internally.

## Declaration

```quxlang
::thread_pool INCLUDE_IF(OS_LINUX || OS_WINDOWS || OS_MACOS) STRUCT MOVE_ONLY
```

`MOVE_ONLY` removes the copy operations. The pool stores its
[`std::thread`](../std/thread.md) objects in a
[`std::dynarr`](../std/dynarr.md) and refers to the queue state of the context,
which has a stable address. The structure is declared on Linux, macOS, and
Windows.

The requirements that the source states with `ASSERT` are evaluated only when
the `ASSERT_ENABLED` policy is enabled, as described in
[`io::async_context`](async-context.md#declaration).

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Starts the completion workers or transfers ownership of a pool. |
| [`DESTRUCTOR`](#destructor) | Joins the workers of a joinable pool. |
| [`joinable`](#joinable) | Reports whether the object owns a completion consumer pool. |
| [`join`](#join) | Waits for outstanding operations, stops the workers, and joins them. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@context MUT& async_context, @worker_count SZ)    // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& thread_pool)                         // (2)
```

1. Attaches the pool to `context` and starts `worker_count` threads. Each
   thread processes completions of the context until the thread receives a stop
   event.
2. Transfers the workers and the attachment from `OTHER`. `OTHER` is left
   without workers, and its [`joinable`](#joinable) function returns `FALSE`.

**Parameters**: `context` is the context whose completions the workers process.
`worker_count` is the number of threads.

**Preconditions** (1): the context outlives the pool.

**Checked failures** (1): `ASSERT` requires `context.valid()` to return `TRUE`,
requires `worker_count` to be positive and at most the `entries` value given to
the constructor of the context, and requires that the context has no attached
pool and no thread inside `wait_one`.

**Exceptions** (1): an exception that the creation of a thread throws is
rethrown after the constructor has stopped and joined every worker already
started and has detached the pool from the context.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`join`](#join) when the pool is joinable. The destructor therefore blocks
until every outstanding operation of the context has completed.

### `joinable`

```quxlang
.joinable FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the object is attached to a context. `FALSE`
after [`join`](#join) and for a moved-from pool.

### `join`

```quxlang
.join FUNCTION() MUT
```

Performs the following steps in order:

1. Blocks until the number of outstanding operations of the context is zero.
   The workers continue to process completions during this step.
2. Posts one stop event per worker to the native queue.
3. Joins every worker thread.
4. Detaches the pool from the context. The context then accepts manual waiters
   or a new pool.

After the call, [`joinable`](#joinable) returns `FALSE`.

**Preconditions**: submissions from other threads stop before the call; an
operation submitted after step 1 can remain without a consumer. The function is
not called from a completion callback, because a worker cannot join itself.

**Checked failures**: `ASSERT` requires [`joinable`](#joinable) to return
`TRUE`.

**Exceptions**: a failure to post a stop event throws
[`async_error::worker_stop_failed`](async-context.md#ioasync_error).

## Implementation types

Each worker thread runs an `io::async_worker_entry`, which
[`io::async_context`](async-context.md#implementation-types) describes.

## Target restrictions

The structure is declared when `OS_LINUX || OS_WINDOWS || OS_MACOS` holds. The
form of the stop event on each target is listed in
[Platform implementations](async-context.md#platform-implementations). The pool
requires native thread support through [`std::thread`](../std/thread.md).

## Example

The program attaches two workers to a context, submits three positioned reads
of `/dev/zero`, and joins the pool. The callbacks run on the worker threads.

```quxlang
--8<-- "example-programs/reference-programs/completion_workers/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/completion_workers/expected-output.txt"
```

## See also

- [`io::async_context`](async-context.md)
- [`io::async_file`](async-file.md)
- [`io::async_runtime`](async-runtime.md)
- [`std::thread`](../std/thread.md)
- [`std::thread_pool_cort_runtime`](../std/thread-pool-coroutine.md)
