# Allocator benchmarks

## mimalloc configuration

On macOS every runner builds mimalloc with compiler thread-local storage for the
thread heap and with `pthread_self()` for the thread identifier. The default
macOS configuration of mimalloc instead reads the thread register directly and
uses two unassigned thread-specific slots of libpthread, which are private
implementation details of the operating system. The runners define
`MI_TLS_MODEL_THREAD_LOCAL`, `MI_USE_BUILTIN_THREAD_POINTER`, and a
`__builtin_thread_pointer()` macro that expands to `pthread_self()`, so the
comparison uses documented operating system interfaces only.

## Bulk allocation and recursive deallocation

The `run_bulk_lifetimes.py` runner compares Quxlang and C++ with mimalloc on a
single-threaded connected-node workload. Each cycle constructs a balanced binary
tree containing exactly `floor(memory_budget / node_size)` separately allocated
nodes. No node is freed during construction. The subsequent postorder traversal
recursively deletes the complete tree. Balanced subtrees keep recursion depth
logarithmic in the node count.

The following command measures a 16 GiB live-node target with 64-byte, 1 KiB,
and 8 KiB nodes:

```sh
python3 quxlang/benchmarks/allocators/run_bulk_lifetimes.py \
  --mimalloc-source misc/build/sources/mimalloc \
  --memory-mib 16384 --sizes 64 1024 8192
```

The budget includes child pointers and payload within live node objects.
Allocator metadata, retained free allocations, and other process memory remain
outside that budget. The target is not an RSS limit. Supported node sizes are
32, 64, 256, 1024, 4096, and 8192 bytes. The runner requires a supported 64-bit
native host.

Both implementations zero-initialize each node before assigning its children
and payload identities. Each node receives a consecutive identity through
opaque writes at 4096-byte payload intervals and at the final payload word.
Recursive deletion reads the same
words and returns their checksum and the number of deleted nodes. The runner
verifies both values against an independent arithmetic reference. Small
verification cases include incomplete trees and budgets with trailing bytes.

Allocation and deallocation have separate timers. Allocation includes node
construction, linking, and payload writes. Deallocation includes traversal,
checksum accumulation, and deletion. Each executable starts with its allocator's
initial state; first-use costs are included. Calibration increases the number
of complete build-and-destroy cycles, rather than the peak live-node target.
Every recorded sample contains at least three seconds of accumulated allocation
work and three seconds of accumulated deallocation work. Implementations use
identical cycle counts, interleaved execution, and three repetitions by default.

The default output directory is `tmp/allocator-comparison-out/bulk-lifetimes`.
Its `results.json` records phase samples, median phase and total seconds per
cycle, calibration history, checksums, build commands, and source and executable
hashes. The `--build-only` option compiles both implementations without running
the workload. The `--memory-mib`, `--cycles`, `--repetitions`, and `--min-seconds`
options control the workload and measurement duration.

## Runtime-sized arrays

The `run_dynamic_arrays.py` runner compares Quxlang and C++ with mimalloc on
single-threaded arrays of 64-bit words whose element count is known only at
run time. Quxlang allocates through `DEFAULT_ALLOCATOR#(U64)::MULTI_ALLOC` and
`MULTI_DEALLOC`; C++ allocates through `std::allocator` with sized
deallocation. Every allocation therefore passes through the allocator's
runtime size dispatch instead of a size class selected at compile time.

```sh
python3 quxlang/benchmarks/allocators/run_dynamic_arrays.py \
  --mimalloc-source misc/build/sources/mimalloc \
  --sizes 64 1024 8192 16384 65536 1048576
```

Each size is a maximum array size in bytes and is a power of two from 16 bytes
to 1 GiB. Sizes above the largest slab size class measure the direct-mapping
path. The runner provides three patterns:

- `fixed` retains a rolling window of arrays that all have the maximum size.
  The difference from the equal-size case of `run_lifetimes.py` is the cost of
  runtime size dispatch.
- `random` retains a rolling window of arrays whose element counts have a
  uniformly distributed binary magnitude up to the maximum size. Consecutive
  allocations select different size classes.
- `growth` repeats the capacity sequence of an appended array. Each step
  allocates twice the previous capacity and releases the previous array, from
  one element to the maximum size. Element copying is excluded, so the result
  isolates allocator cost.

Arrays are not initialized. Each array receives its sequence number in its
first and last elements through opaque writes, and both elements contribute to
a checksum at release. The runner requires equal checksums from both
implementations. Window storage is created outside timing. The work count is
allocations for `fixed` and `random` and complete growth sequences for
`growth`; `results.json` records median nanoseconds per unit of work.

The default output directory is `tmp/allocator-comparison-out/dynamic-arrays`.
The `--windows`, `--patterns`, `--operations`, `--repetitions`, and
`--min-seconds` options control the workload and measurement duration.
