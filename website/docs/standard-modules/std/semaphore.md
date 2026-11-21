# `std::semaphore`

Module `std` · source file `synchronization.qxs`

`std::semaphore` is a process-local counting semaphore. The object holds a
count of permits. `acquire` consumes one permit and blocks while no permit is
available, `try_acquire` consumes a permit only when one is available, and
`release` adds one or more permits. The type is composed of a
[`std::ordering_mutex`](mutexes.md#stdordering_mutex), a
[`std::condition_variable`](condition-variable.md), and a permit count of type
`SZ`. The properties shared with the other primitives of the source file are
described in [Synchronization primitives](synchronization.md).

## Declaration

```quxlang
::semaphore STRUCT ROOTED
```

`ROOTED` removes copy and move construction, so a semaphore remains at its
construction address. The permit count has no upper bound other than the range
of `SZ`.

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates a semaphore with an initial number of permits. |
| [`acquire`](#acquire) | Blocks until one permit is available and consumes it. |
| [`try_acquire`](#try_acquire) | Consumes one available permit without waiting for a permit. |
| [`release`](#release) | Adds permits and wakes blocked acquirers. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@initial_count SZ DEFAULT(0))
```

Creates a semaphore that contains `initial_count` permits.

**Parameters**

- `initial_count` — the initial number of permits. The default is zero.

**Checked failures**: The constructors of the `ordering_mutex` and
`condition_variable` members run first and have the failures specified for
those types.

### `acquire`

```quxlang
.acquire FUNCTION()
```

Locks the internal mutex, waits on the internal condition variable while the
permit count is zero, decrements the count, and unlocks the mutex. The function
rechecks the count after every return of the wait, so a spurious wake-up does
not consume a permit.

### `try_acquire`

```quxlang
.try_acquire FUNCTION(): BOOL
```

Locks the internal mutex, decrements the permit count when it is nonzero, and
unlocks the mutex. The function does not wait for a permit. The function
blocks only while another thread holds the internal mutex.

**Return value**: `TRUE` when the function consumed one permit, and `FALSE`
when the permit count was zero.

### `release`

```quxlang
.release FUNCTION(@count SZ DEFAULT(1))
```

Adds `count` permits while the internal mutex is locked. After the mutex is
unlocked, the function calls `notify_one` on the internal condition variable
when `count` is `1` and `notify_all` otherwise.

**Parameters**

- `count` — the number of permits to add. The default is one.

**Checked failures**

- `ASSERT(count != 0, "Semaphore release count must be nonzero.")`
- `ASSERT(.count <= ((0 AS SZ) #!!) - count, "Semaphore count overflow.")`,
  which fails when the sum of the current permit count and `count` exceeds the
  maximum value of `SZ`.

The assertions are evaluated only when the `ASSERT_ENABLED` policy is enabled
(see [Failure Statements](../../reference/diagnostics-and-failure.md)). When the
policy is disabled, both conditions are preconditions.

## Target restrictions

The type declares no target-conditional code of its own. Its member functions
are implemented on the targets on which both `std::ordering_mutex` and
`std::condition_variable` are implemented: `OS_LINUX && ENVIRONMENT_IS_STATIC`,
`ENVIRONMENT_IS_GLIBC`, `OS_WINDOWS`, and `OS_MACOS`, subject to the
architecture restrictions of the native `ordering_mutex` bindings. On every
other target, including JVM targets, construction reaches
[`UNIMPLEMENTED`](../../reference/diagnostics-and-failure.md#unimplemented)
in the constructors of the members. The complete table is in
[Synchronization primitives](synchronization.md#target-restrictions).

## Example

A worker thread blocks in `acquire` until the main thread releases a permit.
The main thread writes the published value before the release and joins the
worker before it prints the result. The program then releases two permits and
consumes them with `try_acquire`.

```quxlang
--8<-- "example-programs/reference-programs/semaphore_permits/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/semaphore_permits/expected-output.txt"
```

## See also

- [Synchronization primitives](synchronization.md)
- [`std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`](mutexes.md)
- [`std::condition_variable`](condition-variable.md)
- [`std::thread`](thread.md)
