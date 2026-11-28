# Socket operations

Module `net` · source files `socket_async.qxs`, `socket_async_uring.qxs`, `socket_async_readiness.qxs`, `socket_async_windows.qxs`, `socket_transport.qxs`, `socket_readiness.qxs`, `socket_operations.qxs`, `socket_state.qxs`, `socket_extensions.qxs`

The `net` module has six socket operations that can wait for the network:
`connect`, `accept`, `read`, `write`, `send`, and `receive`. Each operation is
available in three forms with one result and one set of failure conditions: a
synchronous function, a function that returns a future, and a coroutine. This
page specifies the three forms, the lifetime and concurrency contract that they
share, the properties of the sockets that the module creates, and the native
mechanism behind each form on every target. The parameters and results of the
individual operations are specified on the pages of
[`net::tcp_connection`](tcp-connection.md),
[`net::tcp_listener`](tcp-listener.md), and
[`net::udp_session`](udp-session.md).

The source files that this page covers declare no public name. Their
declarations are private to the module and implement the public members of the
[owner](socket-owners.md) and [reference](socket-references.md) types.

## Operation summary

| Operation | Declared in | Synchronous and coroutine name | Future name | Result |
| --- | --- | --- | --- | --- |
| Connect | `tcp_connection`, `tcp_connection_ref` | [`connect`](tcp-connection.md#connect) | `async_connect` | `VOID` |
| Accept | `tcp_listener`, `tcp_listener_ref` | [`accept`](tcp-listener.md#accept) | `async_accept` | `tcp_connection` |
| Read | `tcp_connection`, `tcp_connection_ref` | [`read`](tcp-connection.md#read) | `async_read` | `SZ` |
| Write | `tcp_connection`, `tcp_connection_ref` | [`write`](tcp-connection.md#write) | `async_write` | `SZ` |
| Send | `udp_session`, `udp_session_ref` | [`send`](udp-session.md#send) | `async_send` | `SZ` |
| Receive | `udp_session`, `udp_session_ref` | [`receive`](udp-session.md#receive) | `async_receive` | `datagram_result` |

The operations `bind`, `listen`, `shutdown`, `local_endpoint`, and `close`
complete without waiting for the network and have a synchronous form only.

## Synchronous form

```quxlang
VAR received SZ := connection.read(@buffer buffer);
```

A call from an ordinary function selects the synchronous function. A
coroutine controller without a socket submission interface also selects it.
The function blocks the calling thread until completion. A native socket
failure throws [`net::error`](error.md).

The synchronous form requires no runtime and starts no thread. The function has
no time limit: a `read` on a connection whose peer sends nothing, and an
`accept` on a listener that no client contacts, do not return.

## Future form

```quxlang
VAR runtime io::async_runtime2;
VAR pending := connection.async_read(@runtime runtime, @buffer buffer);
VAR received SZ := pending.get();
```

Each `async_` function takes a mutable reference to an `io::async_runtime2`
or `io::async_runtime`. The runtime takes ownership of the operation and
returns a single-consumer `std::future` of its result type. For example,
`async_connect` returns `std::future#VOID`, and `async_accept` returns
`std::future#net::tcp_connection`. [Future contract](../std/future.md)

- The function starts the operation on the calling thread and returns without
  waiting for its completion. An operation that completes during the start
  produces a future that is already ready.
- The runtime processes pending operations through its native completion or
  readiness queue. Its scheduler and file workers are available for other work.
- A failure is stored in the future, and `get` or `AWAIT` rethrows it. The
  `async_` function itself does not throw `net::error`.
- The function copies the borrowed socket descriptor, the buffer reference, and the endpoint
  argument into the operation at the call.

## Coroutine form

```quxlang
::read COROUTINE(@connection net::tcp_connection_ref, @buffer io::buffer_ref): SZ
{
  RETURN connection.read(@buffer buffer);
}
```

`ON(runtime) RUN` selects `io::async_controller2` for an `io::async_runtime2`
and `io::async_controller` for an `io::async_runtime`. Both controllers
select the coroutine forms of all six operations. Nested coroutine calls and
coroutines launched with `GO` inherit the selected runtime.

Under `io::async_controller2`, the socket coroutine stores its operation and
waiter in its frame. The waiter borrows the operation, starts the native
request, and arranges resumption when the operation completes. The socket
coroutine returns the result or rethrows the failure. An inline completion
allows the coroutine to continue immediately. Under `io::async_controller`,
the socket coroutine submits the operation and awaits its future.
[Processor-backed runtime](../io/async-runtime2.md) ·
[I/O controller](../io/async-controller.md)

## Lifetime and concurrency contract

- The socket remains open until every operation on it has completed. Closing a
  socket, destroying its owner, or assigning to its owner while an operation is
  pending violates the contract.
- The storage that a `buffer` argument designates remains alive until the
  operation completes: until the synchronous function returns, until the future
  is ready, or until the coroutine resumes.
- For the coroutine form called through an owner, the owner object remains
  alive and at its address until the coroutine resumes, as
  [Socket owner types](socket-owners.md#stability-during-coroutine-operations)
  specifies.
- The caller serializes the operations of each direction on one socket. One
  receiving operation and one sending operation can be pending at the same
  time.
- Both runtimes wait for pending socket operations during `drain()` and
  destruction. An `accept` on a listener with no incoming connection can keep
  the drain pending indefinitely.
- On Windows, the first native asynchronous submission associates the open
  socket with its runtime's completion port. A native submission through a
  different runtime fails with `io::async_error::submission_failed`. All
  owners and references to the open socket share its association record.
  An in-flight operation retains a descriptor copy so its record stays alive
  during deferred start and completion. The caller keeps the socket open. After all operations complete, a
  successful close through the socket owner or a reference invalidates all
  copies. A newly opened socket has its own record even if Windows reuses a
  numeric handle. [Windows descriptor ownership](../io/io-descriptor-ref.md)

## Socket creation

The address constructor of an [owner type](socket-owners.md#constructor) and
the `accept` operation create native sockets. The module configures every such
socket before the program receives it:

| Target | Creation | Properties |
| --- | --- | --- |
| Linux | One `socket` call, or one `accept4` call for an accepted connection, with the nonblocking and close-on-exec flags | Nonblocking. Not inherited by a new program image. Sends pass the no-signal flag, so a write to a closed connection raises no `SIGPIPE`. |
| macOS | A new socket uses three configuration calls. An accepted socket inherits nonblocking and no-signal behavior from the listener and sets close-on-exec with one call | Close-on-exec, nonblocking, and the `SO_NOSIGPIPE` option. A failed configuration call closes the socket and throws the translated error. |
| Windows | One Winsock startup call (version 2.2) per socket, then socket creation with the overlapped and no-inherit flags | Blocking mode with support for overlapped I/O. Not inherited by a child process. `close` releases the startup reference. |

A TCP socket uses the stream type with the TCP protocol, and a UDP socket uses
the datagram type with the UDP protocol. The address family is IPv4 or IPv6
according to the address given at creation. The module sets no other socket
option; in particular, no address-reuse option is set.

## Platform implementations

Each backend defines a distinct request structure for each of the six
asynchronous socket operations. The request borrows the socket and any
caller-supplied payload buffer through completion.

The processor-backed runtime uses the same native queue for socket
notifications and positioned file completions. The target and the Linux
`io::linux_use_io_uring` module option select the socket transport at compile
time. [`io::async_runtime2`](../io/async-runtime2.md)

| Target | Synchronous form | Future and coroutine forms |
| --- | --- | --- |
| Linux, `io::linux_use_io_uring` equal to `TRUE` (default) | Nonblocking native call; on a would-block result, `ppoll` on the calling thread, then a repeated call | One io_uring request per operation, submitted to the ring of the runtime |
| Linux, `io::linux_use_io_uring` equal to `FALSE` | As above | The submitting thread attempts the nonblocking operation. The runtime waits for epoll readiness after a would-block result. |
| macOS | Nonblocking native call; on a would-block result, `poll` on the calling thread, then a repeated call | As for the Linux epoll backend, with a one-shot kqueue event as the readiness registration |
| Windows | Blocking Winsock call | Overlapped Winsock call whose completion packet is delivered to the I/O completion port of the runtime |

**Readiness-based targets (macOS, and Linux with the epoll backend)**. The
operation attempts the nonblocking native call when it is submitted. When the
call would block, the operation registers readable readiness for `accept`,
`read`, and `receive`, or writable readiness for `connect`, `write`, and
`send`. When the notification arrives, a completion
worker repeats the native call and registers a further notification after a
spurious readiness. A `connect` completes at the first writable notification;
the operation then reads the pending error of the socket to determine success
or failure. A notification that carries an error completes the operation with
the translated error.

On Linux, `io::async_runtime2` tracks a pending reader and writer
independently under one epoll watch for the socket. On macOS, it uses the
kqueue read and write filters. Both readiness backends deliver notifications
on the runtime's completion thread.

**Synchronous waiting on Linux and macOS**. The synchronous form uses the same
nonblocking calls and waits with `ppoll` (Linux) or `poll` (macOS) without a
time limit. A wait that a signal interrupts is repeated. `connect` reads the
pending error of the socket after the socket becomes writable.

**Linux io_uring**. `connect`, `accept`, `read`, and `write` submit the corresponding connect,
accept, receive, and send requests. A UDP send or receive retains one socket
descriptor, a 28-byte IPv4/IPv6 address buffer, one `iovec`, and one
`msghdr` at a stable address. The completed byte count replaces the vector
length. A receive decodes the source endpoint when the caller retrieves the
result. A request that
completes with an interruption is submitted again, except for `connect`, which
reports `error::interrupted`. Accepted sockets are created nonblocking and
close-on-exec.

**Windows**. `read` and `write` use overlapped `WSARecv` and `WSASend`, `send`
and `receive` use overlapped `WSASendTo` and `WSARecvFrom`, `connect` uses
`ConnectEx`, and `accept` uses `AcceptEx`. The operation retrieves the
`ConnectEx` or `AcceptEx` entry point from the socket provider for each call.
`ConnectEx` requires a bound socket, so `connect` binds an unbound socket to
the unspecified address with an ephemeral port. `accept` creates the socket for
the incoming connection before it calls `AcceptEx`. After completion, `connect`
and `accept` apply the connection context update option to the connected
socket. A datagram that is longer than the buffer is reported as a truncated
`datagram_result`; the native message-size error is not propagated.

**Empty buffers and transfer limits**. An empty buffer completes a TCP
`read` or `write` with zero on an open socket. A UDP `receive` with an empty
buffer consumes one datagram. Its `truncated` flag distinguishes a nonempty
datagram from an empty datagram. The io_uring receive uses a zero-length
vector and reads `MSG_TRUNC` from the completed message header.
One native TCP transfer is limited to 2,147,483,647 bytes.

## Target restrictions

The module supports Linux, macOS, and Windows. Its public declarations carry no
[`INCLUDE_IF`](../../reference/availability-and-targets.md) condition, and the
native socket handle type on which they depend is declared for these three
targets only. The future and coroutine forms accept `io::async_runtime` and
`io::async_runtime2` on all three operating systems. The option
[`io::linux_use_io_uring`](../io/index.md#module-options) selects the Linux
mechanism for the future and coroutine forms and has no effect on the
synchronous form.

## Example

The program runs an echo server and a client as two coroutines on one runtime
with a single scheduler worker. Every `accept`, `connect`, `read`, and `write`
call in the coroutines selects the coroutine form.

```quxlang
--8<-- "example-programs/reference-programs/socket_coroutines/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/socket_coroutines/expected-output.txt"
```

## See also

- [`net::tcp_connection`](tcp-connection.md)
- [`net::tcp_listener`](tcp-listener.md)
- [`net::udp_session`](udp-session.md)
- [`net::error`](error.md)
- [`io::async_runtime`](../io/async-runtime.md)
- [`io::async_controller`](../io/async-controller.md)
- [Native operations](../io/native-operation.md)
- [`std::future`](../std/future.md)
- [Thread-pool coroutines](../../reference/thread-pool-coroutines.md)
