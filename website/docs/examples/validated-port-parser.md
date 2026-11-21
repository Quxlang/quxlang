# Port number validation with exceptions

The program parses four configuration fields as port numbers, reports a syntax error for an
individual field, and abandons the configuration when a value is out of range. The program
demonstrates [exceptions](../overview/exceptions.md): `THROW` with structure values, typed `CATCH`
clauses, propagation to an enclosing handler, and destruction of local objects during propagation.

```quxlang
--8<-- "example-programs/programs/validated_port_parser/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/validated_port_parser/expected-output.txt"
```

## Explanation

`syntax_error` and `range_error` are ordinary structures. [A thrown
object](../reference/exceptions.md#thrown-objects) requires no particular base type. The function
`parse_port` fills the fields of a local error object and throws it with `THROW error;`. For each
digit, the function converts the `BYTE` to `U8`, subtracts the code of the character `0`, and
converts the difference to `I64`. The final conversion `AS CHECKED I32`
[checks](../reference/conversions.md#modes) that the value fits the result type.

The structure `field_scope` prints one line in its constructor and one line in its destructor. The
function `report_field` declares the object `scope` before its `TRY` statement, so the lifetime of
the object extends to the end of the function.

The `CATCH` clause in `report_field` binds a `CONST& syntax_error`. When `parse_port` throws a
`syntax_error` for the field `44x3`, the handler prints the offset of the invalid character,
execution continues after the `TRY` statement, and the function returns normally.

[Handler matching](../reference/exceptions.md#typed-matching) uses the type of the thrown object, so
the `range_error` for the field `70000` does not match the handler in `report_field` and propagates
to the caller. [Propagation destroys](../reference/exceptions.md#cleanup-and-noexcept) the live
local objects of every scope that it exits, and the destructor of `scope` prints `field 2: done`
before the handler in `main` executes. The loop in `main` is inside the `TRY` block of that handler,
so the remaining field `22` is not parsed.
