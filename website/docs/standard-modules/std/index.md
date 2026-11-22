# Module `std`

The `std` module provides general-purpose library facilities: containers, strings, ownership
types, formatting, numerics, algorithms, threads, synchronization, futures, and the library side
of coroutines. A source file gains access to the module with `IMPORT std;` and names its
declarations with the prefix `std::`.

## Pages

**Containers**

| Page | Contents |
| --- | --- |
| [`std::dynarr`](dynarr.md) | Contiguous growable array |
| [`std::geometric_array`](geometric-array.md) | Segmented growable array with stable element addresses |
| [`std::list`](list.md) | Doubly linked list |
| [`std::set`](set.md) | Ordered set of unique keys |
| [`std::map`](map.md) | Ordered key/value map |
| [`std::thread_safe_map`](thread-safe-map.md) | Ordered map with synchronized indexed reads and writes |
| [`std::hadix_map`](hadix-map.md) | Hash-radix map and the `std::unordered_map` aliases |
| [`std::default_hasher`](default-hasher.md) | Default hash policy and the hasher contract |
| [`std::avl_tree`](avl-tree.md) | Rank-indexed balanced sequence |
| [`std::container_error`](container-error.md) | Error enumeration thrown by containers |

**Text and formatting**

| Page | Contents |
| --- | --- |
| [`std::string`](string.md) | Owning byte string |
| [`std::cord`](cord.md) | Balanced rope for large byte sequences |
| [`std::fmt`, `std::printf`, and `std::print`](format.md) | Compile-time-validated formatting and output |
| [`std::byte_regex`](byte-regex.md) | Byte-oriented regular expressions |
| [`std::buffered_writer`, `std::buffered_reader`, and standard streams](buffered-io.md) | Buffered transfer and the standard input and output streams |

**Ownership and optional values**

| Page | Contents |
| --- | --- |
| [`std::unique_ptr`](unique-ptr.md) | Exclusive owner of one allocated object |
| [`std::shared_ptr`, `std::strong_ptr`, and `std::weak_ptr`](shared-ptr.md) | Reference-counted shared ownership |
| [`std::optional`](optional.md) | Inline optional value |

**Numerics and process state**

| Page | Contents |
| --- | --- |
| [`std::sqrt`](math.md) | Square root |
| [`std::big_integer`](big-integer.md) | Arbitrary-precision signed integer |
| [`std::args` and `std::env`](process-arguments.md) | Command-line arguments and environment entries |

**Algorithms**

| Page | Contents |
| --- | --- |
| [Algorithms](algorithms.md) | Sequence model, argument conventions, and an index of every algorithm |
| [Iterator utilities](algorithms-iterators.md) | Function objects, ranges, and insert iterators |
| [Search, comparison, and fold algorithms](algorithms-search.md) | Read-only algorithms |
| [Transfer and modification algorithms](algorithms-transfer.md) | Copy, move, transform, fill, replace, remove, rotate, and partition |
| [Ordering algorithms](algorithms-ordering.md) | Heaps, sorting, binary search, merging, and permutations |
| [Buffered algorithms](algorithms-buffered.md) | Stable algorithms that use caller-supplied scratch storage |
| [Range forms](algorithms-ranges.md) | Overloads that accept a range in place of an iterator pair |

**Threads and synchronization**

| Page | Contents |
| --- | --- |
| [`std::thread`](thread.md) | Owner of one native thread |
| [Synchronization primitives](synchronization.md) | Summary of the synchronization types and their target support |
| [`std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`](mutexes.md) | Mutual exclusion |
| [`std::condition_variable`](condition-variable.md) | Blocking until notification |
| [`std::semaphore`](semaphore.md) | Counting semaphore |
| [`std::synchronized_pointer_queue`](synchronized-pointer-queue.md) | Bounded blocking queue of pointers |

**Futures and coroutines**

| Page | Contents |
| --- | --- |
| [`std::future`, `std::promise`, and `std::task`](future.md) | Asynchronous result ownership |
| [`std::coroutine_view` and `std::coroutine_handle`](coroutine.md) | Adapters for coroutine frames |
| [`std::cpp_coroutine_handle` and `std::cpp_style_coroutine_base`](cpp-coroutine.md) | C++-style promise and awaiter adaptation |
| [`std::thread_pool_cort_runtime`, `std::thread_pool_cort_controller`, and `std::cort_yield`](thread-pool-coroutine.md) | Worker pool for coroutines |

## Target restrictions

Most declarations of the module are available on every target. Threads, synchronization
primitives, process arguments, and the standard streams depend on the platform. Each page states
its restrictions, and [Implementation Status: Standard Modules](../../status/standard-modules.md)
contains the complete availability tables.
