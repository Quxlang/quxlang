# Dispatch jobs in priority order

A list allows a dispatcher to add regular jobs at the back and an urgent job at the front.

```quxlang
LANGUAGE QUXLANG EN 0.0;

IMPORT std;

::main FUNCTION(): I32
{
  VAR jobs std::list#I32;
  jobs.push_back(100);
  jobs.push_back(101);
  jobs.push_front(99);
  LOOP WHILE (jobs.empty() == FALSE) DO
  {
    VAR job I32 := jobs.front();
    jobs.pop_front();
    std::printf#("dispatch {job}\n")(@job job);
  }
  RETURN 0;
}
```

Using the [example bundle](index.md#example-bundle), `./out/output/app` prints the following output.

```text
dispatch 99
dispatch 100
dispatch 101
```

The loop copies the front value before popping its node. Removing the node
ends the lifetime of the element it contained.
