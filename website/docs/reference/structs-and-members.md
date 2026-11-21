# Structures

A `STRUCT` is a value type with fields, member functions, and nested
declarations:

```quxlang
::point STRUCT
{
  .x VAR I32;
  .y VAR I32;

  .CONSTRUCTOR FUNCTION(@x I32, @y I32)
  {
    .x := x;
    .y := y;
  }

  .sum FUNCTION() CONST: I32
  {
    RETURN .x + .y;
  }

  ::origin FUNCTION(): point
  {
    RETURN point(@x 0, @y 0);
  }
}
```

- `.x`, `.y`, and `.sum` are instance members.
- `.x` in a member body is shorthand for the member of `THIS`.
- `::origin` is nested in `point` but receives no object.

## Public field reflection

[Public Field Reflection](public-field-reflection.md) provides field counts,
names, declared types, and access by compile-time name or index through
`PUBLIC_FIELD_*`. Only directly declared public instance fields are included,
in declaration order. Private and inherited fields are excluded.

## Receiver qualifiers

A member function may place `MUT`, `CONST`, `WRITE`, or `TEMP` after its
parameter list:

```quxlang
.read FUNCTION() CONST: I32
{
  RETURN .x;
}
```

The suffix reuses the ordinary parameter model. `FUNCTION() CONST` is
semantically the same receiver form as an explicit
`@THIS CONST& THISTYPE` parameter, while keeping the common member spelling
compact. `THIS` names the current object and `THISTYPE` names its formal owner
type.

## Struct modifiers

Modifiers appear between `STRUCT` and its body. The current set includes
`MOVE_ONLY`, `NOT_COPYABLE`, `NO_IMPLICIT_DEFAULT_CONSTRUCTOR`,
`NO_IMPLICIT_CONSTRUCTORS`, `NO_IMPLICIT_ASSIGNMENT`, `NO_IMPLICIT_COPY`,
`ROOTED`, `ANTESTATAL`, `SERIALOID`, `NONSTATIC`, `STRINGLIKE`, `POLYMORPHIC`,
`VIRTUAL_POLYMORPHIC`, and `FINAL`.

```quxlang
::unique_owner STRUCT MOVE_ONLY
{
  .allocation VAR MUT->BYTE;
}
```

The modifiers constrain generated special functions or declare specialized
static/serialization contracts. See
[Constructors and destructors](constructors-and-destructors.md),
[Inheritance](inheritance.md),
[`STATIC` Compile-Time Constants](static-compile-time-constants.md),
and [Serialization](serialization.md).

### `ROOTED`

A `ROOTED` structure has a fixed object address for its entire lifetime. The
modifier suppresses the generated copy constructor, move constructor,
assignment operator, and swap operator.

```quxlang
::rooted_value STRUCT ROOTED
{
  .value VAR I32;

  .CONSTRUCTOR FUNCTION(@value I32)
  {
    .value := value;
  }
}

VAR object rooted_value :(@value 7);
```

The following constraints apply:

- A `ROOTED` structure cannot declare a copy constructor, a move constructor,
  or a swap operator, including the `OPERATOR<->RHS` form.
- `ROOTED` and `MOVE_ONLY` cannot be combined.
- Copy construction, move construction, assignment, swap, by-value argument
  passing, and the return of a named local object are ill-formed for a
  `ROOTED` type.
- A structure that contains a `ROOTED` field has no generated move constructor
  or swap operator.
- A template structure preserves the modifier in every instantiation.

A `ROOTED` object is constructed in place and accessed through references or
pointers. The modifier suits objects whose address is registered elsewhere,
such as synchronization primitives.

## `IBC_STRUCT`

An ordinary `STRUCT` may reorder fields to optimize layout. `IBC_STRUCT`
preserves C-compatible field order and padding for external layouts:

```quxlang
::wire_pair IBC_STRUCT
{
  .left VAR U32;
  .right VAR U32;
}
```

`IBC_STRUCT` provides the C-compatible layout required at a binary boundary.
Internal layout preferences do not require `IBC_STRUCT`.

[Composites](composites.md) provide anonymous structural record types with
inferred fields and ordinary struct lifetime semantics. Their field names and
types participate in structural identity and static reflection.

## Relocation permission

`TRIVIALLY_RELOCATABLE` and `TRIVIALLY_RELOCATABLE_IF(condition)` authorize
compiler-owned lifetime transfer. See [Trivial Relocation](move-semantics.md#trivial-relocation)
for the invariant and destruction contract.
