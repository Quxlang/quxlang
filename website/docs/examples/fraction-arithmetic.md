# Fraction arithmetic with user-defined operators

The program defines a rational number type that stores fractions in lowest terms and evaluates sums,
differences, products, and comparisons of fractions. The program demonstrates [user-defined
operators](../overview/user-defined-operators.md), including an `RHS` operator, equality, and
three-way comparison.

```quxlang
--8<-- "example-programs/programs/fraction_arithmetic/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/fraction_arithmetic/expected-output.txt"
```

## Explanation

The function `greatest_common_divisor` implements the Euclidean algorithm with a [`LOOP WHILE`
statement](../reference/while-loops.md). The function assigns to its value parameters, which are
local objects of the function. The constructor of `fraction` divides the numerator and the
denominator by their greatest common divisor and gives the divisor the sign of the denominator, so
every stored denominator is positive. The declaration of `third` supplies 2 and 6 and stores 1 and
3.

The members `OPERATOR+`, `OPERATOR-`, and `OPERATOR*` are [operator
members](../reference/user-defined-operators.md#operator-member-declarations). Each member has a
`CONST` receiver, receives the right operand as `@OTHER`, and returns a new `fraction`. In the
expression that computes each numerator, `*` binds more tightly than `+` and `-`, as the [operator
precedence table](../reference/operator-precedence.md) specifies.

For the expression `3 * half`, the left operand provides no applicable `OPERATOR*`. The compiler
then selects the member [`OPERATOR*
RHS`](../reference/user-defined-operators.md#binary-lookup-and-rhs) of the right operand, which
receives the left operand as `OTHER`.

[`OPERATOR==`](../reference/user-defined-operators.md#equality-and-ordering) returns `BOOL` and
supplies both `==` and `!=`. A fieldwise comparison is sufficient because both operands are stored
in lowest terms. `OPERATOR<=>` returns `ORDER` and supplies `<`, `>`, `<=`, and `>=`. The member
compares the cross products of the two fractions with the built-in [three-way
comparison](../reference/comparison-operators.md#three-way-comparison) of `I64`, which is correct
because both denominators are positive. In the final statement of `main`, `+` binds more tightly
than `>=`, so the comparison tests `half` against the sum 5/6.
