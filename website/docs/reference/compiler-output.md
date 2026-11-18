# Compiler Output and Diagnostics

The compiler provides two commands. The `build` command takes an input bundle
and an output directory:

```console
qxc build ./bundle ./out
```

The compiler reads `./bundle/qxcbuild.yml`, loads its configured targets, and
writes each output to `./out/output/<output-key>`. The output mapping key is
its exact relative path; the compiler adds no filename extension.

To compile selected targets, supply their names separated by commas:

```console
qxc build ./bundle ./out linux-x64,macos-arm64
```

Without a target list, all configured targets are compiled. Unknown target
names are errors. Configuration validation covers the entire manifest before
selection. Targets without outputs still run their enabled static tests.

## Checking without building

```console
qxc check ./bundle
qxc check ./bundle linux-x64,macos-arm64
```

The `check` command takes an input bundle and no output directory. The command
evaluates every frontend stage of each configured output: parsing, semantic
analysis, template instantiation, type layout, lowering to the compiler's
intermediate representation, and enabled static tests. The command performs no
optimization, machine code generation, or linking, and writes no files.

A bundle that passes `check` is accepted by every frontend stage of `build`.
Failures that originate in machine code generation, assembly encoding of
`ASM_PROCEDURE` bodies, or linking are reported only by `build`.

The exit status is zero when every selected target passes and nonzero
otherwise. Target selection follows the same rules as `build`.

## Debug compiler artifacts

```console
qxc build ./bundle ./out --debug-compile-output
```

`--debug-compile-output` writes compiler-stage artifacts below
`./out/build/<target>` for diagnosis and can be combined with target selection.
The option is accepted only by `build`.
Output-specific intermediates retain the output key's directory structure
beneath that target's build directory. Shared routine artifacts remain grouped
by target.

## Reproducibility boundary

Artifact-affecting choices belong to `qxcbuild.yml`: module versions, target
architecture, hosted environment, CPU steppings, module options, outputs, and
backend mode. The compiler does not search for installed source libraries or
accept ambient optimization flags that silently redefine the configured
artifact.

Source-level failures use the declarations and statements documented under
[Tests](tests.md) and
[Diagnostics and explicit failure](diagnostics-and-failure.md).
