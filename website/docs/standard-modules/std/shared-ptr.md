# `std::shared_ptr`, `std::strong_ptr`, and `std::weak_ptr`

Module `std` · source file `shared_ptr.qxs`

The shared-ownership pointers keep one heap-allocated object alive for as long
as at least one owner refers to it. The source file provides two independent
families. `std::shared_ptr` stores the object together with a single atomic
reference count and supports no weak observers. `std::strong_ptr` stores the
object in a control block with separate atomic strong and weak counts, and
`std::weak_ptr` observes an object that `strong_ptr` owns without extending
the lifetime of that object. A `weak_ptr` cannot observe a `shared_ptr`.
[`std::unique_ptr`](unique-ptr.md) provides exclusive ownership without a
reference count.

| Property | `shared_ptr#T` | `strong_ptr#T` |
| --- | --- | --- |
| Allocation | One allocation that holds the count and the `T` | One control block that holds the strong count, the weak count, and the storage of the `T` |
| Weak observers | Not supported | [`weak_ptr#T`](#weak_ptr) |
| Factory with constructor arguments | [`make_shared`](#make_shared) | Not provided |
| Factory from one value | [`make_shared_value`](#make_shared_value) | [`make_strong_value`](#make_strong_value) |
| Comparison | `OPERATOR==` | `OPERATOR<=>` |
| Release of the allocation | When the final owner is released | When the final owner and the final weak observer are released |

## Common properties

Each of the three pointer types has exactly one field, a pointer to the shared
allocation, and an empty pointer holds a null pointer. All three types are
copyable and movable. The reference counts are `ATOMIC#SZ` objects that are
updated with atomic operations, so separate pointer objects that refer to the
same object may be copied, reset, and destroyed on different threads. The
pointer field itself is not atomic; the types provide no synchronization for
concurrent access to one pointer object.

Neither family adopts a raw pointer. An owner is created only by the factory
functions, by copying or moving another owner, or, for `strong_ptr`, by
[`weak_ptr::lock`](#weak_ptr-lock).

None of the three types declares an assignment operator. Assignment uses the
compiler-generated operation described in
[Assignment Operators](../../reference/assignment-operators.md). Copy
assignment releases the reference that the receiver held, which destroys the
previously owned object when the receiver was its final owner, and makes the
receiver share ownership with the source. Move assignment releases the
previous reference in the same manner, transfers ownership, and leaves the
source empty. Self-assignment preserves ownership.

The member functions are declared without a receiver qualifier and are
callable on a `CONST&` pointer. The dereference operators return `MUT& T`: a
constant owner provides mutable access to the shared object.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not
evaluated results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults.

### Target restrictions

The three types and the factory functions are declared for every target. The
unit tests of the pointer lifetimes are marked as known to be broken on JVM
targets (`KNOWN_BROKEN_IF(ARCH_IS_JVM)`).

## `std::shared_ptr` { #shared_ptr }

```quxlang
::shared_ptr TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the type of the shared object. The object and its reference count
occupy one allocation. The final owner destroys the object and releases the
allocation in one step.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#shared_ptr-constructor) | Creates an empty pointer, shares ownership, or transfers ownership. |
| [`DESTRUCTOR`](#shared_ptr-destructor) | Releases the reference of the pointer. |
| [`reset`](#shared_ptr-reset) | Releases ownership and leaves the pointer empty. |
| [`has_value`](#shared_ptr-has_value) | Reports whether the pointer owns an object. |
| [`get`](#shared_ptr-get) | Returns a pointer to the owned object. |
| [`use_count`](#shared_ptr-use_count) | Returns the current number of owners. |
| [`OPERATOR->`](#shared_ptr-dereference) | Returns a mutable reference to the owned object. |
| [`OPERATOR==`](#shared_ptr-equality) | Compares ownership identities. |
| [`make_shared`](#make_shared) | Constructs an object from forwarded arguments and returns its first owner. |
| [`make_shared_value`](#make_shared_value) | Constructs an object from one value and returns its first owner. |

### `CONSTRUCTOR` { #shared_ptr-constructor }

```quxlang
.CONSTRUCTOR FUNCTION()                                // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& shared_ptr#(T))    // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& shared_ptr#(T))     // (3)
```

1. Creates an empty pointer.
2. Shares the ownership that `OTHER` holds. When `OTHER` is nonempty, the
   constructor increments the reference count. When `OTHER` is empty, the new
   pointer is empty.
3. Transfers ownership from `OTHER` without changing the reference count.
   `OTHER` is left empty.

**Checked failures**: (2) asserts that the observed count is nonzero, with the
message `Cannot retain an expired shared object.`, and that the count is below
the maximum value of `SZ`, with the message `Shared reference count overflow.`

**Complexity**: Constant.

### `DESTRUCTOR` { #shared_ptr-destructor }

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor calls [`reset`](#shared_ptr-reset).

### `reset` { #shared_ptr-reset }

```quxlang
.reset FUNCTION()
```

The function leaves the pointer empty. When the pointer was nonempty, the
function decrements the reference count; when the pointer was the final owner,
the function deletes the allocation, which destroys the `T`. A call on an
empty pointer has no effect.

**Checked failures**: The function asserts that the count was nonzero before
the decrement, with the message `Shared reference count underflow.`

### `has_value` { #shared_ptr-has_value }

```quxlang
.has_value FUNCTION(): BOOL
```

**Return value**: `TRUE` when the pointer owns an object, and `FALSE` when the
pointer is empty.

### `get` { #shared_ptr-get }

```quxlang
.get FUNCTION(): MUT->T
```

**Return value**: A pointer to the owned object, or a null pointer when the
pointer is empty. The returned pointer does not participate in ownership and
is valid only while an owner keeps the object alive.

### `use_count` { #shared_ptr-use_count }

```quxlang
.use_count FUNCTION(): SZ
```

**Return value**: The current number of owners of the object, or zero when the
pointer is empty. When other threads create or release owners concurrently,
the value is a snapshot.

### `OPERATOR->` { #shared_ptr-dereference }

```quxlang
.OPERATOR-> FUNCTION(): MUT& T
```

**Return value**: A mutable reference to the owned object. Member access
through the pointer is written `pointer->.member`.

**Checked failures**: The operator asserts that the pointer is nonempty, with
the message `Cannot dereference an empty shared pointer.`

### `OPERATOR==` { #shared_ptr-equality }

```quxlang
.OPERATOR== FUNCTION(@OTHER CONST& shared_ptr#(T)): BOOL
```

**Return value**: `TRUE` when both pointers refer to the same allocation, or
when both pointers are empty. The operator compares identities and does not
compare the values of the owned objects.

### `make_shared`

```quxlang
::make_shared TEMPLATE(@T TYPE AUTO) FUNCTION(%...values TT, @KWARGS ...): shared_ptr#(T)
```

The function allocates the object together with its reference count and
constructs the `T` in place by forwarding every positional argument and every
named argument to the constructor of `T`. Because the object is constructed
directly in its final storage, `T` need not be copyable or movable; a `ROOTED`
type is supported. A call without arguments default-constructs the object.

**Return value**: The first owner of the new object. Its `use_count()` is 1.

**Exceptions**: An exception thrown by the constructor of `T` propagates to
the caller.

### `make_shared_value`

```quxlang
::make_shared_value TEMPLATE(@T TYPE AUTO) FUNCTION(@ARG:value AUTO& T): shared_ptr#(T)
```

The function allocates the object together with its reference count and
constructs the `T` from `value`. The reference qualification of `value` is
forwarded, so the object is copy-constructed from a `CONST&` or `MUT&`
argument and move-constructed from a `TEMP&` argument.

**Return value**: The first owner of the new object.

**Exceptions**: An exception thrown by the constructor of `T` propagates to
the caller.

## `std::strong_ptr` { #strong_ptr }

```quxlang
::strong_ptr TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the type of the shared object. The control block holds the strong
count, the weak count, and typed storage for the `T`. All strong owners
together hold one implicit weak reference. The final strong owner destroys the
`T` and releases the implicit weak reference; the control block is released
when the weak count reaches zero. A `weak_ptr` therefore keeps the memory of
the control block allocated after the `T` is destroyed, and never keeps the
`T` alive.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#strong_ptr-constructor) | Creates an empty pointer, shares ownership, or transfers ownership. |
| [`DESTRUCTOR`](#strong_ptr-destructor) | Releases the strong reference of the pointer. |
| [`reset`](#strong_ptr-reset) | Releases ownership and leaves the pointer empty. |
| [`has_value`](#strong_ptr-has_value) | Reports whether the pointer owns a live object. |
| [`get`](#strong_ptr-get) | Returns a pointer to the owned object. |
| [`use_count`](#strong_ptr-use_count) | Returns the current number of strong owners. |
| [`OPERATOR->`](#strong_ptr-dereference) | Returns a mutable reference to the owned object. |
| [`OPERATOR<=>`](#strong_ptr-comparison) | Orders ownership identities. |
| [`make_strong`](#make_strong) | Default-constructs an object and returns its first strong owner. |
| [`make_strong_value`](#make_strong_value) | Constructs an object from one value and returns its first strong owner. |

### `CONSTRUCTOR` { #strong_ptr-constructor }

```quxlang
.CONSTRUCTOR FUNCTION()                                // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& strong_ptr#(T))    // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& strong_ptr#(T))     // (3)
```

1. Creates an empty pointer.
2. Shares the ownership that `OTHER` holds. When `OTHER` is nonempty, the
   constructor increments the strong count.
3. Transfers ownership from `OTHER` without changing the strong count. `OTHER`
   is left empty.

**Checked failures**: (2) asserts that the observed strong count is nonzero,
with the message `Cannot retain an expired shared object.`, and that the count
is below the maximum value of `SZ`, with the message
`Shared reference count overflow.`

**Complexity**: Constant.

### `DESTRUCTOR` { #strong_ptr-destructor }

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor calls [`reset`](#strong_ptr-reset).

### `reset` { #strong_ptr-reset }

```quxlang
.reset FUNCTION()
```

The function leaves the pointer empty. When the pointer was nonempty, the
function decrements the strong count. When the pointer was the final strong
owner, the function destroys the `T`, releases the implicit weak reference,
and deletes the control block if no `weak_ptr` refers to it. A call on an
empty pointer has no effect.

**Checked failures**: The function asserts that the strong count was nonzero
before the decrement, with the message `Shared reference count underflow.`,
and that the weak count was nonzero before the release of the implicit weak
reference, with the message `Weak reference count underflow.`

### `has_value` { #strong_ptr-has_value }

```quxlang
.has_value FUNCTION(): BOOL
```

**Return value**: `TRUE` when the pointer owns an object, and `FALSE` when the
pointer is empty.

### `get` { #strong_ptr-get }

```quxlang
.get FUNCTION(): MUT->T
```

**Return value**: A pointer to the owned object, or a null pointer when the
pointer is empty. The returned pointer does not participate in ownership.

### `use_count` { #strong_ptr-use_count }

```quxlang
.use_count FUNCTION(): SZ
```

**Return value**: The current number of strong owners of the object, or zero
when the pointer is empty. Weak observers are not counted.

### `OPERATOR->` { #strong_ptr-dereference }

```quxlang
.OPERATOR-> FUNCTION(): MUT& T
```

**Return value**: A mutable reference to the owned object.

**Checked failures**: The operator asserts that the pointer is nonempty, with
the message `Cannot dereference an empty shared pointer.`

### `OPERATOR<=>` { #strong_ptr-comparison }

```quxlang
.OPERATOR<=> FUNCTION(@OTHER CONST& strong_ptr#(T)): ORDER
```

**Return value**: The ordering of the control-block pointers of the two
operands. Two pointers compare as `ORDER::EQUAL` when both refer to the same
control block or when both are empty. The operator supplies the comparison
expressions for `strong_ptr`, such as `first == second`; it compares
identities and does not compare the values of the owned objects.

### `make_strong`

```quxlang
::make_strong TEMPLATE(@T TYPE AUTO) FUNCTION(): strong_ptr#(T)
```

The function allocates one control block that contains a default-constructed
`T`. The function accepts no constructor arguments.

**Return value**: The first strong owner of the new object. Its `use_count()`
is 1.

**Exceptions**: An exception thrown by the default constructor of `T`
propagates to the caller.

### `make_strong_value`

```quxlang
::make_strong_value TEMPLATE(@T TYPE AUTO)
    FUNCTION(@ARG:value AUTO& T): strong_ptr#(T)
```

The function allocates one control block that contains a `T` constructed from
`value`. The reference qualification of `value` is forwarded, so the object is
copy-constructed from a `CONST&` or `MUT&` argument and move-constructed from
a `TEMP&` argument.

**Return value**: The first strong owner of the new object.

**Exceptions**: An exception thrown by the constructor of `T` propagates to
the caller.

## `std::weak_ptr` { #weak_ptr }

```quxlang
::weak_ptr TEMPLATE(@T TYPE AUTO) STRUCT
```

A `weak_ptr#T` observes an object that `strong_ptr#T` owns. The observation
holds one weak reference to the control block and no strong reference. The
object is reached only through [`lock`](#weak_ptr-lock), which returns a
strong owner when the object is still alive. `weak_ptr` declares no
dereference operator and no comparison operator.

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#weak_ptr-constructor) | Creates an empty observer, observes a strong owner, or copies or transfers an observation. |
| [`DESTRUCTOR`](#weak_ptr-destructor) | Releases the weak reference of the observer. |
| [`reset`](#weak_ptr-reset) | Releases the observation and leaves the observer empty. |
| [`use_count`](#weak_ptr-use_count) | Returns the current number of strong owners. |
| [`expired`](#weak_ptr-expired) | Reports whether no strong owner remains. |
| [`lock`](#weak_ptr-lock) | Acquires shared ownership when the object is still alive. |

### `CONSTRUCTOR` { #weak_ptr-constructor }

```quxlang
.CONSTRUCTOR FUNCTION()                                // (1)
.CONSTRUCTOR FUNCTION(@OTHER CONST& strong_ptr#(T))    // (2)
.CONSTRUCTOR FUNCTION(@OTHER CONST& weak_ptr#(T))      // (3)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& weak_ptr#(T))       // (4)
```

1. Creates an empty observer.
2. Observes the object that `OTHER` owns and increments the weak count. When
   `OTHER` is empty, the new observer is empty. The construction is written
   `std::weak_ptr#T(@OTHER owner)`.
3. Copies the observation of `OTHER` and increments the weak count.
4. Transfers the observation of `OTHER` without changing the weak count.
   `OTHER` is left empty.

No constructor accepts a `shared_ptr`.

**Checked failures**: (2) and (3) assert that the observed weak count is
nonzero, with the message `Cannot retain a released shared control block.`,
and that the count is below the maximum value of `SZ`, with the message
`Weak reference count overflow.`

### `DESTRUCTOR` { #weak_ptr-destructor }

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor calls [`reset`](#weak_ptr-reset).

### `reset` { #weak_ptr-reset }

```quxlang
.reset FUNCTION()
```

The function leaves the observer empty. When the observer was nonempty, the
function decrements the weak count, and deletes the control block when the
observer held the final weak reference. The final weak reference can be
released only after every strong owner is released, because the strong owners
hold the implicit weak reference. A call on an empty observer has no effect.

**Checked failures**: The function asserts that the weak count was nonzero
before the decrement, with the message `Weak reference count underflow.`, and,
before the control block is deleted, that the strong count is zero, with the
message `A live shared object must retain its implicit weak reference.`

### `use_count` { #weak_ptr-use_count }

```quxlang
.use_count FUNCTION(): SZ
```

**Return value**: The current number of strong owners of the observed object,
or zero when the observer is empty.

### `expired` { #weak_ptr-expired }

```quxlang
.expired FUNCTION(): BOOL
```

**Return value**: `TRUE` when `use_count()` is zero. An empty observer and an
observer whose object is destroyed both report `TRUE`. When other threads hold
strong owners, a result of `FALSE` can be outdated by the time the caller acts
on it; [`lock`](#weak_ptr-lock) performs the test and the acquisition as one
atomic step.

### `lock` { #weak_ptr-lock }

```quxlang
.lock FUNCTION(): strong_ptr#(T)
```

The function increments the strong count with a compare-and-exchange loop that
succeeds only while the count is nonzero. An object whose final strong owner
is already released is therefore never revived.

**Return value**: A strong owner of the observed object when the object is
alive. An empty `strong_ptr#(T)` when the observer is empty or the object is
destroyed.

**Checked failures**: The function asserts that the strong count is below the
maximum value of `SZ`, with the message `Shared reference count overflow.`

## Example

The program shares one object between two `shared_ptr` owners, and then
observes a `strong_ptr` object through a `weak_ptr` before and after the final
strong owner is released.

```quxlang
--8<-- "example-programs/reference-programs/shared_ptr_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/shared_ptr_example/expected-output.txt"
```

## See also

- [`std::unique_ptr`](unique-ptr.md)
- [`std::optional`](optional.md)
- [`std::thread`](thread.md)
- [`std::future`](future.md)
- [Atomics](../../reference/atomics.md)
- [Move Semantics](../../reference/move-semantics.md)
- [Object Storage and Lifetime](../../reference/typed-storage-and-lifetime.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
