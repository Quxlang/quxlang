# Socket owner types

Module `net` · source file `socket_owners.qxs`

`net::tcp_connection`, `net::tcp_listener`, and `net::udp_session` are
move-only owners of one native socket. An owner holds one
[socket reference](socket-references.md) together with the exclusive obligation
to close the socket: destruction closes an owned socket exactly once, a move
transfers the obligation and leaves the source closed, and `release` returns
the obligation to the caller. This page specifies the members that the three
owner types share; the operations of each transport are specified in
[`net::tcp_connection`](tcp-connection.md),
[`net::tcp_listener`](tcp-listener.md), and
[`net::udp_session`](udp-session.md).

## Declarations

```quxlang
::tcp_connection ALIAS socket_owner#(@kind socket_kind::connection);
::tcp_listener ALIAS socket_owner#(@kind socket_kind::listener);
::udp_session ALIAS socket_owner#(@kind socket_kind::datagram);
```

The three names are aliases of specializations of one structure template,
`socket_owner`, declared `STRUCT MOVE_ONLY`. The template and its value
parameter type `socket_kind` are private to the module; programs name the
aliases. `MOVE_ONLY` removes the copy operations. The owner stores one
reference in a `PRIVATE(CLASS)` field and no other state.

In the signatures on this page, `socket_reference#(@kind kind)` denotes the
reference type of the same transport, and `socket_owner#(@kind kind)` denotes
the owner type itself:

| Owner type | `socket_reference#(@kind kind)` | Socket created by the address constructor |
| --- | --- | --- |
| `tcp_connection` | [`tcp_connection_ref`](socket-references.md) | TCP stream socket |
| `tcp_listener` | [`tcp_listener_ref`](socket-references.md) | TCP stream socket |
| `udp_session` | [`udp_session_ref`](socket-references.md) | UDP datagram socket |

An owner is either *open* (the stored reference is valid) or *closed* (the
stored reference is invalid).

## Member summary

The following members exist in all three owner types.

**Construction, assignment, and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates a closed owner, creates a socket, adopts a reference, or transfers ownership. |
| [`OPERATOR:=`](#operator) | Closes the owned socket and transfers ownership from another owner. |
| [`DESTRUCTOR`](#destructor) | Closes the owned socket. |

**Ownership**

| Member | Description |
| --- | --- |
| [`reference`](#reference) | Returns a non-owning reference to the owned socket. |
| [`is_open`](#is_open) | Reports whether the owner holds a socket. |
| [`release`](#release) | Transfers the closing obligation to the caller. |
| [`close`](#close) | Closes the owned socket and reports the outcome. |

**Local endpoint**

| Member | Description |
| --- | --- |
| [`local_endpoint`](#local_endpoint) | Returns the local address and port of the owned socket. |
| [`bind`](#bind) | Binds the owned socket to a local endpoint. |

Every transport-specific member of an owner forwards to the member of the same
name in the reference type and is listed on the page of its transport.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                                             // (1)
.CONSTRUCTOR FUNCTION(@address CONST& ip_address)                   // (2)
.CONSTRUCTOR FUNCTION(@reference socket_reference#(@kind kind))     // (3)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& socket_owner#(@kind kind))       // (4)
```

1. Creates a closed owner.
2. Creates a new native socket for the IP version of `address` and takes
   ownership of it. The socket is a TCP socket for `tcp_connection` and
   `tcp_listener` and a UDP socket for `udp_session`. The constructor uses only
   the active alternative of `address`; the socket is neither bound nor
   connected.
3. Adopts the exclusive closing obligation of `reference`. An invalid reference
   produces a closed owner.
4. Transfers ownership from `OTHER` and leaves `OTHER` closed.

**Exceptions** (2): a failed socket creation or configuration throws the
translated [`net::error`](error.md), for example `error::resource_limit` when
the process has no free descriptor. No socket remains open when the constructor
throws.

**Preconditions** (3): no other owner holds the closing obligation for the
socket, and no other copy of the reference is used to close it.

The sockets that overload (2) creates are not inherited by a program image that
the process starts, and are configured as
[Socket creation](socket-operations.md#socket-creation) describes.

### `OPERATOR:=`

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& socket_owner#(@kind kind)) MUT
```

Takes the reference out of `OTHER`, closes the socket that this owner
previously held, and stores the incoming reference. `OTHER` is left closed. The
result of closing the previous socket is discarded. Assignment of an owner to
itself preserves the socket.

**Preconditions**: no operation is pending on the socket that this owner held
before the assignment.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Calls [`close`](#close) and discards the result. Destroying a closed owner
performs no native call.

**Preconditions**: every pending operation on the owned socket has completed.

### `reference`

```quxlang
.reference FUNCTION() CONST: socket_reference#(@kind kind)
```

**Return value**: a copy of the owned reference. The owner retains the closing
obligation. A closed owner returns an invalid reference.

The returned reference borrows the socket. On Windows, successful closure
through any reference invalidates all copies, including the owner's reference.
On Linux and macOS, the owner and its references retain their descriptor number
after another reference closes it. The owner normally closes the socket or
transfers that obligation through `release()`.

### `is_open`

```quxlang
.is_open FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the owned reference is valid, as reported by
[`is_valid`](socket-references.md#is_valid) of the reference.

### `release`

```quxlang
.release FUNCTION() MUT: socket_reference#(@kind kind)
```

Leaves this owner closed and returns the previously owned reference. The caller
becomes responsible for closing the socket, either through
[`close`](socket-references.md#close) of the reference or by constructing a new
owner from it.

**Return value**: the previously owned reference, or an invalid reference when
the owner is closed.

### `close`

```quxlang
.close FUNCTION() MUT: BOOL
```

Relinquishes ownership and then closes the socket with
[`close`](socket-references.md#close) of the reference.

**Return value**: `TRUE` for a closed owner, without a native call. Otherwise
the result of the native close.

The owner is closed after the call in every case. A failed native close is not
retried.

**Preconditions**: no operation is pending on the socket.

### `local_endpoint`

```quxlang
.local_endpoint FUNCTION() CONST: endpoint
```

Returns [`local_endpoint`](socket-references.md#local_endpoint) of the owned
reference.

**Exceptions**: those of the reference function. A closed owner throws
`error::closed`.

### `bind`

```quxlang
.bind FUNCTION(@endpoint CONST& endpoint) CONST
```

Calls [`bind`](socket-references.md#bind) of the owned reference.

**Exceptions**: those of the reference function. A closed owner throws
`error::closed`.

## Stability during coroutine operations

The coroutine forms of `connect`, `accept`, `read`, `write`, `send`, and
`receive` borrow the owner. On both `io::async_runtime2` and
`io::async_runtime`, the caller keeps the owner open and at a stable address
until the call returns. On `io::async_runtime2`, the socket coroutine keeps
the operation and its waiter in the coroutine frame.

The `async_` members submit a borrowed socket descriptor. The owner can move
while the future is pending. Its socket and the referenced buffers must stay
alive through completion. On Windows the operation retains its own copy of the descriptor, which
keeps the open-handle record alive. Moving the owner preserves the socket's
completion-port association. Successful
closure invalidates all references sharing the record.
[Socket operations](socket-operations.md#lifetime-and-concurrency-contract)

## Target restrictions

The owner types are usable on Linux, macOS, and Windows, which are the targets
for which the module declares the native socket handle type. On Windows, every
open socket holds one reference to the Winsock initialization of the process,
which the address constructor and `accept` acquire and `close` releases.

## Example

```quxlang
--8<-- "example-programs/reference-programs/socket_ownership/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/socket_ownership/expected-output.txt"
```

## See also

- [Socket reference types](socket-references.md)
- [`net::tcp_connection`](tcp-connection.md)
- [`net::tcp_listener`](tcp-listener.md)
- [`net::udp_session`](udp-session.md)
- [Socket operations](socket-operations.md)
- [`net::ip_address` and `net::endpoint`](address.md)
- [`io::file`](../io/file.md)
- [Move Semantics](../../reference/move-semantics.md)
