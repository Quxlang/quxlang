# Buffered algorithms

Module `std` · source file `algorithm_buffered.qxs`

The functions on this page reorder a sequence stably and require temporary
storage for elements. The functions do not allocate memory. The caller supplies
uninitialized storage through a
[`std::scratch_ref`](algorithms-iterators.md#stdscratch_ref) object, and the
functions construct and destroy temporary element objects in that storage. The
shared sequence model and the argument-passing conventions are specified in
[Algorithms](algorithms.md). The unbuffered counterparts are
[`partition`](algorithms-transfer.md#partition) and
[`sort`](algorithms-ordering.md#sort).

## Conventions

Every function has an iterator form and a range form. The iterator form is
declared in `algorithm_buffered.qxs`. The range form is declared in
`algorithm_ranges.qxs`; it evaluates `range.BEGIN()` and `range.END()` once
each and calls the iterator form with the results, as specified in
[Range forms](algorithms-ranges.md). Every argument is a named argument.

A parameter list that ends in `@KWARGS:options ...` accepts the named arguments
listed in the table that follows the declarations. `N` is the number of
elements in [`first`, `last`).

The callable arguments have the following contracts.

| Argument | Invocation | Result type |
| --- | --- | --- |
| `@predicate` | `predicate(projection(element))` | `BOOL` |
| `@compare` | `compare(@left a, @right b)` with projected elements as operands | `ORDER` |
| `@projection` | `projection(element)` | Any type accepted by the consuming callable |

## Scratch storage

The argument `@scratch` is a `std::scratch_ref#T` object, where `T` is the
element type of the sequence. The object borrows `size()` consecutive
`TYPED_STORAGE(T)` slots that begin at `data()`. The following example provides
six slots for elements of type `reading`.

```quxlang
VAR storage [6]TYPED_STORAGE(reading);
VAR scratch std::scratch_ref#reading :(@storage storage[&0], @count 6);
```

The functions use the scratch storage as follows.

- An element is transferred into a slot by move construction with `PLACE AT`
  and transferred back by move assignment. The element type therefore requires
  a move constructor and a move assignment operator; a copy operation is not
  required.
- Every object that a function constructs in the scratch storage is destroyed
  before the function returns or propagates an exception. The slots contain no
  live objects after the call, and one `scratch_ref` object can serve several
  consecutive calls.
- The functions obtain the element type from `scratch_type::element_type` and
  require `first->` to produce a mutable reference to that type.

**Preconditions.** The slots contain no live objects on entry, remain alive for
the duration of the call, and are disjoint from the elements of the sequence.
The storage provides at least the number of slots that the function section
states. The functions do not read `size()`; a call with fewer slots than
required is undefined behavior. A `scratch_ref` object with a null storage
pointer and a count of zero is valid for a call that requires zero slots.

**Exceptions.** The functions contain no exception handlers. An exception
thrown by a callable, a move constructor, or a move assignment operator
propagates to the caller after the objects in the scratch storage have been
destroyed. Elements that had been moved into the scratch storage and not yet
moved back are lost: the corresponding sequence positions hold moved-from
objects. Every sequence position continues to hold a live object.

## Function summary

| Function | Description |
| --- | --- |
| [`stable_partition`](#stable_partition) | Partitions a sequence by a predicate and preserves the relative order within both groups. |
| [`inplace_merge`](#inplace_merge) | Merges two adjacent sorted subsequences and preserves the relative order of equal elements. |
| [`stable_sort`](#stable_sort) | Sorts a sequence and preserves the relative order of equal elements. |

## Functions

### `stable_partition`

```quxlang
// (1) iterator form
::stable_partition FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::stable_partition FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@scratch` | `CONST& AUTO(scratch_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function reorders the sequence so that every element for which
`predicate(projection(element))` returns `TRUE` precedes every element for
which the result is `FALSE`. The relative order of the elements within each
group is preserved.

The function first counts the elements. The function then moves every element
into the scratch storage: an element of the `TRUE` group is placed in the next
slot from the beginning, and an element of the `FALSE` group is placed in the
next slot from position `N - 1` downward. Finally the function moves the `TRUE`
group and then the `FALSE` group back into the sequence.

**Iterator requirements.** The iterator type supports repeated traversal; the
sequence is traversed three times. The sentinel type of `last` can differ from
the iterator type.

**Preconditions.** The scratch storage provides at least `N` slots.

**Return value.** The result type is `input_iterator`. The result denotes the
first element of the `FALSE` group, or the end position when that group is
empty.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`, `N` move constructions, and `N` move assignments.

### `inplace_merge`

```quxlang
// (1) iterator form
::inplace_merge FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::inplace_merge FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@middle` | `AUTO(input_iterator)` | Required. |
| `@scratch` | `CONST& AUTO(scratch_type)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function merges the adjacent sorted subsequences [`first`, `middle`) and
[`middle`, `last`) into one sorted sequence [`first`, `last`). When two elements
compare equal, the element of the first subsequence precedes the element of the
second subsequence. The relative order of the elements of each subsequence is
preserved.

The function counts both subsequences and returns without effect when either
is empty. Otherwise the function moves the shorter subsequence into the scratch
storage. When the first subsequence is not longer than the second, the function
merges forward from `first`. Otherwise the function merges backward from
`last`.

**Iterator requirements.** `first`, `last`, and `middle` have the same type.
That type supports repeated traversal and `--`.

**Preconditions.** `middle` denotes a position in [`first`, `last`]. Both
subsequences are sorted with respect to `compare` and `projection`. The scratch
storage provides at least `min(middle - first, last - middle)` slots.

**Return value.** The function returns no value.

**Complexity.** When both subsequences are nonempty, the function performs at
most `N - 1` applications of `compare`, a number of move constructions equal to
the length of the shorter subsequence, and at most `N` move assignments.

### `stable_sort`

```quxlang
// (1) iterator form
::stable_sort FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @KWARGS:options ...)
// (2) range form
::stable_sort FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@scratch` | `CONST& AUTO(scratch_type)` | Required. |
| `@compare` | `TT(compare_type)` | Optional; the default is a `std::compare_three_way` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function sorts the sequence. The relative order of elements that compare
equal is preserved.

The implementation is a top-down merge sort. A sequence of fewer than two
elements is unchanged. A longer sequence is divided at the position
`first + N / 2`; the function sorts both halves recursively with the same
scratch storage and merges them with [`inplace_merge`](#inplace_merge).

**Iterator requirements.** `first` and `last` have the same type. That type
supports `last - first` with a signed element distance as the result,
`first + count` for a `count` of type `SZ`, repeated traversal, and `--`.

**Preconditions.** The scratch storage provides at least `N` slots. `last` is
reachable from `first`; the function converts `last - first` to `SZ` with
`AS ASSUME`, and a negative distance is undefined behavior.

**Return value.** The function returns no value.

**Complexity.** The function performs `O(N log N)` applications of `compare`.
The recursion depth is logarithmic in `N`.

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_buffered/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_buffered/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Range forms](algorithms-ranges.md)
- [Iterator utilities](algorithms-iterators.md)
- [Ordering algorithms](algorithms-ordering.md)
- [Transfer and modification algorithms](algorithms-transfer.md)
- [Typed storage and lifetime](../../reference/typed-storage-and-lifetime.md)
- [Move semantics](../../reference/move-semantics.md)
- [Exceptions](../../reference/exceptions.md)
