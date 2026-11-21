# `std::dynarr`

Module `std` · source file `dynarr.qxs`

`std::dynarr` owns a contiguous sequence of `T` objects in one allocation
obtained from the default allocator of the runtime module. The array grows
geometrically, preserves element order, and destroys every removed element
exactly once. Growth relocates the elements, so a reallocation invalidates
every pointer, reference, and iterator into the array.
[`std::geometric_array`](geometric-array.md) provides indexed storage whose
elements keep their addresses during growth, and [`std::list`](list.md)
provides a node-based sequence with stable iterators.

## Declaration

```quxlang
::dynarr TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the element type. The declaration carries no struct modifier. Each member
function instantiates only the operations of `T` that the member function uses:
the copy constructor requires a copy-constructible `T`, growth requires a
move-constructible `T`, and `resize` requires a default-constructible `T` when
the array grows. A `MOVE_ONLY` element type is supported by `push_back`, the
`TEMP&` overload of `insert`, `erase`, `reserve`, and the move constructor.

`dynarr` declares no assignment operator and no swap operator. Assignment uses
the compiler-generated operation described in
[Assignment Operators](../../reference/assignment-operators.md). Copy
assignment produces an independent copy, and move assignment leaves the source
array empty.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not evaluated
results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults for each build type.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Random-access position that yields `MUT& T`. |
| [`const_iterator`](#iterator-and-const_iterator) | Random-access position that yields `CONST& T`. |
| [`index_iterator`](#projection-types) | Forward position that yields a zero-based index of type `SZ`. |
| [`index_range`](#projection-types) | Range of the zero-based indexes of the array. |
| [`index_value`](#projection-types) | Reference to one mutable element together with its index. |
| [`const_index_value`](#projection-types) | Reference to one constant element together with its index. |
| [`index_value_iterator`](#projection-types) | Forward position that yields `index_value`. |
| [`const_index_value_iterator`](#projection-types) | Forward position that yields `const_index_value`. |
| [`index_value_range`](#projection-types) | Range of `index_value` projections. |
| [`const_index_value_range`](#projection-types) | Range of `const_index_value` projections. |

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty array, copies an array, or transfers an allocation. |
| [`DESTRUCTOR`](#destructor) | Destroys all elements and releases the allocation. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of live elements. |
| [`capacity`](#capacity) | Returns the number of elements that fit without reallocation. |
| [`empty`](#empty) | Reports whether the array contains no elements. |
| [`reserve`](#reserve) | Ensures a minimum capacity. |

**Element access**

| Member | Description |
| --- | --- |
| [`OPERATOR[]`](#operator-index) | Returns a reference to the element at an index. |
| [`front`](#front) | Returns a reference to the first element. |
| [`back`](#back) | Returns a reference to the final element. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`push_back`](#push_back) | Appends one element. |
| [`insert`](#insert) | Inserts one element before an index. |
| [`erase`](#erase) | Removes the element at an index. |
| [`pop_back`](#pop_back) | Removes the final element. |
| [`resize`](#resize) | Changes the number of elements. |
| [`clear`](#clear) | Removes all elements and retains the allocation. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators to the first element and to the position after the final element. |
| [`VALUES`](#values) | Returns the array as its value range. |
| [`INDEXES`](#indexes) | Returns the range of zero-based indexes. |
| [`IV_PAIRS`](#iv_pairs) | Returns the range of index/value projections. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                              // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& dynarr#(T))      // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& dynarr#(T))       // (3)
```

1. Creates an empty array. The constructor allocates no storage, and
   `capacity()` returns zero.
2. Reserves storage for exactly `OTHER.size()` elements and copy-constructs
   every element of `OTHER` in index order.
3. Exchanges the allocation, size, and capacity of the new array with those of
   `OTHER`. `OTHER` is left empty with a capacity of zero.

**Complexity**: (1) and (3) are constant. (2) is linear in `OTHER.size()`.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys the elements from the final element to the first
element and then releases the allocation when `capacity()` is nonzero.

**Complexity**: Linear in `size()`.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of live elements.

### `capacity`

```quxlang
.capacity FUNCTION() CONST: SZ
```

**Return value**: The number of elements that the current allocation holds.
The value is zero for an array that owns no allocation.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when `size()` is zero.

### `reserve`

```quxlang
.reserve FUNCTION(@ARG:capacity SZ) MUT: VOID
```

`reserve` ensures that the allocation holds at least `capacity` elements. The
function has no effect when `capacity` does not exceed the current capacity.
Otherwise the function allocates storage for exactly `capacity` elements,
move-constructs every element into the new storage in index order, destroys the
moved-from elements, and releases the previous allocation. The size of the
array does not change.

**Complexity**: Linear in `size()` when a reallocation occurs.

**Iterator and reference validity**: A reallocation invalidates every pointer,
reference, and iterator into the array.

### `push_back`

```quxlang
.push_back FUNCTION(@ARG:value T) MUT: VOID
```

`push_back` receives `value` by value and inserts it at index `size()` through
the `TEMP&` overload of [`insert`](#insert). The argument is copied or moved
into the parameter before the array grows, so the argument may refer to an
element of the same array.

**Checked failures**: The function asserts with `ASSERT` that `size()` is less
than the maximum `SZ` value.

**Complexity**: Constant when the capacity suffices. A reallocation is linear in
`size()`. Capacity growth doubles the capacity, starting from one element.

**Iterator and reference validity**: A reallocation invalidates every pointer,
reference, and iterator into the array.

### `insert`

```quxlang
.insert FUNCTION(@index SZ, @value CONST& T) MUT: VOID      // (1)
.insert FUNCTION(@index SZ, @value TEMP& T) MUT: VOID       // (2)
```

`insert` places a new element before the element at `index`. An `index` equal
to `size()` appends the element.

1. Copy-constructs the new element from `value`.
2. Move-constructs the new element from `value`.

Both overloads first construct a staged local object from `value`, then grow
the allocation when `size()` equals `capacity()`, then relocate every element
at or after `index` one position toward the end by move construction followed
by destruction of the source, and finally move-construct the staged object into
position `index`. Because the staged object is constructed before any element
is relocated, `value` may refer to an element of the same array.

**Parameters**: `index` is the zero-based position that the new element
occupies. `value` is the source of the new element.

**Checked failures**: The function asserts with `ASSERT` that `index <= size()`
and that `size()` is less than the maximum `SZ` value.

**Complexity**: Linear in `size() - index`. A reallocation is linear in
`size()`.

**Iterator and reference validity**: A reallocation invalidates every pointer,
reference, and iterator into the array. Without a reallocation, the operation
invalidates pointers, references, and iterators to the elements at or after
`index`.

### `erase`

```quxlang
.erase FUNCTION(@index SZ) MUT: VOID
```

`erase` destroys the element at `index` and relocates every later element one
position toward the beginning by move construction followed by destruction of
the source. The capacity does not change.

**Checked failures**: The function asserts with `ASSERT` that `index < size()`.

**Complexity**: Linear in `size() - index`.

**Iterator and reference validity**: The operation invalidates pointers,
references, and iterators to the elements at or after `index`.

### `pop_back`

```quxlang
.pop_back FUNCTION() MUT: VOID
```

`pop_back` destroys the final element and decrements the size. The capacity
does not change.

**Checked failures**: The function asserts with `ASSERT` that the array is not
empty.

**Complexity**: Constant.

### `resize`

```quxlang
.resize FUNCTION(@ARG:size SZ) MUT: VOID
```

`resize` changes the number of elements to `size`. When `size` is less than
`size()`, the function destroys trailing elements through `pop_back`. When
`size` is greater than `size()`, the function grows the capacity geometrically
until `size` elements fit and default-constructs each new element in place.

**Complexity**: Linear in the difference between `size` and the previous size.
A reallocation is linear in the previous size.

**Iterator and reference validity**: A reallocation invalidates every pointer,
reference, and iterator into the array. A reduction invalidates pointers,
references, and iterators to the destroyed elements.

### `clear`

```quxlang
.clear FUNCTION() MUT: VOID
```

`clear` destroys every element from the final element to the first element.
The allocation and `capacity()` are retained.

**Complexity**: Linear in `size()`.

### `front`

```quxlang
.front FUNCTION() MUT: MUT& T        // (1)
.front FUNCTION() CONST: CONST& T    // (2)
```

**Return value**: A reference to the element at index zero. Overload (1)
returns a mutable reference and overload (2) returns a constant reference.

**Checked failures**: The function asserts with `ASSERT` that the array is not
empty.

### `back`

```quxlang
.back FUNCTION() MUT: MUT& T        // (1)
.back FUNCTION() CONST: CONST& T    // (2)
```

**Return value**: A reference to the element at index `size() - 1`. Overload
(1) returns a mutable reference and overload (2) returns a constant reference.

**Checked failures**: The function asserts with `ASSERT` that the array is not
empty.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%index SZ) MUT: MUT& T        // (1)
.OPERATOR[] FUNCTION(%index SZ) CONST: CONST& T    // (2)
```

**Return value**: A reference to the element at the zero-based `index`.
Overload (1) returns a mutable reference and overload (2) returns a constant
reference.

**Checked failures**: The function asserts with `ASSERT` that `index < size()`.

**Complexity**: Constant.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: dynarr#(T)::iterator            // (1)
.BEGIN FUNCTION() CONST: dynarr#(T)::const_iterator    // (2)
.END FUNCTION() MUT: dynarr#(T)::iterator              // (3)
.END FUNCTION() CONST: dynarr#(T)::const_iterator      // (4)
```

`BEGIN` returns an iterator at index zero. `END` returns an iterator at index
`size()`, which is one position beyond the final element. Both iterators are
equal for an empty array. These members implement the range protocol of
[`LOOP` statements](../../reference/loop-statements.md), so `LOOP ITEM(item)
IN(array)` visits every element in index order.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: MUT& dynarr#(T)        // (1)
.VALUES FUNCTION() CONST: CONST& dynarr#(T)    // (2)
```

**Return value**: A reference to the array itself. `LOOP VALUE(value)
IN(array)` iterates this range and binds each element.

### `INDEXES`

```quxlang
.INDEXES FUNCTION() CONST: dynarr#(T)::index_range
```

**Return value**: A range that yields the indexes `0` through `size() - 1` as
`SZ` values. `LOOP INDEX(index) IN(array)` iterates this range. The range
records `size()` at the time of the call.

### `IV_PAIRS`

```quxlang
.IV_PAIRS FUNCTION() MUT: dynarr#(T)::index_value_range            // (1)
.IV_PAIRS FUNCTION() CONST: dynarr#(T)::const_index_value_range    // (2)
```

**Return value**: A range whose elements provide `.INDEX()` and `.VALUE()`.
`LOOP INDEX(index) VALUE(value) IN(array)` iterates this range. Overload (1)
binds `value` as `MUT& T` and overload (2) binds `value` as `CONST& T`. The
range records the allocation and `size()` at the time of the call.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types store the address of the allocation and a zero-based element index.
`iterator` yields mutable elements and `const_iterator` yields constant
elements. An iterator remains valid until the owning array reallocates, or
until the array shifts or destroys the element that the iterator designates.
The operators perform no bounds checks.

```quxlang
.CONSTRUCTOR FUNCTION(@storage MUT=>>TYPED_STORAGE(T), @index SZ)                 // (1) iterator
.CONSTRUCTOR FUNCTION(@storage CONST=>>TYPED_STORAGE(T), @index SZ)               // (1) const_iterator
.CONSTRUCTOR FUNCTION(@OTHER CONST& dynarr#(T)::iterator)                         // (2) const_iterator
.OPERATOR== FUNCTION(@OTHER CONST& dynarr#(T)::iterator) CONST: BOOL              // (3)
.OPERATOR<=> FUNCTION(@OTHER CONST& dynarr#(T)::iterator) CONST: ORDER            // (4)
.OPERATOR++ FUNCTION() MUT: dynarr#(T)::iterator                                  // (5)
.OPERATOR-- FUNCTION() MUT: dynarr#(T)::iterator                                  // (6)
.OPERATOR+= FUNCTION(@OTHER SZ) MUT: MUT& dynarr#(T)::iterator                    // (7)
.OPERATOR-= FUNCTION(@OTHER SZ) MUT: MUT& dynarr#(T)::iterator                    // (8)
.OPERATOR+ FUNCTION(@OTHER SZ) CONST: dynarr#(T)::iterator                        // (9)
.OPERATOR- FUNCTION(@OTHER SZ) CONST: dynarr#(T)::iterator                        // (10)
.OPERATOR- FUNCTION(@OTHER CONST& dynarr#(T)::iterator) CONST: iterator_difference  // (11)
.OPERATOR-> FUNCTION() CONST: MUT& T                                              // (12) iterator
.OPERATOR-> FUNCTION() CONST: CONST& T                                            // (12) const_iterator
```

Signatures (3) through (11) are shown for `iterator`. `const_iterator` declares
the same operators with `dynarr#(T)::const_iterator` in place of
`dynarr#(T)::iterator`.

1. Creates an iterator at `index` within `storage`.
2. Converts a mutable iterator to a constant iterator at the same position.
3. Returns `TRUE` when both iterators designate the same allocation and the
   same index.
4. Orders two iterators by index. The operator asserts with `ASSERT` that both
   iterators designate the same allocation.
5. Advances the iterator by one element and returns its previous value.
6. Moves the iterator back by one element and returns its previous value.
7. Advances the iterator by `OTHER` elements and returns the iterator.
8. Moves the iterator back by `OTHER` elements and returns the iterator.
9. Returns the iterator `OTHER` elements after the current position.
10. Returns the iterator `OTHER` elements before the current position.
11. Returns the signed number of elements from `OTHER` to the current position
    as `std::iterator_difference`, which is `I32` when `SZ` has 32 bits and
    `I64` when `SZ` has 64 bits. The operator asserts with `ASSERT` that both
    iterators designate the same allocation.
12. Returns a reference to the designated element. The iterator must designate
    a live element.

## Projection types

The projection types implement the `INDEXES` and `IV_PAIRS` members of the
range protocol. User code normally obtains them through
[`INDEXES`](#indexes), [`IV_PAIRS`](#iv_pairs), or a `LOOP` statement.

```quxlang
::index_iterator STRUCT
::index_range STRUCT
::index_value STRUCT
::const_index_value STRUCT
::index_value_iterator STRUCT
::const_index_value_iterator STRUCT
::index_value_range STRUCT
::const_index_value_range STRUCT
```

**`index_iterator`** iterates zero-based indexes.

```quxlang
.CONSTRUCTOR FUNCTION(@index SZ)
.OPERATOR== FUNCTION(@OTHER CONST& dynarr#(T)::index_iterator) CONST: BOOL
.OPERATOR<=> FUNCTION(@OTHER CONST& dynarr#(T)::index_iterator) CONST: ORDER
.OPERATOR++ FUNCTION() MUT: dynarr#(T)::index_iterator
.OPERATOR+= FUNCTION(@OTHER SZ) MUT: MUT& dynarr#(T)::index_iterator
.OPERATOR-> FUNCTION() CONST: SZ
```

The comparison operators compare indexes. `OPERATOR++` advances by one index
and returns the previous iterator, `OPERATOR+=` advances by `OTHER` indexes,
and `OPERATOR->` returns the current index by value.

**`index_range`** presents `count` indexes as a range.

```quxlang
.CONSTRUCTOR FUNCTION(@count SZ)
.BEGIN FUNCTION() CONST: dynarr#(T)::index_iterator
.END FUNCTION() CONST: dynarr#(T)::index_iterator
```

`BEGIN` returns an iterator at index zero and `END` returns an iterator at
index `count`.

**`index_value`** and **`const_index_value`** refer to one element together
with its index.

```quxlang
.CONSTRUCTOR FUNCTION(@storage MUT=>>TYPED_STORAGE(T), @index SZ)      // index_value
.CONSTRUCTOR FUNCTION(@storage CONST=>>TYPED_STORAGE(T), @index SZ)    // const_index_value
.INDEX FUNCTION() CONST: SZ
.VALUE FUNCTION() CONST: MUT& T                                        // index_value
.VALUE FUNCTION() CONST: CONST& T                                      // const_index_value
```

`INDEX` returns the zero-based index and `VALUE` returns a reference to the
element at that index.

**`index_value_iterator`** and **`const_index_value_iterator`** iterate
index/value projections.

```quxlang
.CONSTRUCTOR FUNCTION(@storage MUT=>>TYPED_STORAGE(T), @index SZ)
.OPERATOR== FUNCTION(@OTHER CONST& dynarr#(T)::index_value_iterator) CONST: BOOL
.OPERATOR<=> FUNCTION(@OTHER CONST& dynarr#(T)::index_value_iterator) CONST: ORDER
.OPERATOR++ FUNCTION() MUT: dynarr#(T)::index_value_iterator
.OPERATOR+= FUNCTION(@OTHER SZ) MUT: MUT& dynarr#(T)::index_value_iterator
.OPERATOR-> FUNCTION() CONST: dynarr#(T)::index_value
```

The signatures are shown for `index_value_iterator`.
`const_index_value_iterator` declares the same members with
`CONST=>>TYPED_STORAGE(T)`, `dynarr#(T)::const_index_value_iterator`, and
`dynarr#(T)::const_index_value`. `OPERATOR==` compares the allocation and the
index. `OPERATOR<=>` orders by index and asserts with `ASSERT` that both
iterators designate the same allocation. `OPERATOR->` returns the projection
for the current position by value.

**`index_value_range`** and **`const_index_value_range`** present the
projections of `count` elements as a range.

```quxlang
.CONSTRUCTOR FUNCTION(@storage MUT=>>TYPED_STORAGE(T), @count SZ)       // index_value_range
.CONSTRUCTOR FUNCTION(@storage CONST=>>TYPED_STORAGE(T), @count SZ)     // const_index_value_range
.BEGIN FUNCTION() CONST: dynarr#(T)::index_value_iterator               // index_value_range
.END FUNCTION() CONST: dynarr#(T)::index_value_iterator                 // index_value_range
.BEGIN FUNCTION() CONST: dynarr#(T)::const_index_value_iterator         // const_index_value_range
.END FUNCTION() CONST: dynarr#(T)::const_index_value_iterator           // const_index_value_range
```

`BEGIN` returns an iterator at index zero and `END` returns an iterator at
index `count`.

## Example

The program inserts into the middle of an array, erases the first element, and
iterates the remaining elements with their indexes.

```quxlang
--8<-- "example-programs/reference-programs/dynarr_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/dynarr_example/expected-output.txt"
```

## See also

- [`std::geometric_array`](geometric-array.md)
- [`std::list`](list.md)
- [`std::cord`](cord.md)
- [`std::string`](string.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
- [Object Storage and Lifetime](../../reference/typed-storage-and-lifetime.md)
- [Move Semantics](../../reference/move-semantics.md)
