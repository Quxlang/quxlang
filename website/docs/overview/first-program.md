# First Program

This page builds and runs one complete program. The procedure requires a `qxc` compiler and a
checkout of the Quxlang repository, as [Getting Started](getting-started.md) describes.

## Source file

A Quxlang source file begins with a language declaration. Import declarations follow, and each
module-level declaration begins with `::name`.

```quxlang
--8<-- "example-programs/programs/hello_world/main.qxs"
```

The program writes one line of text to standard output. The example establishes several rules
that apply throughout the language:

- Keywords are uppercase, and user identifiers are lowercase.
- `LANGUAGE QUXLANG EN 0.0;` is the language declaration.
- `IMPORT std;` makes the standard module available under the name `std`.
- `::main` is a declaration in the current module. The function `main` of the application module
  is the entry function of the executable.
- `FUNCTION(): I32` declares a function that has no parameters and returns an `I32` value.
- `std::print` writes a string to standard output and appends no newline, so the string literal
  ends with the escape sequence `\n`.
- `RETURN 0;` ends the function. The returned value becomes the exit status of the process.

[`std::printf`](../standard-modules/std/format.md#printf) writes formatted text.
[Syntax at a Glance](syntax-examples.md) presents a longer program that declares parameters, local
variables, and conditional statements.

## Bundle layout

The compiler operates on a source bundle. The bundle for this program has the following layout.

```text
first-bundle/
├── qxcbuild.yml
└── modules/
    ├── app/
    │   └── sources/
    │       └── main.qxs
    ├── runtime/
    ├── syscall/
    ├── posix/
    ├── std/
    ├── io/
    └── net/
```

The source file above is stored as `modules/app/sources/main.qxs`. The remaining six directories
are the standard modules, which a bundle contains as source. The following commands, run from
the root of the repository checkout, create the bundle.

```sh
mkdir -p first-bundle/modules/app/sources
for module in runtime syscall posix std io net; do
  cp -R "quxlang/tests/testdata/testbundle/modules/$module" first-bundle/modules/
done
```

## Bundle configuration

The file `first-bundle/qxcbuild.yml` defines one target and one output. The following
configuration targets Linux on x86-64. For macOS on Apple silicon, the `platform` value is
`macos` and the `cpu` value is `ARM64`.

```yaml
targets:
  native:
    platform: linux
    cpu: x64
    backend: llvm
    modules:
      RUNTIME: {source: runtime}
      syscall: {source: syscall}
      posix: {source: posix}
      std: {source: std}
      io: {source: io}
      net: {source: net}
      app: {source: app}

outputs:
  app:
    target: native
    type: executable
    main_module: app
    backend_llvm_options:
      build_type: Release
```

The `targets` section defines compilation settings and maps each logical module name to a source
directory. The `outputs` section selects a target and an output type for each artifact path. A
target without outputs produces no artifact and only runs its enabled static tests.

## Build and execution

```console
$ qxc build ./first-bundle ./out
$ ./out/output/app
Hello, world!
```

The output key `app` is the exact artifact path below `./out/output`. The compiler adds no
filename extension.

## Further reading

[Source Bundles and Targets](source-bundles.md) explains the bundle model.
[The `qxcbuild.yml` File](qxcbuild-file.md) introduces targets, module mappings, and outputs.
[Source Files and Imports](source-files-and-imports.md) describes the file preamble and the
`IMPORT` declaration.
