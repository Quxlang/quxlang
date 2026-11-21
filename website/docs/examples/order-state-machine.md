# Order state machine with enumerations

The program advances an order through its states in response to a sequence of events and ignores
each event that is invalid in the current state. The program demonstrates
[enumerations](../overview/enums.md) with a fixed width, a default value, an explicit value, an
associated function, and ordering comparisons.

```quxlang
--8<-- "example-programs/programs/order_state_machine/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/order_state_machine/expected-output.txt"
```

## Explanation

`order_state` is an [`ENUM`](../reference/enums.md#declaration-forms) with the representation width
`BITS(8)`. Entries without an explicit value are [assigned in declaration
order](../reference/enums.md#explicit-and-implicit-values) starting at zero, and `cancelled`
receives the explicit value 100. The entry `received` is marked
[`DEFAULT`](../reference/enums.md#default-and-null), so default construction selects it. The body
after the entry list declares the [associated
function](../reference/enums.md#associated-declarations-and-serialization) `name`, which the program
calls as `order_state::name(state)`. `order_event` declares no width, and the compiler infers the
smallest sufficient unsigned width.

The function `advance` computes the successor state. Values of one enumeration support [equality and
ordering](../reference/enums.md#names-comparison-and-conversion) according to their integer
representations, so the condition `state < order_state::shipped` holds for the states `received`,
`paid`, and `packed`. A cancellation event is accepted only in those states. Every other combination
of state and event that the function does not list returns the state unchanged.

In `main`, the declaration `VAR state order_state;` default-constructs the state `received`, and an
[`ITEM` loop](../reference/arrays.md#iteration) processes the six events of the array. The second
event, `dispatch`, arrives in the state `paid` and is ignored. The cancellation arrives after the
order has reached the state `shipped` and is also ignored, so the final event completes the
delivery.
