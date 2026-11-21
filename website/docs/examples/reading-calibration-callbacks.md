# Reading calibration with lambdas and procedure pointers

The program adds a calibration offset to four temperature readings, which are stored in tenths of a
degree Celsius, sums the readings, and converts them to two other scales through a procedure pointer
that is selected during execution. The program demonstrates [lambdas](../overview/lambdas.md) with
value captures and reference captures, a deduced callable parameter, and [procedure
pointers](../overview/procedure-pointers-and-function-values.md).

```quxlang
--8<-- "example-programs/programs/reading_calibration_callbacks/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/reading_calibration_callbacks/expected-output.txt"
```

## Explanation

The `ALIAS` declaration gives the name `conversion` to the [procedure pointer
type](../reference/procedure-pointers-and-function-values.md#procedure-pointers)
`CONST->PROCEDURE(@ARG I32: I32)`. The postfix operator `<-` converts a free function to a procedure
pointer, and `select_conversion` returns one of two such pointers according to its argument. The
parentheses in `to_fahrenheit_tenths` are required: `+` binds more tightly than `/` in the [operator
precedence table](../reference/operator-precedence.md), so the unparenthesized expression
`celsius_tenths * 9 / 5 + 320` divides the product by 325.

The parameter `@action AUTO(callable)` of `for_each_reading`
[deduces](../reference/type-queries-and-deduction.md#deduction-patterns) the type of its argument,
so the function accepts each closure type. The [`ITEM` loop](../reference/arrays.md#iteration) binds
writable access to each element of the mutable array, and the call `action(reading)` supplies that
element as the single `@ARG` argument.

The lambda `calibrate` has the capture list `[=offset]`. A [by-value
capture](../reference/lambdas.md#explicit-capture-lists) stores an independent copy when the lambda
value is created, so the later assignment `offset := 1000` does not affect the closure, and each
reading increases by 3. The second lambda has no capture list and [captures `total` by
reference](../reference/lambdas.md#implicit-reference-capture); each call adds one reading to the
variable of `main`. The sum of the calibrated readings is 852.

The function `print_converted` receives a procedure pointer as an ordinary parameter and calls it
with one bare argument. The first call passes the pointer to `to_fahrenheit_tenths`, and the second
call passes the pointer to `to_kelvin_tenths`.
