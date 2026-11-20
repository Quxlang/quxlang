# Compute a large byte-rate quotient

This program computes a byte rate whose total and quotient both exceed the
signed 64-bit integer range.

```quxlang
LANGUAGE QUXLANG EN 0.0;

IMPORT std;

::main FUNCTION(): I32
{
  VAR bytes std::big_integer := 70000000000000000000;
  VAR seconds std::big_integer := 3;
  TRY
  {
    VAR rate std::big_integer := bytes.quotient(seconds);
    std::printf#("{rate} bytes/s\n")(@rate rate.to_string());
  }
  CATCH error CONST& std::big_integer_error
  {
    std::print("rate unavailable\n");
    RETURN 1;
  }
  RETURN 0;
}
```

Using the [example bundle](index.md#example-bundle), `./out/output/app` prints the following output.

```text
23333333333333333333 bytes/s
```

`quotient` rounds toward negative infinity and returns a `std::big_integer`.
`to_string()` formats every digit of the result. The catch handles a zero
divisor.
