# Path-based file opening

Module `io` · source file `filesystem.qxs`

The functions `io::open_read`, `io::open_read_write`, and `io::open_replace`
open a file that a path names and return an [`io::file`](file.md) that owns the
resulting native file. The functions differ in the access mode and in the
treatment of an absent or existing file. `io::create_temporary_path` creates an
empty temporary file on JVM targets and returns its path.

## Function summary

| Function | Access | Absent file | Existing contents |
| --- | --- | --- | --- |
| [`open_read`](#open_read) | Reading | Failure | Preserved |
| [`open_read_write`](#open_read_write) | Reading and writing | Created | Preserved |
| [`open_replace`](#open_replace) | Reading and writing | Created | Discarded |
| [`create_temporary_path`](#create_temporary_path) | Not applicable | Creates a new temporary file | Not applicable |

## Functions

### `open_read`

```quxlang
::open_read INCLUDE_IF(OS_LINUX || OS_MACOS)
  FUNCTION(@path CONST& std::string): file    // (1)
::open_read INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(@path CONST& std::string): file    // (2)
```

Opens an existing file for reading.

1. Opens `path` with the native read-only access mode.
2. Constructs a `java.io.RandomAccessFile` with mode `"r"`.

**Parameters**: `path` is the path of the file, as a
[`std::string`](../std/string.md) without a terminating null byte.

**Return value**: an open `io::file` that owns the new native file. The initial
file position is the beginning of the file.

**Exceptions**: [POSIX failure conditions](#posix-failure-conditions) specifies
the exceptions of overload (1), and [JVM targets](#jvm-targets) specifies the
exceptions of overload (2).

### `open_read_write`

```quxlang
::open_read_write INCLUDE_IF(OS_LINUX || OS_MACOS)
  FUNCTION(@path CONST& std::string): file    // (1)
::open_read_write INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(@path CONST& std::string): file    // (2)
```

Opens a file for reading and writing and creates the file when it is absent.
Existing contents are preserved, and the initial file position is the beginning
of the file.

1. Opens `path` with the native read-write access mode and the creation flag.
2. Constructs a `java.io.RandomAccessFile` with mode `"rw"`.

**Return value**: an open `io::file` that owns the new native file.

**Exceptions**: [POSIX failure conditions](#posix-failure-conditions) specifies
the exceptions of overload (1), and [JVM targets](#jvm-targets) specifies the
exceptions of overload (2).

### `open_replace`

```quxlang
::open_replace INCLUDE_IF(OS_LINUX || OS_MACOS)
  FUNCTION(@path CONST& std::string): file    // (1)
::open_replace INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(@path CONST& std::string): file    // (2)
```

Opens a file for reading and writing, creates the file when it is absent, and
discards existing contents.

1. Opens `path` with the native read-write access mode, the creation flag, and
   the truncation flag.
2. Constructs a `java.io.RandomAccessFile` with mode `"rw"` and sets the file
   length to zero.

**Return value**: an open `io::file` that owns the new, empty native file.

**Exceptions**: [POSIX failure conditions](#posix-failure-conditions) specifies
the exceptions of overload (1), and [JVM targets](#jvm-targets) specifies the
exceptions of overload (2).

### `create_temporary_path`

```quxlang
::create_temporary_path INCLUDE_IF(ARCH_IS_JVM)
  FUNCTION(): std::string
```

Creates an empty temporary file through `java.io.File.createTempFile` with the
name prefix `quxlang-cortado-` and the name suffix `.tmp`, registers the file
for deletion when the Java virtual machine exits, and returns the path of the
file.

**Return value**: the path of the new temporary file.

**Target restrictions**: the function is declared only when `ARCH_IS_JVM` holds.

## POSIX failure conditions

On Linux and macOS, each function copies `path` into a null-terminated byte
sequence, calls the native `open` function, and transfers the returned
descriptor to the result. The following conditions apply to all three
functions.

**Exceptions**:

- A `path` that contains a byte equal to zero throws the `I32` value `-1`
  before any native call.
- A failed native `open` call throws the negative `I32` value that the call
  returns. On Linux the value is the negated error number of the system call. On
  macOS the value is `-1`.

No file remains open when a function throws.

**Descriptor properties**: every descriptor is opened with the close-on-exec
flag, so a program image started by the process does not inherit the descriptor.

**Creation permissions**: a file that `open_read_write` or `open_replace`
creates receives read and write permission for the owner, the group, and other
users, reduced by the file mode creation mask of the process.

## JVM targets

On targets for which `ARCH_IS_JVM` holds, the functions convert `path` to a Java
string and construct a `java.io.RandomAccessFile`. Java I/O exceptions are not
translated; they propagate out of the generated JVM program. The returned owner
holds the JVM form of [`io::file_ref`](file-ref.md#jvm-targets).

## Target restrictions

| Target | `open_read`, `open_read_write`, `open_replace` | `create_temporary_path` |
| --- | --- | --- |
| Linux, macOS | Declared; native `open` | Not declared |
| JVM | Declared; `java.io.RandomAccessFile` | Declared |
| Windows | Not declared | Not declared |

The module declares no path-based opening function for Windows. A Windows
program can import a newly opened native handle into an `io_descriptor_ref`,
construct a `file_ref` from that descriptor, and transfer the closing
obligation to an `io::file`. Copies of the descriptor carry its association
across these operations. [Windows descriptor imports](io-descriptor-ref.md#constructor)

The module declares no function that removes, renames, or queries a file.

## Example

The program opens `/dev/null` with two access modes and shows both failure
forms. `/dev/null` accepts every write and reports end of file for every read,
so the program leaves no file behind.

```quxlang
--8<-- "example-programs/reference-programs/open_files/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/open_files/expected-output.txt"
```

## See also

- [`io::file`](file.md)
- [`io::file_ref`](file-ref.md)
- [`std::string`](../std/string.md)
- [Exceptions](../../reference/exceptions.md)
