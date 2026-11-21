# Reading filtering and transformation with range algorithms

The program counts, filters, transforms, and inspects a series of sensor readings in which the value
-1 marks an invalid reading. The program demonstrates the [range
forms](../standard-modules/std/algorithms-ranges.md) of the standard algorithms `count`, `count_if`,
`find_if_not`, `copy_if`, `transform`, `max_element`, `all_of`, and `for_each`, together with
[`std::back_inserter`](../standard-modules/std/algorithms-iterators.md#stdback_inserter) and
[lambdas](../overview/lambdas.md) with expression bodies.

```quxlang
--8<-- "example-programs/programs/reading_filter_transform/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/reading_filter_transform/expected-output.txt"
```

## Explanation

Every range form receives its sequence through the named argument `@range`, which accepts an lvalue
that provides `BEGIN()` and `END()`. The built-in array `raw_readings` and the
[`std::dynarr`](../standard-modules/std/dynarr.md) objects satisfy this requirement. The lambda
`is_valid` has an [expression body](../reference/lambdas.md#expression-bodies): the expression after
`=` is the returned value.

[`count`](../standard-modules/std/algorithms-search.md#count) returns the number of elements that
equal `@value`, and [`count_if`](../standard-modules/std/algorithms-search.md#count_if) returns the
number of elements for which `@predicate` returns `TRUE`.
[`find_if_not`](../standard-modules/std/algorithms-search.md#find_if_not) returns an iterator to the
first element that does not satisfy the predicate. For a built-in array, the iterator is an array
pointer, and the subtraction of `raw_readings.BEGIN()` yields the element index 1.

[`copy_if`](../standard-modules/std/algorithms-transfer.md#copy_if) copies each element that
satisfies the predicate and preserves the relative order of the copied elements. The output iterator
that `std::back_inserter` creates appends each assigned value to `valid_readings` with `push_back`.
[`transform`](../standard-modules/std/algorithms-transfer.md#transform) assigns the result of
`@operation` for each element to its output iterator. In the expression `reading * 10 + 5`, `*`
binds more tightly than `+`.

[`max_element`](../standard-modules/std/algorithms-search.md#max_element) returns an iterator to the
first of the maximum elements, and the expression `hottest->` dereferences that iterator.
[`all_of`](../standard-modules/std/algorithms-search.md#all_of) returns `TRUE` when the predicate
holds for every element. The helper `print_readings` passes a lambda with a block body as the
`@action` argument of [`for_each`](../standard-modules/std/algorithms-search.md#for_each), which
invokes the lambda once for each element in order.
