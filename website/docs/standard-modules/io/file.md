# `io::file`

Module `io` · source file `filesystem.qxs`

`io::file` is a move-only owner of one [`io::file_ref`](file-ref.md). The owner
has exclusive responsibility for closing the native file: destruction closes a
valid owned reference exactly once, a move transfers the responsibility and
leaves the source closed, and `release` returns the responsibility to the
caller. `io::file_ref` is the non-owning counterpart, and
[`io::async_file`](async-file.md) is the owner used for callback-based
positioned transfers on an [`io::async_context`](async-context.md).

## Declaration

```quxlang
::file STRUCT MOVE_ONLY
```

`MOVE_ONLY` removes the copy operations. The structure stores one `file_ref` in
a `PRIVATE(CLASS)` field. The declaration carries no
[`INCLUDE_IF`](../../reference/availability-and-targets.md) condition; the
structure is usable on every target that declares `io::file_ref` (Linux, macOS,
Windows, and JVM targets). The coroutine overloads exist on Linux, macOS, and
Windows only.

An owner is either *open* (the stored reference is valid) or *closed* (the
stored reference is empty).

## Member summary

**Construction, assignment, and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates a closed owner, adopts a reference, or transfers ownership. |
| [`OPERATOR:=`](#operator) | Closes the current file and transfers ownership from another owner. |
| [`DESTRUCTOR`](#destructor) | Closes the owned file. |

**Ownership**

| Member | Description |
| --- | --- |
| [`reference`](#reference) | Returns a non-owning reference to the owned file. |
| [`is_open`](#is_open) | Reports whether the owner holds a valid reference. |
| [`release`](#release) | Transfers the closing responsibility to the caller. |
| [`close`](#close) | Closes the owned file and reports the outcome. |

**Transfers**

| Member | Description |
| --- | --- |
| [`read`](#read) | Reads at the current file position on the calling thread. |
| [`write`](#write) | Writes at the current file position on the calling thread. |
| [`read` (coroutine)](#read-and-write-coroutines) | Suspends until a positioned or cursor-based read completes. |
| [`write` (coroutine)](#read-and-write-coroutines) | Suspends until a positioned or cursor-based write completes. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                          // (1)
.CONSTRUCTOR FUNCTION(@native_ref file_ref)      // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& file)         // (3)
```

1. Creates a closed owner.
2. Takes exclusive ownership of `native_ref`. An empty reference produces a
   closed owner.
3. Transfers ownership from `OTHER` and leaves `OTHER` closed.

**Preconditions** (2): no other owner holds the closing responsibility for the
native file that `native_ref` designates. After construction, the caller does
not close the native file through any other copy of the reference.

### `OPERATOR:=`

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& file) MUT
```

Takes the reference out of `OTHER`, closes the file that this owner previously
held, and stores the incoming reference. `OTHER` is left closed. The result of
closing the previous file is discarded. Assignment of an owner to itself
preserves ownership and does not close the file.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`close`](#close) and discards the result. Destroying a closed owner
performs no native call. The destructor also runs during exception unwinding,
so a file opened in a scope that exits through an exception is closed.

### `reference`

```quxlang
.reference FUNCTION() CONST: file_ref
```

**Return value**: a copy of the owned reference. The owner retains the closing
responsibility. A closed owner returns an empty reference.

The returned reference is valid for transfers until the owner closes or releases
the file. Calling `close` on the returned reference closes the native file
without the knowledge of the owner; the owner then closes the same descriptor
value a second time, which is a lifetime violation.

### `is_open`

```quxlang
.is_open FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the owned reference is valid, as reported by
[`file_ref::is_valid`](file-ref.md#is_valid).

### `release`

```quxlang
.release FUNCTION() MUT: file_ref
```

Leaves this owner closed and returns the previously owned reference. The caller
becomes responsible for closing the native file.

**Return value**: the previously owned reference, or an empty reference when the
owner is closed.

### `read`

```quxlang
.read FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)
```

Performs [`file_ref::read`](file-ref.md#read) through the owned reference when
the owner is open. A closed owner performs no native call and stores an
unsuccessful result (`succeeded` equal to `FALSE`, `size` equal to zero).

### `write`

```quxlang
.write FUNCTION(@buffer CONST& buffer_ref, @result WRITE& transfer_result)
```

Performs [`file_ref::write`](file-ref.md#write) through the owned reference when
the owner is open. A closed owner performs no native call and stores an
unsuccessful result.

### `close`

```quxlang
.close FUNCTION(): BOOL
```

Relinquishes ownership and then closes the native file with
[`file_ref::close`](file-ref.md#close).

**Return value**: `TRUE` for a closed owner, without a native call. Otherwise the
result of the native close.

The owner is closed after the call in every case. A failed native close is not
retried, because ownership was relinquished before the native call.

### `read` and `write` coroutines

```quxlang
.read INCLUDE_IF(OS_LINUX || OS_MACOS || OS_WINDOWS)
  COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @offset U64, @result WRITE& transfer_result)   // (1)
.write INCLUDE_IF(OS_LINUX || OS_MACOS || OS_WINDOWS)
  COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @offset U64, @result WRITE& transfer_result)   // (2)
.read INCLUDE_IF(OS_LINUX || OS_MACOS || OS_WINDOWS)
  COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @result WRITE& transfer_result)                // (3)
.write INCLUDE_IF(OS_LINUX || OS_MACOS || OS_WINDOWS)
  COROUTINE(@CONTROLLER async_controller#AUTO(result_type), @buffer CONST& buffer_ref, @result WRITE& transfer_result)                // (4)
```

Each overload calls the corresponding
[coroutine overload of `file_ref`](file-ref.md#read-and-write-coroutines)
through the owned reference: overloads (1) and (2) perform a positioned transfer
at `offset` through the native completion queue of the selected
[`io::async_runtime`](async-runtime.md), and overloads (3) and (4) perform a
cursor-based transfer on a file worker of that runtime. The overloads are
selected for calls made from a coroutine whose controller is an
[`io::async_controller`](async-controller.md).

**Preconditions**: the owner, the native file, and the storage that `buffer`
designates remain alive, and the owner is neither moved, released, nor closed,
until the calling coroutine resumes.

**Target restrictions**: the overloads are not declared on JVM targets. The
Windows requirements of the `file_ref` overloads apply.

## Example

```quxlang
--8<-- "example-programs/reference-programs/file_ownership/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/file_ownership/expected-output.txt"
```

## See also

- [`io::file_ref`](file-ref.md)
- [Path-based file opening](filesystem.md)
- [`io::async_file`](async-file.md)
- [`io::buffer_ref` and `io::transfer_result`](buffer-ref.md)
- [Move Semantics](../../reference/move-semantics.md)
- [Constructors and Destructors](../../reference/constructors-and-destructors.md)
