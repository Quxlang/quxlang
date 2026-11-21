# Sensor polling through generics and interfaces

The program polls two unrelated sensor types through one owning generic type and writes the samples
in two interchangeable report styles. The program demonstrates structural type erasure with
[`GENERIC`](../overview/generics.md) and nominal dispatch with [`INTERFACE` and
`IMPLEMENTATION`](../overview/interfaces-and-implementations.md).

```quxlang
--8<-- "example-programs/programs/sensor_polling_generic/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/sensor_polling_generic/expected-output.txt"
```

## Explanation

The declaration `::sensor GENERIC` lists the
[operations](../reference/generics.md#operation-declarations) `label`, with a `CONST` receiver, and
`sample`, with a `MUT` receiver. The structures `thermometer` and `door_counter` are unrelated types
that provide member functions with these names and signatures. Neither structure refers to `sensor`;
a concrete type satisfies a generic by providing a callable member for every operation.

The array `sensors` holds two `sensor` values that are initialized from the object `hallway` and
from a default-constructed `door_counter`. An [owning
generic](../reference/generics.md#owning-generic) copy-constructs or move-constructs the concrete
value into storage that the generic owns. A call through the generic therefore modifies the stored
copy, and the final line shows that `hallway` retains the value 205.

`report_style` is an
[`INTERFACE`](../reference/interfaces-and-implementations.md#interface-declarations) with the single
dispatch slot `write`. The declarations `plain_style` and `tagged_style` are
[implementations](../reference/interfaces-and-implementations.md#implementation-declarations) of
that interface, and each provides a nested function `::write` that matches the slot by name and
signature. The name of an implementation is the value that [selects its dispatch
table](../reference/interfaces-and-implementations.md#constructing-and-assigning-handles), so `main`
passes `plain_style` and `tagged_style` as the `@style` argument. An interface value identifies an
implementation and owns no object.

The function `poll` receives the array by mutable reference and calls `label` and `sample` on each
element through the generic. The call `style.write` dispatches through the implementation that the
handle currently identifies. The second call to `poll` continues from the state that the first call
left in the stored sensors: the temperature advances from 209 to 213, and the door counter advances
from 1 to 2.
