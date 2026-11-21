# `std::synchronized_pointer_queue`

Module `std` · source file `synchronized_pointer_queue.qxs`

`std::synchronized_pointer_queue` is a bounded first-in, first-out queue of
mutable pointers that is safe for concurrent producers and consumers. The
capacity is a template value, and the slots are stored inside the queue object
without dynamic allocation. `push` blocks while the queue is full, and `pop`
blocks while the queue is empty. The queue stores borrowed pointers: it neither
owns nor destroys the objects to which the entries point, and ownership of each
pointee transfers according to the protocol of the calling program. A null
pointer is an ordinary entry. The queue is implemented with a
[`std::unfair_mutex`](mutexes.md#stdunfair_mutex) and two
[`std::condition_variable`](condition-variable.md) objects.

## Declaration

```quxlang
::synchronized_pointer_queue TEMPLATE(@T TYPE, @capacity VALUE SZ) STRUCT ROOTED
```

- `T` — the pointee type. Every entry has the type `MUT->T`.
- `capacity` — the maximum number of entries that the queue holds. The value
  must be positive.

A specialization names both parameters, for example
`std::synchronized_pointer_queue#(@T I32, @capacity 2)`. `ROOTED` removes copy
and move construction, so a queue remains at its construction address. The type
declares no constructor and no destructor; a default-initialized queue is
empty.

**Preconditions**: `capacity` is greater than zero. Every thread that accesses
the queue finishes its access before the queue is destroyed.

## Member summary

| Member | Description |
| --- | --- |
| [`push`](#push) | Appends a pointer after waiting for an available slot. |
| [`pop`](#pop) | Removes and returns the oldest pointer after waiting for an entry. |

## Member functions

### `push`

```quxlang
.push FUNCTION(@ARG:value MUT->T) MUT
```

Locks the queue, waits while the queue holds `capacity` entries, stores `value`
after the newest entry, wakes one thread that is blocked in `pop`, and unlocks
the queue.

**Parameters**

- `value` — the pointer to append. `NULL` is permitted and is stored as an
  ordinary entry.

**Complexity**: Constant, excluding the time during which the function is
blocked.

### `pop`

```quxlang
.pop FUNCTION() MUT: MUT->T
```

Locks the queue, waits while the queue is empty, removes the oldest entry,
wakes one thread that is blocked in `push`, and unlocks the queue.

**Return value**: The removed pointer. Entries are returned in the order in
which the corresponding `push` calls stored them. A returned null pointer is
an entry that a `push` call stored; the queue does not use the null pointer to
report a state.

**Complexity**: Constant, excluding the time during which the function is
blocked.

## Target restrictions

The type declares no target-conditional code of its own. The member functions
require `std::unfair_mutex` and `std::condition_variable` and are therefore
implemented when `OS_LINUX || OS_WINDOWS || OS_MACOS` holds (see
[Synchronization primitives](synchronization.md#target-restrictions)). The
tests of the type in the standard test bundle are declared only when
`ARCH_IS_LAYOUTLESS` is `FALSE`.

## Example

A producer thread pushes the addresses of four array elements through a queue
of capacity two and then pushes a null pointer, which this program uses as its
end-of-stream marker. The main thread pops entries until it receives the
marker. One producer and one consumer preserve the order of the entries, so the
output is the same on every run.

```quxlang
--8<-- "example-programs/reference-programs/pointer_queue_transfer/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/pointer_queue_transfer/expected-output.txt"
```

## See also

- [`std::thread`](thread.md)
- [`std::condition_variable`](condition-variable.md)
- [`std::thin_mutex`, `std::ordering_mutex`, and `std::unfair_mutex`](mutexes.md)
- [`std::future`, `std::promise`, and `std::task`](future.md)
- [Pointers](../../reference/pointers.md)
