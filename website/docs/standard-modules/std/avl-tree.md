# `std::avl_tree`

Module `std` · source file `avl_tree.qxs`

`std::avl_tree` owns a sequence of `T` objects in the individually allocated
nodes of a height-balanced binary tree. Every node caches the size of its
subtree, so the tree addresses elements by zero-based rank. Insertion at a
rank, erasure at a rank, indexed access, splitting, and concatenation each take
logarithmic time, and none of these operations moves an element: rotations,
splits, and joins preserve element addresses. The tree imposes no ordering on
its elements; [`lower_bound`](#lower_bound) supports a caller that keeps the
sequence sorted. [`std::map`](map.md) and [`std::set`](set.md) provide
key-ordered containers, and [`std::hadix_map`](hadix-map.md) uses `avl_tree`
for the overflow entries of its buckets.

## Declaration

```quxlang
::avl_tree TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the element type. The destructor of `T` must not throw. A combined
element count that `SZ` cannot represent causes
[`std::container_error::size_overflow`](container-error.md) to be thrown.

A requirement that the source states with `INVARIANT` is checked when the
`INVARIANT_CHECKED` policy is enabled and is emitted as an optimizer assumption
otherwise. A violated requirement that is not checked results in undefined
behavior. [Compilation Policies](../../reference/compilation-policies.md)
specifies how an output selects policies.

**Target restrictions**: The test bundle annotates the unit tests of `avl_tree`
with `KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Forward position that yields `MUT& T`. |
| [`const_iterator`](#iterator-and-const_iterator) | Forward position that yields `CONST& T`. |

## Member summary

**Construction and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty tree, transfers nodes, or copies a tree. |
| [`DESTRUCTOR`](#destructor) | Destroys all elements and releases their nodes. |
| [`OPERATOR:=`](#operator-assign) | Replaces the contents by copy or by transfer. |
| [`OPERATOR<->`](#operator-swap) | Exchanges the contents of two trees. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of elements. |
| [`empty`](#empty) | Reports whether the tree is empty. |
| [`height`](#height) | Returns the height of the tree. |

**Element access**

| Member | Description |
| --- | --- |
| [`OPERATOR[]`](#operator-index) | Returns a reference to the element at a rank. |
| [`lower_bound`](#lower_bound) | Returns the first rank whose element is not less than a key. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`insert`](#insert) | Inserts an element before a rank. |
| [`erase`](#erase) | Destroys the element at a rank. |
| [`split_off`](#split_off) | Transfers a suffix into a new tree. |
| [`append`](#append) | Transfers another tree to the end of this tree. |
| [`clear`](#clear) | Destroys all elements. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators to the first element and to the past-the-end position. |
| [`VALUES`](#values) | Returns the tree as its element range. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                           // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& avl_tree#T)    // (2)
.CONSTRUCTOR FUNCTION(@OTHER CONST& avl_tree#T)   // (3)
```

1. Creates an empty tree.
2. Transfers the nodes of `OTHER` and leaves `OTHER` empty. Element addresses
   are preserved.
3. Copy-constructs every element of `OTHER` into a new node. The copy has the
   same sequence order and the same balanced topology as `OTHER`.

**Exceptions**: (3) propagates an exception thrown by the copy constructor of
`T`. The nodes already copied are destroyed.

**Complexity**: (1) and (2) are constant. (3) is linear in `OTHER.size()`.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys all elements and releases their nodes through
[`clear`](#clear).

### `OPERATOR:=` {: #operator-assign }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& avl_tree#T) MUT    // (1)
.OPERATOR:= FUNCTION(@OTHER TEMP& avl_tree#T) MUT     // (2)
```

1. Constructs a copy of `OTHER` and then exchanges the copy with the current
   tree. The previous contents are destroyed with the temporary copy. A
   failure during the copy leaves the current tree unchanged.
2. Transfers the nodes of `OTHER` to the current tree, destroys the previous
   contents, and leaves `OTHER` empty. Assignment from the same tree object
   preserves its contents.

### `OPERATOR<->` {: #operator-swap }

```quxlang
.OPERATOR<-> FUNCTION(@OTHER MUT& avl_tree#T) MUT
```

The swap operator exchanges the root nodes of the two trees. No element is
moved.

**Complexity**: Constant.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of elements.

**Complexity**: Constant. The root node caches the element count.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the tree contains no elements.

### `height`

```quxlang
.height FUNCTION() CONST: SZ
```

**Return value**: The cached height of the tree, which is the number of nodes
on the longest path from the root to a leaf. The value is zero for an empty
tree.

### `insert`

```quxlang
.insert FUNCTION(@index SZ, @value AUTO& T) MUT: MUT& T
```

`insert` allocates a node, constructs its element from `value`, and links the
node so that the new element has rank `index`. Elements at ranks `index` and
above move one rank toward the end. An `index` equal to `size()` appends the
element. The tree is modified only after the node has been constructed.

**Parameters**: `index` is the rank that the new element receives. `value` is
copied when the argument is a `CONST&` or `MUT&` reference and is moved when
the argument is a `TEMP&` reference.

**Return value**: A mutable reference to the inserted element.

**Preconditions**: `index <= size()`. The requirement is stated with
`INVARIANT`.

**Exceptions**: The function throws `std::container_error::size_overflow` when
`size()` equals the maximum `SZ` value. An exception thrown by the constructor
of `T` propagates and leaves the tree unchanged.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `erase`

```quxlang
.erase FUNCTION(@ARG:index SZ) MUT
```

`erase` detaches the node at rank `index`, destroys its element, and releases
the node. The addresses of the retained elements are preserved.

**Preconditions**: `index < size()`. The requirement is stated with
`INVARIANT`.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: The operation invalidates iterators and
references to the erased element only.

### `split_off`

```quxlang
.split_off FUNCTION(@ARG:index SZ) MUT: avl_tree#T
```

`split_off` divides the sequence before rank `index`. The current tree retains
the elements at ranks `0` through `index - 1`.

**Return value**: A tree that owns the elements formerly at ranks `index`
through `size() - 1`, in the same order.

**Preconditions**: `index <= size()`. The requirement is stated with
`INVARIANT`.

**Complexity**: Logarithmic in `size()`. The operation allocates no memory and
moves no element.

**Iterator and reference validity**: Element addresses are preserved.

### `append`

```quxlang
.append FUNCTION(@ARG:other MUT& avl_tree#T) MUT
```

`append` transfers the complete sequence of `other` to the end of the current
tree and leaves `other` empty. The operation allocates no memory and moves no
element.

**Preconditions**: `other` is a different tree object. The requirement is
stated with `INVARIANT`.

**Exceptions**: The function throws `std::container_error::size_overflow` when
`size() + other.size()` exceeds the maximum `SZ` value. Both trees are
unchanged in that case.

**Complexity**: Logarithmic in the sizes of the two trees.

**Iterator and reference validity**: Element addresses are preserved.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` destroys all elements and releases all nodes.

**Complexity**: Linear in `size()`.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%index SZ) MUT: MUT& T        // (1)
.OPERATOR[] FUNCTION(%index SZ) CONST: CONST& T    // (2)
```

**Return value**: A reference to the element at rank `index`. Overload (1)
returns a mutable reference and overload (2) returns a constant reference.

**Preconditions**: `index < size()`. The requirement is stated with
`INVARIANT`.

**Complexity**: Logarithmic in `size()`.

### `lower_bound`

```quxlang
.lower_bound FUNCTION(@ARG:key CONST& AUTO) CONST: SZ
```

`lower_bound` performs a binary descent that evaluates `key <=> element` at
each visited node.

**Parameters**: `key` has any type for which `key <=> element` yields `ORDER`
for an element of type `T`.

**Return value**: The first rank whose element is not less than `key`. The
result is `size()` when every element is less than `key`.

**Preconditions**: The sequence is sorted in ascending order with respect to
the comparison, and the comparison between `key` and the elements is consistent
with that order. The requirement is not checked.

**Complexity**: Logarithmic in `size()`.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: avl_tree#(T)::iterator            // (1)
.END FUNCTION() MUT: avl_tree#(T)::iterator              // (2)
.BEGIN FUNCTION() CONST: avl_tree#(T)::const_iterator    // (3)
.END FUNCTION() CONST: avl_tree#(T)::const_iterator      // (4)
```

`BEGIN` returns an iterator to the element at rank zero. `END` returns the
past-the-end iterator. Both iterators are equal for an empty tree.
`LOOP ITEM(item) IN(tree)` visits every element in rank order.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: MUT& avl_tree#T        // (1)
.VALUES FUNCTION() CONST: CONST& avl_tree#T    // (2)
```

**Return value**: A reference to the tree itself, for element iteration with
`LOOP VALUE(value) IN(tree)`. The tree declares no `INDEXES` or `IV_PAIRS`
member.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types traverse the elements in rank order. An iterator stores the address
of a node, and the past-the-end iterator stores a null address. Erasure
invalidates the iterators to the erased element. A transfer of ownership may
invalidate an iterator. The constructor that takes a node is private to module
`std`, so user code obtains iterators from [`BEGIN` and `END`](#begin-and-end).
`const_iterator` declares no conversion from `iterator`.

```quxlang
.OPERATOR== FUNCTION(@OTHER CONST& avl_tree#(T)::iterator) CONST: BOOL    // (1)
.OPERATOR++ FUNCTION() MUT: avl_tree#(T)::iterator                        // (2)
.OPERATOR-> FUNCTION() CONST: MUT& T                                      // (3) iterator
.OPERATOR-> FUNCTION() CONST: CONST& T                                    // (3) const_iterator
```

Signatures (1) and (2) are shown for `iterator`. `const_iterator` declares the
same operators with `avl_tree#(T)::const_iterator` in place of
`avl_tree#(T)::iterator`.

1. Returns `TRUE` when both iterators designate the same node or are both
   past-the-end iterators.
2. Advances to the element at the next rank and returns the previous position.
   The iterator must not be the past-the-end iterator; the requirement is
   stated with `INVARIANT`.
3. Returns a reference to the current element. The iterator must not be the
   past-the-end iterator; the requirement is stated with `INVARIANT`.

## Implementation types

`avl_node` is the node type shared by `avl_tree`, [`std::map`](map.md),
[`std::set`](set.md), and [`std::hadix_map`](hadix-map.md). The declaration is
`PRIVATE(MODULE, RUNTIME_MODULE)` and is not accessible to user code. Its value
template parameters `track_parent`, `track_height`, and `track_subtree_size`
select whether a node stores a parent pointer, an absolute height, and a
subtree size. `avl_tree` instantiates the node with all three selections
enabled.

## Example

The program builds a sorted sequence, inserts at the rank reported by
`lower_bound`, splits the sequence, erases by rank, and rejoins the parts.

```quxlang
--8<-- "example-programs/reference-programs/avl_tree_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/avl_tree_example/expected-output.txt"
```

## See also

- [`std::map`](map.md)
- [`std::set`](set.md)
- [`std::hadix_map`](hadix-map.md)
- [`std::list`](list.md)
- [`std::container_error`](container-error.md)
- [Comparison Operators](../../reference/comparison-operators.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
