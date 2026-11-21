# Shared configuration lifetime

The program shares one configuration object among several owners and reports the number of owners as
ownership is added and released. The program demonstrates
[`std::shared_ptr`](../standard-modules/std/shared-ptr.md#shared_ptr),
[`std::make_shared`](../standard-modules/std/shared-ptr.md#make_shared), and the member functions
`use_count` and `reset`.

```quxlang
--8<-- "example-programs/programs/shared_configuration_lifetime/main.qxs"
```

The program prints the following output.

```text
--8<-- "example-programs/programs/shared_configuration_lifetime/expected-output.txt"
```

## Explanation

`std::make_shared#configuration(@revision 3)` constructs a `configuration` object together with its
reference count, forwards the named argument to the constructor, and returns the first owner.
[`use_count`](../standard-modules/std/shared-ptr.md#shared_ptr-use_count) therefore returns 1 for
`loader`.

The declaration of `service` copies `loader`. The [copy
constructor](../standard-modules/std/shared-ptr.md#shared_ptr-constructor) shares the ownership of
the same object and increments the count. The function `describe` receives its parameter by value,
so the call creates a third owner for the duration of the call, and the count returns to 2 when the
parameter is destroyed. The [equality
operator](../standard-modules/std/shared-ptr.md#shared_ptr-equality) compares ownership identities
and returns `TRUE` for two pointers that own the same object.

[`reset`](../standard-modules/std/shared-ptr.md#shared_ptr-reset) releases the ownership of one
pointer and leaves that pointer empty. After `loader.reset()`, the object remains alive because
`service` still owns it. The call `service.reset()` releases the final ownership, which destroys the
configuration object. The destructor of `configuration` therefore prints its line before the line
`service released`.
