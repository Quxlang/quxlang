# Sequences

## `std::geometric_array`

**`push_back`**

The push_back function appends a copy of value or moves value into the array. Existing
element addresses remain valid.

Appends a value implicitly converted to T. Existing element addresses remain valid.

??? note "Member signatures"

    ```quxlang
    .push_back FUNCTION(@ARG:value AUTO& T) MUT

    .push_back FUNCTION(@ARG:value T) MUT PRIORITY(-1)
    ```

## `std::list`

**`push_front`**

Copy-constructs value at the beginning of this list.

Move-constructs value at the beginning of this list.

??? note "Member signatures"

    ```quxlang
    .push_front FUNCTION(@ARG:value CONST& T) MUT

    .push_front FUNCTION(@ARG:value TEMP& T) MUT WEAK_PRIORITY(1)
    ```

**`push_back`**

Copy-constructs value at the end of this list.

Move-constructs value at the end of this list.

??? note "Member signatures"

    ```quxlang
    .push_back FUNCTION(@ARG:value CONST& T) MUT

    .push_back FUNCTION(@ARG:value TEMP& T) MUT WEAK_PRIORITY(1)
    ```
