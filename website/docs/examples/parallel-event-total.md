# Parallel event total under a mutex

The program starts four threads that each add to a shared total one thousand times and prints the
combined result after every thread has finished. The program demonstrates
[`std::thread`](../standard-modules/std/thread.md),
[`std::thin_mutex`](../standard-modules/std/mutexes.md#stdthin_mutex), and
[lambdas](../overview/lambdas.md) with explicit capture lists.

```quxlang
--8<-- "example-programs/programs/parallel_event_total/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/parallel_event_total/expected-output.txt"
```

## Explanation

The lambda that is bound to `count_events` has the [explicit capture
list](../reference/lambdas.md#explicit-capture-lists) `[total_mutex, total, batches,
=worker_number]`. The first three entries capture the named local variables of `main` by reference.
The entry `=worker_number` captures the current value of the loop variable, so each closure stores
its own copy of the worker number.

The expression `std::thread(count_events)` invokes the
[constructor](../standard-modules/std/thread.md#constructor) of `std::thread`, which copies the
closure into storage that the thread owns and starts a native thread that calls the closure.
`std::thread` is a `MOVE_ONLY` type, and [`push_back`](../standard-modules/std/dynarr.md#push_back)
moves each thread object into the array `workers`. The reference captures designate objects of
`main`, which remain alive until every thread has finished.

Each worker performs both updates between the calls to
[`lock`](../standard-modules/std/mutexes.md#thin_mutexlock) and
[`unlock`](../standard-modules/std/mutexes.md#thin_mutexunlock). `lock` blocks until the calling
thread acquires the mutex, so at most one thread modifies `total` and `batches` at any time and no
update is lost. Four workers record 1,000 batches each, and worker `n` adds `n` in every batch,
which yields 4,000 batches and 10,000 events.

The second loop calls [`join`](../standard-modules/std/thread.md#join) on every thread object.
`join` blocks the calling thread until the owned thread finishes. The program reads the two results
only after the block that contains `workers` has ended, so the output does not depend on the
scheduling of the threads.
