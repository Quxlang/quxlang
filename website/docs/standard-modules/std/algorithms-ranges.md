# Range forms

Module `std` · source file `algorithm_ranges.qxs`

Every [algorithm function](algorithms.md) of module `std` has a range form in
addition to its iterator form. A range form receives a range object in place of
each pair of iterators, obtains the sequence boundaries from the range, and
calls the iterator form of the same name. The range forms contain no algorithm
logic of their own: the effects, preconditions, complexity, and exception
behavior of each call are those of the iterator form applied to the sequence
[`range.BEGIN()`, `range.END()`).

## Range requirements

The following requirements apply to every function on this page.

**Range objects.** A range argument is an lvalue `range` for which the
expressions `range.BEGIN()` and `range.END()` are valid. Built-in arrays,
[`std::dynarr`](dynarr.md), [`std::list`](list.md),
[`std::geometric_array`](geometric-array.md), and
[`std::iterator_range`](algorithms-iterators.md#stditerator_range) objects
satisfy this requirement, as does any user-defined type that declares `BEGIN`
and `END` member functions. The range parameters are declared `AUTO& AUTO`, so
a range form accepts a mutable lvalue and a constant lvalue. The iterators that
`BEGIN()` and `END()` return for the supplied lvalue determine whether the
iterator form can modify the elements: a function that reorders or assigns
elements is rejected during compilation when the range is a constant lvalue
whose iterators designate constant elements.

**Boundary evaluation.** A range form evaluates `range.BEGIN()` exactly once
and `range.END()` exactly once. The results become the arguments `@first` and
`@last` of the iterator form. A form with two ranges passes the boundaries of
`range1` as `@first1` and `@last1` and the boundaries of `range2` as `@first2`
and `@last2`.

**Lvalue requirement.** A range form borrows its range for the duration of the
call, and a returned iterator designates a position in the borrowed range.
Any temporary range argument produces the compilation diagnostic
`Algorithm range arguments must be lvalues.` This requirement applies to
containers and views, including the direct result of
[`std::subrange`](algorithms-iterators.md#stdsubrange). A range object that is
produced by a function call is stored in a variable before the variable is
passed to a range form.

**Forwarded arguments.** The named arguments other than the ranges are captured
by `@KWARGS:options ...` and forwarded unchanged to the iterator form. Each
section lists the arguments that the iterator form requires and the arguments
that the iterator form supplies by default. The contracts of these arguments
are specified with the iterator form.

**Iterator types.** The iterator form deduces its iterator type from the result
of `range.BEGIN()` and its sentinel type from the result of `range.END()`. An
iterator form that declares `first` and `last` with one deduced type requires
`BEGIN()` and `END()` to return the same type; the affected sections state this
requirement.

**Return value.** A range form whose declaration ends in `: AUTO` returns the
result of the iterator form. A range form whose declaration has no return type
returns no value.

**Exceptions.** The range forms contain no exception handlers. An exception
thrown by `BEGIN()`, by `END()`, or by the iterator form propagates to the
caller.

## Function summary

In the column "Other named arguments", the arguments before the word
"optional" are required.

**Range forms of the search, comparison, and fold algorithms**

| Function | Range arguments | Other named arguments | Result | Iterator form |
| --- | --- | --- | --- | --- |
| [`all_of`](#all_of) | `@range` | `@predicate`; optional `@projection` | `BOOL` | [`all_of`](algorithms-search.md#all_of) |
| [`any_of`](#any_of) | `@range` | `@predicate`; optional `@projection` | `BOOL` | [`any_of`](algorithms-search.md#any_of) |
| [`none_of`](#none_of) | `@range` | `@predicate`; optional `@projection` | `BOOL` | [`none_of`](algorithms-search.md#none_of) |
| [`count`](#count) | `@range` | `@value`; optional `@equal`, `@projection` | `SZ` | [`count`](algorithms-search.md#count) |
| [`count_if`](#count_if) | `@range` | `@predicate`; optional `@projection` | `SZ` | [`count_if`](algorithms-search.md#count_if) |
| [`find`](#find) | `@range` | `@value`; optional `@equal`, `@projection` | Iterator | [`find`](algorithms-search.md#find) |
| [`find_if`](#find_if) | `@range` | `@predicate`; optional `@projection` | Iterator | [`find_if`](algorithms-search.md#find_if) |
| [`find_if_not`](#find_if_not) | `@range` | `@predicate`; optional `@projection` | Iterator | [`find_if_not`](algorithms-search.md#find_if_not) |
| [`for_each`](#for_each) | `@range` | `@action` | None | [`for_each`](algorithms-search.md#for_each) |
| [`find_first_of`](#find_first_of) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | Iterator | [`find_first_of`](algorithms-search.md#find_first_of) |
| [`adjacent_find`](#adjacent_find) | `@range` | optional `@equal`, `@projection` | Iterator | [`adjacent_find`](algorithms-search.md#adjacent_find) |
| [`search`](#search) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | `std::iterator_range` | [`search`](algorithms-search.md#search) |
| [`find_end`](#find_end) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | `std::iterator_range` | [`find_end`](algorithms-search.md#find_end) |
| [`search_n`](#search_n) | `@range` | `@count`, `@value`; optional `@equal`, `@projection` | `std::iterator_range` | [`search_n`](algorithms-search.md#search_n) |
| [`equal`](#equal) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | `BOOL` | [`equal`](algorithms-search.md#equal) |
| [`mismatch`](#mismatch) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | Composite `in1`, `in2` | [`mismatch`](algorithms-search.md#mismatch) |
| [`lexicographical_compare`](#lexicographical_compare) | `@range1`, `@range2` | optional `@compare`, `@projection1`, `@projection2` | `ORDER` | [`lexicographical_compare`](algorithms-search.md#lexicographical_compare) |
| [`is_permutation`](#is_permutation) | `@range1`, `@range2` | optional `@equal`, `@projection1`, `@projection2` | `BOOL` | [`is_permutation`](algorithms-search.md#is_permutation) |
| [`min_element`](#min_element) | `@range` | optional `@compare`, `@projection` | Iterator | [`min_element`](algorithms-search.md#min_element) |
| [`max_element`](#max_element) | `@range` | optional `@compare`, `@projection` | Iterator | [`max_element`](algorithms-search.md#max_element) |
| [`minmax_element`](#minmax_element) | `@range` | optional `@compare`, `@projection` | Composite `min`, `max` | [`minmax_element`](algorithms-search.md#minmax_element) |
| [`fold_left`](#fold_left) | `@range` | `@initial`, `@operation` | Accumulator | [`fold_left`](algorithms-search.md#fold_left) |
| [`fold_right`](#fold_right) | `@range` | `@initial`, `@operation` | Accumulator | [`fold_right`](algorithms-search.md#fold_right) |

**Range forms of the transfer and modification algorithms**

| Function | Range arguments | Other named arguments | Result | Iterator form |
| --- | --- | --- | --- | --- |
| [`copy`](#copy) | `@range` | `@output` | Composite `in`, `out` | [`copy`](algorithms-transfer.md#copy) |
| [`move`](#move) | `@range` | `@output` | Composite `in`, `out` | [`move`](algorithms-transfer.md#move) |
| [`copy_backward`](#copy_backward) | `@range` | `@output_last` | Composite `in`, `out` | [`copy_backward`](algorithms-transfer.md#copy_backward) |
| [`move_backward`](#move_backward) | `@range` | `@output_last` | Composite `in`, `out` | [`move_backward`](algorithms-transfer.md#move_backward) |
| [`copy_if`](#copy_if) | `@range` | `@output`, `@predicate`; optional `@projection` | Composite `in`, `out` | [`copy_if`](algorithms-transfer.md#copy_if) |
| [`remove_copy`](#remove_copy) | `@range` | `@output`, `@value`; optional `@equal`, `@projection` | Composite `in`, `out` | [`remove_copy`](algorithms-transfer.md#remove_copy) |
| [`remove_copy_if`](#remove_copy_if) | `@range` | `@output`, `@predicate`; optional `@projection` | Composite `in`, `out` | [`remove_copy_if`](algorithms-transfer.md#remove_copy_if) |
| [`transform`](#transform) | `@range`, or `@range1` and `@range2` | `@output`, `@operation` | Composite | [`transform`](algorithms-transfer.md#transform) |
| [`fill`](#fill) | `@range` | `@value` | None | [`fill`](algorithms-transfer.md#fill) |
| [`generate`](#generate) | `@range` | `@generator` | None | [`generate`](algorithms-transfer.md#generate) |
| [`replace`](#replace) | `@range` | `@old_value`, `@new_value`; optional `@equal`, `@projection` | None | [`replace`](algorithms-transfer.md#replace) |
| [`replace_if`](#replace_if) | `@range` | `@predicate`, `@new_value`; optional `@projection` | None | [`replace_if`](algorithms-transfer.md#replace_if) |
| [`replace_copy`](#replace_copy) | `@range` | `@output`, `@old_value`, `@new_value`; optional `@equal`, `@projection` | Composite `in`, `out` | [`replace_copy`](algorithms-transfer.md#replace_copy) |
| [`replace_copy_if`](#replace_copy_if) | `@range` | `@output`, `@predicate`, `@new_value`; optional `@projection` | Composite `in`, `out` | [`replace_copy_if`](algorithms-transfer.md#replace_copy_if) |
| [`remove`](#remove) | `@range` | `@value`; optional `@equal`, `@projection` | Iterator | [`remove`](algorithms-transfer.md#remove) |
| [`remove_if`](#remove_if) | `@range` | `@predicate`; optional `@projection` | Iterator | [`remove_if`](algorithms-transfer.md#remove_if) |
| [`unique`](#unique) | `@range` | optional `@equal`, `@projection` | Iterator | [`unique`](algorithms-transfer.md#unique) |
| [`unique_copy`](#unique_copy) | `@range` | `@output`; optional `@equal`, `@projection` | Composite `in`, `out` | [`unique_copy`](algorithms-transfer.md#unique_copy) |
| [`reverse`](#reverse) | `@range` | None | None | [`reverse`](algorithms-transfer.md#reverse) |
| [`reverse_copy`](#reverse_copy) | `@range` | `@output` | Composite `in`, `out` | [`reverse_copy`](algorithms-transfer.md#reverse_copy) |
| [`rotate`](#rotate) | `@range` | `@middle` | Iterator | [`rotate`](algorithms-transfer.md#rotate) |
| [`rotate_copy`](#rotate_copy) | `@range` | `@middle`, `@output` | Composite `in`, `out` | [`rotate_copy`](algorithms-transfer.md#rotate_copy) |
| [`swap_ranges`](#swap_ranges) | `@range1`, `@range2` | None | Composite `in1`, `in2` | [`swap_ranges`](algorithms-transfer.md#swap_ranges) |
| [`partition`](#partition) | `@range` | `@predicate`; optional `@projection` | Iterator | [`partition`](algorithms-transfer.md#partition) |
| [`is_partitioned`](#is_partitioned) | `@range` | `@predicate`; optional `@projection` | `BOOL` | [`is_partitioned`](algorithms-transfer.md#is_partitioned) |
| [`partition_copy`](#partition_copy) | `@range` | `@output_true`, `@output_false`, `@predicate`; optional `@projection` | Composite `in`, `out_true`, `out_false` | [`partition_copy`](algorithms-transfer.md#partition_copy) |

**Range forms of the ordering algorithms**

| Function | Range arguments | Other named arguments | Result | Iterator form |
| --- | --- | --- | --- | --- |
| [`make_heap`](#make_heap) | `@range` | optional `@compare`, `@projection` | None | [`make_heap`](algorithms-ordering.md#make_heap) |
| [`push_heap`](#push_heap) | `@range` | optional `@compare`, `@projection` | None | [`push_heap`](algorithms-ordering.md#push_heap) |
| [`pop_heap`](#pop_heap) | `@range` | optional `@compare`, `@projection` | None | [`pop_heap`](algorithms-ordering.md#pop_heap) |
| [`sort_heap`](#sort_heap) | `@range` | optional `@compare`, `@projection` | None | [`sort_heap`](algorithms-ordering.md#sort_heap) |
| [`is_heap_until`](#is_heap_until) | `@range` | optional `@compare`, `@projection` | Iterator | [`is_heap_until`](algorithms-ordering.md#is_heap_until) |
| [`is_heap`](#is_heap) | `@range` | optional `@compare`, `@projection` | `BOOL` | [`is_heap`](algorithms-ordering.md#is_heap) |
| [`sort`](#sort) | `@range` | optional `@compare`, `@projection` | None | [`sort`](algorithms-ordering.md#sort) |
| [`nth_element`](#nth_element) | `@range` | `@nth`; optional `@compare`, `@projection` | None | [`nth_element`](algorithms-ordering.md#nth_element) |
| [`partial_sort`](#partial_sort) | `@range` | `@middle`; optional `@compare`, `@projection` | None | [`partial_sort`](algorithms-ordering.md#partial_sort) |
| [`partial_sort_copy`](#partial_sort_copy) | `@range` | `@output_first`, `@output_last`; optional `@compare`, `@projection` | Composite `in`, `out` | [`partial_sort_copy`](algorithms-ordering.md#partial_sort_copy) |
| [`is_sorted_until`](#is_sorted_until) | `@range` | optional `@compare`, `@projection` | Iterator | [`is_sorted_until`](algorithms-ordering.md#is_sorted_until) |
| [`is_sorted`](#is_sorted) | `@range` | optional `@compare`, `@projection` | `BOOL` | [`is_sorted`](algorithms-ordering.md#is_sorted) |
| [`lower_bound`](#lower_bound) | `@range` | `@value`; optional `@compare`, `@projection` | Iterator | [`lower_bound`](algorithms-ordering.md#lower_bound) |
| [`upper_bound`](#upper_bound) | `@range` | `@value`; optional `@compare`, `@projection` | Iterator | [`upper_bound`](algorithms-ordering.md#upper_bound) |
| [`partition_point`](#partition_point) | `@range` | `@predicate`; optional `@projection` | Iterator | [`partition_point`](algorithms-ordering.md#partition_point) |
| [`equal_range`](#equal_range) | `@range` | `@value`; optional `@compare`, `@projection` | `std::iterator_range` | [`equal_range`](algorithms-ordering.md#equal_range) |
| [`binary_search`](#binary_search) | `@range` | `@value`; optional `@compare`, `@projection` | `BOOL` | [`binary_search`](algorithms-ordering.md#binary_search) |
| [`merge`](#merge) | `@range1`, `@range2` | `@output`; optional `@compare`, `@projection1`, `@projection2` | Composite `in1`, `in2`, `out` | [`merge`](algorithms-ordering.md#merge) |
| [`set_union`](#set_union) | `@range1`, `@range2` | `@output`; optional `@compare`, `@projection1`, `@projection2` | Composite `in1`, `in2`, `out` | [`set_union`](algorithms-ordering.md#set_union) |
| [`set_intersection`](#set_intersection) | `@range1`, `@range2` | `@output`; optional `@compare`, `@projection1`, `@projection2` | Composite `in1`, `in2`, `out` | [`set_intersection`](algorithms-ordering.md#set_intersection) |
| [`set_difference`](#set_difference) | `@range1`, `@range2` | `@output`; optional `@compare`, `@projection1`, `@projection2` | Composite `in1`, `in2`, `out` | [`set_difference`](algorithms-ordering.md#set_difference) |
| [`set_symmetric_difference`](#set_symmetric_difference) | `@range1`, `@range2` | `@output`; optional `@compare`, `@projection1`, `@projection2` | Composite `in1`, `in2`, `out` | [`set_symmetric_difference`](algorithms-ordering.md#set_symmetric_difference) |
| [`includes`](#includes) | `@range1`, `@range2` | optional `@compare`, `@projection1`, `@projection2` | `BOOL` | [`includes`](algorithms-ordering.md#includes) |
| [`next_permutation`](#next_permutation) | `@range` | optional `@compare`, `@projection` | `BOOL` | [`next_permutation`](algorithms-ordering.md#next_permutation) |
| [`prev_permutation`](#prev_permutation) | `@range` | optional `@compare`, `@projection` | `BOOL` | [`prev_permutation`](algorithms-ordering.md#prev_permutation) |

**Range forms of the buffered algorithms**

| Function | Range arguments | Other named arguments | Result | Iterator form |
| --- | --- | --- | --- | --- |
| [`stable_partition`](#stable_partition) | `@range` | `@predicate`, `@scratch`; optional `@projection` | Iterator | [`stable_partition`](algorithms-buffered.md#stable_partition) |
| [`inplace_merge`](#inplace_merge) | `@range` | `@middle`, `@scratch`; optional `@compare`, `@projection` | None | [`inplace_merge`](algorithms-buffered.md#inplace_merge) |
| [`stable_sort`](#stable_sort) | `@range` | `@scratch`; optional `@compare`, `@projection` | None | [`stable_sort`](algorithms-buffered.md#stable_sort) |

## Range forms of the search, comparison, and fold algorithms

The iterator forms of the functions in this section are specified in [Search, comparison, and fold algorithms](algorithms-search.md).

### `all_of`

```quxlang
// (1) range form
::all_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::all_of FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`all_of`](algorithms-search.md#all_of) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`all_of`](algorithms-search.md#all_of).

### `any_of`

```quxlang
// (1) range form
::any_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::any_of FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`any_of`](algorithms-search.md#any_of) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`any_of`](algorithms-search.md#any_of).

### `none_of`

```quxlang
// (1) range form
::none_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::none_of FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`none_of`](algorithms-search.md#none_of) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`none_of`](algorithms-search.md#none_of).

### `count`

```quxlang
// (1) range form
::count FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::count FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`count`](algorithms-search.md#count) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `SZ`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`count`](algorithms-search.md#count).

### `count_if`

```quxlang
// (1) range form
::count_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::count_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`count_if`](algorithms-search.md#count_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `SZ`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`count_if`](algorithms-search.md#count_if).

### `find`

```quxlang
// (1) range form
::find FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::find FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`find`](algorithms-search.md#find) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`find`](algorithms-search.md#find).

### `find_if`

```quxlang
// (1) range form
::find_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::find_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`find_if`](algorithms-search.md#find_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`find_if`](algorithms-search.md#find_if).

### `find_if_not`

```quxlang
// (1) range form
::find_if_not FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::find_if_not FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`find_if_not`](algorithms-search.md#find_if_not) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`find_if_not`](algorithms-search.md#find_if_not).

### `for_each`

```quxlang
// (1) range form
::for_each FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::for_each FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`for_each`](algorithms-search.md#for_each) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@action` (required).

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`for_each`](algorithms-search.md#for_each).

### `find_first_of`

```quxlang
::find_first_of FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`find_first_of`](algorithms-search.md#find_first_of) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range1.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`find_first_of`](algorithms-search.md#find_first_of).

### `adjacent_find`

```quxlang
// (1) range form
::adjacent_find FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::adjacent_find FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`adjacent_find`](algorithms-search.md#adjacent_find) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@equal` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`adjacent_find`](algorithms-search.md#adjacent_find).

### `search`

```quxlang
::search FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`search`](algorithms-search.md#search) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a [`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose iterator type and sentinel type are both the type that `range1.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`search`](algorithms-search.md#search).

### `find_end`

```quxlang
::find_end FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`find_end`](algorithms-search.md#find_end) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a [`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose iterator type and sentinel type are both the type that `range1.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`find_end`](algorithms-search.md#find_end).

### `search_n`

```quxlang
// (1) range form
::search_n FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::search_n FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`search_n`](algorithms-search.md#search_n) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@count` (required), `@value` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a [`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose iterator type and sentinel type are both the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`search_n`](algorithms-search.md#search_n).

### `equal`

```quxlang
::equal FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`equal`](algorithms-search.md#equal) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`equal`](algorithms-search.md#equal).

### `mismatch`

```quxlang
::mismatch FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`mismatch`](algorithms-search.md#mismatch) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1` and `in2`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`mismatch`](algorithms-search.md#mismatch).

### `lexicographical_compare`

```quxlang
::lexicographical_compare FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`lexicographical_compare`](algorithms-search.md#lexicographical_compare) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `ORDER`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`lexicographical_compare`](algorithms-search.md#lexicographical_compare).

### `is_permutation`

```quxlang
::is_permutation FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`is_permutation`](algorithms-search.md#is_permutation) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@equal` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_permutation`](algorithms-search.md#is_permutation).

### `min_element`

```quxlang
// (1) range form
::min_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::min_element FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`min_element`](algorithms-search.md#min_element) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`min_element`](algorithms-search.md#min_element).

### `max_element`

```quxlang
// (1) range form
::max_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::max_element FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`max_element`](algorithms-search.md#max_element) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`max_element`](algorithms-search.md#max_element).

### `minmax_element`

```quxlang
// (1) range form
::minmax_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::minmax_element FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`minmax_element`](algorithms-search.md#minmax_element) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `min` and `max`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`minmax_element`](algorithms-search.md#minmax_element).

### `fold_left`

```quxlang
// (1) range form
::fold_left FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::fold_left FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`fold_left`](algorithms-search.md#fold_left) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@initial` (required) and `@operation` (required).

**Return value.** The function returns the result of the iterator form. The result type is the value type of the `@initial` argument.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`fold_left`](algorithms-search.md#fold_left).

### `fold_right`

```quxlang
// (1) range form
::fold_right FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::fold_right FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`fold_right`](algorithms-search.md#fold_right) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@initial` (required) and `@operation` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result type is the value type of the `@initial` argument.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`fold_right`](algorithms-search.md#fold_right).

## Range forms of the transfer and modification algorithms

The iterator forms of the functions in this section are specified in [Transfer and modification algorithms](algorithms-transfer.md).

### `copy`

```quxlang
// (1) range form
::copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`copy`](algorithms-transfer.md#copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`copy`](algorithms-transfer.md#copy).

### `move`

```quxlang
// (1) range form
::move FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::move FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`move`](algorithms-transfer.md#move) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`move`](algorithms-transfer.md#move).

### `copy_backward`

```quxlang
// (1) range form
::copy_backward FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::copy_backward FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`copy_backward`](algorithms-transfer.md#copy_backward) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output_last` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`copy_backward`](algorithms-transfer.md#copy_backward).

### `move_backward`

```quxlang
// (1) range form
::move_backward FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::move_backward FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`move_backward`](algorithms-transfer.md#move_backward) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output_last` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`move_backward`](algorithms-transfer.md#move_backward).

### `copy_if`

```quxlang
// (1) range form
::copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::copy_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`copy_if`](algorithms-transfer.md#copy_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@predicate` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`copy_if`](algorithms-transfer.md#copy_if).

### `remove_copy`

```quxlang
// (1) range form
::remove_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::remove_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`remove_copy`](algorithms-transfer.md#remove_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@value` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`remove_copy`](algorithms-transfer.md#remove_copy).

### `remove_copy_if`

```quxlang
// (1) range form
::remove_copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::remove_copy_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`remove_copy_if`](algorithms-transfer.md#remove_copy_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@predicate` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`remove_copy_if`](algorithms-transfer.md#remove_copy_if).

### `transform`

```quxlang
// (1) range form
::transform FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::transform FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
// (3) range form
::transform FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

1. The function calls the unary iterator form of [`transform`](algorithms-transfer.md#transform) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`
3. The function calls the binary iterator form of [`transform`](algorithms-transfer.md#transform) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required) and `@operation` (required).

**Return value.** The function returns the result of the iterator form. The result of overload (1) is a composite with the fields `in` and `out`. The result of overload (3) is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`transform`](algorithms-transfer.md#transform).

### `fill`

```quxlang
// (1) range form
::fill FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::fill FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`fill`](algorithms-transfer.md#fill) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required).

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`fill`](algorithms-transfer.md#fill).

### `generate`

```quxlang
// (1) range form
::generate FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::generate FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`generate`](algorithms-transfer.md#generate) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@generator` (required).

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`generate`](algorithms-transfer.md#generate).

### `replace`

```quxlang
// (1) range form
::replace FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::replace FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`replace`](algorithms-transfer.md#replace) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@old_value` (required), `@equal` (optional), `@new_value` (required), and `@projection` (optional).

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`replace`](algorithms-transfer.md#replace).

### `replace_if`

```quxlang
// (1) range form
::replace_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::replace_if FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`replace_if`](algorithms-transfer.md#replace_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required), `@new_value` (required), and `@projection` (optional).

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`replace_if`](algorithms-transfer.md#replace_if).

### `replace_copy`

```quxlang
// (1) range form
::replace_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::replace_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`replace_copy`](algorithms-transfer.md#replace_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@old_value` (required), `@equal` (optional), `@new_value` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`replace_copy`](algorithms-transfer.md#replace_copy).

### `replace_copy_if`

```quxlang
// (1) range form
::replace_copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::replace_copy_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`replace_copy_if`](algorithms-transfer.md#replace_copy_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@predicate` (required), `@new_value` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`replace_copy_if`](algorithms-transfer.md#replace_copy_if).

### `remove`

```quxlang
// (1) range form
::remove FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::remove FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`remove`](algorithms-transfer.md#remove) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`remove`](algorithms-transfer.md#remove).

### `remove_if`

```quxlang
// (1) range form
::remove_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::remove_if FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`remove_if`](algorithms-transfer.md#remove_if) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`remove_if`](algorithms-transfer.md#remove_if).

### `unique`

```quxlang
// (1) range form
::unique FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::unique FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`unique`](algorithms-transfer.md#unique) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@equal` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`unique`](algorithms-transfer.md#unique).

### `unique_copy`

```quxlang
// (1) range form
::unique_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::unique_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`unique_copy`](algorithms-transfer.md#unique_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required), `@equal` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`unique_copy`](algorithms-transfer.md#unique_copy).

### `reverse`

```quxlang
// (1) range form
::reverse FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::reverse FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`reverse`](algorithms-transfer.md#reverse) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts no named argument, because the iterator form declares no parameter other than the boundaries.

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`reverse`](algorithms-transfer.md#reverse).

### `reverse_copy`

```quxlang
// (1) range form
::reverse_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::reverse_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`reverse_copy`](algorithms-transfer.md#reverse_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`reverse_copy`](algorithms-transfer.md#reverse_copy).

### `rotate`

```quxlang
// (1) range form
::rotate FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::rotate FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`rotate`](algorithms-transfer.md#rotate) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@middle` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`rotate`](algorithms-transfer.md#rotate).

### `rotate_copy`

```quxlang
// (1) range form
::rotate_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::rotate_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`rotate_copy`](algorithms-transfer.md#rotate_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@middle` (required) and `@output` (required).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`rotate_copy`](algorithms-transfer.md#rotate_copy).

### `swap_ranges`

```quxlang
::swap_ranges FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`swap_ranges`](algorithms-transfer.md#swap_ranges) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts no named argument, because the iterator form declares no parameter other than the boundaries.

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1` and `in2`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`swap_ranges`](algorithms-transfer.md#swap_ranges).

### `partition`

```quxlang
// (1) range form
::partition FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::partition FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`partition`](algorithms-transfer.md#partition) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`partition`](algorithms-transfer.md#partition).

### `is_partitioned`

```quxlang
// (1) range form
::is_partitioned FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::is_partitioned FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`is_partitioned`](algorithms-transfer.md#is_partitioned) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_partitioned`](algorithms-transfer.md#is_partitioned).

### `partition_copy`

```quxlang
// (1) range form
::partition_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::partition_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`partition_copy`](algorithms-transfer.md#partition_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output_true` (required), `@output_false` (required), `@predicate` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in`, `out_true`, and `out_false`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`partition_copy`](algorithms-transfer.md#partition_copy).

## Range forms of the ordering algorithms

The iterator forms of the functions in this section are specified in [Ordering algorithms](algorithms-ordering.md).

### `make_heap`

```quxlang
// (1) range form
::make_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::make_heap FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`make_heap`](algorithms-ordering.md#make_heap) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`make_heap`](algorithms-ordering.md#make_heap).

### `push_heap`

```quxlang
// (1) range form
::push_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::push_heap FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`push_heap`](algorithms-ordering.md#push_heap) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`push_heap`](algorithms-ordering.md#push_heap).

### `pop_heap`

```quxlang
// (1) range form
::pop_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::pop_heap FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`pop_heap`](algorithms-ordering.md#pop_heap) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`pop_heap`](algorithms-ordering.md#pop_heap).

### `sort_heap`

```quxlang
// (1) range form
::sort_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::sort_heap FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`sort_heap`](algorithms-ordering.md#sort_heap) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`sort_heap`](algorithms-ordering.md#sort_heap).

### `is_heap_until`

```quxlang
// (1) range form
::is_heap_until FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::is_heap_until FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`is_heap_until`](algorithms-ordering.md#is_heap_until) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_heap_until`](algorithms-ordering.md#is_heap_until).

### `is_heap`

```quxlang
// (1) range form
::is_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::is_heap FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`is_heap`](algorithms-ordering.md#is_heap) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_heap`](algorithms-ordering.md#is_heap).

### `sort`

```quxlang
// (1) range form
::sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::sort FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`sort`](algorithms-ordering.md#sort) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`sort`](algorithms-ordering.md#sort).

### `nth_element`

```quxlang
// (1) range form
::nth_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::nth_element FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`nth_element`](algorithms-ordering.md#nth_element) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@nth` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`nth_element`](algorithms-ordering.md#nth_element).

### `partial_sort`

```quxlang
// (1) range form
::partial_sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::partial_sort FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`partial_sort`](algorithms-ordering.md#partial_sort) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@middle` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`partial_sort`](algorithms-ordering.md#partial_sort).

### `partial_sort_copy`

```quxlang
// (1) range form
::partial_sort_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::partial_sort_copy FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`partial_sort_copy`](algorithms-ordering.md#partial_sort_copy) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@output_first` (required), `@output_last` (required), `@compare` (optional), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in` and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`partial_sort_copy`](algorithms-ordering.md#partial_sort_copy).

### `is_sorted_until`

```quxlang
// (1) range form
::is_sorted_until FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::is_sorted_until FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`is_sorted_until`](algorithms-ordering.md#is_sorted_until) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_sorted_until`](algorithms-ordering.md#is_sorted_until).

### `is_sorted`

```quxlang
// (1) range form
::is_sorted FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::is_sorted FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`is_sorted`](algorithms-ordering.md#is_sorted) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`is_sorted`](algorithms-ordering.md#is_sorted).

### `lower_bound`

```quxlang
// (1) range form
::lower_bound FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::lower_bound FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`lower_bound`](algorithms-ordering.md#lower_bound) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`lower_bound`](algorithms-ordering.md#lower_bound).

### `upper_bound`

```quxlang
// (1) range form
::upper_bound FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::upper_bound FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`upper_bound`](algorithms-ordering.md#upper_bound) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`upper_bound`](algorithms-ordering.md#upper_bound).

### `partition_point`

```quxlang
// (1) range form
::partition_point FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::partition_point FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`partition_point`](algorithms-ordering.md#partition_point) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`partition_point`](algorithms-ordering.md#partition_point).

### `equal_range`

```quxlang
// (1) range form
::equal_range FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::equal_range FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`equal_range`](algorithms-ordering.md#equal_range) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result is a [`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose iterator type and sentinel type are both the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`equal_range`](algorithms-ordering.md#equal_range).

### `binary_search`

```quxlang
// (1) range form
::binary_search FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::binary_search FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`binary_search`](algorithms-ordering.md#binary_search) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@value` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`binary_search`](algorithms-ordering.md#binary_search).

### `merge`

```quxlang
::merge FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`merge`](algorithms-ordering.md#merge) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required), `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`merge`](algorithms-ordering.md#merge).

### `set_union`

```quxlang
::set_union FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`set_union`](algorithms-ordering.md#set_union) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required), `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`set_union`](algorithms-ordering.md#set_union).

### `set_intersection`

```quxlang
::set_intersection FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`set_intersection`](algorithms-ordering.md#set_intersection) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required), `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`set_intersection`](algorithms-ordering.md#set_intersection).

### `set_difference`

```quxlang
::set_difference FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`set_difference`](algorithms-ordering.md#set_difference) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required), `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`set_difference`](algorithms-ordering.md#set_difference).

### `set_symmetric_difference`

```quxlang
::set_symmetric_difference FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`set_symmetric_difference`](algorithms-ordering.md#set_symmetric_difference) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@output` (required), `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result is a composite with the fields `in1`, `in2`, and `out`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`set_symmetric_difference`](algorithms-ordering.md#set_symmetric_difference).

### `includes`

```quxlang
::includes FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function calls the iterator form of [`includes`](algorithms-ordering.md#includes) with `@first1 range1.BEGIN()`, `@last1 range1.END()`, `@first2 range2.BEGIN()`, `@last2 range2.END()`, and the named arguments captured by `options`.

**Named arguments.** `options` accepts `@compare` (optional), `@projection1` (optional), and `@projection2` (optional).

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`includes`](algorithms-ordering.md#includes).

### `next_permutation`

```quxlang
// (1) range form
::next_permutation FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::next_permutation FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`next_permutation`](algorithms-ordering.md#next_permutation) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`next_permutation`](algorithms-ordering.md#next_permutation).

### `prev_permutation`

```quxlang
// (1) range form
::prev_permutation FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::prev_permutation FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`prev_permutation`](algorithms-ordering.md#prev_permutation) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@compare` (optional) and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns the result of the iterator form. The result type is `BOOL`.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`prev_permutation`](algorithms-ordering.md#prev_permutation).

## Range forms of the buffered algorithms

The iterator forms of the functions in this section are specified in [Buffered algorithms](algorithms-buffered.md).

### `stable_partition`

```quxlang
// (1) range form
::stable_partition FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (2) rejects a temporary range
::stable_partition FUNCTION(@range TEMP& AUTO, @KWARGS ...): AUTO
```

1. The function calls the iterator form of [`stable_partition`](algorithms-buffered.md#stable_partition) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@predicate` (required), `@scratch` (required), and `@projection` (optional).

**Return value.** The function returns the result of the iterator form. The result is an iterator of the type that `range.BEGIN()` returns.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`stable_partition`](algorithms-buffered.md#stable_partition).

### `inplace_merge`

```quxlang
// (1) range form
::inplace_merge FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::inplace_merge FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`inplace_merge`](algorithms-buffered.md#inplace_merge) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@middle` (required), `@scratch` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`inplace_merge`](algorithms-buffered.md#inplace_merge).

### `stable_sort`

```quxlang
// (1) range form
::stable_sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
// (2) rejects a temporary range
::stable_sort FUNCTION(@range TEMP& AUTO, @KWARGS ...)
```

1. The function calls the iterator form of [`stable_sort`](algorithms-buffered.md#stable_sort) with `@first range.BEGIN()`, `@last range.END()`, and the named arguments captured by `options`.
2. The overload is selected when `range` is a temporary. The overload stops compilation with the diagnostic `Algorithm range arguments must be lvalues.`

**Named arguments.** `options` accepts `@scratch` (required), `@compare` (optional), and `@projection` (optional).

**Range requirements.** `range.BEGIN()` and `range.END()` return the same type, because the iterator form declares `first` and `last` with one deduced type.

**Return value.** The function returns no value.

The effects, preconditions, complexity, and exception behavior are those of the iterator form of [`stable_sort`](algorithms-buffered.md#stable_sort).

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_range_forms/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_range_forms/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Iterator utilities](algorithms-iterators.md)
- [Search, comparison, and fold algorithms](algorithms-search.md)
- [Transfer and modification algorithms](algorithms-transfer.md)
- [Ordering algorithms](algorithms-ordering.md)
- [Buffered algorithms](algorithms-buffered.md)
- [Call arguments](../../reference/call-arguments.md)
- [Variadic packs](../../reference/variadic-packs.md)
- [Failure statements](../../reference/diagnostics-and-failure.md)
