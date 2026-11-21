# Generic value utilities with deduced types

The program defines the functions `maximum`, `clamp`, `exchange`, and `largest_element` for
arbitrary ordered types and applies them to a structure, to integers, to floating-point values, and
to an array. The program demonstrates [`AUTO(name)` type
deduction](../overview/type-queries-and-deduction.md),
[templates](../overview/templates-and-value-parameters.md) with type and value parameters, and the
[swap operator](../overview/swap-operator.md).

```quxlang
--8<-- "example-programs/programs/generic_value_utilities/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/generic_value_utilities/expected-output.txt"
```

## Explanation

Both parameters of `maximum` have the type pattern `AUTO(value_type)`. The [deduction
pattern](../reference/type-queries-and-deduction.md#deduction-patterns) matches the non-reference
type of the argument, and every occurrence of the name `value_type` must bind the same type. The
return type refers to the deduced type by that name. The first call passes two `version` objects.
The structure `version` declares no comparison operator; the [generated
comparison](../reference/comparison-operators.md#built-in-comparable-types) of a structure is
lexicographic in field order, so version 2.11 is greater than version 2.7.

The function `clamp` uses the same pattern for three parameters. Each call converts its numeric
literals with `AS`, which gives all three arguments one concrete type, `I32` in the first call and
`F64` in the second call.

The parameters of `exchange` have the type `MUT& AUTO(value_type)` and refer to the variables of the
caller. The [swap operator](../reference/swap-operator.md) `<->` exchanges the values of the two
objects.

`largest_element` is a function template with the type parameter `T` and the [value
parameter](../reference/templates-and-value-parameters.md#value-parameters) `element_count`, which
together form the parameter type `CONST& [element_count]T`. The expression `largest_element#(@T I64,
@count 5)` supplies both template arguments by name. The body applies `maximum` to each element
through an [`ITEM` loop](../reference/arrays.md#iteration).
