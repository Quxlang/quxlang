# `net::tcp_listener` and `net::tcp_listener_ref`

Module `net` · source files `socket_owners.qxs`, `socket_refs.qxs`

`net::tcp_listener` owns one TCP socket that accepts incoming connections, and
`net::tcp_listener_ref` refers to one without owning it. A listener is created
for an IP version, bound to a local endpoint with `bind`, placed in the
listening state with `listen`, and then asked for connections with `accept`.
Each accepted connection is returned as an owning
[`net::tcp_connection`](tcp-connection.md). The members that every socket type
shares are specified in [Socket owner types](socket-owners.md) and
[Socket reference types](socket-references.md).

## Declarations

```quxlang
::tcp_listener ALIAS socket_owner#(@kind socket_kind::listener);
::tcp_listener_ref ALIAS socket_reference#(@kind socket_kind::listener);
```

`tcp_listener` is a move-only owner; `tcp_listener_ref` is a copyable
reference. The listener operations are declared in the private templates with
the condition `INCLUDE_IF(kind == socket_kind::listener)`, so they exist only
in these two types.

## Member summary

**Members of `tcp_listener` shared with every owner type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-owners.md#constructor) | Creates a closed owner, creates an unbound TCP socket, adopts a reference, or transfers ownership. |
| [`OPERATOR:=`](socket-owners.md#operator), [`DESTRUCTOR`](socket-owners.md#destructor) | Transfer ownership and close the socket. |
| [`reference`](socket-owners.md#reference), [`is_open`](socket-owners.md#is_open), [`release`](socket-owners.md#release), [`close`](socket-owners.md#close) | Ownership operations. |
| [`local_endpoint`](socket-owners.md#local_endpoint), [`bind`](socket-owners.md#bind) | Local endpoint operations. |

**Members of `tcp_listener_ref` shared with every reference type**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](socket-references.md#constructor), [`is_valid`](socket-references.md#is_valid) | Invalid reference and validity test. |
| [`local_endpoint`](socket-references.md#local_endpoint), [`bind`](socket-references.md#bind), [`close`](socket-references.md#close) | Local endpoint operations and explicit close. |

**Listener operations of both types**

| Member | Description |
| --- | --- |
| [`listen`](#listen) | Places the bound socket in the listening state. |
| [`accept`](#accept) | Waits for one incoming connection and returns its owner. |
| [`async_accept`](#accept) | Future-returning form of `accept`. |

The owner type declares each listener operation with the same signature as the
reference type and forwards the call to its owned reference.

## Member functions

### `listen`

```quxlang
.listen INCLUDE_IF(kind == socket_kind::listener) FUNCTION(@backlog I32 DEFAULT(128)) CONST
```

Places the socket in the listening state. The operating system then completes
incoming connections and queues them until `accept` removes them.

**Parameters**: `backlog` is the queue length that the function passes to the
native `listen` call. The default value is 128.

A listener that serves a known port is bound to its local endpoint with
[`bind`](socket-references.md#bind) before the call. The function does not
require a prior `bind`: on macOS, a socket that was not bound listens on an
ephemeral port, which `local_endpoint` reports.

**Exceptions**: a `backlog` that is zero or negative throws
`error::invalid_argument` before any native call. A failed native call throws
the translated [`net::error`](error.md); an invalid reference throws
`error::closed`.

### `accept`

```quxlang
.accept INCLUDE_IF(kind == socket_kind::listener) FUNCTION() CONST: tcp_connection                                         // (1)
.async_accept INCLUDE_IF(kind == socket_kind::listener)
  FUNCTION(@runtime MUT& AUTO(runtime_type)) CONST
  ENABLE_IF(SAME_TYPES(runtime_type, io::async_runtime) || SAME_TYPES(runtime_type, io::async_runtime2)): std::future#tcp_connection  // (2)
.accept INCLUDE_IF(kind == socket_kind::listener)
  COROUTINE(@CONTROLLER AUTO(controller_type)) CONST
  ENABLE_IF(TYPE_HAS_MEMBER(controller_type, "submit")): tcp_connection  // (3)
```

Removes one established connection from the queue of the listener, waiting
until a connection is available.

1. Blocks the calling thread.
2. Submits the operation to the supplied `io::async_runtime2` or
   `io::async_runtime` and returns a future for the connection. The operation
   can be submitted before any client connects.
3. Suspends a coroutine running on either I/O runtime until a connection is
   accepted or an error occurs. The `io::async_runtime2` form keeps the
   pending accept in the socket coroutine's frame.

[Socket operations](socket-operations.md) specifies the three forms.

**Return value**: an open [`net::tcp_connection`](tcp-connection.md) that owns
the accepted socket. The accepted socket has the properties of a socket that
the module creates, as [Socket creation](socket-operations.md#socket-creation)
describes. Its local port is the port of the listener. The listener remains in
the listening state and accepts further connections.

**Exceptions**: a failed native call throws the translated
[`net::error`](error.md); an invalid reference throws `error::closed`. On
macOS, a socket that is not in the listening state throws
`error::invalid_argument`. In form (2), the exception is delivered through the
future. When an exception is thrown
after the operating system has produced the accepted socket, the socket is
closed before the exception propagates.

**Preconditions**: no other `accept` is in progress on the listener. In forms
(2) and (3), the listener remains open until the operation completes.

**Target restrictions**: on Windows, forms (2) and (3) use `AcceptEx`. The
operation creates the socket for the incoming connection, with the IP version
of the local endpoint of the listener, when the operation starts.

## Scope of the listener interface

`accept` does not report the remote endpoint of the accepted connection, and
the module declares no function that returns it. A pending `accept` has no
deadline and cannot be cancelled; closing the listener while an `accept` is
pending violates the lifetime contract.

## Example

The program submits an accept operation before the client connects and obtains
the accepted connection from the future.

```quxlang
--8<-- "example-programs/reference-programs/tcp_accept/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/tcp_accept/expected-output.txt"
```

## See also

- [`net::tcp_connection`](tcp-connection.md)
- [Socket owner types](socket-owners.md)
- [Socket reference types](socket-references.md)
- [Socket operations](socket-operations.md)
- [`net::error`](error.md)
- [`io::async_runtime`](../io/async-runtime.md)
- [`io::async_runtime2`](../io/async-runtime2.md)
- [`std::future`](../std/future.md)
