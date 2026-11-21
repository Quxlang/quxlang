# `std::byte_regex`

Module `std` · source file `byte_regex.qxs`

`std::byte_regex` is a compiled regular expression that operates on bytes. The
constructor translates a pattern into a sparse nondeterministic automaton, and
the member functions count or replace the nonoverlapping matches in a
[`std::dynarr#BYTE`](dynarr.md). Every search selects the leftmost match and,
among the matches that start at that position, the longest match. The automaton
uses storage proportional to the size of the pattern, and matching uses no
recursion. The supported syntax is a deliberately small subset: grouping,
anchors, and counted repetition are rejected. Text is not decoded; a multibyte
character encoding is matched as its individual bytes.

## Declaration

```quxlang
::byte_regex STRUCT
{
  ::error ENUM [invalid_pattern];
}
```

`byte_regex` carries no struct modifier and declares no copy constructor, move
constructor, assignment operator, or destructor; the compiler-generated
operations apply to the private program storage. The compiled program is
immutable after construction: `count` and `replace` are `CONST` member
functions that keep their working state in local objects.

### `byte_regex::error`

| Enumerator | Meaning |
| --- | --- |
| `invalid_pattern` | The pattern is malformed or uses an unsupported construct. |

The constructor throws a value of this type. A handler catches the value with
`CATCH error CONST& std::byte_regex::error`.

### Target restrictions

The type is declared for every target. The tests of the type are marked as
known to be broken on JVM targets (`KNOWN_BROKEN_IF(ARCH_IS_JVM)`).

## Member summary

| Member | Description |
| --- | --- |
| [`CONSTRUCTOR`](#constructor) | Compiles a pattern. |
| [`count`](#count) | Returns the number of nonoverlapping matches in an input. |
| [`replace`](#replace) | Returns a copy of an input in which every nonoverlapping match is replaced by literal bytes. |

## Pattern syntax

A pattern is a sequence of alternatives separated by `|`. Each alternative is
a sequence of elements, and each element matches exactly one byte unless a
quantifier modifies the element. An alternative may be empty; an empty
alternative, and therefore also the empty pattern, matches the empty byte
sequence.

| Element | Matches |
| --- | --- |
| Literal byte | That byte. Every byte is literal except the bytes with a special meaning listed in this section. |
| `\` followed by a byte | `\n` matches a linefeed (10), `\r` matches a carriage return (13), and `\t` matches a horizontal tab (9). Any other byte after `\` matches itself, which quotes a byte that otherwise has a special meaning. |
| `.` | Any byte except a linefeed. |
| `[...]` | Any byte that belongs to the character class. |
| `[^...]` | Any byte that does not belong to the character class. A negated class matches a linefeed unless the class lists the linefeed. |

A character class contains one or more items. An item is a single byte or a
range `first-last`, which includes every byte value from `first` through
`last`. An item byte may be written with a backslash escape. A `-` that
directly precedes the closing `]` is a literal byte, and a `^` that is not the
first byte of the class is a literal byte. The first unescaped `]` closes the
class.

| Quantifier | Meaning |
| --- | --- |
| `*` | The preceding element occurs zero or more times. |
| `+` | The preceding element occurs one or more times. |
| `?` | The preceding element occurs zero times or one time. |

A quantifier applies to the single element that precedes it. An element
accepts at most one quantifier.

The following conditions make a pattern invalid:

- An unescaped `(`, `)`, `{`, `}`, `^`, or `$` appears outside a character
  class.
- A quantifier appears at the beginning of the pattern, directly after `|`, or
  directly after another quantifier.
- A character class is empty, is not closed by `]`, or contains a range whose
  last byte value is less than its first byte value.
- The pattern ends with a single `\`.

An unescaped `]` outside a character class is a literal byte.

## Matching rules

A search starts at an offset and reports one match as a half-open byte
interval. Among all matches at or after the offset, the search selects the
match with the smallest start position; among the matches with that start
position, the search selects the match with the greatest length. The order of
the alternatives in the pattern does not affect the result: the pattern
`ab|abc` matches `abc` in the input `zabc`.

`count` and `replace` search repeatedly. After a nonempty match, the next
search starts at the end of that match. After an empty match, the next search
starts one byte later, so a pattern that matches the empty sequence produces
one match at each such position, including one possible match at the end of
the input. The pattern `a*` therefore matches the input `aaab` three times:
`aaa`, the empty sequence before `b`, and the empty sequence at the end.

## Member functions

### `CONSTRUCTOR`

```quxlang
.CONSTRUCTOR FUNCTION(@EXPLICIT:pattern CONST& string)
```

The constructor compiles `pattern`. The argument is named `EXPLICIT`, so the
construction is written with that name:
`VAR expression std::byte_regex :(@EXPLICIT pattern);`.

**Parameters**: `pattern` is a [`std::string`](string.md) that contains the
pattern bytes. The constructor does not retain a reference to `pattern`.

**Exceptions**: An invalid pattern throws `byte_regex::error::invalid_pattern`.

**Complexity**: Linear in the size of the pattern. The compiled program
contains one state for each pattern element and one accepting state for each
alternative.

### `count`

```quxlang
.count FUNCTION(@ARG:input CONST& dynarr#BYTE) CONST: SZ
```

**Parameters**: `input` is the byte sequence to search.

**Return value**: The number of nonoverlapping matches that the
[matching rules](#matching-rules) produce.

**Complexity**: The function allocates working storage proportional to the
number of program states. Each search examines the input bytes from its
start offset until no active state can extend or precede the selected match,
or until the end of the input. The work for each input byte is proportional to
the number of active states, which does not exceed the number of program
states.

### `replace`

```quxlang
.replace FUNCTION(@input CONST& dynarr#BYTE, @replacement STRING_CONSTANT) CONST: dynarr#BYTE
```

**Parameters**: `input` is the byte sequence to search. `replacement` is a
string constant whose bytes replace each match. The replacement is literal
text; the function interprets no escape sequence and no reference to the
matched bytes. An empty replacement deletes every match.

**Return value**: A new `dynarr#BYTE` that contains the bytes of `input`
outside the matches, in order, with the bytes of `replacement` in place of
each match. `input` is unchanged.

**Complexity**: The search cost of [`count`](#count) applies. The function
reserves `input.size()` bytes for the result before the first search.

## Example

```quxlang
--8<-- "example-programs/reference-programs/byte_regex_example/main.qxs"
```

Output:

```text
--8<-- "example-programs/reference-programs/byte_regex_example/expected-output.txt"
```

## See also

- [`std::dynarr`](dynarr.md)
- [`std::string`](string.md)
- [`std::buffered_writer` and standard streams](buffered-io.md)
- [Exception Handling](../../reference/exceptions.md)
- [Stringlike Types](../../reference/stringlike-types.md)
