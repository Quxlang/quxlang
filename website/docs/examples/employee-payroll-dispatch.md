# Payroll computation through virtual dispatch

The program computes the monthly pay of a salaried employee and of an hourly contractor through one
base structure and reports the destruction of each record. The program demonstrates
[inheritance](../overview/inheritance.md) with `POLYMORPHIC` structures, base delegates, virtual and
pure virtual functions, and [`NEW` and `DELETE`](../overview/new-and-delete.md) with virtual
destruction.

```quxlang
--8<-- "example-programs/programs/employee_payroll_dispatch/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/employee_payroll_dispatch/expected-output.txt"
```

## Explanation

`employee` is declared `STRUCT POLYMORPHIC`, the
[category](../reference/inheritance.md#polymorphism-categories) that provides runtime type identity
and [virtual functions](../reference/inheritance.md#virtual-functions). The member `role` is
declared `VIRTUAL` with a body, which introduces a virtual slot together with its base
implementation. The member `monthly_pay` is declared `VIRTUAL(PURE)` and has no body. A type that
has a pure virtual slot is abstract, so an `employee` object cannot be constructed directly.

Each derived structure declares the named base subobject `.employee_part BASE employee` and is
itself `POLYMORPHIC`. The [base
delegate](../reference/inheritance.md#constructors-and-base-delegates) `:>
.employee_part:(@identifier identifier)` passes the identifier to the constructor of the base
subobject, which is constructed before the fields and before the constructor body. Both derived
structures override `monthly_pay` with `VIRTUAL(OVERRIDE)`. Only `hourly_employee` overrides `role`,
so a `salaried_employee` object uses the base implementation.

The function `print_pay` receives a `CONST& employee`. A reference to a derived structure [converts
implicitly](../reference/inheritance.md#static-hierarchy-conversions) to a reference to its base,
and the calls to `role` and `monthly_pay` dispatch to the implementations of the runtime type. The
expression `member.identifier` reads the field of the base subobject.

The [`NEW` expression](../reference/new-and-delete.md#named-and-positional-constructor-arguments)
allocates and constructs an `hourly_employee` object, and the resulting pointer converts to
`MUT->employee`. The expression `contractor->` dereferences the pointer. A polymorphic structure has
a virtual destructor, so [`DELETE`](../reference/new-and-delete.md#polymorphic-deletion) through the
base pointer invokes the destructor of `hourly_employee`. The body of that destructor executes
first, and the destructor of the base subobject executes afterward. The local object `manager` is
destroyed when `main` returns, after the line `end of main` is printed.
