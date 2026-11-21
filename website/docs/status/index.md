# Implementation Status

The Implementation Status section records which parts of Quxlang exist in the
current compiler, standard modules, and test bundle, and how thoroughly each
part is exercised. The section covers compilation targets, backends, language
features, standard-module facilities, and known defects.

Quxlang has no stable release. The language, the `qxcbuild.yml` schema, the
standard modules, and the compiler command line can change between
revisions without a compatibility period. The status recorded in this section
describes the repository state of 2026-11-19.

The [Reference](../reference/index.md) specifies the behavior of each
implemented feature. The Implementation Status section specifies only whether
a feature, target, or facility is available, and under which restrictions.

## Pages in this section

| Page | Contents |
| --- | --- |
| [Targets](targets.md) | Platform, CPU, binary-format, and environment combinations; the level at which the test bundle exercises each combination; planned and reserved targets and output kinds |
| [Backends](backends.md) | The LLVM backend and the experimental Cortado backend; language features that are unavailable or behave differently on the Cortado backend and on layoutless targets; build types |
| [Language Features](language-features.md) | One status entry for each language reference page; individual forms that are reserved, parsed without an implementation, or restricted to particular targets |
| [Standard Modules](standard-modules.md) | Platform availability of the facilities in the `std`, `io`, and `net` modules and in the supporting `posix`, `syscall`, and `RUNTIME` modules |
| [Known Issues](known-issues.md) | Tests annotated `KNOWN_BROKEN` or `KNOWN_FAILING`, and other verified defects and limitations |

## Status vocabulary

| Status | Definition |
| --- | --- |
| Implemented | The parser, semantic analysis, and LLVM code generation implement the feature, and the test bundle contains tests of the feature that are active on at least one configured target. |
| Partially implemented | The principal behavior is implemented. At least one form that the Reference or the compiler source identifies is rejected, or is unavailable on a configured target. The entry lists each such form. |
| Target-restricted | The feature is implemented only for specific platforms, CPUs, environments, or backends. The entry names the supported set. |
| Experimental | An implementation exists in the compiler source. The test bundle configuration does not build the implementation, and the configuration key carries an `experimental` marker. |
| Reserved syntax | The parser or the configuration loader accepts the spelling. No semantic rule or generation path gives the spelling an effect. |
| Not implemented | The compiler rejects the form, or no code path for the form exists. |
| Planned | Project documentation states the intention to add the item. The repository contains no implementation. |

The following terms describe how a target is exercised.

| Term | Definition |
| --- | --- |
| Checked | CTest runs `qxc check` for the target. The command evaluates every frontend stage and the enabled static tests, and performs no code generation or linking. |
| Built | The test bundle configures at least one output for the target, so `qxc build` generates machine code and links the output. |
| Executed | A generated executable for the target has run on hardware or under an emulator, and the repository records the result. |

## Determination of status

Every entry in this section derives from repository content. The following
sources are used, in descending order of authority.

1. **Compiler source.** The configuration loader
   (`quxlang/sources/loaders/source_loader.cpp`) defines the accepted target
   and output values. Diagnostics in `quxlang/sources` and `quxlang/include`
   identify forms that the compiler rejects.
2. **Test bundle.** The bundle at `quxlang/tests/testdata/testbundle` contains
   the standard modules and the language tests. Its `qxcbuild.yml` file defines
   the targets and outputs that the project builds. The project rules require
   the complete bundle to compile for every configured target.
3. **Test annotations.** `KNOWN_BROKEN`, `KNOWN_BROKEN_IF`, `KNOWN_FAILING`, and
   `KNOWN_FAILING_IF` mark tests that do not pass. `INCLUDE_IF` conditions
   remove tests and declarations from targets that do not support them.
   [Known Issues](known-issues.md) defines the annotations and lists the
   annotated tests.
4. **Standard-module source.** `STATIC_IF` selections, `INCLUDE_IF`
   conditions, `COMPILATION_ERROR` statements, and `UNIMPLEMENTED` statements
   in the module sources determine platform availability of each facility.
5. **Documentation.** Statements in the Reference and on the home page are
   used where the compiler source and the test bundle neither confirm nor
   contradict them. Each such entry names the page that makes the statement.

An entry marked Implemented asserts that tests exist and are active. An
Implemented entry does not assert the absence of defects.

## Test declarations and execution

The test bundle uses three kinds of test declaration. A `STATIC_TEST` runs in
the constant evaluator during compilation. A `UNIT_TEST` runs in a generated
`unit_test_suite` executable. A `DUAL_TEST` runs in both modes. The
[Tests](../reference/tests.md) reference specifies the declarations.

Static tests run for every target that `qxc check` or `qxc build` processes,
independent of the host machine, because the constant evaluator models the
configured target. Unit tests produce evidence only when the generated
executable runs. [Targets](targets.md) records, for each target, whether the
repository contains evidence of execution.
