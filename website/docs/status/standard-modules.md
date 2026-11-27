# Standard Modules

The standard modules are ordinary Quxlang source modules. The compiler
contains no built-in library; each source bundle supplies its own copy of the
modules and maps them in `qxcbuild.yml`. The authoritative copy resides in the
test bundle at `quxlang/tests/testdata/testbundle/modules`.

!!! warning "No compatibility guarantee"
    The design of the standard modules is not final. Names, signatures,
    module boundaries, and platform coverage can change in any revision, and
    no compatibility guarantee applies to any declaration in these modules.

The [Standard Modules](../standard-modules/index.md) section documents the
interfaces. This page records only the platform availability of each
facility. Availability is derived from the `INCLUDE_IF` conditions,
`STATIC_IF` selections, `COMPILATION_ERROR` statements, and `UNIMPLEMENTED`
statements in the module sources.

## Modules

| Logical module | Source directory | Role | Status |
| --- | --- | --- | --- |
| `std` | `std` | Containers, strings, algorithms, formatting, smart pointers, threads, synchronization, coroutine support | Implemented; platform-dependent facilities are listed below |
| `io` | `io` | Files, descriptors, asynchronous I/O | Target-restricted |
| `net` | `net` | IP addresses, TCP and UDP sockets | Target-restricted to Linux, macOS, and Windows |
| `posix` | `posix` | Declarations of POSIX threading procedures | Target-restricted to macOS and to Linux x64 with the `glibc` environment |
| `syscall` | `syscall` | Operating-system call interfaces | Target-restricted; one namespace for each operating system |
| `RUNTIME` | `runtime` | Program startup, allocation, exception unwinding, thread runtime, CPU detection, unit-test and benchmark drivers | Implemented for every target in the test bundle |

Every target of the test bundle maps all six modules, and every unit-test
suite of the bundle includes the tests of all six modules.

The column headings in the tables below use the following abbreviations for
target groups: **Linux** denotes every Linux target; **Linux static** and
**Linux glibc** denote the Linux environments `static` and `glibc`;
**JVM** denotes the experimental Cortado backend, which the test bundle does
not build.

## Module `std`

### Facilities with platform-dependent availability

| Facility | Linux | macOS | Windows | JVM | Behavior on other targets |
| --- | --- | --- | --- | --- | --- |
| `std::args::count` | Yes | Yes | No | Yes | Compilation error: `std::args is not supported on this platform.` |
| `std::args::at` | Yes | Yes | No | Yes | Same compilation error |
| `std::args::address_at` | Yes | Yes | No | No | Same compilation error |
| `std::env::count`, `std::env::at` | Yes | No | No | No | Compilation error: `std::env is not supported on this platform.` |
| `std::standard_output`, `std::standard_input` | Yes | Yes | Yes | No | The declarations do not exist. |
| `std::print`, `std::printf` | Yes | Yes | Yes | Yes | The functions write through `std::standard_output`, or through the Java standard output stream on the JVM. |
| `std::thread` | Linux static and Linux glibc | Yes | Yes | No | Compilation error: `std::thread is not supported on this target.` |
| `std::thin_mutex` | Yes | Yes | Yes | No | The operations reach `UNIMPLEMENTED`. |
| `std::ordering_mutex` | Linux static; Linux glibc on x64 | Yes | x64 | No | The operations reach `UNIMPLEMENTED`. |
| `std::condition_variable` | Yes | Yes | Yes | No | The operations reach `UNIMPLEMENTED`. |
| `std::semaphore` | Yes | Yes | Yes | No | The operations reach `UNIMPLEMENTED`. |
| `std::thread_pool_cort_runtime` and thread-pool coroutine launch | Where `std::thread` is available | Yes | Yes | No | The runtime stores `std::thread` workers. |
| `std::java_string_from_string`, `std::string_from_java_string` | No | No | No | Yes | The declarations do not exist. |

The compilation errors in this table use `COMPILATION_ERROR ON_LOWER`. The
error is reported when a program reaches the function during lowering, and a
program that does not call the function compiles.

`std::thread` uses a different mechanism on each platform.

| Platform | Mechanism |
| --- | --- |
| Linux, `static` environment | Thread creation in the runtime module without a C library, with architecture-specific entry code for every Linux CPU of the test bundle |
| Linux, `glibc` environment | `pthread_create` and `pthread_join` with symbol version `GLIBC_2.34` |
| macOS | `pthread_create` and `pthread_join` from `/usr/lib/libSystem.B.dylib` |
| Windows | `CreateThread`, through `syscall::windows::create_thread` |

`std::ordering_mutex` uses `std::thin_mutex` on Linux with the `static`
environment and a native mutex on the other platforms. The native mutex
storage is declared for x64 only on Linux with `glibc` and on Windows, and for
x64 and ARM64 on macOS.

### Facilities without a target condition

The following source files of the `std` module contain no target predicate.
Their declarations are available on every target on which their dependencies
compile.

| Area | Source files |
| --- | --- |
| Algorithms | `algorithm_buffered.qxs`, `algorithm_iterators.qxs`, `algorithm_ordering.qxs`, `algorithm_ranges.qxs`, `algorithm_search.qxs`, `algorithm_transfer.qxs` |
| Sequences | `dynarr.qxs`, `list.qxs`, `geometric_array.qxs`, `cord.qxs`, `synchronized_pointer_queue.qxs` |
| Associative containers | `avl_tree.qxs`, `ordered_containers.qxs`, `hadix_map.qxs`, `default_hasher.qxs` |
| Text and numbers | `format.qxs`, `byte_regex.qxs`, `big_integer.qxs`, `math.qxs` |
| Ownership and optional values | `optional.qxs`, `unique_ptr.qxs`, `shared_ptr.qxs`, `container_error.qxs` |
| Coroutine support | `coroutine.qxs`, `cpp_coroutine.qxs`, `future.qxs`, `thread_pool_coroutine.qxs` |

The absence of a target condition does not establish correct behavior on the
Cortado backend. Tests of containers, `std::cord`, `std::big_integer`,
`std::byte_regex`, shared pointers, and the AVL tree carry annotations that
mark them as broken on a JVM target; see
[Known Issues](known-issues.md#tests-annotated-for-the-cortado-backend).

## Module `io`

| Facility | Linux | macOS | Windows | JVM |
| --- | --- | --- | --- | --- |
| `io::file_ref`, `io::file` (synchronous read, write, close) | Yes | Yes | Yes | Yes |
| `io::open_read`, `io::open_read_write`, `io::open_replace` | Yes | Yes | No | Yes |
| `io::create_temporary_path` | No | No | No | Yes |
| `io::io_descriptor_ref` | Yes | Yes | Yes | No |
| Asynchronous `read` and `write` members of `io::file` | Yes | Yes | Yes | No |
| `io::async_runtime`, `io::async_context`, `io::async_file`, `io::thread_pool` | Yes | Yes | Yes | No |
| `io::io_uring_processor` and the `io_uring` submission types | Yes | No | No | No |
| `io::epoll_processor` and the `epoll` registration types | Yes | No | No | No |

The module contains no function that opens a file by path on Windows. A
Windows program constructs an `io::file_ref` from a native handle. The Windows
file test in the test bundle operates on a null handle only, so synchronous
Windows file transfers have no test that reads or writes data.

### Asynchronous I/O mechanism

`io::async_context` is an alias for one platform runtime.

| Platform | Runtime alias | Mechanism |
| --- | --- | --- |
| Linux, module option `linux_use_io_uring` is `TRUE` (default) | `io::io_uring_async_runtime` | `io_uring` completion queue |
| Linux, module option `linux_use_io_uring` is `FALSE` | `io::epoll_async_runtime` | `epoll` readiness with worker-backed positioned file transfers |
| macOS | `io::kqueue_async_runtime` | `kqueue`, with positioned file transfers issued as POSIX AIO requests that complete through the queue |
| Windows | `io::overlapped_async_runtime` | Overlapped I/O completion |

`io::io_uring_processor`, `io::epoll_processor`, and their tests exist only on
Linux. The unit tests of `io::async_runtime` are included on Linux, macOS, and
Windows; the positioned round-trip test is included on Linux and macOS only.

## Module `net`

The `net` module selects its implementation with `STATIC_IF` chains over
`OS_LINUX`, `OS_MACOS`, and `OS_WINDOWS`. The module contains no JVM
implementation.

| Facility | Linux | macOS | Windows | JVM |
| --- | --- | --- | --- | --- |
| `net::ipv4_address`, `net::ipv6_address`, `net::ip_address`, `net::endpoint`, `net::error` | Yes | Yes | Yes | Declared; no socket operation is available |
| `net::tcp_connection`, `net::tcp_listener`, `net::udp_session` | Yes | Yes | Yes | No |
| Asynchronous socket operations through `io::async_runtime` | Yes (`io_uring` or `epoll`) | Yes (`kqueue`) | Yes (overlapped I/O) | No |

The socket tests in `main_test_net_sockets.qxs` carry no target condition and
are part of the unit-test suite of every target in the test bundle.

## Supporting modules

### Module `posix`

| Declarations | Availability |
| --- | --- |
| `native_mutex_storage` and the `pthread_mutex_*` procedures | macOS on x64 and ARM64; Linux x64 with the `glibc` environment |
| `native_condition_storage` and the `pthread_cond_*` procedures | macOS on x64 and ARM64 |
| `sched_yield` | macOS |

### Module `syscall`

| Namespace or declaration group | Availability |
| --- | --- |
| `syscall::linux` (system calls, futex, `futex_waitv`, `epoll`, `io_uring`, sockets, clocks) | Linux |
| `syscall::macos` (system interfaces, asynchronous I/O control blocks, sockets, clocks) | macOS |
| `syscall::windows` (kernel interfaces, sockets, socket extensions) | Windows |
| `syscall::windows::native_mutex_storage` and the critical-section procedures | Windows x64 |
| Socket address structures shared by Linux and macOS | Linux and macOS |

### Module `RUNTIME`

The runtime module supplies the declarations that the compiler requires from
`MODULE(RUNTIME)`. The
[Runtime Module Contracts](../reference/runtime-module-contracts.md) reference
specifies the required declarations.

| Facility | Availability |
| --- | --- |
| `PROGRAM_START` | Linux on x64, x86, ARM64, ARM32, RISC-V 32, RISC-V 64, and z/Architecture; Windows on x64; macOS on ARM64 |
| `CHECK_STACK` stack probe | Windows on x64 |
| Exception unwinder | Every native target of the test bundle |
| CPU capability detectors (`DETECT_*`) | x64 feature and vendor capabilities; x86 vendor attributes; ARM64 capabilities on macOS |
| Address-based waiting and notification | Linux for 4-byte values; macOS 14.4 or later for 4-byte and 8-byte values; Windows for 1-byte, 2-byte, 4-byte, and 8-byte values; other targets reach `UNIMPLEMENTED` |
| Thread yield | Linux, macOS, Windows; other targets reach `UNIMPLEMENTED` |
| Benchmark clock (`BENCHMARK_CLOCK`) | Linux, macOS, Windows, and the JVM; other targets produce the compilation error `Benchmark timing is unavailable on this target` |
| Command-line arguments of a unit-test suite | Linux, macOS, Windows, and the JVM |
| Diagnostic output (`write`) | Linux, macOS, Windows, and the JVM; other targets produce the compilation error `Not implemented` |

The unit-test driver reads command-line arguments on Windows through the
Windows command-line interface, whereas `std::args` has no Windows
implementation.

On a JVM target the unit-test driver invokes each test without a `TRY` block.
The driver therefore does not catch an exception that a test throws on that
target.
