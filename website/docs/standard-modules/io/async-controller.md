# `io::async_controller`

Module `io` · source file `async_runtime.qxs`

`io::async_controller` is the coroutine controller of every coroutine that runs
on an [`io::async_runtime`](async-runtime.md). The controller extends
[`std::thread_pool_cort_controller`](../std/thread-pool-coroutine.md#stdthread_pool_cort_controller)
with a reference to the runtime, so that nested coroutine calls, `GO`
expressions, coroutine constructors, and coroutine destructors remain on the
same runtime, and so that an operation can reach the native completion queue
through [`submit`](#submit). A coroutine overload that declares a
`CONTROLLER` parameter of this type is selected only for callers that run under
the controller; this rule is the mechanism by which `read`, `write`, `connect`,
`accept`, `send`, and `receive` suspend inside an I/O coroutine and block in
ordinary code.

## Declaration

```quxlang
::async_controller TEMPLATE(@T TYPE) STRUCT
{
  .scheduling BASE std::thread_pool_cort_controller#T;
}
```

`T` is the declared return type of the coroutine that the controller drives;
`VOID` is permitted. The structure has one public base,
`std::thread_pool_cort_controller#T`, with the base projection member
`scheduling`. The base supplies the `continuation` and `CO_AWAIT` members
through inherited member lookup, and the scheduling of ordinary functions. The
remaining state of the controller, which consists of the runtime pointer, the
staged value, and the staged exception, is private to the module.

The declaration carries no
[`INCLUDE_IF`](../../reference/availability-and-targets.md) condition. The
structure is usable on the targets that declare `io::async_runtime`: Linux,
macOS, and Windows.

Programs do not construct a controller. The runtime constructs one controller
per invocation inside [`async_runtime::launch`](async-runtime.md#launch).

## Member summary

**Operations for coroutine bodies**

| Member | Description |
| --- | --- |
| [`submit`](#submit) | Submits a native operation through the runtime of the invocation. |

**Controller hooks**

| Member | Invoked for | Description |
| --- | --- | --- |
| [`CONSTRUCTOR`](#constructor) | Invocation setup | Borrows the runtime. |
| [`CO_RETURN`](#co_return) | `RETURN` | Stages the result. |
| [`CO_EXCEPTION`](#co_exception) | An unhandled exception | Stages the exception. |
| [`CO_GO`](#co_go) | `GO callable(arguments...)` | Schedules a coroutine on the runtime or a function on its scheduler. |
| [`CO_COROUTINE_CALL`](#co_coroutine_call) | A coroutine call | Schedules the child and returns a task that the caller awaits. |
| [`CO_CALL_CTOR`](#co_call_ctor) | A coroutine constructor | Schedules the constructor against the destination object. |
| [`CO_CALL_DTOR`](#co_call_dtor) | A coroutine destructor | Schedules the destructor. |

## Coroutine overloads selected by the controller

A coroutine declaration can name its controller type explicitly:

```quxlang
.read COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @offset U64, @result WRITE& transfer_result)
```

The parameter `@CONTROLLER async_controller#AUTO(result_type)` restricts the
coroutine to invocations whose controller is a specialization of
`io::async_controller`, and deduces the template argument. Inside the body, the
name `CONTROLLER` designates the controller of the calling invocation.

A type can therefore declare a function and a coroutine with the same name and
the same remaining parameters. A call from a coroutine that runs under
`io::async_controller` selects the coroutine, and the caller suspends until the
callee completes. A call from an ordinary function, or from a coroutine that
runs under another controller, selects the function. The `io` module declares
such pairs for [`io::file_ref`](file-ref.md#read-and-write-coroutines) and
[`io::file`](file.md#read-and-write-coroutines), and the `net` module declares
them for every [socket operation](../net/socket-operations.md).

A coroutine that declares no `CONTROLLER` parameter accepts every controller.
When `ON(runtime) RUN` launches such a coroutine on an `io::async_runtime`, the
coroutine runs under `io::async_controller`, and the calls in its body select
the suspending overloads.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@runtime MUT& async_runtime)
```

Stores the address of `runtime`. The runtime outlives the controller.

### `submit`

```quxlang
.submit FUNCTION(@operation AUTO& AUTO(operation_type)): AUTO
```

Passes `operation` to [`async_runtime::submit`](async-runtime.md#submit) of the
runtime that owns the invocation.

**Return value**: the `std::future#(operation_type::value_type)` that the
runtime returns. A coroutine body awaits the future:

```quxlang
RETURN AWAIT(CONTROLLER.submit(@operation MOVE(operation)));
```

The fragment is the final statement of the `read` coroutine of the `net`
module's socket reference type. `AWAIT` suspends the coroutine until a
completion worker publishes the result, and rethrows an exception that the
operation delivered. [Native operations](native-operation.md) specifies the
members that `operation_type` supplies.

The function is the interface through which a module other than `io` implements
a suspending operation; the runtime pointer of the controller is private to the
`io` module.

### `CO_RETURN`

```quxlang
.CO_RETURN FUNCTION(@ARG AUTO& AUTO): completed_operation    // (1)
.CO_RETURN FUNCTION(): completed_operation                   // (2)
```

1. Stages the returned value in the controller. A temporary is moved, and any
   other argument is copied.
2. Stages successful completion without a value.

The runtime publishes the staged value to the future of the invocation after
the coroutine frame has finished its cleanup. The returned
`completed_operation` is a module-private awaiter that completes without
suspending the frame.

### `CO_EXCEPTION`

```quxlang
.CO_EXCEPTION FUNCTION(@ARG CONST& EXCEPTION_PTR): completed_operation
```

Stages the exception that left the coroutine body or its cleanup. After the
frame has finished, the runtime delivers the staged exception through the
future of the invocation in place of a value.

### `CO_GO`

```quxlang
.CO_GO FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @group MUT->std::task_group DEFAULT(NULL)): AUTO
.CO_GO FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @USING:group MUT& std::task_group): AUTO
```

Implements `GO callable(arguments...)` inside a coroutine that runs on the
runtime. The optional `USING(group)` clause registers the invocation with a
[`std::task_group`](../../reference/thread-pool-coroutines.md#task-groups).

- When `callable` has a coroutine overload that accepts `io::async_controller`
  and the given argument types, the function schedules that coroutine through
  [`async_runtime::CO_RUN`](async-runtime.md#co_run). The child runs under its
  own `io::async_controller` on the same runtime.
- Otherwise the function forwards to `CO_GO` of the base controller, which
  captures a call of an ordinary function and schedules it on the scheduler
  workers of the runtime.

**Return value**: a `std::future` for the return type of the callable. `GO`
does not wait for the child; the parent obtains the result with `AWAIT`.

### `CO_COROUTINE_CALL`

```quxlang
.CO_COROUTINE_CALL FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ...): AUTO
```

Implements a direct call of a coroutine from a coroutine. The function
schedules the child with [`CO_GO`](#co_go) and wraps the resulting future in a
[`std::task`](../std/future.md). The ordinary coroutine call protocol awaits
the task, so the caller suspends until the child completes and then receives
the value or the exception of the child.

### `CO_CALL_CTOR`

```quxlang
.CO_CALL_CTOR FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ..., @DESTINATION:destination MUT& AUTO): AUTO
```

Schedules a coroutine constructor through
[`async_runtime::launch`](async-runtime.md#launch) with `destination` as the
receiver. The destination object is owned by the caller.

### `CO_CALL_DTOR`

```quxlang
.CO_CALL_DTOR FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ...): AUTO
```

Schedules a coroutine destructor through
[`async_runtime::launch`](async-runtime.md#launch).

## Example

The program launches one coroutine that calls a second coroutine directly,
starts the same coroutine concurrently with `GO`, and starts an ordinary
function with `GO`. All three run on the same runtime.

```quxlang
--8<-- "example-programs/reference-programs/controller_coroutines/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/controller_coroutines/expected-output.txt"
```

## See also

- [`io::async_runtime`](async-runtime.md)
- [Native operations](native-operation.md)
- [`io::file_ref`](file-ref.md)
- [Socket operations](../net/socket-operations.md)
- [`std::thread_pool_cort_controller`](../std/thread-pool-coroutine.md#stdthread_pool_cort_controller)
- [`std::future`](../std/future.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
