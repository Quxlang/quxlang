# `std::thread_pool_cort_runtime`, `std::thread_pool_cort_controller`, and `std::cort_yield`

Module `std` · source file `thread_pool_coroutine.qxs`

The source file `thread_pool_coroutine.qxs` declares a worker pool that
executes coroutines, synchronous functions launched with `GO`, and posted
callables. `std::thread_pool_cort_runtime` owns the worker threads and an
unbounded ready queue, and is the operand of `ON(runtime) RUN`. Each scheduled
invocation is allocated separately and delivers its result through a
[`std::future`](future.md). `std::thread_pool_cort_controller` is the
default controller supplied by the pool, and
`std::cort_yield` is an awaiter that returns the current invocation to the
ready queue. The language rules for `RUN`, `GO`, coroutine calls, and callable
capture are specified in
[Thread-pool coroutines](../../reference/thread-pool-coroutines.md); this page
documents the library declarations.

| Entity | Description |
| --- | --- |
| [`std::thread_pool_cort_runtime`](#stdthread_pool_cort_runtime) | Worker pool and scheduler. |
| [`std::thread_pool_cort_controller`](#stdthread_pool_cort_controller) | Controller of a coroutine that runs on the pool. |
| [`std::cort_yield`](#stdcort_yield) | Awaiter that queues another activation of the current invocation. |

## `std::thread_pool_cort_runtime`

```quxlang
::thread_pool_cort_runtime STRUCT ROOTED
```

A runtime owns its worker threads, which are [`std::thread`](thread.md)
objects, and an unbounded intrusive first-in, first-out queue of ready work.
`ROOTED` removes copy and move construction, so the runtime remains at its
construction address; workers and scheduled invocations hold pointers to it.

Every scheduled invocation has at most one active driver. A notification that
arrives while a worker drives the invocation is recorded, and the invocation
is queued again only after that drive returns. The pool retains ownership of a
scheduled invocation until its execution and cleanup finish, independently of
the lifetime of the returned future. The pool provides no operation that
cancels submitted work.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#thread_pool_cort_runtimeconstructor) | Starts the worker threads. |
| [`DESTRUCTOR`](#thread_pool_cort_runtimedestructor) | Waits for outstanding work and joins the workers. |
| [`drain`](#thread_pool_cort_runtimedrain) | Waits until all submitted work has completed. |
| [`CO_RUN`](#thread_pool_cort_runtimeco_run) | Schedules a coroutine and returns its future. |
| [`launch_with_controller`](#thread_pool_cort_runtimelaunch_with_controller) | Schedules a selected coroutine with a concrete controller and returns its future. |
| [`post`](#thread_pool_cort_runtimepost) | Queues a synchronous callable. |

### `thread_pool_cort_runtime::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@workers SZ DEFAULT(4))
```

Starts `workers` worker threads. Each worker removes ready work from the queue
and executes it until the pool stops.

**Parameters**

- `workers` — the number of worker threads. The default is four.

**Checked failures**:
`ASSERT(workers > 0, "A coroutine thread pool requires at least one worker.")`.
The assertion is evaluated only when the `ASSERT_ENABLED` policy is enabled;
otherwise a positive count is a precondition. Thread creation has the checked
failures of the [`std::thread` constructor](thread.md#constructor).

**Exceptions**: When starting a worker throws, the constructor requests the
workers that already run to stop and rethrows the exception.

### `thread_pool_cort_runtime::DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`drain`](#thread_pool_cort_runtimedrain), requests the idle workers to
stop, and joins every worker thread.

**Preconditions**: The destructor runs on a thread that is not a worker of
this pool. No submission from outside the pool overlaps destruction. Every
object that a scheduled invocation borrows remains alive until that invocation
finishes.

### `thread_pool_cort_runtime::drain`

```quxlang
.drain FUNCTION() MUT
```

Blocks the calling thread until every submitted invocation and posted
operation, including its cleanup, has completed. Work that is submitted while
`drain` waits is included. A coroutine that waits for a promise completed
outside the pool keeps `drain` blocked until that producer completes or is
abandoned.

**Preconditions**: The function is called from a thread that is not a worker
of this pool.

### `thread_pool_cort_runtime::CO_RUN`

```quxlang
.CO_RUN FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @group MUT->task_group DEFAULT(NULL)) MUT: AUTO
```

The expression `ON(runtime) RUN coroutine(arguments...)` calls this hook with
the callable binding and the argument pack. The function selects the coroutine
specialization for the argument types, with a `RECEIVER` entry of the pack
selecting a member coroutine, and then performs the following steps on the
calling thread:

1. Allocates one invocation with `NEW`. The invocation holds the frame, a
   `thread_pool_cort_controller#T`, and a [`std::promise#T`](future.md#stdpromise),
   where `T` is the declared return type of the coroutine.
2. Captures the arguments in the frame.
3. Obtains the future of the promise.
4. Registers the invocation with `group` when the pointer is non-null.
5. Appends the invocation to the ready queue and wakes one worker.

A worker later drives the frame. When the frame reports terminal completion,
which follows the destruction of its locals and captured arguments, the worker
publishes the staged result to the promise and deletes the invocation.

**Parameters**

- `child` — the callable binding of the `RUN` expression.
- `arguments` — the arguments of the `RUN` expression. Arguments bound to
  value parameters are captured before the function returns. Arguments bound
  to reference parameters, and a member receiver, are borrowed and must remain
  alive until the invocation finishes.

**Return value**: A `std::future#T`. Destruction of the future does not cancel
or wait for the invocation.

### `thread_pool_cort_runtime::launch_with_controller`

```quxlang
.launch_with_controller TEMPLATE(@T:descriptor_type TYPE AUTO)
  FUNCTION(@controller AUTO& AUTO(controller_type), @ARGUMENTS:arguments ...,
    @group MUT->task_group DEFAULT(NULL)) MUT: future#descriptor_type::RETURN_TYPE
```

Schedules a selected coroutine descriptor on the pool with the supplied
controller. If `T` is the descriptor's return type, `controller_type` must
be `thread_pool_cort_controller#T` or have an unambiguous base of that type.
The descriptor must accept `controller_type` as its concrete `CONTROLLER`.

The invocation constructs its controller from the forwarded argument and
connects the scheduling base to its continuation and result producer. The
same allocation contains the controller and the coroutine frame. The pool
captures the arguments, registers the optional task group, and publishes the
result after frame cleanup as specified for `CO_RUN`. Objects borrowed by a
controller, such as an owning I/O runtime, must outlive the invocation.

Module `io` uses this entry point to schedule a frame with an
`io::async_controller2#T` that submits native work on the owning
`io::async_runtime2`. [Asynchronous I/O runtime](../io/async-runtime2.md)

### `thread_pool_cort_runtime::post`

```quxlang
.post FUNCTION(@operation AUTO& AUTO(operation_type)) MUT
```

Allocates a work item that owns a copy of `operation`, or the moved operation
when the argument is expiring, and appends the item to the ready queue. A
worker calls the call operator of the stored object once with no arguments and
then destroys the object. The posted operation counts as outstanding work for
[`drain`](#thread_pool_cort_runtimedrain).

**Parameters**

- `operation` — a callable object with a call operator that takes no
  arguments.

**Preconditions**: The call operator completes synchronously and does not
throw.

## `std::thread_pool_cort_controller`

```quxlang
::thread_pool_cort_controller TEMPLATE(@T TYPE) STRUCT
```

`T` is the declared return type of the coroutine that the controller serves.
Each scheduled invocation owns a controller whose scheduling base connects the
coroutine to its pool, result promise and notification target. The compiler
calls the `CO_` hooks for the
corresponding constructs in the coroutine body, and an awaiter receives the
controller as the `CONTROLLER` argument of `AWAIT_SUSPEND`.

| Member | Invoked for | Description |
| --- | --- | --- |
| [`continuation`](#thread_pool_cort_controllercontinuation) | Awaiters | Returns the notification target of the invocation. |
| [`CO_AWAIT`](#thread_pool_cort_controllerco_await) | `AWAIT(operand)` | Borrows a native awaiter. |
| [`CO_RETURN`](#thread_pool_cort_controllerco_return) | `RETURN` | Stages the result. |
| [`CO_EXCEPTION`](#thread_pool_cort_controllerco_exception) | An unhandled exception | Stages the exception. |
| [`CO_GO`](#thread_pool_cort_controllerco_go) | `GO callable(arguments...)` with an optional `USING` clause | Schedules a coroutine or a function and returns its future. |
| [`launch_function`](#thread_pool_cort_controllerlaunch_function) | Controller implementations | Schedules a synchronous overload and returns its future. |
| [`CO_COROUTINE_CALL`](#thread_pool_cort_controllerco_coroutine_call) | A coroutine call | Schedules the child and returns a task that the caller awaits. |
| [`CO_CALL_CTOR`](#thread_pool_cort_controllerco_call_ctor) | A coroutine constructor | Schedules the constructor against the destination object. |
| [`CO_CALL_DTOR`](#thread_pool_cort_controllerco_call_dtor) | A coroutine destructor | Schedules the destructor. |

### `thread_pool_cort_controller::continuation`

```quxlang
.continuation FUNCTION() CONST: MUT->async_continuation
```

**Return value**: A pointer to the
[`std::async_continuation`](future.md#stdasync_continuation) of the current
invocation. The object is stored in the invocation and keeps its address until
the invocation is deleted. Its notification procedure records the notification
with the pool: an invocation that is not being driven is appended to the ready
queue unless it is already queued, and an invocation that is being driven is
queued again after the current drive returns.

### `thread_pool_cort_controller::CO_AWAIT`

```quxlang
.CO_AWAIT FUNCTION(@ARG AUTO& AUTO(awaiter_type)): MUT& awaiter_type
```

**Return value**: A mutable reference to the operand, which serves as the
awaiter for the duration of its suspension. The operand therefore supplies the
native awaiter protocol (`AWAIT_READY`, `AWAIT_SUSPEND`, `AWAIT_RESUME`)
itself. `std::future`, `std::task`, and `std::cort_yield` satisfy this
requirement.

### `thread_pool_cort_controller::CO_RETURN`

```quxlang
.CO_RETURN FUNCTION(@ARG AUTO& AUTO): cort_completion    // (1)
.CO_RETURN FUNCTION(): cort_completion                   // (2)
```

1. Stages the forwarded return operand as the value of the result.
2. Stages successful completion without a value.

The staged result becomes visible to the consumer when the worker publishes
it, after the frame has reported terminal completion.

**Return value**: A `cort_completion`, a module-private awaiter that reports
immediate readiness and never suspends.

### `thread_pool_cort_controller::CO_EXCEPTION`

```quxlang
.CO_EXCEPTION FUNCTION(@ARG CONST& EXCEPTION_PTR): cort_completion
```

Stages the exception `ARG`, which originates from the coroutine body or from
cleanup, as the result, and discards a previously staged value. The consumer
receives the exception from `future::get` or at its `AWAIT` expression.

### `thread_pool_cort_controller::CO_GO`

```quxlang
.CO_GO FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @group MUT->task_group DEFAULT(NULL)): AUTO
.CO_GO FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @USING:group MUT& task_group): AUTO
```

Schedules `child` on the pool of the current invocation and returns without
waiting for it. `@USING` is supplied by the `USING(group)` clause and registers
the child with the group before scheduling. The group joins the child after
cleanup. [Task groups](../../reference/thread-pool-coroutines.md#task-groups)
describes group lifetime and scope exit.

- When `IS_COROUTINE_CALLABLE_WITH` reports that a coroutine overload of
  `child` accepts the argument types, the function forwards to
  [`thread_pool_cort_runtime::CO_RUN`](#thread_pool_cort_runtimeco_run).
- Otherwise the function captures a synchronous call with `BIND_CALL` and
  schedules a module-private coroutine that invokes the captured call. Argument
  conversions and default arguments are evaluated during capture; the function
  body runs on a worker.

**Return value**: A `std::future#U`, where `U` is the return type of the
selected coroutine or function. A function that returns no value produces a
`std::future#VOID`. An exception thrown by the function or the coroutine is
delivered through the future.

### `thread_pool_cort_controller::launch_function`

```quxlang
.launch_function FUNCTION(@child AUTO, @ARGUMENTS:arguments ...,
  @group MUT->task_group DEFAULT(NULL)): AUTO
```

Resolves the supplied binding as a synchronous function with `BIND_CALL`.
The argument pack can include a `RECEIVER` for a member function. The call
captures arguments on the submitting thread and executes the function on a
worker. The returned `future#U` carries the function's return value or
exception. A supplied task group tracks the invocation through cleanup.

A derived controller can use this function after checking for coroutine
overloads that accept its own controller type.

### `thread_pool_cort_controller::CO_COROUTINE_CALL`

```quxlang
.CO_COROUTINE_CALL FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ...): AUTO
```

Schedules the child through [`CO_GO`](#thread_pool_cort_controllerco_go) and
wraps the resulting future.

**Return value**: A [`std::task#U`](future.md#stdtask), where `U` is the
`value_type` of the future. The coroutine call protocol awaits the task, so the
calling invocation suspends while the child is pending and its worker executes
other ready work. Each recursive call allocates an independent invocation.

### `thread_pool_cort_controller::CO_CALL_CTOR`

```quxlang
.CO_CALL_CTOR FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @DESTINATION:destination MUT& AUTO): AUTO
```

Schedules a coroutine constructor as an independent invocation whose receiver
is the caller-owned object `destination`.

**Return value**: The future of the scheduled constructor invocation.

### `thread_pool_cort_controller::CO_CALL_DTOR`

```quxlang
.CO_CALL_DTOR FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ...): AUTO
```

Schedules a coroutine destructor as an independent invocation. The calling
coroutine remains suspended until the cleanup completes.

**Return value**: The future of the scheduled destructor invocation.

## `std::cort_yield`

```quxlang
::cort_yield STRUCT
```

`cort_yield` is an awaiter with no state. The expression
`AWAIT(std::cort_yield())` suspends the current pool invocation and queues
another activation of it behind the work that is already ready, so that other
ready work can run first.

```quxlang
.AWAIT_READY FUNCTION() CONST: BOOL
.AWAIT_SUSPEND FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO): BOOL
.AWAIT_RESUME FUNCTION()
.TRY_CANCEL FUNCTION(@CONTROLLER MUT& AUTO, @FRAME MUT& AUTO) CONST NOEXCEPT: BOOL
```

- `AWAIT_READY` returns `FALSE`, so the invocation always suspends.
- `AWAIT_SUSPEND` calls the notification procedure of
  `CONTROLLER.continuation()` and returns `TRUE`. The notification arrives
  while the driver of the invocation is active, so the pool queues the next
  activation after the current drive returns. `CONTROLLER` supplies a
  `continuation()` member function that returns `MUT->async_continuation`.
- `AWAIT_RESUME` returns no value.
- `TRY_CANCEL` returns `FALSE`: the queued notification is retained until the
  next activation.

## Target restrictions

The source file declares no target-conditional code. The runtime requires
[`std::thread`](thread.md), `std::unfair_mutex`, and
`std::condition_variable`. It is therefore implemented on the targets that
`std::thread` supports: `OS_MACOS`, `ENVIRONMENT_IS_GLIBC`, `OS_WINDOWS`, and
`OS_LINUX && ENVIRONMENT_IS_STATIC`. On every other target, including JVM
targets, the construction of a worker thread is a compilation error. The pool
operations are native runtime operations and are not available during constant
evaluation.

## Example

The coroutine `sum` launches the synchronous function `increment` with `GO`,
calls itself recursively, and awaits the launched result. The main thread
blocks in `get` for the final value. The coroutine `count_twice` yields once
to the ready queue; the main thread calls `drain` before it reads the counter
that the coroutine modifies.

```quxlang
--8<-- "example-programs/reference-programs/thread_pool_sum/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/thread_pool_sum/expected-output.txt"
```

## See also

- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
- [`std::future`, `std::promise`, and `std::task`](future.md)
- [`std::thread`](thread.md)
- [Synchronization primitives](synchronization.md)
- [`std::coroutine_view` and `std::coroutine_handle`](coroutine.md)
- [`std::cpp_coroutine_handle` and `std::cpp_style_coroutine_base`](cpp-coroutine.md)
- [Asynchronous runtime of module `io`](../io/async-runtime.md)
- [Functions](../../reference/functions-and-parameters.md)
