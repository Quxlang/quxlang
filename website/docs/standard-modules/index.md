# Standard Modules Reference

The Standard Modules Reference documents the public declarations of the modules that accompany
the compiler: `std`, `io`, and `net`. Each page specifies one type or one family of functions with
its declarations, preconditions, failure conditions, complexity, and target restrictions.

!!! warning "No compatibility guarantee"
    The standard-module design is not final. Declarations can change or disappear between
    revisions. The module sources currently reside in the compiler test bundle at
    `quxlang/tests/testdata/testbundle/modules`.

## Modules

| Module | Contents |
| --- | --- |
| [`std`](std/index.md) | Containers, strings, ownership types, formatting, numerics, algorithms, threads, synchronization, futures, and coroutine support |
| [`io`](io/index.md) | Native files, buffers, the asynchronous completion queue, and the asynchronous runtime |
| [`net`](net/index.md) | IP addresses, endpoints, and TCP and UDP sockets with synchronous, future, and coroutine operations |

The modules `RUNTIME`, `syscall`, and `posix` supply the runtime contracts and operating-system
bindings on which the three documented modules depend. Their declarations are implementation
interfaces and have no reference pages. [Runtime Module
Contracts](../reference/runtime-module-contracts.md) specifies the declarations that the compiler
requires from `RUNTIME`.

## Use of a standard module

A compiler installation contains no implicit library. A source bundle includes the module sources
under its `modules/` directory, and each target maps the logical module names in `qxcbuild.yml`.
A source file then imports the module by its logical name.

```yaml
modules:
  RUNTIME: {source: runtime}
  syscall: {source: syscall}
  posix: {source: posix}
  std: {source: std}
  io: {source: io}
  net: {source: net}
```

```quxlang
IMPORT std;
IMPORT io;
```

[Example Programs](../examples/index.md#example-bundle) gives the complete procedure for a new
bundle.

## Page conventions

Each page states the source file of its declarations and presents the following sections where
they apply.

| Section | Contents |
| --- | --- |
| Declaration | The declaration as written in the module source, with template parameters and type modifiers |
| Member summary | A table of every public member |
| Member functions | Each overload set with its exact signatures and numbered descriptions |
| Nonmember functions | Factory functions, operators, and other related declarations |
| Example | One complete program with its output. The page includes the program from `website/example-programs/reference-programs`, and the [verification script](../examples/index.md#verification-of-all-examples) builds and runs it. |
| See also | Related standard-module, language reference, and example pages |

Within a function description, labelled paragraphs state the contract.

| Label | Meaning |
| --- | --- |
| **Preconditions** | Requirements that the function does not check. A violation is undefined behavior. |
| **Checked failures** | Requirements enforced by `ASSERT`, `INVARIANT`, or a bounds check. The active [compilation policies](../reference/compilation-policies.md) determine whether the check executes. |
| **Exceptions** | The types and values that the function throws. |
| **Complexity** | The asymptotic cost, derived from the implementation. |
| **Iterator and reference validity** | The iterators, pointers, and references that the function invalidates. |
| **Target restrictions** | Platforms, CPUs, environments, or backends on which the declaration is unavailable or behaves differently. |

[Implementation Status: Standard Modules](../status/standard-modules.md) summarizes platform
availability across all modules.
