# `std::big_integer`

Module `std` · source file `big_integer.qxs`

`std::big_integer` is an owning signed integer of arbitrary precision. The
magnitude is stored as a sequence of base-1,000,000,000 limbs in a
[`std::dynarr#U32`](dynarr.md), least significant limb first, together with a
sign flag; the storage grows as the value requires. Addition, subtraction,
multiplication, and floor division are exact for every magnitude. The
arithmetic operations are named member functions that modify the receiver;
the type declares no arithmetic operators and no comparison operators. The
enumeration `std::big_integer_error` identifies the failures of division.
[`std::sqrt`](math.md) is the other numeric facility of module `std`.

## Declaration

```quxlang
::big_integer_error ENUM [division_by_zero, quotient_out_of_range];

::big_integer STRUCT
```

`big_integer` carries no struct modifier and declares no copy constructor, move
constructor, assignment operator, or destructor. The compiler-generated
operations copy, move, assign, and destroy the limb array and the sign flag, so
a copy is an independent value. The representation is private. Zero is
represented by an empty limb array with a cleared sign flag; a negative zero
does not exist.

### `big_integer_error`

| Enumerator | Meaning |
| --- | --- |
| `division_by_zero` | The divisor of `quotient` or `quotient_as` is zero. |
| `quotient_out_of_range` | The floor quotient computed by `quotient_as#T` is outside the range of `T`. |

A division failure throws a value of type `big_integer_error`. A handler
catches the value with `CATCH error CONST& std::big_integer_error`.

### Target restrictions

The type is declared for every target. The tests that call `quotient_as` and
the tests of the division exceptions are marked as known to be broken on JVM
targets (`KNOWN_BROKEN_IF(ARCH_IS_JVM)`). The tests of `add`, `subtract`,
`multiply`, `quotient`, and `to_string` that involve neither `quotient_as` nor
an exception carry no such marking.

## Member summary

**Construction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Constructs zero, or an exact value from an integral value or a decimal integer literal. |

**Arithmetic**

| Member | Description |
| --- | --- |
| [`add`](#add) | Adds another `big_integer` to the receiver. |
| [`subtract`](#subtract) | Subtracts another `big_integer` from the receiver. |
| [`multiply`](#multiply) | Multiplies the receiver by a `big_integer` or by an integral value. |
| [`quotient`](#quotient) | Returns the floor quotient as a `big_integer`. |
| [`quotient_as`](#quotient_as) | Returns the floor quotient as a checked value of an integral type. |

**Conversion**

| Member | Description |
| --- | --- |
| [`to_string`](#to_string) | Returns the signed decimal representation. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                                                    // (1)
.CONSTRUCTOR FUNCTION(@OTHER AUTO(integer_type)) ENABLE_IF(IS_INTEGRAL(integer_type))      // (2)
.CONSTRUCTOR FUNCTION(@OTHER NUMERIC_LITERAL_ANY(literal_type))                            // (3)
```

1. Constructs zero.
2. Constructs the exact value of `OTHER`. `integer_type` is any signed or
   unsigned integral type of any width, including widths such as `I5`, `U128`,
   and `I128`. The minimum value of a signed type is converted exactly.
3. Constructs the exact value of a decimal integer literal. The literal may
   exceed the range of every machine integer type, and may be negative. The
   limbs are derived from the decimal spelling of the literal.

Overloads (2) and (3) take the argument `OTHER` and therefore permit
initialization of the forms `VAR value std::big_integer := count;` and
`VAR value std::big_integer := 70000000000000000000;`.

**Compilation errors**: For (3), a literal whose decimal spelling is not an
integer is rejected with the message
`A big_integer requires a decimal integer literal.`

**Complexity**: (1) is constant. (2) is linear in the number of limbs of the
value. (3) is linear in the number of digits of the literal.

### `add`

```quxlang
.add FUNCTION(@ARG:other CONST& big_integer) MUT
```

The function replaces the receiver with the sum of the receiver and `other`.
The operands may have any combination of signs. `other` may be the receiver
itself, in which case the function doubles the value.

**Complexity**: Linear in the larger limb count of the two operands. When the
signs differ and the magnitude of `other` exceeds the magnitude of the
receiver, the function also copies `other`.

### `subtract`

```quxlang
.subtract FUNCTION(@ARG:other CONST& big_integer) MUT
```

The function replaces the receiver with the difference of the receiver and
`other`. `other` may be the receiver itself, in which case the result is zero.

**Complexity**: The complexity of [`add`](#add) applies.

### `multiply`

```quxlang
.multiply FUNCTION(@ARG:factor CONST& big_integer) MUT                                         // (1)
.multiply FUNCTION(@ARG:factor AUTO(factor_type)) MUT ENABLE_IF(IS_INTEGRAL(factor_type))      // (2)
```

1. Replaces the receiver with the product of the receiver and `factor`.
   `factor` may be the receiver itself, in which case the function squares the
   value.
2. Replaces the receiver with the product of the receiver and an integral
   value of any width and signedness. A factor type of at most 32 bits is
   multiplied in place as one machine word. A wider factor is first converted
   to a `big_integer` and then multiplied by overload (1).

The sign of the product is negative exactly when the operands have different
signs and the product is nonzero.

**Complexity**: For (1), a `factor` of one limb is multiplied in time linear
in the limb count of the receiver. A `factor` of several limbs uses schoolbook
multiplication, whose time is proportional to the product of the two limb
counts, and allocates a separate result. For (2), a factor type of at most 32
bits takes time linear in the limb count of the receiver.

### `quotient`

```quxlang
.quotient FUNCTION(@ARG:divisor CONST& big_integer) CONST: big_integer
```

The function divides the receiver by `divisor` and rounds the exact quotient
toward negative infinity. The receiver is unchanged.

**Return value**: The floor quotient. For example, dividing −7 by 3 yields −3,
dividing 7 by −3 yields −3, and dividing −7 by −3 yields 2. The function
provides no remainder.

**Exceptions**: A zero `divisor` throws `big_integer_error::division_by_zero`.

**Complexity**: A divisor of one limb is divided in time linear in the limb
count of the receiver. A divisor of several limbs uses schoolbook long
division on normalized copies of both operands; the time is proportional to
the product of the limb count of the quotient and the limb count of the
divisor.

### `quotient_as`

```quxlang
.quotient_as TEMPLATE(@T TYPE AUTO) ENABLE_IF(IS_INTEGRAL(T))
  FUNCTION(@ARG:divisor CONST& big_integer) CONST: T
```

The function computes the floor quotient with [`quotient`](#quotient) and
converts the result to the integral type `T` with a range check.
`total.quotient_as#U64(divisor)` requests an unsigned 64-bit result. Dividing
by a `big_integer` whose value is 1 converts the receiver itself to `T`.

**Template parameters**: `T` is the result type. `T` must be an integral type;
any width and signedness is accepted.

**Return value**: The floor quotient as a `T`. The minimum value of a signed
`T` is a valid result.

**Exceptions**: A zero `divisor` throws `big_integer_error::division_by_zero`.
A floor quotient outside the range of `T`, including every negative quotient
when `T` is unsigned, throws `big_integer_error::quotient_out_of_range`.

**Target restrictions**: The restrictions stated under
[Target restrictions](#target-restrictions) apply.

### `to_string`

```quxlang
.to_string FUNCTION() CONST: string
```

**Return value**: A [`std::string`](string.md) that contains the decimal
digits of the value without leading zeroes. A negative value starts with `-`.
Zero is returned as `0`.

**Complexity**: Linear in the limb count.

## Example

The program computes 30 factorial, divides the result by a literal that
exceeds the 64-bit range, demonstrates floor rounding, and handles a quotient
that does not fit the requested type.

```quxlang
--8<-- "example-programs/reference-programs/big_integer_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/big_integer_example/expected-output.txt"
```

## See also

- [`std::sqrt`](math.md)
- [`std::dynarr`](dynarr.md)
- [`std::string`](string.md)
- [`std::fmt`, `std::printf`, and `std::print`](format.md)
- [Compute a large byte-rate quotient](../../examples/average-byte-rate.md)
- [Exception Handling](../../reference/exceptions.md)
- [Enums](../../reference/enums.md)
- [Arithmetic Operators](../../reference/arithmetic-operators.md)
