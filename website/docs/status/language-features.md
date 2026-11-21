# Language Features

This page assigns one status to each page of the language
[Reference](../reference/index.md) and lists every individual form that is
reserved, parsed without an implementation, or restricted to particular
targets.

The [status vocabulary](index.md#status-vocabulary) defines each status. A
status describes the LLVM backend and constant evaluation. The experimental
Cortado backend differs for the features that
[Backends](backends.md#feature-availability-by-backend) lists; the tables on
this page name those features in the notes column.

## Status by reference page

### Language syntax

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Lexical Structure](../reference/lexical-structure.md) | Implemented | None |
| [Source Files and Imports](../reference/source-files-and-imports.md) | Implemented | None |
| [Namespaces](../reference/namespaces.md) | Implemented | None |
| [Declaration Documentation](../reference/declaration-documentation.md) | Implemented | None |
| [Privacy](../reference/privacy.md) | Implemented | A base declaration cannot carry declaration privacy. |
| [Target Availability](../reference/availability-and-targets.md) | Partially implemented | `TARGET("name")`, the kernel predicates, and `OS_BSD` are not implemented. `UNWIND_FORMAT_IS_DWARF_EH_FRAME` is the only unwind-format predicate that is `TRUE` on a target that generates output. |

### Declarations, values, and types

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Variables](../reference/variables.md) | Implemented | The declaration tags `CONSTEXPR_READABLE` and `CONSTEXPR_READWRITE` are reserved syntax. |
| [Thread-Local Variables](../reference/thread-local-variables.md) | Implemented | Cortado: registration of `PER_THREAD` destructors is rejected. |
| [`STATIC` Constants](../reference/static-compile-time-constants.md) | Implemented | The compiler rejects an initializer argument list on a `STATIC` constant of a compile-time constant category. |
| [Primitive Types and Literals](../reference/primitive-types-and-literals.md) | Implemented | The LLVM backend represents the floating-point formats `F16E5`, `F16E8`, `F32E8`, `F64E11`, and `F128E15`. Other formats exist only in constant evaluation. |
| [Arrays](../reference/arrays.md) | Implemented | None |
| [Composites](../reference/composites.md) | Implemented | Layout-dependent operations require a target with byte layout. |
| [References](../reference/references.md) | Implemented | None |
| [Pointers](../reference/pointers.md) | Implemented | GC pointers (`~>T`) are usable only on a managed-runtime target, which is the experimental Cortado backend. |
| [Optional and Owning Values](../reference/optional-and-owning-values.md) | Implemented | None |
| [Type Queries and Deduction](../reference/type-queries-and-deduction.md) | Implemented | `TYPE_IS_POLYMORPHIC` does not resolve `THISTYPE`. `ALIGNOF` and `CELLSIZEOF` are unavailable on layoutless targets. Cortado: `DYNAMIC_TYPE_OF` is not implemented. |
| [Conversions](../reference/conversions.md) | Implemented | Cortado: the tests of failing `AS CHECKED` conversions are annotated as known broken. |

### Functions and generic programming

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Functions](../reference/functions-and-parameters.md) | Implemented | The `@...` spelling for keyword capture is rejected; `@KWARGS ...` is the accepted form. |
| [C++-style Coroutines](../reference/cpp-style-coroutines.md) | Implemented | The compiler rejects a virtual `COROUTINE` declaration. Cortado: unverified. |
| [Thread-pool Coroutines](../reference/thread-pool-coroutines.md) | Implemented | The thread pool requires `std::thread`, which is available on Linux with the `static` or `glibc` environment, on macOS, and on Windows. Cortado: unverified. |
| [`RETURN` Statements](../reference/return-statements.md) | Implemented | None |
| [Call Arguments](../reference/call-arguments.md) | Implemented | None |
| [Default Arguments](../reference/default-arguments.md) | Implemented | The compiler rejects a default for a static-value parameter. |
| [Overload Resolution](../reference/overload-resolution.md) | Implemented | None |
| [Procedure Pointers and Function Values](../reference/procedure-pointers-and-function-values.md) | Partially implemented | Conversion of a bound member to a runtime procedure pointer is not implemented. |
| [Templates](../reference/templates-and-value-parameters.md) | Implemented | None |
| [Variadic Packs](../reference/variadic-packs.md) | Implemented | None |
| [Packs](../reference/packs.md) | Implemented | None |
| [Lambdas](../reference/lambdas.md) | Implemented | None |
| [Interfaces and Implementations](../reference/interfaces-and-implementations.md) | Implemented | None |
| [Generics](../reference/generics.md) | Implemented | None |

### Expressions and statements

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Operator Precedence](../reference/operator-precedence.md) | Implemented | None |
| [Assignment Operators](../reference/assignment-operators.md) | Implemented | None |
| [Increment and Decrement](../reference/increment-and-decrement.md) | Implemented | None |
| [Swap Operator](../reference/swap-operator.md) | Implemented | None |
| [Move Semantics](../reference/move-semantics.md) | Implemented | None |
| [Arithmetic Operators](../reference/arithmetic-operators.md) | Implemented | The language defines no `%?` or `%!` operator. Cortado: failure of the checked operators is incomplete. |
| [Comparison Operators](../reference/comparison-operators.md) | Implemented | None |
| [Floating-Point Ordering](../reference/floating-point-ordering.md) | Implemented | None |
| [Logical Operators](../reference/logical-operators.md) | Implemented | None |
| [Bitwise Operators](../reference/bitwise-operators.md) | Implemented | Linux x86: the test of 64-bit rotation cycles is annotated as known broken. |
| [User-Defined Operators](../reference/user-defined-operators.md) | Implemented | None |
| [Conditional Statements](../reference/conditional-statements.md) | Implemented | None |
| [`LOOP WHILE` Loops](../reference/while-loops.md) | Implemented | None |
| [`LOOP` Statements](../reference/loop-statements.md) | Implemented | None |
| [Labels and `GOTO`](../reference/labels-and-goto.md) | Implemented | None |
| [Exception Handling](../reference/exceptions.md) | Implemented | Translation of foreign C++ or SEH exceptions and unwinding through external shared libraries are not implemented. Execution on Windows is unverified. Cortado: not implemented. |
| [Failure Statements](../reference/diagnostics-and-failure.md) | Implemented | None |
| [Compilation Policies](../reference/compilation-policies.md) | Implemented | None |

### Structures, lifetime, and alternatives

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Structures](../reference/structs-and-members.md) | Implemented | None |
| [Public Field Reflection](../reference/public-field-reflection.md) | Implemented | A selector that is evaluated separately, such as a `STATIC_WHILE` condition, cannot resolve `DECLTYPE` of a surrounding runtime local. |
| [Inheritance](../reference/inheritance.md) | Partially implemented | An anonymous `.BASE` requires exactly one active direct base and cannot be virtual. Covariant return types, dynamic reference casts, and a complete-object dynamic cast are not implemented. Cortado: inheritance is not implemented. |
| [Constructors and Destructors](../reference/constructors-and-destructors.md) | Implemented | None |
| [Object Storage and Lifetime](../reference/typed-storage-and-lifetime.md) | Implemented | `ALIGNED_STORAGE` is unavailable on layoutless targets. |
| [Compile-Time Allocation](../reference/constexpr-allocation.md) | Implemented | None |
| [`NEW` and `DELETE`](../reference/new-and-delete.md) | Implemented | Cortado: polymorphic allocation, virtual destruction, and allocation-info recovery are not implemented. |
| [Enums](../reference/enums.md) | Implemented | None |
| [Flagsets](../reference/flagsets.md) | Implemented | None |
| [Unions](../reference/unions.md) | Implemented | None |
| [Variants](../reference/variants.md) | Implemented | None |
| [`VISIT`](../reference/visit.md) | Implemented | None |
| [`MATCH`](../reference/match.md) | Implemented | None |

### Evaluation, interoperation, and execution

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Compile-Time Evaluation](../reference/compile-time-evaluation.md) | Implemented | One static test of the runtime module is annotated `KNOWN_FAILING`. See [Known Issues](known-issues.md#tests-annotated-known_failing). |
| [Runtime Selection](../reference/runtime-selection.md) | Implemented | None |
| [Serialization](../reference/serialization.md) | Implemented | None |
| [Stringlike Types](../reference/stringlike-types.md) | Implemented | None |
| [Variable-Length Integer Serialization](../reference/integer-serialization.md) | Implemented | None |
| [External Types](../reference/external-types.md) | Target-restricted | An `EXTERN_TYPE` declaration is accepted on every target. Values are reachable only through GC pointers, which require the experimental Cortado backend. |
| [External Memory](../reference/external-memory.md) | Implemented | `IBC_GETADDR`, `IBC_PUN`, `IBC_LOAD`, and `IBC_WRITE` fail during constant evaluation and are unavailable on the JVM. |
| [External Procedures](../reference/external-procedures.md) | Target-restricted | Dynamic imports are implemented for the `glibc` environment on Linux, for macOS, and for Windows. On ELF targets the linker resolves only the library name `glibc`. The PE linker rejects `OPTIONAL` imports. The JVM call conventions require the experimental Cortado backend. |
| [Assembly Procedures](../reference/assembly-procedures.md) | Partially implemented | `ASM_INLINE_FUNCTION` is not implemented. `EXTERNAL("C", …)` and `EXTERNAL("LINKER", …)` operands are not implemented end to end for ARM-family assembly. Assembly procedures cannot run during constant evaluation. |
| [Atomics](../reference/atomics.md) | Implemented | Atomic storage wider than the native atomic width of the target CPU is not implemented. Linux x86: the test of 64-bit read-modify-write operations is annotated as known broken. Cortado: read-modify-write operations are not implemented. |
| [Tests](../reference/tests.md) | Implemented | The annotations `KNOWN_BROKEN`, `KNOWN_BROKEN_IF`, `KNOWN_FAILING`, and `KNOWN_FAILING_IF` are implemented. [Known Issues](known-issues.md#test-annotations) defines them. |

### Toolchain and output

| Feature | Status | Restrictions and notes |
| --- | --- | --- |
| [Toolchain](../reference/toolchain.md) | Implemented | None |
| [`qxcbuild.yml`](../reference/qxcbuild-file.md) | Implemented | The output kinds `shared_library`, `static_library`, and `image` and the binary format `wasm` are reserved syntax. See [Targets](targets.md#output-kinds). |
| [Build Options](../reference/build-options.md) | Implemented | None |
| [CPU Capabilities and Steppings](../reference/cpu-capabilities-and-steppings.md) | Implemented | Runtime selection of a capability requires a detector in the runtime module. The runtime module in the test bundle defines detectors for x64 capabilities, for the x86 vendor attributes, and for ARM64 capabilities on macOS. |
| [Program Startup and Runtime Hooks](../reference/program-startup-and-runtime-hooks.md) | Implemented | The runtime module in the test bundle defines `PROGRAM_START` for every Linux CPU, for Windows x64, and for macOS ARM64. |
| [Runtime Module Contracts](../reference/runtime-module-contracts.md) | Implemented | None |
| [Backends and Layout](../reference/backends-and-layout.md) | Implemented | See [Backends](backends.md). |
| [Compiler Output](../reference/compiler-output.md) | Implemented | The `build` and `check` commands are implemented. |

## Reserved syntax

The compiler or the configuration loader accepts each of the following
spellings and assigns no behavior to it.

| Form | Accepted by | Stated by |
| --- | --- | --- |
| `CONSTEXPR_READABLE` and `CONSTEXPR_READWRITE` between `VAR` and the type | Declaration parser | [Variables](../reference/variables.md#reserved-declaration-tags) |
| Output kinds `shared_library`, `static_library`, and `image` | Configuration loader | [`qxcbuild.yml`](../reference/qxcbuild-file.md#outputs) |
| Binary format `wasm` and the predicate `BINARY_WASM` | Configuration loader and expression parser | Compiler source; the LLVM backend has no target triple for the format |
| `OS_BSD` and the kernel predicates `KERNEL_LINUX`, `KERNEL_NT`, `KERNEL_BSD`, and `KERNEL_XNU` | Keyword table | [Target Availability](../reference/availability-and-targets.md#unsupported-expressions) |
| Environments `musl`, `bionic`, `ucrt`, `cygwin`, and `freestanding` | Configuration loader | [`qxcbuild.yml`](../reference/qxcbuild-file.md#target-identity). Each value sets its `ENVIRONMENT_IS_*` predicate; the compiler and the standard modules contain no further support. |

## Forms that are parsed and not implemented

| Form | Behavior | Stated by |
| --- | --- | --- |
| `ASM_INLINE_FUNCTION` with register-bound `CALLABLE` and `CLOBBER` clauses | Rejected with `ASM_INLINE_FUNCTION backend support is not implemented` | [Assembly Procedures](../reference/assembly-procedures.md#implementation-status) |
| `EXTERNAL("C", "symbol")` and `EXTERNAL("LINKER", "symbol")` operands in ARM-family assembly | Not implemented end to end | [Assembly Procedures](../reference/assembly-procedures.md#implementation-status) |
| `TARGET("name")` expression | The parser accepts the expression; generation fails as unimplemented | [Target Availability](../reference/availability-and-targets.md#unsupported-expressions) |
| Conversion of a bound member to a runtime procedure pointer | Rejected with `Bound method procedure pointers are not yet supported` | [Procedure Pointers and Function Values](../reference/procedure-pointers-and-function-values.md) |
| Covariant return types of virtual functions | Virtual signatures require exact parameter and return types | [Inheritance](../reference/inheritance.md#virtual-functions) |
| Dynamic reference casts and a complete-object dynamic cast | Not implemented; dynamic casts apply to pointers | [Inheritance](../reference/inheritance.md#dynamic-casts) |
| Translation of C++ or SEH exceptions and unwinding through external shared libraries | Not implemented | [Exception Handling](../reference/exceptions.md#constant-evaluation-and-native-support) |

## Restrictions diagnosed by the compiler

The compiler source rejects the following forms. The Reference does not state
these restrictions.

| Form | Diagnostic | Related page |
| --- | --- | --- |
| A virtual `COROUTINE` member declaration | `Virtual COROUTINE declarations are not supported` | [C++-style Coroutines](../reference/cpp-style-coroutines.md), [Inheritance](../reference/inheritance.md) |
| A default for a static-value parameter | `Defaulted static-value parameters are not supported` | [Default Arguments](../reference/default-arguments.md), [Templates](../reference/templates-and-value-parameters.md) |
| An initializer argument list on a `STATIC` declaration of type `STRING_CONSTANT` or `NUMERIC_CONSTANT`, on a serialoid `STATIC` declaration, or on an antestatal `STATIC` declaration | `… initializer argument lists are not implemented` | [`STATIC` Constants](../reference/static-compile-time-constants.md) |
| Atomic load, store, exchange, compare-exchange, or read-modify-write on storage wider than the native atomic width | `Non-native atomic … lowering is not implemented for storage width …` | [Atomics](../reference/atomics.md) |
| An optional import in a Windows PE output | `Optional Windows imports are not supported by static PE import tables` | [External Procedures](../reference/external-procedures.md) |
| A dynamic library name other than `glibc` on an ELF target | `ELF dynamic library is not supported for this target environment` | [External Procedures](../reference/external-procedures.md) |
| The `glibc` environment on a CPU other than x64, x86, or ARM64 | `glibc dynamic linking is not supported for this CPU target` | [`qxcbuild.yml`](../reference/qxcbuild-file.md#target-identity) |

## Target-restricted forms

| Form | Available on | Stated by |
| --- | --- | --- |
| `ASM_PROCEDURE` with the tag `X64`, `X86`, `ARM64`, `ARM32`, `RISCV32`, `RISCV64`, or `Z_ARCH` | A target whose CPU matches the tag; native runtime code only | [Assembly Procedures](../reference/assembly-procedures.md#execution-and-target-support) |
| Inheritance, `DYNAMIC_TYPE_OF`, unchecked static downcasts, and dynamic casts | LLVM targets and constant evaluation | [Inheritance](../reference/inheritance.md), [Type Queries and Deduction](../reference/type-queries-and-deduction.md#dynamic-type-identity) |
| Exception handling at run time | LLVM targets | [Exception Handling](../reference/exceptions.md#constant-evaluation-and-native-support) |
| `SIZEOF` for types other than the eight fixed-width integers, `ALIGNOF`, `CELLSIZEOF`, and `ALIGNED_STORAGE` | Targets with byte layout | [Backends and Layout](../reference/backends-and-layout.md) |
| `IBC_GETADDR`, `IBC_PUN`, `IBC_LOAD`, and `IBC_WRITE` | LLVM targets at run time | [External Memory](../reference/external-memory.md#runtime-address-operations) |
| Atomic read-modify-write members | LLVM targets, within the native atomic width | [Atomics](../reference/atomics.md#integer-read-modify-write-operations) |
| Runtime-sized array allocation and generator construction | LLVM targets and constant evaluation | [`NEW` and `DELETE`](../reference/new-and-delete.md#runtime-sized-arrays) |
| `HAVE_<CAPABILITY>` queries with a detected result | A CPU family for which the runtime module defines a detector; a query for another CPU family is `FALSE` | [CPU Capabilities and Steppings](../reference/cpu-capabilities-and-steppings.md) |
| JVM call conventions (`JVM_STATIC` through `JVM_PUTSTATIC`) and GC pointers | The experimental Cortado backend | [External Procedures](../reference/external-procedures.md#jvm-call-conventions), [External Types](../reference/external-types.md) |

## Implemented features without a reference page

The test bundle uses the following features, and the Reference contains no
page that specifies them.

| Feature | Evidence |
| --- | --- |
| `BENCHMARK` declarations with `BENCHMARK_LOOP` and `ONESHOT` bodies, and the output kinds `benchmark_suite` and `benchmark_executable` | The `benchmarks` and `benchmark_files` modules of the test bundle; four benchmark outputs for `macos-arm64` and two for `linux-x64` |
| Test annotations `KNOWN_BROKEN`, `KNOWN_BROKEN_IF`, `KNOWN_FAILING`, and `KNOWN_FAILING_IF` | 322 annotated test declarations in the test bundle |
| Command-line options of a unit-test suite: `--list-tests`, `--test-filter=PATTERN`, `--include-failing-tests`, and `--help` | `UNIT_TEST_MAIN` in the runtime module |
| The `--debug-timing` option of `qxc build` and `qxc check` | Usage text of the compiler |

## Semantics that the Reference marks as subject to change

The Reference identifies the following implemented behaviors as likely to
change in a later revision.

| Behavior | Stated by |
| --- | --- |
| Bypass of virtual dispatch through a qualified member call | [Inheritance](../reference/inheritance.md#inherited-member-lookup) |
| Construction order of virtual bases, direct bases, fields, and the constructor body | [Inheritance](../reference/inheritance.md#constructors-and-base-delegates) |
| A dynamic cast of a null pointer, which currently yields null | [Inheritance](../reference/inheritance.md#dynamic-casts) |
| The restriction of an anonymous `.BASE` to a single nonvirtual base | [Inheritance](../reference/inheritance.md#direct-base-declarations) |
