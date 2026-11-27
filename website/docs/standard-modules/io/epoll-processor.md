# `io::epoll_processor`

Module `io` · source file `epoll_processor.qxs`

`io::epoll_processor` owns one Linux epoll instance and delivers readiness
notifications to borrowed operation objects on the thread that asks for them.
Any number of operations can wait on one descriptor. The processor keeps the
operations of each descriptor in a [`std::hadix_map`](../std/hadix-map.md)
entry and maintains one one-shot kernel registration per descriptor, armed with
a mask covering the interests of those operations. The result of each readiness
notification decides whether its operation continues to wait. The processor is independent
of [`io::async_context`](async-context.md),
[`io::async_runtime`](async-runtime.md), coroutine controllers, and futures; it
starts no thread. The processor is available on Linux for both values of the
option [`io::linux_use_io_uring`](index.md#module-options), which selects only
the backend of `io::async_context`.

## Declarations

```quxlang
::epoll_error INCLUDE_IF(OS_LINUX) ENUM [ initialization_failed, registration_failed, wait_failed ];
::epoll_events INCLUDE_IF(OS_LINUX) FLAGSET BITS(32)
::operation_status ENUM [ pending, finished ];
::epoll_operation_ref INCLUDE_IF(OS_LINUX) GENERIC_REF INCOMPARABLE
::epoll_registration_id INCLUDE_IF(OS_LINUX) STRUCT
::epoll_processor INCLUDE_IF(OS_LINUX) STRUCT ROOTED
```

| Entity | Description |
| --- | --- |
| [`io::epoll_processor`](#member-summary) | Owner of the epoll instance. |
| [`io::epoll_operation_ref`](#ioepoll_operation_ref) | Borrowed interface of an operation object. |
| [`io::epoll_events`](#ioepoll_events) | Set of readiness conditions. |
| [`io::operation_status`](#iooperation_status) | Result of one notification. |
| [`io::epoll_registration_id`](#ioepoll_registration_id) | Identity of an outstanding registration, used for removal. |
| [`io::epoll_error`](#ioepoll_error) | Exception type of the processor. |

`ROOTED` removes copy and move construction, so a processor remains at its
address for its whole lifetime.

## Registration lifecycle

A registration is *outstanding* from publication by [`add`](#add) until one
of the following events:

- the notification of the operation returns `operation_status::finished`;
- [`remove`](#remove) ends the registration;
- the processor delivers the terminal `rearm_failed` callback.

Each call of `add` allocates one registration record that holds a copy of the
operation reference, and appends the record to the list of its descriptor. A
descriptor has an entry in the descriptor table while it has a listed
registration or undergoes notification. The processor releases a record when
its registration stops being outstanding.

The kernel holds at most one registration per descriptor. The processor arms
that kernel registration with `EPOLLONESHOT` and with the union of the
interests of the listed registrations, so the kernel reports each arming at
most once. An arming that already covers the interest of a new registration is
left unchanged, and `add` then performs no system call.

A kernel event notifies every listed registration whose interest contains a
reported condition. An event that reports `error` or `hangup` notifies every
listed registration of the descriptor. When several operations wait on the
same descriptor, their notification order is unspecified. A registration that
the event does not select remains listed. After the notifications, the
processor arms the descriptor again for the registrations that remain.
When the last registration ends, the processor deletes the kernel watch with
`EPOLL_CTL_DEL` and erases the descriptor table entry. During notification,
the processor performs this cleanup after the readiness callbacks for that
event return. Selection accumulates the interests of the unselected operations.
New and continuing registrations contribute their interests as they join the
list, so rearming requires no second traversal. Removal can leave a wider
interest mask until the next selection.

Each arming carries a new generation number, which the kernel returns with the
event. The processor discards an event whose generation differs from the
recorded generation of the descriptor and an event of a descriptor without a
table entry. Such stale events arise when a registration is added or removed
after the kernel transferred an event of the descriptor.

A notification reports readiness that the kernel observed before the
notification began. An operation treats a notification as permission to
attempt its transfer and returns `operation_status::pending` when the transfer
would block. Several operations that wait for the same condition of one
descriptor all receive the notification, although the first transfer can
consume the readiness.

## Concurrency contract

| Role | Member functions | Requirement |
| --- | --- | --- |
| Producer | [`add`](#add) | Any thread. Several producers can run concurrently with each other and with the consumer. |
| Consumer | [`process_events`](#process_events), [`remove`](#remove) | One serialized consumer. A notification runs in the consumer role and can call `add` and `remove`. |
| Exclusive | Construction, destruction | No other access to the processor. |

One [`std::ordering_mutex`](../std/mutexes.md) protects the descriptor table,
the registration lists, and the arming of kernel registrations. `add` holds the
mutex for its whole table update, including its system call. `process_events`
holds the mutex while it selects the registrations of an event and while it
arms the descriptor afterwards, and releases the mutex for the wait and for
every notification. A notification can begin on the consumer thread as soon as
the call of `add` that registered its operation releases the mutex.
[`pending_count`](#pending_count) reads atomic state and returns an advisory
snapshot.

## Member summary

**Construction and destruction**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Opens the epoll descriptor. |
| [`DESTRUCTOR`](#destructor) | Closes the epoll descriptor of a processor without outstanding registrations. |

**Registration**

| Member | Description |
| --- | --- |
| [`add`](#add) | Registers an operation for its descriptor and interest. |
| [`remove`](#remove) | Ends an outstanding registration without notification. |
| [`poll_descriptor`](#poll_descriptor) | Borrows the Linux descriptor for use in an outer event loop. |
| [`pending_count`](#pending_count) | Returns the number of outstanding registrations. |

**Notification**

| Member | Description |
| --- | --- |
| [`process_events`](#process_events) | Waits for a bounded number of readiness events and notifies the operations that they select. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()
```

Opens one epoll descriptor with close-on-exec enabled (`EPOLL_CLOEXEC`).

**Exceptions**: `epoll_error::initialization_failed` is thrown when the kernel
rejects the request.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

Closes the epoll descriptor, which discards every kernel registration of the
processor. The destructor closes no watched descriptor.

**Preconditions**: no registration is outstanding, so that
[`pending_count`](#pending_count) returns zero. The destructor evaluates this
requirement in every build configuration: a nonzero count results in `PANIC`. A
failure to close the epoll descriptor also results in `PANIC`.

### `add`

```quxlang
.add FUNCTION(@ARG:operation epoll_operation_ref) MUT: epoll_registration_id
```

Calls `operation.descriptor` and `operation.interest`, allocates a registration
record, and appends the record to the list of the descriptor. The function
then arms the kernel registration of the descriptor unless one of the
following conditions holds:

- the current arming already covers the interest;
- the consumer is notifying registrations of the descriptor, in which case the
  consumer arms the descriptor when those notifications finish.

The first registration creates the watch with `EPOLL_CTL_ADD`. An existing
watch whose interest needs widening is updated with `EPOLL_CTL_MOD`.

**Parameters**: `operation` borrows the operation object. The registration
keeps the borrowed reference while the registration is outstanding.

**Return value**: the identity of the new registration.

**Preconditions**:

- The operation object remains at one address, and alive, while the
  registration is outstanding.
- The descriptor is open and remains open while the registration is
  outstanding. When a readiness callback ends the last registration, the
  descriptor must stay open through cleanup of that event. A caller can close
  it after `process_events` returns.

**Exceptions**: `epoll_error::registration_failed` is thrown when the kernel
rejects the arming. Causes include an invalid descriptor, a file type without
readiness notification such as a regular file, and an exhausted watch limit.
The processor then retains no reference to the operation, `pending_count` is
unchanged, and the other registrations of the descriptor keep their arming. An
allocation failure propagates with the same guarantees.

### `remove`

```quxlang
.remove FUNCTION(@ARG:registration epoll_registration_id) MUT
```

Ends an outstanding registration without notifying its operation, releases its
record, and decrements [`pending_count`](#pending_count). The operation
receives no notification after the function returns. This guarantee includes a
notification that the event in progress already selected for the registration
and an event that the kernel already transferred to the call of
[`process_events`](#process_events) in progress. The operation object can
therefore be destroyed as soon as `remove` returns.

Removing the last registration outside notification deletes the watch before
`remove` returns. A kernel registration that remains armed
for a wider interest than the remaining registrations produces at most one
event, which selects no removed registration.

A readiness notification can remove any outstanding registration, including its own. A
notification that removes its own registration does not continue, whatever
status it returns.

**Preconditions**:

- `registration` identifies an outstanding registration of this processor.
- The call is serialized with `process_events`.

### `poll_descriptor`

```quxlang
.poll_descriptor FUNCTION() CONST: io_descriptor_ref
```

The borrowed descriptor is readable while the processor has kernel events
available. An outer event loop can monitor it for readability and call
`process_events` with a zero timeout. The processor owns the descriptor and
its interest list. The borrow is valid during the processor's lifetime.

### `pending_count`

```quxlang
.pending_count FUNCTION() CONST: SZ
```

**Return value**: the number of outstanding registrations.

### `process_events`

```quxlang
.process_events FUNCTION(@maximum SZ, @timeout_milliseconds I32 DEFAULT(0)) MUT: SZ
```

Transfers at most `maximum` readiness events from the kernel and, for each
event, calls `notify` of every registration that the event selects, on the
calling thread. The function transfers events in batches of at most 32 and
waits again only after a full batch.

**Parameters**:

- `maximum` bounds the number of kernel events that the call transfers. One
  event can produce several notifications. A value of zero returns without
  entering the kernel.
- `timeout_milliseconds` bounds the first wait. Zero polls without blocking. A
  negative value waits indefinitely. Every later wait of the same call polls
  without blocking.

**Return value**: the number of notifications delivered. A stale event and an
event that selects no registration are transferred and add nothing to the
result. A timeout and a signal interruption end the call without an error.

Each notification receives the reported conditions that the interest of its
operation contains, together with `error` and `hangup`. The processor acts on
the returned status:

| Status | Effect |
| --- | --- |
| `operation_status::pending` | The processor calls `interest` and returns the registration to the list of its descriptor. |
| `operation_status::finished` | The processor releases the registration record, decrements `pending_count`, and does not access the operation again. |

After the notifications of an event, the processor arms the descriptor once
with the accumulated interest mask (`EPOLL_CTL_MOD`).

**Exceptions**:

- `epoll_error::wait_failed` is thrown when the wait system call fails for a
  reason other than a signal interruption.
- `epoll_error::registration_failed` is thrown when the arming after an event
  fails. The processor retires the watch, calls `rearm_failed` on each affected
  operation that has not been removed, and finishes the transferred events
  before throwing. This includes operations skipped by the readiness event
  and registrations added during its callbacks. A failure callback can cancel
  another outstanding registration or register a new operation on the same
  descriptor. New registrations receive a fresh watch.

**Preconditions**: the call is serialized with other consumer calls, and no
notification calls `process_events`. A build with the `ASSERT_ENABLED` policy
evaluates the second requirement.

## `io::epoll_operation_ref`

```quxlang
::epoll_operation_ref INCLUDE_IF(OS_LINUX) GENERIC_REF INCOMPARABLE
{
  .descriptor FUNCTION() CONST NOEXCEPT: io_descriptor_ref;
  .interest FUNCTION() CONST NOEXCEPT: epoll_events;
  .notify FUNCTION(@events epoll_events) MUT NOEXCEPT: operation_status;
  .rearm_failed FUNCTION() MUT NOEXCEPT;
}
```

`io::epoll_operation_ref` is a non-owning generic reference to an object that
supplies the four member functions. An object of a type with these members
converts to the reference at the call of `add`.
[Generics](../../reference/generics.md) specifies `GENERIC_REF`.

- `descriptor` returns an [`io::io_descriptor_ref`](io-descriptor-ref.md)
  borrowing the open descriptor to watch. The processor stores the reference
  during `add`. The operation owner retains responsibility for its lifetime.
- `interest` returns the conditions that the operation waits for. The
  processor calls the function during `add` and after each notification that
  returns `operation_status::pending`, so one registration can wait for
  different conditions in sequence. An operation with an empty interest
  receives only `error` and `hangup`.
- `notify` receives the observed conditions and returns the status of the
  operation. The function does not block and does not call `process_events`.
  A function that returns `operation_status::finished` can release the
  operation object before it returns, because the processor does not access the
  operation afterwards. The descriptor follows the lifetime requirements of
  [`add`](#add).
- `rearm_failed` reports a terminal failure to continue the registration. It
  runs on the consumer thread outside the processor mutex. The callback may
  release its operation object. The processor makes no further calls on the
  operation for this registration.

All four functions are `NOEXCEPT`. The events of a batch have already left
the kernel when `notify` runs, so an exception could not return the remaining
events to the kernel.

## `io::epoll_events`

```quxlang
::epoll_events INCLUDE_IF(OS_LINUX) FLAGSET BITS(32)
[
  readable = 1,
  priority = 2,
  writable = 4,
  error = 8,
  hangup = 16,
  peer_closed = 8192
];
```

Each entry has the mask of the corresponding native event.

| Entry | Native event | Meaning |
| --- | --- | --- |
| `readable` | `EPOLLIN` | A read operation does not block. |
| `priority` | `EPOLLPRI` | An exceptional condition is present, such as out-of-band data. |
| `writable` | `EPOLLOUT` | A write operation does not block. |
| `error` | `EPOLLERR` | An error condition is present. |
| `hangup` | `EPOLLHUP` | The peer closed the channel. |
| `peer_closed` | `EPOLLRDHUP` | The peer of a stream socket closed its writing half. |

The type serves as the interest of a registration and as the argument of a
notification. The kernel reports `error` and `hangup` for every armed
descriptor, whether or not an interest contains them, and the processor
delivers both to every registration of the descriptor. The processor adds
`EPOLLONESHOT` to every arming; the flagset declares no entry for trigger
modes. [Flagsets](../../reference/flagsets.md) specifies membership tests and
set operators.

## `io::operation_status`

```quxlang
::operation_status ENUM [ pending, finished ];
```

| Enumerator | Meaning |
| --- | --- |
| `pending` | The operation requires a further notification. |
| `finished` | The operation requires no further notification. |

## `io::epoll_registration_id`

```quxlang
::epoll_registration_id INCLUDE_IF(OS_LINUX) STRUCT
```

An `epoll_registration_id` identifies one outstanding registration in the
processor that returned it. The structure has no public member; its only use is
as the argument of [`remove`](#remove). An identity expires when the
notification that returns `operation_status::finished` begins, when `remove`
ends the registration, or when its terminal `rearm_failed` callback begins. Because
`remove` and notifications both run in the consumer role, the consumer observes
every one of these events before it can pass the identity to `remove`.

## `io::epoll_error`

| Enumerator | Thrown by | Condition |
| --- | --- | --- |
| `initialization_failed` | [`CONSTRUCTOR`](#constructor) | The kernel did not create the epoll instance. |
| `registration_failed` | [`add`](#add), [`process_events`](#process_events) | The kernel rejected the arming of a descriptor. |
| `wait_failed` | [`process_events`](#process_events) | The wait system call failed for a reason other than a signal. |

## Target restrictions

| Declaration | Declared when |
| --- | --- |
| `epoll_processor`, `epoll_operation_ref`, `epoll_events`, `epoll_registration_id`, `epoll_error` | `OS_LINUX` |
| `operation_status` | Every target |

## Example

The following fragment is taken from the unit tests of the processor
(`main_test_epoll_processor.qxs`). The fragment requires Linux. The operation
type returns `pending` for a configured number of notifications and then
returns `finished`. The first test registers an operation on a readable event
counter, observes one notification per call while the operation is pending, and
observes no notification after the operation finished, although the counter
remains readable. The second test registers a reader and a writer on one event
counter that is writable and not yet readable.

```quxlang
::waiter STRUCT
{
  .watched VAR io::io_descriptor_ref;
  .wanted VAR io::epoll_events;
  .repeats VAR SZ;
  .count VAR SZ;
  .events VAR io::epoll_events;
  .failures VAR SZ;

  .descriptor FUNCTION() CONST NOEXCEPT: io::io_descriptor_ref { RETURN .watched; }
  .interest FUNCTION() CONST NOEXCEPT: io::epoll_events { RETURN .wanted; }
  .rearm_failed FUNCTION() MUT NOEXCEPT { .failures++; }
  .notify FUNCTION(@events io::epoll_events) MUT NOEXCEPT: io::operation_status
  {
    .events := events;
    .count++;
    IF (.count <= .repeats) { RETURN io::operation_status::pending; }
    RETURN io::operation_status::finished;
  }
}

DOC <$ A pending result rearms the registration, and a finished result ends it. A finished registration stays silent while its descriptor remains ready. $>
::rearming UNIT_TEST
{
  VAR descriptor io::io_descriptor_ref := open_counter(1);
  TEST_ASSERT(descriptor.is_valid());
  DEFER { syscall::linux::close(@fd descriptor.native_handle()); }
  VAR operation waiter;
  operation.watched := descriptor;
  operation.wanted := io::epoll_events::readable;
  operation.repeats := 1;
  VAR processor io::epoll_processor;
  processor.add(operation);
  TEST_ASSERT(processor.pending_count() == 1);
  TEST_ASSERT(processor.process_events(@maximum 8, @timeout_milliseconds 1000) == 1);
  TEST_ASSERT(operation.count == 1 && operation.events.readable);
  TEST_ASSERT(processor.pending_count() == 1);
  TEST_ASSERT(processor.process_events(@maximum 8, @timeout_milliseconds 1000) == 1);
  TEST_ASSERT(operation.count == 2);
  TEST_ASSERT(processor.pending_count() == 0);
  TEST_ASSERT(processor.process_events(@maximum 8) == 0);
  TEST_ASSERT(operation.count == 2);
}

DOC <$ A reader and a writer share one descriptor, and each receives only the conditions that its interest selects. $>
::shared_descriptor UNIT_TEST
{
  VAR descriptor io::io_descriptor_ref := open_counter(0);
  TEST_ASSERT(descriptor.is_valid());
  DEFER { syscall::linux::close(@fd descriptor.native_handle()); }
  VAR reader waiter;
  reader.watched := descriptor;
  reader.wanted := io::epoll_events::readable;
  VAR writer waiter;
  writer.watched := descriptor;
  writer.wanted := io::epoll_events::writable;
  VAR processor io::epoll_processor;
  processor.add(reader);
  processor.add(writer);
  TEST_ASSERT(processor.pending_count() == 2);
  TEST_ASSERT(processor.process_events(@maximum 8, @timeout_milliseconds 1000) == 1);
  TEST_ASSERT(reader.count == 0 && writer.count == 1);
  TEST_ASSERT(writer.events.writable && ((writer.events.readable) == FALSE));
  TEST_ASSERT(processor.pending_count() == 1);
  TEST_ASSERT(processor.process_events(@maximum 8) == 0);
  TEST_ASSERT(signal_counter(descriptor));
  TEST_ASSERT(processor.process_events(@maximum 8, @timeout_milliseconds 1000) == 1);
  TEST_ASSERT(reader.count == 1 && writer.count == 1);
  TEST_ASSERT(reader.events.readable && ((reader.events.writable) == FALSE));
  TEST_ASSERT(processor.pending_count() == 0);
}
```

## See also

- [`io::io_uring_processor`](io-uring-processor.md)
- [`std::hadix_map`](../std/hadix-map.md)
- [`std::ordering_mutex`](../std/mutexes.md)
- [`io::async_context`](async-context.md)
- [Module `io`](index.md)
- [Generics](../../reference/generics.md)
- [Flagsets](../../reference/flagsets.md)
- [Tests](../../reference/tests.md)
- [Availability and Targets](../../reference/availability-and-targets.md)
