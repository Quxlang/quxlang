# Example Programs

Each page in this section presents one complete program, its output, and an explanation of the
constructs that the program uses. The source file and the recorded output of every example are
stored in the repository under `website/example-programs/programs`, and the pages include those
files directly. The script `website/example-programs/verify.py` builds every program with `qxc`,
runs it, and compares its standard output with the recorded output.

## Language features

| Example | Features |
| --- | --- |
| [Prime sieve with clause-based loops](prime-sieve.md) | `LOOP` sequences, filters, labels, `BREAK`, `CONTINUE` |
| [Order state machine with enumerations](order-state-machine.md) | Enumerations, defaults, associated functions, ordering |
| [Bank account transfers and object lifetime](bank-account-lifetime.md) | Structures, constructors, destructors, receiver qualifiers |
| [Reading calibration with lambdas and procedure pointers](reading-calibration-callbacks.md) | Lambda captures, procedure pointers, operator precedence |
| [Fraction arithmetic with user-defined operators](fraction-arithmetic.md) | Operator members, `RHS` operators, `OPERATOR<=>` |
| [Shape areas with variants](shape-area-variant.md) | Variants, `ISA`, `UNWRAP`, `MATCH`, `VISIT` |
| [Payroll computation through virtual dispatch](employee-payroll-dispatch.md) | Inheritance, virtual functions, `NEW`, `DELETE` |
| [Sensor polling through generics and interfaces](sensor-polling-generic.md) | `GENERIC` type erasure, `INTERFACE`, `IMPLEMENTATION` |
| [Generic value utilities with deduced types](generic-value-utilities.md) | `AUTO(name)` deduction, templates, swap |
| [Port number validation with exceptions](validated-port-parser.md) | `THROW`, typed `CATCH`, propagation, cleanup |
| [Compile-time checksum lookup table](checksum-lookup-table.md) | `STATIC`, `STATIC_IF`, `STATIC_TEST`, bitwise operators |
| [Square generator with a C++-style coroutine](square-generator-coroutine.md) | `COROUTINE`, `YIELD`, promise types, coroutine handles |
| [Recursive sum on a thread-pool coroutine runtime](thread-pool-coroutine-sum.md) | `ON ... RUN`, `GO`, `AWAIT`, futures |

## Standard modules

| Example | Facilities |
| --- | --- |
| [Formatted inventory report](formatted-inventory-report.md) | `std::printf`, `std::fmt`, format styles |
| [Label reversal and vowel count](label-reversal-vowel-count.md) | `std::string` concatenation, indexing, comparison |
| [Number redaction in a log line](log-number-redaction.md) | `std::byte_regex` counting and replacement |
| [First matching record as an optional value](first-matching-record.md) | `std::dynarr`, `std::optional` |
| [Station code deduplication](deduplicate-stations.md) | `std::set` insertion results |
| [Status code frequency table](status-code-frequency.md) | `std::map` indexing, ordered iteration, `at` |
| [Priority job dispatch](dispatch-jobs.md) | `std::list` end operations |
| [Stable element references during telemetry growth](stable-telemetry.md) | `std::geometric_array` address stability |
| [Reading filtering and transformation with range algorithms](reading-filter-transform.md) | Range algorithms, `back_inserter`, expression lambdas |
| [Search in sorted readings](sorted-reading-search.md) | `sort`, `binary_search`, `lower_bound` |
| [Sensor ownership transfer with unique pointers](sensor-ownership-transfer.md) | `std::unique_ptr`, `MOVE`, swap |
| [Shared configuration lifetime](shared-configuration-lifetime.md) | `std::shared_ptr` reference counting |
| [Large byte-rate quotient](average-byte-rate.md) | `std::big_integer` division, typed exceptions |
| [Parallel event total under a mutex](parallel-event-total.md) | `std::thread`, `std::thin_mutex`, capture lists |
| [Buffered measurement report on standard output](buffered-measurement-report.md) | `std::buffered_writer`, `io::file_ref`, `io::buffer_ref` |

## Example bundle

The examples require `qxc` and the standard module sources from the same checkout.
[Getting Started](../overview/getting-started.md) describes how to build `qxc`. The following
commands, run from the root of the checkout, create a bundle that contains the standard modules
and an empty application module.

```sh
mkdir -p example-bundle/modules/app/sources
for module in runtime syscall posix std io net; do
  cp -R "quxlang/tests/testdata/testbundle/modules/$module" example-bundle/modules/
done
```

The program source is stored as `example-bundle/modules/app/sources/main.qxs`. Every example
defines the entry point `::main`, so the application module contains one example source file at a
time.

The following `example-bundle/qxcbuild.yml` file defines a Linux x86-64 target. For macOS on
Apple silicon, the `platform` value is `macos` and the `cpu` value is `ARM64`.

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

The following commands build and run the program. The output key `app` determines the artifact
path `out/output/app`.

```sh
qxc build ./example-bundle ./out
./out/output/app
```

[Source Bundles and Targets](../overview/source-bundles.md) explains how a target maps logical
module names to source directories.

## Verification of all examples

The verification script assembles the same kind of bundle for the host platform, with one module
and one executable output for each example program.

```sh
python3 website/example-programs/verify.py misc/build/buildspaces/system-clang/quxlang/Release/qxc
```

The script supports macOS on ARM64 and Linux on x86-64 and ARM64. Its exit status is zero when
every program builds, exits with status zero, and prints its recorded output.

The examples on the [Standard Modules Reference](../standard-modules/index.md) pages are stored
under `website/example-programs/reference-programs` and are verified by the same script.

```sh
python3 website/example-programs/verify.py misc/build/buildspaces/system-clang/quxlang/Release/qxc \
  --programs-directory=website/example-programs/reference-programs
```
