# `io::io_descriptor_ref`

Module `io` · source files `io_descriptor_ref.qxs` and `windows_completion_bindings.qxs`

`io::io_descriptor_ref` borrows one native resource. Copies identify the
same open file or socket. A reference can outlive the resource, and destroying
the last reference does not close it. [File references](file-ref.md) ·
[File owners](file.md)

## Declaration

```quxlang
::io_descriptor_ref INCLUDE_IF(OS_LINUX || OS_MACOS) STRUCT
::io_descriptor_ref INCLUDE_IF(OS_WINDOWS) STRUCT
```

On Linux and macOS, the reference contains an `I32` file descriptor.
On Windows, the reference holds shared ownership of a record containing
the native handle and its completion-port association. A non-null raw handle
import allocates a record. Copying the reference shares that record.

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an invalid reference or borrows a native value. |
| [`native_handle`](#native_handle) | Returns the stored native value. |
| [`is_valid`](#is_valid) | Reports whether the stored value can identify an open resource. |
| [`close_windows_handle`](#close_windows_handle) | Invokes a native close operation and invalidates copies on success. |

## Member functions

### `CONSTRUCTOR`

Linux and macOS:

```quxlang
.CONSTRUCTOR FUNCTION()                          // (1)
.CONSTRUCTOR FUNCTION(@native_handle I32)        // (2)
```

Windows:

```quxlang
.CONSTRUCTOR FUNCTION()                          // (1)
.CONSTRUCTOR FUNCTION(@native_handle ADDRESS)    // (2)
```

1. Creates an invalid reference. The POSIX structure stores `-1`. The Windows
   structure has no record and returns a null native handle.
2. Stores `native_handle` without acquiring ownership and without validating
   the value against the operating system.

On Windows, a raw import starts a fresh record. The caller imports each open
handle once and passes copies for subsequent use. Constructing a second
reference from `native_handle()` cannot recover an existing association.
Windows keeps a native handle associated with its first completion port until
the handle closes. An independently opened handle gets its own record, even
if Windows recycles a previous numeric value. If allocation fails during a raw
import, the caller retains responsibility for the native resource.

### `native_handle`

```quxlang
.native_handle FUNCTION() CONST: I32        // Linux and macOS
.native_handle FUNCTION() CONST: ADDRESS    // Windows
```

**Return value**: the stored descriptor or handle, in the form that native
system interfaces accept.

### `is_valid`

```quxlang
.is_valid FUNCTION() CONST: BOOL
```

**Return value**: on Linux and macOS, `TRUE` when the stored descriptor is
nonnegative. On Windows, `TRUE` when the stored handle is neither the null
address nor `INVALID_HANDLE_VALUE` (the address one below the null address).

On POSIX targets, closing the descriptor leaves its numeric value in copies.
On Windows, a successful close through the shared record clears the handle,
and every copy then reports `FALSE`. Closure through an external native call
bypasses the record. Calls to `is_valid` do not query the operating system.

### `close_windows_handle`

```quxlang
.close_windows_handle FUNCTION(@close AUTO& AUTO) CONST: BOOL
```

Calls `close(@handle native_handle())`. A `TRUE` result clears the shared
handle and releases its port identity. All descriptor copies then return a null native handle. A `FALSE` result preserves the handle and association.
The callback selects the native close operation appropriate to the resource.
The caller has exclusive access for the duration of closure, after all
submissions, completions, and other uses of the handle have finished.
[Closing a file reference](file-ref.md#close) ·
[Closing a socket reference](../net/socket-references.md#close)

## Target restrictions

| Target | Native value type | Invalid values |
| --- | --- | --- |
| Linux, macOS | `I32` | Every negative value. The default value is `-1`. |
| Windows | `ADDRESS` | The null address and `INVALID_HANDLE_VALUE`. The default value is the null address. |

The type is not declared on other targets, including JVM targets.

## Example

```quxlang
--8<-- "example-programs/reference-programs/descriptor_reference/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/descriptor_reference/expected-output.txt"
```

## See also

- [`io::file_ref`](file-ref.md)
- [`io::file`](file.md)
- [`io::async_context`](async-context.md)
- [Module `io`](index.md)
- [Availability and Targets](../../reference/availability-and-targets.md)
