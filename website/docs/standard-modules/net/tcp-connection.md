# `net::tcp_connection` and `net::tcp_connection_ref`

Module `net` · source files `socket_owners.qxs`, `socket_refs.qxs`

`net::tcp_connection` owns one TCP stream socket, and `net::tcp_connection_ref`
refers to one without owning it. Both types provide the same connection
operations: `connect` establishes a connection to a remote endpoint, `read` and
`write` transfer bytes, and `shutdown` disables one or both directions. A
connection object is created unconnected by its address constructor and
connected with `connect`, or is returned already connected by
[`accept`](tcp-listener.md#accept) of a listener. The members that every socket
type shares are specified in [Socket owner types](socket-owners.md) and
[Socket reference types](socket-references.md).

## Declarations

```quxlang
::tcp_connection ALIAS socket_owner#(@kind socket_kind::connection);
::tcp_connection_ref ALIAS socket_reference#(@kind socket_kind::connection);

::shutdown_direction ENUM [receive, send, both];
```

`tcp_connection` is a move-only owner; `tcp_connection_ref` is a copyable
reference. The connection operations are declared in the private templates
with the condition `INCLUDE_IF(kind == socket_kind::connection)`, so they exist
only in these two types.

## Member summary

**Members of `tcp_connection` shared with every owner type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-owners.md#constructor) | Creates a closed owner, creates an unconnected TCP socket, adopts a reference, or transfers ownership. |
| [`OPERATOR:=`](socket-owners.md#operator), [`DESTRUCTOR`](socket-owners.md#destructor) | Transfer ownership and close the socket. |
| [`reference`](socket-owners.md#reference), [`is_open`](socket-owners.md#is_open), [`release`](socket-owners.md#release), [`close`](socket-owners.md#close) | Ownership operations. |
| [`local_endpoint`](socket-owners.md#local_endpoint), [`bind`](socket-owners.md#bind) | Local endpoint operations. |

**Members of `tcp_connection_ref` shared with every reference type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-references.md#constructor), [`is_valid`](socket-references.md#is_valid) | Invalid reference and validity test. |
| [`local_endpoint`](socket-references.md#local_endpoint), [`bind`](socket-references.md#bind), [`close`](socket-references.md#close) | Local endpoint operations and explicit close. |

**Connection operations of both types**

| Member | Description |
| --- | --- |
| [`connect`](#connect) | Establishes a connection to a remote endpoint. |
| [`read`](#read) | Receives a prefix of the requested bytes. |
| [`write`](#write) | Sends a prefix of the given bytes. |
| [`shutdown`](#shutdown) | Disables receiving, sending, or both. |
| [`async_connect`](#connect), [`async_read`](#read), [`async_write`](#write) | Future-returning forms of the operations. |

The owner type declares each connection operation with the same signature as
the reference type and forwards the call to its owned reference. The signatures
below are those of both types.

## Operation forms

`connect`, `read`, and `write` each exist in three forms, which
[Socket operations](socket-operations.md) specifies in full:

1. A synchronous function that blocks the calling thread until the operation
   completes and throws [`net::error`](error.md) on failure.
2. `async_connect`, `async_read`, and `async_write` submit the operation to
   the supplied `io::async_runtime2` or `io::async_runtime` and return a
   single-consumer future.
3. `connect`, `read`, and `write` suspend a coroutine running on either I/O
   runtime until completion. The `io::async_runtime2` form borrows the operation
   from the socket coroutine's frame.

The result of an operation and the `net::error` values that report its native
failures are the same in all three forms. Forms (2) and (3) also propagate
submission errors from the selected runtime.

## Member functions

### `connect`

```quxlang
.connect INCLUDE_IF(kind == socket_kind::connection) FUNCTION(@endpoint CONST& endpoint) CONST: VOID                         // (1)
.async_connect INCLUDE_IF(kind == socket_kind::connection)
  FUNCTION(@runtime MUT& AUTO(runtime_type), @endpoint CONST& endpoint) CONST
  ENABLE_IF(SAME_TYPES(runtime_type, io::async_runtime) || SAME_TYPES(runtime_type, io::async_runtime2)): std::future#VOID  // (2)
.connect INCLUDE_IF(kind == socket_kind::connection)
  COROUTINE(@CONTROLLER AUTO(controller_type), @endpoint CONST& endpoint) CONST
  ENABLE_IF(TYPE_HAS_MEMBER(controller_type, "submit")): VOID  // (3)
```

Connects the socket to the remote endpoint `endpoint`. The operation completes
when the connection is established or has failed.

1. Blocks the calling thread.
2. Submits the operation to `runtime` and returns a future without a value.
3. Suspends the calling coroutine.

**Parameters**: `endpoint` is the remote address and port. The IP version of
its address is the version with which the socket was created. `runtime` is the
runtime that performs the operation.

**Exceptions**: a failed connection attempt throws the
[`net::error`](error.md) that corresponds to the native error, for example
`error::connection_refused` for a port on which no socket listens. An invalid
reference throws `error::closed`. In form (2), the exception is delivered
through the future.

A failed `connect` does not close the socket, and the owner retains the closing
obligation. A new connection attempt uses a new socket: on macOS, a further
`connect` on a socket whose attempt failed throws `error::invalid_argument`.

**Target restrictions**: on Windows, forms (2) and (3) use `ConnectEx`, which
requires a bound socket. When the socket is not bound, the operation first
binds it to the unspecified address of the IP version of `endpoint` with an
ephemeral port; a binding that the program established with `bind` is
preserved.

### `read`

```quxlang
.read INCLUDE_IF(kind == socket_kind::connection) FUNCTION(@buffer CONST& io::buffer_ref) CONST: SZ                         // (1)
.async_read INCLUDE_IF(kind == socket_kind::connection)
  FUNCTION(@runtime MUT& AUTO(runtime_type), @buffer CONST& io::buffer_ref) CONST
  ENABLE_IF(SAME_TYPES(runtime_type, io::async_runtime) || SAME_TYPES(runtime_type, io::async_runtime2)): std::future#SZ  // (2)
.read INCLUDE_IF(kind == socket_kind::connection)
  COROUTINE(@CONTROLLER AUTO(controller_type), @buffer CONST& io::buffer_ref) CONST
  ENABLE_IF(TYPE_HAS_MEMBER(controller_type, "submit")): SZ  // (3)
```

Receives at most `buffer.size` bytes from the connection into the storage that
`buffer` designates. The operation waits until at least one byte is available,
the peer has ended its sending direction, or an error occurs, and then
transfers the bytes that are available.

**Parameters**: `buffer` is the [`io::buffer_ref`](../io/buffer-ref.md) that
designates the destination bytes.

**Return value**: the number of bytes received, which can be less than
`buffer.size`.

- A result of zero for a nonempty buffer denotes the orderly end of the stream:
  the peer has shut down its sending direction or closed the connection, and
  all preceding bytes have been received.
- An empty buffer returns zero immediately without a native call.

One native transfer moves at most 2,147,483,647 bytes. A native call that a
signal interrupts is repeated.

**Exceptions**: a failed native transfer throws the translated
[`net::error`](error.md), for example `error::connection_reset`. A socket that
is not connected and an invalid reference throw `error::closed`.

**Preconditions**: the storage that `buffer` designates remains alive until the
operation completes. No other receiving operation is in progress on the
connection.

### `write`

```quxlang
.write INCLUDE_IF(kind == socket_kind::connection) FUNCTION(@buffer CONST& io::buffer_ref) CONST: SZ                        // (1)
.async_write INCLUDE_IF(kind == socket_kind::connection)
  FUNCTION(@runtime MUT& AUTO(runtime_type), @buffer CONST& io::buffer_ref) CONST
  ENABLE_IF(SAME_TYPES(runtime_type, io::async_runtime) || SAME_TYPES(runtime_type, io::async_runtime2)): std::future#SZ  // (2)
.write INCLUDE_IF(kind == socket_kind::connection)
  COROUTINE(@CONTROLLER AUTO(controller_type), @buffer CONST& io::buffer_ref) CONST
  ENABLE_IF(TYPE_HAS_MEMBER(controller_type, "submit")): SZ  // (3)
```

Sends at most `buffer.size` bytes from the storage that `buffer` designates.
The referenced bytes are not modified.

**Return value**: the number of bytes that the operating system accepted for
transmission, which can be less than `buffer.size`. A caller that requires the
complete range repeats the operation with a `buffer_ref` that covers the bytes
not yet accepted. An empty buffer returns zero immediately without a native
call.

One native transfer moves at most 2,147,483,647 bytes. A native call that a
signal interrupts is repeated.

**Exceptions**: a failed native transfer throws the translated
[`net::error`](error.md). A write on a connection whose peer no longer accepts
data throws `error::closed` or `error::connection_reset`; the module suppresses
the broken-pipe signal that Linux and macOS otherwise raise for such a write.

**Preconditions**: the storage that `buffer` designates remains alive until the
operation completes. No other sending operation is in progress on the
connection.

### `shutdown`

```quxlang
.shutdown INCLUDE_IF(kind == socket_kind::connection) FUNCTION(@direction shutdown_direction) CONST
```

Disables the selected directions of the connection with one native call. The
socket remains open, and the closing obligation is unchanged. The function has
a synchronous form only.

**Parameters**: `direction` selects the directions.

| `net::shutdown_direction` | Effect |
| --- | --- |
| `receive` | Disables further receiving on this socket. |
| `send` | Disables further sending. The peer observes the end of the stream after it has received all bytes already sent. |
| `both` | Disables both directions. |

**Exceptions**: a failed native call throws the translated
[`net::error`](error.md). A socket that is not connected and an invalid
reference throw `error::closed`.

## Scope of the connection interface

The module declares no function that returns the remote endpoint of a
connection, no socket option interface, and no deadline or cancellation for a
pending operation.

## Example

The program connects two sockets over the loopback interface with synchronous
operations, transfers five bytes, and observes the end of the stream after a
shutdown.

```quxlang
--8<-- "example-programs/reference-programs/tcp_round_trip/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/tcp_round_trip/expected-output.txt"
```

## See also

- [Socket owner types](socket-owners.md)
- [Socket reference types](socket-references.md)
- [`net::tcp_listener`](tcp-listener.md)
- [Socket operations](socket-operations.md)
- [`net::error`](error.md)
- [`io::buffer_ref`](../io/buffer-ref.md)
- [`io::async_runtime`](../io/async-runtime.md)
- [`io::async_runtime2`](../io/async-runtime2.md)
