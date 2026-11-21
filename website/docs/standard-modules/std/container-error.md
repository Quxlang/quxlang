# `std::container_error`

Module `std` · source file `container_error.qxs`

`std::container_error` identifies a container operation that was rejected. The
standard containers throw a value of this enumeration; a handler receives it
with `CATCH error CONST& std::container_error`.

## Declaration

```quxlang
::container_error ENUM [size_overflow, missing_key];
```

## Enumerators

| Enumerator | Description |
| --- | --- |
| `size_overflow` | The operation requires an element count or a capacity that `SZ` cannot represent. |
| `missing_key` | A checked lookup did not find the requested key. |

## Throwing functions

The following functions throw `std::container_error::size_overflow`.

| Type | Functions |
| --- | --- |
| [`std::geometric_array`](geometric-array.md) | `reserve`, `resize`, `emplace_back`, `push_back` |
| [`std::avl_tree`](avl-tree.md) | `insert`, `append` |
| [`std::map`](map.md) | `insert`, `insert_or_assign`, `OPERATOR[]` |
| [`std::set`](set.md) | `insert` |
| [`std::hadix_map`](hadix-map.md) | `insert`, `insert_or_assign`, `OPERATOR[]` |

The following functions throw `std::container_error::missing_key`.

| Type | Functions |
| --- | --- |
| [`std::map`](map.md) | `at` |
| [`std::hadix_map`](hadix-map.md) | `at` |

[`std::dynarr`](dynarr.md), [`std::list`](list.md), and
[`std::cord`](cord.md) state their size limits and index requirements with
`ASSERT` and do not throw `container_error`.

## Example

The program catches both enumerators.

```quxlang
--8<-- "example-programs/reference-programs/container_error_example/main.qxs"
```

```text
--8<-- "example-programs/reference-programs/container_error_example/expected-output.txt"
```

## See also

- [Exception Handling](../../reference/exceptions.md)
- [Enums](../../reference/enums.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
