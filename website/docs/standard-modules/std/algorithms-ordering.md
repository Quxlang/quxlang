# Ordering algorithms

Module `std` · source file `algorithm_ordering.qxs`

The functions on this page depend on a three-way ordering of the elements. The
page covers heap maintenance, sorting, selection, tests for sorted order,
binary search, merging, multiset operations on sorted sequences, and
permutation enumeration. The shared sequence model and the argument-passing
conventions are specified in [Algorithms](algorithms.md). The stable sort and
the in-place merge, which require scratch storage, are specified in
[Buffered algorithms](algorithms-buffered.md).

## Conventions

Every function has an iterator form and a range form. The iterator form is
declared in `algorithm_ordering.qxs`. The range form is declared in
`algorithm_ranges.qxs`; it evaluates `range.BEGIN()` and `range.END()` once
each and calls the iterator form with the results, as specified in
[Range forms](algorithms-ranges.md). Every argument is a named argument.

A parameter list that ends in `@KWARGS:options ...` accepts the named arguments
listed in the table that follows the declarations.

The following notation applies to every section.

- `N` is the number of elements in [`first`, `last`) or [`first1`, `last1`).
- `M` is the number of elements in [`first2`, `last2`).
- A *projected element* is the result of `projection(element)`.
- A declared return type of `AUTO` is deduced from the implementation. Each
  section states the resulting type.

**Ordering.** The argument `@compare` is invoked as
`compare(@left a, @right b)` and returns `ORDER`. The operands are projected
elements, except that a lookup value is passed without projection. The default
comparator is a
[`std::compare_three_way`](algorithms-iterators.md#stdcompare_three_way)
object, which applies the operator `<=>`. For floating-point elements, `<=>` is
the total order specified in
[Floating-point ordering](../../reference/floating-point-ordering.md). A
callable whose result type is not `ORDER`, such as a callable that returns
`BOOL`, is rejected during compilation.

The following terms are defined with respect to `compare` and the projection.

- An element `a` *precedes* an element `b` when
  `compare(@left projection(a), @right projection(b))` returns `ORDER::LESS`.
- A sequence is *sorted* when no element precedes its predecessor.
- A sequence is a *heap* when, for every zero-based index `i` greater than
  zero, the element at index `(i - 1) / 2` does not precede the element at
  index `i`. The first element of a nonempty heap is a maximum element.

A descending order is obtained with a comparator that exchanges its operands,
for example `-< (@left I32, @right I32): ORDER = right <=> left`.

**Iterator requirements.** Each section names one of the following sets of
requirements.

| Term | Required operations |
| --- | --- |
| Single-pass input | `first != last`, `first++`, `first->`. The sentinel type of `last` can differ from the iterator type. |
| Forward | The single-pass operations, and copies of an iterator denote stable positions that can be traversed again. |
| Bidirectional | The forward operations and `--`. `first` and `last` have the same type. |
| Random access | The forward operations, `last - first` with a signed element distance as the result, and `first + count` for a `count` of type `SZ`. `first` and `last` have the same type. |

[`pop_heap`](#pop_heap), [`sort_heap`](#sort_heap), [`sort`](#sort),
[`nth_element`](#nth_element), and [`partial_sort`](#partial_sort) additionally
use `last - count` for a `count` of type `SZ`, and all of these functions
except `pop_heap` additionally use `--`. [`sort`](#sort) and
[`nth_element`](#nth_element) additionally compare two iterators with `<`.
Array pointers and the iterators of [`std::dynarr`](dynarr.md) provide all of
these operations.

A function that reorders elements exchanges them with the swap operator `<->`
and requires `first->` to produce a mutable reference. A range of constant
elements is rejected during compilation.

**Preconditions.** `last` is reachable from `first` by repeated `++`. Each
iterator in [`first`, `last`) is dereferenceable. The random-access functions
convert `last - first` to `SZ` with `AS ASSUME`; a negative distance is
undefined behavior.

**Exceptions.** The functions contain no exception handlers. An exception
thrown by a callable, an iterator operation, or an element operation propagates
to the caller. A reordering function that is interrupted by an exception leaves
the elements in an intermediate arrangement.

## Function summary

**Heap operations**

| Function | Description |
| --- | --- |
| [`make_heap`](#make_heap) | Arranges a sequence into a heap. |
| [`push_heap`](#push_heap) | Extends a heap by the final element of the sequence. |
| [`pop_heap`](#pop_heap) | Moves the maximum element of a heap to the final position. |
| [`sort_heap`](#sort_heap) | Converts a heap into a sorted sequence. |
| [`is_heap_until`](#is_heap_until) | Finds the first element that violates the heap property. |
| [`is_heap`](#is_heap) | Tests whether a sequence is a heap. |

**Sorting and selection**

| Function | Description |
| --- | --- |
| [`sort`](#sort) | Sorts a sequence. |
| [`nth_element`](#nth_element) | Places one element at its sorted position and partitions the sequence around it. |
| [`partial_sort`](#partial_sort) | Sorts the smallest elements into a prefix of the sequence. |
| [`partial_sort_copy`](#partial_sort_copy) | Copies the smallest elements into a bounded destination in sorted order. |
| [`is_sorted_until`](#is_sorted_until) | Finds the first element that precedes its predecessor. |
| [`is_sorted`](#is_sorted) | Tests whether a sequence is sorted. |

**Binary search**

| Function | Description |
| --- | --- |
| [`lower_bound`](#lower_bound) | Finds the first element that does not precede a value. |
| [`upper_bound`](#upper_bound) | Finds the first element that a value precedes. |
| [`equal_range`](#equal_range) | Finds the subsequence of elements equal to a value. |
| [`binary_search`](#binary_search) | Tests whether a sorted sequence contains a value. |
| [`partition_point`](#partition_point) | Finds the boundary of a partitioned sequence. |

**Merging and multiset operations**

| Function | Description |
| --- | --- |
| [`merge`](#merge) | Merges two sorted sequences. |
| [`set_union`](#set_union) | Writes the multiset union of two sorted sequences. |
| [`set_intersection`](#set_intersection) | Writes the multiset intersection of two sorted sequences. |
| [`set_difference`](#set_difference) | Writes the multiset difference of two sorted sequences. |
| [`set_symmetric_difference`](#set_symmetric_difference) | Writes the multiset symmetric difference of two sorted sequences. |
| [`includes`](#includes) | Tests whether one sorted sequence contains another as a multiset. |

**Permutations**

| Function | Description |
| --- | --- |
| [`next_permutation`](#next_permutation) | Rearranges a sequence into the next permutation. |
| [`prev_permutation`](#prev_permutation) | Rearranges a sequence into the preceding permutation. |

## Heap operations

### `make_heap`

```quxlang
// (1) iterator form
::make_heap FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::make_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function rearranges the elements so that the sequence is a heap.

**Iterator requirements.** Random access.

**Return value.** The function returns no value.

**Complexity.** The number of applications of `compare` is linear in `N`.

### `push_heap`

```quxlang
// (1) iterator form
::push_heap FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::push_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function extends the heap [`first`, `last - 1`) by the element at
`last - 1`. The function exchanges the new element with its parent until the
parent does not precede it. An empty sequence is unchanged.

**Iterator requirements.** Random access.

**Preconditions.** The sequence [`first`, `last - 1`) is a heap.

**Return value.** The function returns no value.

**Complexity.** The function performs a number of applications of `compare`
that is logarithmic in `N`.

### `pop_heap`

```quxlang
// (1) iterator form
::pop_heap FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::pop_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function exchanges the first element with the final element and restores
the heap property of [`first`, `last - 1`). After the call, the element at
`last - 1` is the former maximum element. A sequence of fewer than two elements
is unchanged.

**Iterator requirements.** Random access.

**Preconditions.** The sequence [`first`, `last`) is a heap.

**Return value.** The function returns no value.

**Complexity.** The function performs a number of applications of `compare`
that is logarithmic in `N`.

### `sort_heap`

```quxlang
// (1) iterator form
::sort_heap FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::sort_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies [`pop_heap`](#pop_heap) to successively shorter prefixes
of the sequence. After the call, the sequence is sorted.

**Iterator requirements.** Random access.

**Preconditions.** The sequence [`first`, `last`) is a heap.

**Return value.** The function returns no value.

**Complexity.** The function performs `O(N log N)` applications of `compare`.

### `is_heap_until`

```quxlang
// (1) iterator form
::is_heap_until FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::is_heap_until FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function examines the elements in index order and compares each element
after the first with its parent.

**Iterator requirements.** Random access.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that its parent precedes. The result is `last` when the sequence
is a heap, including when the sequence is empty.

**Complexity.** The function performs at most `N - 1` applications of
`compare`.

### `is_heap`

```quxlang
// (1) iterator form
::is_heap FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::is_heap FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

**Iterator requirements.** Random access.

**Return value.** The result type is `BOOL`. The result is `TRUE` when
[`is_heap_until`](#is_heap_until) returns `last` for the same arguments.

**Complexity.** The function performs at most `N - 1` applications of
`compare`.

## Sorting and selection

### `sort`

```quxlang
// (1) iterator form
::sort FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function sorts the sequence. The relative order of elements that compare
equal is not preserved; [`stable_sort`](algorithms-buffered.md#stable_sort)
preserves that order.

The implementation is a deterministic introsort and uses no random numbers.

- A subsequence of at most 16 elements is sorted by insertion.
- A longer subsequence is partitioned around the median of its first, middle,
  and final elements. The partition has three parts: the elements that precede
  the pivot, the elements equal to the pivot, and the elements that the pivot
  precedes.
- The function recurses into the smaller outer part and continues iteratively
  with the larger outer part.
- The partitioning depth is limited to twice the binary logarithm of `N`,
  rounded down. A subsequence that reaches the limit is sorted with
  [`make_heap`](#make_heap) and [`sort_heap`](#sort_heap).

**Iterator requirements.** Random access.

**Return value.** The function returns no value.

**Complexity.** The function performs `O(N log N)` applications of `compare` in
the worst case and uses `O(log N)` stack space.

### `nth_element`

```quxlang
// (1) iterator form
::nth_element FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::nth_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@nth` | `AUTO(input_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function rearranges the sequence so that the element at `nth` is the
element that would occupy that position in the sorted sequence. After the call,
no element after `nth` precedes the element at `nth`, and the element at `nth`
precedes no element before `nth`. The order within the two outer groups is
unspecified. The function has no effect when `nth` equals `last`.

The implementation is a deterministic median-of-medians selection. A sequence
of at most five elements is sorted by insertion. A longer sequence is divided
into groups of five elements; the median of the group medians is selected by a
recursive call and becomes the pivot of a three-part partition.

**Iterator requirements.** Random access. `nth` has the same type as `first`
and `last`.

**Preconditions.** `nth` denotes a position in [`first`, `last`].

**Return value.** The function returns no value.

**Complexity.** The function performs a number of applications of `compare`
that is linear in `N` in the worst case.

### `partial_sort`

```quxlang
// (1) iterator form
::partial_sort FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::partial_sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@middle` | `AUTO(input_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function rearranges the sequence so that [`first`, `middle`) contains the
`middle - first` smallest elements in sorted order. The order of the elements
in [`middle`, `last`) is unspecified. The function has no effect when `first`
equals `middle`.

The implementation arranges [`first`, `middle`) into a heap, exchanges each
later element that precedes the heap maximum with that maximum, and finally
applies [`sort_heap`](#sort_heap) to the prefix.

**Iterator requirements.** Random access. `middle` has the same type as `first`
and `last`.

**Preconditions.** `middle` denotes a position in [`first`, `last`].

**Return value.** The function returns no value.

**Complexity.** The function performs `O(N log K)` applications of `compare`,
where `K` is `middle - first`.

### `partial_sort_copy`

```quxlang
// (1) iterator form
::partial_sort_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::partial_sort_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output_first` | `AUTO(output_iterator)` | Required. |
| `@output_last` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies the smallest elements of the input sequence into the
destination [`output_first`, `output_last`) in sorted order. The number of
elements written is the smaller of `N` and the number of destination positions.
The input sequence is not modified.

The implementation copies input elements until the destination is full or the
input ends, arranges the written elements into a heap, replaces the heap
maximum with each later input element that precedes it, and finally applies
[`sort_heap`](#sort_heap) to the written elements. The projection is applied to
input elements and to destination elements.

**Iterator requirements.** The input is a single-pass input. `output_first` and
`output_last` have the same type, and that type satisfies the requirements of
[`make_heap`](#make_heap) and [`sort_heap`](#sort_heap).

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input; the complete input is traversed. `out` is the position
after the last element written.

**Complexity.** The function performs `O(N log K)` applications of `compare`,
where `K` is the number of elements written.

### `is_sorted_until`

```quxlang
// (1) iterator form
::is_sorted_until FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::is_sorted_until FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function compares each element after the first with its predecessor.

**Iterator requirements.** Forward.

**Return value.** The result type is `input_iterator`. The result denotes the
first element for which
`compare(@left projection(predecessor), @right projection(element))` returns
`ORDER::GREATER`. The result is the end position
when the sequence is sorted, including when the sequence contains fewer than
two elements.

**Complexity.** The function performs at most `N - 1` applications of
`compare`.

### `is_sorted`

```quxlang
// (1) iterator form
::is_sorted FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::is_sorted FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

**Iterator requirements.** Forward. The iterator type supports `==` with the
type of `last`.

**Return value.** The result type is `BOOL`. The result is `TRUE` when
[`is_sorted_until`](#is_sorted_until) returns a position equal to `last` for
the same arguments.

**Complexity.** The function performs at most `N - 1` applications of
`compare`.

## Binary search

The binary search functions require random-access iterators. A call with
iterators that provide only forward traversal is rejected during compilation.
The lookup value `value` is passed to `compare` as the `@right` operand without
projection, so the type of `value` can differ from the element type.

### `lower_bound`

```quxlang
// (1) iterator form
::lower_bound FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::lower_bound FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function finds the first element for which
`compare(@left projection(element), @right value)` does not return
`ORDER::LESS`.

**Iterator requirements.** Random access.

**Preconditions.** Every element for which the comparison returns `ORDER::LESS`
is located before every other element. A sorted sequence satisfies this
requirement.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that does not precede `value`, or `last` when every element
precedes `value`. The result is `first` when the sequence is empty.

**Complexity.** The function performs at most `floor(log2(N)) + 1` applications
of `compare` for a nonempty sequence. The number of iterator operations is also
logarithmic in `N`.

### `upper_bound`

```quxlang
// (1) iterator form
::upper_bound FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::upper_bound FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function finds the first element for which
`compare(@left projection(element), @right value)` returns `ORDER::GREATER`.

**Iterator requirements.** Random access.

**Preconditions.** Every element for which the comparison does not return
`ORDER::GREATER` is located before every other element. A sorted sequence
satisfies this requirement.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that `value` precedes, or `last` when no such element exists. The
result is `first` when the sequence is empty.

**Complexity.** The function performs at most `floor(log2(N)) + 1` applications
of `compare` for a nonempty sequence.

### `equal_range`

```quxlang
// (1) iterator form
::equal_range FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::equal_range FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function computes [`lower_bound`](#lower_bound) for the complete sequence
and [`upper_bound`](#upper_bound) for the part of the sequence that begins at
the lower bound.

**Iterator requirements.** Random access.

**Preconditions.** The sequence is sorted.

**Return value.** The result is a
[`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose
iterator type and sentinel type are both `input_iterator`. `BEGIN()` is the
lower bound and `END()` is the upper bound. The range contains the elements
that compare equal to `value`. The range is empty when no such element exists.

**Complexity.** The function performs `O(log N)` applications of `compare`.

### `binary_search`

```quxlang
// (1) iterator form
::binary_search FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::binary_search FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function computes [`lower_bound`](#lower_bound) and compares the element at
the resulting position with `value`.

**Iterator requirements.** Random access.

**Preconditions.** The sequence is sorted.

**Return value.** The result type is `BOOL`. The result is `TRUE` when the
lower bound differs from `last` and
`compare(@left projection(element), @right value)` returns `ORDER::EQUAL` for
the element at the lower bound.

**Complexity.** The function performs `O(log N)` applications of `compare`.

### `partition_point`

```quxlang
// (1) iterator form
::partition_point FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::partition_point FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function finds the first element for which
`predicate(projection(element))` returns `FALSE`. The predicate returns `BOOL`.

**Iterator requirements.** Random access.

**Preconditions.** Every element for which `predicate` returns `TRUE` is
located before every element for which `predicate` returns `FALSE`.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that does not satisfy `predicate`, or `last` when every element
satisfies `predicate`.

**Complexity.** The function performs at most `floor(log2(N)) + 1` applications
of `predicate` for a nonempty sequence.

## Merging and multiset operations

The functions in this section read two sorted sequences and compare their
elements through
`compare(@left projection1(element1), @right projection2(element2))`. The
functions that produce output copy the selected elements to `output` and return
a composite with the fields `in1` of type `iterator1`, `in2` of type
`iterator2`, and `out` of type `output_iterator`. `in1` and `in2` are the
positions at which the traversal of each input stopped, and `out` is the
position after the last element written.

**Iterator requirements.** Both inputs are single-pass inputs. `output`
supports `output-> := value` and `output++`.

**Preconditions.** Both input sequences are sorted with respect to `compare`
and their respective projections.

**Complexity.** The number of applications of `compare` is linear in `N + M`;
every comparison advances at least one input.

In the descriptions, an element value that occurs `m` times in the first
sequence and `n` times in the second sequence is considered.

### `merge`

```quxlang
// (1) iterator form
::merge FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::merge FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function writes all elements of both sequences in sorted order. When two
elements compare equal, the element of the first sequence is written first. The
relative order of the elements of each input is preserved. The output contains
`N + M` elements, and both inputs are traversed completely.

### `set_union`

```quxlang
// (1) iterator form
::set_union FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::set_union FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function writes the elements that occur in either sequence. When two
elements compare equal, the function writes the element of the first sequence
once and advances both inputs. An element value is written `max(m, n)` times.
Both inputs are traversed completely.

### `set_intersection`

```quxlang
// (1) iterator form
::set_intersection FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::set_intersection FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function writes the elements that occur in both sequences. When two
elements compare equal, the function writes the element of the first sequence
and advances both inputs. An element value is written `min(m, n)` times. The
traversal stops when either input ends.

### `set_difference`

```quxlang
// (1) iterator form
::set_difference FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::set_difference FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function writes the elements of the first sequence that have no matching
element in the second sequence. When two elements compare equal, the function
advances both inputs without writing. An element value is written
`max(m - n, 0)` times. The first input is traversed completely; `in2` is the
position at which the traversal of the second input stopped.

### `set_symmetric_difference`

```quxlang
// (1) iterator form
::set_symmetric_difference FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::set_symmetric_difference FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function writes the elements of each sequence that have no matching element
in the other sequence. When two elements compare equal, the function advances
both inputs without writing. An element value is written `|m - n|` times. Both
inputs are traversed completely.

### `includes`

```quxlang
// (1) iterator form
::includes FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::includes FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function tests whether the first sequence contains every element of the
second sequence, counting multiplicities: each element of the second sequence
is matched with a distinct element of the first sequence that compares equal.

**Return value.** The result type is `BOOL`. The result is `TRUE` when every
element of the second sequence is matched, including when the second sequence
is empty. The result is `FALSE` otherwise.

**Complexity.** The function performs at most `N` applications of `compare`.

## Permutations

### `next_permutation`

```quxlang
// (1) iterator form
::next_permutation FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::next_permutation FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function rearranges the sequence into the next permutation in the
lexicographical order that `compare` defines. When the sequence is already the
last permutation, the function reverses the sequence, which produces the first
permutation, and returns `FALSE`. A sequence of fewer than two elements is
unchanged.

**Iterator requirements.** Bidirectional.

**Return value.** The result type is `BOOL`. The result is `TRUE` when a next
permutation exists. The result is `FALSE` when the sequence was the last
permutation or contains fewer than two elements.

**Complexity.** The number of applications of `compare` and the number of
exchanges are linear in `N`.

### `prev_permutation`

```quxlang
// (1) iterator form
::prev_permutation FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...): AUTO
// (2) range form
::prev_permutation FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function rearranges the sequence into the preceding permutation in the
lexicographical order that `compare` defines. When the sequence is already the
first permutation, the function reverses the sequence, which produces the last
permutation, and returns `FALSE`. A sequence of fewer than two elements is
unchanged.

**Iterator requirements.** Bidirectional.

**Return value.** The result type is `BOOL`. The result is `TRUE` when a
preceding permutation exists. The result is `FALSE` when the sequence was the
first permutation or contains fewer than two elements.

**Complexity.** The number of applications of `compare` and the number of
exchanges are linear in `N`.

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_ordering/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_ordering/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Range forms](algorithms-ranges.md)
- [Iterator utilities](algorithms-iterators.md)
- [Buffered algorithms](algorithms-buffered.md)
- [Search, comparison, and fold algorithms](algorithms-search.md)
- [`std::dynarr`](dynarr.md)
- [`std::set`](set.md)
- [`std::map`](map.md)
- [Comparison operators](../../reference/comparison-operators.md)
- [Floating-point ordering](../../reference/floating-point-ordering.md)
