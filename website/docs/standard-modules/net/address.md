# `net::ip_address` and `net::endpoint`

Module `net` · source file `address.qxs`

The address types of the `net` module are plain values. `net::ipv4_address` and
`net::ipv6_address` hold the bytes of one address, `net::ip_address` holds an
address of either version, and `net::endpoint` combines an address with a port
and an IPv6 scope identifier. An endpoint names the local side of a socket in
[`bind`](socket-references.md#bind) and
[`local_endpoint`](socket-references.md#local_endpoint), the remote side in
[`connect`](tcp-connection.md#connect) and [`send`](udp-session.md#send), and
the sender of a datagram in
[`net::datagram_result`](udp-session.md#netdatagram_result).

## Declarations

```quxlang
::ipv4_address STRUCT
{
  .data VAR [4]U8;
}

::ipv6_address STRUCT
{
  .data VAR [16]U8;
}

::ip_address INLINE_VARIANT [ipv4_address DEFAULT, ipv6_address];

::endpoint STRUCT
{
  .address VAR ip_address;
  .port VAR U16;
  .scope_id VAR U32;
}
```

All four types are declared on every target, carry no struct modifier, and are
copyable. The module declares no constructor and no member function for them;
a program assigns the fields directly.

## `net::ipv4_address`

| Member | Description |
| --- | --- |
| `data` | The four bytes of the address in network order. `data[0]` is the first byte of the dotted-decimal notation. |

A default-initialized value holds four zero bytes, which is the unspecified
address `0.0.0.0`. The loopback address `127.0.0.1` has `data[0]` equal to 127
and `data[3]` equal to 1.

## `net::ipv6_address`

| Member | Description |
| --- | --- |
| `data` | The sixteen bytes of the address in network order. |

A default-initialized value holds sixteen zero bytes, which is the unspecified
address `::`. The loopback address `::1` has `data[15]` equal to 1.

## `net::ip_address`

`net::ip_address` is an inline variant with the alternatives `ipv4_address` and
`ipv6_address`. The `DEFAULT` marker selects `ipv4_address` as the alternative
of a default-initialized value, so a default-initialized `ip_address` is the
IPv4 unspecified address.

| Operation | Form |
| --- | --- |
| Construction from an alternative | `net::ip_address(@OTHER address)` |
| Test of the active alternative | `value ISA net::ipv6_address` |
| Access to the active alternative | `MATCH value AS address { TYPE net::ipv4_address { ... } TYPE net::ipv6_address { ... } }` |

[Variants](../../reference/variants.md) and [`MATCH`](../../reference/match.md)
specify these operations.

The active alternative determines the IP version of every socket operation that
receives the address. The constructor of a
[socket owner](socket-owners.md#constructor) that takes an address creates an
IPv4 socket for an `ipv4_address` and an IPv6 socket for an `ipv6_address`; the
constructor uses only the version and ignores the address bytes.

## `net::endpoint`

| Member | Description |
| --- | --- |
| `address` | The IP address. |
| `port` | The port number in host order. The module converts the value to network order at the native boundary. |
| `scope_id` | The interface scope of an IPv6 address. The module passes the value to the operating system for IPv6 endpoints only. |

A default-initialized endpoint has the IPv4 unspecified address, port zero, and
scope zero. In [`bind`](socket-references.md#bind), port zero requests an
ephemeral port that the operating system selects;
[`local_endpoint`](socket-references.md#local_endpoint) returns the selected
port afterwards.

An endpoint that the module returns from `local_endpoint` or in a
`datagram_result` has a `scope_id` of zero when its address is an IPv4 address,
and the scope that the operating system reports when its address is an IPv6
address.

## Scope of the address interface

The module declares no function that parses or formats a textual address, and
no name resolution. A program fills the `data` bytes from numeric values.

## Example

```quxlang
--8<-- "example-programs/reference-programs/endpoint_values/main.qxs"
```

Output on macOS ARM64:

```text
--8<-- "example-programs/reference-programs/endpoint_values/expected-output.txt"
```

## See also

- [Socket owner types](socket-owners.md)
- [Socket reference types](socket-references.md)
- [`net::udp_session`](udp-session.md)
- [`net::error`](error.md)
- [Variants](../../reference/variants.md)
- [`MATCH`](../../reference/match.md)
