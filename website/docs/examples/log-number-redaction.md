# Number redaction in a log line

The program counts the decimal numbers in a log line, replaces each number with a placeholder, and
shows the rejection of an unsupported pattern. The program demonstrates
[`std::byte_regex`](../standard-modules/std/byte-regex.md) together with
[`std::dynarr`](../standard-modules/std/dynarr.md) byte sequences and an [exception
handler](../overview/exceptions.md).

```quxlang
--8<-- "example-programs/programs/log_number_redaction/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/log_number_redaction/expected-output.txt"
```

## Explanation

The member functions of `std::byte_regex` operate on `std::dynarr#BYTE` sequences. The helper
`bytes_of` copies a string constant into a [`std::string`](../standard-modules/std/string.md) and
appends each byte of that string to a new array. The helper `print_line` performs the opposite
conversion and writes the resulting string with
[`std::print`](../standard-modules/std/format.md#print).

The [constructor](../standard-modules/std/byte-regex.md#constructor) compiles a pattern that is
supplied as a `std::string` through the argument named `EXPLICIT`. In the
[pattern](../standard-modules/std/byte-regex.md#pattern-syntax) `[0-9]+`, the character class
matches one decimal digit and the quantifier `+` requires one or more repetitions.
[`count`](../standard-modules/std/byte-regex.md#count) returns the number of nonoverlapping matches.
Matching selects the leftmost longest match, so the digit sequence `4821` forms one match and the
result is 3. [`replace`](../standard-modules/std/byte-regex.md#replace) returns a new byte array in
which the literal replacement bytes take the place of each match.

`std::byte_regex` rejects grouping. The construction of `grouped` throws a value of the enumeration
`std::byte_regex::error` when the constructor encounters the opening parenthesis. The [`CATCH`
clause](../reference/exceptions.md#typed-matching) binds a constant reference to that value and
prints a message. The statement that follows the declaration inside the `TRY` block does not
execute.
