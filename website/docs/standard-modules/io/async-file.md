# `io::async_file`

Module `io` · source file `async_io.qxs`

`io::async_file` is a move-only owner of one native file that is bound to one
[`io::async_context`](async-context.md). The type performs positioned transfers
with completion callbacks and counts the transfers that are in flight on the
file. Every member function that uses the file takes the context as an argument
and rejects a context other than the context given at construction. The
callback-free owner for synchronous transfers is [`io::file`](file.md).
Coroutines on `io::async_runtime` and `io::async_runtime2` can issue positioned
or cursor-based transfers through `io::file` or `io::file_ref`.
[File coroutine overloads](file-ref.md#read-and-write-coroutines)

## Declaration

```quxlang
::async_file INCLUDE_IF(OS_LINUX || OS_WINDOWS || OS_MACOS)
  STRUCT MOVE_ONLY
```

`MOVE_ONLY` removes the copy operations. The owner keeps the owned
[`io::file`](file.md), the identity of the context, and the count of pending
transfers in a separate allocation with a stable address. A move transfers the
pointer to that allocation, so transfers that are in flight during a move
complete normally. The structure is declared on Linux, macOS, and Windows.

The requirements that the source states with `ASSERT` are evaluated only when
the `ASSERT_ENABLED` policy is enabled, as described in
[`io::async_context`](async-context.md#declaration).

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Takes ownership of a native file and binds it to a context, or transfers ownership. |
| [`DESTRUCTOR`](#destructor) | Closes the owned file. |

**Transfers**

| Member | Description |
| --- | --- |
| [`read`](#read) | Starts a positioned read with a completion callback. |
| [`write`](#write) | Starts a positioned write with a completion callback. |

**Observers and ownership**

| Member | Description |
| --- | --- |
| [`pending`](#pending) | Returns the number of unfinished transfers on this file. |
| [`is_open`](#is_open) | Reports whether the owner holds an open native file. |
| [`reference`](#reference) | Returns a non-owning reference to the native file. |
| [`close`](#close) | Closes the native file. |

## Context identity

Every member function except the constructors and the destructor takes a
parameter `@context MUT& async_context` and performs the following checks before
any other effect.

**Checked failures**: `ASSERT` requires `context.valid()` to return `TRUE`,
requires that the owner is not moved-from, and requires that `context` holds the
queue state of the context given at construction. A context that received that
queue state through a move satisfies the last requirement, because the queue
state is the identity that the owner records.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@context MUT& async_context, @native_ref file_ref)    // (1)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& async_file)                              // (2)
```

1. Takes ownership of the native file that `native_ref` designates and binds the
   owner to `context`. The owner becomes responsible for closing the file.
2. Transfers the file state, including the transfers that are in flight, from
   `OTHER`. `OTHER` becomes moved-from: its destructor performs no operation,
   and every member function that takes a context rejects it.

**Parameters**: `context` is the context that performs every transfer of this
owner. `native_ref` is the [`io::file_ref`](file-ref.md) to adopt; an empty
reference produces an owner for which [`is_open`](#is_open) returns `FALSE`.

**Preconditions** (1): no other owner holds the closing responsibility for the
native file. The context outlives the owner.

**Checked failures** (1): `ASSERT` requires `context.valid()` to return `TRUE`.

**Target restrictions** (1): on Windows, the handle was opened with
`FILE_FLAG_OVERLAPPED`, and the constructor associates the handle with the
completion port of `context`. A failed association results in `PANIC`.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Closes the owned native file, discards the result of the close, and releases the
file state. Destroying a moved-from owner performs no operation.

**Checked failures**: `ASSERT` requires that no transfer of this file is
pending.

### `read`

```quxlang
.read FUNCTION(
    @context MUT& async_context,
    @buffer CONST& buffer_ref,
    @offset U64,
    @callback AUTO& AUTO(callback_type)
  )
```

Starts a positioned read of at most `buffer.size` bytes at byte offset `offset`
of the owned file. The transfer follows the completion rules of
[`async_context::read`](async-context.md#read): the context invokes `callback`
exactly once with an [`io::transfer_result`](buffer-ref.md#iotransfer_result),
on a thread that consumes completions of the context, and a submission that the
native queue rejects invokes `callback` before `read` returns, with `succeeded`
equal to `FALSE` and `size` equal to zero.

**Parameters**: `buffer` designates the destination bytes. `offset` is the byte
offset in the file. `callback` is an object that is callable as
`callback(@result transfer_result)`; the context takes ownership of the
callable.

**Preconditions**: the context, the owner, the storage that `buffer` designates,
and every object that the callable refers to remain alive until the callable
returns. The callable does not throw.

**Checked failures**: the [context identity](#context-identity) checks, and an
`ASSERT` that the owned file is open.

### `write`

```quxlang
.write FUNCTION(
    @context MUT& async_context,
    @buffer CONST& buffer_ref,
    @offset U64,
    @callback AUTO& AUTO(callback_type)
  )
```

Starts a positioned write of at most `buffer.size` bytes at byte offset
`offset` of the owned file. The parameters, completion rules, preconditions, and
checked failures are those of [`read`](#read). The referenced bytes are not
modified. A successful result can report fewer bytes than `buffer.size`.

### `pending`

```quxlang
.pending FUNCTION(@context MUT& async_context): SZ
```

**Return value**: the number of transfers that were started on this file and
whose completion has not been delivered. The count of a transfer ends
immediately before the context invokes the callback of that transfer, so a
callback observes a count that excludes its own transfer.

**Checked failures**: the [context identity](#context-identity) checks.

### `is_open`

```quxlang
.is_open FUNCTION(@context MUT& async_context): BOOL
```

**Return value**: `TRUE` when the owner holds a valid native file reference.
`FALSE` after [`close`](#close) and for an owner constructed from an empty
reference.

**Checked failures**: the [context identity](#context-identity) checks.

### `reference`

```quxlang
.reference FUNCTION(@context MUT& async_context): file_ref
```

**Return value**: a non-owning [`io::file_ref`](file-ref.md) to the owned native
file. The owner retains the closing responsibility. The reference is valid until
the owner closes the file or is destroyed.

**Checked failures**: the [context identity](#context-identity) checks.

### `close`

```quxlang
.close FUNCTION(@context MUT& async_context): BOOL
```

Closes the owned native file with [`io::file::close`](file.md#close). The owner
remains bound to the context and holds no file after the call.

**Return value**: `TRUE` when the native close succeeds, and `TRUE` without a
native call when the file is already closed. `FALSE` when the native close
fails; the close is not retried.

**Checked failures**: the [context identity](#context-identity) checks, and an
`ASSERT` that no transfer of this file is pending.

## Target restrictions

The structure is declared when `OS_LINUX || OS_WINDOWS || OS_MACOS` holds. The
mechanism that performs a positioned transfer on each target is specified in
[Platform implementations](async-context.md#platform-implementations). On
Windows, the constructor associates the open file with the context's port, or
reuses the association held by the supplied descriptor. Its transfers use that
binding. Successful closure invalidates Windows file references that share
the open-handle record. [Windows descriptor ownership](io-descriptor-ref.md)

## Example

The program adopts a descriptor that `io::open_read` returned, reads four bytes
of `/dev/zero`, and consumes the completion on the main thread.

```quxlang
--8<-- "example-programs/reference-programs/asynchronous_file/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/asynchronous_file/expected-output.txt"
```

## See also

- [`io::async_context`](async-context.md)
- [`io::thread_pool`](thread-pool.md)
- [`io::file`](file.md)
- [`io::file_ref`](file-ref.md)
- [`io::buffer_ref` and `io::transfer_result`](buffer-ref.md)
- [`io::async_runtime`](async-runtime.md)
