# `std::fmt`, `std::printf`, and `std::print`

Module `std` · source file `format.qxs`

The formatting functions convert values to text according to a format string
that is a template argument. `std::fmt` returns the text as a
[`std::string`](string.md), `std::printf` writes the text to standard output,
and `std::print` writes one unformatted string to standard output. The format
string is parsed during compilation: every field, every argument, and every
modifier is validated before code is generated, and a mismatch is a
compilation error. The generated code contains no format-string interpreter;
each field is lowered to a direct call for the static type of its argument.

## Function summary

| Function | Description |
| --- | --- |
| [`fmt`](#fmt) | Returns formatted text as an owned string. |
| [`printf`](#printf) | Writes formatted text to standard output and flushes. |
| [`print`](#print) | Writes a string constant or an owned string to standard output and flushes. |

## Functions

### `fmt`

```quxlang
::fmt TEMPLATE(@T:text VALUE STRING_CONSTANT)
  FUNCTION(%...args CONST& AUTO, @KWARGS ...): string
```

The function formats `text` with the supplied arguments and returns the result.

**Template parameters**: `text` is the [format string](#format-string). The
template argument must be a compile-time `STRING_CONSTANT`: a string literal,
as in `std::fmt#("{} items")(% [count])`, or a `STATIC` constant. A runtime
variable of type `STRING_CONSTANT` is rejected.

**Parameters**: `args` is the positional argument pack, written as a
`% [...]` group. `KWARGS` receives every named argument, written as
`@name value`. Each argument is evaluated exactly once, in the order in which
the call lists the arguments, regardless of the order or the number of the
fields that select the argument.

**Return value**: A `std::string` that contains the formatted bytes.

**Compilation errors**: The conditions listed under
[Compile-time validation](#compile-time-validation) apply.

**Target restrictions**: The function has no target restriction and is also
usable during compile-time evaluation.

### `printf`

```quxlang
::printf TEMPLATE(@T:text VALUE STRING_CONSTANT)
  FUNCTION(%...args CONST& AUTO, @KWARGS ...)
```

The function formats `text` with the supplied arguments, writes the result to
standard output, and flushes the output before the function returns. The
function appends no newline. The template parameter, the parameters, and the
compile-time validation are identical to those of [`fmt`](#fmt). The formatted
bytes pass directly to the output writer; the function creates no intermediate
string.

**Exceptions**: An output failure throws the `I32` value `1`.

**Target restrictions**: The function requires a standard output interface. On
targets other than the JVM, the function uses
[`std::standard_output`](buffered-io.md#standard_output), which is declared for
Linux, macOS, and Windows targets. On JVM targets, the function writes to the
Java standard output stream.

### `print`

```quxlang
::print FUNCTION(@ARG:text STRING_CONSTANT)                                                    // (1)
::print FUNCTION(@ARG:text CONST& AUTO(text_type)) ENABLE_IF(SAME_TYPES(text_type, string))    // (2)
```

1. Writes the bytes of a string constant to standard output.
2. Writes the bytes of a `std::string` to standard output.

Both overloads flush the output before returning and append no newline. The
text is not interpreted as a format string; braces are written unchanged.

**Exceptions**: An output failure throws the `I32` value `1`.

**Target restrictions**: The restrictions of [`printf`](#printf) apply.

## Format string

A format string is a sequence of literal bytes and replacement fields.

```text
format-string  = { literal-byte | "{{" | "}}" | field } ;
field          = "{" [ selector ] [ ":" [ "." digits ] [ style ] ] "}" ;
selector       = digits | name ;
style          = "d" | "b" | "B" | "o" | "x" | "X" | "f" | "F" | "e" | "E" ;
```

A literal byte other than `{` and `}` is copied to the output unchanged,
including a zero byte. The sequence `{{` produces one `{`, and the sequence
`}}` produces one `}`.

### Argument selection

The selector of a field determines the argument that the field formats. A
selector extends to the first `:` or `}` and must not contain `{`.

| Selector | Selected argument |
| --- | --- |
| Empty, as in `{}` | The next positional argument. The first such field selects index zero, and each further field selects the next index. |
| Decimal digits, as in `{1}` | The positional argument with that zero-based index. |
| Any other text, as in `{name}` | The named argument whose name equals the selector exactly. |

A format string must not combine empty selectors with decimal selectors. Named
selectors may be combined with either positional form. One argument may be
selected by several fields. Every positional argument and every named argument
must be selected by at least one field.

### Default formatting

A field without a style formats its argument according to the static type of
the argument. A type that is absent from the following table is not
formattable.

| Argument type | Output |
| --- | --- |
| `STRING_CONSTANT` | The bytes of the string, unchanged. |
| `std::string` | The bytes of the string, unchanged. |
| `BOOL` | `true` or `false`. |
| Integral type of any width (`IS_INTEGRAL`) | Decimal digits, preceded by `-` for a negative value. |
| Numeric literal | The exact decimal spelling of the constant; the literal `1.50` is written as `1.50`. |
| `F32`, `F64` | Decimal notation with six fractional digits. A nonzero value with a magnitude below 0.0001, or with a magnitude of at least 10^16, is written in scientific notation with six fractional digits and an exponent that carries a sign and no padding, as in `1.000000e+30`. |

A NaN is written as `nan` without a sign. An infinity is written as `inf` or
`-inf`. A negative zero is written with its sign, as in `-0.000000`.

### Integer styles

An integer style requires an argument of an integral type or an integer literal.

| Style | Output |
| --- | --- |
| `d` | Decimal. |
| `b`, `B` | Binary. |
| `o` | Octal. |
| `x` | Hexadecimal with the digits `a` through `f`. |
| `X` | Hexadecimal with the digits `A` through `F`. |

The output carries no radix prefix and no padding. A negative value is written
as `-` followed by the digits of its magnitude; the minimum value of a signed
type is formatted correctly. An integer literal is converted exactly,
independently of the width of any machine integer type.

### Floating-point styles

A floating-point style requires an argument of type `F32` or `F64`, or a
numeric literal. A numeric literal is converted to `F64` before formatting.

| Style | Output |
| --- | --- |
| `f` | Fixed notation: integer digits, a decimal point, and the fractional digits. |
| `F` | Fixed notation; an infinity is written as `INF` and a NaN as `NAN`. |
| `e` | Scientific notation: one integer digit, the fractional digits, `e`, an exponent sign, and at least two exponent digits, as in `1.250000e+01`. |
| `E` | Scientific notation with `E`; an infinity is written as `INF` and a NaN as `NAN`. |

The optional precision `.digits` precedes the style and specifies the number
of fractional digits, from zero through eighteen. The default precision is six.
With a precision of zero, the output contains no decimal point. A precision
requires one of the four floating-point styles.

The conversion is exact: the function computes the decimal digits from the
binary value with integer arithmetic and rounds the final digit to the nearest
value. A value that lies exactly halfway between two candidates is rounded
away from zero, so `-2.5` with `{:.0f}` produces `-3`.

### Compile-time validation

Each of the following conditions is a compilation error.

- A field selects a positional index that is not supplied, or a name that
  matches no named argument.
- A positional argument or a named argument is selected by no field.
- The format string combines empty selectors with decimal selectors.
- A field is not terminated by `}`, a `}` appears without a matching `{` and
  is not doubled, or a selector contains `{`.
- A style is not one of the ten listed styles, or a byte other than `}`
  follows the style.
- A precision has no digits, exceeds eighteen, or accompanies a style other
  than `f`, `F`, `e`, or `E`.
- An integral argument carries a floating-point style, or an `F32` or `F64`
  argument carries an integer style.
- A numeric literal whose decimal spelling is not an integer carries an
  integer style.
- An argument of type `STRING_CONSTANT`, `std::string`, or `BOOL` carries a
  style.
- An argument has a type that is not formattable.
- The format string is not a compile-time constant.

## Example

```quxlang
--8<-- "example-programs/reference-programs/format_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/format_example/expected-output.txt"
```

## See also

- [`std::string`](string.md)
- [`std::buffered_writer` and standard streams](buffered-io.md)
- [Call Arguments](../../reference/call-arguments.md)
- [Variadic Packs](../../reference/variadic-packs.md)
- [Templates](../../reference/templates-and-value-parameters.md)
- [Primitive Types and Literals](../../reference/primitive-types-and-literals.md)
