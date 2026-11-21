# `std::cpp_coroutine_handle` and `std::cpp_style_coroutine_base`

Module `std` · source file `cpp_coroutine.qxs`

The source file `cpp_coroutine.qxs` declares the library half of the C++-style
coroutine adapter. `std::cpp_style_coroutine_base` is a stateless scheduler
that launches a coroutine through the `promise_type` nested in the declared
return type of the coroutine. `std::cpp_coroutine_handle` is the non-owning
reference to one launched invocation that the adapter passes to the promise
and to C++-style awaiters. `std::cpp_suspend_always` and
`std::cpp_suspend_never` are the two elementary C++-style awaiters. The promise
operations, the awaiter operations, and the rules that connect `RETURN`,
`YIELD`, and `AWAIT` to them are specified in
[C++-style coroutines](../../reference/cpp-style-coroutines.md); this page
documents the library declarations.

| Entity | Description |
| --- | --- |
| [`std::cpp_coroutine_handle`](#stdcpp_coroutine_handle) | Copyable, non-owning reference to one coroutine invocation. |
| [`std::cpp_suspend_always`](#stdcpp_suspend_always) | C++-style awaiter that always suspends. |
| [`std::cpp_suspend_never`](#stdcpp_suspend_never) | C++-style awaiter that completes synchronously. |
| [`std::cpp_style_coroutine_base`](#stdcpp_style_coroutine_base) | Scheduler that launches coroutines through C++-style promises. |

## `std::cpp_coroutine_handle`

```quxlang
::cpp_coroutine_handle TEMPLATE(@T:promise_type TYPE) STRUCT
```

A `cpp_coroutine_handle#promise_type` refers to a coroutine invocation whose
promise has the type `promise_type`. The handle does not own the allocation of
the invocation. Copies of a handle refer to the same invocation, and
[`destroy`](#cpp_coroutine_handledestroy) releases the invocation and
invalidates every handle that refers to it. The specialization
`cpp_coroutine_handle#VOID` erases the promise type: it preserves the identity
of the invocation and provides no `promise` member function. A handle that
refers to no invocation is a *null handle*.

Handles use the representation of the Quxlang adapter and provide no
interoperability with a foreign C++ coroutine ABI.

**Construction and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#cpp_coroutine_handleconstructor) | Creates a null handle, copies a handle, or erases the promise type. |
| [`OPERATOR:=`](#cpp_coroutine_handle-assignment) | Rebinds or clears the handle. |
| [`from_address`](#cpp_coroutine_handlefrom_address) | Reconstructs a handle from an exported identity. |

**Observers**

| Member | Description |
| --- | --- |
| [`OPERATOR??`](#cpp_coroutine_handle-test) | Reports whether the handle refers to an invocation. |
| [`address`](#cpp_coroutine_handleaddress) | Exports the identity of the invocation. |
| [`done`](#cpp_coroutine_handledone) | Reports final suspension. |
| [`promise`](#cpp_coroutine_handlepromise) | Returns the promise of the invocation. |
| [`OPERATOR==`](#cpp_coroutine_handle-equality) | Compares invocation identities. |

**Control**

| Member | Description |
| --- | --- |
| [`resume`](#cpp_coroutine_handleresume) | Resumes a suspended invocation. |
| [`OPERATOR()`](#cpp_coroutine_handle-call) | Resumes a suspended invocation through the call operator. |
| [`destroy`](#cpp_coroutine_handledestroy) | Destroys a suspended invocation. |

### `cpp_coroutine_handle::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                                    // (1)
.CONSTRUCTOR FUNCTION(@OTHER NULL_TYPE)                                    // (2)
.CONSTRUCTOR FUNCTION(@OTHER CONST& cpp_coroutine_handle#promise_type)     // (3)
.CONSTRUCTOR FUNCTION(@OTHER CONST& cpp_coroutine_handle#AUTO(other_promise))
  ENABLE_IF(SAME_TYPES(promise_type, VOID) && SAME_TYPES(other_promise, VOID) == FALSE)    // (4)
```

1. Constructs a null handle.
2. Constructs a null handle from `NULL`.
3. Copies the invocation reference of `OTHER`. Ownership is not transferred.
4. Constructs a `cpp_coroutine_handle#VOID` from a handle with a promise type
   other than `VOID`. The result refers to the same invocation. This overload
   participates only when the constructed type is `cpp_coroutine_handle#VOID`.

The adapter creates the handle of a new invocation and passes it to
`get_return_object` and to `await_suspend`.

### `cpp_coroutine_handle::OPERATOR:=` { #cpp_coroutine_handle-assignment }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& cpp_coroutine_handle#promise_type) MUT    // (1)
.OPERATOR:= FUNCTION(@OTHER NULL_TYPE) MUT                                   // (2)
```

1. Rebinds the handle to the invocation of `OTHER`.
2. Clears the handle.

Neither overload destroys the invocation to which the handle referred before
the assignment.

### `cpp_coroutine_handle::from_address`

```quxlang
::from_address FUNCTION(@address MUT->VOID): cpp_coroutine_handle#promise_type
```

`from_address` is a static member function, called in the form
`std::cpp_coroutine_handle#(promise_type)::from_address(@address identity)`.

**Parameters**

- `address` — an identity returned by [`address`](#cpp_coroutine_handleaddress),
  including the null identity of a null handle.

**Return value**: A handle that refers to the invocation identified by
`address`, or a null handle for the null identity.

**Preconditions**: `address` was returned by `address()`. `promise_type` is
`VOID` or is the promise type of the invocation that `address` identifies.

### `cpp_coroutine_handle::OPERATOR??` { #cpp_coroutine_handle-test }

```quxlang
.OPERATOR?? FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the handle refers to an invocation, and `FALSE`
for a null handle. The function does not detect a handle whose invocation has
been destroyed.

### `cpp_coroutine_handle::address`

```quxlang
.address FUNCTION() CONST: MUT->VOID
```

**Return value**: The identity of the invocation as an untyped pointer, or the
null pointer for a null handle. Handles that refer to the same invocation
return the same identity for every promise type.

### `cpp_coroutine_handle::done`

```quxlang
.done FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the body of the coroutine has completed and the
invocation is at final suspension, and `FALSE` when the invocation is
suspended at an earlier point.

**Preconditions**: The handle refers to a live, suspended invocation.

### `cpp_coroutine_handle::promise`

```quxlang
.promise FUNCTION() CONST ENABLE_IF(SAME_TYPES(promise_type, VOID) == FALSE): MUT& promise_type
```

**Return value**: A mutable reference to the promise object of the invocation.
The function participates only when `promise_type` is not `VOID`.

**Preconditions**: The handle is not null, the invocation is alive, and
`promise_type` is the promise type of the invocation.

### `cpp_coroutine_handle::OPERATOR==` { #cpp_coroutine_handle-equality }

```quxlang
.OPERATOR== FUNCTION(@OTHER CONST& cpp_coroutine_handle#AUTO(other_promise)) CONST: BOOL
```

**Return value**: `TRUE` when both handles refer to the same invocation or
both handles are null. The operands can have different promise types.

### `cpp_coroutine_handle::resume`

```quxlang
.resume FUNCTION() CONST
```

Resumes the suspended invocation and drives it until its next suspension or
its completion. When an awaiter returns a coroutine handle from
`await_suspend`, the function continues with the designated invocation after
the current drive has returned, and repeats this step until no transfer is
pending. Successive transfers therefore run in one loop and do not nest
activations. When final suspension completes synchronously, the drive deletes
the invocation, and every handle that refers to it becomes invalid.

The adapter delivers an unhandled exception of the coroutine body to the
`unhandled_exception` operation of the promise, which runs while that
exception is the current exception.

**Preconditions**: The handle is not null. The invocation is alive, is
suspended, and is not at final suspension. No other activation of the
invocation is in progress.

### `cpp_coroutine_handle::OPERATOR()` { #cpp_coroutine_handle-call }

```quxlang
.OPERATOR() FUNCTION() CONST
```

Calls [`resume`](#cpp_coroutine_handleresume).

### `cpp_coroutine_handle::destroy`

```quxlang
.destroy FUNCTION() CONST
```

Destroys the suspended invocation: the function releases a retained initial or
final awaiter, destroys the frame when the body has not completed, and deletes
the allocation that holds the promise, the controller, and the frame. Frame
destruction releases captured arguments and live locals and does not enter
`CATCH` handlers of the coroutine body. Every handle that refers to the
invocation is invalid afterwards.

**Preconditions**: The handle refers to a live, suspended invocation. The
caller has retired every external registration that would resume the
invocation.

## `std::cpp_suspend_always`

```quxlang
::cpp_suspend_always STRUCT
```

A C++-style awaiter that always suspends. The awaiter suits the
`initial_suspend`, `final_suspend`, and `yield_value` operations of a promise
and can be the operand of `AWAIT`.

```quxlang
.await_ready FUNCTION() CONST NOEXCEPT: BOOL
.await_suspend FUNCTION(@handle cpp_coroutine_handle#AUTO(promise_type)) CONST NOEXCEPT
.await_resume FUNCTION() CONST NOEXCEPT
```

- `await_ready` returns `FALSE`.
- `await_suspend` performs no operation. The invocation remains suspended
  until a holder of a handle calls `resume`.
- `await_resume` returns no value.

## `std::cpp_suspend_never`

```quxlang
::cpp_suspend_never STRUCT
```

A C++-style awaiter that completes synchronously.

```quxlang
.await_ready FUNCTION() CONST NOEXCEPT: BOOL
.await_suspend FUNCTION(@handle cpp_coroutine_handle#AUTO(promise_type)) CONST NOEXCEPT
.await_resume FUNCTION() CONST NOEXCEPT
```

- `await_ready` returns `TRUE`, so the invocation does not suspend.
- `await_suspend` performs no operation.
- `await_resume` returns no value.

A promise whose `initial_suspend` returns `cpp_suspend_never` starts the body
before the `RUN` expression returns. A promise whose `final_suspend` returns
`cpp_suspend_never` releases the invocation as soon as the body completes.

## `std::cpp_style_coroutine_base`

```quxlang
::cpp_style_coroutine_base STRUCT
```

A scheduler with no retained state. An object of the type is the operand of
`ON(scheduler) RUN coroutine(arguments...)`. The type can also be a base of an
application scheduler, which then inherits `CO_RUN`.

### `cpp_style_coroutine_base::CO_RUN`

```quxlang
.CO_RUN FUNCTION(@COROUTINE:child AUTO, @ARGUMENTS:arguments ...): AUTO
```

The `RUN` expression calls this hook with the callable binding and the
argument pack. The function performs the following steps:

1. Selects the coroutine specialization for the argument types. A `RECEIVER`
   entry of the argument pack selects a member coroutine.
2. Allocates one invocation with `NEW`. The invocation holds a
   default-constructed promise of the type `promise_type` nested in the
   declared return type, the controller, and the frame, and captures the
   arguments in the frame.
3. Calls `get_return_object(@handle handle)` on the promise with a handle to
   the new invocation.
4. Evaluates `initial_suspend()`. When the initial awaiter is ready or declines
   suspension, the function resumes the invocation immediately; otherwise the
   invocation remains suspended before its body.
5. Returns the object produced in step 3.

**Parameters**

- `child` — the callable binding of the `RUN` expression.
- `arguments` — the arguments of the `RUN` expression.

**Return value**: The return object produced by `get_return_object`, of the
declared return type of the coroutine. Ownership of the invocation follows the
policy of that type; the scheduler retains nothing.

**Exceptions**: When `get_return_object` throws, the function releases the
invocation, including the captured arguments, and rethrows the exception.

## Target restrictions

The source file declares no target-conditional code. The adapter requires
`NEW` and `DELETE`. The tests of the adapter in the standard test bundle are
`DUAL_TEST` declarations, which run during constant evaluation and in native
code.

## Example

The program declares a generator type whose promise stores each yielded
value. `RUN` returns the generator while the invocation is suspended at
initial suspension. Each `resume` advances the coroutine to the next `YIELD`,
and the destructor of the generator destroys the invocation at final
suspension.

```quxlang
--8<-- "example-programs/reference-programs/cpp_generator/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/cpp_generator/expected-output.txt"
```

## See also

- [C++-style coroutines](../../reference/cpp-style-coroutines.md)
- [`std::coroutine_view` and `std::coroutine_handle`](coroutine.md)
- [`std::thread_pool_cort_runtime`](thread-pool-coroutine.md)
- [Lambdas](../../reference/lambdas.md)
- [Exception Handling](../../reference/exceptions.md)
- [`NEW` and `DELETE`](../../reference/new-and-delete.md)
