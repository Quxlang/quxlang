# Examples

| Example | Features |
| --- | --- |
| [Deduplicate station codes](deduplicate-stations.md) | Ordered sets and insertion results |
| [Dispatch jobs in priority order](dispatch-jobs.md) | Doubly linked lists and end operations |
| [Keep a reference while telemetry grows](stable-telemetry.md) | Segmented arrays and element lifetime |
| [Compute a large byte-rate quotient](average-byte-rate.md) | Arbitrary-precision arithmetic and typed exceptions |

## Example bundle

The examples require `qxc` and the standard module sources from the same
checkout. The [compiler setup guide](../overview/getting-started.md) describes
how to build `qxc`. The following commands copy the standard modules into a
new bundle from the root of the checkout.

```sh
mkdir -p example-bundle/modules/app/sources
for module in runtime syscall posix std io net; do
  cp -R "quxlang/tests/testdata/testbundle/modules/$module" example-bundle/modules/
done
```

The program goes in `example-bundle/modules/app/sources/main.qxs`. Each example
defines the same entry point, so the bundle contains one example source file at
a time.

The following `example-bundle/qxcbuild.yml` defines a Linux x86-64 target.
For Apple silicon macOS, the target's `platform` is `macos` and its `cpu` is
`ARM64`.

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

With `qxc` on the executable search path, the following command builds the
program. The output key `app` determines the artifact path `out/output/app`.

```sh
qxc build ./example-bundle ./out
```

The [Source Bundles and Targets overview](../overview/source-bundles.md)
explains how a target maps logical module names to source directories.
