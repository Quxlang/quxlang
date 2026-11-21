# `std::cord`

Module `std` · source file `cord.qxs`

`std::cord` owns a sequence of `T` objects as a balanced rope: a
height-balanced binary tree whose leaves are [`std::dynarr`](dynarr.md) element
arrays. Concatenation transfers ownership of both operands into one tree
without copying or moving any element, and each cord owns its tree uniquely;
no tree node is shared and no reference count is kept. Indexed access descends
the tree. `flatten` produces one contiguous array. [`std::string`](string.md)
and `std::dynarr` store their elements contiguously and copy on concatenation.

## Declaration

```quxlang
::cord TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the element type. Copy construction and the constant overload of
[`flatten`](#flatten) require a copy-constructible `T`; every other operation
accepts a `MOVE_ONLY` element type. Both operands of a concatenation have the
same element type.

`cord` declares no assignment operator and no iteration members. Assignment
uses the compiler-generated operation: copy assignment replaces the tree with
an independent deep copy and is rejected at compile time for an element type
that is not copyable, and move assignment leaves the source cord empty.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not evaluated
results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults for each build type.

**Target restrictions**: The test bundle annotates two repeated-concatenation
unit tests of `cord` with `KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member summary

**Construction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty cord or a single-leaf cord, copies a cord, or transfers a cord. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of elements. |
| [`empty`](#empty) | Reports whether the cord contains no elements. |

**Element access**

| Member | Description |
| --- | --- |
| [`OPERATOR[]`](#operator-index) | Returns a reference to the element at an index. |
| [`flatten`](#flatten) | Returns all elements as one contiguous array. |

**Concatenation**

| Member | Description |
| --- | --- |
| [`concat`](#concat) | Appends a cord by move. |
| [`OPERATOR+=`](#operator-plus-assign) | Appends a cord by move. |
| [`OPERATOR+`](#operator-plus) | Joins two temporary cords. |

**Nonmember functions**

| Function | Description |
| --- | --- |
| [`std::concat`](#stdconcat) | Joins two cords by move. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                               // (1)
.CONSTRUCTOR INCLUDE_IF(SAME_TYPES(T, BYTE))
  FUNCTION(@OTHER STRING_CONSTANT)                    // (2)
.CONSTRUCTOR FUNCTION(@value CONST& T)                // (3)
.CONSTRUCTOR FUNCTION(@value TEMP& T)                 // (4)
.CONSTRUCTOR FUNCTION(@OTHER CONST& dynarr#(T))       // (5)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& dynarr#(T))        // (6)
.CONSTRUCTOR FUNCTION(@OTHER CONST& cord#(T))         // (7)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& cord#(T))          // (8)
```

1. Creates an empty cord.
2. Copies the bytes of a string constant into one leaf. The constructor exists
   only for `cord#(BYTE)`, so `VAR text std::cord#BYTE := "abc";` is valid and
   the same initialization of a cord with another element type is a
   compilation error. An empty string constant produces an empty cord.
3. Copies one element into a new single-element leaf. The parameter is named
   `@value`, so the constructor is called as `std::cord#(I32)(@value element)`.
4. Moves one element into a new single-element leaf.
5. Copies an element array into one independently owned leaf. An empty array
   produces an empty cord.
6. Moves the allocation of an element array into one leaf and leaves the array
   empty. No element is copied or moved. An empty array produces an empty cord.
7. Deep-copies the complete tree and every element of `OTHER`.
8. Transfers the complete tree of `OTHER` and leaves `OTHER` empty.

**Complexity**: (1), (6), and (8) are constant. (2), (5), and (7) are linear in
the number of elements.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of elements that the cord represents.

**Complexity**: Constant. Each branch caches its element count.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the cord contains no elements.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%index SZ) CONST: CONST& T    // (1)
.OPERATOR[] FUNCTION(%index SZ) MUT: MUT& T        // (2)
```

**Return value**: A reference to the element at the zero-based `index` in
logical order. Overload (1) returns a constant reference and overload (2)
returns a mutable reference.

**Checked failures**: The operator asserts with `ASSERT` that
`index < size()`.

**Complexity**: Proportional to the height of the tree, which is logarithmic in
the number of leaves.

### `flatten`

```quxlang
.flatten FUNCTION() CONST: dynarr#(T)    // (1)
.flatten FUNCTION() TEMP: dynarr#(T)     // (2)
```

1. Copies all elements in logical order into one contiguous dynamic array. The
   cord is unchanged. The overload requires a copy-constructible `T`.
2. Moves all elements in logical order into one contiguous dynamic array and
   leaves the cord empty. The overload is selected for a temporary receiver,
   for example `MOVE(text).flatten()`.

**Return value**: A [`std::dynarr`](dynarr.md) whose capacity was reserved for
`size()` elements.

**Complexity**: Linear in `size()`.

### `concat`

```quxlang
.concat FUNCTION(@suffix TEMP& cord#(T)) MUT: MUT& cord#(T)
```

`concat` appends the elements of `suffix` after the elements of the current
cord by joining the two trees and restoring the height balance. `suffix` is
left empty. No element is copied or moved. An empty operand contributes no
node: the result is the other operand.

**Return value**: A reference to the current cord.

**Checked failures**: The function asserts with `ASSERT` that `suffix` is a
different cord object and that the combined element count does not exceed the
maximum `SZ` value.

**Complexity**: Proportional to the difference between the heights of the two
trees.

**Iterator and reference validity**: References to elements remain valid,
because the leaf arrays are transferred without relocation of their elements.

### `OPERATOR+=` {: #operator-plus-assign }

```quxlang
.OPERATOR+= FUNCTION(@OTHER TEMP& cord#(T)) MUT: MUT& cord#(T)
```

The operator appends `OTHER` through [`concat`](#concat) and returns the
current cord. `OTHER` is left empty. The right operand must be a temporary
reference, for example `text += MOVE(suffix);`.

### `OPERATOR+` {: #operator-plus }

```quxlang
.OPERATOR+ FUNCTION(@OTHER TEMP& cord#(T)) TEMP: cord#(T)
```

The operator moves both operands into a balanced concatenation result. Both
operands must be temporary references, for example
`MOVE(prefix) + MOVE(suffix)`, and both are left empty.

**Return value**: A cord that contains the elements of the left operand
followed by the elements of `OTHER`.

**Checked failures**: The conditions of [`concat`](#concat) apply.

## Nonmember functions

### `std::concat`

```quxlang
::concat TEMPLATE(@T TYPE AUTO)
  FUNCTION(@left TEMP& cord#(T), @right TEMP& cord#(T)): cord#(T)
```

`std::concat` moves two cords into one balanced result without copying leaf
elements. Both arguments are left empty. A call names the element type
explicitly: `std::concat#(I32)(@left MOVE(first), @right MOVE(second))`.

**Return value**: A cord that contains the elements of `left` followed by the
elements of `right`.

**Checked failures**: The conditions of [`concat`](#concat) apply.

## Implementation types

`cord_branch` and `cord_value`, and the functions `cord_take_branch`,
`cord_make_branch`, `cord_balance_children`, and `cord_join_balanced`, are
`PRIVATE(MODULE)`. A `cord_value` is a union with the options `empty`, `leaf`
(one `dynarr#(T)`), and `branch` (two nonempty subtrees with a cached element
count and height). Leaves are never merged: each concatenation of two nonempty
cords adds one branch.

## Example

The program joins three byte cords and flattens the result.

```quxlang
--8<-- "example-programs/reference-programs/cord_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/cord_example/expected-output.txt"
```

## See also

- [`std::dynarr`](dynarr.md)
- [`std::string`](string.md)
- [`std::avl_tree`](avl-tree.md)
- [Move Semantics](../../reference/move-semantics.md)
- [Unions](../../reference/unions.md)
