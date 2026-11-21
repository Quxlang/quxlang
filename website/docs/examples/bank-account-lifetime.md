# Bank account transfers and object lifetime

The program opens three bank accounts, transfers money between them, and reports the construction
and the destruction of every account. The program demonstrates
[structures](../overview/structs-and-members.md), [constructors and
destructors](../overview/constructors-and-destructors.md), receiver qualifiers on member functions,
reference parameters, and scope-based destruction.

```quxlang
--8<-- "example-programs/programs/bank_account_lifetime/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/bank_account_lifetime/expected-output.txt"
```

## Explanation

The structure `account` declares two fields, one constructor, and one destructor. The
[constructor](../reference/constructors-and-destructors.md#ordinary-construction) assigns both
fields and prints a line, and the
[destructor](../reference/constructors-and-destructors.md#destruction) prints the final balance.
Inside a member body, `.number` and `.balance` designate the fields of the current object.

The suffix after each member parameter list is a [receiver
qualifier](../reference/structs-and-members.md#receiver-qualifiers). `deposit` and `withdraw` are
declared `MUT` and receive mutable access to the account, and `can_cover` is declared `CONST` and
receives constant access. Each of these functions has one parameter, which is declared as `@ARG` and
bound to the local name `amount`; a call supplies [one bare
argument](../reference/call-arguments.md#the-single-bare-arg-form). In `withdraw`, the [`UNLESS`
statement](../reference/conditional-statements.md) executes its block when the condition is false.

The function `transfer` declares three named parameters. The two `MUT& account` [reference
parameters](../reference/functions-and-parameters.md#parameter-types-and-access) give the function
write access to the accounts of the caller. The first transfer requests 900 from an account that
holds 375 after the deposit, so `withdraw` returns `FALSE` and neither balance changes.

The declaration `VAR checking account :(@number 1001, @opening_balance 250);` passes named arguments
to the constructor. The inner block of `main` declares `escrow`, and the end of that block [destroys
the object](../reference/variables.md#local-scope-and-lifetime) before the line `end of main` is
printed. The return from `main` destroys the remaining local objects in the reverse of their
construction order, so the destructor of `savings` executes before the destructor of `checking`.
