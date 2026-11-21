# `std::map`

Module `std` · source file `ordered_containers.qxs`

`std::map` owns unique keys and their associated values in ascending key order.
Each entry occupies one individually allocated node of a height-balanced binary
tree, so lookup, insertion, and removal have worst-case logarithmic complexity
and no operation moves or reassigns a retained entry. The key of an entry is
immutable; the associated value is mutable. [`std::set`](set.md) stores keys
without associated values, and [`std::hadix_map`](hadix-map.md) provides
hash-based lookup without key ordering.

## Declaration

```quxlang
::map TEMPLATE(@INDEX TYPE AUTO, @VALUE TYPE AUTO) STRUCT
```

`INDEX` is the key type and `VALUE` is the associated value type. An
instantiation is written `std::map#[INDEX:VALUE]`, for example
`std::map#[I32:I32]`, or with named arguments as
`std::map#(@INDEX I32, @VALUE I32)`.

The map orders keys with `key <=> stored_key`, so `INDEX` must provide a
three-way comparison that yields `ORDER`. Two keys are the same key when the
comparison yields `ORDER::EQUAL`. The comparison must define a consistent total
order for the stored keys. `VALUE` must be default-constructible for
[`OPERATOR[]`](#operator-index) and assignable for
[`insert_or_assign`](#insert_or_assign). The map supports `MOVE_ONLY` key and
value types through the forwarding parameters of the insertion functions.

`map` declares no assignment operator and no swap operator. Assignment uses the
compiler-generated operation described in
[Assignment Operators](../../reference/assignment-operators.md). Copy
assignment produces an independent copy, and move assignment leaves the source
map empty.

A requirement that the source states with `INVARIANT` is checked when the
`INVARIANT_CHECKED` policy is enabled and is emitted as an optimizer assumption
otherwise. A violated requirement that is not checked results in undefined
behavior. [Compilation Policies](../../reference/compilation-policies.md)
specifies how an output selects policies.

**Target restrictions**: The test bundle annotates the unit tests of `map` with
`KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member types

| Type | Description |
| --- | --- |
| [`item_ref`](#item_ref-and-const_item_ref) | Refers to one entry with an immutable key and a mutable value. |
| [`const_item_ref`](#item_ref-and-const_item_ref) | Refers to one entry with an immutable key and a constant value. |
| [`iterator`](#iterators) | Forward position that yields `item_ref`. |
| [`const_iterator`](#iterators) | Forward position that yields `const_item_ref`. |
| [`key_iterator`](#iterators) | Forward position that yields `CONST& INDEX`. |
| [`value_iterator`](#iterators) | Forward position that yields `MUT& VALUE`. |
| [`const_value_iterator`](#iterators) | Forward position that yields `CONST& VALUE`. |
| [`item_range`](#ranges) | Range of `iterator` positions. |
| [`const_item_range`](#ranges) | Range of `const_iterator` positions. |
| [`key_range`](#ranges) | Range of `key_iterator` positions. |
| [`value_range`](#ranges) | Range of `value_iterator` positions. |
| [`const_value_range`](#ranges) | Range of `const_value_iterator` positions. |

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty map, copies a map, or transfers all entries. |
| [`DESTRUCTOR`](#destructor) | Destroys every entry and releases every node. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of entries. |
| [`empty`](#empty) | Reports whether the map contains no entries. |

**Lookup**

| Member | Description |
| --- | --- |
| [`contains`](#contains) | Reports whether a key is present. |
| [`find`](#find) | Returns an iterator for a key. |
| [`at`](#at) | Returns the value for a key and throws when the key is absent. |
| [`OPERATOR[]`](#operator-index) | Returns the value for a key and inserts a default value when the key is absent. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`insert`](#insert) | Inserts an entry when the key is absent. |
| [`insert_or_assign`](#insert_or_assign) | Inserts an entry or assigns the value of an existing entry. |
| [`erase`](#erase) | Removes the entry for a key. |
| [`clear`](#clear) | Removes every entry. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return item iterators to the first entry and to the past-the-end position. |
| [`items`](#items) | Returns the item range. |
| [`INDEXES`](#indexes) | Returns the key range. |
| [`VALUES`](#values) | Returns the value range. |
| [`IV_PAIRS`](#iv_pairs) | Returns the key/value pair range. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                 // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& map#[INDEX:VALUE])  // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& map#[INDEX:VALUE])   // (3)
```

1. Constructs an empty map.
2. Copy-constructs every entry of `OTHER`. The copy reproduces the tree
   topology of `OTHER`.
3. Transfers all entries of `OTHER` to the new map and leaves `OTHER` empty. No
   entry is copied or moved.

**Exceptions**: (2) propagates an exception thrown by a copy constructor of
`INDEX` or `VALUE`. The nodes already copied are destroyed.

**Complexity**: (1) and (3) are constant. (2) is linear in `OTHER.size()`.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys every entry and releases every node allocation.

**Complexity**: Linear in `size()`.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of entries.

**Complexity**: Constant.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the map contains no entries.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` removes and destroys every entry.

**Complexity**: Linear in `size()`.

### `contains`

```quxlang
.contains FUNCTION(@key CONST& INDEX) CONST: BOOL
```

**Parameters**: `key` is a named parameter; a call passes it as `@key`.

**Return value**: `TRUE` when an entry with a key equal to `key` is present.

**Complexity**: Logarithmic in `size()`.

### `find`

```quxlang
.find FUNCTION(@key CONST& INDEX) MUT: map#(@INDEX INDEX, @VALUE VALUE)::iterator            // (1)
.find FUNCTION(@key CONST& INDEX) CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_iterator    // (2)
```

**Return value**: An iterator positioned at the entry whose key equals `key`,
or an iterator equal to `END()` when no such entry is present. Overload (1)
returns a mutable iterator and overload (2) returns a constant iterator.

**Complexity**: Logarithmic in `size()`.

### `at`

```quxlang
.at FUNCTION(@key CONST& INDEX) MUT: MUT& VALUE        // (1)
.at FUNCTION(@key CONST& INDEX) CONST: CONST& VALUE    // (2)
```

**Return value**: A reference to the value associated with `key`. Overload (1)
returns a mutable reference and overload (2) returns a constant reference.

**Exceptions**: The function throws
[`std::container_error::missing_key`](container-error.md) when `key` is absent.
The map is unchanged.

**Complexity**: Logarithmic in `size()`.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%key AUTO& INDEX) MUT: MUT& VALUE
```

The subscript operator returns the value associated with `key`. When `key` is
absent, the operator inserts a new entry whose key is constructed from `key`
and whose value is default-constructed. A temporary key argument is moved from
only when the insertion occurs.

**Return value**: A mutable reference to the existing or newly inserted value.

**Exceptions**: The operator throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `insert`

```quxlang
.insert FUNCTION(
    @key AUTO& INDEX,
    @value AUTO& VALUE
  ) MUT: BOOL
```

`insert` adds an entry when no entry with an equal key is present. The key and
the value of the new entry are constructed from the forwarded arguments: a
`CONST&` or `MUT&` argument is copied and a `TEMP&` argument is moved. When the
key is already present, the map is unchanged and neither argument is consumed.

**Return value**: `TRUE` when the entry was inserted and `FALSE` when the key
was already present.

**Exceptions**: The function throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `insert_or_assign`

```quxlang
.insert_or_assign FUNCTION(
    @key AUTO& INDEX,
    @value AUTO& VALUE
  ) MUT: BOOL
```

`insert_or_assign` adds an entry when no entry with an equal key is present.
When the key is already present, the function assigns the forwarded `value` to
the existing associated value and does not consume `key`.

**Return value**: `TRUE` only when a new entry was inserted. `FALSE` indicates
an assignment to an existing entry.

**Exceptions**: The function throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: No iterator or reference is invalidated.

### `erase`

```quxlang
.erase FUNCTION(@key CONST& INDEX) MUT: BOOL
```

`erase` removes the entry whose key equals `key`, destroys the entry, and
releases its node. The removal relinks nodes and does not move or assign any
retained entry.

**Return value**: `TRUE` when an entry was erased and `FALSE` when `key` was
absent.

**Complexity**: Logarithmic in `size()`.

**Iterator and reference validity**: The operation invalidates iterators and
references to the erased entry only.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::iterator            // (1)
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_iterator    // (2)
.END FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::iterator              // (3)
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_iterator      // (4)
```

`BEGIN` returns an iterator to the entry with the least key. `END` returns the
past-the-end iterator. Both iterators are equal for an empty map.
`LOOP ITEM(item) IN(map)` binds `item` to an `item_ref` or a `const_item_ref`
for each entry in ascending key order.

**Complexity**: `BEGIN` is logarithmic in `size()`. `END` is constant.

### `items`

```quxlang
.items FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::item_range            // (1)
.items FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_item_range    // (2)
```

**Return value**: A range over the entries in ascending key order. The range
yields the same positions as [`BEGIN` and `END`](#begin-and-end).

### `INDEXES`

```quxlang
.INDEXES FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::key_range
```

**Return value**: A range over the immutable keys in ascending order.
`LOOP INDEX(key) IN(map)` iterates this range.

### `VALUES`

```quxlang
.VALUES FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::value_range            // (1)
.VALUES FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_value_range    // (2)
```

**Return value**: A range over the associated values in ascending key order.
`LOOP VALUE(value) IN(map)` iterates this range. Overload (1) yields mutable
values and overload (2) yields constant values.

### `IV_PAIRS`

```quxlang
.IV_PAIRS FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::item_range            // (1)
.IV_PAIRS FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_item_range    // (2)
```

**Return value**: A range whose elements provide `.INDEX()` and `.VALUE()`, in
ascending key order. `LOOP INDEX(key) VALUE(value) IN(map)` iterates this
range and binds `key` as `CONST& INDEX`. Overload (1) binds `value` as
`MUT& VALUE` and overload (2) binds `value` as `CONST& VALUE`.

## `item_ref` and `const_item_ref`

```quxlang
::item_ref STRUCT
::const_item_ref STRUCT
```

An item reference stores the addresses of the key and the value of one entry.
The reference is valid while the entry remains in its container.
[`std::hadix_map`](hadix-map.md) iterators yield the same two types.

```quxlang
.CONSTRUCTOR FUNCTION(@key CONST& INDEX, @value MUT& VALUE)      // item_ref
.CONSTRUCTOR FUNCTION(@key CONST& INDEX, @value CONST& VALUE)    // const_item_ref
.key FUNCTION() CONST: CONST& INDEX
.INDEX FUNCTION() CONST: CONST& INDEX
.value FUNCTION() CONST: MUT& VALUE                              // item_ref
.VALUE FUNCTION() CONST: MUT& VALUE                              // item_ref
.value FUNCTION() CONST: CONST& VALUE                            // const_item_ref
.VALUE FUNCTION() CONST: CONST& VALUE                            // const_item_ref
```

`key` and `INDEX` return the immutable key. `value` and `VALUE` return the
associated value. The uppercase members implement the projection protocol of
`LOOP INDEX(...) VALUE(...)`.

## Iterators

```quxlang
::iterator STRUCT
::const_iterator STRUCT
::key_iterator STRUCT
::value_iterator STRUCT
::const_value_iterator STRUCT
```

Each iterator type stores the address of one tree node and advances in
ascending key order. A default-constructed iterator and a past-the-end iterator
both store a null address and compare equal. An iterator remains valid until
its entry is erased. The constructor that takes a node is private to module
`std`.

```quxlang
.CONSTRUCTOR FUNCTION()                                                                      // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& map#(@INDEX INDEX, @VALUE VALUE)::iterator)              // (2) const_iterator
.CONSTRUCTOR FUNCTION(@OTHER CONST& map#(@INDEX INDEX, @VALUE VALUE)::value_iterator)        // (2) const_value_iterator
.OPERATOR== FUNCTION(@OTHER CONST& map#(@INDEX INDEX, @VALUE VALUE)::iterator) CONST: BOOL   // (3)
.OPERATOR++ FUNCTION() MUT: map#(@INDEX INDEX, @VALUE VALUE)::iterator                       // (4)
.OPERATOR-> FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::item_ref                     // (5) iterator
.OPERATOR-> FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_item_ref               // (5) const_iterator
.OPERATOR-> FUNCTION() CONST: CONST& INDEX                                                   // (5) key_iterator
.OPERATOR-> FUNCTION() CONST: MUT& VALUE                                                     // (5) value_iterator
.OPERATOR-> FUNCTION() CONST: CONST& VALUE                                                   // (5) const_value_iterator
```

Signatures (3) and (4) are shown for `iterator`. Each of the five types
declares both operators with its own type in place of `iterator`.

1. Creates an unpositioned iterator. Every iterator type declares this
   constructor.
2. Converts a mutable iterator to the corresponding constant iterator at the
   same position.
3. Returns `TRUE` when both iterators designate the same node.
4. Advances to the entry with the next greater key and returns the previous
   position. The iterator must designate an entry; the requirement is stated
   with `INVARIANT`.
5. Returns the projection of the current entry: an item reference by value, the
   key, or the associated value. The iterator must designate an entry; the
   requirement is stated with `INVARIANT`.

**Complexity**: `OPERATOR++` is logarithmic in `size()` in the worst case. A
complete traversal is linear in `size()`.

## Ranges

```quxlang
::item_range STRUCT
::const_item_range STRUCT
::key_range STRUCT
::value_range STRUCT
::const_value_range STRUCT
```

Each range type stores the address of the first node at the time of the call
that created the range. The constructor that takes a node is private to module
`std`.

```quxlang
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::iterator                // item_range
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::iterator                  // item_range
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_iterator          // const_item_range
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_iterator            // const_item_range
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::key_iterator            // key_range
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::key_iterator              // key_range
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::value_iterator          // value_range
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::value_iterator            // value_range
.BEGIN FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_value_iterator    // const_value_range
.END FUNCTION() CONST: map#(@INDEX INDEX, @VALUE VALUE)::const_value_iterator      // const_value_range
```

`BEGIN` returns an iterator at the first entry and `END` returns the
past-the-end iterator.

## Implementation types

`ordered_tree`, `ordered_tree_position`, `ordered_map_entry`, and
`ordered_set_entry` implement the tree that `map` and [`std::set`](set.md)
share. These declarations and the node type `avl_node` are
`PRIVATE(MODULE, RUNTIME_MODULE)` and are not accessible to user code.

## Example

The program inserts entries, replaces a value, erases a key, iterates the
entries in key order, and handles a checked lookup of an absent key.

```quxlang
--8<-- "example-programs/reference-programs/map_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/map_example/expected-output.txt"
```

## See also

- [`std::set`](set.md)
- [`std::hadix_map`](hadix-map.md)
- [`std::avl_tree`](avl-tree.md)
- [`std::container_error`](container-error.md)
- [Comparison Operators](../../reference/comparison-operators.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
- [Exception Handling](../../reference/exceptions.md)
