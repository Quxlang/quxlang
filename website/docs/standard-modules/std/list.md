# `std::list`

Module `std` · source file `list.qxs`

`std::list` owns a sequence of `T` objects in individually allocated, doubly
linked nodes. Each node is created with `NEW` and released with `DELETE`.
Insertion and erasure at any position take constant time and preserve the
address of, and the iterators to, every element that is not erased. The list
provides bidirectional iteration and no indexed access.
[`std::dynarr`](dynarr.md) and [`std::geometric_array`](geometric-array.md)
provide indexed sequences.

## Declaration

```quxlang
::list TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the element type. The declaration carries no struct modifier. The
`CONST& T` overloads of the insertion functions and the copy constructor
require a copy-constructible `T`. The `TEMP& T` overloads require a
move-constructible `T` and accept a `MOVE_ONLY` element type.

`list` declares no assignment operator and no swap operator. Assignment uses
the compiler-generated operation described in
[Assignment Operators](../../reference/assignment-operators.md). Copy
assignment produces an independent copy, and move assignment leaves the source
list empty.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not evaluated
results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults for each build type.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Bidirectional position that yields `MUT& T`. |
| [`const_iterator`](#iterator-and-const_iterator) | Bidirectional position that yields `CONST& T`. |

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty list, copies a list, or transfers all nodes. |
| [`DESTRUCTOR`](#destructor) | Destroys every element and releases every node. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of elements. |
| [`empty`](#empty) | Reports whether the list contains no elements. |

**Element access**

| Member | Description |
| --- | --- |
| [`front`](#front) | Returns a reference to the first element. |
| [`back`](#back) | Returns a reference to the final element. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`push_front`](#push_front) | Inserts an element at the beginning. |
| [`push_back`](#push_back) | Inserts an element at the end. |
| [`pop_front`](#pop_front) | Removes the first element. |
| [`pop_back`](#pop_back) | Removes the final element. |
| [`insert`](#insert) | Inserts an element before a position. |
| [`erase`](#erase) | Removes the element at a position. |
| [`clear`](#clear) | Removes every element. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators to the first element and to the past-the-end position. |
| [`VALUES`](#values) | Returns the list as its element range. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                            // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& list#(T))      // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& list#(T))       // (3)
```

1. Creates an empty list.
2. Copy-constructs each element of `OTHER` in sequence order into a new node.
3. Transfers all nodes of `OTHER` to the new list and leaves `OTHER` empty. No
   element is copied or moved.

**Complexity**: (1) and (3) are constant. (2) is linear in `OTHER.size()`.

**Iterator and reference validity**: After (3), references to the transferred
elements remain valid. An iterator obtained from `OTHER` continues to record
`OTHER` as its owner and therefore does not satisfy the owner requirement of
[`insert`](#insert) and [`erase`](#erase) on the new list.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys every remaining element from the first element to the
final element and releases every node.

**Complexity**: Linear in `size()`.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of elements in the list.

**Complexity**: Constant. The list stores its element count.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the list contains no elements.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` removes and destroys every element through repeated
[`pop_front`](#pop_front) calls and leaves the list empty.

**Complexity**: Linear in `size()`.

**Iterator and reference validity**: The operation invalidates every iterator
and reference to an element. The past-the-end iterator remains valid.

### `push_front`

```quxlang
.push_front FUNCTION(@ARG:value CONST& T) MUT                      // (1)
.push_front FUNCTION(@ARG:value TEMP& T) MUT WEAK_PRIORITY(1)      // (2)
```

1. Copy-constructs `value` in a new node at the beginning of the list.
2. Move-constructs `value` in a new node at the beginning of the list.
   `WEAK_PRIORITY(1)` selects this overload when overload resolution does not
   otherwise distinguish the two overloads.

**Checked failures**: The function asserts with `ASSERT` that `size()` is less
than the maximum `SZ` value.

**Complexity**: Constant.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `push_back`

```quxlang
.push_back FUNCTION(@ARG:value CONST& T) MUT                      // (1)
.push_back FUNCTION(@ARG:value TEMP& T) MUT WEAK_PRIORITY(1)      // (2)
```

1. Copy-constructs `value` in a new node at the end of the list.
2. Move-constructs `value` in a new node at the end of the list.
   `WEAK_PRIORITY(1)` selects this overload when overload resolution does not
   otherwise distinguish the two overloads.

**Checked failures**: The function asserts with `ASSERT` that `size()` is less
than the maximum `SZ` value.

**Complexity**: Constant.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `pop_front`

```quxlang
.pop_front FUNCTION() MUT
```

`pop_front` unlinks the first node, destroys its element, and releases the
node.

**Checked failures**: The function asserts with `ASSERT` that the list is not
empty.

**Complexity**: Constant.

**Iterator and reference validity**: The operation invalidates iterators and
references to the removed element only.

### `pop_back`

```quxlang
.pop_back FUNCTION() MUT
```

`pop_back` unlinks the final node, destroys its element, and releases the node.

**Checked failures**: The function asserts with `ASSERT` that the list is not
empty.

**Complexity**: Constant.

**Iterator and reference validity**: The operation invalidates iterators and
references to the removed element only.

### `front`

```quxlang
.front FUNCTION() MUT: MUT& T        // (1)
.front FUNCTION() CONST: CONST& T    // (2)
```

**Return value**: A reference to the first element. Overload (1) returns a
mutable reference and overload (2) returns a constant reference.

**Checked failures**: The function asserts with `ASSERT` that the list is not
empty.

### `back`

```quxlang
.back FUNCTION() MUT: MUT& T        // (1)
.back FUNCTION() CONST: CONST& T    // (2)
```

**Return value**: A reference to the final element. Overload (1) returns a
mutable reference and overload (2) returns a constant reference.

**Checked failures**: The function asserts with `ASSERT` that the list is not
empty.

### `insert`

```quxlang
.insert FUNCTION(
    @position CONST& list#(T)::const_iterator,
    @value CONST& T
  ) MUT: list#(T)::iterator                         // (1)
.insert FUNCTION(
    @position CONST& list#(T)::const_iterator,
    @value TEMP& T
  ) MUT: list#(T)::iterator                         // (2)
```

`insert` links a new node immediately before `position`. A past-the-end
`position` appends the element.

1. Copy-constructs `value` in the new node.
2. Move-constructs `value` in the new node.

**Parameters**: `position` designates the element before which the new element
is inserted, or the past-the-end position. A mutable `iterator` argument
converts to `const_iterator`. `value` is the source of the new element.

**Return value**: A mutable iterator to the inserted element.

**Checked failures**: The function asserts with `ASSERT` that `position` was
obtained from this list object and that `size()` is less than the maximum `SZ`
value.

**Complexity**: Constant.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `erase`

```quxlang
.erase FUNCTION(@position CONST& list#(T)::const_iterator) MUT: list#(T)::iterator
```

`erase` unlinks the node at `position`, destroys its element, and releases the
node.

**Return value**: A mutable iterator to the element that followed the erased
element, or the past-the-end iterator when the erased element was the final
element.

**Checked failures**: The function asserts with `ASSERT` that `position` was
obtained from this list object and that `position` is not the past-the-end
position.

**Complexity**: Constant.

**Iterator and reference validity**: The operation invalidates iterators and
references to the erased element only.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: list#(T)::iterator            // (1)
.BEGIN FUNCTION() CONST: list#(T)::const_iterator    // (2)
.END FUNCTION() MUT: list#(T)::iterator              // (3)
.END FUNCTION() CONST: list#(T)::const_iterator      // (4)
```

`BEGIN` returns an iterator to the first element. `END` returns the
past-the-end iterator. Both iterators are equal for an empty list. These
members implement the range protocol of
[`LOOP` statements](../../reference/loop-statements.md), so
`LOOP ITEM(item) IN(list)` visits every element in sequence order.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: MUT& list#(T)        // (1)
.VALUES FUNCTION() CONST: CONST& list#(T)    // (2)
```

**Return value**: A reference to the list itself. `LOOP VALUE(value) IN(list)`
iterates this range and binds each element. The list declares no `INDEXES` or
`IV_PAIRS` member.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types provide bidirectional traversal. An iterator stores the address of
its owning list object and the address of a node; the past-the-end position
stores a null node address. `iterator` yields mutable elements and
`const_iterator` yields constant elements. An iterator remains valid until the
element that it designates is removed. The constructor that takes an owner and
a node is private to module `std`.

```quxlang
.CONSTRUCTOR FUNCTION()                                             // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& list#(T)::iterator)             // (2) const_iterator
.OPERATOR== FUNCTION(@OTHER CONST& list#(T)::iterator) CONST: BOOL  // (3)
.OPERATOR++ FUNCTION() MUT: list#(T)::iterator                      // (4)
.OPERATOR-- FUNCTION() MUT: list#(T)::iterator                      // (5)
.OPERATOR-> FUNCTION() CONST: MUT& T                                // (6) iterator
.OPERATOR-> FUNCTION() CONST: CONST& T                              // (6) const_iterator
```

Signatures (3) through (5) are shown for `iterator`. `const_iterator` declares
the same operators with `list#(T)::const_iterator` in place of
`list#(T)::iterator`.

1. Creates an iterator that is not associated with a list. Such an iterator
   does not compare equal to an iterator obtained from a list.
2. Converts a mutable iterator to a constant iterator at the same position.
3. Returns `TRUE` when both iterators record the same list object and the same
   position.
4. Advances to the next element and returns the previous position. The
   operator asserts with `ASSERT` that the iterator is not the past-the-end
   position.
5. Moves to the previous element and returns the previous position.
   Decrementing the past-the-end iterator yields the final element. The
   operator asserts with `ASSERT` that the iterator is associated with a list
   and that the iterator does not designate the first element or the
   past-the-end position of an empty list.
6. Returns a reference to the current element. The operator asserts with
   `ASSERT` that the iterator is not the past-the-end position.

The iterators declare no ordering operator and no arithmetic operator.

## Example

The program inserts before a position, erases through a second position, and
iterates the remaining elements.

```quxlang
--8<-- "example-programs/reference-programs/list_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/list_example/expected-output.txt"
```

## See also

- [`std::dynarr`](dynarr.md)
- [`std::geometric_array`](geometric-array.md)
- [`std::avl_tree`](avl-tree.md), which provides logarithmic insertion at an
  index
- [Dispatch jobs in priority order](../../examples/dispatch-jobs.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
- [`NEW` and `DELETE`](../../reference/new-and-delete.md)
- [Overload Resolution](../../reference/overload-resolution.md)
