# Targets

A Quxlang target combines a platform, a CPU, a binary format, an environment,
and a backend. The [`qxcbuild.yml` reference](../reference/qxcbuild-file.md)
specifies the configuration keys. This page records which combinations the
compiler accepts, which combinations the test bundle builds, and which
combinations have recorded execution results.

The terms Checked, Built, and Executed are defined in the
[status vocabulary](index.md#status-vocabulary).

## Target matrix

The test bundle (`quxlang/tests/testdata/testbundle/qxcbuild.yml`) configures
ten targets. Every configured target uses the LLVM backend. Each target is
Built, and each target configures at least one `unit_test_suite` output.

| Bundle target | Platform | CPU | Binary format | Environment | Checked by CTest | Unit-test suite | Recorded execution |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `linux-x64` | `linux` | `x64` | ELF | `static` (default) | Yes | Yes | Exception-handling validation stated by the Reference |
| `linux-x64-glibc` | `linux` | `x64` | ELF | `glibc` | Yes | Yes | Exception-handling validation stated by the Reference |
| `linux-x86` | `linux` | `x86` | ELF | `static` (default) | Yes | Yes | Exception-handling validation stated by the Reference |
| `linux-arm64` | `linux` | `ARM64` | ELF | `static` (default) | Yes | Yes | Exception-handling validation stated by the Reference |
| `linux-arm32` | `linux` | `ARM32` | ELF | `static` (default) | No | Yes | None recorded |
| `linux-riscv32` | `linux` | `RISCV32` | ELF | `static` (default) | No | Yes | None recorded; no hardware testing |
| `linux-riscv64` | `linux` | `RISCV64` | ELF | `static` (default) | No | Yes | None recorded; no hardware testing |
| `linux-z-arch` | `linux` | `z_arch` | ELF | `static` (default) | Yes | Yes | Exception-handling validation stated by the Reference |
| `windows-x64` | `windows` | `x64` | PE | `msvc` (default) | Yes | Yes | None; execution is unverified |
| `macos-arm64` | `macos` | `ARM64` | Mach-O | `libsystem` (default) | Yes | Yes | Exception-handling validation stated by the Reference; the [documentation example programs](../examples/index.md) are built and run on this target |

The columns have the following meaning.

- **Checked by CTest.** The compiler build registers one CTest case named
  `qxc_check_test.<target>` for each of seven targets. Each case runs
  `qxc check` on the test bundle, which evaluates every frontend stage and the
  enabled static tests of the target. The cases perform no code generation.
  `linux-arm32`, `linux-riscv32`, and `linux-riscv64` have no such case.
- **Unit-test suite.** The target has an output of type `unit_test_suite`. The
  suite is an executable that runs the `UNIT_TEST` and `DUAL_TEST`
  declarations of the selected modules.
- **Recorded execution.** The column states the evidence in the repository
  that a generated executable for the target has run.

No automated test in the repository executes a generated executable. The
project rules require a contributor to run the unit-test suite of the host
platform and to confirm that the complete bundle compiles for every target
before a change is complete. The
[Exception Handling reference](../reference/exceptions.md#constant-evaluation-and-native-support)
states that exception-handling execution has been validated on macOS ARM64
and on Linux x86, x86-64 with the `static` and `glibc` environments, ARM64,
and s390x. The repository does not record the host machine or the emulator
that the Linux validation used.

## Configured outputs

| Bundle target | Executables | Unit-test suites | Benchmark outputs |
| --- | --- | --- | --- |
| `linux-x64` | `helloworld` (`Release`) | `tests` (`quick`) | `benchmarks` suite, `benchmark-copy` executable (`Release`) |
| `linux-x64-glibc` | None | `tests` (`quick`) | None |
| `linux-x86` | `thread-smoke` | `tests` (`quick`) | None |
| `linux-arm64` | `helloworld` (`quick`), `helloworld.dbg` (`Debug`) | `tests` (`quick`) | None |
| `linux-arm32` | `helloworld` (`quick`) | `tests` (`quick`) | None |
| `linux-riscv32` | `helloworld` (`Quick`), `helloworld.dbg` (`Debug`), `helloworld.release` (`Release`), `stepping-demo` (`Release`) | `tests` (`Quick`), `tests.release` (`Release`) | None |
| `linux-riscv64` | `helloworld` (`Quick`), `helloworld.dbg` (`Debug`), `helloworld.release` (`Release`), `stepping-demo` (`Release`) | `tests` (`Quick`), `tests.release` (`Release`) | None |
| `linux-z-arch` | None | `tests` (`Development`, the default build type) | None |
| `windows-x64` | `stepping-demo.exe` (`Release`) | `tests.exe` (`Development`) | None |
| `macos-arm64` | `stepping-demo`, `integer-blur` (`Release`) | `tests` (`Development`) | `benchmarks` suite, `benchmark-numeric` and `benchmark-copy` executables (`Release`) |

The RISC-V unit-test suites select the modules `RUNTIME`, `std`, `syscall`,
`posix`, `io`, `net`, and `tests`. The unit-test suites of the other targets
additionally select the fixture modules `foolib` and `barlib`.

## Accepted configuration values

The configuration loader accepts more values than the test bundle exercises.
Acceptance by the loader does not establish that a combination generates a
working executable.

| Key | Accepted values | Values exercised by the test bundle | Status of the remaining values |
| --- | --- | --- | --- |
| `platform` | `linux`, `windows`, `macos`, `jvm` | `linux`, `windows`, `macos` | `jvm` selects the [experimental Cortado backend](backends.md#cortado-backend). |
| `cpu` | `x64`, `x86`, `ARM32`, `ARM64`, `RISCV32`, `RISCV64`, `z_arch`, `jvm` | All native values on `linux`; `x64` on `windows`; `ARM64` on `macos` | Other platform and CPU pairs are not built by the test bundle. |
| `binary` | `elf`, `macho`, `pe`, `wasm` | `elf`, `macho`, `pe`, each as the platform default | `wasm` is reserved: the LLVM backend has no target triple or unwind format for the value. |
| `environment` | `glibc`, `musl`, `bionic`, `msvc`, `ucrt`, `cygwin`, `static`, `libsystem`, `freestanding` | `static` and `glibc` on `linux`; `msvc` on `windows`; `libsystem` on `macos` | `musl`, `bionic`, `ucrt`, `cygwin`, and `freestanding` are accepted and set the corresponding `ENVIRONMENT_IS_*` predicate. The test bundle and the standard modules contain no support for them. |
| `backend` | `llvm`, `experimental_cortado` | `llvm` | `experimental_cortado` is [Experimental](backends.md#cortado-backend). |

The output stage of the LLVM backend links executables only for the pairs
Linux with ELF, macOS with Mach-O, and Windows with PE. Every other
platform and binary-format pair is rejected with the diagnostic
`LLVM output kind is not supported`.

## Notes for each target family

### Linux x64 and x86

The `static` environment produces statically linked executables that issue
system calls directly. The `glibc` environment produces dynamically linked
executables. The ELF linker contains a dynamic-loader path for x64, x86, and
ARM64, and resolves only the library name `glibc`. The test bundle builds the
`glibc` environment for x64 only, and the `posix` module declares its glibc
mutex interface for x64 only.

Two tests are annotated as known broken on `linux-x86`. See
[Known Issues](known-issues.md#tests-broken-on-x86).

### Linux ARM64

The `linux-arm64` target is Built with the `quick` and `Debug` build types.
The optional `quxlang_static_gtests` CTest executable, which registers each
static test of the `tests` module as an individual test case, evaluates the
tests for the `linux-arm64` target. The executable is registered only when
the CMake option `QUXLANG_DISCOVER_STATIC_TESTS` is enabled, and the option is
disabled by default.

### Linux ARM32

The compiler uses the LLVM triple `armv7-unknown-linux-gnueabi-elf` and
reads the thread pointer from the `TPIDRURO` register. The
[`qxcbuild.yml` reference](../reference/qxcbuild-file.md#target-identity)
specifies the instruction set as ARMv7-A with NEON and VFP, little-endian
ELF32, and the GNU EABI core-register convention for floating-point
arguments. Assembly procedures for the `ARM32` tag execute in ARM state.

The target was added on 2026-11-14. CTest does not check the target, and the
repository records no execution result.

### Linux RISC-V

The `RISCV32` and `RISCV64` CPUs use the RV32GC and RV64GC instruction sets.
The compiler fixes the extensions `I`, `M`, `A`, `F`, `D`, `C`, `ZICSR`, and
`ZIFENCEI` to enabled in every stepping, selects the ILP32D and LP64D calling
conventions, and disables linker relaxation. A stepping that disables a
required extension is rejected.

The targets were added on 2026-11-16. The commit that added them states that
the targets are not tested with real hardware. CTest does not check the
targets, and the repository records no execution result.

### Linux z/Architecture

The `z_arch` CPU uses the LLVM architecture name `s390x`. The test bundle builds only a unit-test suite for the target, with the
`Development` build type and every configured module.

### Windows x64

The Windows target uses the PE binary format and the `msvc` environment. The
PE linker accepts x86, x64, and ARM64 machine types. The runtime module in the
test bundle supplies the `PROGRAM_START` and `CHECK_STACK` assembly procedures
for x64 only, so x64 is the only Windows CPU with a complete startup path.

The `windows-x64` target is the only bundle target that configures explicit
CPU steppings: x86-64 feature levels 1 through 4 and one AMD-specific
stepping tuned for Zen 1.

Windows executables are produced by cross-compilation. The Reference states
that Windows execution remains unverified. The PE linker rejects optional
imports (`Optional Windows imports are not supported by static PE import
tables`).

### macOS ARM64

The macOS target uses the Mach-O binary format and the `libsystem`
environment. The Mach-O linker accepts 64-bit targets only and contains
relocation support for ARM64 and x64. The runtime module supplies
`PROGRAM_START` for ARM64 only, so ARM64 is the only macOS CPU with a complete
startup path.

The default minimum operating-system version in the generated
`LC_BUILD_VERSION` load command is 11.0.0. The test bundle sets
`macho_config.minimum_os_version` to `14.4`. The runtime module documents that
address-based waiting with 8-byte values requires macOS 14.4 or later.

## JVM targets

A target with `platform: jvm` or `cpu: jvm` selects the Cortado backend, which
generates an executable JAR file. The test bundle configures no JVM target.
[Backends](backends.md#cortado-backend) describes the status of the backend.

## Planned platforms and CPUs

The home page states the following intentions. The repository contains no
implementation of any item in this table.

| Item | Kind | Status |
| --- | --- | --- |
| BSD operating systems | Platform | Planned. The `OS_BSD` predicate is a keyword without an implementation, and the configuration loader rejects BSD platform names. |
| GNU Hurd and other open-source operating systems | Platform | Planned |
| POWER | CPU | Planned |
| SPARC | CPU | Planned |
| LoongArch64 | CPU | Planned |
| WebAssembly | Binary format | Reserved syntax. The `binary: wasm` value and the `BINARY_WASM` predicate exist; the LLVM backend rejects the format. |

## Output kinds

| Output `type` | Status | Notes |
| --- | --- | --- |
| `executable` | Implemented | Linked for Linux ELF, macOS Mach-O, and Windows PE. |
| `unit_test_suite` | Implemented | Built for every target in the test bundle. |
| `benchmark_suite` | Implemented | Built by the test bundle for `linux-x64` and `macos-arm64`. |
| `benchmark_executable` | Implemented | Built by the test bundle for `linux-x64` and `macos-arm64`. |
| `shared_library` | Reserved syntax | The configuration loader accepts the value. The LLVM backend rejects the output with `LLVM output kind is not supported`. |
| `static_library` | Reserved syntax | Same behavior as `shared_library`. |
| `image` | Reserved syntax | Same behavior as `shared_library`. |

The `qxcbuild.yml` reference specifies the `executable` and `unit_test_suite`
kinds. The benchmark kinds are accepted by the loader and used by the test
bundle. The Reference does not specify them.

## Unwind formats

The compiler selects the DWARF `.eh_frame` unwind format for every ELF,
Mach-O, and PE target. `UNWIND_FORMAT_IS_DWARF_EH_FRAME` is therefore `TRUE` on
every such target, and the predicates `UNWIND_FORMAT_IS_NONE`,
`UNWIND_FORMAT_IS_ARM_EHABI`, `UNWIND_FORMAT_IS_WINDOWS_SEH`,
`UNWIND_FORMAT_IS_SJLJ`, and `UNWIND_FORMAT_IS_WASM` are `FALSE`. No unwind
format is defined for the `wasm` binary format or for a JVM target; evaluation
of an unwind-format predicate on such a target is a compiler error.

## Compiler host platforms

The compiler is developed with Clang and libc++. The project README describes
GCC, libstdc++, and MSVC as toolchains that work intermittently. The project
rules state that building the compiler on Ubuntu is unreliable and recommend
an Arch-based container image. The README lists LLVM 22 as a dependency.

The host platform does not affect generated output. A `qxc` executable on any
host cross-compiles every configured target.
