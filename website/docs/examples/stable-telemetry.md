# Stable element references during telemetry growth

The program stores a pointer to the first reading of a growing sequence, appends sixteen further
readings, and then modifies the first reading through the stored pointer. The program demonstrates
the address stability of [`std::geometric_array`](../standard-modules/std/geometric-array.md) and
[instance pointers](../overview/pointers.md).

```quxlang
--8<-- "example-programs/programs/stable_telemetry/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/stable_telemetry/expected-output.txt"
```

## Explanation

[`push_back`](../standard-modules/std/geometric-array.md#push_back) appends the first reading. The
[index operator](../standard-modules/std/geometric-array.md#operator-index) returns a mutable
reference to that element, and the postfix `<-` operator converts the reference to an [instance
pointer](../reference/pointers.md#instance-pointers) of type `MUT->I32`.

The [numeric sequence loop](../reference/loop-statements.md#numeric-sequence-loops) appends the
values 0 through 15. A geometric array stores its elements in separately allocated segments whose
capacities are successive powers of two. Growth allocates further segments and never relocates an
existing element, so the pointer `first` continues to designate the first element.

The assignment `first-> := 22` dereferences the pointer and replaces the first reading. The final
statement reads the element through the array and prints the value 22 together with the element
count 17.
