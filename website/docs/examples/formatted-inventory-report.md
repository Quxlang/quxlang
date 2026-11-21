# Formatted inventory report

The program prints one inventory record with every argument-selection form and every style of the
standard format strings. The program demonstrates
[`std::printf`](../standard-modules/std/format.md#printf),
[`std::fmt`](../standard-modules/std/format.md#fmt), and
[`std::print`](../standard-modules/std/format.md#print).

```quxlang
--8<-- "example-programs/programs/formatted_inventory_report/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/formatted_inventory_report/expected-output.txt"
```

## Explanation

The [format string](../standard-modules/std/format.md#format-string) is the template argument that
follows `#`, and the compiler parses it during compilation. A field that selects no supplied
argument, an argument that no field selects, and a style that does not apply to the type of its
argument are compilation errors. The first three `std::printf` calls show the three forms of
[argument selection](../standard-modules/std/format.md#argument-selection). An empty field `{}`
selects the next argument of the [positional
group](../reference/call-arguments.md#positional-groups) `% [...]`, a field such as `{1}` selects a
positional argument by its zero-based index, and a field such as `{name}` selects the named argument
`@name`. One argument may be selected by several fields.

A field without a style applies the [default
formatting](../standard-modules/std/format.md#default-formatting) of the static type of its
argument: a `std::string` contributes its bytes, an integer is written in decimal notation, and a
`BOOL` value is written as `true` or `false`. The [integer
styles](../standard-modules/std/format.md#integer-styles) `d`, `x`, `X`, `o`, and `b` write the `U8`
value 172 in decimal, lowercase hexadecimal, uppercase hexadecimal, octal, and binary notation
without a radix prefix.

The [floating-point styles](../standard-modules/std/format.md#floating-point-styles) apply to the
`F64` value 12.5. The default formatting writes six fractional digits, `.2f` selects fixed notation
with two fractional digits, and `e` selects scientific notation. The field `{mass:.1E}` requests one
fractional digit; the value 1.25 lies halfway between two candidates and is rounded away from zero,
which produces `1.3E+01`. In the next call, the sequences `{{` and `}}` produce literal braces, and
the numeric literal `0.50` is written with its exact decimal spelling.

`std::fmt` formats with the same rules and returns the text as a
[`std::string`](../standard-modules/std/string.md). The program prints the size of that string and
the string itself through named fields. `std::print` writes a `std::string` or a string constant
without interpreting the text as a format string.
