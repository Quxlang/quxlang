# `std::args` and `std::env`

Module `std` · source file `std.qxs`

The namespaces `std::args` and `std::env` provide read-only access to the
command-line arguments and to the environment entries that the operating
system or the launcher supplied to the process. The functions read values that
the runtime module recorded at process start; the functions never modify the
arguments or the environment. Argument index zero identifies the launcher, and
the arguments that the user supplied begin at index one on every supported
target. Both namespaces are restricted to specific targets; the restrictions
are listed under [Target restrictions](#target-restrictions).

## Declaration

```quxlang
::args NAMESPACE
{
  ::count FUNCTION(): SZ
  ::address_at FUNCTION(@index SZ): ADDRESS
  ::at FUNCTION(@index SZ): string
}

::env NAMESPACE
{
  ::count FUNCTION(): SZ
  ::at FUNCTION(@index SZ): ADDRESS
}
```

## Function summary

| Function | Description |
| --- | --- |
| [`args::count`](#argscount) | Returns the number of arguments, including the launcher. |
| [`args::address_at`](#argsaddress_at) | Returns the address of the null-terminated bytes of one argument. |
| [`args::at`](#argsat) | Returns one argument as an owned string. |
| [`env::count`](#envcount) | Returns the number of environment entries. |
| [`env::at`](#envat) | Returns the address of one environment entry. |

## Target restrictions

Each function selects its implementation with `STATIC_IF` on the target
predicates. A function that has no implementation for the active target
contains `COMPILATION_ERROR ON_LOWER`: the compiler reports the error when a
call of the function remains reachable in the code that is lowered for that
target. A program that does not call the function compiles for that target.

| Function | Linux (`OS_LINUX`) | macOS (`OS_MACOS`) | JVM (`ARCH_IS_JVM`) | Other targets, including Windows |
| --- | --- | --- | --- | --- |
| `args::count` | Supported | Supported | Supported | Compilation error |
| `args::address_at` | Supported | Supported | Compilation error | Compilation error |
| `args::at` | Supported | Supported | Supported | Compilation error |
| `env::count` | Supported | Compilation error | Compilation error | Compilation error |
| `env::at` | Supported | Compilation error | Compilation error | Compilation error |

The error message is `std::args is not supported on this platform.` for the
functions of `std::args`, and `std::env is not supported on this platform.`
for the functions of `std::env`.

The only test of `args::count` and `args::at` in the test bundle targets the
JVM, expects one argument with the value `quxlang`, and is marked as known to
be broken (`KNOWN_BROKEN_IF(ARCH_IS_JVM)`).

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not
evaluated results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults.

## Functions of `std::args`

### `args::count` { #argscount }

```quxlang
::count FUNCTION(): SZ
```

**Return value**: The number of process arguments. The count includes the
launcher at index zero, so a process that received no user-supplied argument
reports a count of 1. On Linux and macOS targets, the value is the argument
count that the operating system passed to the program entry point. On JVM
targets, the value is supplied by the generated Java entry class.

**Complexity**: Constant.

### `args::address_at` { #argsaddress_at }

```quxlang
::address_at FUNCTION(@index SZ): ADDRESS
```

**Parameters**: `index` is the zero-based argument index.

**Return value**: The address of the first byte of the argument. The bytes
are terminated by a zero byte and belong to the process image; the function
copies nothing. The address is interpreted with an `IBC` pointer, as in
`IBC_PUN#(IBC CONST=>>BYTE)(@ADDR address)`.

**Checked failures**: The function asserts that `index` is less than the
argument count.

**Complexity**: Constant.

**Target restrictions**: Linux and macOS targets only. JVM targets provide no
argument addresses.

### `args::at` { #argsat }

```quxlang
::at FUNCTION(@index SZ): string
```

**Parameters**: `index` is the zero-based argument index.

**Return value**: A [`std::string`](string.md) that contains a copy of the
argument. On Linux and macOS targets, the string contains the bytes of the
argument up to, and not including, the terminating zero byte. On JVM targets,
the string contains the UTF-8 encoding of the Java string that holds the
argument.

**Checked failures**: The function asserts that `index` is less than
`count()`.

**Complexity**: Linear in the length of the argument. The function allocates
the storage of the returned string.

## Functions of `std::env`

### `env::count` { #envcount }

```quxlang
::count FUNCTION(): SZ
```

**Return value**: The number of environment entries. The runtime module counts
the entries of the null-terminated environment vector on the initial process
stack during startup.

**Complexity**: Constant.

**Target restrictions**: Linux targets only.

### `env::at` { #envat }

```quxlang
::at FUNCTION(@index SZ): ADDRESS
```

**Parameters**: `index` is the zero-based entry index.

**Return value**: The address of the environment entry, as stored in the
environment vector of the initial process stack. The function copies nothing
and performs no parsing; `std::env` provides no lookup by variable name and no
conversion to [`std::string`](string.md). By the Linux process-start
convention, each entry is a null-terminated byte string of the form
`NAME=value`.

**Checked failures**: The function asserts that `index` is less than the
environment entry count.

**Complexity**: Constant.

**Target restrictions**: Linux targets only.

## Example

The program reports the number of user-supplied arguments and prints each
argument. The output shown is produced by a run without arguments.

```quxlang
--8<-- "example-programs/reference-programs/process_arguments_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/process_arguments_example/expected-output.txt"
```

## See also

- [`std::string`](string.md)
- [`std::fmt`, `std::printf`, and `std::print`](format.md)
- [Target Availability](../../reference/availability-and-targets.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
- [Program Startup and Runtime Hooks](../../reference/program-startup-and-runtime-hooks.md)
- [External Memory](../../reference/external-memory.md)
