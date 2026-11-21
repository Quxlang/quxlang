# Overview of `LOOP` Statements

Quxlang has one loop statement. A `LOOP` statement consists of the keyword
`LOOP`, an optional label, a set of clauses, the keyword `DO`, and a body. The
clauses determine how the loop starts, when it continues, and how it advances.
The same statement therefore expresses condition-controlled loops, counted
loops, numeric sequences, and container iteration.

## Condition-controlled loops

A `WHILE` clause supplies a condition that is tested before the first iteration
and before every later iteration. The body does not execute when the condition
is initially false.

```quxlang
VAR index SZ := 0;
LOOP WHILE (index < count) DO
{
  process(index);
  index++;
}
```

`LOOP WHILE` is a `LOOP` statement whose only clause is `WHILE`. Further clauses
can be added to the same header. The combination of `INIT`, `WHILE`, and `STEP`
expresses a conventional counted loop:

```quxlang
VAR total I32 := 0;

LOOP INIT { VAR index I32 := 0; }
    WHILE(index < 5)
    STEP { index++; }
    DO
{
  total += index;
};

ASSERT(total == 10);
```

`INIT` runs once, `WHILE` runs before each iteration, and `STEP` runs after the
body. A header with no clauses, `LOOP DO`, is an unconditional loop that
repeats until a `BREAK`, `RETURN`, or thrown exception leaves it.

`POSTTEST` checks after the body, so the body runs at least once when no
`WHILE` clause is present:

```quxlang
VAR attempts I32 := 0;
LOOP POSTTEST(should_retry()) DO
{
  attempts++;
  perform_attempt();
};
```

## Numeric sequences

`VALUE`, `FROM`, and either `TO` or `UNTIL` describe a numeric sequence:

```quxlang
VAR inclusive_total I32 := 0;
LOOP VALUE(value I32) FROM(0) TO(6) BY(2) DO
{
  inclusive_total += value;
};
ASSERT(inclusive_total == 12);

VAR exclusive_total I32 := 0;
LOOP VALUE(value I32) FROM(0) UNTIL(4) DO
{
  exclusive_total += value;
};
ASSERT(exclusive_total == 6);
```

`TO` includes the bound; `UNTIL` excludes it. `BY` defaults to one. The type in
`VALUE(value I32)` determines the type of the sequence variable, bound, and
step. When the binding omits its type, the `FROM` expression must have a
concrete type, as in `VALUE(value) FROM(0 AS I32)`.

An index of type `SZ` can start from an untyped literal:

```quxlang
VAR count SZ := 4;
LOOP VALUE(index SZ) FROM(0) UNTIL(count) DO
{
  process_index(index);
};
```

A `FILTER` clause skips values without changing the sequence step:

```quxlang
LOOP VALUE(value I32) FROM(0) UNTIL(10)
    FILTER((value % 2) == 0)
    DO
{
  use_even_value(value);
};
```

## Arrays and containers

`ITEM(name) IN(range)` iterates direct elements. When the iterator returns a
reference, the binding can modify the element:

```quxlang
VAR values [4]I32 :[1, 2, 3, 4];
VAR total I32 := 0;

LOOP ITEM(item MUT& I32) IN(values) DO
{
  total += item;
  item++;
};

ASSERT(total == 10);
ASSERT(values[0] == 2);
```

`VALUE`, `INDEX`, `ITEM`, and `ITER` accept a type after the binding name.
For container elements, a reference type such as `MUT& I32` retains access to
the original element. A value type such as `I32` constructs a separate object
from that element. An omitted type preserves the iterator's result.

Container types participate through `.BEGIN()` and `.END()`. They may also
provide `.VALUES()`, `.INDEXES()`, and `.IV_PAIRS()` projections for
`VALUE`, `INDEX`, or combined `INDEX VALUE` loops.

An `ITER(name)` clause exposes the iterator to the loop body and to a custom
`STEP` clause:

```quxlang
LOOP ITER(iterator) ITEM(item) IN(container)
    FILTER(item != 0)
    STEP { iterator++; }
    DO
{
  consume(item);
};
```

The default iterator step is `iterator++`. `BY(amount)` uses
`iterator += amount` instead.

## Loop control

`BREAK` exits the loop. `CONTINUE` abandons the rest of the body and proceeds
through the normal continuation path of the loop: the `POSTTEST` clause, then
the `STEP` clause or the automatic sequence or iterator step.

```quxlang
VAR index SZ := 0;
LOOP WHILE (index < count) DO
{
  index++;
  IF (should_skip(index))
  {
    CONTINUE;
  }
  IF (finished(index))
  {
    BREAK;
  }
  process(index);
}
```

A loop that has neither a `STEP` clause nor an automatic step returns directly
to its `WHILE` condition after `CONTINUE`. The body of such a loop must update
the state that the condition depends on before `CONTINUE` executes.

A label after `LOOP` allows a nested loop to target an outer loop:

```quxlang
LOOP :rows VALUE(row I32) FROM(0) UNTIL(height) DO
{
  LOOP VALUE(column I32) FROM(0) UNTIL(width) DO
  {
    IF (skip_remaining_columns(row, column))
    {
      CONTINUE :rows;
    }
  };
};
```

## Reference

See the [`LOOP` Statements Reference](../reference/loop-statements.md) for clause-form
selection, condition conversion, exact phase ordering, iterator protocols,
binding scope, object lifetime, boundary comparisons, and every invalid clause
combination.
