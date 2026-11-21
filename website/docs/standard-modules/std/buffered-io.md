# `std::buffered_writer`, `std::buffered_reader`, and standard streams

Module `std` · source file `buffered_io.qxs`

The buffered I/O facilities transfer bytes between a program and a file through
a fixed buffer of 16,384 bytes that is stored inside the buffering object.
`std::buffered_writer` collects output bytes and writes them to the file in
large transfers, and `std::buffered_reader` reads ahead from the file and
supplies single bytes and lines. Both types borrow an
[`io::file_ref`](../io/file-ref.md) and never close the file. The functions
`std::standard_output` and `std::standard_input` return file references for
the standard streams of the process. [`std::printf`](format.md) provides
formatted output without an explicit writer object.

## Summary

| Entity | Description |
| --- | --- |
| [`standard_output`](#standard_output) | Returns a file reference for the standard output of the process. |
| [`standard_input`](#standard_input) | Returns a file reference for the standard input of the process. |
| [`buffered_writer`](#buffered_writer) | Buffers output bytes for a borrowed file. |
| [`buffered_reader`](#buffered_reader) | Reads bytes and lines from a borrowed file. |

Every failure that these types report is thrown as the `I32` value `1`.

## Standard stream functions

### `standard_output`

```quxlang
::standard_output INCLUDE_IF(OS_MACOS || OS_LINUX)
  FUNCTION(): io::file_ref                              // (1)
::standard_output INCLUDE_IF(OS_WINDOWS)
  FUNCTION(): io::file_ref                              // (2)
```

1. Returns a file reference that borrows descriptor 1.
2. Returns a file reference for the handle that
   `syscall::windows::get_std_handle(@kind -11)` returns.

**Return value**: A non-owning [`io::file_ref`](../io/file-ref.md). The
reference does not close the stream when the reference is destroyed.

**Target restrictions**: The function is declared only when `OS_MACOS`,
`OS_LINUX`, or `OS_WINDOWS` is true. The function is not declared for JVM
targets.

### `standard_input`

```quxlang
::standard_input INCLUDE_IF(OS_MACOS || OS_LINUX)
  FUNCTION(): io::file_ref                              // (1)
::standard_input INCLUDE_IF(OS_WINDOWS)
  FUNCTION(): io::file_ref                              // (2)
```

1. Returns a file reference that borrows descriptor 0.
2. Returns a file reference for the handle that
   `syscall::windows::get_std_handle(@kind -10)` returns.

**Return value**: A non-owning [`io::file_ref`](../io/file-ref.md).

**Target restrictions**: The restrictions of
[`standard_output`](#standard_output) apply.

## `std::buffered_writer` { #buffered_writer }

```quxlang
::buffered_writer STRUCT MOVE_ONLY
```

A `buffered_writer` appends bytes to an internal buffer of 16,384 bytes and
transfers the buffer to the file when the buffer is full or when `flush` is
called. The type is move-only. The buffer is part of the object, so a writer
occupies more than 16 KiB of storage and performs no heap allocation.

The type declares no destructor. Bytes that remain in the buffer when the
writer is destroyed are discarded; a program calls [`flush`](#flush) before
the end of the lifetime of the writer to transfer them.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#buffered_writer-constructor) | Borrows a file. |
| [`flush`](#flush) | Writes all pending bytes to the file. |
| [`write_byte`](#write_byte) | Appends one byte. |
| [`write`](#write) | Appends a string constant or a byte array, or writes a byte range directly. |
| [`write_unsigned`](#write_unsigned) | Appends the decimal representation of a `U64`. |
| [`write_signed`](#write_signed) | Appends the decimal representation of an `I64`. |
| [`write_fixed`](#write_fixed) | Appends an `F64` in fixed decimal notation. |

### `CONSTRUCTOR` { #buffered_writer-constructor }

```quxlang
.CONSTRUCTOR FUNCTION(@EXPLICIT:file io::file_ref)
```

The constructor stores `file` and creates an empty buffer. The writer does not
assume responsibility for closing the file. The argument is named `EXPLICIT`:
a writer is constructed with
`VAR output std::buffered_writer :(@EXPLICIT std::standard_output());` or with
the explicit conversion `file AS std::buffered_writer`.

**Preconditions**: The file that `file` refers to remains open for as long as
the writer transfers bytes to it.

### `flush`

```quxlang
.flush FUNCTION() MUT
```

The function writes all pending bytes to the file. A native write that
transfers only part of the pending bytes is followed by further writes until
the buffer is empty.

**Exceptions**: The function throws the `I32` value `1` when a transfer fails
or transfers zero bytes. The bytes that were transferred before the failure
remain written, and the bytes that were not transferred remain in the buffer
for a subsequent `flush`.

### `write_byte`

```quxlang
.write_byte FUNCTION(@ARG:value BYTE) MUT
```

The function appends `value` to the buffer. The function flushes a full buffer
before it stores the byte, and flushes again when the stored byte fills the
buffer.

**Exceptions**: The function propagates the exception of [`flush`](#flush).
When the flush that precedes storage fails, `value` is not appended. When the
flush that follows storage fails, `value` remains in the buffer.

### `write`

```quxlang
.write FUNCTION(@ARG:text STRING_CONSTANT) MUT           // (1)
.write FUNCTION(@ARG:text CONST& dynarr#BYTE) MUT        // (2)
.write FUNCTION(@ARG:bytes CONST& io::buffer_ref) MUT    // (3)
```

1. Appends every byte of a string constant with [`write_byte`](#write_byte).
2. Appends every byte of a [`std::dynarr#BYTE`](dynarr.md) with
   [`write_byte`](#write_byte).
3. Flushes the pending bytes and then writes the byte range that `bytes`
   designates directly to the file, without copying the range into the buffer.
   Partial transfers are continued until the complete range is written.

**Parameters**: For (3), `bytes` is an
[`io::buffer_ref`](../io/buffer-ref.md) whose storage remains alive during the
call.

**Exceptions**: (1) and (2) propagate the exception of [`flush`](#flush).
(3) throws the `I32` value `1` when the initial flush fails, or when a
transfer of the range fails or transfers zero bytes.

### `write_unsigned`

```quxlang
.write_unsigned FUNCTION(@ARG:value U64) MUT
```

The function appends the decimal digits of `value` without leading zeroes.

**Exceptions**: The function propagates the exception of [`flush`](#flush).

### `write_signed`

```quxlang
.write_signed FUNCTION(@ARG:value I64) MUT
```

The function appends the decimal representation of `value`. A negative value
is preceded by `-`. The minimum value of `I64` is formatted correctly.

**Exceptions**: The function propagates the exception of [`flush`](#flush).

### `write_fixed`

```quxlang
.write_fixed FUNCTION(@value F64, @precision SZ) MUT
```

The function appends `value` in fixed decimal notation: an optional `-`, the
integer digits, and, when `precision` is nonzero, a decimal point followed by
exactly `precision` fractional digits.

**Parameters**: `value` is a finite value. `precision` is the number of
fractional digits, from zero through eighteen.

The function rounds the exact binary64 value to the requested number of
fractional digits. Halfway values round away from zero.
`write_fixed(@value 0.375, @precision 2)` appends `0.38`. A negative value
that rounds to zero retains its sign, as in `-0.00`.

**Exceptions**: The function throws the `I32` value `1` before appending any
bytes when `precision` exceeds eighteen, when `value` is nonfinite, or when
its magnitude is at least 2^64. The function also propagates the exception of
[`flush`](#flush).

## `std::buffered_reader` { #buffered_reader }

```quxlang
::buffered_reader STRUCT MOVE_ONLY
```

A `buffered_reader` reads up to 16,384 bytes from the file in one transfer and
supplies the bytes from its internal buffer. The type is move-only, stores the
buffer inside the object, and performs no heap allocation. The reader requests
more bytes from the file only when the buffer is exhausted.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#buffered_reader-constructor) | Borrows a file. |
| [`read_byte`](#read_byte) | Supplies the next byte. |
| [`read_line`](#read_line) | Supplies the bytes that precede the next linefeed. |

### `CONSTRUCTOR` { #buffered_reader-constructor }

```quxlang
.CONSTRUCTOR FUNCTION(@EXPLICIT:file io::file_ref)
```

The constructor stores `file` and creates an empty buffer. The reader does not
assume responsibility for closing the file. A reader is constructed with
`VAR input std::buffered_reader :(@EXPLICIT std::standard_input());`.

**Preconditions**: The file that `file` refers to remains open for as long as
the reader transfers bytes from it.

### `read_byte`

```quxlang
.read_byte FUNCTION(@ARG:value WRITE& BYTE) MUT: BOOL
```

The function stores the next byte of the file in `value`. When the buffer is
exhausted, the function first reads up to 16,384 bytes from the file.

**Parameters**: `value` receives the byte. At end of file, `value` receives
zero.

**Return value**: `TRUE` when a byte was supplied, and `FALSE` at end of file,
which is a successful transfer of zero bytes.

**Exceptions**: The function throws the `I32` value `1` when the read fails.

### `read_line`

```quxlang
.read_line FUNCTION(@ARG:line MUT& dynarr#BYTE) MUT: BOOL
```

The function clears `line` and then appends the bytes that precede the next
linefeed (byte value 10). The linefeed is consumed and is not stored. Any
other byte, including a carriage return, is stored unchanged.

**Parameters**: `line` is a [`std::dynarr#BYTE`](dynarr.md) that receives the
line. Its previous contents are discarded, and its allocation is reused.

**Return value**: `TRUE` when a linefeed terminated the line, or when end of
file terminated a line that contains at least one byte. `FALSE` when end of
file was reached before any byte was read; `line` is then empty. An empty line
that is terminated by a linefeed yields `TRUE` with an empty `line`.

**Exceptions**: The function propagates the exception of
[`read_byte`](#read_byte).

## Target restrictions

`buffered_writer` and `buffered_reader` carry no `INCLUDE_IF` condition; both
types depend on the `read` and `write` member functions of
[`io::file_ref`](../io/file-ref.md) for the target. `standard_output` and
`standard_input` are declared only for Linux, macOS, and Windows targets. The
unit test of partial-transfer recovery in `buffered_writer` runs only on macOS
targets.

## Example

```quxlang
--8<-- "example-programs/reference-programs/buffered_io_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/buffered_io_example/expected-output.txt"
```

## See also

- [`std::fmt`, `std::printf`, and `std::print`](format.md)
- [`std::dynarr`](dynarr.md)
- [`std::byte_regex`](byte-regex.md)
- [`io::file_ref`](../io/file-ref.md)
- [`io::buffer_ref` and `io::transfer_result`](../io/buffer-ref.md)
- [`io::io_descriptor_ref`](../io/io-descriptor-ref.md)
- [Target Availability](../../reference/availability-and-targets.md)
- [Exception Handling](../../reference/exceptions.md)
