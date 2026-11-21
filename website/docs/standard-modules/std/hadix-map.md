# `std::hadix_map`

Module `std` · source file `hadix_map.qxs`

`std::hadix_map` owns unique keys and mapped values in a hash-radix table.
Each entry is a separately allocated record that never moves while it remains
in the map, so references to mapped values stay valid across insertion,
erasure of other keys, and bucket reorganization. A geometric bucket directory
indexes the records: each bucket holds fifteen unordered inline index entries
and an ordered overflow sequence. The table grows and shrinks by one bucket at
a time. `std::unordered_map` is an alias of `hadix_map`.
[`std::map`](map.md) provides the same lookup interface with ascending key
order.

## Declaration

```quxlang
::hadix_map TEMPLATE(@INDEX TYPE AUTO, @VALUE TYPE AUTO, @hasher_type TYPE AUTO) STRUCT    // (1)

::hadix_map TEMPLATE(@INDEX TYPE AUTO, @VALUE TYPE AUTO)
  ALIAS hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type default_hasher);               // (2)

::unordered_map TEMPLATE(@INDEX TYPE AUTO, @VALUE TYPE AUTO, @hasher_type TYPE AUTO)
  ALIAS hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type);                  // (3)

::unordered_map TEMPLATE(@INDEX TYPE AUTO, @VALUE TYPE AUTO)
  ALIAS hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type default_hasher);               // (4)
```

1. The principal template. `INDEX` is the key type, `VALUE` is the mapped
   type, and `hasher_type` is the hash policy.
2. The two-parameter form selects [`std::default_hasher`](default-hasher.md).
   The instantiation `std::hadix_map#[U64:U64]` names this form.
3. `std::unordered_map` with an explicit hash policy names the same type as
   form (1).
4. `std::unordered_map#[INDEX:VALUE]` names the same type as form (2).

The template parameters have the following requirements.

- `hasher_type` provides a member `hash` that accepts a constant key reference
  and returns `U64`. The map stores one hasher object. The default constructor
  of the map requires a default-constructible hasher.
- `INDEX` provides a three-way comparison `key <=> key` that yields `ORDER` and
  defines a total order. Keys that compare equal must have identical hashes.
  The hash and the comparison of a stored key must not change while the key is
  stored.
- With `std::default_hasher`, `INDEX` additionally provides `SERIALIZE`, and
  keys that compare equal must serialize to identical bytes.
- `VALUE` is default-constructible for [`OPERATOR[]`](#operator-index) and
  assignable for [`insert_or_assign`](#insert_or_assign).
- Destructors and the swap of two hasher objects must not throw.

The map declares `BEGIN`, `END`, `items`, and `IV_PAIRS` and therefore supports
`LOOP ITEM(item) IN(map)` and `LOOP INDEX(key) VALUE(value) IN(map)`. The map
declares no `INDEXES` or `VALUES` member. Iteration follows bucket order, which
is unrelated to key order and to insertion order.

A requirement that the source states with `INVARIANT` is checked when the
`INVARIANT_CHECKED` policy is enabled and is emitted as an optimizer assumption
otherwise. A violated requirement that is not checked results in undefined
behavior. [Compilation Policies](../../reference/compilation-policies.md)
specifies how an output selects policies.

**Target restrictions**: The test bundle annotates the unit tests of
`hadix_map` with `KNOWN_BROKEN_IF(ARCH_IS_JVM)`.

## Member types

| Type | Description |
| --- | --- |
| [`iterator`](#iterator-and-const_iterator) | Forward position that yields `map#[INDEX:VALUE]::item_ref`. |
| [`const_iterator`](#iterator-and-const_iterator) | Forward position that yields `map#[INDEX:VALUE]::const_item_ref`. |

## Member summary

**Construction and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty table, copies a table, or transfers a table. |
| [`OPERATOR:=`](#operator-assign) | Replaces the contents by copy or by transfer. |
| [`OPERATOR<->`](#operator-swap) | Exchanges the contents and the hashers of two tables. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of entries. |
| [`empty`](#empty) | Reports whether the table contains no entries. |
| [`bucket_count`](#bucket_count) | Returns the current number of buckets. |

**Lookup**

| Member | Description |
| --- | --- |
| [`contains`](#contains) | Reports whether a key is present. |
| [`find`](#find) | Returns an iterator for a key. |
| [`at`](#at) | Returns the mapped value for a key and throws when the key is absent. |
| [`OPERATOR[]`](#operator-index) | Returns the mapped value for a key and inserts a default value when the key is absent. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`insert`](#insert) | Inserts an entry when the key is absent. |
| [`insert_or_assign`](#insert_or_assign) | Inserts an entry or assigns the mapped value of an existing entry. |
| [`erase`](#erase) | Removes the entry for a key. |
| [`clear`](#clear) | Removes every entry. |

**Iteration**

| Member | Description |
| --- | --- |
| [`BEGIN`, `END`](#begin-and-end) | Return iterators to the first item and to the past-the-end position. |
| [`items`](#items) | Returns the table as its item range. |
| [`IV_PAIRS`](#iv_pairs) | Returns the table as its key/value pair range. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                                                                 // (1)
.CONSTRUCTOR FUNCTION(@hasher AUTO& hasher_type)                                                        // (2)
.CONSTRUCTOR FUNCTION(@OTHER CONST& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type))   // (3)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type))    // (4)
```

1. Creates an empty table with a default-constructed hasher. The table has no
   buckets.
2. Creates an empty table whose hasher is copied or moved from `hasher`
   according to the reference qualifier of the argument.
3. Copies the hasher of `OTHER` and inserts a copy of every entry of `OTHER`
   into independent records.
4. Copies the hasher of `OTHER` and transfers the buckets and records of
   `OTHER`. `OTHER` is left empty and retains its hasher. Record addresses are
   preserved.

**Exceptions**: (3) propagates an exception thrown by a copy constructor of
`INDEX` or `VALUE`. The records already copied are destroyed.

**Complexity**: (1), (2), and (4) are constant apart from the hasher copy. (3)
evaluates the hasher once for each entry of `OTHER`.

### `OPERATOR:=` {: #operator-assign }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)) MUT    // (1)
.OPERATOR:= FUNCTION(@OTHER TEMP& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)) MUT     // (2)
```

1. Constructs a copy of `OTHER` and then exchanges the copy with the current
   table. A failure during the copy leaves the current table unchanged.
2. Move-constructs a replacement from `OTHER`, exchanges the replacement with
   the current table, and destroys the previous contents. `OTHER` is left empty.
   Assignment from the same table object preserves its contents.

After either assignment the current table uses a copy of the hasher of `OTHER`.

### `OPERATOR<->` {: #operator-swap }

```quxlang
.OPERATOR<-> FUNCTION(@OTHER MUT& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)) MUT
```

The swap operator exchanges the hashers, the bucket directories, and the entry
counts of the two tables. Record addresses are preserved.

### `size`

```quxlang
.size FUNCTION() CONST: SZ
```

**Return value**: The number of entries.

### `empty`

```quxlang
.empty FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the table contains no entries.

### `bucket_count`

```quxlang
.bucket_count FUNCTION() CONST: SZ
```

**Return value**: The current number of buckets. An insertion adds one bucket
when the bucket count is less than `size() / 8 + 1`, evaluated before the new
entry is counted. An erasure removes one bucket when the bucket count exceeds
the number of buckets required for eight entries per bucket by more than two.

### `clear`

```quxlang
.clear FUNCTION() MUT
```

`clear` destroys all records and all buckets. The hasher is retained.

**Complexity**: Linear in `size()`.

### `contains`

```quxlang
.contains FUNCTION(@ARG:key CONST& INDEX) CONST: BOOL
```

**Return value**: `TRUE` when an entry with a key equal to `key` is present.

**Complexity**: One hasher evaluation and one bucket search. A bucket search
scans at most fifteen inline entries and then searches the ordered overflow
sequence of the bucket in time logarithmic in the length of that sequence.

### `find`

```quxlang
.find FUNCTION(@ARG:key CONST& INDEX) MUT: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::iterator            // (1)
.find FUNCTION(@ARG:key CONST& INDEX) CONST: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::const_iterator    // (2)
```

**Return value**: An iterator positioned at the entry whose key equals `key`,
or an iterator equal to `END()` when no such entry is present.

**Complexity**: One hasher evaluation and one bucket search. An empty table
returns `END()` without evaluating the hasher.

### `at`

```quxlang
.at FUNCTION(@ARG:key CONST& INDEX) MUT: MUT& VALUE        // (1)
.at FUNCTION(@ARG:key CONST& INDEX) CONST: CONST& VALUE    // (2)
```

**Return value**: A reference to the mapped value of `key`. Overload (1)
returns a mutable reference and overload (2) returns a constant reference.

**Exceptions**: The function throws
[`std::container_error::missing_key`](container-error.md) when `key` is absent.
The table is unchanged.

**Complexity**: One hasher evaluation and one bucket search.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%key AUTO& INDEX) MUT: MUT& VALUE
```

The subscript operator returns the mapped value of `key`. When `key` is absent,
the operator allocates a record whose key is constructed from the forwarded
`key` and whose mapped value is default-constructed, and inserts the record. A
temporary key argument is moved from only when the insertion occurs.

**Return value**: A mutable reference to the existing or newly inserted mapped
value.

**Exceptions**: The operator throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value. A failed allocation or
value construction preserves the existing entries; the bucket count may change.

**Iterator and reference validity**: An insertion may invalidate iterators.
References to existing mapped values remain valid.

### `insert`

```quxlang
.insert FUNCTION(@key AUTO& INDEX, @value AUTO& VALUE) MUT: BOOL
```

`insert` adds an entry when no entry with an equal key is present. The key and
the mapped value of the new record are constructed from the forwarded
arguments: a `CONST&` or `MUT&` argument is copied and a `TEMP&` argument is
moved. When the key is already present, the table is unchanged and neither
argument is consumed.

**Return value**: `TRUE` when the entry was inserted and `FALSE` when the key
was already present.

**Exceptions**: The function throws
[`std::container_error::size_overflow`](container-error.md) when an insertion
is required and `size()` equals the maximum `SZ` value. A failed allocation or
value construction preserves the existing entries; the bucket count may change.

**Complexity**: One hasher evaluation, one bucket search, and at most one
bucket split.

**Iterator and reference validity**: An insertion may invalidate iterators.
References to existing mapped values remain valid.

### `insert_or_assign`

```quxlang
.insert_or_assign FUNCTION(@key AUTO& INDEX, @value AUTO& VALUE) MUT: BOOL
```

`insert_or_assign` adds an entry when no entry with an equal key is present.
When the key is already present, the function assigns the forwarded `value` to
the existing mapped value and does not consume `key`.

**Return value**: `TRUE` when a new entry was inserted and `FALSE` when an
existing mapped value was assigned.

**Exceptions**: The conditions of [`insert`](#insert) apply.

**Iterator and reference validity**: An insertion may invalidate iterators.
References to existing mapped values remain valid.

### `erase`

```quxlang
.erase FUNCTION(@ARG:key CONST& INDEX) MUT: BOOL
```

`erase` removes the entry whose key equals `key` and destroys its record. The
function then joins at most one pair of buckets when the table holds more than
two buckets beyond the required count.

**Return value**: `TRUE` when an entry was erased and `FALSE` when `key` was
absent.

**Complexity**: One hasher evaluation, one bucket search, and at most one
bucket join. An empty table returns `FALSE` without evaluating the hasher.

**Iterator and reference validity**: An erasure may invalidate iterators.
References to the mapped values of the retained entries remain valid.

### `BEGIN` and `END`

```quxlang
.BEGIN FUNCTION() MUT: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::iterator            // (1)
.END FUNCTION() MUT: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::iterator              // (2)
.BEGIN FUNCTION() CONST: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::const_iterator    // (3)
.END FUNCTION() CONST: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::const_iterator      // (4)
```

`BEGIN` returns an iterator to the first item in bucket order. `END` returns
the past-the-end iterator. Both iterators are equal for an empty table.

### `items`

```quxlang
.items FUNCTION() MUT: MUT& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)        // (1)
.items FUNCTION() CONST: CONST& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)    // (2)
```

**Return value**: A reference to the table itself. `LOOP ITEM(item) IN(map)`
and `LOOP ITEM(item) IN(map.items())` bind `item` to an item reference with an
immutable key.

### `IV_PAIRS`

```quxlang
.IV_PAIRS FUNCTION() MUT: MUT& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)        // (1)
.IV_PAIRS FUNCTION() CONST: CONST& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)    // (2)
```

**Return value**: A reference to the table itself.
`LOOP INDEX(key) VALUE(value) IN(map)` iterates this range, binds `key` as
`CONST& INDEX`, and binds `value` as `MUT& VALUE` for a mutable table or as
`CONST& VALUE` for a constant table.

## `iterator` and `const_iterator`

```quxlang
::iterator STRUCT
::const_iterator STRUCT
```

Both types traverse the items in bucket order: the inline entries of each
bucket followed by its overflow entries. An iterator stores the address of its
owning table, a bucket index, and a position within the bucket. A structural
change of the table may invalidate iterators; references to retained records
remain valid. The constructor is private to module `std`, so user code obtains
iterators from [`BEGIN` and `END`](#begin-and-end) and [`find`](#find).
`const_iterator` declares no conversion from `iterator`.

```quxlang
.OPERATOR== FUNCTION(@OTHER CONST& hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::iterator) CONST: BOOL    // (1)
.OPERATOR++ FUNCTION() MUT: hadix_map#(@INDEX INDEX, @VALUE VALUE, @hasher_type hasher_type)::iterator                        // (2)
.OPERATOR-> FUNCTION() CONST: map#[INDEX:VALUE]::item_ref                                                                     // (3) iterator
.OPERATOR-> FUNCTION() CONST: map#[INDEX:VALUE]::const_item_ref                                                               // (3) const_iterator
```

Signatures (1) and (2) are shown for `iterator`. `const_iterator` declares the
same operators with its own type.

1. Returns `TRUE` when both iterators record the same table object and the same
   traversal position.
2. Advances to the next item and returns the previous iterator. The iterator
   must not be the past-the-end iterator; the requirement is stated with
   `INVARIANT`.
3. Returns an [item reference](map.md#item_ref-and-const_item_ref) by value.
   `key()` and `INDEX()` return the immutable key; `value()` and `VALUE()`
   return the mapped value. The iterator must not be the past-the-end iterator;
   the requirement is stated with `INVARIANT`.

## Implementation types

The nested types `boundary`, `record`, `index`, `probe`, `search_result`, and
`bucket`, and the nested function `reverse_hash`, are
`PRIVATE(MODULE, RUNTIME_MODULE)` and are not accessible to user code. A
`record` owns one key and its mapped value. A `bucket` holds fifteen inline
hash and record entries and a [`std::avl_tree`](avl-tree.md) of overflow
entries ordered by bit-reversed hash and then by key. The bucket directory is a
[`std::geometric_array`](geometric-array.md).

## Example

The program counts through the subscript operator, replaces a mapped value,
and iterates the key/value pairs.

```quxlang
--8<-- "example-programs/reference-programs/hadix_map_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/hadix_map_example/expected-output.txt"
```

## See also

- [`std::default_hasher`](default-hasher.md)
- [`std::map`](map.md)
- [`std::set`](set.md)
- [`std::container_error`](container-error.md)
- [Serialization](../../reference/serialization.md)
- [Comparison Operators](../../reference/comparison-operators.md)
- [`LOOP` Statements](../../reference/loop-statements.md)
