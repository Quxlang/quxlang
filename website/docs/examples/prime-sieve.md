# Prime sieve with clause-based loops

The program computes the prime numbers below 60 with the sieve of Eratosthenes, determines the
largest of them, and expresses 56 as the sum of two primes. The program demonstrates the forms of
the [`LOOP` statement](../overview/loop-statements.md): numeric sequences, an index projection with
a filter, a `WHILE` loop, and loop labels.

```quxlang
--8<-- "example-programs/programs/prime_sieve/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/prime_sieve/expected-output.txt"
```

## Explanation

The namespace-scope [`STATIC` constant](../reference/static-compile-time-constants.md) `limit` is a
compile-time value and serves as the element count of the array type `[limit]BOOL`. The array is
[default-constructed](../reference/arrays.md#default-construction), which leaves every element
`FALSE`.

The first loop is a [numeric sequence loop](../reference/loop-statements.md#numeric-sequence-loops).
The explicit type in `VALUE(candidate SZ)` determines the sequence type, so `FROM(2)` accepts an
untyped literal, and `UNTIL(limit)` is an exclusive bound. `BREAK` ends the loop when the square of
the candidate reaches the limit, and `CONTINUE` advances to the next candidate when the current
candidate is composite. The inner loop starts at the square of the candidate, and its clause
`BY(candidate)` supplies the increment.

The loop that prints the primes is an [iterator
loop](../reference/loop-statements.md#iterator-loops). The clause `INDEX(number)` binds each
zero-based index of the array, and the clause `FILTER` skips the body for every index whose element
is `TRUE`. The subsequent `LOOP WHILE` statement is a [conventional
loop](../reference/loop-statements.md#conventional-loops) that tests its condition before every
iteration.

The final loop carries the [label](../reference/loop-statements.md#break-continue-labels-and-scope)
`search`. The statement `CONTINUE :search;` names the labeled loop, and the statement `BREAK
:search;` inside the inner loop exits both loops when the first pair of primes with the required sum
is found. In the condition of the inner `IF` statement, `+` binds more tightly than `==`, and `==`
binds more tightly than `&&`, as the [operator precedence
table](../reference/operator-precedence.md) specifies.
