# Deduplicate station codes

This program counts distinct station codes and detects a repeated code.

```quxlang
LANGUAGE QUXLANG EN 0.0;

IMPORT std;

::main FUNCTION(): I32
{
  VAR stations std::set#I32;
  stations.insert(31);
  stations.insert(14);
  VAR inserted BOOL := stations.insert(31);
  std::printf#("{count} stations, repeated code added: {inserted}\n")(
    @count stations.size(), @inserted inserted);
  RETURN 0;
}
```

Using the [example bundle](index.md#example-bundle), `./out/output/app` prints the following output.

```text
2 stations, repeated code added: false
```

A set stores one element per key. `insert` reports whether a new element
was added.
