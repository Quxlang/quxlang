# `std::string`

Module `std` · source file `string.qxs`

`std::string` owns a contiguous, growable sequence of `BYTE` values in one
allocation obtained from the default allocator of the runtime module. The
string stores an explicit length and no terminator, so a zero byte is ordinary
content. The type is `STRINGLIKE`: a string literal converts implicitly to
`std::string`, and a `std::string` produced by compile-time evaluation converts
to `STRING_CONSTANT`. [`std::cord`](cord.md) with `BYTE` elements provides
concatenation without copying, and [`std::dynarr`](dynarr.md) provides the
general contiguous sequence.

## Declaration

```quxlang
::string STRUCT
  STRINGLIKE SERIALOID
```

`STRINGLIKE` declares that the serialized form of the type is a length-prefixed
byte sequence that the compiler can convert to a string constant, as specified
in [Stringlike Types](../../reference/stringlike-types.md). `SERIALOID` is the
struct modifier of the [serialization](../../reference/serialization.md)
protocol; `string` supplies explicit `SERIALIZE` and `DESERIALIZE` members.

`size`, `OPERATOR[]`, `OPERATOR==`, `OPERATOR<=>`, `OPERATOR+`, and
`SERIALIZE` are callable on a constant string.

Assignment and swap use compiler-generated operations. Copy assignment
produces an independent copy. Move assignment leaves the source string with a
size of zero. Self-assignment and self-swap preserve the contents. Bytes are
read by index through [`OPERATOR[]`](#operator-index).

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not evaluated
results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults for each build type.

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty string, copies a string constant, copies a string, or transfers an allocation. |
| [`DESTRUCTOR`](#destructor) | Releases the allocation. |

**Capacity**

| Member | Description |
| --- | --- |
| [`size`](#size) | Returns the number of bytes. |
| [`reserve`](#reserve) | Ensures a minimum capacity. |

**Element access**

| Member | Description |
| --- | --- |
| [`OPERATOR[]`](#operator-index) | Returns the byte at an index. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`push_back`](#push_back) | Appends one byte. |
| [`pop_back`](#pop_back) | Removes the final byte. |

**Operators**

| Member | Description |
| --- | --- |
| [`OPERATOR+`](#operator-plus) | Returns the concatenation of two strings. |
| [`OPERATOR==`](#operator-equal) | Compares two strings for equal content. |
| [`OPERATOR<=>`](#operator-order) | Orders strings lexicographically by unsigned byte value. |

**Serialization**

| Member | Description |
| --- | --- |
| [`SERIALIZE`](#serialize) | Writes the length prefix and the bytes. |
| [`DESERIALIZE`](#deserialize) | Replaces the contents from a length prefix and bytes. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                           // (1)
.CONSTRUCTOR FUNCTION(@OTHER STRING_CONSTANT)     // (2)
.CONSTRUCTOR FUNCTION(@OTHER CONST& string)       // (3)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& string)        // (4)
```

1. Creates an empty string without allocating storage.
2. Copies the bytes of a string constant. Because the parameter is named
   `@OTHER`, a string literal converts implicitly: `VAR text std::string :=
   "abc";` and `"abc" AS std::string` both select this constructor.
3. Copies the bytes of `OTHER` into a new allocation.
4. Transfers the allocation of `OTHER` and leaves `OTHER` empty with no
   allocation.

**Complexity**: (1) and (4) are constant. (2) and (3) are linear in the number
of bytes.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor destroys the stored bytes and releases the allocation when the
capacity is nonzero.

### `size`

```quxlang
.size FUNCTION(): SZ
```

**Return value**: The number of bytes in the string.

### `reserve`

```quxlang
.reserve FUNCTION(@ARG:capacity SZ)
```

`reserve` ensures that the allocation holds at least `capacity` bytes. The
function has no effect when `capacity` does not exceed the current capacity.
Otherwise the function allocates exactly `capacity` bytes, copies the stored
bytes, and releases the previous allocation. The size does not change.

**Complexity**: Linear in `size()` when a reallocation occurs.

### `push_back`

```quxlang
.push_back FUNCTION(@ARG:value BYTE)
```

`push_back` appends `value`. When the size equals the capacity, the function
first doubles the capacity, starting from one byte.

**Complexity**: Constant when the capacity suffices. A reallocation is linear
in `size()`.

### `pop_back`

```quxlang
.pop_back FUNCTION()
```

`pop_back` removes the final byte. The capacity does not change.

**Preconditions**: The string is not empty. The function performs no check.

### `OPERATOR+` {: #operator-plus }

```quxlang
.OPERATOR+ FUNCTION(@OTHER CONST& string): string
```

**Return value**: A new string that contains the bytes of the left operand
followed by the bytes of `OTHER`. Neither operand is modified.

**Complexity**: Linear in the combined size.

### `OPERATOR==` {: #operator-equal }

```quxlang
.OPERATOR== FUNCTION(@OTHER CONST& string): BOOL
```

**Return value**: `TRUE` when both strings have the same size and the same byte
at every index. A string literal operand converts to `string`, so
`text == "abc"` is valid.

**Complexity**: Linear in `size()` when the sizes are equal, and constant
otherwise.

### `OPERATOR[]` {: #operator-index }

```quxlang
.OPERATOR[] FUNCTION(%index SZ): BYTE
```

**Return value**: The byte at the zero-based `index`, by value. The operator
does not provide a reference to the stored byte.

**Checked failures**: The operator asserts with `ASSERT` that
`index < size()`.

### `OPERATOR<=>` {: #operator-order }

```quxlang
.OPERATOR<=> FUNCTION(@OTHER CONST& string) CONST: ORDER
```

The operator compares bytes in index order, interpreting each byte as an
unsigned value. The first unequal pair determines the result. If the shorter
string is a prefix of the longer string, the shorter string sorts first.
Strings with identical contents and lengths compare equal. The operator
examines at most the number of bytes in the shorter string.

### `SERIALIZE`

```quxlang
.SERIALIZE FUNCTION(@OUTPUT_ITERATOR:output AUTO(it)): it
```

`SERIALIZE` writes `size()` with `SERIALIZE_UINTANY`, which is the
[variable-length integer encoding](../../reference/integer-serialization.md),
and then writes every byte in index order.

**Return value**: The output iterator positioned after the final written byte.

### `DESERIALIZE`

```quxlang
.DESERIALIZE FUNCTION(@INPUT_ITERATOR:input AUTO(it)): it
```

`DESERIALIZE` reads a length with `DESERIALIZE_UINTANY`, removes the current
contents, reserves the length, and appends that number of bytes from the input.

**Return value**: The input iterator positioned after the final consumed byte.

## Nonmember functions

### `std::java_string_from_string`

```quxlang
::java_string_from_string INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(@value CONST& string): ~> RUNTIME_MODULE::java_string
```

The function serializes `value` into a Java byte buffer, positions the buffer
after the length prefix, and decodes the payload bytes with the UTF-8 charset
decoder.

**Return value**: An external reference to a Java string.

**Target restrictions**: The declaration exists only when `ARCH_IS_JVM` is
true.

### `std::string_from_java_string`

```quxlang
::string_from_java_string INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(@value ~> RUNTIME_MODULE::java_string): string
```

The function encodes the Java string with the UTF-8 charset encoder and
deserializes the encoded bytes into a `string`.

**Return value**: A `string` that holds the UTF-8 encoding of `value`.

**Checked failures**: The function asserts with `ASSERT` that `value` is not
null.

**Target restrictions**: The declaration exists only when `ARCH_IS_JVM` is
true.

## Implementation types

The source file declares five helper types with `INCLUDE_IF(ARCH_IS_JVM)`:
`jvm_counting_output_iterator`, `jvm_byte_buffer_output_slot`,
`jvm_byte_buffer_output_iterator`, `jvm_byte_buffer_input_iterator`, and
`jvm_prefixed_byte_buffer_input_iterator`. They adapt Java byte buffers to the
serialization iterator protocol for the two conversion functions above. The
declarations are public and exist only on JVM targets.

## Example

The program concatenates two strings, appends a byte, and inspects the result.

```quxlang
--8<-- "example-programs/reference-programs/string_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/string_example/expected-output.txt"
```

## See also

- [`std::cord`](cord.md)
- [`std::dynarr`](dynarr.md)
- [`std::default_hasher`](default-hasher.md)
- [`std::fmt`, `std::printf`, and `std::print`](format.md)
- [Stringlike Types](../../reference/stringlike-types.md)
- [Serialization](../../reference/serialization.md)
- [External Types](../../reference/external-types.md)
