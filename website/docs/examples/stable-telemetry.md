# Keep a reference while telemetry grows

A geometric array retains the address of its first reading while later readings are appended.

```quxlang
LANGUAGE QUXLANG EN 0.0;

IMPORT std;

::main FUNCTION(): I32
{
  VAR readings std::geometric_array#I32;
  readings.push_back(21);
  VAR first MUT->I32 := readings[0]<-;
  LOOP VALUE(index I32) FROM(0) UNTIL(16) DO
  {
    readings.push_back(index);
  }
  first-> := 22;
  std::printf#("first {first}, readings {count}\n")(
    @first readings[0], @count readings.size());
  RETURN 0;
}
```

Using the [example bundle](index.md#example-bundle), `./out/output/app` prints the following output.

```text
first 22, readings 17
```

Growth allocates additional segments. The reference stays usable because the
first element stays alive in its existing segment.
