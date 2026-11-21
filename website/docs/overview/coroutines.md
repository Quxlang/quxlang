# Overview of Coroutines

A coroutine is a callable whose invocation can suspend and later resume. A coroutine declaration
uses the `COROUTINE` keyword in the position where a function declaration uses `FUNCTION`. The
scheduling policy is not part of the declaration: the object that launches the coroutine supplies
a controller, and the controller determines the launch result, the behavior of `AWAIT`, and the
thread on which the body runs.

The standard module provides two launch objects. `std::thread_pool_cort_runtime` runs coroutines
on worker threads and returns futures. `std::cpp_style_coroutine_base` drives coroutines through
C++-style promise and awaiter operations.

## Thread-pool coroutines

```quxlang
--8<-- "example-programs/programs/thread_pool_coroutine_sum/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/thread_pool_coroutine_sum/expected-output.txt"
```

`ON(runtime) RUN sum(4)` launches the coroutine on the pool and returns a `std::future#I32`.
Within a scheduled coroutine, three forms start other work:

- `GO increment(depth)` schedules a function or coroutine and returns a future immediately.
- An ordinary call such as `sum(depth - 1)` suspends the caller until the called coroutine
  completes and then produces its result.
- `AWAIT(expression)` suspends the invocation until the awaited operation completes.

A suspended invocation does not occupy a worker thread. One worker can therefore execute a
recursive coroutine without blocking on its own queued work. `get()` and `wait()` block the
calling thread and are appropriate outside the pool; a coroutine inside the pool uses `AWAIT`.

The pool owns each scheduled invocation until its execution and cleanup finish. `drain()` waits
for all submitted invocations. Objects that an invocation receives by reference must remain alive
until that invocation finishes.

## C++-style coroutines

```quxlang
--8<-- "example-programs/programs/square_generator_coroutine/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/square_generator_coroutine/expected-output.txt"
```

With `std::cpp_style_coroutine_base`, the declared return type of the coroutine supplies a nested
`promise_type`. The promise operations determine whether the body starts immediately
(`initial_suspend`), what a `YIELD` or `RETURN` statement does (`yield_value`, `return_value`,
`return_void`), and whether the invocation remains allocated after completion (`final_suspend`).
This model suits generators and application-defined task types, where the caller resumes the
invocation explicitly through a `std::cpp_coroutine_handle`.

A handle does not own its invocation. The return object defines the ownership policy; the
`generator` structure in the example destroys the invocation in its destructor.

## Cost model

Each invocation allocates its frame, and a C++-style invocation also allocates its promise and
controller, at stable addresses. Arguments are captured into the frame at launch. Thread-pool
launches additionally allocate a shared result state for the returned future. The pool and its
synchronization operations require native threading support and are unavailable during
compile-time evaluation.

## Reference

The [C++-style Coroutines Reference](../reference/cpp-style-coroutines.md) specifies the promise
operations, the awaiter operations, and the handle contract. The
[Thread-pool Coroutines Reference](../reference/thread-pool-coroutines.md) specifies the launch
protocol, the controller hooks, futures, and pool lifetime. The standard-module pages
[`std::thread_pool_cort_runtime`](../standard-modules/std/thread-pool-coroutine.md),
[`std::cpp_coroutine_handle`](../standard-modules/std/cpp-coroutine.md), and
[`std::future`](../standard-modules/std/future.md) document the library types.
