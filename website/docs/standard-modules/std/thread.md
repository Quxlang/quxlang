# `std::thread`

Module `std` · source file `thread.qxs`

`std::thread` owns one native thread of execution. The entry constructor stores
an independent copy of a callable entry object in a heap-allocated control
block and starts a native thread that invokes the entry once. The owner always
joins the thread before the control block is released: `join` waits explicitly,
and the destructor joins a thread that is still joinable. The type declares no
operation that detaches a thread from its owner. Mutual exclusion and signaling
between threads are provided by the
[synchronization primitives](synchronization.md).

## Declaration

```quxlang
::thread STRUCT MOVE_ONLY
```

`MOVE_ONLY` removes copy construction. Ownership of the native thread transfers
only through the move constructor. The type declares no assignment operator.

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Starts a native thread from an entry object, or transfers ownership of a thread. |
| [`DESTRUCTOR`](#destructor) | Joins the owned thread when the object is joinable. |
| [`joinable`](#joinable) | Reports whether the object owns an unjoined native thread. |
| [`join`](#join) | Waits for the owned thread and releases its control block. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@ARG:entry AUTO& AUTO(entry_type))   // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& thread)                 // (2)
```

1. Allocates a control block, constructs an entry object of type `entry_type`
   inside the control block from `entry`, and starts a native thread. An lvalue
   argument is copied and an expiring argument is moved, so the stored entry is
   independent of the argument. The new thread performs the following steps in
   order: it registers its identity with the runtime module through
   `RUNTIME_MODULE::thread_runtime_start`, invokes the call operator of the
   stored entry with no arguments, destroys the stored entry, and calls
   `RUNTIME_MODULE::thread_finish`, which drains the per-thread lifetime
   described in [Thread-Local Variables](../../reference/thread-local-variables.md).
   The stored entry is destroyed before the destructors of `PER_THREAD`
   variables run.
2. Transfers ownership of the native thread from `OTHER` without joining it.
   `OTHER` is not joinable afterwards.

**Parameters**

- `entry` — a callable object. The stored entry is held in a module-private
  move-only generic that requires a call operator invocable on a mutable
  object with no arguments and a `VOID` return type. A callable whose call
  operator returns another type is rejected at compile time. An lvalue of a
  type that has no copy constructor is rejected at compile time; such an entry
  is passed as an expiring value. A lambda that captures by reference refers to
  the original objects, which must remain alive at their original addresses
  until the thread finishes (see [Lambdas](../../reference/lambdas.md)).
- `OTHER` — the thread object whose ownership is transferred.

Overload (1) is an ordinary constructor and does not define a conversion. The
forms `VAR worker std::thread := entry;`, `entry AS std::thread`,
`entry AS EXPLICIT std::thread`, and `entry AS CHECKED std::thread` are
rejected at compile time. A thread is started with the call form
`std::thread(entry)`.

**Checked failures**

When the native thread cannot be created, overload (1) destroys the stored
entry, deletes the control block, and panics with the message listed for the
target under [Target restrictions](#target-restrictions).

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`join`](#join) when [`joinable`](#joinable) returns `TRUE`, and has no
effect otherwise. Destruction of a joinable object therefore blocks until the
owned thread finishes.

**Checked failures**

The destructor has the checked failures of `join`. In particular, destruction
of a joinable `thread` object on the thread that the object owns panics with
`"A thread cannot join itself."`.

### `joinable`

```quxlang
.joinable FUNCTION(): BOOL
```

**Return value**

`TRUE` when the object owns a control block, which is the state after
overload (1) of the constructor returns and before `join` completes or
ownership is moved to another object. `FALSE` after `join` and for a
moved-from object.

### `join`

```quxlang
.join FUNCTION()
```

Blocks the calling thread until the owned thread finishes, releases the native
thread resources, and deletes the control block. The object is not joinable
afterwards.

**Checked failures**

- The function panics with `"Cannot join a non-joinable thread."` when
  `joinable()` is `FALSE`. A second `join` on the same object therefore panics.
- The function panics with `"A thread cannot join itself."` when the calling
  thread is the owned thread.
- The function panics with the target-specific message listed under
  [Target restrictions](#target-restrictions) when the native wait fails.

## Target restrictions

The constructor and `join` select one implementation at compile time. The
conditions are evaluated in the listed order, and the first condition that
holds selects the row.

| Condition | Thread creation | Creation failure | Join | Join failure |
| --- | --- | --- | --- | --- |
| `OS_MACOS` | `pthread_create` from `/usr/lib/libSystem.B.dylib` with default attributes | `PANIC "pthread_create failed."` | `pthread_join` | `PANIC "pthread_join failed."` |
| `ENVIRONMENT_IS_GLIBC` | `pthread_create` with symbol version `GLIBC_2.34` and default attributes | `PANIC "pthread_create failed."` | `pthread_join` with symbol version `GLIBC_2.34` | `PANIC "pthread_join failed."` |
| `OS_WINDOWS` | `CreateThread` from `kernel32` with a stack size of zero and creation flags of zero | `PANIC "CreateThread failed."` | `WaitForSingleObject` with an infinite timeout, followed by `CloseHandle` | `PANIC "WaitForSingleObject failed while joining a thread."` or `PANIC "CloseHandle failed after joining a thread."` |
| `OS_LINUX && ENVIRONMENT_IS_STATIC` | The runtime module allocates a stack mapping and a thread-local storage block, and `RUNTIME_MODULE::static_linux_clone` starts the thread. | `PANIC "clone failed while creating a thread."` after the storage block and the stack mapping are released | `RUNTIME_MODULE::static_linux_thread_wait`, followed by release of the storage block and the stack mapping | None |
| Any other target | `COMPILATION_ERROR ON_LOWER "std::thread is not supported on this target."` | — | — | — |

The last row applies to every target for which none of the preceding
conditions holds. That set includes JVM targets, for which every
operating-system predicate is `FALSE`, and Linux targets whose environment is
neither `static` nor `glibc`. The error is a lowering error (see
[Failure Statements](../../reference/diagnostics-and-failure.md)):
it is reported when a call of constructor overload (1) remains reachable in
generated code. The target predicates are listed in
[Target Availability](../../reference/availability-and-targets.md).

## Example

The program starts two threads from copies of one entry object, joins both
threads before it reads their results, and relies on the destructor to join a
third thread started from a lambda.

```quxlang
--8<-- "example-programs/reference-programs/thread_join/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/thread_join/expected-output.txt"
```

## See also

- [Synchronization primitives](synchronization.md)
- [`std::synchronized_pointer_queue`](synchronized-pointer-queue.md)
- [`std::future`, `std::promise`, and `std::task`](future.md)
- [`std::thread_pool_cort_runtime`](thread-pool-coroutine.md)
- [Thread-Local Variables](../../reference/thread-local-variables.md)
- [Atomics](../../reference/atomics.md)
- [Lambdas](../../reference/lambdas.md)
- [Runtime Module Contracts](../../reference/runtime-module-contracts.md)
