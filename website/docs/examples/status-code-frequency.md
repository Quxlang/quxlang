# Status code frequency table

The program counts the occurrences of each status code in a list of responses, prints the counts in
ascending order of the codes, and handles the lookup of a code that was not observed. The program
demonstrates [`std::map`](../standard-modules/std/map.md), including insertion through the index
operator, ordered iteration, checked lookup, and
[`std::container_error`](../standard-modules/std/container-error.md).

```quxlang
--8<-- "example-programs/programs/status_code_frequency/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/status_code_frequency/expected-output.txt"
```

## Explanation

The type `std::map#[I32:SZ]` uses the compact [template argument
form](../reference/templates-and-value-parameters.md#argument-syntax) for a key type and a value
type. The [index operator](../standard-modules/std/map.md#operator-index) returns a mutable
reference to the value that is associated with a key. When the key is absent, the operator first
inserts an entry with a default-constructed value, which is zero for `SZ`. The statement
`frequencies[status_code]++;` therefore counts each occurrence without a separate insertion step.

The loop `LOOP INDEX(status_code) VALUE(count) IN(frequencies)` iterates the
[`IV_PAIRS`](../standard-modules/std/map.md#iv_pairs) range of the map and binds the key and the
value of each entry. The map keeps its entries in ascending key order, so the output lists the codes
in that order although the array `responses` is unsorted.

[`size`](../standard-modules/std/map.md#size) returns the number of distinct keys.
[`at`](../standard-modules/std/map.md#at) returns a reference to the value of an existing key, and
[`contains`](../standard-modules/std/map.md#contains) reports whether a key is present. Both
functions receive the key as the named argument `@key`.

`at` throws `std::container_error::missing_key` when the key is absent and leaves the map unchanged.
The exception leaves the `TRY` block before `std::printf` writes any output, and the [`CATCH`
clause](../reference/exceptions.md#typed-matching) for `std::container_error` prints the replacement
line.
