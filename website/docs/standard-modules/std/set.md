# `std::set`

Module `std` · source file `ordered_containers.qxs`

`std::set` owns unique keys in ascending order. Each key occupies one
individually allocated node of a height-balanced binary tree, so lookup,
insertion, and removal have worst-case logarithmic complexity and no operation
moves or reassigns a retained key. Stored keys are immutable.
[`std::map`](map.md) associates a mutable value with each key and shares the
same tree implementation.

## Declaration

```quxlang
::set TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the key type. The set orders keys with `key <=> stored_key`, so `T` must
provide a three-way comparison that yields `ORDER`. Two keys are the same key
when the comparison yields `ORDER::EQUAL`. The comparison must define a
consistent total order for the stored keys. The set supports a `MOVE_ONLY` key
type through the forwarding parameter of [`insert`](#insert).

`set` declares no assignment operator and no swap operator. Assignment uses the
compiler-generated operation described in
[Assignment Operators](../../reference/assignment-operators.md). Copy
assignment produces an independent copy, and move assignment leaves the source
set empty.

A requirement that the source states with `INVARIANT` is checked when the
`INVARIANT_CHECKED` policy is enabled and is emitted as an optimizer assumption
otherwise. A violated requirement that is not checked results in undefined
behavior. [Compilation Policies](../../reference/compilation-policies.md)
specifies how an output selects policies.

**Target restrictions**: The test bundle annotates the unit tests of `set` with
`KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Forward position that yields `CONST& T`; obtained from a mutable set. |
| [`const_iterator`](#iterator-and-const_iterator) | Forward position that yields `CONST& T`; obtained from a constant set. |
| [`range`](#range-and-const_range) | Range of `iterator` positions. |
| [`const_range`](#range-and-const_range) | Range of `const_iterator` positions. |

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty set, copies a set, or transfers all keys. |
| [`DESTRUCTOR`](#destructor) | Destroys every key and releases every node. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of keys. |
| [`empty`](#empty) | Reports whether the set contains no keys. |

**Lookup**

| Member | Description |
| --- | --- |
| [`contains`](#contains) | Reports whether a key is present. |
| [`find`](#find) | Returns an iterator for a key. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`insert`](#insert) | Inserts a key when it is absent. |
| [`erase`](#erase) | Removes a key. |
| [`clear`](#clear) | Removes every key. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators to the least key and to the past-the-end position. |
| [`items`](#items) | Returns the key range. |
| [`VALUES`](#values) | Returns the key range for `LOOP VALUE` iteration. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                         // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& set#(T))    // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& set#(T))     // (3)
```

1. Constructs an empty set.
2. Copy-constructs every key of `OTHER`. The copy reproduces the tree topology
   of `OTHER`.
3. Transfers all keys of `OTHER` to the new set and leaves `OTHER` empty. No
   key is copied or moved.

**Exceptions**: (2) propagates an exception thrown by the copy constructor of
`T`. The nodes already copied are destroyed.

**Complexity**: (1) and (3) are constant. (2) is linear in `OTHER.size()`.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys every key and releases every node allocation.

**Complexity**: Linear in `size()`.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of keys.

**Complexity**: Constant.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the set contains no keys.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` removes and destroys every key.

**Complexity**: Linear in `size()`.

### `contains`

```quxlang
.contains FUNCTION(@key CONST& T) CONST: BOOL
```

**Parameters**: `key` is a named parameter; a call passes it as `@key`.

**Return value**: `TRUE` when a key equal to `key` is present.

**Complexity**: Logarithmic in `size()`.

### `find`

```quxlang
.find FUNCTION(@key CONST& T) MUT: set#(T)::iterator            // (1)
.find FUNCTION(@key CONST& T) CONST: set#(T)::const_iterator    // (2)
```

**Return value**: An iterator positioned at the stored key equal to `key`, or
an iterator equal to `END()` when no such key is present.

**Complexity**: Logarithmic in `size()`.

### `insert`

```quxlang
.insert FUNCTION(@ARG:key AUTO& T) MUT: BOOL            // (1)
.insert FUNCTION(@ARG:key T) MUT PRIORITY(-1): BOOL     // (2)
```

`insert` adds `key` when no equal key is present.

1. Copies a `CONST&` or `MUT&` argument and moves a `TEMP&` argument into the
   new node. When an equal key is already present, the set is unchanged and a
   temporary argument is not consumed.
2. Accepts an argument that requires an implicit conversion to `T`. The
   overload has `PRIORITY(-1)` and is selected only when overload (1) is not
   viable.

**Return value**: `TRUE` when the key was inserted and `FALSE` when an equal
key was already present.

**Exceptions**: The function throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `erase`

```quxlang
.erase FUNCTION(@key CONST& T) MUT: BOOL
```

`erase` removes the stored key equal to `key`, destroys it, and releases its
node. The removal relinks nodes and does not move or assign any retained key.

**Return value**: `TRUE` when a key was erased and `FALSE` when `key` was
absent.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: The operation invalidates iterators and
references to the erased key only.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: set#(T)::iterator            // (1)
.BEGIN FUNCTION() CONST: set#(T)::const_iterator    // (2)
.END FUNCTION() MUT: set#(T)::iterator              // (3)
.END FUNCTION() CONST: set#(T)::const_iterator      // (4)
```

`BEGIN` returns an iterator to the least key. `END` returns the past-the-end
iterator. Both iterators are equal for an empty set. `LOOP ITEM(key) IN(set)`
visits every key in ascending order.

**Complexity**: `BEGIN` is logarithmic in `size()`. `END` is constant.

### `items`

```quxlang
.items FUNCTION() MUT: set#(T)::range            // (1)
.items FUNCTION() CONST: set#(T)::const_range    // (2)
```

**Return value**: A range over the immutable keys in ascending order.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: set#(T)::range            // (1)
.VALUES FUNCTION() CONST: set#(T)::const_range    // (2)
```

**Return value**: A range over the immutable keys in ascending order.
`LOOP VALUE(key) IN(set)` iterates this range. The set declares no `INDEXES` or
`IV_PAIRS` member.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types store the address of one tree node, advance in ascending key order,
and yield a constant reference to the stored key. A default-constructed
iterator and a past-the-end iterator both store a null address and compare
equal. An iterator remains valid until its key is erased. The constructor that
takes a node is private to module `std`.

```quxlang
.CONSTRUCTOR FUNCTION()                                             // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& set#(T)::iterator)              // (2) const_iterator
.OPERATOR== FUNCTION(@OTHER CONST& set#(T)::iterator) CONST: BOOL   // (3)
.OPERATOR++ FUNCTION() MUT: set#(T)::iterator                       // (4)
.OPERATOR-> FUNCTION() CONST: CONST& T                              // (5)
```

Signatures (3) and (4) are shown for `iterator`. `const_iterator` declares the
same operators with `set#(T)::const_iterator` in place of `set#(T)::iterator`.

1. Creates an unpositioned iterator.
2. Converts an `iterator` to a `const_iterator` at the same position.
3. Returns `TRUE` when both iterators designate the same node.
4. Advances to the next greater key and returns the previous position. The
   iterator must designate a key; the requirement is stated with `INVARIANT`.
5. Returns a constant reference to the current key. The iterator must designate
   a key; the requirement is stated with `INVARIANT`.

**Complexity**: `OPERATOR++` is logarithmic in `size()` in the worst case. A
complete traversal is linear in `size()`.

## `range` and `const_range`

```quxlang
::range STRUCT
::const_range STRUCT
```

Each range type stores the address of the first node at the time of the call
that created the range. The constructor that takes a node is private to module
`std`.

```quxlang
.BEGIN FUNCTION() CONST: set#(T)::iterator          // range
.END FUNCTION() CONST: set#(T)::iterator            // range
.BEGIN FUNCTION() CONST: set#(T)::const_iterator    // const_range
.END FUNCTION() CONST: set#(T)::const_iterator      // const_range
```

`BEGIN` returns an iterator at the least key and `END` returns the
past-the-end iterator.

## Implementation types

`ordered_tree`, `ordered_tree_position`, and `ordered_set_entry` implement the
tree that `set` and [`std::map`](map.md) share. These declarations and the node
type `avl_node` are `PRIVATE(MODULE, RUNTIME_MODULE)` and are not accessible to
user code.

## Example

The program inserts keys, repeats an insertion, erases a key, and iterates the
remaining keys in ascending order.

```quxlang
--8<-- "example-programs/reference-programs/set_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/set_example/expected-output.txt"
```

## See also

- [`std::map`](map.md)
- [`std::hadix_map`](hadix-map.md)
- [`std::avl_tree`](avl-tree.md)
- [`std::container_error`](container-error.md)
- [Deduplicate station codes](../../examples/deduplicate-stations.md)
- [Comparison Operators](../../reference/comparison-operators.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
