# Numeric Utilities

`std::big_integer` is an owning signed integer with variable-length
storage. It supports exact addition, subtraction, and multiplication.
`quotient` returns a `big_integer` rounded toward negative infinity.
`quotient_as#T` produces a checked result in the integral type `T`.
`to_string()` returns the full decimal representation.

[Compute a large byte-rate quotient](../examples/average-byte-rate.md)
demonstrates a quotient that exceeds the signed 64-bit range.

## `std::big_integer_error`

`division_by_zero` identifies a zero divisor. `quotient_out_of_range`
identifies a `quotient_as#T` result outside the range of `T`.

??? note "Declared signatures"

    ```quxlang
    ::big_integer_error ENUM [division_by_zero, quotient_out_of_range]
    ```

## `std::big_integer`

Owns a signed arbitrary-precision integer in base one billion.

??? note "Declared signatures"

    ```quxlang
    ::big_integer STRUCT
    ```

**`CONSTRUCTOR`**

Constructs zero.

Constructs an exact value from a signed or unsigned integral type of any
supported width. A decimal integer literal can initialize a `big_integer`
directly, including a literal wider than a machine integer.

??? note "Member signatures"

    ```quxlang
    .CONSTRUCTOR FUNCTION()

    .CONSTRUCTOR FUNCTION(@OTHER AUTO(integer_type)) ENABLE_IF(IS_INTEGRAL(integer_type))

    .CONSTRUCTOR FUNCTION(@OTHER NUMERIC_LITERAL_ANY(literal_type))
    ```

**`multiply`**

Multiplies by another `big_integer` or by a signed or unsigned integral
factor. Self-multiplication squares the value.

??? note "Member signatures"

    ```quxlang
    .multiply FUNCTION(@ARG:factor CONST& big_integer) MUT

    .multiply FUNCTION(@ARG:factor AUTO(factor_type)) MUT ENABLE_IF(IS_INTEGRAL(factor_type))
    ```

**`quotient`**

Returns the signed floor quotient as a `big_integer`. For example, dividing
−7 by 3 produces −3. A zero divisor throws `big_integer_error::division_by_zero`.

??? note "Member signatures"

    ```quxlang
    .quotient FUNCTION(@ARG:divisor CONST& big_integer) CONST: big_integer
    ```

**`to_string`**

Returns an owned decimal string. Negative values start with `-`, and zero
formats as `0`. The magnitude has no leading zeroes.

??? note "Member signatures"

    ```quxlang
    .to_string FUNCTION() CONST: string
    ```

**`quotient_as#T`**

Returns the floor quotient in the integral type `T`. For example,
`total.quotient_as#U64(divisor)` requests an unsigned 64-bit result.
A zero divisor throws `big_integer_error::division_by_zero`. A quotient
outside the range of `T` throws `big_integer_error::quotient_out_of_range`.

??? note "Member signatures"

    ```quxlang
    .quotient_as TEMPLATE(@T TYPE AUTO) ENABLE_IF(IS_INTEGRAL(T))
      FUNCTION(@ARG:divisor CONST& big_integer) CONST: T
    ```
