# Compile-time checksum lookup table

The program computes an eight-bit cyclic redundancy check of a nine-byte message with a lookup table
of 256 entries that the compiler evaluates. The program demonstrates namespace-scope [`STATIC`
constants](../overview/static-compile-time-constants.md),
[`STATIC_IF`](../overview/compile-time-evaluation.md), a [template value
parameter](../overview/templates-and-value-parameters.md), a [`STATIC_TEST`](../overview/tests.md),
and [bitwise operators](../overview/bitwise-operators.md).

```quxlang
--8<-- "example-programs/programs/checksum_lookup_table/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/checksum_lookup_table/expected-output.txt"
```

## Explanation

`polynomial` and `use_lookup_table` are namespace-scope [`STATIC`
constants](../reference/static-compile-time-constants.md), whose initializers are evaluated during
compilation. The function `table_entry` computes the checksum remainder of one byte value. Each
[bitwise operator](../reference/bitwise-operators.md) carries the `#` prefix: `#&&` computes the
bitwise conjunction, `#++` [shifts](../reference/bitwise-operators.md#shifts) bits toward higher
positions, and `#^^` computes the bitwise exclusive disjunction.

The function `build_table` fills an array of type `[256]U8`. The clause `INDEX(index)` of its
[iterator loop](../reference/loop-statements.md#range-projections) binds each zero-based index of
the array, and `AS CHECKED U8` converts the index with a range check. The constant `checksum_table`
is initialized with the call `build_table()`. The compiler executes that call during compilation and
emits the resulting array into the program.

`checksum` is a function template with the [value
parameter](../reference/templates-and-value-parameters.md#value-parameters) `byte_count`, which
determines the array type `[byte_count]U8` of the function parameter. The expression
`checksum#(@count 9)` selects the instantiation for nine bytes.
[`STATIC_IF`](../reference/compile-time-evaluation.md#static_if) evaluates its condition during
generation and generates only the selected body. The constant `use_lookup_table` is `TRUE`, so the
instantiation contains the table lookup and the body after `STATIC_ELSE` is not generated.

`standard_check_value` is a [`STATIC_TEST`](../reference/tests.md#static_test), which the compiler
evaluates. [`TEST_ASSERT`](../reference/tests.md#unconditional-test-checks) requires the checksum of
the byte values 49 through 57, which encode the digits `1` through `9`, to equal 244.

In `main`, the [function-local
`STATIC`](../reference/compile-time-evaluation.md#function-local-static-declarations) declaration
evaluates `expected` during compilation, and the declaration of `computed` calls the same function
during execution. The `x` [integer style](../standard-modules/std/format.md#integer-styles) prints
both values as the hexadecimal digits `f4`. The assignment `message[4] := 48` replaces the digit `5`
with the digit `0`, and the checksum of the modified message differs from `expected`.
