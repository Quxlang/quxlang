# `std::optional`

Module `std` · source file `optional.qxs`

`std::optional` stores either no value or one value of type `T`. The value
occupies typed storage inside the optional object, so an optional performs no
heap allocation. An optional that contains a value is *engaged*; an optional
that contains no value is *empty*. Moving an optional moves the contained value
and leaves the source engaged with its moved-from value.
[`std::unique_ptr`](unique-ptr.md) represents an optional object that is
allocated separately and owned exclusively.

## Declaration

```quxlang
::optional TEMPLATE(@T TYPE AUTO) STRUCT
```

`T` is the type of the contained value. The declaration carries no struct
modifier. The private state consists of one `TYPED_STORAGE(T)` field and one
`BOOL` engagement flag. Each member function instantiates only the operations
of `T` that the member function uses: the copy constructor and copy assignment
require a copyable `T`, and an optional of a `MOVE_ONLY` type such as
`std::unique_ptr#I32` supports move construction, move assignment, swap, and
`emplace`.

`T` may be a fixed-size array type or a pointer type. `NULL` always denotes
the empty optional; an optional of `MUT->I32` that is constructed with `@value`
from a null pointer variable is engaged and contains a null pointer.

A requirement that the source states with `ASSERT` is evaluated only when the
`ASSERT_ENABLED` policy is enabled. A violated requirement that is not
evaluated results in undefined behavior.
[Compilation Policies](../../reference/compilation-policies.md) specifies the
policy defaults.

## Member summary

**Construction, destruction, and assignment**

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Creates an empty optional, an engaged optional, or a copy of another optional. |
| [`DESTRUCTOR`](#destructor) | Destroys the contained value, if present. |
| [`OPERATOR:=`](#operator-assign) | Copies or moves the state of another optional, or empties the optional. |
| [`OPERATOR<->`](#operator-swap) | Exchanges values and engagement with another optional. |

**Observers**

| Member | Description |
| --- | --- |
| [`has_value`](#has_value) | Reports whether the optional is engaged. |
| [`OPERATOR??`](#operator-has-value) | Reports whether the optional is engaged. |
| [`value`](#value) | Returns a reference to the contained value. |
| [`OPERATOR->`](#operator-dereference) | Returns a reference to the contained value. |

**Modifiers**

| Member | Description |
| --- | --- |
| [`reset`](#reset) | Destroys the contained value and leaves the optional empty. |
| [`emplace`](#emplace) | Replaces the contained value with a newly constructed value. |

**Nonmember functions**

| Function | Description |
| --- | --- |
| [`make_optional_value`](#make_optional_value) | Creates an engaged optional from one value. |

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION()                               // (1)
.CONSTRUCTOR FUNCTION(@value AUTO& AUTO)              // (2)
.CONSTRUCTOR FUNCTION(@OTHER NULL_TYPE)               // (3)
.CONSTRUCTOR FUNCTION(@OTHER AUTO& T)                 // (4)
.CONSTRUCTOR FUNCTION(@OTHER CONST& optional#T)       // (5)
.CONSTRUCTOR FUNCTION(@OTHER TEMP& optional#T)        // (6)
```

1. Creates an empty optional. No `T` is constructed.
2. Creates an engaged optional. The constructor forwards `value` to
   [`emplace`](#emplace) as the `OTHER` argument, so the contained value is
   constructed as `T :(@OTHER value)`. The argument may have any type from
   which `T` is constructible in that form. The named argument `@value`
   selects this overload: `VAR option std::optional#I32 :(@value 5);`.
3. Creates an empty optional from `NULL`. This overload permits
   `VAR option std::optional#I32 := NULL;` and `RETURN NULL;` in a function
   that returns an optional.
4. Creates an engaged optional from a `T`. The reference qualification of
   `OTHER` is preserved: a `CONST&` or `MUT&` argument is copied, and a `TEMP&`
   argument is moved. This overload provides the implicit conversion from `T`
   to `optional#T` in initialization, argument passing, and `RETURN`
   statements.
5. Copies `OTHER`. The new optional is engaged exactly when `OTHER` is engaged,
   and the contained value is copy-constructed from the value of `OTHER`.
6. Moves `OTHER`. The new optional is engaged exactly when `OTHER` is engaged,
   and the contained value is move-constructed from the value of `OTHER`.
   `OTHER` remains engaged and contains the moved-from value.

**Exceptions**: An exception thrown by the selected constructor of `T`
propagates to the caller.

### `DESTRUCTOR`

```quxlang
.DESTRUCTOR FUNCTION()
```

The destructor calls [`reset`](#reset), which destroys the contained value when
the optional is engaged. An empty optional destroys no `T`.

### `OPERATOR:=` { #operator-assign }

```quxlang
.OPERATOR:= FUNCTION(@OTHER CONST& optional#T) MUT    // (1)
.OPERATOR:= FUNCTION(@OTHER NULL_TYPE) MUT            // (2)
.OPERATOR:= FUNCTION(@OTHER TEMP& optional#T) MUT     // (3)
```

1. Copies the state of `OTHER`. When `OTHER` is engaged and the receiver is
   engaged, the operator copy-assigns the contained value. When `OTHER` is
   engaged and the receiver is empty, the operator copy-constructs a new
   contained value. When `OTHER` is empty, the operator calls
   [`reset`](#reset).
2. Destroys the contained value, if present, and leaves the optional empty.
3. Moves the state of `OTHER`. The cases correspond to (1), with move
   assignment and move construction of the contained value. `OTHER` retains
   its engagement; an engaged `OTHER` contains the moved-from value afterward.

Overloads (1) and (3) compare the addresses of the receiver and `OTHER` and
return without any effect on self-assignment.

### `OPERATOR<->` { #operator-swap }

```quxlang
.OPERATOR<-> FUNCTION(@OTHER MUT& optional#T) MUT
```

The operator exchanges the values and the engagement of the receiver and
`OTHER`. When both optionals are engaged, the operator applies `<->` to the two
contained values. When exactly one optional is engaged, the operator
move-constructs the value into the empty optional and then resets the
previously engaged optional. When both optionals are empty, or when `OTHER` is
the receiver, the operator has no effect.

### `has_value`

```quxlang
.has_value FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the optional is engaged, and `FALSE` otherwise.

### `OPERATOR??` { #operator-has-value }

```quxlang
.OPERATOR?? FUNCTION() CONST: BOOL
```

**Return value**: `TRUE` when the optional is engaged, and `FALSE` otherwise.
The expression `option??` is equivalent to `option.has_value()`.

### `value`

```quxlang
.value FUNCTION(@THIS AUTO& THISTYPE): AUTO& T
```

**Return value**: A reference to the contained value. The reference carries
the access qualification of the receiver: a mutable optional yields `MUT& T`,
and a `CONST&` optional yields `CONST& T`. The reference remains valid until
the contained value is destroyed.

**Checked failures**: The function asserts that the optional is engaged, with
the message `Cannot access an empty optional.`

### `OPERATOR->` { #operator-dereference }

```quxlang
.OPERATOR-> FUNCTION(@THIS AUTO& THISTYPE): AUTO& T
```

**Return value**: The result of [`value`](#value). Member access through the
optional is written `option->.member`.

**Checked failures**: The requirements of [`value`](#value) apply.

### `reset`

```quxlang
.reset FUNCTION() MUT
```

The function destroys the contained value when the optional is engaged, and
leaves the optional empty. The engagement flag is cleared before the destructor
of `T` runs. A call on an empty optional has no effect.

### `emplace`

```quxlang
.emplace FUNCTION(%...values TT, @KWARGS ...) MUT: MUT& T
```

The function first calls [`reset`](#reset), and then constructs a new `T` in
the storage of the optional by forwarding every positional argument and every
named argument to the constructor of `T`. Positional arguments and named
arguments may be mixed in one call, for example
`option.emplace(% [1, 2], @offset 3)`. A call without arguments
default-constructs the value. `option.emplace(@OTHER source)` copy-constructs
or move-constructs the value from `source`.

**Return value**: A mutable reference to the new contained value.

**Exceptions**: An exception thrown by the constructor of `T` propagates to the
caller. The previous value is already destroyed at that point, and the optional
remains empty.

## Nonmember functions

### `make_optional_value`

```quxlang
::make_optional_value TEMPLATE(@T TYPE AUTO) FUNCTION(@ARG:value AUTO& AUTO): optional#T
```

The function forwards `value` to constructor overload (2), which constructs the
contained `T` from `value`.

**Return value**: An engaged `optional#T`.

## Example

The program returns an optional index from a search function, tests
engagement, constructs a value in place, and empties an optional by assigning
`NULL`.

```quxlang
--8<-- "example-programs/reference-programs/optional_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/optional_example/expected-output.txt"
```

## See also

- [`std::unique_ptr`](unique-ptr.md)
- [`std::shared_ptr`, `std::strong_ptr`, and `std::weak_ptr`](shared-ptr.md)
- [Optional and Owning Values](../../reference/optional-and-owning-values.md)
- [Object Storage and Lifetime](../../reference/typed-storage-and-lifetime.md)
- [Move Semantics](../../reference/move-semantics.md)
- [Failure Statements](../../reference/diagnostics-and-failure.md)
