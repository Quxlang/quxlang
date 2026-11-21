# `io::buffer_ref` and `io::transfer_result`

Module `io` · source file `filesystem.qxs`

`io::buffer_ref` is a non-owning reference to a contiguous range of mutable
bytes. `io::transfer_result` reports the outcome of one transfer into or out of
such a range. Every read and write operation of the `io` module and of the
[`net`](../net/index.md) module accepts a `buffer_ref`. The file operations of
[`io::file_ref`](file-ref.md), [`io::file`](file.md),
[`io::async_context`](async-context.md), and
[`io::async_runtime`](async-runtime.md) report a `transfer_result`.

## Declaration

```quxlang
::buffer_ref STRUCT
::transfer_result STRUCT
```

Both structures are declared on every target, carry no struct modifier, and are
copyable. Copying a `buffer_ref` copies the pointer and the size; the copy
refers to the same storage.

## `io::buffer_ref`

### Data members

```quxlang
.data VAR MUT=>>BYTE;
.size VAR SZ;
```

| Member | Description |
| --- | --- |
| `data` | Pointer to the first byte of the referenced range. |
| `size` | Number of bytes in the referenced range. |

The referenced range is `data[0]` through `data[size - 1]`. Both members are
public and assignable.

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@data MUT=>>BYTE, @size SZ)
```

Creates a reference to the `size` bytes that begin at `data`.

**Parameters**: `data` points to the first byte of the range. `size` is the
number of bytes in the range. A `size` of zero denotes an empty range.

**Preconditions**: the storage that `data` designates contains at least `size`
bytes. The storage remains alive for every operation that uses the reference;
for an asynchronous operation, the storage remains alive until the operation
completes. The constructor performs no validation.

The source declares no other constructor. The structure also supports default
initialization, which the `io` and `net` modules use for members and local
variables of type `buffer_ref`; a default-initialized reference is assigned
before use.

A write operation reads the referenced bytes and does not modify them. The
`data` member is nevertheless a mutable pointer, so a write operation requires
a pointer to mutable storage.

## `io::transfer_result`

### Data members

```quxlang
.size VAR SZ;
.succeeded VAR BOOL;
```

| Member | Description |
| --- | --- |
| `size` | Number of bytes transferred. The value is zero when `succeeded` is `FALSE`, except as stated under [Target restrictions](#target-restrictions). |
| `succeeded` | `TRUE` when the native transfer completed without an error. |

A default-initialized `transfer_result` has `size` equal to zero and `succeeded`
equal to `FALSE`. This value is the unsuccessful result that a closed
[`io::file`](file.md) reports.

A successful transfer can move fewer bytes than the buffer holds. A successful
read of zero bytes into a nonempty buffer denotes end of file. A caller that
requires the complete range repeats the operation with a `buffer_ref` that
covers the bytes not yet transferred.

## Target restrictions

On Windows, the synchronous `read` and `write` functions of
[`io::file_ref`](file-ref.md) store the byte count that the native call reports
in `size` without resetting the count on failure. On Linux and macOS, the same
functions set `size` to zero whenever `succeeded` is `FALSE`.

## Example

The program reads from `/dev/zero` into the first two bytes of a four-byte
array and then reads through a closed owner.

```quxlang
--8<-- "example-programs/reference-programs/transfer_buffer/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/transfer_buffer/expected-output.txt"
```

## See also

- [`io::file_ref`](file-ref.md)
- [`io::file`](file.md)
- [`io::async_runtime`](async-runtime.md)
- [`std::buffered_writer` and `std::buffered_reader`](../std/buffered-io.md)
- [Pointers](../../reference/pointers.md)
