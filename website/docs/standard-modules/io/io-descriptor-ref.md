# `io::io_descriptor_ref`

Module `io` · source file `io_descriptor_ref.qxs`

`io::io_descriptor_ref` stores one native I/O resource identifier: a file
descriptor on Linux and macOS, or a `HANDLE` on Windows. The type is a
non-owning value. A copy refers to the same native resource, and destruction
performs no native operation. [`io::file_ref`](file-ref.md) adds transfer
operations to a descriptor reference, and [`io::file`](file.md) adds exclusive
ownership.

## Declaration

```quxlang
::io_descriptor_ref INCLUDE_IF(OS_LINUX || OS_MACOS) STRUCT
::io_descriptor_ref INCLUDE_IF(OS_WINDOWS) STRUCT
```

The module declares two structures with the same name and selects one with
[`INCLUDE_IF`](../../reference/availability-and-targets.md). The POSIX
structure stores an `I32`. The Windows structure stores an `ADDRESS`. Both
structures keep the stored value in a `PRIVATE(CLASS)` field, declare no struct
modifier, and are copyable. The size of the structure equals the size of the
stored value: `SIZEOF(I32)` on Linux and macOS and `SIZEOF(ADDRESS)` on Windows.

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an invalid reference or borrows a native value. |
| [`native_handle`](#native_handle) | Returns the stored native value. |
| [`is_valid`](#is_valid) | Reports whether the stored value can identify an open resource. |

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
   structure stores the null address.
2. Stores `native_handle` without acquiring ownership and without validating
   the value against the operating system.

**Parameters**: `native_handle` is the descriptor or handle to borrow.

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

The function inspects only the stored value. A reference whose resource was
closed through another copy continues to report `TRUE`.

## Target restrictions

| Target | Stored type | Invalid values |
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
