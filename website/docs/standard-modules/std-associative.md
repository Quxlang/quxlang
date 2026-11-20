# Sets

## `std::set`

**`insert`**

Inserts a key, consuming a temporary only when insertion succeeds.

Inserts a key after implicit conversion to T when the argument cannot bind directly.

??? note "Member signatures"

    ```quxlang
    .insert FUNCTION(@ARG:key AUTO& T) MUT: BOOL

    .insert FUNCTION(@ARG:key T) MUT PRIORITY(-1): BOOL
    ```
