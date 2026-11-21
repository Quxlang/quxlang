# Recursive sum on a thread-pool coroutine runtime

The program computes a recursive sum on a pool of two worker threads and then runs a coroutine
that yields to the pool between two increments. The program demonstrates `COROUTINE` declarations,
the `ON(...) RUN`, `GO`, and `AWAIT` expressions, and
[`std::thread_pool_cort_runtime`](../standard-modules/std/thread-pool-coroutine.md).

```quxlang
--8<-- "example-programs/programs/thread_pool_coroutine_sum/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/thread_pool_coroutine_sum/expected-output.txt"
```

## Explanation

The declaration `VAR runtime std::thread_pool_cort_runtime:(@workers 2);` starts two worker
threads. The expression `ON(runtime) RUN sum(4)` launches the coroutine `sum` on the pool and
returns a [`std::future#I32`](../standard-modules/std/future.md). The call `result.get()` blocks
the main thread until the coroutine publishes its result.

Inside `sum`, the expression `GO increment(depth)` schedules the synchronous function `increment`
on the pool and returns a future without waiting for completion. The recursive call
`sum(depth - 1)` is an ordinary coroutine call: the calling invocation suspends until the called
invocation completes, and the worker thread remains available for other invocations during that
time. The expression `AWAIT(independent)` suspends the invocation until the future is ready and
then produces its value. The result for depth 4 is 1 + 2 + 3 + 4 + 5 = 15.

The coroutine `count_twice` receives its argument by `MUT&` reference, so the variable `counter`
in `main` must outlive the invocation. The expression `AWAIT(std::cort_yield())` queues another
activation of the invocation and returns the worker to the ready queue of the pool. The call
`runtime.drain()` waits for every submitted invocation and its cleanup, after which the main
thread reads `counter`.

The [Thread-pool coroutines reference](../reference/thread-pool-coroutines.md) specifies the
launch protocol, the controller hooks, and the lifetime requirements of the pool. The
[Coroutines overview](../overview/coroutines.md) introduces both coroutine models.
