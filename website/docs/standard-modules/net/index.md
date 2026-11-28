# Module `net`

Module `net` · public declarations in `address.qxs`, `socket_owners.qxs`, and `socket_refs.qxs`

The `net` module provides TCP connections, TCP listeners, and UDP sessions over
IPv4 and IPv6 on Linux, macOS, and Windows. The interface consists of IP
endpoint values, portable error categories, and two families of socket types:
move-only owners that close their socket automatically, and copyable references
that do not own it. Native socket handles and native error numbers are
implementation details that the interface does not expose. The six TCP and
UDP operations that wait for the network each provide blocking,
future-returning, and coroutine forms. Both `io::async_runtime2` and
`io::async_runtime` support the asynchronous forms.
[Socket operations](socket-operations.md)

A source file gains access to the module with `IMPORT net;` and names its
declarations with the prefix `net::`. Buffers are passed as
[`io::buffer_ref`](../io/buffer-ref.md) values, so a program that transfers
data also imports the `io` module.

## Pages

| Page | Contents |
| --- | --- |
| [`net::ip_address` and `net::endpoint`](address.md) | `ipv4_address`, `ipv6_address`, `ip_address`, `endpoint`. |
| [`net::error`](error.md) | Portable error categories and their native error numbers. |
| [Socket owner types](socket-owners.md) | Construction, ownership transfer, and closing, shared by `tcp_connection`, `tcp_listener`, and `udp_session`. |
| [Socket reference types](socket-references.md) | Members shared by `tcp_connection_ref`, `tcp_listener_ref`, and `udp_session_ref`. |
| [`net::tcp_connection` and `net::tcp_connection_ref`](tcp-connection.md) | `connect`, `read`, `write`, `shutdown`, `shutdown_direction`. |
| [`net::tcp_listener` and `net::tcp_listener_ref`](tcp-listener.md) | `listen`, `accept`. |
| [`net::udp_session` and `net::udp_session_ref`](udp-session.md) | `send`, `receive`, `datagram_result`. |
| [Socket operations](socket-operations.md) | TCP connect, accept, read, and write, plus UDP send and receive. Includes blocking and asynchronous forms, lifetime requirements, and platform implementations. |

## Public declarations

| Declaration | Kind | Source file |
| --- | --- | --- |
| `ipv4_address`, `ipv6_address`, `endpoint`, `datagram_result` | `STRUCT` | `address.qxs` |
| `ip_address` | `INLINE_VARIANT` | `address.qxs` |
| `error` | `ENUM` | `address.qxs` |
| `shutdown_direction` | `ENUM` | `socket_refs.qxs` |
| `tcp_connection_ref`, `tcp_listener_ref`, `udp_session_ref` | `ALIAS` of a private structure template | `socket_refs.qxs` |
| `tcp_connection`, `tcp_listener`, `udp_session` | `ALIAS` of a private structure template | `socket_owners.qxs` |

The module implements its asynchronous transports separately for Linux
io_uring, POSIX socket readiness, and Windows overlapped I/O. They share
socket ownership, endpoint conversion, and portable error reporting.
[Socket operations](socket-operations.md#platform-implementations) ·
[`net::error`](error.md)

The native socket procedures and ABI structures are declared in the `syscall`
module, in the namespaces `syscall::linux`, `syscall::macos`, and
`syscall::windows`. The `net` module implements resource ownership, endpoint
conversion, and error translation on top of them.

## Resource types

| Reference | Owner | Operations |
| --- | --- | --- |
| `tcp_connection_ref` | `tcp_connection` | `bind`, `connect`, `read`, `write`, `shutdown` |
| `tcp_listener_ref` | `tcp_listener` | `bind`, `listen`, `accept` |
| `udp_session_ref` | `udp_session` | `bind`, `send`, `receive` |

Every resource type provides `local_endpoint` and `close`. A reference
provides `is_valid`; an owner provides `is_open`, `reference`, and `release`.

## Target restrictions

The public declarations of the module carry no
[`INCLUDE_IF`](../../reference/availability-and-targets.md) condition. The
socket types are usable on the targets for which the module declares its native
socket handle type: Linux and macOS, where the handle is an `I32` descriptor,
and Windows, where the handle is a `UINTPTR` socket handle. The value types
`ipv4_address`, `ipv6_address`, `ip_address`, `endpoint`, `datagram_result`,
`error`, and `shutdown_direction` have no native dependency.

The module has no option of its own. On Linux, the option
[`io::linux_use_io_uring`](../io/index.md#module-options) of the `io` module
selects whether the future and coroutine forms of the socket operations use
io_uring requests or readiness notifications, as
[Platform implementations](socket-operations.md#platform-implementations)
describes.

## Scope

The module declares no name resolution, no textual address parsing or
formatting, no socket option interface, no deadline or cancellation for a
pending operation, and no function that returns the remote endpoint of a
connection.

## See also

- [Module `io`](../io/index.md)
- [`io::async_runtime`](../io/async-runtime.md)
- [`io::async_runtime2`](../io/async-runtime2.md)
- [`io::buffer_ref`](../io/buffer-ref.md)
- [`std::future`](../std/future.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
