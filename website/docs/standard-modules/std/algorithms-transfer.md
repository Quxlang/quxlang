# Transfer and modification algorithms

Module `std` · source file `algorithm_transfer.qxs`

The functions on this page write elements. The page covers copying and moving
between sequences, transformation, filling, replacement, removal of selected
elements, reordering by reversal and rotation, and partitioning. The shared
sequence model and the argument-passing conventions are specified in
[Algorithms](algorithms.md). The stable partition that requires scratch storage
is specified in [Buffered algorithms](algorithms-buffered.md).

## Conventions

Every function has an iterator form and a range form. The iterator form is
declared in `algorithm_transfer.qxs`. The range form is declared in
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
- A declared return type of `AUTO` is deduced from the implementation. Each
  section states the resulting type.

The callable arguments have the following contracts.

| Argument | Invocation | Result type |
| --- | --- | --- |
| `@predicate` | `predicate(projection(element))` | `BOOL` |
| `@equal` | `equal(@left a, @right b)` | `BOOL` |
| `@projection` | `projection(element)` | Any type accepted by the consuming callable |
| `@operation` (unary [`transform`](#transform)) | `operation(element)` | A type assignable to an output position |
| `@operation` (binary [`transform`](#transform)) | `operation(@left a, @right b)` | A type assignable to an output position |
| `@generator` | `generator()` | A type assignable to an element |

A projection selects the value that is tested. The element that is copied,
moved, or replaced is always the unprojected element.

**Output iterators.** An output iterator supports `output-> := value` and
`output++`. A pointer into an array, a container iterator, and a
[`std::back_insert_iterator`](algorithms-iterators.md#stdback_insert_iterator)
are output iterators. A function that writes through an ordinary iterator
requires the destination sequence to contain a live element at every position
that is written. A `std::back_insert_iterator` appends a new element for every
assignment and has no such requirement.

**Result composites.** A function that advances several cursors returns a
composite whose fields hold the final cursor values. The field `in` (or `in1`,
`in2`) is a final input position and the field `out` (or `out_true`,
`out_false`) is the position after the last element written. The fields own
their iterator values.

**Iterator requirements.** Unless a section states a stronger requirement, an
input iterator supports `first != last`, `first++`, and `first->`, and the
sentinel type of `last` can differ from the iterator type. A function that
modifies the input sequence requires `first->` to produce a mutable reference
to the element.

**Preconditions.** `last` is reachable from `first` by repeated `++`. Each
iterator in [`first`, `last`) is dereferenceable. Each written output position
is valid.

**Exceptions.** The functions contain no exception handlers. An exception
thrown by a callable, an iterator operation, or an element operation propagates
to the caller. Elements that were assigned before the exception retain their
new values.

## Function summary

**Copying and moving**

| Function | Description |
| --- | --- |
| [`copy`](#copy) | Copies the elements to a destination in traversal order. |
| [`move`](#move) | Moves the elements to a destination in traversal order. |
| [`copy_backward`](#copy_backward) | Copies the elements to a destination in reverse traversal order. |
| [`move_backward`](#move_backward) | Moves the elements to a destination in reverse traversal order. |
| [`copy_if`](#copy_if) | Copies the elements that satisfy a predicate. |
| [`remove_copy`](#remove_copy) | Copies the elements that are not equivalent to a value. |
| [`remove_copy_if`](#remove_copy_if) | Copies the elements that do not satisfy a predicate. |
| [`unique_copy`](#unique_copy) | Copies the first element of each run of equivalent adjacent elements. |
| [`reverse_copy`](#reverse_copy) | Copies the elements in reverse order. |
| [`rotate_copy`](#rotate_copy) | Copies a rotated arrangement of the elements. |
| [`partition_copy`](#partition_copy) | Copies the elements to one of two destinations according to a predicate. |

**Transformation and assignment**

| Function | Description |
| --- | --- |
| [`transform`](#transform) | Writes the result of a unary or binary operation for each element. |
| [`fill`](#fill) | Assigns one value to every element. |
| [`generate`](#generate) | Assigns successive generator results to the elements. |
| [`replace`](#replace) | Replaces the elements equivalent to a value. |
| [`replace_if`](#replace_if) | Replaces the elements that satisfy a predicate. |
| [`replace_copy`](#replace_copy) | Copies the elements and substitutes those equivalent to a value. |
| [`replace_copy_if`](#replace_copy_if) | Copies the elements and substitutes those that satisfy a predicate. |
| [`swap_ranges`](#swap_ranges) | Exchanges the corresponding elements of two sequences. |

**Removal**

| Function | Description |
| --- | --- |
| [`remove`](#remove) | Compacts the elements that are not equivalent to a value. |
| [`remove_if`](#remove_if) | Compacts the elements that do not satisfy a predicate. |
| [`unique`](#unique) | Compacts the first element of each run of equivalent adjacent elements. |

**Reordering and partitioning**

| Function | Description |
| --- | --- |
| [`reverse`](#reverse) | Reverses the order of the elements. |
| [`rotate`](#rotate) | Exchanges two adjacent subsequences. |
| [`partition`](#partition) | Places the elements that satisfy a predicate before the other elements. |
| [`is_partitioned`](#is_partitioned) | Tests whether a sequence is partitioned by a predicate. |

## Copying and moving

### `copy`

```quxlang
// (1) iterator form
::copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @output AUTO(output_iterator)): AUTO
// (2) range form
::copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function assigns each element of the input sequence to the output position
`output-> := element` in traversal order and advances both cursors after each
assignment.

When `input_iterator` and `output_iterator` are the same type and `first`
equals `output`, the function performs no assignment and only advances both
cursors to the end. A copy of a sequence onto itself is therefore a no-op.

The assignments are performed in ascending order from `first`. When `output`
denotes a position inside (`first`, `last`), later input elements are
overwritten before they are read. [`copy_backward`](#copy_backward) performs
the assignments in the opposite order.

The range form accepts the named argument `@output`.

**Iterator requirements.** When `input_iterator` and `output_iterator` are the
same type, that type supports `first == output`.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` assignments, or zero
assignments for a copy of a sequence onto itself.

### `move`

```quxlang
// (1) iterator form
::move FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @output AUTO(output_iterator)): AUTO
// (2) range form
::move FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function move-assigns each element of the input sequence to the output
position in traversal order: the function binds `element` to the result of
`first->` and evaluates `output-> := MOVE(element)`. When `first->` produces a
mutable reference, each input element is left in a moved-from state.

When `input_iterator` and `output_iterator` are the same type and `first`
equals `output`, the function performs no assignment and only advances both
cursors to the end.

The range form accepts the named argument `@output`.

**Iterator requirements.** When `input_iterator` and `output_iterator` are the
same type, that type supports `first == output`.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` move assignments, or zero
move assignments for a move of a sequence onto itself.

### `copy_backward`

```quxlang
// (1) iterator form
::copy_backward FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @output_last AUTO(output_iterator)): AUTO
// (2) range form
::copy_backward FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function copies the elements in reverse traversal order. The function
decrements `last` and `output_last` together and assigns
`output_last-> := last->` until `last` equals `first`. The destination sequence
ends at the initial value of `output_last`, and the relative order of the
elements is preserved. A destination that ends after the end of the input and
overlaps the input is therefore written without overwriting unread elements.

When `input_iterator` and `output_iterator` are the same type and `last` equals
`output_last`, the function performs no assignment and only decrements both
cursors.

The range form accepts the named argument `@output_last`.

**Iterator requirements.** `first` and `last` have the same type. The input
iterator type and the output iterator type support `--`.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` equals
`first`. `out` denotes the first element of the destination sequence.

**Complexity.** The function performs exactly `N` assignments, or zero
assignments for a copy of a sequence onto itself.

### `move_backward`

```quxlang
// (1) iterator form
::move_backward FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @output_last AUTO(output_iterator)): AUTO
// (2) range form
::move_backward FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function move-assigns the elements in reverse traversal order. The
traversal and the self-transfer rule are those of
[`copy_backward`](#copy_backward). Each assignment binds `element` to the
result of `last->` and evaluates `output_last-> := MOVE(element)`.

The range form accepts the named argument `@output_last`.

**Iterator requirements.** `first` and `last` have the same type. The input
iterator type and the output iterator type support `--`.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` equals
`first`. `out` denotes the first element of the destination sequence.

**Complexity.** The function performs exactly `N` move assignments, or zero
move assignments for a move of a sequence onto itself.

### `copy_if`

```quxlang
// (1) iterator form
::copy_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies each element for which `predicate(projection(element))`
returns `TRUE`. The copied elements retain their relative order.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`.

### `remove_copy`

```quxlang
// (1) iterator form
::remove_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::remove_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies each element for which
`equal(@left projection(element), @right value)` returns `FALSE`. The copied
elements retain their relative order. The input sequence is not modified.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` applications of `equal` and
of `projection`.

### `remove_copy_if`

```quxlang
// (1) iterator form
::remove_copy_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::remove_copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies each element for which `predicate(projection(element))`
returns `FALSE`. The copied elements retain their relative order. The input
sequence is not modified.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`.

### `unique_copy`

```quxlang
// (1) iterator form
::unique_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::unique_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies the first element of each run of adjacent elements that are
equivalent under `equal(@left projection(a), @right projection(b))`. The
function retains an iterator to the most recently copied input element and
compares each later element with that element.

**Iterator requirements.** The input iterator type supports repeated traversal,
because the retained iterator is dereferenced after the input cursor has
advanced.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N - 1` applications of `equal`
for a nonempty sequence.

### `reverse_copy`

```quxlang
// (1) iterator form
::reverse_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @output AUTO(output_iterator)): AUTO
// (2) range form
::reverse_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function copies the elements from the last to the first. The input sequence
is not modified.

The range form accepts the named argument `@output`.

**Iterator requirements.** `first` and `last` have the same type, and that type
supports `--`.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` equals
`last` and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` assignments.

### `rotate_copy`

```quxlang
// (1) iterator form
::rotate_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @middle AUTO(input_iterator), @output AUTO(output_iterator)): AUTO
// (2) range form
::rotate_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function copies the elements of [`middle`, `last`) followed by the elements
of [`first`, `middle`). The input sequence is not modified.

The range form accepts the named arguments `@middle` and `@output`.

**Iterator requirements.** `first`, `last`, and `middle` have the same type,
and that type supports repeated traversal.

**Preconditions.** `middle` denotes a position in [`first`, `last`].

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` equals
`last` and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` assignments.

### `partition_copy`

```quxlang
// (1) iterator form
::partition_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::partition_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output_true` | `AUTO(true_iterator)` | Required. |
| `@output_false` | `AUTO(false_iterator)` | Required. |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function copies each element for which `predicate(projection(element))`
returns `TRUE` to `output_true` and every other element to `output_false`. Each
destination receives its elements in their input order.

**Return value.** The result is a composite with the field `in` of type
`input_iterator`, the field `out_true` of type `true_iterator`, and the field
`out_false` of type `false_iterator`. `in` is the end position of the input.
`out_true` and `out_false` are the positions after the last element written to
the respective destination.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`.

## Transformation and assignment

### `transform`

```quxlang
// (1) iterator form
::transform FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @output AUTO(output_iterator), @operation TT(operation_type)): AUTO
// (2) iterator form
::transform FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2), @output AUTO(output_iterator), @operation TT(operation_type)): AUTO
// (3) range form
::transform FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
// (4) range form
::transform FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

1. The unary iterator form assigns `operation(element)` to the output position
   for each element of the input sequence.
2. The binary iterator form traverses both input sequences together and assigns
   `operation(@left element1, @right element2)` to the output position for each
   pair of corresponding elements. The traversal stops when either input
   sequence ends.
3. The unary range form accepts the named arguments `@output` and `@operation`.
4. The binary range form accepts the named arguments `@output` and
   `@operation`.

The functions accept no projection.

**Return value.** The unary forms return a composite with the field `in` of
type `input_iterator` and the field `out` of type `output_iterator`. The binary
forms return a composite with the fields `in1` of type `iterator1`, `in2` of
type `iterator2`, and `out` of type `output_iterator`. The input fields denote
the positions at which the traversal stopped, and `out` is the position after
the last element written.

**Complexity.** The unary forms perform exactly `N` invocations of `operation`.
The binary forms perform exactly `min(N, M)` invocations of `operation`.

### `fill`

```quxlang
// (1) iterator form
::fill FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @value CONST& AUTO)
// (2) range form
::fill FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

The function assigns `value` to every element of the sequence.

The range form accepts the named argument `@value`.

**Return value.** The function returns no value.

**Complexity.** The function performs exactly `N` assignments.

### `generate`

```quxlang
// (1) iterator form
::generate FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @generator TT(generator_type))
// (2) range form
::generate FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

The function assigns the result of `generator()` to each element in traversal
order. The function invokes `generator` once for each element.

The range form accepts the named argument `@generator`.

**Return value.** The function returns no value.

**Complexity.** The function performs exactly `N` invocations of `generator`.

### `replace`

```quxlang
// (1) iterator form
::replace FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...)
// (2) range form
::replace FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@old_value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@new_value` | `CONST& AUTO` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function assigns `new_value` to each element for which
`equal(@left projection(element), @right old_value)` returns `TRUE`. The
complete element is replaced; the projection selects only the compared value.

**Return value.** The function returns no value.

**Complexity.** The function performs exactly `N` applications of `equal` and
of `projection`.

### `replace_if`

```quxlang
// (1) iterator form
::replace_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...)
// (2) range form
::replace_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@new_value` | `CONST& AUTO` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function assigns `new_value` to each element for which
`predicate(projection(element))` returns `TRUE`.

**Return value.** The function returns no value.

**Complexity.** The function performs exactly `N` applications of `predicate`
and of `projection`.

### `replace_copy`

```quxlang
// (1) iterator form
::replace_copy FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::replace_copy FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@old_value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@new_value` | `CONST& AUTO` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function writes one output element for each input element. The output
element is `new_value` when `equal(@left projection(element), @right old_value)`
returns `TRUE`, and a copy of the input element otherwise. The input sequence
is not modified.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` applications of `equal` and
exactly `N` assignments.

### `replace_copy_if`

```quxlang
// (1) iterator form
::replace_copy_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::replace_copy_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@output` | `AUTO(output_iterator)` | Required. |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@new_value` | `CONST& AUTO` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function writes one output element for each input element. The output
element is `new_value` when `predicate(projection(element))` returns `TRUE`,
and a copy of the input element otherwise. The input sequence is not modified.

**Return value.** The result is a composite with the field `in` of type
`input_iterator` and the field `out` of type `output_iterator`. `in` is the end
position of the input and `out` is the position after the last element written.

**Complexity.** The function performs exactly `N` applications of `predicate`
and exactly `N` assignments.

### `swap_ranges`

```quxlang
// (1) iterator form
::swap_ranges FUNCTION(@first1 AUTO(iterator1), @last1 AUTO(sentinel1), @first2 AUTO(iterator2), @last2 AUTO(sentinel2)): AUTO
// (2) range form
::swap_ranges FUNCTION(@range1 AUTO& AUTO, @range2 AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function traverses both sequences together and exchanges each pair of
corresponding elements with the swap operator `<->`. The traversal stops when
either sequence ends.

**Return value.** The result is a composite with the fields `in1` of type
`iterator1` and `in2` of type `iterator2`. The fields denote the positions at
which the traversal stopped.

**Complexity.** The function performs exactly `min(N, M)` exchanges.

## Removal

The removal functions compact the retained elements toward the beginning of
the sequence and return the end of the retained elements. The functions do not
destroy elements and do not change the size of a container. The elements
between the returned position and `last` remain live objects; an element that
was the source of a move assignment is in a moved-from state.

### `remove`

```quxlang
// (1) iterator form
::remove FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::remove FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@value` | `CONST& AUTO` | Required. |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function retains each element for which
`equal(@left projection(element), @right value)` returns `FALSE`. The function
advances to the first element that is equivalent to `value` and then
move-assigns each later retained element to the next free position. The
retained elements keep their relative order.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result is the
position after the last retained element. The result is the end position when
no element is equivalent to `value`.

**Complexity.** The function performs exactly `N` applications of `equal` and
at most `N - 1` move assignments.

### `remove_if`

```quxlang
// (1) iterator form
::remove_if FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::remove_if FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function retains each element for which `predicate(projection(element))`
returns `FALSE`. The compaction method is that of [`remove`](#remove). The
retained elements keep their relative order.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result is the
position after the last retained element. The result is the end position when
no element satisfies `predicate`.

**Complexity.** The function performs exactly `N` applications of `predicate`
and at most `N - 1` move assignments.

### `unique`

```quxlang
// (1) iterator form
::unique FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::unique FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@equal` | `TT(equal_type)` | Optional; the default is a `std::equal_to` object. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function retains the first element of each run of adjacent elements that
are equivalent under `equal(@left projection(a), @right projection(b))`. Each
element is compared with the most recently retained element. A retained element
that is not already at its final position is move-assigned to that position.
The retained elements keep their relative order.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result is the
position after the last retained element. The result is `first` when the
sequence is empty.

**Complexity.** The function performs exactly `N - 1` applications of `equal`
for a nonempty sequence.

## Reordering and partitioning

### `reverse`

```quxlang
// (1) iterator form
::reverse FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator))
// (2) range form
::reverse FUNCTION(@range AUTO& AUTO, @KWARGS:options ...)
```

The function reverses the order of the elements by exchanging elements from
both ends with the swap operator `<->`.

**Iterator requirements.** `first` and `last` have the same type, and that type
supports `--`.

**Return value.** The function returns no value.

**Complexity.** The function performs exactly `N / 2` exchanges, rounded down.

### `rotate`

```quxlang
// (1) iterator form
::rotate FUNCTION(@first AUTO(input_iterator), @last AUTO(input_iterator), @middle AUTO(input_iterator)): input_iterator
// (2) range form
::rotate FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

The function exchanges the adjacent subsequences [`first`, `middle`) and
[`middle`, `last`). After the call, the elements that were in
[`middle`, `last`) precede the elements that were in [`first`, `middle`), and
both groups keep their internal order. The function uses only exchanges with
the swap operator `<->` and forward traversal.

The range form accepts the named argument `@middle`.

**Iterator requirements.** `first`, `last`, and `middle` have the same type,
and that type supports repeated traversal.

**Preconditions.** `middle` denotes a position in [`first`, `last`].

**Return value.** The result type is `input_iterator`. The result denotes the
new position of the element that was at `first`. The result is `last` when
`first` equals `middle`, and `first` when `middle` equals `last`.

**Complexity.** The number of exchanges is linear in `N`.

### `partition`

```quxlang
// (1) iterator form
::partition FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::partition FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function reorders the sequence so that every element for which
`predicate(projection(element))` returns `TRUE` precedes every element for
which the result is `FALSE`. The function exchanges elements with the swap
operator `<->`. The relative order of the elements within each group is not
preserved. [`stable_partition`](algorithms-buffered.md#stable_partition)
preserves the relative order.

**Iterator requirements.** The iterator type supports repeated traversal.

**Return value.** The result type is `input_iterator`. The result denotes the
first element of the group for which `predicate` returns `FALSE`, or the end
position when that group is empty.

**Complexity.** The function performs exactly `N` applications of `predicate`
and at most `N` exchanges.

### `is_partitioned`

```quxlang
// (1) iterator form
::is_partitioned FUNCTION(@first AUTO(input_iterator), @last AUTO(sentinel_type), @KWARGS:options ...): AUTO
// (2) range form
::is_partitioned FUNCTION(@range AUTO& AUTO, @KWARGS:options ...): AUTO
```

Named arguments captured by `options`:

| Argument | Declared type | Status |
| --- | --- | --- |
| `@predicate` | `TT(predicate_type)` | Required. |
| `@projection` | `TT(projection_type)` | Optional; the default is a `std::identity` object. |

The function tests whether every element for which
`predicate(projection(element))` returns `TRUE` precedes every element for
which the result is `FALSE`.

**Return value.** The result type is `BOOL`. The result is `TRUE` for a
partitioned sequence and for an empty sequence.

**Complexity.** The function performs at most `N + 1` applications of
`predicate`; the first element that does not satisfy `predicate` is tested
twice.

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_transfers/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_transfers/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Range forms](algorithms-ranges.md)
- [Iterator utilities](algorithms-iterators.md)
- [Buffered algorithms](algorithms-buffered.md)
- [`std::dynarr`](dynarr.md)
- [Swap operator](../../reference/swap-operator.md)
- [Move semantics](../../reference/move-semantics.md)
- [Composites](../../reference/composites.md)
