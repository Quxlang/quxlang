# `std::future`, `std::promise`, and `std::task`

Module `std` · source file `future.qxs`

The source file `future.qxs` declares the asynchronous result types of module
`std`. A `promise#T` owns the single producer of a result of type `T`, and a
`future#T` owns the single consumer. Both objects share one heap-allocated
result state through a [`std::shared_ptr`](shared-ptr.md), so the producer and
the consumer can be destroyed in either order and can run on different
threads. A consumer retrieves the result by blocking in `get` or by applying
`AWAIT` to the future inside a coroutine. A `task#T` wraps the future of an
eagerly scheduled coroutine. The language-level use of these types with `GO`,
`RUN`, and coroutine calls is specified in
[Thread-pool coroutines](../../reference/thread-pool-coroutines.md).

| Entity | Description |
| --- | --- |
| [`std::future`](#stdfuture) | Move-only owner of the consumer of an asynchronous result. |
| [`std::promise`](#stdpromise) | Move-only owner of the producer of an asynchronous result. |
| [`std::task`](#stdtask) | Move-only owner of the result of an eagerly scheduled coroutine. |
| [`std::broken_promise`](#stdbroken_promise) | Exception type that reports an abandoned producer. |
| [`std::async_continuation`](#stdasync_continuation) | Notification target that a coroutine controller registers with a result state. |

## Result state

A result state stores an `EXCEPTION_PTR` for a failure, a value of type `T`
when `T` is non-void, and space for one
[`async_continuation`](#stdasync_continuation) pointer. One atomic word records
publication and the presence of blocking waiters and a coroutine continuation.
The producer stores the result before publishing it. A thread that observes
publication with an acquire load can read the result.

A producer publishes at most once. Publication wakes all registered blocking
waiters and calls the registered continuation on the publishing thread. A
blocking waiter that races with publication either observes the published
state or waits on the address of the same word until the producer wakes it.

The producer and consumer can run on different threads. Multiple threads may
call `wait` and `is_ready` on the same valid future while its owner keeps it
alive. One coroutine may register a continuation during these observations.
The owner synchronizes operations that consume, move, replace, or destroy
the future with all observers. The same rules apply to a task's future.

## `std::future`

```quxlang
::future TEMPLATE(@T TYPE) STRUCT MOVE_ONLY
```

`T` is the result type. `T` can be `VOID`, in which case the state stores no
value and `get` returns no value. `MOVE_ONLY` removes copy construction. A
future is *valid* while it owns consumer access to a result state. `get` and
`AWAIT` consume the result once and leave the future invalid. Destruction
releases consumer ownership without waiting for the producer.

**Member types**

| Member | Definition |
| --- | --- |
| `value_type` | `T` |

**Construction and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#futureconstructor) | Creates an empty future or transfers consumer ownership. |
| [`OPERATOR:=`](#future-assignment) | Replaces consumer ownership. |

**Observers and retrieval**

| Member | Description |
| --- | --- |
| [`valid`](#futurevalid) | Reports whether the future owns consumer access to a result state. |
| [`is_ready`](#futureis_ready) | Reports whether the producer has published. |
| [`wait`](#futurewait) | Blocks until the producer has published. |
| [`get`](#futureget) | Blocks until publication and consumes the result. |

**Awaiter protocol**

| Member | Description |
| --- | --- |
| [`AWAIT_READY`](#futureawait_ready) | Tests readiness before suspension. |
| [`AWAIT_SUSPEND`](#futureawait_suspend) | Registers the continuation of the awaiting coroutine. |
| [`AWAIT_RESUME`](#futureawait_resume) | Consumes the published result after resumption. |
| [`TRY_CANCEL`](#futuretry_cancel) | Declines cancellation of a registered wait. |

### `future::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                         // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& future#T)    // (2)
```

1. Constructs an empty future. `valid()` returns `FALSE`.
2. Transfers consumer ownership from `OTHER` and leaves `OTHER` empty.

A valid future is obtained from [`promise::get_future`](#promiseget_future),
from a `RUN` or `GO` expression on a
[`std::thread_pool_cort_runtime`](thread-pool-coroutine.md), or from an
asynchronous operation of another module.

### `future::OPERATOR:=` { #future-assignment }

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& future#T) MUT
```

Transfers consumer ownership from `OTHER`, releases the result state that the
future owned before the assignment without waiting for its producer, and
leaves `OTHER` empty.

### `future::valid`

```quxlang
.valid FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the future owns consumer access to a result
state. `FALSE` for a default-constructed future, for a moved-from future, and
after `get` or `AWAIT_RESUME`.

### `future::is_ready`

```quxlang
.is_ready FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the producer has published a value, an
exception, or abandonment. The function performs one acquire load of the
publication word.

**Preconditions**: `valid()` is `TRUE`.

### `future::wait`

```quxlang
.wait FUNCTION() CONST
```

Blocks the calling thread until the producer has published. The function does
not consume the result. A coroutine that runs on a pool worker uses `AWAIT`
for a pending result, so that the worker thread remains available for other
invocations.

**Preconditions**: `valid()` is `TRUE`.

### `future::get`

```quxlang
.get FUNCTION() MUT: T
```

Removes consumer ownership from the future, blocks the calling thread until
the producer has published, and delivers the result. The future is invalid
after the call, including when the call throws.

**Return value**: The stored value, moved out of the result state. The
function returns no value when `T` is `VOID`.

**Preconditions**: `valid()` is `TRUE`, and no await registration of this
future is outstanding.

**Exceptions**: When the producer published an exception, the function
rethrows that exception with `THROW_EXCEPTION_PTR`. When the producer was
abandoned, the rethrown exception is a
[`std::broken_promise`](#stdbroken_promise).

### `future::AWAIT_READY`

```quxlang
.AWAIT_READY FUNCTION() CONST: BOOL
```

**Return value**: The result of [`is_ready`](#futureis_ready).

**Preconditions**: `valid()` is `TRUE`.

### `future::AWAIT_SUSPEND`

```quxlang
.AWAIT_SUSPEND FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO) MUT: BOOL
```

Obtains the continuation of the awaiting invocation by calling
`CONTROLLER.continuation()` and registers it through the publication word.
If registration precedes publication, the producer notifies the continuation
once. If publication precedes registration, the caller continues immediately.

**Parameters**

- `CONTROLLER` — the controller of the awaiting coroutine. The controller
  supplies a `continuation()` member function that returns
  `MUT->async_continuation`;
  [`std::thread_pool_cort_controller`](thread-pool-coroutine.md#stdthread_pool_cort_controller)
  satisfies this requirement.
- `FRAME` — the frame of the awaiting coroutine. The function does not use
  the frame.

**Return value**: `TRUE` when the continuation was registered and the
invocation suspends. `FALSE` when the producer has already published, in which
case the invocation continues without suspending.

**Preconditions**: `valid()` is `TRUE`. The registered continuation remains
alive until its notification begins.

### `future::AWAIT_RESUME`

```quxlang
.AWAIT_RESUME FUNCTION() MUT: T
```

Returns the result of [`get`](#futureget). The value of an `AWAIT` expression
applied to a future is this result, and a producer exception is rethrown at
the `AWAIT` expression.

### `future::TRY_CANCEL`

```quxlang
.TRY_CANCEL FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO) CONST NOEXCEPT: BOOL
```

**Return value**: `FALSE`. A registered continuation remains registered until
the producer completes, so a wait on a future cannot be cancelled.

## `std::promise`

```quxlang
::promise TEMPLATE(@T TYPE) STRUCT MOVE_ONLY
```

`T` is the result type. `MOVE_ONLY` removes copy construction. A promise owns
the producer of one result state. `get_future` is called once, and completion
is supplied at most once through `set_value` or `set_exception`. A moved-from
promise has no producer ownership.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#promiseconstructor) | Creates a new result state or transfers producer ownership. |
| [`DESTRUCTOR`](#promisedestructor) | Publishes `broken_promise` for an incomplete result. |
| [`OPERATOR:=`](#promise-assignment) | Abandons the previous result and transfers producer ownership. |
| [`get_future`](#promiseget_future) | Returns the consumer of the result state. |
| [`set_value`](#promiseset_value) | Publishes a value or successful completion. |
| [`set_exception`](#promiseset_exception) | Publishes an exception. |

### `promise::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                          // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& promise#T)    // (2)
```

1. Allocates a new, incomplete result state with `make_shared`.
2. Transfers producer ownership from `OTHER` and leaves `OTHER` without
   producer ownership.

### `promise::DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

When the promise owns a result state that has not been published, the
destructor publishes an exception of type
[`std::broken_promise`](#stdbroken_promise) to that state. The destructor has
no other effect on a completed or moved-from promise.

### `promise::OPERATOR:=` { #promise-assignment }

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& promise#T) MUT
```

Publishes `broken_promise` to the result state that the promise owned before
the assignment when that state is incomplete, releases that state, transfers
producer ownership from `OTHER`, and leaves `OTHER` without producer
ownership. Assignment of a promise to itself has no effect.

### `promise::get_future`

```quxlang
.get_future FUNCTION() MUT: future#T
```

**Return value**: A valid `future#T` that shares the result state of the
promise.

**Preconditions**: The promise has producer ownership. The function is called
at most once for each result state, because a result state supports one
consumer.

### `promise::set_value`

```quxlang
.set_value FUNCTION(@value AUTO& AUTO) MUT ENABLE_IF(SAME_TYPES(T, VOID) == FALSE)    // (1)
.set_value FUNCTION() MUT ENABLE_IF(SAME_TYPES(T, VOID))                              // (2)
```

1. Constructs the stored value from `value`, which is forwarded, and publishes
   the result state. This overload participates only when `T` is not `VOID`.
2. Publishes successful completion without a value. This overload participates
   only when `T` is `VOID`.

Publication wakes every thread blocked in `wait` or `get` on the corresponding
future and calls the notification procedure of a registered continuation on
the calling thread.

**Parameters**

- `value` — the object from which the stored `T` is constructed. An lvalue is
  copied, and an expiring value is moved.

**Preconditions**: The promise has producer ownership, and the result state
has not been published.

### `promise::set_exception`

```quxlang
.set_exception FUNCTION(@error CONST& EXCEPTION_PTR) MUT
```

Stores `error` in the result state and publishes the state. The consumer
receives the exception from [`future::get`](#futureget) or at its `AWAIT`
expression. An `EXCEPTION_PTR` for the exception that is currently handled is
obtained with `CURRENT_EXCEPTION()` (see
[Exception Handling](../../reference/exceptions.md)).

**Preconditions**: The promise has producer ownership, and the result state
has not been published.

## `std::task`

```quxlang
::task TEMPLATE(@T TYPE) STRUCT MOVE_ONLY
```

A task owns the result of an eagerly scheduled coroutine. The task holds one
private `future#T` and forwards every operation to that future. The
`CO_COROUTINE_CALL` hook of
[`std::thread_pool_cort_controller`](thread-pool-coroutine.md#stdthread_pool_cort_controller)
returns a task for a coroutine that is called from another pool coroutine, and
the ordinary coroutine call protocol awaits that task. The type declares no
default constructor.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#taskconstructor) | Takes ownership of a scheduled result or transfers it from another task. |
| [`OPERATOR:=`](#task-assignment) | Replaces ownership of the scheduled result. |
| [`valid`](#task-observers-and-retrieval) | Reports whether the task owns a result. |
| [`is_ready`](#task-observers-and-retrieval) | Reports completion. |
| [`wait`](#task-observers-and-retrieval) | Blocks until completion. |
| [`get`](#task-observers-and-retrieval) | Blocks until completion and consumes the result. |
| [`AWAIT_READY`, `AWAIT_SUSPEND`, `AWAIT_RESUME`, `TRY_CANCEL`](#task-awaiter-protocol) | Awaiter protocol of the contained future. |

### `task::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@result TEMP& future#T)    // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& task#T)       // (2)
```

1. Takes ownership of the scheduled result `result`.
2. Transfers the scheduled result from `OTHER` and leaves `OTHER` without a
   result.

### `task::OPERATOR:=` { #task-assignment }

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& task#T) MUT
```

Transfers the scheduled result from `OTHER`, releases the result that the task
owned before the assignment without waiting for its producer, and leaves
`OTHER` without a result.

### Task observers and retrieval

```quxlang
.valid FUNCTION() CONST: BOOL
.is_ready FUNCTION() CONST: BOOL
.wait FUNCTION() CONST
.get FUNCTION() MUT: T
```

Each function returns the result of the function of the same name on the
contained future and has the same preconditions and exceptions:
[`future::valid`](#futurevalid), [`future::is_ready`](#futureis_ready),
[`future::wait`](#futurewait), and [`future::get`](#futureget). `wait` and
`get` block the calling thread and are intended for a thread outside the pool
that executes the coroutine.

### Task awaiter protocol

```quxlang
.AWAIT_READY FUNCTION() CONST: BOOL
.AWAIT_SUSPEND FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO) MUT: BOOL
.AWAIT_RESUME FUNCTION() MUT: T
.TRY_CANCEL FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO) CONST NOEXCEPT: BOOL
```

`AWAIT_READY`, `AWAIT_SUSPEND`, and `AWAIT_RESUME` forward to
[`future::AWAIT_READY`](#futureawait_ready),
[`future::AWAIT_SUSPEND`](#futureawait_suspend), and
[`future::AWAIT_RESUME`](#futureawait_resume). `TRY_CANCEL` returns `FALSE`:
the notification registration of the producer is retained until completion.

## `std::broken_promise`

```quxlang
::broken_promise STRUCT {}
```

`broken_promise` identifies a producer that was destroyed, or replaced through
move assignment, before it completed its result. The structure has no members.
The consumer of the abandoned result receives a `broken_promise` exception
from `get` or at its `AWAIT` expression, and a handler of the form
`CATCH error CONST& std::broken_promise` selects it.

## `std::async_continuation`

```quxlang
::async_continuation STRUCT ROOTED
{
  .context VAR MUT->VOID;
  .notify VAR CONST->PROCEDURE(@context MUT->VOID);
}
```

An `async_continuation` refers to a stable continuation and its asynchronous
notification operation. `ROOTED` removes copy and move construction, so a
registered pointer to the object remains valid while the object is alive.

| Field | Description |
| --- | --- |
| `context` | An untyped pointer that identifies the notification target. |
| `notify` | The procedure that delivers the notification. Publication calls `notify(@context context)`. |

A coroutine controller supplies a pointer to its continuation through a
`continuation()` member function, and an awaiter such as
[`future::AWAIT_SUSPEND`](#futureawait_suspend) registers that pointer.

**Preconditions**: The notification target remains alive until the
notification begins. The notification procedure completes every access to the
target before it publishes work that can destroy the target. The notification
procedure does not throw.

## Target restrictions

Blocking waits and wakeups use the native runtime's 32-bit address-wait
operations on Linux, Windows, and macOS. These operations require runtime
execution. [Synchronization primitives](synchronization.md#target-restrictions)

## Example

A worker thread completes a promise, and the main thread retrieves the value
with `get`. The program then destroys a second promise without completing it
and observes the resulting `broken_promise` exception.

```quxlang
--8<-- "example-programs/reference-programs/promise_future/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/promise_future/expected-output.txt"
```

## See also

- [`std::thread_pool_cort_runtime`, `std::thread_pool_cort_controller`, and `std::cort_yield`](thread-pool-coroutine.md)
- [`std::shared_ptr`](shared-ptr.md)
- [`std::thread`](thread.md)
- [Synchronization primitives](synchronization.md)
- [Asynchronous runtime of module `io`](../io/async-runtime.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
- [Exception Handling](../../reference/exceptions.md)
