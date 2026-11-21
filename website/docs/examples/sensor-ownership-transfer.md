# Sensor ownership transfer with unique pointers

The program allocates two sensors and moves, exchanges, and hands over their ownership while each
sensor reports its construction and its destruction. The program demonstrates
[`std::unique_ptr`](../standard-modules/std/unique-ptr.md),
[`std::make_unique`](../standard-modules/std/unique-ptr.md#make_unique),
[`MOVE`](../overview/move-semantics.md), and the [swap operator](../overview/swap-operator.md).

```quxlang
--8<-- "example-programs/programs/sensor_ownership_transfer/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/sensor_ownership_transfer/expected-output.txt"
```

## Explanation

The structure `sensor` carries the [modifier](../reference/structs-and-members.md#struct-modifiers)
`MOVE_ONLY`. `std::make_unique#sensor(@identifier 7)` allocates one `sensor` with `NEW`, forwards
the named argument to the constructor, and returns the owner of the new object. The `AUTO`
declaration deduces the type `std::unique_ptr#sensor` from the initializer.

`std::unique_ptr` is itself `MOVE_ONLY` and cannot be copied. The expression `MOVE(bench_sensor)`
requests an [expiring reference](../reference/references.md#named-temporary-references), which
selects the [move constructor](../standard-modules/std/unique-ptr.md#constructor). The constructor
transfers the ownership to `installed_sensor` and leaves `bench_sensor` empty, as the two results of
[`has_value`](../standard-modules/std/unique-ptr.md#has_value) show. The transfer constructs no
sensor and destroys no sensor.

The statement `installed_sensor <-> replacement_sensor;` applies the [swap
operator](../standard-modules/std/unique-ptr.md#operator-swap) of `std::unique_ptr`, which exchanges
the stored pointers. Afterward, `installed_sensor` owns sensor 8 and `replacement_sensor` owns
sensor 7. The expression `installed_sensor->.identifier` selects a member of the owned object.

The function `decommission` receives a `std::unique_ptr#sensor` by value, and the call moves the
ownership of sensor 7 into the parameter. The
[destructor](../standard-modules/std/unique-ptr.md#destructor) of the parameter deletes the sensor
when the function returns, so the line `sensor 7 closed` precedes the line `leaving main`. The
return from `main` destroys `installed_sensor`, which deletes sensor 8. The two empty owners delete
nothing.
