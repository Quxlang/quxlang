# `std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`

Module `std` · source file `synchronization.qxs`

The three mutex types provide mutual exclusion between native threads through
the same three operations: `lock`, `try_lock`, and `unlock`. Every type is
non-recursive, process-local, and fixed at its construction address.
`thin_mutex` minimizes object size, `ordering_mutex` delegates to the native
mutex of the target environment, and `unfair_mutex` names the mutex that the
runtime module also uses for the allocator. Each type satisfies the mutex
requirement of [`condition_variable::wait`](condition-variable.md#wait). The
properties shared with the other primitives of the source file are described
in [Synchronization primitives](synchronization.md).

| Entity | Description |
| --- | --- |
| [`std::thin_mutex`](#stdthin_mutex) | A mutex whose only state is one `ATOMIC#I32`. |
| [`std::ordering_mutex`](#stdordering_mutex) | A mutex backed by the native mutex of the target environment. |
| [`std::unfair_mutex`](#stdunfair_mutex) | An alias of the runtime module's mutex. |

## `std::thin_mutex`

```quxlang
::thin_mutex STRUCT ROOTED
```

`thin_mutex` is a compact non-recursive mutex optimized to minimize object
size. Its only field is one `ATOMIC#I32`, which is present when
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds. `ROOTED` removes copy and move
construction.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#thin_mutexconstructor) | Creates an unlocked mutex. |
| [`DESTRUCTOR`](#thin_mutexdestructor) | Destroys the mutex. |
| [`lock`](#thin_mutexlock) | Blocks until the calling thread owns the mutex. |
| [`try_lock`](#thin_mutextry_lock) | Attempts to acquire the mutex without blocking. |
| [`unlock`](#thin_mutexunlock) | Releases the mutex. |

### `thin_mutex::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()
```

Creates an unlocked mutex.

**Target restrictions**: The constructor is `UNIMPLEMENTED` unless
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

### `thin_mutex::DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Destroys the mutex. The destructor performs no operation on the supported
targets.

**Preconditions**: No thread owns the mutex, waits for the mutex, or uses the
mutex after destruction begins.

**Target restrictions**: The destructor is `UNIMPLEMENTED` unless
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

### `thin_mutex::lock`

```quxlang
.lock FUNCTION()
```

Blocks until the calling thread acquires the mutex.

When `OS_LINUX || OS_WINDOWS` holds, the state value `0` denotes an unlocked
mutex, `1` denotes a locked mutex without recorded contention, and `2` denotes
a locked mutex with recorded contention. An uncontended acquisition is one
atomic compare-exchange from `0` to `1`. A contended acquisition records
contention, blocks in an address wait while the state remains `2`, and
acquires the mutex in state `2`, so that the subsequent `unlock` wakes another
waiting thread. When `OS_MACOS` holds, the function calls
`os_unfair_lock_lock` on the state.

**Preconditions**: The calling thread does not own the mutex. When
`OS_LINUX || OS_WINDOWS` holds, a call by the owning thread never returns.

**Target restrictions**: The function is `UNIMPLEMENTED` unless
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

### `thin_mutex::try_lock`

```quxlang
.try_lock FUNCTION(): BOOL
```

Attempts to acquire the mutex without blocking.

**Return value**: `TRUE` when the calling thread acquired the mutex, and
`FALSE` otherwise. When `OS_LINUX || OS_WINDOWS` holds, the function performs
one atomic compare-exchange from `0` to `1` and returns `FALSE` whenever the
mutex is locked, including when the calling thread owns it. When `OS_MACOS`
holds, the function returns the result of `os_unfair_lock_trylock`.

**Target restrictions**: The function is `UNIMPLEMENTED` unless
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

### `thin_mutex::unlock`

```quxlang
.unlock FUNCTION()
```

Releases the mutex. When `OS_LINUX || OS_WINDOWS` holds and contention was
recorded, the function stores the unlocked state and wakes at most one waiting
thread. When `OS_MACOS` holds, the function calls `os_unfair_lock_unlock`.

**Preconditions**: The calling thread owns the mutex. The Linux and Windows
implementations do not record the owning thread.

**Checked failures**: When `OS_LINUX || OS_WINDOWS` holds, the function
contains `ASSERT(expected_uncontended == 2, "Cannot unlock an unlocked mutex.")`,
which fails when the mutex is unlocked. The assertion is evaluated only when
the `ASSERT_ENABLED` policy is enabled (see
[Failure Statements](../../reference/diagnostics-and-failure.md)).

**Target restrictions**: The function is `UNIMPLEMENTED` unless
`OS_LINUX || OS_WINDOWS || OS_MACOS` holds.

## `std::ordering_mutex`

```quxlang
::ordering_mutex STRUCT ROOTED
```

`ordering_mutex` is a non-recursive mutex optimized for traditional
throughput-oriented locking. The type stores the native mutex object of the
target environment and forwards each operation to the native implementation.
`ROOTED` removes copy and move construction.

Each member function selects its implementation with the following
conditions, which are evaluated in the listed order:

| Condition | Storage | Native operations |
| --- | --- | --- |
| `OS_LINUX && ENVIRONMENT_IS_STATIC` | A `thin_mutex` | The corresponding `thin_mutex` member function |
| `ENVIRONMENT_IS_GLIBC` | `posix::native_mutex_storage` | `pthread_mutex_init`, `pthread_mutex_destroy`, `pthread_mutex_lock`, `pthread_mutex_trylock`, `pthread_mutex_unlock` with symbol version `GLIBC_2.2.5` |
| `OS_WINDOWS` | `syscall::windows::native_mutex_storage` | `InitializeCriticalSection`, `DeleteCriticalSection`, `EnterCriticalSection`, `TryEnterCriticalSection`, `LeaveCriticalSection` from `kernel32` |
| `OS_MACOS` | `posix::native_mutex_storage` | The `pthread_mutex_*` functions from `/usr/lib/libSystem.B.dylib` |
| Any other target | None | Every member function is `UNIMPLEMENTED`. |

Module `posix` declares the glibc bindings only when
`OS_LINUX && ARCH_IS_X64 && ENVIRONMENT_IS_GLIBC` holds and the macOS bindings
only when `OS_MACOS && (ARCH_IS_X64 || ARCH_IS_ARM64)` holds. Module `syscall`
declares the Windows critical-section bindings only when `ARCH_IS_X64` holds.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#ordering_mutexconstructor) | Creates an unlocked mutex. |
| [`DESTRUCTOR`](#ordering_mutexdestructor) | Releases the native mutex. |
| [`lock`](#ordering_mutexlock) | Blocks until the calling thread owns the mutex. |
| [`try_lock`](#ordering_mutextry_lock) | Attempts to acquire the mutex without blocking. |
| [`unlock`](#ordering_mutexunlock) | Releases the mutex. |

### `ordering_mutex::CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()
```

Creates an unlocked mutex. The glibc and macOS implementations call
`pthread_mutex_init` with a null attribute pointer, which selects the default
mutex attributes. The Windows implementation calls
`InitializeCriticalSection`.

**Checked failures**: The glibc and macOS implementations panic with
`"pthread_mutex_init failed."` when `pthread_mutex_init` returns a nonzero
value.

### `ordering_mutex::DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Releases the native mutex.

**Preconditions**: No thread owns the mutex, waits for the mutex, or uses the
mutex after destruction begins.

**Checked failures**: The glibc and macOS implementations panic with
`"pthread_mutex_destroy failed."` when `pthread_mutex_destroy` returns a
nonzero value.

### `ordering_mutex::lock`

```quxlang
.lock FUNCTION()
```

Blocks until the calling thread acquires the mutex.

**Preconditions**: The calling thread does not own the mutex.

**Checked failures**: The glibc and macOS implementations panic with
`"pthread_mutex_lock failed."` when `pthread_mutex_lock` returns a nonzero
value.

### `ordering_mutex::try_lock`

```quxlang
.try_lock FUNCTION(): BOOL
```

Attempts to acquire the mutex without blocking.

**Return value**: `TRUE` when the calling thread acquired the mutex, and
`FALSE` otherwise. The glibc and macOS implementations return `TRUE` when
`pthread_mutex_trylock` returns `0` and `FALSE` when it returns `16`. The
Windows implementation returns the result of `TryEnterCriticalSection`. The
static Linux implementation returns the result of
[`thin_mutex::try_lock`](#thin_mutextry_lock).

**Checked failures**: The glibc and macOS implementations panic with
`"pthread_mutex_trylock failed."` when `pthread_mutex_trylock` returns a value
other than `0` or `16`.

### `ordering_mutex::unlock`

```quxlang
.unlock FUNCTION()
```

Releases the mutex.

**Preconditions**: The calling thread owns the mutex.

**Checked failures**: The glibc and macOS implementations panic with
`"pthread_mutex_unlock failed."` when `pthread_mutex_unlock` returns a nonzero
value. The static Linux implementation has the checked failure of
[`thin_mutex::unlock`](#thin_mutexunlock).

## `std::unfair_mutex`

```quxlang
::unfair_mutex ALIAS RUNTIME_MODULE::mutex;
```

`unfair_mutex` is a process-local mutex shared with the allocator. A contending
thread can acquire ownership before a thread that blocked earlier. The alias
names the `mutex` structure of the module that the target binds as the runtime
module (see [Runtime Module Contracts](../../reference/runtime-module-contracts.md)).
The following description applies to the runtime module that accompanies the
standard modules, which declares the structure as follows:

```quxlang
::mutex STRUCT ROOTED
```

The structure holds one private `ATOMIC#I32` and declares no constructor or
destructor. A default-initialized object is unlocked. `ROOTED` removes copy and
move construction, and the mutex remains at a stable address until every user
has left. `std::synchronized_pointer_queue`, `std::future`, and
`std::thread_pool_cort_runtime` use this mutex internally.

| Member | Description |
| --- | --- |
| [`try_lock`](#unfair_mutextry_lock) | Attempts to acquire the mutex without waiting. |
| [`lock`](#unfair_mutexlock) | Blocks until the calling thread owns the mutex. |
| [`unlock`](#unfair_mutexunlock) | Releases the mutex and wakes one sleeping contender. |

### `unfair_mutex::try_lock`

```quxlang
.try_lock FUNCTION(): BOOL
```

Attempts acquisition without waiting through one atomic compare-exchange from
the unlocked state.

**Return value**: `TRUE` when the calling thread acquired the mutex. `FALSE`
whenever the mutex is locked, including when the calling thread owns it.

### `unfair_mutex::lock`

```quxlang
.lock FUNCTION()
```

Acquires the mutex and blocks while another thread owns it. The state encoding
and the acquisition algorithm are those described for
[`thin_mutex::lock`](#thin_mutexlock) under `OS_LINUX || OS_WINDOWS`. The
contended path waits with `futex_wait` when `OS_LINUX` holds, with
`WaitOnAddress` when `OS_WINDOWS` holds, and with
`RUNTIME_MODULE::wait_at_u32_address` when `OS_MACOS` holds.

**Preconditions**: The calling thread does not own the mutex. A call by the
owning thread never returns.

**Target restrictions**: The contended path is `UNIMPLEMENTED` unless
`OS_LINUX`, `OS_WINDOWS`, or `OS_MACOS` holds.

### `unfair_mutex::unlock`

```quxlang
.unlock FUNCTION()
```

Releases the mutex. When contention was recorded, the function stores the
unlocked state and notifies one sleeping contender.

**Preconditions**: The calling thread owns the mutex. The implementation does
not record the owning thread.

**Checked failures**: The function contains
`ASSERT(expected_uncontended == 2, "Cannot unlock an unlocked allocator mutex.")`,
which fails when the mutex is unlocked. The assertion is evaluated only when
the `ASSERT_ENABLED` policy is enabled.

**Target restrictions**: The contended path is `UNIMPLEMENTED` unless
`OS_LINUX`, `OS_WINDOWS`, or `OS_MACOS` holds.

## Example

Four threads increment one counter under an `ordering_mutex`. The program
joins every thread before it reads the counter. The program then shows the
result of `try_lock` on an unlocked `thin_mutex` and on an `unfair_mutex` that
the calling thread already owns.

```quxlang
--8<-- "example-programs/reference-programs/mutex_counter/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/mutex_counter/expected-output.txt"
```

## See also

- [Synchronization primitives](synchronization.md)
- [`std::condition_variable`](condition-variable.md)
- [`std::semaphore`](semaphore.md)
- [`std::thread`](thread.md)
- [`std::dynarr`](dynarr.md)
- [Atomics](../../reference/atomics.md)
- [Target Availability](../../reference/availability-and-targets.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
