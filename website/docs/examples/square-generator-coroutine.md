# Square generator with a C++-style coroutine

The program produces the first three square numbers from a coroutine that suspends after each
value. The program demonstrates a `COROUTINE` declaration, the `YIELD` statement, a user-defined
promise type, and the [`std::cpp_coroutine_handle`](../standard-modules/std/cpp-coroutine.md)
operations that resume and destroy an invocation.

```quxlang
--8<-- "example-programs/programs/square_generator_coroutine/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/square_generator_coroutine/expected-output.txt"
```

## Explanation

The structure `generator` is the declared return type of the coroutine `squares`. Its nested
structure `promise_type` supplies the [promise
operations](../reference/cpp-style-coroutines.md#promise-operations) that the C++-style coroutine
adapter invokes. `get_return_object` receives the handle of the new invocation and constructs the
`generator` that the launch expression returns. `initial_suspend` returns
`std::cpp_suspend_always`, so the body of `squares` does not start until the first `resume` call.

Each `YIELD` statement passes its operand to `yield_value`. The promise stores the operand in its
`value` field and returns `std::cpp_suspend_always`, which suspends the invocation and returns
control to the caller of `resume`. When the body reaches its end, the adapter invokes
`return_void` and then `final_suspend`. The invocation remains allocated at final suspension
until `destroy` is called.

In `main`, the expression `ON(scheduler) RUN squares(3)` launches the coroutine through
[`std::cpp_style_coroutine_base`](../standard-modules/std/cpp-coroutine.md). The loop reads the
most recent value through `handle.promise()` and resumes the invocation until `handle.done()`
reports final suspension.

The handle is a non-owning reference to the invocation. The `generator` structure therefore
establishes ownership: the structure is [`MOVE_ONLY`](../reference/move-semantics.md), its move
constructor clears the source handle, and its
[destructor](../reference/constructors-and-destructors.md) destroys the suspended invocation.
The [Coroutines overview](../overview/coroutines.md) introduces both coroutine models.
