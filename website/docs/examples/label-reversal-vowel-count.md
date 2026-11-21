# Label reversal and vowel count

The program concatenates two labels, counts the vowels of the result, reverses two strings, and
tests each string for equality with its reversal. The program demonstrates the construction,
concatenation, indexing, growth, and comparison operations of
[`std::string`](../standard-modules/std/string.md).

```quxlang
--8<-- "example-programs/programs/label_reversal_vowel_count/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/label_reversal_vowel_count/expected-output.txt"
```

## Explanation

The declaration `VAR label std::string := "level";` converts a string literal through the
[constructor](../standard-modules/std/string.md#constructor) that accepts a `STRING_CONSTANT`.
[`OPERATOR+`](../standard-modules/std/string.md#operator-plus) returns a new string that contains
the bytes of both operands and modifies neither operand.
[`push_back`](../standard-modules/std/string.md#push_back) appends one `BYTE`, which the character
literal `'!'` supplies.

`std::string` declares no iteration members, so the program reads bytes by index. The [numeric
sequence loop](../reference/loop-statements.md#numeric-sequence-loops) binds each index from zero up
to, and excluding, the value that [`size`](../standard-modules/std/string.md#size) returns. The
[index operator](../standard-modules/std/string.md#operator-index) returns the byte by value, and
`is_vowel` compares that byte with five character literals.

The function `reversed_copy` receives a `CONST& std::string` and returns a new string.
[`reserve`](../standard-modules/std/string.md#reserve) allocates storage for the complete result
before the loop, so the subsequent `push_back` calls perform no reallocation. The loop reads the
source from its final byte toward its first byte.

[`OPERATOR==`](../standard-modules/std/string.md#operator-equal) returns `TRUE` when both strings
have the same size and the same byte at every index. The label `level` equals its reversal, and the
extended label does not.
