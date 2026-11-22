# `std::thread_safe_map`

Module `std` · source file `thread_safe_map.qxs`

`std::thread_safe_map#[INDEX:VALUE]` stores unique keys in a [`std::map`](map.md).
One mutex protects the contents. Each indexed read or indexed assignment holds
the mutex for the complete operation, including value construction or assignment.
A sequence of accesses such as `entries[key] := entries[key] + 1` acquires the
mutex separately for the read and the write.

```quxlang
VAR entries std::thread_safe_map#[I32:I32];
entries[7] := 21;
VAR previous I32 := entries[7];
entries[7] := 42;
```

The map starts empty. Its `ROOTED` declaration fixes its address for its
lifetime. All accesses must finish before destruction begins. Key comparisons
and value operations performed under the mutex must avoid reentering the same
map.

| Member | Behavior |
| --- | --- |
| `.OPERATOR[] FUNCTION(%key CONST& INDEX) MUT: VALUE` | Copies the mapped value while holding the mutex. A missing key throws `std::container_error::missing_key`. |
| `.OPERATOR[]:= FUNCTION(%key INDEX, @OTHER VALUE) MUT` | Inserts a missing key with its supplied value or assigns the supplied value to the existing entry. |

Both operations take logarithmic time in the number of entries, excluding lock
contention and the cost of key and value operations. Reads require a copyable
value. Writes use the construction and assignment operations required by
`std::map::insert_or_assign`. An exception releases the mutex during unwinding.
