# Shape areas with variants

The program stores circles, rectangles, and empty slots in an array of variants, describes each
slot, and totals the areas of the stored shapes. The program demonstrates
[`INLINE_VARIANT`](../overview/variants.md), `ISA`, `UNWRAP`, [`MATCH`](../overview/match.md) with
guarded arms, and [`VISIT`](../overview/visit.md).

```quxlang
--8<-- "example-programs/programs/shape_area_variant/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/shape_area_variant/expected-output.txt"
```

## Explanation

The declaration `::shape INLINE_VARIANT [circle DEFAULT, rectangle, VOID];` defines a
[variant](../reference/variants.md#declaration-forms) that stores its payload inside the variant
object. The marker `DEFAULT` selects `circle` for default construction, and `VOID` is an alternative
without a payload. The array declaration default-constructs four variants, each with an active
`circle`. The three assignments in `main` [select an
alternative](../reference/variants.md#construction) through the type of the assigned value: a
`rectangle` value selects the `rectangle` alternative, and `NULL` selects `VOID`.

The expression `shapes[0] ISA circle` [tests the active alternative](../reference/variants.md#isa).
The expression `UNWRAP shapes[0] INTO circle` [returns a reference](../reference/variants.md#unwrap)
to the payload. The subject is mutable, so the reference permits the assignment to `radius`.

In `describe`, the [`MATCH` statement](../reference/match.md#matching-variants) selects an arm by
the active alternative, and the header clause `AS payload` binds a reference to the payload in every
arm that has one. The two `rectangle` arms form a [guarded
sequence](../reference/match.md#guards-and-otherwise): the `WHERE` arm handles a rectangle with
equal sides, and the `OTHERWISE` arm handles every other rectangle. The `TYPE VOID` arm has no
payload binding.

The [`VISIT` statement](../reference/visit.md#specialized-region) in `main` generates its block once
for each alternative other than `VOID`. Inside the block, the name `item` denotes the active
payload, so ordinary overload resolution selects the `area` overload for a `circle` or for a
`rectangle`. When `VOID` is active, execution [skips the
block](../reference/visit.md#void-alternatives), and the program therefore counts three shapes. The
literal `3.141592653589793` has no exact `F64` representation; the
[conversion](../reference/conversions.md#modes) `AS APPROXIMATE F64` permits the inexact result.
