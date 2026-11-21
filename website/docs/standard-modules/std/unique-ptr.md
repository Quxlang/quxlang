# `std::unique_ptr`

Module `std` · source file `unique_ptr.qxs`

`std::unique_ptr` owns at most one object of type `T` that was allocated by
`NEW`, and deletes that object when ownership ends. Ownership is exclusive: the
type is move-only, and every move leaves the source owner empty. The owner
stores one `MUT->T` and no other state.
[`std::shared_ptr` and `std::strong_ptr`](shared-ptr.md) provide shared
ownership with reference counting, and [`std::optional`](optional.md) stores
an optional value inline without a heap allocation.

## Declaration

```quxlang
::unique_ptr TEMPLATE(@T TYPE AUTO) STRUCT MOVE_ONLY
```

`T` is the type of the owned object. `T` may be a fixed-size array type such as
`[2]I32`; the owner then manages the array through the `NEW` and `DELETE`
operations of that array type. The `MOVE_ONLY` modifier removes the copy
constructor and copy assignment, so an attempt to copy a `unique_ptr` is a
compilation error.

The owned pointer is a private field. An empty owner holds a null pointer.

## Member summary

**Construction, destruction, and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty owner, adopts a pointer, or transfers ownership. |
| [`DESTRUCTOR`](#destructor) | Deletes the owned object, if present. |
| [`OPERATOR:=`](#operator-assign) | Deletes the previous object and transfers ownership from another owner. |
| [`OPERATOR<->`](#operator-swap) | Exchanges ownership with another owner. |

**Observers**

| Member | Description |
| --- | --- |
| [`get`](#get) | Returns the owned pointer without transferring ownership. |
| [`has_value`](#has_value) | Reports whether the owner contains an object. |
| [`OPERATOR??`](#operator-has-value) | Reports whether the owner contains an object. |
| [`OPERATOR->`](#operator-dereference) | Returns a mutable reference to the owned object. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`release`](#release) | Relinquishes ownership without deleting the object. |
| [`reset`](#reset) | Deletes the owned object or replaces it with another object. |

**Nonmember functions**

| Function | Description |
| --- | --- |
| [`make_unique`](#make_unique) | Allocates a `T` and returns its owner. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                              // (1)
.CONSTRUCTOR FUNCTION(@pointer MUT->T) NOEXCEPT      // (2)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& unique_ptr#T)     // (3)
```

1. Creates an empty owner.
2. Adopts `pointer`. The caller relinquishes ownership of the object to the new
   owner.
3. Transfers ownership from `OTHER` and leaves `OTHER` empty.

**Parameters**: `pointer` is a pointer obtained from `NEW T`, or a null
pointer. `OTHER` is the owner whose object is transferred.

**Preconditions**: For (2), a non-null `pointer` refers to a live object that
was allocated by `NEW T`, and no other owner deletes that object.

**Complexity**: Constant.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor calls [`reset`](#reset), which deletes the owned object when the
owner is nonempty.

### `OPERATOR:=` { #operator-assign }

```quxlang
.OPERATOR:= FUNCTION(@OTHER TEMP& unique_ptr#T) MUT
```

The operator releases the pointer of `OTHER`, stores that pointer in the
receiver, and deletes the object that the receiver owned previously. `OTHER` is
left empty. Self-move assignment preserves ownership and deletes nothing.

Copy assignment from a `CONST&` or `MUT&` owner is not available.

### `OPERATOR<->` { #operator-swap }

```quxlang
.OPERATOR<-> FUNCTION(@OTHER MUT& unique_ptr#T) MUT
```

The operator exchanges the stored pointers of the receiver and `OTHER`. The
operation constructs no object and destroys no object.

**Complexity**: Constant.

### `get`

```quxlang
.get FUNCTION() CONST: MUT->T
```

**Return value**: The owned pointer, or a null pointer when the owner is empty.
The owner retains ownership. The returned pointer is valid until the owner
deletes or releases the object.

### `has_value`

```quxlang
.has_value FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the stored pointer is non-null, and `FALSE`
otherwise.

### `OPERATOR??` { #operator-has-value }

```quxlang
.OPERATOR?? FUNCTION() CONST: BOOL
```

**Return value**: The result of [`has_value`](#has_value). The expression
`owner??` therefore tests whether `owner` contains an object.

### `OPERATOR->` { #operator-dereference }

```quxlang
.OPERATOR-> FUNCTION() CONST: MUT& T
```

**Return value**: A mutable reference to the owned object. The receiver is
`CONST`, and the result is `MUT& T`: a constant owner provides mutable access
to the object that it owns. Member access through the owner is written
`owner->.member`.

**Preconditions**: The owner is nonempty. The operator performs no check;
dereferencing an empty owner dereferences a null pointer, which is invalid.

### `release`

```quxlang
.release FUNCTION() MUT: MUT->T
```

The function stores a null pointer in the owner and deletes nothing.

**Return value**: The pointer that the owner held before the call, or a null
pointer when the owner was empty. The caller becomes responsible for deleting
the object.

### `reset`

```quxlang
.reset FUNCTION() MUT                       // (1)
.reset FUNCTION(@pointer MUT->T) MUT        // (2)
```

1. Deletes the owned object, if present, and leaves the owner empty. The
   function is equivalent to `reset(@pointer NULL)`.
2. Stores `pointer` in the owner and then deletes the previously owned object
   with `DELETE OPTIONAL`, which skips a null pointer. When `pointer` equals the
   currently owned pointer, the function returns without any effect.

**Parameters**: `pointer` is a pointer obtained from `NEW T`, or a null
pointer.

**Preconditions**: A non-null `pointer` that differs from the owned pointer
refers to a live object that was allocated by `NEW T` and that no other owner
deletes.

## Nonmember functions

### `make_unique`

```quxlang
::make_unique TEMPLATE(@T TYPE AUTO) FUNCTION(%...values TT, @KWARGS ...): unique_ptr#T
```

The function allocates one `T` with `NEW` and forwards every positional
argument and every named argument to the constructor of `T`. The call
`std::make_unique#T()` default-constructs the object, and
`std::make_unique#T(@OTHER value)` constructs the object from `value`. A
`MOVE_ONLY` argument is forwarded without a copy.

**Return value**: A `unique_ptr#T` that owns the new object.

**Exceptions**: An exception thrown by the selected constructor of `T`
propagates to the caller.

## Example

The program transfers ownership between owners, replaces an owned object,
releases an object for manual deletion, and relies on the destructor for the
final object.

```quxlang
--8<-- "example-programs/reference-programs/unique_ptr_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/unique_ptr_example/expected-output.txt"
```

## See also

- [`std::shared_ptr`, `std::strong_ptr`, and `std::weak_ptr`](shared-ptr.md)
- [`std::optional`](optional.md)
- [Optional and Owning Values](../../reference/optional-and-owning-values.md)
- [Move Semantics](../../reference/move-semantics.md)
- [`NEW` and `DELETE`](../../reference/new-and-delete.md)
- [Swap Operator](../../reference/swap-operator.md)
