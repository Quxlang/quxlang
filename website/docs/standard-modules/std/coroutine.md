# `std::coroutine_view` and `std::coroutine_handle`

Module `std` · source file `coroutine.qxs`

`std::coroutine_view` and `std::coroutine_handle` provide storage adapters for
the frame of one concrete coroutine specialization. A coroutine frame has a
fixed address for the entire invocation and can be neither copied nor moved.
`coroutine_view` borrows a frame that the caller owns, and `coroutine_handle`
owns a heap-allocated frame whose ownership can move while the frame keeps its
address. Both types forward the frame operations `INITIALIZE`, `RESUME`, and
`TRY_CANCEL` and add no scheduling policy: the caller supplies the controller
and decides when each activation runs. Schedulers that also select the
controller and manage results are provided by
[`std::cpp_style_coroutine_base`](cpp-coroutine.md) and
[`std::thread_pool_cort_runtime`](thread-pool-coroutine.md).

| Entity | Description |
| --- | --- |
| [`std::coroutine_view`](#stdcoroutine_view) | Non-owning reference to a caller-owned coroutine frame. |
| [`std::coroutine_handle`](#stdcoroutine_handle) | Move-only owner of a heap-allocated coroutine frame. |

## Coroutine descriptors

Both templates take one type parameter, `descriptor`, declared as
`@T:descriptor TYPE AUTO`. The argument is a concrete coroutine specialization
that names the parameter types and the controller type, for example
`accumulate#(@limit I32, @CONTROLLER step_controller)`. The templates use the
following members of the descriptor:

| Descriptor member | Use |
| --- | --- |
| `FRAME_TYPE` | The frame type that the adapter refers to or allocates. |
| `CONTROLLER_TYPE` | The controller type that `RESUME` and `TRY_CANCEL` accept. |
| `CANCELLATION_MODE` | A `COROUTINE_CANCELLATION_MODE` value: `NONE` for a coroutine without a cancellation declaration, `COOPERATIVE` for a `CANCELLABLE` coroutine, and `INSTANT` for a `CANCELLABLE(INSTANT)` coroutine. `TRY_CANCEL` participates only when the value differs from `NONE`. |

The controller adapts awaitables through its `CO_AWAIT` hook, and each
returned awaiter supplies its readiness, suspension, and result operations.

## `std::coroutine_view`

```quxlang
::coroutine_view TEMPLATE(@T:descriptor TYPE AUTO) STRUCT
```

A `coroutine_view` stores one pointer to a `descriptor::FRAME_TYPE` object and
does not own the frame. The view neither initializes nor destroys the frame on
construction or destruction.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#coroutine_viewconstructor) | Binds an existing frame. |
| [`get`](#coroutine_viewget) | Returns the borrowed frame. |
| [`INITIALIZE`](#coroutine_viewinitialize) | Captures coroutine arguments in the frame. |
| [`RESUME`](#coroutine_viewresume) | Drives one activation of the frame. |
| [`TRY_CANCEL`](#coroutine_viewtry_cancel) | Requests cancellation of the invocation. |

### `coroutine_view::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@frame MUT& descriptor::FRAME_TYPE) NOEXCEPT
```

Binds the existing frame `frame` without initializing or executing it.

**Preconditions**: The frame outlives the view and every continuation
registration made on behalf of the frame.

### `coroutine_view::get`

```quxlang
.get FUNCTION() CONST: MUT& descriptor::FRAME_TYPE
```

**Return value**: A mutable reference to the borrowed frame.

### `coroutine_view::INITIALIZE`

```quxlang
.INITIALIZE FUNCTION(%...values TT, @KWARGS ...) MUT
```

Forwards the positional arguments and the named arguments to the `INITIALIZE`
operation of the frame, which captures the coroutine arguments. Initialization
executes neither the coroutine body nor any controller hook. A frame that has
completed can be initialized again and then runs a new invocation in the same
storage.

**Parameters**

- `values`, `KWARGS` — the arguments of the coroutine, named as in the
  coroutine declaration.

**Preconditions**: The frame is empty or completed, and every continuation
registration of a previous invocation has been retired.

**Exceptions**: An exception thrown while an argument is captured propagates
to the caller. The frame rolls back the arguments already captured and remains
available for initialization.

### `coroutine_view::RESUME`

```quxlang
.RESUME FUNCTION(@CONTROLLER MUT& descriptor::CONTROLLER_TYPE) MUT: BOOL
```

Drives the frame through one activation with the controller `CONTROLLER`. The
activation runs the coroutine body until the body suspends or completes.

**Return value**: `TRUE` when the invocation has reached terminal completion,
and `FALSE` when the invocation is suspended.

**Preconditions**: The frame is initialized, the activation is an authorized
activation of the invocation, and no other activation of the same frame is in
progress.

### `coroutine_view::TRY_CANCEL`

```quxlang
.TRY_CANCEL FUNCTION(@CONTROLLER MUT& descriptor::CONTROLLER_TYPE) MUT
  ENABLE_IF(descriptor::CANCELLATION_MODE != COROUTINE_CANCELLATION_MODE::NONE): BOOL
```

Requests the cancellation that the specialization permits by forwarding to the
`TRY_CANCEL` operation of the frame. The function participates only for a
coroutine declared `CANCELLABLE` or `CANCELLABLE(INSTANT)`. A call for a
coroutine without a cancellation declaration is rejected at compile time.

**Return value**: `TRUE` when the invocation has reached terminal completion,
and `FALSE` otherwise.

## `std::coroutine_handle`

```quxlang
::coroutine_handle TEMPLATE(@T:descriptor TYPE AUTO) STRUCT MOVE_ONLY
```

A `coroutine_handle` owns one `descriptor::FRAME_TYPE` object allocated with
`NEW`. `MOVE_ONLY` removes copy construction. Moving a handle transfers
ownership of the allocation, and the frame keeps its address, so a handle can
be moved while its invocation is suspended. The handle stores one pointer.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#coroutine_handleconstructor) | Allocates an empty frame or transfers frame ownership. |
| [`DESTRUCTOR`](#coroutine_handledestructor) | Deletes the owned frame. |
| [`has_value`](#coroutine_handlehas_value) | Reports whether the handle owns a frame. |
| [`get`](#coroutine_handleget) | Returns the owned frame. |
| [`INITIALIZE`](#coroutine_handleinitialize) | Captures coroutine arguments in the frame. |
| [`RESUME`](#coroutine_handleresume) | Drives one activation of the frame. |
| [`TRY_CANCEL`](#coroutine_handletry_cancel) | Requests cancellation of the invocation. |

### `coroutine_handle::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                         // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& coroutine_handle#descriptor) // (2)
```

1. Allocates an empty frame with `NEW` without executing the coroutine.
2. Transfers frame ownership from `OTHER` and leaves `OTHER` empty.

### `coroutine_handle::DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Deletes the owned frame with `DELETE OPTIONAL`. The destructor has no effect
on an empty handle.

**Preconditions**: The owned frame is empty or completed, and every
continuation registration has been retired.

### `coroutine_handle::has_value`

```quxlang
.has_value FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the handle owns a frame, and `FALSE` for a
moved-from handle.

### `coroutine_handle::get`

```quxlang
.get FUNCTION() CONST: MUT& descriptor::FRAME_TYPE
```

**Return value**: A mutable reference to the owned frame.

**Preconditions**: `has_value()` is `TRUE`.

### `coroutine_handle::INITIALIZE`

```quxlang
.INITIALIZE FUNCTION(%...values TT, @KWARGS ...) MUT
```

Forwards the arguments to the `INITIALIZE` operation of the owned frame. The
behavior, exceptions, and frame-state preconditions are those of
[`coroutine_view::INITIALIZE`](#coroutine_viewinitialize).

**Preconditions**: `has_value()` is `TRUE`.

### `coroutine_handle::RESUME`

```quxlang
.RESUME FUNCTION(@CONTROLLER MUT& descriptor::CONTROLLER_TYPE) MUT: BOOL
```

Drives the owned frame through one activation. The return value and the
frame-state preconditions are those of
[`coroutine_view::RESUME`](#coroutine_viewresume).

**Preconditions**: `has_value()` is `TRUE`.

### `coroutine_handle::TRY_CANCEL`

```quxlang
.TRY_CANCEL FUNCTION(@CONTROLLER MUT& descriptor::CONTROLLER_TYPE) MUT
  ENABLE_IF(descriptor::CANCELLATION_MODE != COROUTINE_CANCELLATION_MODE::NONE): BOOL
```

Forwards to the `TRY_CANCEL` operation of the owned frame. The availability
and the return value are those of
[`coroutine_view::TRY_CANCEL`](#coroutine_viewtry_cancel).

**Preconditions**: `has_value()` is `TRUE`.

## Target restrictions

The source file declares no target-conditional code. The tests of both types
in the standard test bundle are `DUAL_TEST` declarations, which run during
constant evaluation and in native code.

## Example

The program defines a minimal controller whose `CO_AWAIT` hook maps the
operand `FALSE` to an awaiter that always suspends. The main function owns a
frame through a `coroutine_handle`, resumes the frame until `RESUME` reports
completion, moves the handle, and then reuses the completed frame through a
`coroutine_view`.

```quxlang
--8<-- "example-programs/reference-programs/coroutine_handle_steps/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/coroutine_handle_steps/expected-output.txt"
```

## See also

- [`std::cpp_coroutine_handle` and `std::cpp_style_coroutine_base`](cpp-coroutine.md)
- [`std::thread_pool_cort_runtime`](thread-pool-coroutine.md)
- [`std::future`, `std::promise`, and `std::task`](future.md)
- [C++-style coroutines](../../reference/cpp-style-coroutines.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
- [Functions](../../reference/functions-and-parameters.md)
- [`RETURN` Statements](../../reference/return-statements.md)
- [Packs](../../reference/packs.md)
