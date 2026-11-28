# `io::kqueue_processor`

Module `io` · source file `kqueue_processor.qxs`

`io::kqueue_processor` owns a Darwin kqueue for descriptor readiness, raised
signals and positioned file transfers. Each registration names an
`io::kqueue_target`. The processor dequeues events and delivers each event to
the target of its registration.

## Lifetime and concurrency

Construction opens a kqueue and sets its close-on-exec flag. Construction,
registration and wait failures throw `io::kqueue_error`. Its `code` field holds
the Darwin `errno` as an `I32`.

The processor has a stable address (`STRUCT ROOTED`). Registration, raising and
event processing can run concurrently on any threads. Several threads can
process events at the same time. Darwin hands each event to one of them, and
that thread delivers it. A target can receive its event before the registering
call returns.

Destruction requires exclusive access to the processor after every registered
target has received its final event. Closing the queue discards remaining
readiness and signal registrations. A close failure panics. The caller owns
each file descriptor that it registers or uses for a transfer.

`io::async_runtime2` uses one kqueue on macOS for socket readiness, native
file completions and the wake of its consumer thread.

## Targets

```quxlang
::kqueue_target STRUCT ROOTED
{
  .context VAR MUT->VOID;
  .deliver VAR CONST->PROCEDURE(@context MUT->VOID,
    @event CONST->syscall::macos::kernel_event);
}
```

A target identifies the receiver of the events of one operation. The operation
embeds the target at a stable address and assigns both fields before the first
registration. The processor calls `deliver` with `context` and the dequeued
event on the thread that dequeued it. The processor does not access the target
after that call begins, so the receiver may release the target during delivery.

A target serves one pending registration at a time, except that a raised
target keeps its persistent registration. The address of the target
distinguishes registrations of the same descriptor and condition.

Three functions decode the event that a target receives:

| Function | Result |
| --- | --- |
| `io::kqueue_readiness_error(@event event)` | The `errno` reported with a readiness event, or zero. |
| `io::kqueue_transfer_error(@event event)` | The `errno` of a completed file transfer, or zero on success. |
| `io::kqueue_transfer_size(@event event)` | The byte count of a successful file transfer. |

Each function accepts `CONST& syscall::macos::kernel_event`. A receiver applies
the function that corresponds to the registration it made.

## Descriptor readiness

```quxlang
.watch_readable FUNCTION(@descriptor io_descriptor_ref, @target MUT& kqueue_target) CONST
.watch_writable FUNCTION(@descriptor io_descriptor_ref, @target MUT& kqueue_target) CONST
```

Each function arms a one-shot registration for the descriptor and the target.
The target receives one event when the descriptor becomes readable or writable.
Distinct targets observe the same condition independently, and each receives
its own event. The descriptor stays open until the event is delivered.
A registration that Darwin rejects throws `io::kqueue_error`, and the target
receives no event.

## Raised targets

```quxlang
.raise FUNCTION(@target MUT& kqueue_target) CONST
```

`raise` delivers one event to the target from a thread that processes events.
The first call creates a persistent `EVFILT_USER` registration, and later calls
reuse it. Calls made before the event is dequeued coalesce into one delivery. A
call made after delivery produces another event. The target outlives the last
delivery.

## Positioned file transfers

```quxlang
::kqueue_transfer_request STRUCT ROOTED

.read FUNCTION(@request MUT& kqueue_transfer_request, @descriptor io_descriptor_ref,
  @buffer CONST& buffer_ref, @offset U64, @target MUT& kqueue_target) CONST: I32
.write FUNCTION(@request MUT& kqueue_transfer_request, @descriptor io_descriptor_ref,
  @buffer CONST& buffer_ref, @offset U64, @target MUT& kqueue_target) CONST: I32
```

A request contains the native Darwin `aiocb`. `read` and `write` issue one
transfer and return zero when Darwin accepts it. A rejected transfer returns
the Darwin `errno`, creates no completion, and leaves the request available
for reuse. Darwin reports `EAGAIN` when the process has reached its limit of
queued transfers, which `kern.aioprocmax` sets.

The offset is in bytes from the beginning of the file and is at most the
largest signed 64-bit value. Each call requests up to `2³¹ − 1` bytes from the
supplied buffer.

An accepted transfer delivers one event to the target.
`io::kqueue_transfer_error` returns zero for success or the Darwin error
number, and `io::kqueue_transfer_size` returns the byte count of a successful
transfer, including zero at end of file. The queue reaps the transfer when the
event is dequeued. The request, the file descriptor, the buffer storage and the
target stay valid until then. Concurrent transfers use distinct requests.

Positioned transfers require the `SIGEV_KEVENT` notification mode available on
macOS 26. Earlier Darwin kernels reject this mode with `EINVAL`.
`io::kqueue_native_aio_available()` checks the running kernel release and
returns `FALSE` if the release cannot be read. `io::async_runtime2` performs
positioned transfers on file workers on those kernels, and when Darwin reports
`EAGAIN`.
[Darwin 25 AIO](https://github.com/apple-oss-distributions/xnu/blob/xnu-12377.1.9/bsd/kern/kern_aio.c)
and [Darwin 24 AIO](https://github.com/apple-oss-distributions/xnu/blob/xnu-11215.1.10/bsd/kern/kern_aio.c).

## Processing events

```quxlang
.process_events FUNCTION(@maximum SZ, @timeout_milliseconds I32 DEFAULT(0)) CONST: SZ
```

`process_events` dequeues at most `maximum` events and delivers each one to its
target on the calling thread. The result counts the delivered events. A zero
maximum returns immediately and leaves pending events available to later calls.

The timeout applies to the first dequeue. A zero timeout polls, a positive
value bounds the wait in milliseconds, and a negative value waits indefinitely.
Later dequeues of the same call poll. A timeout or an interrupted wait ends the
call.

```quxlang
::wake STRUCT ROOTED
{
  .target VAR io::kqueue_target;
  .count VAR SZ;

  .CONSTRUCTOR FUNCTION()
  {
    .target.context := THIS<- AS REINTERPRET MUT->VOID;
    .target.deliver := on_event<-;
  }

  ::on_event FUNCTION(@context MUT->VOID, @event CONST->syscall::macos::kernel_event)
  {
    VAR self := context AS REINTERPRET MUT->wake;
    self->.count++;
  }
}

VAR processor io::kqueue_processor;
VAR signal wake;
processor.raise(@target signal.target);
VAR delivered SZ := processor.process_events(@maximum 8, @timeout_milliseconds 1000);
```

The declarations are available on `OS_MACOS`.
