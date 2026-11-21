# `std::sqrt`

Module `std` · source file `math.qxs`

`std::sqrt` computes the correctly rounded square root of a binary64 value.
The function is implemented entirely with Quxlang integer and floating-point
arithmetic: the implementation calls no system mathematics routine, contains
no assembly, and reads no floating-point environment. The source file
`math.qxs` declares no other public entity. The language keyword `SQRT`, which
is specified in
[Runtime Module Contracts](../../reference/runtime-module-contracts.md#square-root),
accepts every floating-point format and permits the backend to substitute a
hardware instruction. [`std::big_integer`](big-integer.md) provides
arbitrary-precision integer arithmetic.

## Function summary

| Function | Description |
| --- | --- |
| [`sqrt`](#sqrt) | Returns the binary64 square root, rounded to nearest with ties to even. |

## Functions

### `sqrt`

```quxlang
::sqrt FUNCTION(@ARG:value F64): F64
```

The function returns the binary64 value that is nearest to the exact square
root of `value`. The rounding mode is round-to-nearest with ties to even. The
exact square root of a binary64 value is never halfway between two adjacent
binary64 values, so the tie rule never determines a result.

**Parameters**: `value` is the operand. Subnormal operands are supported.

**Return value**: The result depends on the class of `value`.

| Operand | Result |
| --- | --- |
| Positive zero | Positive zero. |
| Negative zero | Negative zero. |
| Positive finite value, including a subnormal value | The correctly rounded square root. |
| Positive infinity | Positive infinity. |
| Negative nonzero value, including negative infinity | The canonical NaN, whose binary64 encoding is `0x7FF8000000000000`. |
| NaN | The canonical NaN. |

**Exceptions**: The function throws no exception and reports an invalid
operand through the NaN result.

**Complexity**: The function decodes the operand into an integer significand
and an exponent, performs six Newton iterations in binary64 arithmetic to
obtain an estimate, and then corrects the estimate with exact 128-bit integer
squares and remainders. The normalization of a subnormal operand shifts the
significand by at most 52 bit positions. The function allocates no memory.

**Target restrictions**: The function is declared for every target. The tests
of the function are marked as known to be broken on JVM targets
(`KNOWN_BROKEN_IF(ARCH_IS_JVM)`).

## Example

```quxlang
--8<-- "example-programs/reference-programs/math_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/math_example/expected-output.txt"
```

## See also

- [`std::big_integer`](big-integer.md)
- [`std::fmt`, `std::printf`, and `std::print`](format.md)
- [Runtime Module Contracts: square root](../../reference/runtime-module-contracts.md#square-root)
- [Primitive Types and Literals](../../reference/primitive-types-and-literals.md)
