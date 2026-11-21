# `net::error`

Module `net` · source files `address.qxs`, `socket_transport.qxs`

`net::error` is the exception type of the `net` module. Each enumerator names a
portable category of socket failure. The module translates the native error
number of a failed system call into one enumerator at the platform boundary and
throws the enumerator as the exception value; the native number is not
preserved. Asynchronous operations deliver the same value through their
[`std::future`](../std/future.md) or rethrow it in the awaiting coroutine.

## Declaration

```quxlang
::error ENUM [invalid_argument, closed, permission_denied, address_in_use, address_unavailable,
  connection_refused, connection_reset, connection_aborted, network_unreachable, timed_out,
  would_block, interrupted, resource_limit, message_too_large, unsupported, system_failure];
```

The enumeration is declared on every target. A handler of the form
`CATCH failure CONST& net::error` receives the value, and the handler compares
the value with an enumerator such as `net::error::connection_refused`, as the
[example](#example) shows.

## Enumerators

| Enumerator | Meaning |
| --- | --- |
| `invalid_argument` | The operating system or the module rejected an argument or the state of the socket. |
| `closed` | The reference designates no open socket, the socket is not connected, or the connection no longer accepts data. |
| `permission_denied` | The process lacks permission for the operation. |
| `address_in_use` | The local endpoint is occupied by another socket. |
| `address_unavailable` | The requested address is not available on the host. |
| `connection_refused` | The remote host refused the connection. |
| `connection_reset` | The peer reset the connection. |
| `connection_aborted` | The connection or the operation was aborted. |
| `network_unreachable` | No route leads to the network or to the host. |
| `timed_out` | The operation exceeded a time limit of the operating system. |
| `would_block` | A nonblocking native call could not complete immediately. |
| `interrupted` | A signal interrupted a native call. |
| `resource_limit` | A descriptor limit, a memory limit, or a buffer limit was reached. |
| `message_too_large` | A datagram exceeds the permitted size. |
| `unsupported` | The operation or the address family is not supported. |
| `system_failure` | Any native error number that the translation does not list. |

## Translation of native error numbers

The following table lists every native error number that the translation
recognizes on each target. The numbers are `errno` values on Linux and macOS,
and Winsock or Windows system error codes on Windows. Every other number
translates to `system_failure`.

| Enumerator | Linux | macOS | Windows |
| --- | --- | --- | --- |
| `invalid_argument` | 22 | 22 | 10022 |
| `closed` | 9, 32, 107 | 9, 32, 57 | 6, 109, 10038, 10057, 10058, 10093 |
| `permission_denied` | 13 | 13 | 5, 10013 |
| `address_in_use` | 98 | 48 | 10048 |
| `address_unavailable` | 99 | 49 | 10049 |
| `connection_refused` | 111 | 61 | 1225, 10061 |
| `connection_reset` | 104 | 54 | 64, 10054 |
| `connection_aborted` | 103 | 53 | 995, 1236, 10053 |
| `network_unreachable` | 101, 113 | 51, 65 | 1231, 1232, 10051, 10065 |
| `timed_out` | 110 | 60 | 121, 1460, 10060 |
| `would_block` | 11 | 35 | 10035 |
| `interrupted` | 4 | 4 | 10004 |
| `resource_limit` | 12, 23, 24, 105 | 12, 23, 24, 55 | 8, 1450, 10024, 10055 |
| `message_too_large` | 90 | 40 | 10040 |
| `unsupported` | 95, 97 | 45, 47 | 10045, 10047 |

The enumerator `closed` covers three native conditions: a bad descriptor or a
handle that is not a socket, a socket that is not connected, and a write to a
connection that the peer has closed (a broken pipe) or that was shut down. On
Windows, the code 10093 reports that Winsock is not initialized; every open
socket of the module holds a Winsock initialization reference, so the code
identifies a reference with no open socket.

## Errors that the module raises without a native error

| Enumerator | Condition |
| --- | --- |
| `invalid_argument` | [`listen`](tcp-listener.md#listen) with a `backlog` that is zero or negative. A native socket address whose family is neither IPv4 nor IPv6, or whose length is too short, in [`local_endpoint`](socket-references.md#local_endpoint) and [`receive`](udp-session.md#receive). |
| `closed` | A future or coroutine operation on an invalid reference, on the targets on which the operation checks the handle before submission (Linux with io_uring, and Windows). On the other targets the native call reports the same condition. |
| `message_too_large` | [`send`](udp-session.md#send) with a buffer of more than 2,147,483,647 bytes. |
| `system_failure` | On Windows, a failure to clear the inheritance flag of an accepted socket. |

## Errors that the module handles internally

- `would_block`: on Linux and macOS, every socket of the module is
  nonblocking. A synchronous operation that receives this condition waits for
  readiness on the calling thread and repeats the native call; a future or
  coroutine operation registers a readiness notification. The operations
  therefore do not report `would_block` to the caller on these targets.
- `interrupted`: the transfer operations `read`, `write`, `send`, and `receive`
  repeat a native call that a signal interrupts. `connect` treats an
  interrupted call as a connection in progress and waits for its completion.
  The readiness wait of a synchronous operation repeats an interrupted wait.
  An interrupted native `accept` call is not repeated by the synchronous form
  or by the readiness-based asynchronous forms (macOS, and Linux with the epoll
  backend) and is reported as `interrupted`. On Linux with the io_uring
  backend, a request that completes with an interruption is submitted again,
  except for a connect request, which is reported as `interrupted`.

## Other exception types

The future and coroutine forms of the socket operations also propagate
[`io::async_error::submission_failed`](../io/async-context.md#ioasync_error)
when the completion queue of the runtime rejects a registration. The `close`
functions of the [reference](socket-references.md#close) and
[owner](socket-owners.md#close) types report failure through their `BOOL`
result and do not throw.

## Example

The program provokes four failures on the loopback interface and compares each
caught value with the expected enumerator.

```quxlang
--8<-- "example-programs/reference-programs/socket_errors/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/socket_errors/expected-output.txt"
```

## See also

- [Socket operations](socket-operations.md)
- [Socket reference types](socket-references.md)
- [`net::tcp_connection`](tcp-connection.md)
- [`net::udp_session`](udp-session.md)
- [`io::async_error`](../io/async-context.md#ioasync_error)
- [Exceptions](../../reference/exceptions.md)
- [Enums](../../reference/enums.md)
