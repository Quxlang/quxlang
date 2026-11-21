# Getting Started

This page describes the tools that a Quxlang build requires, the procedure that builds the `qxc`
compiler from source, and the sequence of pages that introduces the language.

!!! note "Development status"
    Quxlang is under active development and has no stable language release. Syntax and library
    interfaces can change between revisions. [Implementation Status](../status/index.md) records
    the supported targets, backends, and features.

## Compilation model

Quxlang compiles a complete source bundle rather than one translation unit at a time. A source
bundle contains source modules, a `qxcbuild.yml` file that defines targets and outputs, and
nothing that the compiler locates outside the bundle. The compiler accepts no optimization or
target flags on the command line, and a compiler installation contains no implicit standard
library. The same bundle and the same compiler revision therefore produce the same artifacts on
every host.

## Compiler build

The compiler is built from the [source repository](https://github.com/quxlang/quxlang).

### Requirements

| Requirement | Notes |
| --- | --- |
| C++ compiler | Clang with libc++ is the development configuration. GCC and MSVC builds are maintained on a best-effort basis. |
| CMake and Ninja | CMake is required. Ninja is the generator that the build workspace uses. |
| Git | The dependency setup downloads sources with Git. |
| Go | The build wrappers `cbuild` and `csetup` are installed with `go install`. |
| Memory and storage | 32 GiB of memory and 100 GB of free storage are recommended, because the build compiles LLVM. |

Builds on Ubuntu are currently unreliable. On Linux, a container with an Arch Linux based image
is the recommended build environment.

### Build procedure

The following commands install the build wrappers.

```sh
go install gitlab.com/rpnx/cbuild-go/cmd/cbuild@preview
go install gitlab.com/rpnx/cbuild-go/cmd/csetup@preview
```

The following commands, run from the root of the repository, download the dependencies and
build the Release configuration. The first build compiles LLVM and takes considerably longer
than later builds.

```sh
cd misc/build
csetup download
cbuild build -c Release
```

The resulting compiler is `misc/build/buildspaces/<toolchain>/quxlang/Release/qxc`, where
`<toolchain>` names the host toolchain, for example `system-clang`. The command
`cbuild test -c Release` builds the same configuration and runs the compiler test suite.

The repository also provides a wrapper that performs the setup and runs the tests in one step.

```sh
./configure && make test
```

## Compiler commands

| Command | Effect |
| --- | --- |
| `qxc build <bundle> <output-directory> [targets]` | Compiles the configured targets and writes each artifact to `<output-directory>/output/<output-key>`. |
| `qxc check <bundle> [targets]` | Runs every frontend stage and the enabled static tests, and writes no files. |

The optional `targets` argument is a comma-separated list of target names. The
[`qxc` command-line reference](../reference/compiler-output.md) specifies both commands.

## Introductory sequence

1. [First Program](first-program.md) builds and runs a minimal executable.
2. [Source Bundles and Targets](source-bundles.md) describes the compilation unit and the
   target model.
3. [The `qxcbuild.yml` File](qxcbuild-file.md) describes bundle configuration.
4. [Syntax at a Glance](syntax-examples.md) summarizes the principal syntactic forms.
5. [Functions](functions-and-parameters.md) and [Call Arguments](call-arguments.md) introduce
   named and positional argument groups.
6. The [Overview index](index.md) lists the remaining topics by subject.

[Example Programs](../examples/index.md) contains complete programs with their output.
