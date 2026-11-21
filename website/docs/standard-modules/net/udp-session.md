# `net::udp_session` and `net::udp_session_ref`

Module `net` · source files `socket_owners.qxs`, `socket_refs.qxs`, `address.qxs`

`net::udp_session` owns one UDP socket, and `net::udp_session_ref` refers to
one without owning it. Both types provide the same datagram operations: `send`
transmits one message to a destination endpoint, and `receive` consumes one
message and reports its size, its source, and whether the message was
truncated. A session is not connected to a peer; every `send` names its
destination, and one session exchanges messages with any number of peers. The
members that every socket type shares are specified in
[Socket owner types](socket-owners.md) and
[Socket reference types](socket-references.md).

## Declarations

```quxlang
::udp_session ALIAS socket_owner#(@kind socket_kind::datagram);
::udp_session_ref ALIAS socket_reference#(@kind socket_kind::datagram);

::datagram_result STRUCT
{
  .size VAR SZ;
  .source VAR endpoint;
  .truncated VAR BOOL;
}
```

`udp_session` is a move-only owner; `udp_session_ref` is a copyable reference.
The datagram operations are declared in the private templates with the
condition `INCLUDE_IF(kind == socket_kind::datagram)`, so they exist only in
these two types.

## Member summary

**Members of `udp_session` shared with every owner type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-owners.md#constructor) | Creates a closed owner, creates an unbound UDP socket, adopts a reference, or transfers ownership. |
| [`OPERATOR:=`](socket-owners.md#operator), [`DESTRUCTOR`](socket-owners.md#destructor) | Transfer ownership and close the socket. |
| [`reference`](socket-owners.md#reference), [`is_open`](socket-owners.md#is_open), [`release`](socket-owners.md#release), [`close`](socket-owners.md#close) | Ownership operations. |
| [`local_endpoint`](socket-owners.md#local_endpoint), [`bind`](socket-owners.md#bind) | Local endpoint operations. |

**Members of `udp_session_ref` shared with every reference type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-references.md#constructor), [`is_valid`](socket-references.md#is_valid) | Invalid reference and validity test. |
| [`local_endpoint`](socket-references.md#local_endpoint), [`bind`](socket-references.md#bind), [`close`](socket-references.md#close) | Local endpoint operations and explicit close. |

**Datagram operations of both types**

| Member | Description |
| --- | --- |
| [`send`](#send) | Sends one message to a destination endpoint. |
| [`receive`](#receive) | Consumes one message and reports its source. |
| [`async_send`](#send), [`async_receive`](#receive) | Future-returning forms of the operations. |

The owner type declares each datagram operation with the same signature as the
reference type and forwards the call to its owned reference.

## Operation forms

`send` and `receive` each exist in three forms, which
[Socket operations](socket-operations.md) specifies in full: a synchronous
function that blocks the calling thread, a function with the prefix `async_`
that submits the operation to an
[`io::async_runtime`](../io/async-runtime.md) and returns a
[`std::future`](../std/future.md), and a coroutine with the name of the
synchronous function, which a coroutine that runs on an `io::async_runtime`
selects and which suspends the calling coroutine.

## Member functions

### `send`

```quxlang
.send INCLUDE_IF(kind == socket_kind::datagram) FUNCTION(@buffer CONST& io::buffer_ref, @destination CONST& endpoint) CONST: SZ      // (1)
.async_send INCLUDE_IF(kind == socket_kind::datagram)
  FUNCTION(@runtime MUT& io::async_runtime, @buffer CONST& io::buffer_ref, @destination CONST& endpoint) CONST: std::future#SZ      // (2)
.send INCLUDE_IF(kind == socket_kind::datagram)
  COROUTINE(@CONTROLLER io::async_controller#AUTO(result_type), @buffer CONST& io::buffer_ref, @destination CONST& endpoint) CONST: SZ   // (3)
```

Sends the bytes that `buffer` designates as one message to `destination`. A
message is transmitted completely or not at all; the function does not split a
message. The referenced bytes are not modified.

1. Blocks the calling thread.
2. Submits the operation to `runtime` and returns a future for the byte count.
3. Suspends the calling coroutine.

**Parameters**: `buffer` is the [`io::buffer_ref`](../io/buffer-ref.md) that
designates the message. An empty buffer sends a message of size zero.
`destination` is the address and port of the receiver; the operation copies
the endpoint when it starts.

**Return value**: the number of bytes sent, which is `buffer.size` for a
message that the operating system accepts.

**Exceptions**: a `buffer.size` above 2,147,483,647 throws
`error::message_too_large` before any native call. A failed native call throws
the translated [`net::error`](error.md); a message that exceeds the size that
the transport permits throws `error::message_too_large`, and an invalid
reference throws `error::closed`. A native call that a signal interrupts is
repeated.

**Preconditions**: the storage that `buffer` designates remains alive until the
operation completes. No other sending operation is in progress on the socket.

### `receive`

```quxlang
.receive INCLUDE_IF(kind == socket_kind::datagram) FUNCTION(@buffer CONST& io::buffer_ref) CONST: datagram_result                  // (1)
.async_receive INCLUDE_IF(kind == socket_kind::datagram)
  FUNCTION(@runtime MUT& io::async_runtime, @buffer CONST& io::buffer_ref) CONST: std::future#datagram_result                     // (2)
.receive INCLUDE_IF(kind == socket_kind::datagram)
  COROUTINE(@CONTROLLER io::async_controller#AUTO(result_type), @buffer CONST& io::buffer_ref) CONST: datagram_result             // (3)
```

Waits for one message, removes it from the receive queue of the socket, copies
at most `buffer.size` of its bytes into the storage that `buffer` designates,
and reports the outcome. Each call consumes exactly one message, regardless of
the size of the buffer.

**Parameters**: `buffer` designates the destination bytes.

**Return value**: a [`net::datagram_result`](#netdatagram_result).

- A message that fits in the buffer is copied completely; `truncated` is
  `FALSE`.
- A message that is longer than the buffer is copied up to `buffer.size`
  bytes. `size` is `buffer.size`, `truncated` is `TRUE`, and the remainder of
  the message is discarded.
- A message of size zero produces `size` equal to zero and `truncated` equal to
  `FALSE`.
- An empty buffer still consumes the next message. `size` is zero, and
  `truncated` is `TRUE` exactly when the consumed message contained at least
  one byte.

**Exceptions**: a failed native call throws the translated
[`net::error`](error.md); an invalid reference throws `error::closed`. A source
address whose family is neither IPv4 nor IPv6 throws
`error::invalid_argument`. A native call that a signal interrupts is repeated.

**Preconditions**: the storage that `buffer` designates remains alive until the
operation completes. No other receiving operation is in progress on the socket.

## `net::datagram_result`

| Member | Description |
| --- | --- |
| `size` | The number of bytes copied into the buffer. The value never exceeds the size of the buffer. |
| `source` | The [`net::endpoint`](address.md#netendpoint) of the sender. |
| `truncated` | `TRUE` when the message contained more bytes than were copied. |

The structure is declared on every target, carries no struct modifier, and is
copyable. A default-initialized value has `size` equal to zero, `truncated`
equal to `FALSE`, and a default-initialized `source`.

## Scope of the session interface

The module declares no operation that connects a UDP socket to a fixed peer, no
multicast or broadcast configuration, and no deadline or cancellation for a
pending operation.

## Example

The program exchanges three messages between two sessions on the loopback
interface: a message that fits in the destination buffer, a message that is
truncated, and an empty message.

```quxlang
--8<-- "example-programs/reference-programs/udp_datagrams/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/udp_datagrams/expected-output.txt"
```

## See also

- [Socket owner types](socket-owners.md)
- [Socket reference types](socket-references.md)
- [Socket operations](socket-operations.md)
- [`net::ip_address` and `net::endpoint`](address.md)
- [`net::error`](error.md)
- [`io::buffer_ref`](../io/buffer-ref.md)
- [`io::async_runtime`](../io/async-runtime.md)
