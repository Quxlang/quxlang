# Iterator utilities

Module `std` · source file `algorithm_iterators.qxs`

This page specifies the types and functions that support the
[algorithm functions](algorithms.md) of module `std`: the iterator distance
type, the default callable objects, a borrowed range type, an appending output
iterator, and the descriptor for caller-supplied scratch storage. None of these
entities owns elements or allocates memory.

## Shared requirements

The entities on this page borrow the objects that they refer to. An
[`iterator_range`](#stditerator_range) stores copies of two boundaries and does
not extend the lifetime of the underlying sequence. A
[`back_insert_iterator`](#stdback_insert_iterator) stores a pointer to a
container. A [`scratch_ref`](#stdscratch_ref) stores a pointer to storage. In
each case the referenced object must remain alive, and the stored iterators or
pointers must remain valid, for as long as the borrowing object is used.

The callable types [`identity`](#stdidentity), [`equal_to`](#stdequal_to), and
[`compare_three_way`](#stdcompare_three_way) have no fields and are
default-constructible. The algorithm functions create one object of the
relevant type when a call omits the corresponding named argument.

## Summary

| Entity | Kind | Description |
| --- | --- | --- |
| [`iterator_difference`](#stditerator_difference) | Type alias | Signed element distance between two positions. |
| [`identity`](#stdidentity) | Callable type | Default projection; returns its argument. |
| [`equal_to`](#stdequal_to) | Callable type | Default equality relation; applies `==`. |
| [`compare_three_way`](#stdcompare_three_way) | Callable type | Default ordering relation; applies `<=>`. |
| [`iterator_range`](#stditerator_range) | Type template | Borrowed half-open sequence bounded by an iterator and a sentinel. |
| [`subrange`](#stdsubrange) | Function | Creates an `iterator_range` with deduced template arguments. |
| [`back_insert_iterator`](#stdback_insert_iterator) | Type template | Output iterator that appends to a container. |
| [`back_inserter`](#stdback_inserter) | Function | Creates a `back_insert_iterator` with a deduced container type. |
| [`scratch_ref`](#stdscratch_ref) | Type template | Borrowed uninitialized storage for the buffered algorithms. |

## Distance type

### `std::iterator_difference`

```quxlang
::iterator_difference INCLUDE_IF(BITS(SZ) == 32) ALIAS I32;
::iterator_difference INCLUDE_IF(BITS(SZ) == 64) ALIAS I64;
```

`iterator_difference` is the signed integer type that has the width of `SZ`.
The alias denotes `I32` on a target with a 32-bit `SZ` and `I64` on a target
with a 64-bit `SZ`. Subtraction of two array pointers produces this type, and
the subtraction operators of the random-access container iterators, such as the
iterators of [`std::dynarr`](dynarr.md), return this type.
`std::iterator_difference` is the appropriate result type for the subtraction
operator of a user-defined random-access iterator.

## Callable types

### `std::identity`

```quxlang
::identity STRUCT

.OPERATOR() FUNCTION(@ARG:value AUTO& DECAY(result)) CONST: result
```

`identity` is the default projection of the algorithm functions. The call
operator returns its argument. An argument that is a mutable or constant lvalue
reference is returned as the same reference, so a projected element denotes the
original element. An expiring argument is returned as an owned value.

### `std::equal_to`

```quxlang
::equal_to STRUCT

.OPERATOR() FUNCTION(@left CONST& AUTO, @right CONST& AUTO) CONST: BOOL
```

`equal_to` is the default argument for `@equal`. The call operator returns
`left == right`. The operand types can differ when the operator `==` is
available for the pair of types.

### `std::compare_three_way`

```quxlang
::compare_three_way STRUCT

.OPERATOR() FUNCTION(@left CONST& AUTO, @right CONST& AUTO) CONST: ORDER
```

`compare_three_way` is the default argument for `@compare`. The call operator
returns `left <=> right`. The operand types can differ when the operator `<=>`
is available for the pair of types.

## Borrowed ranges

### `std::iterator_range`

```quxlang
::iterator_range TEMPLATE(@iterator TYPE AUTO, @sentinel TYPE AUTO) STRUCT
```

`iterator_range` represents the half-open sequence [`first`, `last`) by value
copies of its two boundaries. The template parameter `iterator` is the type of
the first boundary and `sentinel` is the type of the exclusive boundary; the
two types can be identical. Both template arguments are named. The stored
boundaries are private fields.

An `iterator_range` object provides `BEGIN()` and `END()` and is therefore
accepted by the `@range` parameter of every
[range form](algorithms-ranges.md). The range forms accept lvalues only: a
range object is stored in a variable before it is passed. The search functions
[`search`](algorithms-search.md#search),
[`find_end`](algorithms-search.md#find_end),
[`search_n`](algorithms-search.md#search_n), and
[`equal_range`](algorithms-ordering.md#equal_range) return `iterator_range`
objects.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#iterator_range-constructor) | Stores the two boundaries. |
| [`BEGIN`](#iterator_range-begin) | Returns the first boundary. |
| [`END`](#iterator_range-end) | Returns the exclusive boundary. |

#### `CONSTRUCTOR` { #iterator_range-constructor }

```quxlang
.CONSTRUCTOR FUNCTION(@first iterator, @last sentinel)
```

The constructor copies `first` and `last` into the range. An explicitly typed
range is declared as follows.

```quxlang
VAR range std::iterator_range#(@iterator MUT=>>I32, @sentinel MUT=>>I32) :(@first values.BEGIN(), @last values.END());
```

**Preconditions.** For use with an algorithm function, `last` is reachable from
`first`.

#### `BEGIN` { #iterator_range-begin }

```quxlang
.BEGIN FUNCTION() CONST: iterator
```

**Return value.** The function returns a copy of the first boundary.

#### `END` { #iterator_range-end }

```quxlang
.END FUNCTION() CONST: sentinel
```

**Return value.** The function returns a copy of the exclusive boundary.

### `std::subrange`

```quxlang
::subrange FUNCTION(@first AUTO(iterator_type), @last AUTO(sentinel_type)): iterator_range#(@iterator iterator_type, @sentinel sentinel_type)
```

`subrange` creates an [`iterator_range`](#stditerator_range) whose template
arguments are deduced from the argument types.

**Parameters.** `first` is the first boundary and `last` is the exclusive
boundary.

**Return value.** The function returns
`iterator_range#(@iterator iterator_type, @sentinel sentinel_type)` constructed
from `first` and `last`.

**Preconditions.** For use with an algorithm function, `last` is reachable from
`first`.

## Output adaptation

### `std::back_insert_iterator`

```quxlang
::back_insert_iterator TEMPLATE(@T TYPE AUTO) STRUCT
```

`back_insert_iterator` is an output iterator that appends to a container. The
template parameter `T` is the container type. Every assignment through the
iterator calls `push_back` on the container with the assigned value, so the
destination grows by one element for each element written. The container type
provides a `push_back` member function that accepts one argument;
[`std::dynarr`](dynarr.md), [`std::list`](list.md), and
[`std::geometric_array`](geometric-array.md) are suitable container types.

The iterator stores a pointer to the container. The type declares no equality
operator, and the increment operator does not change the iterator.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#back_insert_iterator-constructor) | Borrows the destination container. |
| [`OPERATOR:=`](#back_insert_iterator-assignment) | Rebinds the iterator to the destination of another iterator. |
| [`OPERATOR->`](#back_insert_iterator-dereference) | Returns the assignment slot. |
| [`OPERATOR++`](#back_insert_iterator-increment) | Returns an equivalent iterator. |

#### `CONSTRUCTOR` { #back_insert_iterator-constructor }

```quxlang
.CONSTRUCTOR FUNCTION(@container MUT& T)
```

The constructor stores a pointer to `container`. The constructor appends no
element.

#### `OPERATOR:=` { #back_insert_iterator-assignment }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& back_insert_iterator#T) MUT
```

The operator copies the destination of `OTHER`. After the assignment, both
iterators append to the same container. The operator appends no element.

#### `OPERATOR->` { #back_insert_iterator-dereference }

```quxlang
.OPERATOR-> FUNCTION() MUT: MUT& slot    // (1)
.OPERATOR-> FUNCTION() TEMP: MUT& slot   // (2)
```

1. The operator returns the assignment slot of a mutable iterator.
2. The operator returns the assignment slot of a temporary iterator, such as
   the result of a postfix increment.

`slot` is a private member type. An assignment `iterator-> := value` to the
slot calls `push_back` on the borrowed container and forwards `value` with its
access qualification: an lvalue is passed as an lvalue, and an expiring value
is passed as an expiring value. The slot accepts a value of any type that the
container's `push_back` accepts.

**Exceptions.** An exception thrown by `push_back` propagates to the caller.

#### `OPERATOR++` { #back_insert_iterator-increment }

```quxlang
.OPERATOR++ FUNCTION() MUT: back_insert_iterator#T
```

**Return value.** The operator returns a copy of the iterator. The iterator is
unchanged.

### `std::back_inserter`

```quxlang
::back_inserter FUNCTION(@ARG:container MUT& AUTO(container_type)): back_insert_iterator#container_type
```

`back_inserter` creates a
[`back_insert_iterator`](#stdback_insert_iterator) whose container type is
deduced from the argument. The function has a single parameter and is called
with one positional argument: `std::back_inserter(values)`.

**Parameters.** `container` is a mutable lvalue that denotes the destination
container.

**Return value.** The function returns
`back_insert_iterator#container_type` bound to `container`.

## Scratch storage

### `std::scratch_ref`

```quxlang
::scratch_ref TEMPLATE(@T TYPE AUTO) STRUCT
```

`scratch_ref` describes `count` consecutive uninitialized slots of type
`TYPED_STORAGE(T)`. The [buffered algorithms](algorithms-buffered.md) receive a
`scratch_ref` through the named argument `@scratch` and use the slots as
temporary storage for elements of type `T`. The stored pointer and count are
private fields.

The slots contain no live `T` objects on entry to an algorithm. The slots must
remain alive for the duration of the call and must be disjoint from the
elements that the algorithm processes.

| Member | Description |
| --- | --- |
| [`element_type`](#scratch_ref-element_type) | Names the element type `T`. |
| [`CONSTRUCTOR`](#scratch_ref-constructor) | Borrows the storage. |
| [`data`](#scratch_ref-data) | Returns the storage pointer. |
| [`size`](#scratch_ref-size) | Returns the number of slots. |

#### `element_type` { #scratch_ref-element_type }

```quxlang
::element_type ALIAS T;
```

`element_type` names the payload type that each borrowed slot is authorized to
hold.

#### `CONSTRUCTOR` { #scratch_ref-constructor }

```quxlang
.CONSTRUCTOR FUNCTION(@storage MUT=>>TYPED_STORAGE(T), @count SZ)
```

The constructor stores `storage` and `count`. The constructor starts no object
lifetime. A null `storage` pointer with a `count` of zero describes empty
scratch storage.

**Preconditions.** `storage` denotes the first of at least `count` consecutive
slots.

#### `data` { #scratch_ref-data }

```quxlang
.data FUNCTION() CONST: MUT=>>TYPED_STORAGE(T)
```

**Return value.** The function returns the borrowed storage pointer.

#### `size` { #scratch_ref-size }

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value.** The function returns the number of available slots.

## Example

```quxlang
--8<-- "example-programs/reference-programs/algorithm_iterator_utilities/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/algorithm_iterator_utilities/expected-output.txt"
```

## See also

- [Algorithms](algorithms.md)
- [Range forms](algorithms-ranges.md)
- [Buffered algorithms](algorithms-buffered.md)
- [`std::dynarr`](dynarr.md)
- [`std::list`](list.md)
- [`std::geometric_array`](geometric-array.md)
- [Typed storage and lifetime](../../reference/typed-storage-and-lifetime.md)
- [Type queries and deduction](../../reference/type-queries-and-deduction.md)
- [Pointers](../../reference/pointers.md)
