# Buffered measurement report on standard output

The program writes a header to standard output with one direct write and then formats three
measurements through a buffered writer. The program demonstrates
[`std::standard_output`](../standard-modules/std/buffered-io.md#standard_output),
[`io::file_ref`](../standard-modules/io/file-ref.md), [`io::buffer_ref` and
`io::transfer_result`](../standard-modules/io/buffer-ref.md), and
[`std::buffered_writer`](../standard-modules/std/buffered-io.md#buffered_writer).

```quxlang
--8<-- "example-programs/programs/buffered_measurement_report/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/buffered_measurement_report/expected-output.txt"
```

## Explanation

`std::standard_output` returns a non-owning `io::file_ref` that borrows the standard output of the
process. The array `header` holds 14 `BYTE` values that the [positional
initializer](../reference/arrays.md#positional-initialization) supplies as character literals. The
`io::buffer_ref` object designates that storage through a pointer to the first element, which the
expression `header[&0]` produces, and through the element count that
[`SIZE`](../reference/arrays.md#element-count) returns. A buffer reference owns no storage.

[`write`](../standard-modules/io/file-ref.md#write) performs one native write and stores the outcome
in the `io::transfer_result` object that the call passes as `@result`. The function does not throw.
The program inspects the members `succeeded` and `size` and returns the exit status 1 when the write
failed or transferred fewer than 14 bytes.

The [constructor](../standard-modules/std/buffered-io.md#buffered_writer-constructor) of
`std::buffered_writer` receives the file reference as the argument named `EXPLICIT` and borrows the
file. The writer appends bytes to an internal buffer of 16,384 bytes. Inside the [numeric sequence
loop](../reference/loop-statements.md#numeric-sequence-loops),
[`write`](../standard-modules/std/buffered-io.md#write) appends a string constant,
[`write_unsigned`](../standard-modules/std/buffered-io.md#write_unsigned) and
[`write_signed`](../standard-modules/std/buffered-io.md#write_signed) append decimal integers,
[`write_fixed`](../standard-modules/std/buffered-io.md#write_fixed) appends an `F64` value with
exactly three fractional digits, and
[`write_byte`](../standard-modules/std/buffered-io.md#write_byte) appends the line terminator. The
[conversion](../reference/conversions.md#modes) `AS APPROXIMATE F64` converts each `I64` offset to a
floating-point value and permits an inexact result.

`std::buffered_writer` declares no destructor, and bytes that remain in the buffer when the writer
is destroyed are discarded. The call to [`flush`](../standard-modules/std/buffered-io.md#flush)
transfers the three buffered lines to the file. The final
[`std::printf`](../standard-modules/std/format.md#printf) call writes its line after the flush, so
the output appears in the order of the program statements.
