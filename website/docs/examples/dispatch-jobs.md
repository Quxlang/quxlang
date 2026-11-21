# Priority job dispatch

The program queues two regular jobs at the back of a list, places one urgent job at the front, and
dispatches every job in list order. The program demonstrates
[`std::list`](../standard-modules/std/list.md) insertion and removal at both ends and a [`LOOP
WHILE` loop](../overview/while-loops.md).

```quxlang
--8<-- "example-programs/programs/dispatch_jobs/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/dispatch_jobs/expected-output.txt"
```

## Explanation

[`push_back`](../standard-modules/std/list.md#push_back) appends the jobs `100` and `101`, and
[`push_front`](../standard-modules/std/list.md#push_front) places the job `99` before both of them.
Each element occupies its own node, so neither operation moves an existing element.

The [`LOOP WHILE` statement](../reference/while-loops.md) tests its condition before every iteration
and ends when [`empty`](../standard-modules/std/list.md#empty) returns `TRUE`.

In each iteration, [`front`](../standard-modules/std/list.md#front) returns a reference to the first
element, and the declaration of `job` copies that element into a local variable.
[`pop_front`](../standard-modules/std/list.md#pop_front) then unlinks the first node, destroys the
element that the node contains, and releases the node. The copy is necessary because a reference to
the removed element is invalid after `pop_front` returns.
