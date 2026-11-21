# Socket reference types

Module `net` · source file `socket_refs.qxs`

`net::tcp_connection_ref`, `net::tcp_listener_ref`, and `net::udp_session_ref`
are non-owning references to one native socket. A reference stores the native
socket handle and nothing else: a reference has no allocation, no operation
bookkeeping, and no destructor effect. Copies designate the same socket. The
corresponding [socket owner types](socket-owners.md) add exclusive ownership
and close the socket automatically. This page specifies the members that the
three reference types share; the operations of each transport are specified in
[`net::tcp_connection`](tcp-connection.md),
[`net::tcp_listener`](tcp-listener.md), and
[`net::udp_session`](udp-session.md).

## Declarations

```quxlang
::tcp_connection_ref ALIAS socket_reference#(@kind socket_kind::connection);
::tcp_listener_ref ALIAS socket_reference#(@kind socket_kind::listener);
::udp_session_ref ALIAS socket_reference#(@kind socket_kind::datagram);
```

The three names are aliases of specializations of one structure template,
`socket_reference`, which is private to the module together with its value
parameter type `socket_kind`. Programs name the aliases. The template selects
the transport-specific members of each specialization with
[`INCLUDE_IF`](../../reference/availability-and-targets.md) conditions on
`kind`, so a member such as `listen` exists only in `tcp_listener_ref`.

The structure carries no struct modifier and is copyable. Every member function
is declared `CONST`: an operation acts on the socket and leaves the reference
unchanged.

| Reference type | Owner type | Transport | Transport-specific members |
| --- | --- | --- | --- |
| `tcp_connection_ref` | [`tcp_connection`](tcp-connection.md) | TCP stream | `connect`, `read`, `write`, `shutdown`, `async_connect`, `async_read`, `async_write` |
| `tcp_listener_ref` | [`tcp_listener`](tcp-listener.md) | TCP listening socket | `listen`, `accept`, `async_accept` |
| `udp_session_ref` | [`udp_session`](udp-session.md) | UDP | `send`, `receive`, `async_send`, `async_receive` |

## Member summary

The following members exist in all three reference types.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an invalid reference. |
| [`is_valid`](#is_valid) | Reports whether the reference stores a socket handle. |
| [`local_endpoint`](#local_endpoint) | Returns the local address and port of the socket. |
| [`bind`](#bind) | Binds the socket to a local endpoint. |
| [`close`](#close) | Closes the socket. |

## Obtaining a reference

A valid reference originates from a socket owner:
[`reference`](socket-owners.md#reference) returns a reference while the owner
keeps the closing obligation, and [`release`](socket-owners.md#release) returns
a reference together with the closing obligation. The constructor that takes a
native handle is private to the module, so a program cannot wrap a descriptor
that another interface produced.

## Lifetime and concurrency contract

- A reference does not extend the lifetime of its socket. The socket is closed
  exactly once, by its owner or by one call of [`close`](#close), after every
  pending operation on the socket has completed.
- After the socket has been closed, every remaining copy of the reference is
  dangling. The copies still store the handle value, and the operating system
  can assign the same value to an unrelated resource. Using a dangling copy
  violates the lifetime contract.
- The caller serializes the operations of each direction. One receiving
  operation (`read`, `receive`, `accept`) and one sending operation (`write`,
  `send`, `connect`) can be in progress on one socket at the same time; two
  operations of the same direction cannot.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()
```

Creates an invalid reference, which stores the invalid socket value of the
target: `-1` on Linux and macOS, and the all-ones handle value on Windows.

### `is_valid`

```quxlang
.is_valid FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the stored handle differs from the invalid socket
value. The function inspects only the stored value: a copy whose socket was
closed through another copy or through the owner continues to return `TRUE`.

### `local_endpoint`

```quxlang
.local_endpoint FUNCTION() CONST: endpoint
```

Queries the operating system for the local name of the socket.

**Return value**: the local [`net::endpoint`](address.md#netendpoint). For a
socket bound with port zero, the result contains the port that the operating
system selected. The local port of an accepted connection is the port of the
listener. For a socket that is not bound, the outcome is that of the native
query of the target: on macOS, the function returns an endpoint whose port is
zero.

**Exceptions**: a failed native query throws the translated
[`net::error`](error.md); an invalid reference throws `error::closed`. A local
name whose address family is neither IPv4 nor IPv6 throws
`error::invalid_argument`.

### `bind`

```quxlang
.bind FUNCTION(@endpoint CONST& endpoint) CONST
```

Binds the socket to the local address and port of `endpoint`.

**Parameters**: `endpoint` is the local endpoint. Port zero selects an
ephemeral port, which [`local_endpoint`](#local_endpoint) reports afterwards.

**Exceptions**: a failed native call throws the translated
[`net::error`](error.md). An endpoint that another socket already occupies
throws `error::address_in_use`, an address that is not local to the host throws
`error::address_unavailable`, and an invalid reference throws `error::closed`.
On macOS, an endpoint whose IP version differs from the version of the socket,
and a second `bind` of a socket that is already bound, throw
`error::invalid_argument`.

The module sets no address-reuse option on a socket before binding.

### `close`

```quxlang
.close FUNCTION() CONST: BOOL
```

Closes the referenced socket with one native call.

**Return value**: `TRUE` when the native close reports success, and `TRUE`
without a native call for an invalid reference. `FALSE` when the native close
fails.

The function does not modify the reference, which continues to store the closed
handle value, and does not track other copies.

**Preconditions**: no operation on the socket is pending. The socket has not
already been closed through another copy or through an owner.

**Target restrictions**: on Windows, the function also releases the Winsock
initialization reference that the socket holds.

## Target restrictions

The reference types are usable on Linux, macOS, and Windows. The stored handle
is an `I32` descriptor on Linux and macOS and a `UINTPTR` socket handle on
Windows; the module declares the handle type for these targets only.

## Example

```quxlang
--8<-- "example-programs/reference-programs/socket_reference/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/socket_reference/expected-output.txt"
```

## See also

- [Socket owner types](socket-owners.md)
- [`net::tcp_connection`](tcp-connection.md)
- [`net::tcp_listener`](tcp-listener.md)
- [`net::udp_session`](udp-session.md)
- [Socket operations](socket-operations.md)
- [`net::endpoint`](address.md#netendpoint)
- [`net::error`](error.md)
