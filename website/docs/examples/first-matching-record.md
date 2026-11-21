# First matching record as an optional value

The program searches a sequence of station readings for the first reading above a threshold and
represents the absence of such a reading with an empty optional. The program demonstrates
[`std::dynarr`](../standard-modules/std/dynarr.md),
[`std::optional`](../standard-modules/std/optional.md), and a value-projection [`LOOP`
statement](../overview/loop-statements.md).

```quxlang
--8<-- "example-programs/programs/first_matching_record/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/first_matching_record/expected-output.txt"
```

## Explanation

The structure `reading` declares two fields and no constructor. The declaration `VAR entry reading;`
default-constructs the object, and `main` assigns its fields before each call to
[`push_back`](../standard-modules/std/dynarr.md#push_back). `push_back` receives its argument by
value, so each call stores an independent copy and the variable `entry` is reused for the next
reading.

The function `first_reading_above` returns `std::optional#reading`. The clause `VALUE(candidate)` of
the [iterator loop](../reference/loop-statements.md#range-projections) iterates the `VALUES()` range
of the array and binds each element. The statement `RETURN candidate;` converts the `reading` object
to an engaged optional through the [converting
constructor](../standard-modules/std/optional.md#constructor) of `std::optional`, and the statement
`RETURN NULL;` produces an empty optional.

The function `report` tests the result with
[`has_value`](../standard-modules/std/optional.md#has_value). The expression `match->.station`
applies the [dereference operator](../standard-modules/std/optional.md#operator-dereference) of the
optional and selects a member of the contained value. With the threshold 25, the search returns the
reading of station 12, which precedes the higher reading of station 13. With the threshold 40, no
reading qualifies and the optional is empty.

[`reset`](../standard-modules/std/optional.md#reset) destroys the contained value and leaves the
optional empty, so the final call to `has_value` returns `FALSE`.
