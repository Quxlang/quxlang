# Synchronization primitives

Module `std` · source file `synchronization.qxs`

The source file `synchronization.qxs` declares the blocking synchronization
primitives of module `std`: three mutex types, a condition variable, and a
counting semaphore. Every primitive is process-local and blocks the calling
native thread. The primitives coordinate threads created by
[`std::thread`](thread.md) and are the basis of
[`std::synchronized_pointer_queue`](synchronized-pointer-queue.md),
[`std::future`](future.md), and
[`std::thread_pool_cort_runtime`](thread-pool-coroutine.md). Lock-free access to
single objects is provided by the language-level
[atomic types](../../reference/atomics.md).

## Entity summary

| Entity | Kind | Description |
| --- | --- | --- |
| [`std::thin_mutex`](mutexes.md#stdthin_mutex) | `STRUCT ROOTED` | A non-recursive mutex whose only state is one `ATOMIC#I32`. |
| [`std::ordering_mutex`](mutexes.md#stdordering_mutex) | `STRUCT ROOTED` | A non-recursive mutex backed by the native mutex of the target environment. |
| [`std::unfair_mutex`](mutexes.md#stdunfair_mutex) | `ALIAS` | The process-local mutex of the runtime module, shared with the allocator. |
| [`std::condition_variable`](condition-variable.md) | `STRUCT ROOTED` | A condition variable that operates with any type that supplies `lock` and `unlock`. |
| [`std::semaphore`](semaphore.md) | `STRUCT ROOTED` | A counting semaphore composed of an `ordering_mutex` and a `condition_variable`. |

## Common properties

Every structure in this file is declared `ROOTED`. A `ROOTED` structure has
neither a copy constructor nor a move constructor, so an object of one of these
types remains at the address at which it was constructed. A declaration that
copies or moves one of these objects is rejected at compile time. The blocking
operations wait on the address of state stored inside the object, and other
threads refer to the object while they wait. An object is destroyed only after
every thread has finished using it.

Blocking and wake-up use one address-wait mechanism for each operating system:

| Condition | Wait | Wake |
| --- | --- | --- |
| `OS_LINUX` | `syscall::linux::futex_wait` | `syscall::linux::futex_wake` |
| `OS_WINDOWS` | `WaitOnAddress` with an infinite timeout | `WakeByAddressSingle` and `WakeByAddressAll` |
| `OS_MACOS` | `RUNTIME_MODULE::wait_at_u32_address` | `RUNTIME_MODULE::notify_one_at_address` and `RUNTIME_MODULE::notify_all_at_address` |

A wait can return although no notification occurred. Each mutex rechecks its
state after a wait returns, and
[`condition_variable::wait`](condition-variable.md#wait) exposes spurious
returns to its caller.

The operations are native runtime operations and are not available during
constant evaluation.

## Target restrictions

The following table lists the implementation that each entity selects at
compile time. For each entity the conditions are evaluated in the listed
order. `UNIMPLEMENTED` denotes the
[`UNIMPLEMENTED` statement](../../reference/diagnostics-and-failure.md#unimplemented),
which is a compilation error or a panic according to the compilation policy of
the output.

| Entity | Condition | Implementation |
| --- | --- | --- |
| `thin_mutex` | `OS_LINUX` or `OS_WINDOWS` | One `ATOMIC#I32` with address waits |
| | `OS_MACOS` | `os_unfair_lock` operations on one `ATOMIC#I32` |
| | Any other target | Every member function is `UNIMPLEMENTED`. |
| `ordering_mutex` | `OS_LINUX && ENVIRONMENT_IS_STATIC` | A contained `thin_mutex` |
| | `ENVIRONMENT_IS_GLIBC` | `pthread_mutex_t`. Module `posix` declares the required bindings only when `OS_LINUX && ARCH_IS_X64 && ENVIRONMENT_IS_GLIBC` holds. |
| | `OS_WINDOWS` | `CRITICAL_SECTION`. Module `syscall` declares the required bindings only when `ARCH_IS_X64` holds. |
| | `OS_MACOS` | `pthread_mutex_t`. Module `posix` declares the required bindings only when `ARCH_IS_X64` or `ARCH_IS_ARM64` holds. |
| | Any other target | Every member function is `UNIMPLEMENTED`. |
| `unfair_mutex` | `OS_LINUX`, `OS_WINDOWS`, or `OS_MACOS` | One `ATOMIC#I32` with address waits |
| | Any other target | The uncontended paths operate on the atomic state. The contended paths of `lock` and `unlock` are `UNIMPLEMENTED`. |
| `condition_variable` | `OS_LINUX` or `OS_WINDOWS` or `OS_MACOS` | An atomic notification sequence with address waits |
| | Any other target | The constructor is `UNIMPLEMENTED`, and the module-private wait and wake functions used by `wait`, `notify_one`, and `notify_all` are not declared. |
| `semaphore` | Every target | An `ordering_mutex` and a `condition_variable`. The type is implemented on the targets on which both members are implemented. |

JVM targets fall under "Any other target": every operating-system predicate is
`FALSE` for a JVM target. A Linux target whose environment is neither `static`
nor `glibc` supports `thin_mutex`, `unfair_mutex`, and `condition_variable`,
and does not support `ordering_mutex` or `semaphore`. The target predicates are
listed in [Target Availability](../../reference/availability-and-targets.md).

## See also

- [`std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`](mutexes.md)
- [`std::condition_variable`](condition-variable.md)
- [`std::semaphore`](semaphore.md)
- [`std::thread`](thread.md)
- [Atomics](../../reference/atomics.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
- [Compilation Policies](../../reference/compilation-policies.md)
