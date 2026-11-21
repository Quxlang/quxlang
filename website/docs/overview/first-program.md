# First Program

This page builds and runs one complete program. The procedure requires a `qxc` compiler and a
checkout of the Quxlang repository, as [Getting Started](getting-started.md) describes.

## Source file

A Quxlang source file begins with a language declaration. Import declarations follow, and each
module-level declaration begins with `::name`.

```quxlang
--8<-- "example-programs/programs/volume_clamp/main.qxs"
```

The example establishes several rules that apply throughout the language:

- Keywords are uppercase, and user identifiers are lowercase.
- `::clamp` and `::main` are declarations in the current module.
- `@value`, `@minimum`, and `@maximum` are named parameters. The names state the role of each of
  the three same-typed arguments at the call site.
- `: I32` declares the return type.
- `VAR volume I32 := ...;` declares and initializes a local variable.
- `:=` initializes or assigns, and `==` compares.
- `IMPORT std;` makes the standard module available. `std::printf` validates its format string
  during compilation and receives each value as a named argument.

Quxlang also provides positional argument groups for interfaces in which position carries the
meaning. [Call Arguments](call-arguments.md) introduces both forms.

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
volume 100
```

The output key `app` is the exact artifact path below `./out/output`. The compiler adds no
filename extension.

## Further reading

[Source Bundles and Targets](source-bundles.md) explains the bundle model.
[The `qxcbuild.yml` File](qxcbuild-file.md) introduces targets, module mappings, and outputs.
[Source Files and Imports](source-files-and-imports.md) describes the file preamble and the
`IMPORT` declaration.
