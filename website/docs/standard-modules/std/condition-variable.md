# `std::condition_variable`

Module `std` · source file `synchronization.qxs`

`std::condition_variable` is a process-local condition variable backed by an
atomic notification sequence. A waiting thread releases a mutex that it owns,
blocks until the sequence changes, and reacquires the mutex before `wait`
returns. The type operates with every mutex type that supplies `lock` and
`unlock`, including [`std::thin_mutex`, `std::ordering_mutex`, and
`std::unfair_mutex`](mutexes.md). The properties shared with the other
primitives of the source file are described in
[Synchronization primitives](synchronization.md).

## Declaration

```quxlang
::condition_variable STRUCT ROOTED
```

`ROOTED` removes copy and move construction, so a condition variable remains at
its construction address. The private state consists of an `ATOMIC#I32`
notification sequence and an `ATOMIC#U32` count of registered waiters.

A condition variable is associated with a predicate over shared state. The
following rules form the usage contract:

- Every concurrent waiter of one condition variable passes the same mutex, and
  that mutex protects the predicate.
- A waiting thread rechecks the predicate after every return of `wait`,
  because `wait` can return although no notification occurred.
- A notifying thread changes the predicate while it owns the mutex and calls
  `notify_one` or `notify_all` after the change.

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates a condition variable with no waiters. |
| [`wait`](#wait) | Releases a locked mutex, blocks, and reacquires the mutex. |
| [`notify_one`](#notify_one) | Wakes one waiting thread. |
| [`notify_all`](#notify_all) | Wakes every waiting thread. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()
```

Creates a condition variable with no waiters.

**Target restrictions**: The constructor is
[`UNIMPLEMENTED`](../../reference/diagnostics-and-failure.md#unimplemented)
unless `OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

### `wait`

```quxlang
.wait FUNCTION(@locked_mutex MUT& AUTO)
```

Performs the following steps in order:

1. Reads the current notification sequence.
2. Registers the calling thread as a waiter.
3. Calls `locked_mutex.unlock()`.
4. Blocks while the notification sequence equals the value read in step 1.
   The block ends when a notification changes the sequence, and can also end
   spuriously.
5. Removes the waiter registration.
6. Calls `locked_mutex.lock()`.

The sequence is read and the waiter is registered before the mutex is
released. A notification issued after a predicate change made under the same
mutex therefore either changes the sequence before step 4 begins, in which
case step 4 does not block, or wakes the blocked thread.

**Parameters**

- `locked_mutex` — a mutable reference to an object of any type that supplies
  `unlock()` and `lock()` member functions callable with no arguments.

**Preconditions**: The calling thread owns `locked_mutex`. Every thread that
waits concurrently on the same condition variable passes the same mutex.

**Checked failures**: The function performs no checks of its own. The calls
`locked_mutex.unlock()` and `locked_mutex.lock()` have the checked failures
specified for the mutex type.

### `notify_one`

```quxlang
.notify_one FUNCTION()
```

Increments the notification sequence. When at least one waiter is registered,
the function then wakes at most one thread that is blocked in `wait`. A thread
that has registered as a waiter and has not yet blocked observes the changed
sequence and does not block. One call can therefore end the wait of more than
one thread.

### `notify_all`

```quxlang
.notify_all FUNCTION()
```

Increments the notification sequence. When at least one waiter is registered,
the function then wakes every thread that is blocked in `wait`.

## Target restrictions

The type is implemented when `OS_LINUX || OS_WINDOWS || OS_MACOS` holds. The
blocking step of `wait` uses `syscall::linux::futex_wait` on Linux,
`WaitOnAddress` on Windows, and `RUNTIME_MODULE::wait_at_u32_address` on
macOS. On every other target, including JVM targets, the constructor is
`UNIMPLEMENTED`, and the module-private wait and wake functions that `wait`,
`notify_one`, and `notify_all` call are not declared, so a program that calls
these member functions does not compile.

## Example

A worker thread waits until the main thread publishes a value. The predicate
`ready` and the value are written and read only while the mutex is owned, and
the main thread joins the worker before it prints the result.

```quxlang
--8<-- "example-programs/reference-programs/condition_variable_signal/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/condition_variable_signal/expected-output.txt"
```

## See also

- [Synchronization primitives](synchronization.md)
- [`std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`](mutexes.md)
- [`std::semaphore`](semaphore.md)
- [`std::synchronized_pointer_queue`](synchronized-pointer-queue.md)
- [`std::thread`](thread.md)
- [Atomics](../../reference/atomics.md)
