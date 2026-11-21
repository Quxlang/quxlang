# Quxlang

Quxlang is a systems programming language focused on providing fast
cross-platform deterministic and reproducible builds.

This website documents the current compiler surface with feature-focused pages
and source examples. Quxlang remains under active development and has not yet
made a stable language release, so the current reference is descriptive rather
than a long-term compatibility promise.

!!! warning "NO WARRANTY"
    These docs are provided without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in the software.

## Documentation sections

| Section | Contents |
| --- | --- |
| [Overview](overview/index.md) | An example-oriented introduction to each language feature, beginning with [Getting Started](overview/getting-started.md) and a [first program](overview/first-program.md) |
| [Language Reference](reference/index.md) | The complete rules for syntax, semantics, failure conditions, and target restrictions |
| [Standard Modules Reference](standard-modules/index.md) | The public declarations of the `std`, `io`, and `net` modules |
| [Example Programs](examples/index.md) | Complete programs with their output, each built and run by the documentation verification script |
| [Implementation Status](status/index.md) | Supported targets and backends, feature status, and known issues |
| [Philosophy](philosophy/index.md) | The design goals behind determinism, value semantics, safety boundaries, and control flow |
| [Evolution from C++](evolution/index.md) | The principal differences between Quxlang and C++ |

## Development Status

Quxlang is in early development stages, and `qxc` can target Windows, MacOS and Linux.
Future support for various BSD, Hurd, and other open source operating systems is planned for
future work.

The LLVM backend supports x86, x64, ARM32, ARM64, and z/Architecture (s390).
Linux also supports RISC-V RV32GC and RV64GC targets. Support for POWER, SPARC,
and LoongArch64 is planned.

For non-native targets, `qxc` currently supports the Cortado backend for generating
executable jar files which can be run using `java -jar <jarfile>`. Note that the
current VMIR to Cortado translation algorithm is extremely inefficient and has no 
optimization passes or escape analysis (and no, the Java JIT compiler will not 
help much here). The inefficiency is mainly due a large number of bad workarounds
to get totally ordered pointers and pointer arithmetic working in a "correct" 
manner in a virtual machine that doesn't support totally ordered pointers.

CLI/CIR support is planned for the future, but there is no current code for this. It 
is expected to perform much better than JVM due to CIR having pointers in the intermediate
representation, which makes translation relatively trivial.

[Implementation Status](status/index.md) contains the complete target, backend,
and feature tables.

## Other Links

- [Code Repository (GitHub)](https://github.com/quxlang/quxlang)
- [Quxlang Blog](https://quxlang.blog/)
- [Discord](https://discord.gg/Z9qRXXxRtY)

## Website Content

!!! warning "AI Usage"
    This documentation contains a mixture of human written and AI generated content and may contain errors. Please report any issues on [GitHub](https://github.com/Quxlang/quxlang/issues/new).

## Pronunciation & Notes

Quxlang is pronounced like "k-whuh-ks-lang" /ˈkwʌks.læŋɡ/.  Quxlang is a proper noun, similar to Erlang, the language is never called "Qux".

## Current documentation boundary

The Language Reference covers source forms that the current parser accepts and that
the compiler implements. [Implementation Status](status/index.md) lists
target-specific gaps, including the features that are unavailable on the JVM
backend. Forward-looking work is not documented as a current feature.
Low-level VMIR instruction specifications are maintained in repository
engineering documents and are outside the source-language reference.
