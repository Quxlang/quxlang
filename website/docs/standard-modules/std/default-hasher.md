# `std::default_hasher`

Module `std` · source file `default_hasher.qxs`

`std::default_hasher` is the hash policy that
[`std::hadix_map`](hadix-map.md) and `std::unordered_map` select when no policy
is named. The policy hashes the byte stream that the `SERIALIZE` member of a
value emits. The bytes are consumed as they are produced, and no serialization
buffer is allocated. The hash is deterministic and is not cryptographic.

## Declaration

```quxlang
::default_hasher STRUCT
```

The struct has no data members and no declared constructors.

## Member summary

| Member | Description |
| --- | --- |
| [`hash`](#hash) | Returns a 64-bit hash of the serialized bytes of a value. |

## Member functions

### `hash`

```quxlang
.hash FUNCTION(@ARG:value CONST& AUTO) CONST: U64
```

`hash` calls `value.SERIALIZE(@OUTPUT_ITERATOR output)` with an output iterator
that folds each emitted byte into a 64-bit state, and then applies a final
avalanche. The computation proceeds as follows, with wrapping unsigned 64-bit
multiplication.

1. The state starts at `14695981039346656037`.
2. For each serialized byte, the state becomes the exclusive-or of the state
   and the byte, multiplied by `1099511628211`. This step is the 64-bit FNV-1a
   cycle.
3. The state becomes the exclusive-or of the state and the state shifted right
   by 30 bits, multiplied by `13787848793156543929`.
4. The state becomes the exclusive-or of the state and the state shifted right
   by 27 bits, multiplied by `10723151780598845931`.
5. The result is the exclusive-or of the state and the state shifted right by
   31 bits.

**Parameters**: `value` has any type that provides a `SERIALIZE` member
callable on a constant object with an `@OUTPUT_ITERATOR` argument.

**Return value**: The hash of every byte that `value.SERIALIZE` emits.

**Preconditions**: A key type used with this policy serializes equal keys to
identical bytes. A type whose equality differs from the equality of its
serialized bytes requires a custom hash policy.

**Complexity**: Linear in the number of serialized bytes.

## Custom hash policies

A custom policy is a type with a member `hash` that accepts a constant key
reference and returns `U64`. The policy is passed as the `hasher_type` argument
of `std::hadix_map` or `std::unordered_map`.

```quxlang
::identity_hash STRUCT
{
  .hash FUNCTION(@ARG:key CONST& U64) CONST: U64 { RETURN key; }
}
```

## Implementation types

The nested types `output_slot` and `output_iterator` are
`PRIVATE(MODULE, RUNTIME_MODULE)`. They implement the byte sink that
`SERIALIZE` writes to.

## Example

The program hashes three strings and compares the results.

```quxlang
--8<-- "example-programs/reference-programs/default_hasher_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/default_hasher_example/expected-output.txt"
```

## See also

- [`std::hadix_map`](hadix-map.md)
- [`std::string`](string.md)
- [Serialization](../../reference/serialization.md)
