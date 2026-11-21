# Large byte-rate quotient

The program divides a byte total that exceeds the range of every 64-bit integer type by a duration
in seconds and prints the resulting rate. The program demonstrates
[`std::big_integer`](../standard-modules/std/big-integer.md) construction from an integer literal,
floor division, decimal formatting, and a typed [exception handler](../overview/exceptions.md).

```quxlang
--8<-- "example-programs/programs/average_byte_rate/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/average_byte_rate/expected-output.txt"
```

## Explanation

The declarations of `bytes` and `seconds` initialize two `std::big_integer` objects from decimal
integer literals. The [literal constructor](../standard-modules/std/big-integer.md#constructor)
derives the value from the decimal spelling of the literal, so the value `70000000000000000000`,
which exceeds the range of `U64`, is stored exactly.

[`quotient`](../standard-modules/std/big-integer.md#quotient) divides the receiver by its argument,
rounds the exact quotient toward negative infinity, and returns a new `std::big_integer`.
[`to_string`](../standard-modules/std/big-integer.md#to_string) returns a
[`std::string`](../standard-modules/std/string.md) that contains every decimal digit of the result,
and [`std::printf`](../standard-modules/std/format.md#printf) substitutes that string for the
`{rate}` field.

A zero divisor causes `quotient` to throw `std::big_integer_error::division_by_zero`. The [`TRY`
statement](../reference/exceptions.md) encloses the division, and its `CATCH` clause binds a
constant reference to a thrown `std::big_integer_error` value. The handler prints a replacement
message and returns a nonzero exit status. The divisor in this program is 3, so the handler does not
execute.
