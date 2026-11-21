# Quxlang Overview

The Overview introduces Quxlang through examples. Each page presents representative code for one
feature, states the purpose and typical applications of the feature, and summarizes the relevant
performance and implementation tradeoffs. The [Language Reference](../reference/index.md)
specifies exact syntax, semantic rules, restrictions, and edge cases for the same topics.

## Getting started

| Page | Contents |
| --- | --- |
| [Getting Started](getting-started.md) | Required tools, the compiler build, and the compiler commands |
| [First Program](first-program.md) | One complete program from source file to execution |
| [Source Bundles and Targets](source-bundles.md) | The compilation unit and the target model |
| [The `qxcbuild.yml` File](qxcbuild-file.md) | Targets, module mappings, build options, and outputs |
| [Syntax at a Glance](syntax-examples.md) | A compact table of the principal syntactic forms |

The ordinary development workflow has the following shape.

```text
source files -> qxcbuild.yml -> configured output -> executable or unit-test suite
```

## Topics

| Subject | Overview pages |
| --- | --- |
| Source organization | [Lexical Structure](lexical-structure.md), [Source Files and Imports](source-files-and-imports.md), [Namespaces](namespaces.md), [Privacy](privacy.md), [Declaration Documentation](declaration-documentation.md) |
| Values and types | [Variables](variables.md), [`STATIC` Constants](static-compile-time-constants.md), [Primitive Types](primitive-types-and-literals.md), [Arrays](arrays.md), [Composites](composites.md), [References](references.md), [Pointers](pointers.md), [Optional and Owning Values](optional-and-owning-values.md), [Conversions](conversions.md), [Type Queries](type-queries-and-deduction.md), [Stringlike Types](stringlike-types.md) |
| Functions | [Functions](functions-and-parameters.md), [`RETURN` Statements](return-statements.md), [Call Arguments](call-arguments.md), [Default Arguments](default-arguments.md), [Overload Resolution](overload-resolution.md), [Procedure Pointers and Function Values](procedure-pointers-and-function-values.md), [Lambdas](lambdas.md) |
| Operators | [Operator Precedence](operator-precedence.md), [Assignment Operators](assignment-operators.md), [Increment and Decrement](increment-and-decrement.md), [Swap Operator](swap-operator.md), [Move Semantics](move-semantics.md), [Arithmetic Operators](arithmetic-operators.md), [Comparison Operators](comparison-operators.md), [Floating-Point Ordering](floating-point-ordering.md), [Logical Operators](logical-operators.md), [Bitwise Operators](bitwise-operators.md), [User-Defined Operators](user-defined-operators.md) |
| Control flow | [Conditional Statements](conditional-statements.md), [`LOOP WHILE` Loops](while-loops.md), [`LOOP` Statements](loop-statements.md), [Labels and `GOTO`](labels-and-goto.md), [Exception Handling](exceptions.md), [Failure Statements](diagnostics-and-failure.md) |
| User-defined types | [Structures](structs-and-members.md), [Constructors and Destructors](constructors-and-destructors.md), [Inheritance](inheritance.md), [Public Field Reflection](public-field-reflection.md), [Enums](enums.md), [Flagsets](flagsets.md), [Unions](unions.md), [Variants](variants.md), [`VISIT`](visit.md), [`MATCH`](match.md) |
| Generic programming | [Templates](templates-and-value-parameters.md), [Variadic Packs](variadic-packs.md), [Packs](packs.md), [Interfaces](interfaces-and-implementations.md), [Generics](generics.md) |
| Storage and lifetime | [Object Storage and Lifetime](typed-storage-and-lifetime.md), [`NEW` and `DELETE`](new-and-delete.md), [Compile-Time Allocation](constexpr-allocation.md) |
| Compile-time evaluation and configuration | [Compile-Time Evaluation](compile-time-evaluation.md), [Runtime Selection](runtime-selection.md), [Target Availability](availability-and-targets.md), [Build Options](build-options.md), [Compilation Policies](compilation-policies.md) |
| Interoperation | [Serialization](serialization.md), [Integer Serialization](integer-serialization.md), [External Types](external-types.md), [External Memory](external-memory.md), [External Procedures](external-procedures.md), [Assembly Procedures](assembly-procedures.md) |
| Concurrency | [Atomics](atomics.md), [Thread-Local Variables](thread-local-variables.md), [Coroutines](coroutines.md) |
| Testing | [Tests](tests.md) |

## Performance and implementation considerations

The following pages describe decisions that affect generated code or storage.
[Composites](composites.md) describes temporary heterogeneous records, and [Packs](packs.md)
describes argument forwarding without intermediate records. [Move Semantics](move-semantics.md)
and [Swap Operator](swap-operator.md) describe transfers of resource-owning values.
[Inheritance](inheritance.md) describes base-subobject layout and optional dynamic dispatch.
[Compile-Time Allocation](constexpr-allocation.md) and
[Object Storage and Lifetime](typed-storage-and-lifetime.md) describe explicit storage decisions.
[Floating-Point Ordering](floating-point-ordering.md) compares the total value order with the
IEEE predicates. [Build Options](build-options.md) and
[Compilation Policies](compilation-policies.md) describe per-output behavior.

## Related sections

- The [Language Reference](../reference/index.md) specifies each feature completely.
- The [Standard Modules Reference](../standard-modules/index.md) documents the `std`, `io`, and
  `net` modules.
- [Example Programs](../examples/index.md) contains complete programs with their output.
- [Implementation Status](../status/index.md) records the supported targets, backends, and
  features.
