# Station code deduplication

The program inserts three station codes, one of which repeats an earlier code, into an ordered set
and reports the number of distinct codes. The program demonstrates
[`std::set`](../standard-modules/std/set.md) and the result of its `insert` member function.

```quxlang
--8<-- "example-programs/programs/deduplicate_stations/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/deduplicate_stations/expected-output.txt"
```

## Explanation

The type `std::set#I32` instantiates the set template with the key type `I32`. The `#` [template
argument form](../reference/templates-and-value-parameters.md#argument-syntax) supplies the single
type argument. A set stores at most one element for each key and keeps the keys in ascending order.

[`insert`](../standard-modules/std/set.md#insert) adds a key when no equal key is present and
returns `TRUE`. When an equal key is already present, `insert` leaves the set unchanged and returns
`FALSE`. The first two calls discard the result. The third call supplies the key `31` a second time,
so the variable `inserted` receives `FALSE` and [`size`](../standard-modules/std/set.md#size)
returns 2.

[`std::printf`](../standard-modules/std/format.md#printf) receives both values as [named
arguments](../reference/call-arguments.md#named-arguments). A `BOOL` argument is formatted as `true`
or `false`.
