# Search, comparison, and fold algorithms

Module `std` · source file `algorithm_search.qxs`

The functions on this page read a sequence without modifying its order. The
page covers quantifier tests, counting, element search, subsequence search,
sequence comparison, extremum selection, and folds. [`for_each`](#for_each) is
the only function on this page that can modify elements, and it does so only
through the supplied callable. The shared sequence model and the
argument-passing conventions are specified in [Algorithms](algorithms.md).

## Conventions

Every function has an iterator form and a range form. The iterator form is
declared in `algorithm_search.qxs`. The range form is declared in
`algorithm_ranges.qxs`; it evaluates `range.BEGIN()` and `range.END()` once
each and calls the iterator form with the results, as specified in
[Range forms](algorithms-ranges.md). Every argument is a named argument.

A parameter list that ends in `@KWARGS:options ...` accepts the named arguments
listed in the table that follows the declarations. For a range form whose
iterator form declares every parameter explicitly, `options` accepts the
remaining parameters of the iterator form.

The following notation applies to every section.

- `N` is the number of elements in [`first`, `last`) or [`first1`, `last1`).
- `M` is the number of elements in [`first2`, `last2`).
- A *projected element* is the result of `projection(element)`.
- Two values are *equivalent* when `equal(@left a, @right b)` returns `TRUE`.
- A declared return type of `AUTO` is deduced from the implementation. Each
  section states the resulting type.

The callable arguments have the following contracts.

| Argument | Invocation | Result type |
| --- | --- | --- |
| `@predicate` | `predicate(projection(element))` | `BOOL` |
| `@equal` | `equal(@left a, @right b)` | `BOOL` |
| `@compare` | `compare(@left a, @right b)` | `ORDER` |
| `@projection`, `@projection1`, `@projection2` | `projection(element)` | Any type accepted by the consuming callable |

**Iterator requirements.** Unless a section states a stronger requirement, an
iterator supports `first != last`, `first++`, and `first->`, and the sentinel
type of `last` can differ from the iterator type. A function that returns an
iterator or retains a position requires an iterator whose copies denote stable
positions that can be traversed again.

**Preconditions.** `last` is reachable from `first` by repeated `++`. Each
iterator in [`first`, `last`) is dereferenceable.

**Exceptions.** The functions contain no exception handlers. An exception
thrown by a callable, an iterator operation, or an element operation propagates
to the caller.

## Function summary

**Quantifiers and counting**

| Function | Description |
| --- | --- |
| [`all_of`](#all_of) | Tests whether a predicate holds for every element. |
| [`any_of`](#any_of) | Tests whether a predicate holds for at least one element. |
| [`none_of`](#none_of) | Tests whether a predicate holds for no element. |
| [`count`](#count) | Counts the elements equivalent to a value. |
| [`count_if`](#count_if) | Counts the elements that satisfy a predicate. |

**Element search**

| Function | Description |
| --- | --- |
| [`find`](#find) | Finds the first element equivalent to a value. |
| [`find_if`](#find_if) | Finds the first element that satisfies a predicate. |
| [`find_if_not`](#find_if_not) | Finds the first element that does not satisfy a predicate. |
| [`find_first_of`](#find_first_of) | Finds the first element equivalent to any element of a second sequence. |
| [`adjacent_find`](#adjacent_find) | Finds the first pair of equivalent adjacent elements. |
| [`min_element`](#min_element) | Finds the first minimum element. |
| [`max_element`](#max_element) | Finds the first maximum element. |
| [`minmax_element`](#minmax_element) | Finds the first minimum element and the first maximum element. |

**Subsequence search**

| Function | Description |
| --- | --- |
| [`search`](#search) | Finds the first occurrence of a pattern sequence. |
| [`find_end`](#find_end) | Finds the last occurrence of a pattern sequence. |
| [`search_n`](#search_n) | Finds the first run of a given number of equivalent elements. |

**Sequence comparison**

| Function | Description |
| --- | --- |
| [`equal`](#equal) | Tests whether two sequences have equal lengths and equivalent elements. |
| [`mismatch`](#mismatch) | Finds the first position at which two sequences differ. |
| [`lexicographical_compare`](#lexicographical_compare) | Computes the lexicographical three-way order of two sequences. |
| [`is_permutation`](#is_permutation) | Tests whether two sequences contain the same elements with the same multiplicities. |

**Traversal and folds**

| Function | Description |
| --- | --- |
| [`for_each`](#for_each) | Invokes a callable for each element. |
| [`fold_left`](#fold_left) | Folds the elements from the first to the last. |
| [`fold_right`](#fold_right) | Folds the elements from the last to the first. |

## Quantifiers and counting

### `all_of`

```quxlang
// (1) iterator form
::all_of FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::all_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to each projected element in traversal order
and stops at the first element for which the result is `FALSE`.

**Return value.** The result type is `BOOL`. The result is `TRUE` when
`predicate` returns `TRUE` for every projected element or when the sequence is
empty. The result is `FALSE` otherwise.

**Complexity.** The function performs at most `N` applications of `predicate`
and of `projection`.

### `any_of`

```quxlang
// (1) iterator form
::any_of FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::any_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to each projected element in traversal order
and stops at the first element for which the result is `TRUE`.

**Return value.** The result type is `BOOL`. The result is `TRUE` when
`predicate` returns `TRUE` for at least one projected element. The result is
`FALSE` otherwise, including when the sequence is empty.

**Complexity.** The function performs at most `N` applications of `predicate`
and of `projection`.

### `none_of`

```quxlang
// (1) iterator form
::none_of FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::none_of FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to each projected element in traversal order
and stops at the first element for which the result is `TRUE`.

**Return value.** The result type is `BOOL`. The result is `TRUE` when
`predicate` returns `FALSE` for every projected element or when the sequence is
empty. The result is `FALSE` otherwise.

**Complexity.** The function performs at most `N` applications of `predicate`
and of `projection`.

### `count`

```quxlang
// (1) iterator form
::count FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::count FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function evaluates `equal(@left projection(element), @right value)` for
every element. The lookup value `value` is not projected.

**Return value.** The result type is `SZ`. The result is the number of elements
for which the evaluation returns `TRUE`.

**Complexity.** The function performs exactly `N` applications of `equal` and
of `projection`.

### `count_if`

```quxlang
// (1) iterator form
::count_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::count_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to every projected element.

**Return value.** The result type is `SZ`. The result is the number of elements
for which `predicate` returns `TRUE`.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`.

## Element search

### `find`

```quxlang
// (1) iterator form
::find FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::find FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function evaluates `equal(@left projection(element), @right value)` for
each element in traversal order and stops at the first element for which the
result is `TRUE`. The lookup value `value` is not projected, so the type of
`value` can differ from the element type.

**Return value.** The result type is `input_iterator`. The result denotes the
first matching element. When no element matches, the result is the iterator
that has been advanced until it compares equal to `last`.

**Complexity.** The function performs at most `N` applications of `equal` and
of `projection`.

### `find_if`

```quxlang
// (1) iterator form
::find_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::find_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to each projected element in traversal order
and stops at the first element for which the result is `TRUE`.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that satisfies `predicate`, or the end position when no element
satisfies `predicate`.

**Complexity.** The function performs at most `N` applications of `predicate`
and of `projection`.

### `find_if_not`

```quxlang
// (1) iterator form
::find_if_not FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::find_if_not FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function applies `predicate` to each projected element in traversal order
and stops at the first element for which the result is `FALSE`.

**Return value.** The result type is `input_iterator`. The result denotes the
first element that does not satisfy `predicate`, or the end position when every
element satisfies `predicate`.

**Complexity.** The function performs at most `N` applications of `predicate`
and of `projection`.

### `find_first_of`

```quxlang
// (1) iterator form
::find_first_of FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::find_first_of FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function examines the elements of the first sequence in traversal order.
For each element, the function traverses the second sequence from `first2` and
evaluates `equal(@left projection1(element1), @right projection2(element2))`.

**Iterator requirements.** The second sequence is traversed once for each
examined element of the first sequence. `iterator2` therefore supports repeated
traversal from a copy of `first2`.

**Return value.** The result type is `iterator1`. The result denotes the first
element of the first sequence that is equivalent to an element of the second
sequence, or the end position of the first sequence when no such element
exists.

**Complexity.** The function performs at most `N * M` applications of `equal`.

### `adjacent_find`

```quxlang
// (1) iterator form
::adjacent_find FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::adjacent_find FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function evaluates `equal(@left projection(a), @right projection(b))` for
each pair of consecutive elements `a` and `b` in traversal order.

**Iterator requirements.** The function retains an iterator to the preceding
element while the successor is examined. The iterator type supports repeated
traversal.

**Return value.** The result type is `input_iterator`. The result denotes the
first element of the first equivalent pair. The result is the end position when
no pair is equivalent or when the sequence contains fewer than two elements.

**Complexity.** The function performs at most `N - 1` applications of `equal`.

### `min_element`

```quxlang
// (1) iterator form
::min_element FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::min_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function retains a candidate that initially denotes the first element. A
later element replaces the candidate only when
`compare(@left projection(element), @right projection(candidate))` returns
`ORDER::LESS`.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result denotes the
first element among the minimum elements. The result is `first` when the
sequence is empty.

**Complexity.** The function performs exactly `N - 1` applications of `compare`
for a nonempty sequence.

### `max_element`

```quxlang
// (1) iterator form
::max_element FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::max_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function retains a candidate that initially denotes the first element. A
later element replaces the candidate only when
`compare(@left projection(element), @right projection(candidate))` returns
`ORDER::GREATER`.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result denotes the
first element among the maximum elements. The result is `first` when the
sequence is empty.

**Complexity.** The function performs exactly `N - 1` applications of `compare`
for a nonempty sequence.

### `minmax_element`

```quxlang
// (1) iterator form
::minmax_element FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::minmax_element FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function determines the minimum and the maximum in a single traversal. The
replacement rules are those of [`min_element`](#min_element) and
[`max_element`](#max_element).

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result is a composite with the fields `min` and `max`,
both of type `input_iterator`. `min` denotes the first minimum element and
`max` denotes the first maximum element. Both fields equal `first` when the
sequence is empty.

**Complexity.** The function performs exactly `2 * (N - 1)` applications of
`compare` for a nonempty sequence.

## Subsequence search

### `search`

```quxlang
// (1) iterator form
::search FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::search FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function finds the first position in the first sequence at which the
complete second sequence occurs. Two elements match when
`equal(@left projection1(element1), @right projection2(element2))` returns
`TRUE`.

**Iterator requirements.** Both iterator types support repeated traversal.

**Return value.** The result is a
[`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose
iterator type and sentinel type are both `iterator1`. `BEGIN()` denotes the
first element of the match and `END()` denotes the position after the last
element of the match. When the second sequence is empty, the result is the
empty range at `first1`. When no match exists, the result is the empty range at
the end position of the first sequence.

**Complexity.** The function performs at most `N * M` applications of `equal`.

### `find_end`

```quxlang
// (1) iterator form
::find_end FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::find_end FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function finds the last position in the first sequence at which the
complete second sequence occurs. The matching rule is that of
[`search`](#search).

**Iterator requirements.** Both iterator types support repeated traversal.

**Return value.** The result is a
[`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose
iterator type and sentinel type are both `iterator1`. `BEGIN()` and `END()`
bound the last match. When the second sequence is empty or no match exists, the
result is the empty range at the end position of the first sequence.

**Complexity.** The function tests every starting position of the first
sequence and performs at most `N * M` applications of `equal`.

### `search_n`

```quxlang
// (1) iterator form
::search_n FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::search_n FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@count` | `SZ` | Required. |
| `@value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function finds the first run of `count` consecutive elements for which
`equal(@left projection(element), @right value)` returns `TRUE`. The lookup
value `value` is not projected.

**Iterator requirements.** The iterator type supports repeated traversal,
because the function retains the first position of the current run.

**Return value.** The result is a
[`std::iterator_range`](algorithms-iterators.md#stditerator_range) whose
iterator type and sentinel type are both `input_iterator`. `BEGIN()` and
`END()` bound the run. When `count` is zero, the result is the empty range at
`first`. When no run exists, the result is the empty range at the end position.

**Complexity.** The function performs at most `N` applications of `equal` and
of `projection`.

## Sequence comparison

### `equal`

```quxlang
// (1) iterator form
::equal FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::equal FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function traverses both sequences together and evaluates
`equal(@left projection1(element1), @right projection2(element2))` for each
pair of corresponding elements. The traversal stops at the first pair for which
the result is `FALSE` or when either sequence ends.

**Return value.** The result type is `BOOL`. The result is `TRUE` when every
compared pair is equivalent and both sequences end at the same time. Sequences
of different lengths are unequal.

**Complexity.** The function performs at most `min(N, M)` applications of the
`equal` argument.

### `mismatch`

```quxlang
// (1) iterator form
::mismatch FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::mismatch FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function traverses both sequences together and stops at the first pair of
corresponding elements for which
`equal(@left projection1(element1), @right projection2(element2))` returns
`FALSE`, or when either sequence ends.

**Return value.** The result is a composite with the fields `in1` of type
`iterator1` and `in2` of type `iterator2`. The fields denote the positions at
which the traversal stopped.

**Complexity.** The function performs at most `min(N, M)` applications of
`equal`.

### `lexicographical_compare`

```quxlang
// (1) iterator form
::lexicographical_compare FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::lexicographical_compare FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function traverses both sequences together and evaluates
`compare(@left projection1(element1), @right projection2(element2))` for each
pair of corresponding elements.

**Return value.** The result type is `ORDER`. The result is the first
comparison result that differs from `ORDER::EQUAL`. When every compared pair is
equal, the result is `ORDER::GREATER` if the first sequence has remaining
elements, `ORDER::LESS` if the second sequence has remaining elements, and
`ORDER::EQUAL` if both sequences end at the same time.

**Complexity.** The function performs at most `min(N, M)` applications of
`compare`.

### `is_permutation`

```quxlang
// (1) iterator form
::is_permutation FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @KWARGS:options ...): AUTO
// (2) range form
::is_permutation FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection1` | `TT(projection1_type)` | Optional; the default is a `std::identity` object. |
| `@projection2` | `TT(projection2_type)` | Optional; the default is a `std::identity` object. |

The function tests whether each equivalence class of elements occurs the same
number of times in both sequences. The function first counts the elements of
both sequences. For each element of the first sequence that is not equivalent
to an earlier element of the first sequence, the function then counts the
equivalent elements in the remainder of the first sequence and in the complete
second sequence.

Elements of the first sequence are compared with each other through
`equal(@left projection1(a), @right projection1(b))`. An element of the first
sequence is compared with an element of the second sequence through
`equal(@left projection1(a), @right projection2(b))`.

**Iterator requirements.** Both iterator types support repeated traversal.

**Return value.** The result type is `BOOL`. The result is `FALSE` when the
lengths differ or when any multiplicity differs. The result is `TRUE`
otherwise.

**Complexity.** The function performs a number of applications of `equal` that
is quadratic in `N` in the worst case. The function uses a constant amount of
additional storage.

## Traversal and folds

### `for_each`

```quxlang
// (1) iterator form
::for_each FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @action TT(action_type))
// (2) range form
::for_each FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

The function invokes `action(element)` once for each element in traversal
order. The argument of `action` is the result of dereferencing the iterator.
An `action` parameter of a mutable reference type therefore permits
modification of each element. The function accepts no projection.

The range form accepts the named argument `@action`.

**Return value.** The function returns no value. A result of `action` is
discarded.

**Complexity.** The function performs exactly `N` invocations of `action`.

### `fold_left`

```quxlang
// (1) iterator form
::fold_left FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @initial AUTO(accumulator_type), @operation TT(operation_type)): accumulator_type
// (2) range form
::fold_left FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function owns an accumulator that is initialized from `initial`. For each
element in traversal order, the function evaluates
`operation(@accumulator MOVE(accumulator), @value element)` and assigns the
result to the accumulator. The accumulator type `accumulator_type` is deduced
from the `initial` argument.

The range form accepts the named arguments `@initial` and `@operation`.

**Return value.** The result type is `accumulator_type`. The result is the
final accumulator. The result is `initial` when the sequence is empty.

**Complexity.** The function performs exactly `N` invocations of `operation`.

### `fold_right`

```quxlang
// (1) iterator form
::fold_right FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @initial AUTO(accumulator_type), @operation TT(operation_type)): accumulator_type
// (2) range form
::fold_right FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function owns an accumulator that is initialized from `initial`. The
function decrements `last` until `last` equals `first`. After each decrement,
the function evaluates
`operation(@accumulator MOVE(accumulator), @value element)` for the element at
`last` and assigns the result to the accumulator. The elements are therefore
consumed from the last to the first. The argument names of `operation` are
identical to those of [`fold_left`](#fold_left).

The range form accepts the named arguments `@initial` and `@operation`.

**Iterator requirements.** `first` and `last` have the same type, and that type
supports `--`.

**Return value.** The result type is `accumulator_type`. The result is the
final accumulator. The result is `initial` when the sequence is empty.

**Complexity.** The function performs exactly `N` invocations of `operation`.

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_search_queries/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_search_queries/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Range forms](algorithms-ranges.md)
- [Iterator utilities](algorithms-iterators.md)
- [Ordering algorithms](algorithms-ordering.md)
- [Comparison operators](../../reference/comparison-operators.md)
- [Lambdas](../../reference/lambdas.md)
- [Call arguments](../../reference/call-arguments.md)
