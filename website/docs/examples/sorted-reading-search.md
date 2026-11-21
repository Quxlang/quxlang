# Search in sorted readings

The program sorts seven readings and answers membership, position, and order-statistic queries on
the sorted sequence. The program demonstrates the [ordering
algorithms](../standard-modules/std/algorithms-ordering.md) `sort`, `is_sorted`, `binary_search`,
and `lower_bound` on a [`std::dynarr`](../standard-modules/std/dynarr.md).

```quxlang
--8<-- "example-programs/programs/sorted_reading_search/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/sorted_reading_search/expected-output.txt"
```

## Explanation

[`std::sort`](../standard-modules/std/algorithms-ordering.md#sort) receives the array through the
named argument `@range` and sorts the elements with the default three-way comparison, which produces
ascending order for integers. The [`VALUE` loop](../reference/loop-statements.md#range-projections)
then prints the elements in index order.

[`is_sorted`](../standard-modules/std/algorithms-ordering.md#is_sorted) returns a `BOOL` value that
reports whether the sequence is ordered.
[`binary_search`](../standard-modules/std/algorithms-ordering.md#binary_search) requires a sorted
sequence and reports whether an element equal to `@value` is present; the reading 23 is present and
the reading 30 is absent.

[`lower_bound`](../standard-modules/std/algorithms-ordering.md#lower_bound) returns an iterator to
the first element that does not precede `@value`, or the end position when every element precedes
`@value`. The program compares the result with `readings.END()` before it dereferences the iterator
with `position->`. The difference between the iterator and `readings.BEGIN()` is the index of the
element, which is 4 for the reading 35.

[`front`](../standard-modules/std/dynarr.md#front) and
[`back`](../standard-modules/std/dynarr.md#back) return references to the first and the final
element, which are the minimum and the maximum of a sorted sequence. The [index
operator](../standard-modules/std/dynarr.md#operator-index) with the index `readings.size() / 2`
selects the middle element of the seven readings, which is the median.
