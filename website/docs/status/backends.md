# Backends

A backend translates the compiler's intermediate representation into an
output artifact. The compiler contains two backends. The `backend` key of a
target selects one of them; the
[`qxcbuild.yml` reference](../reference/qxcbuild-file.md) specifies the key.

| Backend | `backend` value | Output | Status |
| --- | --- | --- | --- |
| LLVM | `llvm` | Native executables for Linux (ELF), macOS (Mach-O), and Windows (PE) | Implemented. Every target in the test bundle uses this backend. |
| Cortado | `experimental_cortado` | Executable JAR files for the Java Virtual Machine | Experimental. The test bundle configures no target for this backend. |
| CLI/CIL | None | None | Planned. The repository contains no code for this backend. |

A native target uses the LLVM backend by default. A target with
`platform: jvm` or `cpu: jvm` uses the Cortado backend by default. The
configuration loader rejects the LLVM backend on a JVM target and the Cortado
backend on a native target.

## LLVM backend

The LLVM backend generates machine code through LLVM and links the result
with linkers that are part of the compiler: an ELF linker, a Mach-O linker,
and a PE linker.
[Targets](targets.md) lists the supported platform and CPU combinations.

The native exception runtime is the Quxlang unwinder in the runtime module.
The runtime does not depend on libunwind or libgcc.

### Build types

The `build_type` key accepts ten names. Names ignore ASCII case and
underscores.

| Build type | Assertions, bounds checks, and overflow checks | `UNIMPLEMENTED` panics |
| --- | --- | --- |
| `Development` (default), `Debug`, `DebugOpt`, `DebugCompact`, `DebugCompactOpt` | Enabled | Enabled |
| `Quick`, `Compact`, `CompactOpt` | Disabled | Enabled |
| `Release`, `ReleaseDbgSym` | Disabled | Disabled |

The [Compilation Policies](../reference/compilation-policies.md) reference
specifies the policies and their overrides. For `Release` and `ReleaseDbgSym`
outputs, the LLVM backend lowers each procedure into a separate LLVM module,
applies the ThinLTO pre-link pipeline at `O3`, links the modules, and applies
the full LTO pipeline at `O3`.

The test bundle builds outputs with the `Development`, `Debug`, `Quick`, and
`Release` build types. The remaining build types are accepted by the
configuration loader and are not built by the test bundle.

### Restrictions of the LLVM backend

| Area | Restriction | Reference |
| --- | --- | --- |
| Floating-point formats | The backend represents `F16E5`, `F16E8`, `F32E8`, `F64E11`, and `F128E15`. Other formats exist only in constant evaluation. | [Primitive Types and Literals](../reference/primitive-types-and-literals.md) |
| Atomic storage width | Atomic operations require the storage width to fit the largest atomic width that the target CPU supports. Wider atomic storage is not implemented. | [Atomics](../reference/atomics.md) |
| Hardware square root | The backend substitutes a hardware instruction for binary32 with x86 SSE, binary64 with x86 SSE2, and binary32 or binary64 on ARM64. Other configurations call the runtime-module implementation. | [Runtime Module Contracts](../reference/runtime-module-contracts.md) |
| Calling conventions of external procedures | The backend accepts `DEFAULT`, `CCALL`, and `STDCALL`. | [External Procedures](../reference/external-procedures.md) |
| Foreign exceptions | Propagation covers frames of the generated executable. Translation of C++ or SEH exceptions and unwinding through external shared libraries are not implemented. | [Exception Handling](../reference/exceptions.md) |
| Inline assembly functions | `ASM_INLINE_FUNCTION` is parsed and rejected (`ASM_INLINE_FUNCTION backend support is not implemented`). | [Assembly Procedures](../reference/assembly-procedures.md) |

## Cortado backend

The Cortado backend translates the intermediate representation into JVM
bytecode and writes one JAR file with a `Main-Class` manifest entry. The
resulting file runs with `java -jar <file>`.

The backend is Experimental:

- The configuration value is `experimental_cortado`. The loader rejects the
  value `cortado`.
- The test bundle configures no JVM target. The source modules `jvm_interop`
  and `jvm_failure` remain in the bundle, and no target maps them.
- The C++ test suite of the compiler exercises the backend in two respects
  only: rejection of contradictory JVM and native configuration, and
  diagnosis of layout operations on a layoutless target.
- 320 test declarations in the test bundle carry a `KNOWN_BROKEN_IF`
  condition that is `TRUE` on a Cortado target.
  [Known Issues](known-issues.md#tests-annotated-for-the-cortado-backend)
  lists them. Because no JVM target is configured, these annotations are not
  revalidated when the test bundle changes.
- The JVM target was removed from the bundle configuration on 2026-10-26.
  The coroutine tests were added after that date and carry no Cortado
  annotation. Coroutine behavior on the Cortado backend is unverified.

### Options

`backend_cortado_options` accepts one key, `mode`, with the values `standard`
(the default) and `address_sanitizer`. The backend source contains no
optimization pass and does not consult the build type.

### Performance

The home page and the `qxcbuild.yml` reference state the following
characteristics. The repository contains no benchmark that measures them.

- The translation from the intermediate representation to JVM bytecode
  applies no optimization and no escape analysis.
- Generated JVM code is approximately 100 times slower than native code.
- The principal cause is the emulation of totally ordered pointers and
  pointer arithmetic, which the JVM does not provide.

For comparison, the README states that `Release` native code is competitive
with GCC and Clang at `-O2`.

## Layoutless targets

A layoutless target has no source-visible byte layout for types whose
representation the managed runtime controls. The JVM is the only layoutless
CPU: `ARCH_IS_LAYOUTLESS` is `TRUE` exactly when `ARCH_IS_JVM` is `TRUE`.
The [Backends and Layout](../reference/backends-and-layout.md) reference
specifies the layout queries.

## Feature availability by backend

The table lists every language feature for which the Reference, the compiler
source, or a C++ compiler test identifies a difference between the backends.
A feature that the table omits has no documented difference; the absence of an
entry does not establish that the feature works on the Cortado backend.

| Feature | LLVM | Cortado | Reference |
| --- | --- | --- | --- |
| Inheritance: base construction, hierarchy conversions, virtual dispatch, RTTI casts, polymorphic destruction | Implemented | Not implemented; rejected during lowering | [Inheritance](../reference/inheritance.md) |
| `DYNAMIC_TYPE_OF` | Implemented | Not implemented | [Type Queries and Deduction](../reference/type-queries-and-deduction.md#dynamic-type-identity) |
| Polymorphic allocation, virtual destruction, and allocation-info recovery in `NEW` and `DELETE` | Implemented | Not implemented | [`NEW` and `DELETE`](../reference/new-and-delete.md) |
| Runtime-sized array allocation and generator construction | Implemented | Not listed as supported; the Reference names only the LLVM backend and constant evaluation | [`NEW` and `DELETE`](../reference/new-and-delete.md#runtime-sized-arrays) |
| Exception handling (`THROW`, `TRY`, `CATCH`, cleanup during unwinding) | Implemented | Not implemented | [Exception Handling](../reference/exceptions.md#constant-evaluation-and-native-support) |
| Checked arithmetic operators (`+?`, `-?`, `*?`, `/?` and mutating forms) | Implemented | Incomplete | [Arithmetic Operators](../reference/arithmetic-operators.md) |
| Atomic read-modify-write members | Implemented within the native atomic width | Not implemented | [Atomics](../reference/atomics.md#integer-read-modify-write-operations) |
| `PER_THREAD` variables with destructors | Implemented | Not implemented; destructor registration is rejected | [Thread-Local Variables](../reference/thread-local-variables.md) |
| `ASM_PROCEDURE` | Implemented | Lowering error when a program calls the procedure | [Assembly Procedures](../reference/assembly-procedures.md#execution-and-target-support) |
| `IBC_GETADDR`, `IBC_PUN`, `IBC_LOAD`, `IBC_WRITE` | Implemented | Unavailable | [External Memory](../reference/external-memory.md#runtime-address-operations) |
| `SIZEOF` | Every type with a known size | `I8`, `U8`, `I16`, `U16`, `I32`, `U32`, `I64`, and `U64` only | [Backends and Layout](../reference/backends-and-layout.md) |
| `ALIGNOF`, `CELLSIZEOF` | Implemented | Unavailable | [Type Queries and Deduction](../reference/type-queries-and-deduction.md#layout-and-integer-properties) |
| `ALIGNED_STORAGE` | Implemented | Unavailable when a reached declaration uses the type | [Backends and Layout](../reference/backends-and-layout.md) |
| GC pointers (`~>T`) and `EXTERN_TYPE` | Not used by any native declaration in the test bundle | Implemented for pointers to `EXTERN_TYPE` declarations | [External Types](../reference/external-types.md), [Pointers](../reference/pointers.md) |
| `EXTERN_PROCEDURE` calling conventions | `CCALL`, `STDCALL` | `JVM_STATIC`, `JVM_VIRTUAL`, `JVM_INTERFACE`, `JVM_CONSTRUCTOR`, `JVM_GETFIELD`, `JVM_PUTFIELD`, `JVM_GETSTATIC`, `JVM_PUTSTATIC` | [External Procedures](../reference/external-procedures.md#jvm-call-conventions) |
| CPU steppings and `HAVE_*` capability queries | Implemented | Steppings are rejected in the target configuration | [CPU Capabilities and Steppings](../reference/cpu-capabilities-and-steppings.md) |
| `binary` and `environment` target keys | Accepted | Rejected in the target configuration | [`qxcbuild.yml`](../reference/qxcbuild-file.md#target-identity) |
| Optimization by build type | Implemented | Not implemented | [`qxcbuild.yml`](../reference/qxcbuild-file.md#backend-modes) |
| Integer, enum, and flagset widths | Arbitrary widths accepted by the language | Widths beyond the index limit of `java.math.BigInteger` are rejected | [Primitive Types and Literals](../reference/primitive-types-and-literals.md) |

Constant evaluation does not use a backend. The Reference states that
inheritance and exception handling are available during constant evaluation.
A `KNOWN_BROKEN_IF(ARCH_IS_JVM)` annotation on a `DUAL_TEST` nevertheless
disables both the static and the runtime execution of that test on a JVM
target.

## Standard-module differences on the Cortado backend

The standard modules contain JVM-specific declarations for command-line
arguments, standard output, files, and strings.
[Standard Modules](standard-modules.md) lists the facilities that have a
JVM-specific implementation and the facilities that are unavailable on a JVM
target.
