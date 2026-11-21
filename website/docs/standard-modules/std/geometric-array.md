# `std::geometric_array`

Module `std` · source file `geometric_array.qxs`

`std::geometric_array` owns a sequence of `T` objects in separately allocated
segments whose capacities are the successive powers of two, beginning with one
element. Segment `k` holds the elements with indexes `2^k - 1` through
`2^(k+1) - 2`. Appending elements and reserving capacity allocate further
segments and never relocate an existing element, so the address of every
element remains valid until that element is removed. Unused slots contain no
live `T` objects. [`std::dynarr`](dynarr.md) stores its elements contiguously
and relocates them during growth.

## Declaration

```quxlang
::geometric_array TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the element type. The destructor of `T` must not throw. The number of
elements is always less than the maximum `SZ` value; a requested size or
capacity equal to that value causes
[`std::container_error::size_overflow`](container-error.md) to be thrown.

The array declares `BEGIN`, `END`, and `VALUES` and therefore supports
`LOOP ITEM` and `LOOP VALUE` iteration. The array declares no `INDEXES` or
`IV_PAIRS` member.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A requirement that the source states with
`INVARIANT` is checked when the `INVARIANT_CHECKED` policy is enabled and is
emitted as an optimizer assumption otherwise. A violated requirement that is
not checked results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies how
an output selects policies.

**Target restrictions**: The test bundle annotates the growth, error-reporting,
and failure-cleanup tests of `geometric_array` with
`KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Random-access position that yields `MUT& T`. |
| [`const_iterator`](#iterator-and-const_iterator) | Random-access position that yields `CONST& T`. |

## Member summary

**Construction and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty array, copies an array, or transfers segments. |
| [`OPERATOR:=`](#operator-assign) | Replaces the contents by copy or by transfer. |
| [`OPERATOR<->`](#operator-swap) | Exchanges the contents of two arrays. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of live elements. |
| [`capacity`](#capacity) | Returns the number of allocated element slots. |
| [`empty`](#empty) | Reports whether the array contains no elements. |
| [`reserve`](#reserve) | Allocates segments for a minimum capacity. |
| [`shrink_to_fit`](#shrink_to_fit) | Releases all empty trailing segments. |

**Element access**

| Member | Description |
| --- | --- |
| [`OPERATOR[]`](#operator-index) | Returns a reference to the element at an index. |
| [`front`](#front) | Returns a reference to the first element. |
| [`back`](#back) | Returns a reference to the last element. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`emplace_back`](#emplace_back) | Constructs an element at the end from forwarded arguments. |
| [`push_back`](#push_back) | Appends a copied, moved, or converted value. |
| [`pop_back`](#pop_back) | Destroys the last element. |
| [`resize`](#resize) | Changes the number of elements. |
| [`clear`](#clear) | Destroys all elements. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators at the beginning and at the exclusive end. |
| [`VALUES`](#values) | Returns the array as its value range. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& geometric_array#T) // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& geometric_array#T)  // (3)
```

1. Creates an empty array without allocating storage.
2. Copies the elements of `OTHER`, in order, into separately allocated
   segments. Each element is copy-constructed through
   [`emplace_back`](#emplace_back).
3. Transfers the segments of `OTHER` to the new array and leaves `OTHER` empty.
   Element addresses are preserved.

**Exceptions**: (2) propagates an exception thrown by the copy constructor of
`T`. The elements already copied are destroyed.

**Complexity**: (1) and (3) are constant. (2) is linear in `OTHER.size()`.

### `OPERATOR:=` {: #operator-assign }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& geometric_array#T) MUT    // (1)
.OPERATOR:= FUNCTION(@OTHER TEMP& geometric_array#T) MUT     // (2)
```

1. Constructs a copy of `OTHER` and then exchanges the copy with the current
   array. The previous contents are destroyed with the temporary copy. A
   failure during the copy leaves the current array unchanged.
2. Transfers the contents of `OTHER` to the current array, destroys the
   previous contents, and leaves `OTHER` empty. Assignment from the same array
   object preserves its contents.

### `OPERATOR<->` {: #operator-swap }

```quxlang
.OPERATOR<-> FUNCTION(@OTHER MUT& geometric_array#T) MUT
```

The swap operator exchanges the segments and the sizes of the two arrays.
Element addresses are preserved; each element belongs to the other array after
the exchange.

**Complexity**: Constant.

**Iterator and reference validity**: A swap may invalidate the iterators of
both arrays, because an iterator designates its owning array object and a
logical index.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of live elements.

### `capacity`

```quxlang
.capacity FUNCTION() CONST: SZ
```

**Return value**: The number of allocated element slots. An array with `n`
segments returns `2^n - 1`. An array whose segment count equals the number of
bits in `SZ` returns the maximum `SZ` value.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the array contains no elements.

### `reserve`

```quxlang
.reserve FUNCTION(@ARG:capacity SZ) MUT
```

`reserve` allocates segments until the array holds at least
`std::bit_width(capacity)` segments, which provide at least `capacity` element
slots. Existing elements and their addresses are preserved. The function has no
effect when the array already holds that many segments.

**Exceptions**: The function throws `std::container_error::size_overflow` when
`capacity` equals the maximum `SZ` value. When the call exits with an
exception after it has allocated segments, the function releases those
segments, and the elements, size, and capacity are unchanged.

**Iterator and reference validity**: No pointer, reference, or iterator is
invalidated.

### `shrink_to_fit`

```quxlang
.shrink_to_fit FUNCTION() MUT
```

`shrink_to_fit` releases every empty trailing segment, so the array retains
`std::bit_width(size())` segments. Existing element addresses are preserved.

### `emplace_back`

```quxlang
.emplace_back FUNCTION(%...values TT, @KWARGS ...) MUT: MUT& T
```

`emplace_back` constructs a new last element in place from the forwarded
positional and keyword arguments. A call without arguments default-constructs
the element. When the element belongs to a segment that is not yet allocated,
the function allocates that segment first. The size increases only after the
construction succeeds.

**Parameters**: `values` and `KWARGS` are forwarded to the constructor of `T`
with their reference categories preserved. For example, `@OTHER source` selects
the copy or move constructor.

**Return value**: A mutable reference to the new element.

**Exceptions**: The function throws `std::container_error::size_overflow` when
`size()` is not less than the maximum `SZ` value minus one. An exception thrown
by the constructor of `T` propagates, and the array retains its previous
elements and size.

**Complexity**: Constant apart from the segment-index computation, which
executes one step for each significant bit of `size() + 1`, and the growth of
the segment directory, whose length is bounded by the number of bits in `SZ`.

**Iterator and reference validity**: No pointer, reference, or iterator to an
existing element is invalidated.

### `push_back`

```quxlang
.push_back FUNCTION(@ARG:value AUTO& T) MUT            // (1)
.push_back FUNCTION(@ARG:value T) MUT PRIORITY(-1)     // (2)
```

1. Appends a copy of `value`, or moves `value` into the array when the argument
   is a temporary reference. The deduced reference qualifier of the argument is
   forwarded to [`emplace_back`](#emplace_back) as `@OTHER`.
2. Appends a value that is implicitly converted to `T`. The overload has
   `PRIORITY(-1)` and is selected only when overload (1) is not viable.

**Exceptions**: The conditions of [`emplace_back`](#emplace_back) apply.

**Iterator and reference validity**: Existing element addresses remain valid.

### `pop_back`

```quxlang
.pop_back FUNCTION() MUT
```

`pop_back` destroys the last element and then releases trailing segments until
at most one empty trailing segment remains. A `pop_back` call therefore also
releases segments that an earlier `reserve` call allocated beyond that limit.

**Preconditions**: The array contains at least one element. The requirement is
stated with `INVARIANT`.

**Iterator and reference validity**: The operation invalidates pointers,
references, and iterators to the removed element.

### `resize`

```quxlang
.resize FUNCTION(@ARG:size SZ) MUT
```

`resize` changes the number of elements to `size`. The function first removes
elements from the end through [`pop_back`](#pop_back) while the array holds
more than `size` elements. The function then reserves capacity for `size`
elements and default-constructs new elements until the array holds `size`
elements.

**Exceptions**: The function throws `std::container_error::size_overflow` when
`size` equals the maximum `SZ` value. When growth fails with an exception, the
function destroys the elements that the call constructed, so the original
elements and size are preserved. A failed call may change the capacity.

**Complexity**: Linear in the difference between `size` and the previous size.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` destroys all elements from the last element to the first element and
retains at most the initial one-element segment.

**Complexity**: Linear in `size()`.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%index SZ) MUT: MUT& T        // (1)
.OPERATOR[] FUNCTION(%index SZ) CONST: CONST& T    // (2)
```

**Return value**: A reference to the element at the zero-based `index`.
Overload (1) returns a mutable reference and overload (2) returns a constant
reference. The operator selects the segment from the most significant set bit
of `index + 1`.

**Preconditions**: `index < size()`. The requirement is stated with
`INVARIANT`.

**Complexity**: The segment computation executes one step for each significant
bit of `index + 1`. The remaining work is constant.

### `front`

```quxlang
.front FUNCTION(@THIS AUTO& THISTYPE): AUTO& T
```

**Return value**: A reference to the first element. The reference qualifier of
the result is the deduced reference qualifier of the receiver.

**Preconditions**: The array contains at least one element.

### `back`

```quxlang
.back FUNCTION(@THIS AUTO& THISTYPE): AUTO& T
```

**Return value**: A reference to the last element. The reference qualifier of
the result is the deduced reference qualifier of the receiver.

**Preconditions**: The array contains at least one element.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: geometric_array#(T)::iterator            // (1)
.END FUNCTION() MUT: geometric_array#(T)::iterator              // (2)
.BEGIN FUNCTION() CONST: geometric_array#(T)::const_iterator    // (3)
.END FUNCTION() CONST: geometric_array#(T)::const_iterator      // (4)
```

`BEGIN` returns an iterator at logical index zero. `END` returns an iterator at
logical index `size()`, which is the exclusive end of the array at the time of
the call.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: MUT& geometric_array#T        // (1)
.VALUES FUNCTION() CONST: CONST& geometric_array#T    // (2)
```

**Return value**: A reference to the array itself, for element iteration with
`LOOP VALUE(value) IN(array)`.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types provide random access by logical element index. An iterator stores
the address of its owning array object and an index. Appending elements and
reserving capacity therefore do not invalidate an iterator. Removing an element
invalidates the iterators to that element. Moving or swapping the array may
invalidate its iterators. User code obtains iterators from
[`BEGIN` and `END`](#begin-and-end); the constructor that takes an owner and an
index is private to module `std`.

```quxlang
.CONSTRUCTOR FUNCTION(@OTHER CONST& geometric_array#(T)::iterator)                           // (1) const_iterator
.OPERATOR== FUNCTION(@OTHER CONST& geometric_array#(T)::iterator) CONST: BOOL                // (2)
.OPERATOR<=> FUNCTION(@OTHER CONST& geometric_array#(T)::iterator) CONST: ORDER              // (3)
.OPERATOR++ FUNCTION() MUT: geometric_array#(T)::iterator                                    // (4)
.OPERATOR-- FUNCTION() MUT: geometric_array#(T)::iterator                                    // (5)
.OPERATOR+= FUNCTION(@OTHER SZ) MUT: MUT& geometric_array#(T)::iterator                      // (6)
.OPERATOR-= FUNCTION(@OTHER SZ) MUT: MUT& geometric_array#(T)::iterator                      // (7)
.OPERATOR+ FUNCTION(@OTHER SZ) CONST: geometric_array#(T)::iterator                          // (8)
.OPERATOR- FUNCTION(@OTHER SZ) CONST: geometric_array#(T)::iterator                          // (9)
.OPERATOR- FUNCTION(@OTHER CONST& geometric_array#(T)::iterator) CONST: iterator_difference  // (10)
.OPERATOR[] FUNCTION(%offset SZ) CONST: MUT& T                                               // (11) iterator
.OPERATOR[] FUNCTION(%offset SZ) CONST: CONST& T                                             // (11) const_iterator
.OPERATOR-> FUNCTION() CONST: MUT& T                                                         // (12) iterator
.OPERATOR-> FUNCTION() CONST: CONST& T                                                       // (12) const_iterator
```

Signatures (2) through (10) are shown for `iterator`. `const_iterator` declares
the same operators with `geometric_array#(T)::const_iterator` in place of
`geometric_array#(T)::iterator`.

1. Creates a constant iterator that refers to the same array and position as
   `OTHER`.
2. Returns `TRUE` when both iterators refer to the same array and position.
3. Orders the iterators by logical element index. The operator asserts with
   `ASSERT` that both iterators refer to the same array.
4. Advances the iterator by one element and returns its previous value. The
   iterator must precede `END()`; the requirement is stated with `INVARIANT`.
5. Moves the iterator back by one element and returns its previous value. The
   iterator must follow `BEGIN()`.
6. Advances the iterator by `OTHER` elements and returns a reference to the
   modified iterator.
7. Moves the iterator back by `OTHER` elements and returns a reference to the
   modified iterator.
8. Returns an iterator `OTHER` elements after the current position.
9. Returns an iterator `OTHER` elements before the current position.
10. Returns the signed number of elements from `OTHER` to the current position
    as `std::iterator_difference`. The operator asserts with `ASSERT` that both
    iterators refer to the same array. The difference must be representable in
    `std::iterator_difference`.
11. Returns a reference to the element at the specified offset from the
    iterator. The selected position must precede `END()`.
12. Returns a reference to the current element. The iterator must precede
    `END()`.

The position produced by iterator arithmetic must lie between `BEGIN()` and
`END()`, inclusive.

## Nonmember functions

### `std::bit_width`

```quxlang
::bit_width FUNCTION(@ARG:value SZ): SZ
```

**Return value**: The number of significant bits in `value`, which is the
one-based position of the most significant set bit. The result is zero when
`value` is zero.

**Complexity**: One step for each significant bit of `value`.

## Example

The program retains a reference to the first element while the array grows and
then releases unused segments.

```quxlang
--8<-- "example-programs/reference-programs/geometric_array_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/geometric_array_example/expected-output.txt"
```

## See also

- [`std::dynarr`](dynarr.md)
- [`std::list`](list.md)
- [`std::hadix_map`](hadix-map.md), which stores its bucket directory in a
  `geometric_array`
- [`std::container_error`](container-error.md)
- [Keep a reference while telemetry grows](../../examples/stable-telemetry.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
- [Exception Handling](../../reference/exceptions.md)
