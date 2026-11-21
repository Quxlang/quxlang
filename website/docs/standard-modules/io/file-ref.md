# `io::file_ref`

Module `io` · source file `filesystem.qxs`

`io::file_ref` is a non-owning reference to an open native file. The type
provides synchronous cursor-based transfers, an explicit `close` operation, and
coroutine overloads that suspend the calling coroutine while an
[`io::async_runtime`](async-runtime.md) performs the transfer. Copies refer to
the same native file, and destruction performs no native operation.
[`io::file`](file.md) is the corresponding owner that closes the file
automatically, and [`io::io_descriptor_ref`](io-descriptor-ref.md) is the
underlying descriptor reference without transfer operations.

## Declaration

```quxlang
::file_ref INCLUDE_IF(OS_LINUX) STRUCT
::file_ref INCLUDE_IF(OS_MACOS) STRUCT
::file_ref INCLUDE_IF(OS_WINDOWS) STRUCT
::file_ref INCLUDE_IF(ARCH_IS_JVM) STRUCT
```

The module declares one structure per target family and selects one with
[`INCLUDE_IF`](../../reference/availability-and-targets.md). The Linux, macOS,
and Windows structures have the same public members and differ in the native
calls that the members perform. The JVM structure wraps a
`java.io.RandomAccessFile` object and has a different member set, which
[JVM targets](#jvm-targets) describes. The structures carry no struct modifier
and are copyable.

## Member summary

The following members exist on Linux, macOS, and Windows.

**Data members**

| Member | Description |
| --- | --- |
| [`descriptor`](#descriptor) | The borrowed native descriptor or handle. |

**Construction and observation**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty reference or borrows a descriptor. |
| [`is_valid`](#is_valid) | Reports whether the stored descriptor is valid. |

**Synchronous operations**

| Member | Description |
| --- | --- |
| [`read`](#read) | Reads at the current file position on the calling thread. |
| [`write`](#write) | Writes at the current file position on the calling thread. |
| [`close`](#close) | Closes the referenced native file. |

**Coroutine operations**

| Member | Description |
| --- | --- |
| [`read` (coroutine)](#read-and-write-coroutines) | Suspends until a positioned or cursor-based read completes. |
| [`write` (coroutine)](#read-and-write-coroutines) | Suspends until a positioned or cursor-based write completes. |

## Data members

### `descriptor`

```quxlang
.descriptor VAR io_descriptor_ref;
```

The public field holds the borrowed [`io::io_descriptor_ref`](io-descriptor-ref.md).
The native value is available as `descriptor.native_handle()`.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                      // (1)
.CONSTRUCTOR FUNCTION(@descriptor io_descriptor_ref)         // (2)
```

1. Creates an empty reference whose `descriptor` is invalid.
2. Borrows `descriptor` without acquiring ownership.

### `is_valid`

```quxlang
.is_valid FUNCTION() CONST: BOOL
```

**Return value**: `descriptor.is_valid()`. The function inspects only the stored
value. A copy whose native file was closed through another copy continues to
report `TRUE`.

### `read`

```quxlang
.read FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)
```

Performs one native read of at most `buffer.size` bytes at the current file
position of the descriptor and stores the outcome in `result`. The operating
system advances the file position by the number of bytes read.

**Parameters**: `buffer` designates the destination bytes. `result` receives the
[`io::transfer_result`](buffer-ref.md#iotransfer_result).

**Result**: `result.succeeded` is `TRUE` when the native call succeeds. A
successful read can transfer fewer bytes than `buffer.size`. A successful read
of zero bytes into a nonempty buffer denotes end of file. A failed native call,
including a call on an invalid descriptor, produces `succeeded` equal to `FALSE`.
The function does not throw and does not retry an interrupted call.

**Preconditions**: the storage that `buffer` designates is alive and contains
`buffer.size` bytes.

**Target restrictions**: on Linux and macOS the function calls `read` and sets
`result.size` to zero on failure. On Windows the function calls `ReadFile`
without an `OVERLAPPED` structure, requests at most 4,294,967,295 bytes per
call, and stores the byte count that the call reports. A Windows handle opened
with `FILE_FLAG_OVERLAPPED` is not suitable for this synchronous function.

### `write`

```quxlang
.write FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)
```

Performs one native write of at most `buffer.size` bytes at the current file
position of the descriptor and stores the outcome in `result`. The referenced
bytes are not modified.

**Result**: `result.succeeded` is `TRUE` when the native call succeeds. A
successful write can transfer fewer bytes than `buffer.size`; the caller
continues with the bytes not yet written. A failed native call, such as a write
through a descriptor opened for reading only, produces `succeeded` equal to
`FALSE`. The function does not throw and does not retry an interrupted call.

**Target restrictions**: the Linux and macOS implementations call `write`. The
Windows implementation calls `WriteFile` under the same conditions as
[`read`](#read).

### `close`

```quxlang
.close FUNCTION(): BOOL
```

Closes the referenced descriptor or handle with one native call.

**Return value**: `TRUE` when the native close call reports success. Closing an
empty reference or a descriptor that is already closed returns `FALSE` on Linux
and macOS.

The function does not modify the reference and does not track other copies.
After a successful close, this reference and every copy continue to hold the
closed descriptor value, and `is_valid` continues to return `TRUE`. Using any
copy after the close is a lifetime violation: the operating system can assign
the same descriptor value to an unrelated resource.

### `read` and `write` coroutines

```quxlang
.read COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @offset U64, @result WRITE& transfer_result)    // (1)
.write COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @offset U64, @result WRITE& transfer_result)   // (2)
.read COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @result WRITE& transfer_result)                 // (3)
.write COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @result WRITE& transfer_result)                // (4)
```

The coroutine overloads are selected for calls made from a coroutine whose
controller is an [`io::async_controller`](async-controller.md), which is the
controller of every coroutine launched with `ON(runtime) RUN` on an
[`io::async_runtime`](async-runtime.md) and of the coroutines that such a
coroutine calls. Each overload submits the transfer to the runtime of the
controller, suspends the calling coroutine, and stores the transfer outcome in
`result` when the coroutine resumes.

1. Submits a positioned read at byte offset `offset` to the native completion
   queue of the runtime. The file position of the descriptor is not used.
2. Submits a positioned write at byte offset `offset`.
3. Schedules a cursor-based read on a file worker of the runtime. The file
   worker performs the synchronous [`read`](#read); the operating system manages
   the shared file position.
4. Schedules a cursor-based write on a file worker of the runtime.

**Result**: a rejected submission and a failed native transfer both produce
`result.succeeded` equal to `FALSE` with `result.size` equal to zero for
overloads (1) and (2). Overloads (3) and (4) report the result of the
synchronous function. The overloads do not throw for transfer failures.

**Preconditions**: the native file and the storage that `buffer` designates
remain valid until the coroutine resumes. Concurrent operations that use
overlapping buffer storage require external synchronization. On macOS, `offset`
does not exceed 9,223,372,036,854,775,807 for overloads (1) and (2); a larger
offset is reported as a failed transfer.

**Target restrictions**: on Windows, overloads (1) and (2) require a handle
opened with `FILE_FLAG_OVERLAPPED`, and the handle becomes associated with the
completion port of the runtime that first performs a positioned transfer on it.
The mechanism behind positioned transfers on each target is specified in
[Platform implementations](async-context.md#platform-implementations).

An ordinary function, or a coroutine that runs under a controller other than
`io::async_controller`, selects the synchronous [`read`](#read) and
[`write`](#write) functions for the argument lists of overloads (3) and (4).

## JVM targets

On targets for which `ARCH_IS_JVM` holds, `file_ref` refers to a
`java.io.RandomAccessFile` object.

```quxlang
.random_access_file VAR ~> java_random_access_file;

.CONSTRUCTOR FUNCTION()                                                              // (1)
.CONSTRUCTOR FUNCTION(@random_access_file ~> java_random_access_file)               // (2)
.is_valid FUNCTION() CONST: BOOL                                                     // (3)
.read FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)            // (4)
.write FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)           // (5)
.size FUNCTION(): SZ                                                                 // (6)
.truncate FUNCTION()                                                                 // (7)
.close FUNCTION(): BOOL                                                              // (8)
```

1. Creates an empty reference.
2. Creates a non-owning reference to `random_access_file`.
3. Returns `TRUE` when the reference contains a Java file object.
4. Reads one byte at a time until `buffer.size` bytes are stored or the Java
   `read` method reports end of file. The result always has `succeeded` equal to
   `TRUE`; `size` is the number of bytes stored.
5. Writes all `buffer.size` bytes, one byte at a time. The result has
   `succeeded` equal to `TRUE` and `size` equal to `buffer.size`.
6. Returns the current file length in bytes. The conversion to `SZ` is checked.
7. Sets the file length to zero.
8. Calls the Java `close` method and returns `TRUE`.

Java I/O exceptions are not converted to a `transfer_result`; they propagate out
of the generated JVM program. The JVM structure has no `descriptor` field and no
coroutine overloads.

The source file also declares the external types `io::java_file` and
`io::java_random_access_file` and external procedures whose names begin with
`jvm_`. These declarations bind the Java methods that the JVM structure calls
and exist only when `ARCH_IS_JVM` holds.

## Implementation functions

```quxlang
::windows_transfer_size INCLUDE_IF(OS_WINDOWS) FUNCTION(@size SZ): U32
```

`io::windows_transfer_size` returns `size` limited to the largest `U32` value,
which is the largest byte count that `ReadFile` and `WriteFile` accept. The
Windows `read` and `write` functions use the result as the requested count.

## Example

The program writes six bytes to the standard output descriptor of a POSIX
process through a borrowed reference and then attempts the same write through
an empty reference.

```quxlang
--8<-- "example-programs/reference-programs/file_reference/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/file_reference/expected-output.txt"
```

## See also

- [`io::file`](file.md)
- [`io::buffer_ref` and `io::transfer_result`](buffer-ref.md)
- [Path-based file opening](filesystem.md)
- [`io::async_runtime`](async-runtime.md)
- [`std::buffered_writer` and `std::buffered_reader`](../std/buffered-io.md)
