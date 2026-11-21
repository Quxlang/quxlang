# Known Issues

This page lists the tests of the test bundle that are annotated as not
passing, and the other defects and limitations that the repository content
confirms. The test bundle is located at `quxlang/tests/testdata/testbundle`,
and every source path on this page is relative to its `modules` directory.

## Test annotations

A test declaration accepts execution tags after the test kind and the optional
expectation modifier, and before the body.

```quxlang
::wide_rotation_cycles DUAL_TEST KNOWN_BROKEN_IF(ARCH_IS_X86)
{
  cycles#(U64)();
}
```

| Tag | Condition | Effect when the condition holds |
| --- | --- | --- |
| `KNOWN_BROKEN` | Always | The test body is not compiled and not executed. |
| `KNOWN_BROKEN_IF(condition)` | The compile-time Boolean `condition` is `TRUE` for the target | Same effect as `KNOWN_BROKEN` |
| `KNOWN_FAILING` | Always | The test body is compiled and not executed. |
| `KNOWN_FAILING_IF(condition)` | The compile-time Boolean `condition` is `TRUE` for the target | Same effect as `KNOWN_FAILING` |

The tags have the following additional properties.

- A declaration accepts each of the two tag families at most once. A repeated
  tag is a syntax error.
- When both a known-broken condition and a known-failing condition hold, the
  test is known broken.
- A known-broken test places no requirement on its body. The annotation is
  therefore applicable to a body that does not compile on the target.
- A known-failing test requires its body and the dependencies of the body to
  compile. A compilation failure of a known-failing test is a test failure.
  The expectation modifiers `EXPECT_FAIL` and `EXPECT_COMPILATION_FAILURE` are
  not applied to a known-failing test.
- During static testing, the compiler counts the tests of each category. The
  `qxc` summary line has the form
  `<n> static tests passed, <n> known broken, <n> known failing`.
- A unit-test suite retains the name of a known-broken test and prints
  `KNOWN BROKEN: <name>` in place of executing it. A known-failing test is
  compiled into the suite. The suite prints `KNOWN FAILING: <name>` and skips
  the test unless the suite is started with `--include-failing-tests`.
- The `--list-tests` option of a unit-test suite marks the tests with
  `[KNOWN_BROKEN]` and `[KNOWN_FAILING]`.

An `INCLUDE_IF` condition differs from these tags. A test that `INCLUDE_IF`
excludes does not exist on the target and is not counted.

## Summary of annotated tests

The test bundle contains 322 annotated test declarations. The bundle
contains no unconditional `KNOWN_BROKEN` tag and no `KNOWN_FAILING_IF` tag.

| Annotation | Declarations | Targets on which the condition holds |
| --- | --- | --- |
| `KNOWN_FAILING` | 1 | Every target |
| `KNOWN_BROKEN_IF(ARCH_IS_X86)` | 1 | `linux-x86` |
| `KNOWN_BROKEN_IF(ARCH_IS_X86 || ARCH_IS_JVM)` | 1 | `linux-x86`; JVM targets |
| `KNOWN_BROKEN_IF(ARCH_IS_JVM)` | 300 | JVM targets |
| `KNOWN_BROKEN_IF(…)` with a condition of the form `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | 18 | Targets without an operating-system predicate, which are the JVM targets |
| `KNOWN_BROKEN_IF(BACKEND_LLVM == FALSE)` | 1 | Targets of the Cortado backend |

The consequences for the targets of the test bundle are as follows.

- On every configured target, one static test is known failing.
- On `linux-x86`, two tests are known broken.
- On every other configured target, no test is known broken.
- The remaining annotations take effect only on a target of the experimental
  Cortado backend, and the test bundle configures no such target.

## Tests annotated `KNOWN_FAILING`

| Test | Kind | Source file | Annotation | Coverage |
| --- | --- | --- | --- | --- |
| `test_selection_tests::option_boundaries_constexpr` | `STATIC_TEST` | `runtime/sources/test_selection_tests.qxs` | `KNOWN_FAILING` | Retains the compilable constant-text matcher regression that currently fails in constexpr interpretation. |

The test repeats the body of the passing unit test
`test_selection_tests::option_boundaries` as a static test. The body compiles;
its evaluation by the constant evaluator fails.

## Tests broken on x86

| Test | Kind | Source file | Annotation | Coverage |
| --- | --- | --- | --- | --- |
| `atomic_value_tests::read_modify_write_wide_widths` | `UNIT_TEST` | `tests/sources/main_test_61_atomic_values.qxs` | `KNOWN_BROKEN_IF(ARCH_IS_X86 || ARCH_IS_JVM)` | Atomic operations preserve return and stored values for signed and unsigned 64-bit operands. |
| `rotation_count_tests::wide_rotation_cycles` | `DUAL_TEST` | `tests/sources/main_test_70_rotation_counts.qxs` | `KNOWN_BROKEN_IF(ARCH_IS_X86)` | Rotations cover two complete 64-bit cycles using a division-based arithmetic oracle. |

Both tests exercise 64-bit operands on the 32-bit x86 target. The equivalent
tests for operand widths of 32 bits and less carry no annotation.

## Tests annotated for the Cortado backend

320 declarations in 90 source files carry a condition that is
`TRUE` on a target of the Cortado backend. The JVM target was removed from the
configuration of the test bundle on 2026-10-26, so the annotations record the
state of the backend at that time and are not revalidated. Tests added after
that date can fail on the backend without an annotation.

A `DUAL_TEST` with this annotation is disabled in both the static and the
runtime mode on a JVM target.

### Annotated declarations by source file

| Source file | Annotated declarations | `DUAL_TEST` | `UNIT_TEST` | `STATIC_TEST` |
| --- | --- | --- | --- | --- |
| `tests/sources/main_test_31_inheritance.qxs` | 31 | 29 | 0 | 2 |
| `tests/sources/main_test_37_exceptions.qxs` | 22 | 22 | 0 | 0 |
| `tests/sources/main_test_71_checked_arithmetic.qxs` | 15 | 15 | 0 | 0 |
| `tests/sources/main_test_25_concurrency.qxs` | 14 | 0 | 10 | 4 |
| `tests/sources/main_test_28_containers.qxs` | 13 | 1 | 12 | 0 |
| `tests/sources/main_test_hadix_containers.qxs` | 11 | 2 | 9 | 0 |
| `tests/sources/main_test_69_checked_conversions.qxs` | 10 | 10 | 0 | 0 |
| `tests/sources/main_test_214_virtual_generated_members.qxs` | 9 | 8 | 0 | 1 |
| `tests/sources/main_test_22_threads.qxs` | 9 | 0 | 9 | 0 |
| `tests/sources/main_test_benchmark_runtime.qxs` | 9 | 5 | 4 | 0 |
| `tests/sources/main_test_212_boolean_nested_exceptions.qxs` | 8 | 8 | 0 | 0 |
| `tests/sources/main_test_61_atomic_values.qxs` | 7 | 2 | 5 | 0 |
| `tests/sources/main_test_74_union_lifetimes.qxs` | 6 | 6 | 0 | 0 |
| `tests/sources/main_test_52_array_lifetimes.qxs` | 5 | 5 | 0 | 0 |
| `tests/sources/main_test_57_struct_lifetimes.qxs` | 5 | 5 | 0 | 0 |
| `tests/sources/main_test_58_constructor_delegates.qxs` | 5 | 5 | 0 | 0 |
| `tests/sources/main_test_110_wide_flagsets.qxs` | 4 | 4 | 0 | 0 |
| `tests/sources/main_test_142_final_virtual_methods.qxs` | 4 | 2 | 0 | 2 |
| `tests/sources/main_test_165_return_unequal_temporaries.qxs` | 4 | 4 | 0 | 0 |
| `tests/sources/main_test_218_body_lookup.qxs` | 4 | 4 | 0 | 0 |
| `tests/sources/main_test_51_handler_lifetimes.qxs` | 4 | 4 | 0 | 0 |
| `tests/sources/main_test_62_atomic_threads.qxs` | 4 | 0 | 4 | 0 |
| `tests/sources/main_test_big_integer.qxs` | 4 | 3 | 1 | 0 |
| `tests/sources/main_test_06_arguments_and_operators.qxs` | 3 | 2 | 1 | 0 |
| `tests/sources/main_test_118_atomic_pointers.qxs` | 3 | 1 | 2 | 0 |
| `tests/sources/main_test_125_virtual_base_casts.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_172_loop_condition_exceptions.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_215_trivial_relocation.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_225_noexcept_traits.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_70_rotation_counts.qxs` | 3 | 2 | 1 | 0 |
| `tests/sources/main_test_77_return_lifetimes.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_83_defer_captures.qxs` | 3 | 3 | 0 | 0 |
| `tests/sources/main_test_100_union_assignment_failures.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_101_storage_reuse.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_105_test_expect.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_111_generated_declarations.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_121_loop_handler_exits.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_131_atomic_publication.qxs` | 2 | 0 | 2 | 0 |
| `tests/sources/main_test_149_constexpr_global_tags.qxs` | 2 | 1 | 1 | 0 |
| `tests/sources/main_test_167_static_branch_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_168_visit_exception_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_171_runtime_branch_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_173_placement_arguments.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_175_new_argument_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_224_checked_cast_fallback.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_229_square_root_runtime.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_29_cord.qxs` | 2 | 0 | 2 | 0 |
| `tests/sources/main_test_39_polymorphic_base.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_54_conversion_boundaries.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_63_thread_local_objects.qxs` | 2 | 0 | 2 | 0 |
| `tests/sources/main_test_75_reflection_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_76_composite_calls.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_82_nested_exceptions.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_84_handler_selection.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_94_default_lifetimes.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_96_delegate_temporaries.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_97_closure_copy_failures.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_99_nested_unions.qxs` | 2 | 2 | 0 | 0 |
| `tests/sources/main_test_byte_regex.qxs` | 2 | 1 | 1 | 0 |
| `tests/sources/main_test_native_sqrt.qxs` | 2 | 2 | 0 | 0 |
| `std/sources/avl_tree_tests.qxs` | 1 | 0 | 1 | 0 |
| `tests/sources/main_test_02_serialization.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_102_allocated_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_112_global_initialization.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_126_struct_modifiers.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_129_final_structs.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_132_atomic_total_order.qxs` | 1 | 0 | 1 | 0 |
| `tests/sources/main_test_150_ignored_arguments.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_166_assertion_temporaries.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_169_match_guard_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_170_static_choose_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_174_explicit_storage_calls.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_176_delete_evaluation.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_177_throw_operand_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_184_procedure_compatibility.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_186_unevaluated_owned_expressions.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_191_index_argument_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_194_unary_postfix_exceptions.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_199_dereference_boundaries.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_220_optional.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_227_atomic_u64.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_228_constexpr_symbol_identity.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_22_filesystem.qxs` | 1 | 0 | 1 | 0 |
| `tests/sources/main_test_38_exception_threads.qxs` | 1 | 0 | 1 | 0 |
| `tests/sources/main_test_41_short_circuit.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_46_call_binding.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_65_procedure_calls.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_78_conditional_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_79_label_lifetimes.qxs` | 1 | 1 | 0 | 0 |
| `tests/sources/main_test_95_owned_packs.qxs` | 1 | 1 | 0 | 0 |
| **Total** | **320** | **241** | **70** | **9** |

The largest groups correspond to features that the Cortado backend does not
implement: inheritance, exception handling, checked arithmetic and checked
conversions, atomic operations, and threads.
[Backends](backends.md#feature-availability-by-backend) lists those features.

### Annotated declarations

Each block lists the annotated declarations of one source file. The coverage
text is the `DOC` text of the declaration.

??? note "`std/sources/avl_tree_tests.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `avl_structural_invariants` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_02_serialization.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `checked_and_partial_narrowing_test` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_06_arguments_and_operators.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `jvm_command_line_synthetic_argument_test` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `wide_integer_operations_test` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `half_precision_addition_test` | `DUAL_TEST` | `BACKEND_LLVM == FALSE` | Half-precision addition produces an exact representable sum. |

??? note "`tests/sources/main_test_100_union_assignment_failures.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `union_assignment_tests::boxed_retry` | `DUAL_TEST` | `ARCH_IS_JVM` | Boxed union assignment retains its previous alternative on argument-copy failure. |
    | `union_assignment_tests::inline_retry` | `DUAL_TEST` | `ARCH_IS_JVM` | Inline union assignment retains its previous alternative on argument-copy failure. |

??? note "`tests/sources/main_test_101_storage_reuse.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `storage_reuse_tests::repeated_owned_lifetimes` | `DUAL_TEST` | `ARCH_IS_JVM` | Reusing typed storage starts a fresh owned lifetime each time and explicit destruction runs exactly once. |
    | `storage_reuse_tests::retry_failed_placement` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed placement constructor leaves storage reusable without destroying the incomplete object. |

??? note "`tests/sources/main_test_102_allocated_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `allocated_lifetime_tests::partial_construction_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Allocated storage permits partial construction, reverse destruction of the completed prefix, and reuse before deallocation. |

??? note "`tests/sources/main_test_105_test_expect.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `test_expect_tests::condition_evaluated_once` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `test_expect_tests::typed_failure` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_110_wide_flagsets.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `wide_flagset_tests::nonbyte_word_boundary` | `DUAL_TEST` | `ARCH_IS_JVM` | A sixty-five-bit flagset retains its top bit and unnamed reserved bits. |
    | `wide_flagset_tests::full_double_word` | `DUAL_TEST` | `ARCH_IS_JVM` | A 128-bit flagset preserves the top bit through every binary bitwise operation. |
    | `wide_flagset_tests::beyond_double_word` | `DUAL_TEST` | `ARCH_IS_JVM` | A 129-bit flagset preserves implicit flags, reserved bits and byte padding. |
    | `wide_flagset_tests::inferred_four_words` | `DUAL_TEST` | `ARCH_IS_JVM` | A computed mask determines a 256-bit representation. |

??? note "`tests/sources/main_test_111_generated_declarations.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `generated_declaration_tests::snapshot_array_extent` | `DUAL_TEST` | `ARCH_IS_JVM` | A generated array declaration freezes the visible static count as its extent. |
    | `generated_declaration_tests::exception_exits` | `DUAL_TEST` | `ARCH_IS_JVM` | A throw from each generated block unwinds its local before reaching the caller and skips later blocks. |

??? note "`tests/sources/main_test_112_global_initialization.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `global_retry_tests::retry_after_exception` | `DUAL_TEST` | `ARCH_IS_JVM` | Failed initialization cleans up and releases its guard, while success suppresses later retries. |

??? note "`tests/sources/main_test_118_atomic_pointers.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_pointer_tests::comparison_modes` | `DUAL_TEST` | `ARCH_IS_JVM` | Null and live pointer transitions obey the expected-value contract for all supported ordering pairs. |
    | `atomic_pointer_tests::nested_atomic_storage` | `UNIT_TEST` | `ARCH_IS_JVM` | Operations on atomic fields in adjacent array elements affect only the selected storage. |
    | `atomic_pointer_tests::exchange_identity` | `UNIT_TEST` | `ARCH_IS_JVM` | Exchange returns the previous pointer and supports transitions to and from null. |

??? note "`tests/sources/main_test_121_loop_handler_exits.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `loop_handler_exit_tests::while_restores_outer_exception` | `DUAL_TEST` | `ARCH_IS_JVM` | Handler exits clean up inner scopes and restore the enclosing exception before the next iteration or loop exit. |
    | `loop_handler_exit_tests::loop_step_after_handler_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | CONTINUE runs STEP after handler and body cleanup, while BREAK skips STEP. |

??? note "`tests/sources/main_test_125_virtual_base_casts.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `virtual_base_cast_tests::diamond_identity` | `DUAL_TEST` | `ARCH_IS_JVM` | Casts through a shared virtual root recover the correct branches and complete object. |
    | `virtual_base_cast_tests::copied_virtual_root` | `DUAL_TEST` | `ARCH_IS_JVM` | A copied complete object owns an independent virtual root whose dynamic casts target the copy. |
    | `virtual_base_cast_tests::null_and_const_casts` | `DUAL_TEST` | `ARCH_IS_JVM` | Null virtual-base pointers remain null through casts, and const casts preserve a live object's identity. |

??? note "`tests/sources/main_test_126_struct_modifiers.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `struct_modifier_tests::rooted_scope_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Rooted local storage retains field identity through reference mutation and exceptional cleanup. |

??? note "`tests/sources/main_test_129_final_structs.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `final_struct_tests::final_leaf_dispatch` | `DUAL_TEST` | `ARCH_IS_JVM` | Final leaves preserve virtual dispatch and independent copies through base references. |

??? note "`tests/sources/main_test_131_atomic_publication.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_publication_tests::relaxed_rmw_release_sequence` | `UNIT_TEST` | `ARCH_IS_JVM` | An acquire reader observes payload published before a release store and relayed through a relaxed read-modify-write. |
    | `atomic_publication_tests::failed_cas_acquires_payload` | `UNIT_TEST` | `ARCH_IS_JVM` | A failed acquire compare-exchange publishes the observed state and synchronizes preceding payload writes. |

??? note "`tests/sources/main_test_132_atomic_total_order.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_total_order_tests::cross_location_store_load` | `UNIT_TEST` | `ARCH_IS_JVM` | Sequentially consistent store-load pairs cannot both read the preceding zero values across two locations. |

??? note "`tests/sources/main_test_142_final_virtual_methods.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `final_virtual_method_tests::inherited_final_and_overload` | `DUAL_TEST` | `ARCH_IS_JVM` | Final virtual dispatch follows the base receiver while a distinct derived overload remains callable. |
    | `final_virtual_method_tests::inherited_final_override` | `DUAL_TEST` | `ARCH_IS_JVM` | A final override remains selected through another derived level and independently copied storage. |
    | `final_virtual_method_tests::reject_final_root_override` | `STATIC_TEST` | `ARCH_IS_JVM` | A final virtual declaration rejects an otherwise matching override. |
    | `final_virtual_method_tests::reject_final_middle_override` | `STATIC_TEST` | `ARCH_IS_JVM` | Finalizing an override prevents further overrides in descendants. |

??? note "`tests/sources/main_test_149_constexpr_global_tags.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `constexpr_global_tag_tests::native_thread_access` | `UNIT_TEST` | `ARCH_IS_JVM` | Unmarked thread-local globals remain accessible in native execution. |
    | `constexpr_global_tag_tests::thread_access` | `DUAL_TEST` | `ARCH_IS_JVM` | Explicitly permitted thread-local variables are accessible during constexpr execution. |

??? note "`tests/sources/main_test_150_ignored_arguments.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `ignored_argument_tests::owned_pack_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Empty and heterogeneous ignored packs preserve owned argument cleanup on normal and exceptional exits. |

??? note "`tests/sources/main_test_165_return_unequal_temporaries.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `return_unequal_temporary_tests::unequal_paths` | `DUAL_TEST` | `ARCH_IS_JVM` | Unequal comparisons destroy right then left before enclosing-scope cleanup. |
    | `return_unequal_temporary_tests::ordering_expression_paths` | `DUAL_TEST` | `ARCH_IS_JVM` | Comparison expressions evaluate once and preserve operand cleanup on equal, unequal and throwing paths. |
    | `return_unequal_temporary_tests::equal_path` | `DUAL_TEST` | `ARCH_IS_JVM` | Equal operands are destroyed before the next statement executes. |
    | `return_unequal_temporary_tests::throwing_comparison` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing comparison destroys both operands and enclosing locals without entering the equal continuation. |

??? note "`tests/sources/main_test_166_assertion_temporaries.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `assertion_temporary_tests::throwing_condition` | `DUAL_TEST` | `ARCH_IS_JVM` | Exceptions from assertion conditions destroy the operand before enclosing locals and reach the matching handler. |

??? note "`tests/sources/main_test_167_static_branch_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `static_branch_lifetime_tests::normal_continuation` | `DUAL_TEST` | `ARCH_IS_JVM` | Selected branch locals are destroyed in reverse order before the following statement and before the enclosing local. |
    | `static_branch_lifetime_tests::exceptional_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | Exceptions unwind only the selected branch's locals and skip the continuation in each instantiation. |

??? note "`tests/sources/main_test_168_visit_exception_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `visit_exception_lifetime_tests::block_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | Throwing from a VISIT block destroys its retained subject-expression temporary before entering the handler. |
    | `visit_exception_lifetime_tests::extended_continuation_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | VISIT EXTEND retains its subject-expression temporary through nested scopes and releases it during exceptional exit. |

??? note "`tests/sources/main_test_169_match_guard_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `match_guard_lifetime_tests::throwing_guard` | `DUAL_TEST` | `ARCH_IS_JVM` | Throwing guard evaluation releases guard and retained subject-expression temporaries before reaching a handler. |

??? note "`tests/sources/main_test_170_static_choose_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `static_choose_lifetime_tests::throwing_consumer` | `DUAL_TEST` | `ARCH_IS_JVM` | Both static selections clean exactly one temporary and the enclosing local before reaching the exception handler. |

??? note "`tests/sources/main_test_171_runtime_branch_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `runtime_branch_lifetime_tests::normal_continuation` | `DUAL_TEST` | `ARCH_IS_JVM` | The selected phase releases both locals before the shared continuation and preserves the enclosing local until function exit. |
    | `runtime_branch_lifetime_tests::exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A selected phase unwinds both branch locals and the enclosing local before the caller handles its phase-specific exception. |

??? note "`tests/sources/main_test_172_loop_condition_exceptions.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `loop_condition_exception_tests::throwing_filter` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing iterator filter destroys its operand and enclosing local before handling the exception without entering the body. |
    | `loop_condition_exception_tests::throwing_test` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing loop entry test releases its temporary before enclosing cleanup and suppresses both body and step. |
    | `loop_condition_exception_tests::throwing_posttest` | `DUAL_TEST` | `ARCH_IS_JVM` | CONTINUE cleans the body before a throwing posttest; its temporary and enclosing local then unwind without executing STEP. |

??? note "`tests/sources/main_test_173_placement_arguments.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `placement_argument_tests::expression_order` | `DUAL_TEST` | `ARCH_IS_JVM` | Placement expressions evaluate the destination once before named arguments and retain the object until explicit destruction. |
    | `placement_argument_tests::statement_argument_failure` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing placement argument leaves storage reusable without constructing or destroying the target object. |

??? note "`tests/sources/main_test_174_explicit_storage_calls.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `explicit_storage_call_tests::positional_placement` | `DUAL_TEST` | `ARCH_IS_JVM` | Positional placement expression and statement arguments execute once and initialize reusable storage. |

??? note "`tests/sources/main_test_175_new_argument_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `new_argument_lifetime_tests::successful_arguments` | `DUAL_TEST` | `ARCH_IS_JVM` | NEW retains an owned argument through construction and destroys it before the next statement while the allocated object remains live. |
    | `new_argument_lifetime_tests::throwing_argument_and_retry` | `DUAL_TEST` | `ARCH_IS_JVM` | A later NEW argument failure destroys the completed temporary without entering the target constructor, and a following allocation succeeds. |

??? note "`tests/sources/main_test_176_delete_evaluation.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `delete_evaluation_tests::throwing_operand_and_retry` | `DUAL_TEST` | `ARCH_IS_JVM` | DELETE does not destroy an object when its operand throws, and a later successful operand is evaluated once before destruction. |

??? note "`tests/sources/main_test_177_throw_operand_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `throw_operand_lifetime_tests::evaluation_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | THROW destroys operand temporaries before enclosing locals whether operand evaluation returns a value or itself throws. |

??? note "`tests/sources/main_test_184_procedure_compatibility.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `procedure_compatibility_tests::nonthrowing_indirect_call` | `DUAL_TEST` | `ARCH_IS_JVM` | Exact NOEXCEPT procedure pointers retain their target through copying and allow exceptions handled within the callable. |

??? note "`tests/sources/main_test_186_unevaluated_owned_expressions.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `unevaluated_owned_expression_tests::construction_and_throw` | `DUAL_TEST` | `ARCH_IS_JVM` | TYPEOF inspects owned construction and a throwing call without constructing arguments or executing the call. |

??? note "`tests/sources/main_test_191_index_argument_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `index_argument_lifetime_tests::throwing_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Failure of the second index destroys its completed predecessor and skips either index operator body. |

??? note "`tests/sources/main_test_194_unary_postfix_exceptions.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `unary_postfix_exception_tests::receiver_unwinding` | `DUAL_TEST` | `ARCH_IS_JVM` | Each throwing unary operator destroys its temporary receiver before enclosing scope cleanup and handler entry. |

??? note "`tests/sources/main_test_199_dereference_boundaries.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `dereference_boundary_tests::throwing_receiver_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed custom dereference destroys its temporary receiver before enclosing locals and handler entry. |

??? note "`tests/sources/main_test_212_boolean_nested_exceptions.qxs` (8)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `boolean_nested_exception_tests::and_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of && are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::nand_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of !& are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::xor_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of ^^ are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::nor_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of !&#124; are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::or_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of &#124;&#124; are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::implies_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of ^> are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::implied_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of ^< are skipped or unwound before handler entry with enclosing scope cleanup. |
    | `boolean_nested_exception_tests::equivalent_nested_rhs` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested throwing right operands of !^ are skipped or unwound before handler entry with enclosing scope cleanup. |

??? note "`tests/sources/main_test_214_virtual_generated_members.qxs` (9)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `virtual_generated_member_tests::contained_complete_object` | `DUAL_TEST` | `ARCH_IS_JVM` | Generated operations on an ordinary wrapper preserve the lifetime and identity of its virtual polymorphic field. |
    | `virtual_generated_member_tests::contained_construction_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed enclosing constructor destroys its completed virtual polymorphic field once before entering the handler. |
    | `virtual_generated_member_tests::default_and_copy` | `DUAL_TEST` | `ARCH_IS_JVM` | Default construction initializes the virtual base once and copying preserves owned values and complete-object identity. |
    | `virtual_generated_member_tests::move_construction` | `DUAL_TEST` | `ARCH_IS_JVM` | Moving a diamond transfers owned text once while retaining distinct complete-object identities. |
    | `virtual_generated_member_tests::assignment_and_swap` | `DUAL_TEST` | `ARCH_IS_JVM` | Generated assignment and swap update each shared virtual base once without exchanging object identity. |
    | `virtual_generated_member_tests::virtual_base_destruction` | `DUAL_TEST` | `ARCH_IS_JVM` | Generated destructors release the shared virtual base exactly once for each original, copied, and moved object. |
    | `virtual_generated_member_tests::user_constructors` | `DUAL_TEST` | `ARCH_IS_JVM` | User constructors suppress corresponding implicit overloads in both complete objects and base subobjects. |
    | `virtual_generated_member_tests::argument_construction` | `DUAL_TEST` | `ARCH_IS_JVM` | By-value arguments use complete-object constructors for copying and conversion, while an unrelated explicit constructor permits generated default construction. |
    | `virtual_generated_member_tests::default_suppression` | `STATIC_TEST` | `ARCH_IS_JVM` | Virtual polymorphic classes honor the default-constructor suppression tag. |

??? note "`tests/sources/main_test_215_trivial_relocation.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `trivial_relocation_tests::exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A relocated parameter releases its owned resource once when its callee throws. |
    | `trivial_relocation_tests::prvalue_exception_contract` | `DUAL_TEST` | `ARCH_IS_JVM` | Unwinding destroys the one transferred object without an extra constructor or destructor. |
    | `trivial_relocation_tests::return_prvalue_exception_contract` | `DUAL_TEST` | `ARCH_IS_JVM` | Exception cleanup destroys each returned value exactly once. |

??? note "`tests/sources/main_test_218_body_lookup.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `body_lookup_tests::batched_mutations` | `DUAL_TEST` | `ARCH_IS_JVM` | A single static evaluation publishes both changed array extents. |
    | `body_lookup_tests::scope_restoration` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested publications preserve shadowing and surviving outer mutations. |
    | `body_lookup_tests::pointer_aliases` | `DUAL_TEST` | `ARCH_IS_JVM` | Aliases retain storage identity while snapshots freeze their reachable objects. |
    | `body_lookup_tests::nested_type_operands` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested unevaluated operands and local type bindings use the same lexical publications. |

??? note "`tests/sources/main_test_220_optional.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `optional_tests::failed_emplacement` | `DUAL_TEST` | `ARCH_IS_JVM` | Failed replacement leaves no active value and permits a later successful emplacement. |

??? note "`tests/sources/main_test_224_checked_cast_fallback.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `checked_cast_fallback_tests::same_type` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `checked_cast_fallback_tests::selected_exception_propagates` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_225_noexcept_traits.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `noexcept_trait_tests::conditional_execution` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `noexcept_trait_tests::destructor_subobject_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `noexcept_trait_tests::destructor_override` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_227_atomic_u64.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_u64_tests::operations` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_228_constexpr_symbol_identity.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `constexpr_symbol_identity_tests::distinct_format_arguments` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_229_square_root_runtime.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `square_root_runtime_tests::binary64_rounding` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `square_root_runtime_tests::binary32_rounding` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_22_filesystem.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `filesystem_jvm_read_write_test` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_22_threads.qxs` (9)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `thread_lvalue_entry_is_independent` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_const_lvalue_entry_is_copied` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_move_only_temporary_entry` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_move_transfers_join_ownership` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_destructor_joins` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_reference_capturing_closure` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_tls_values_are_independent` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_tls_destructors_are_lifo` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |
    | `thread_entry_destruction_precedes_tls_drain` | `UNIT_TEST` | `(OS_WINDOWS || OS_MACOS || OS_LINUX) == FALSE` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_25_concurrency.qxs` (14)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `strong_ptr_strong_and_weak_lifetimes` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `shared_ptr_move_transfers_ownership` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `shared_ptr_lifetime` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `strong_and_weak_pointer_moves` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `pointer_concurrent_ownership` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `pointer_throwing_value_construction` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `thin_mutex_serializes_threads` | `UNIT_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `ordering_mutex_serializes_threads` | `UNIT_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `condition_variable_releases_and_reacquires_mutex` | `UNIT_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `semaphore_counts_permits` | `UNIT_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `thin_mutex_move_is_rejected` | `STATIC_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `ordering_mutex_move_is_rejected` | `STATIC_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `condition_variable_move_is_rejected` | `STATIC_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |
    | `semaphore_move_is_rejected` | `STATIC_TEST` | `(OS_LINUX || OS_WINDOWS || OS_MACOS) == FALSE` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_28_containers.qxs` (13)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `container_ordered_permuted_erase_and_reinsert` | `UNIT_TEST` | `ARCH_IS_JVM` | Checks membership, values, size, and traversal after each deletion and reinsertion across multiple AVL tree shapes. |
    | `container_map_basic_ordering_and_lookup` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_erasure` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_copy_construction` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_copy_is_independent` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_move_transfers_tree` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_move_only_values` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_ordered_duplicate_preserves_temporary_keys` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_set_ordering_and_erasure` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_set_balancing_stress` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_set_copy_and_move` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `container_map_lifetimes_balance` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `static_set_works` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_29_cord.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `cord_skewed_repeated_concat_remains_ordered` | `UNIT_TEST` | `ARCH_IS_JVM` | Repeatedly appends and prepends integer leaves so both skew directions require balancing while every element remains in its original logical order. |
    | `cord_free_concat_consumes_tall_operand` | `UNIT_TEST` | `ARCH_IS_JVM` | Verifies that free concat preserves every element from a tall left tree before its suffix and leaves both transferred source cords empty. |

??? note "`tests/sources/main_test_31_inheritance.qxs` (31)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `inheritance_tests::virtual_dispatch_and_inherited_member_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies implicit derived-to-base conversion, inherited member lookup, virtual dispatch, and qualified direct calls. |
    | `inheritance_tests::copied_polymorphic_dispatch` | `DUAL_TEST` | `ARCH_IS_JVM` | Copying a derived object preserves derived dispatch and creates independent base-subobject storage. |
    | `inheritance_tests::repeated_base_storage_identity` | `DUAL_TEST` | `ARCH_IS_JVM` | Explicit base selectors preserve separate storage through reference binding and generated copy construction. |
    | `inheritance_tests::secondary_base_virtual_dispatch_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies that a non-primary polymorphic base uses its own stable virtual slot ordinals. |
    | `inheritance_tests::secondary_override_direct_receiver` | `DUAL_TEST` | `ARCH_IS_JVM` | A derived receiver dispatches an override whose virtual slot belongs to a secondary base. |
    | `inheritance_tests::construction_phase_virtual_dispatch_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies that a base constructor dispatches through the base construction phase and that steady dispatch is restored afterward. |
    | `inheritance_tests::destruction_phase_virtual_dispatch_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies most-derived dispatch in the derived destructor body and base dispatch after the base destruction transition. |
    | `inheritance_tests::nested_header_phase_transition_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies that entering a base phase rewrites every runtime header owned by that active base hierarchy. |
    | `inheritance_tests::dynamic_downcast_and_cross_cast_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies successful downcasts and cross-casts as well as a failed unrelated cast in native and constexpr execution. |
    | `inheritance_tests::virtual_enable_if_precedes_slot_resolution_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies disabled virtual declarations do not introduce or override slots and enabled overload ordinals dispatch correctly. |
    | `inheritance_tests::virtual_base_constructor_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies that a complete object owns its shared virtual base while direct bases use synthesized subobject constructors. |
    | `inheritance_tests::base_and_field_destruction_order_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies derived body, derived fields, base body, and base fields are destroyed in that order. |
    | `inheritance_tests::polymorphic_copy_remains_static_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies ordinary copy construction remains available without becoming a virtual operation. |
    | `inheritance_tests::nonvirtual_destructor_tag_test` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `inheritance_tests::virtual_destructor_delete_through_base_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies allocation-info recovery, most-derived virtual destruction, and virtual deallocation through a base pointer. |
    | `inheritance_tests::nonvirtual_polymorphic_delete_exact_type_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Verifies the exact-type NONVIRTUAL deletion path while leaving invalid base deletion as undefined behavior. |
    | `inheritance_tests::animal_test` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `inheritance_tests::unchecked_static_downcast_pointer_and_reference_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Recovers an exact nonpolymorphic complete object from a secondary base pointer or reference. |
    | `inheritance_tests::unchecked_static_downcast_transitive_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Reverses a multi-level base path to the exact complete destination. |
    | `inheritance_tests::unchecked_static_downcast_polymorphic_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Recovers a polymorphic complete object from its secondary base. |
    | `inheritance_tests::unchecked_static_downcast_virtual_diamond_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Accepts multiple inheritance paths to one shared virtual base subobject. |
    | `inheritance_tests::unchecked_static_downcast_virtual_suffix_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Recovers the complete object through a virtual edge with nonvirtual prefix and suffix bases. |
    | `inheritance_tests::unchecked_static_downcast_requires_exact_type_test` | `STATIC_TEST` | `ARCH_IS_JVM` | Constant evaluation rejects a destination that is only an intermediate base of the complete object. |
    | `inheritance_tests::dynamic_type_of_base_pointer_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Reports the complete dynamic type through mutable and constant base pointers. |
    | `inheritance_tests::dynamic_type_of_secondary_and_virtual_base_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Reports the same complete type through secondary and shared virtual base subobjects. |
    | `inheritance_tests::dynamic_type_of_repeated_base_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Both distinct copies of a polymorphic base identify the same complete object type. |
    | `inheritance_tests::dynamic_type_of_evaluates_pointer_once_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Evaluates the pointer expression exactly once, including function-call operands. |
    | `inheritance_tests::dynamic_type_of_lifetime_phase_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Uses the active constructor or destructor type, and the complete type during steady object lifetime. |
    | `inheritance_tests::dynamic_type_of_null_pointer_test` | `STATIC_TEST` | `ARCH_IS_JVM` | Constant evaluation diagnoses the undefined behavior of querying a null pointer. |
    | `inheritance_tests::dynamic_type_of_object_reference_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Reports the complete dynamic type through an object reference. |
    | `inheritance_tests::type_is_polymorphic_enable_if_test` | `DUAL_TEST` | `ARCH_IS_JVM` | Supports querying the enclosing type while selecting its virtual methods. |

??? note "`tests/sources/main_test_37_exceptions.qxs` (22)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `exception_tests::saved_exception` | `DUAL_TEST` | `ARCH_IS_JVM` | A saved handle retains the original object beyond its handler and permits later propagation. |
    | `exception_tests::current_exception_nesting` | `DUAL_TEST` | `ARCH_IS_JVM` | Nested handlers restore the previous current exception after their local cleanup. |
    | `exception_tests::current_during_unwind` | `DUAL_TEST` | `ARCH_IS_JVM` | A propagating exception becomes current only after its handler is entered. |
    | `exception_tests::saved_out_of_memory` | `DUAL_TEST` | `ARCH_IS_JVM` | The permanent sentinel reference survives ordinary handle ownership and repeated throws. |
    | `exception_tests::exact_reference` | `DUAL_TEST` | `ARCH_IS_JVM` | Binds an exception by its explicit reference type. |
    | `exception_tests::call_propagation` | `DUAL_TEST` | `ARCH_IS_JVM` | Finds a caller handler before unwinding a callee. |
    | `exception_tests::mutable_rethrow` | `DUAL_TEST` | `ARCH_IS_JVM` | Rethrow preserves changes to the original exception payload. |
    | `exception_tests::default_and_sentinel` | `DUAL_TEST` | `ARCH_IS_JVM` | Ordinary objects bypass the sentinel handler and reach the default. |
    | `exception_tests::deferred_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Exceptional scope exit executes deferred actions before entering a handler. |
    | `exception_tests::caught_inside_noexcept` | `DUAL_TEST` | `ARCH_IS_JVM` | An internal handler satisfies the callable's nonthrowing contract. |
    | `exception_tests::sentinel_rethrow` | `DUAL_TEST` | `ARCH_IS_JVM` | The allocation-failure sentinel has no ordinary object or typed binding. |
    | `exception_tests::nested_handler` | `DUAL_TEST` | `ARCH_IS_JVM` | Handling another exception does not replace the lexical rethrow payload. |
    | `exception_tests::payload_lifetime` | `DUAL_TEST` | `ARCH_IS_JVM` | The caught payload survives cleanup and is destroyed on handler exit. |
    | `exception_tests::sentinel_during_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A deferred action can handle the sentinel while another exception propagates. |
    | `exception_tests::destructor_nested_exception` | `DUAL_TEST` | `ARCH_IS_JVM` | A destructor may handle a local exception while the original exception continues outward. |
    | `exception_tests::indirect_exception_calls` | `DUAL_TEST` | `ARCH_IS_JVM` | Procedure, virtual, and interface calls propagate through ordinary catcher boundaries. |
    | `exception_tests::universal_polymorphic_view` | `DUAL_TEST` | `ARCH_IS_JVM` | All polymorphic subobjects share one canonical interface with dynamic type and virtual destruction. |
    | `exception_tests::polymorphic_base` | `DUAL_TEST` | `ARCH_IS_JVM` | A unique polymorphic base binds to the corresponding subobject. |
    | `exception_tests::ambiguous_base` | `DUAL_TEST` | `ARCH_IS_JVM` | Repeated base subobjects do not match a typed base handler. |
    | `exception_tests::partial_object_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed constructor destroys completed fields without destroying the complete object. |
    | `exception_tests::handler_exit_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Leaving a handler by GOTO destroys its exception payload. |
    | `exception_tests::partial_array_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed array copy destroys only its completed prefix. |

??? note "`tests/sources/main_test_38_exception_threads.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `exception_threads` | `UNIT_TEST` | `(OS_MACOS || OS_LINUX || OS_WINDOWS) == FALSE` | Handlers are thread-local while exception handles and the permanent sentinel share atomic ownership. |

??? note "`tests/sources/main_test_39_polymorphic_base.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `polymorphic_builtin_tests::canonical_builtin` | `DUAL_TEST` | `ARCH_IS_JVM` | The built-in view shares complete-object identity while lowercase methods remain user-defined. |
    | `polymorphic_builtin_tests::reference_evaluated_once` | `DUAL_TEST` | `ARCH_IS_JVM` | A reference expression is evaluated once and preserves the complete object identity. |

??? note "`tests/sources/main_test_41_short_circuit.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `short_circuit_tests::rhs_exception_and_skipped_throw` | `DUAL_TEST` | `ARCH_IS_JVM` | Unwinds evaluated right operands and never constructs skipped throwing operands. |

??? note "`tests/sources/main_test_46_call_binding.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `call_binding_tests::argument_exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing argument skips later expressions and destroys previously constructed argument temporaries. |

??? note "`tests/sources/main_test_51_handler_lifetimes.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `handler_lifetime_tests::normal_try_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | A normal TRY exit executes local cleanup without entering any handler or establishing a current exception. |
    | `handler_lifetime_tests::handler_return_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Returning from a handler restores the caller's exception state after destroying its locals and payload. |
    | `handler_lifetime_tests::retained_payload_last_handle` | `DUAL_TEST` | `ARCH_IS_JVM` | Copies of EXCEPTION_PTR retain one shared payload, which is destroyed only after the final handle leaves scope. |
    | `handler_lifetime_tests::handler_loop_exits` | `DUAL_TEST` | `ARCH_IS_JVM` | CONTINUE and BREAK from a handler clean handler actions before loop-body locals and restore exception state. |

??? note "`tests/sources/main_test_52_array_lifetimes.qxs` (5)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `array_lifetime_tests::copy_failure_at_each_position` | `DUAL_TEST` | `ARCH_IS_JVM` | Failure at the first, middle, or last element destroys exactly the completed prefix in reverse order. |
    | `array_lifetime_tests::successful_copy_reverse_destruction` | `DUAL_TEST` | `ARCH_IS_JVM` | A complete copied array owns separate elements and destroys them from last to first. |
    | `array_lifetime_tests::empty_array_copy` | `DUAL_TEST` | `ARCH_IS_JVM` | Empty arrays require no element construction or destruction even for nontrivial element types. |
    | `array_lifetime_tests::nested_array_partial_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Failure in the second inner array cleans the completed first inner array without destroying unconstructed elements. |
    | `array_lifetime_tests::partial_array_before_enclosing_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed array copy destroys its completed prefix before enclosing locals and preserves the handler's outer lifetime. |

??? note "`tests/sources/main_test_54_conversion_boundaries.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `conversion_boundary_tests::valid_narrowing_endpoints` | `DUAL_TEST` | `ARCH_IS_JVM` | CHECKED, ASSUME, and PARTIAL preserve exact limits for 8-, 16-, and 32-bit signed destinations. |
    | `conversion_boundary_tests::widening_signedness` | `DUAL_TEST` | `ARCH_IS_JVM` | Widening sign-extends signed values and zero-extends unsigned values, including non-byte-width sources. |

??? note "`tests/sources/main_test_57_struct_lifetimes.qxs` (5)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `struct_lifetime_tests::copy_failure_at_each_field` | `DUAL_TEST` | `ARCH_IS_JVM` | A failed generated copy destroys only completed fields, in reverse declaration order. |
    | `struct_lifetime_tests::copy_independence_and_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Generated copies invoke each field copy constructor and own independently mutable fields. |
    | `struct_lifetime_tests::move_transfers_field_ownership` | `DUAL_TEST` | `ARCH_IS_JVM` | Generated moves invoke field move constructors without copying or duplicating destruction ownership. |
    | `struct_lifetime_tests::constructor_body_failure_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Throwing from a constructor destroys initialized fields but does not invoke the incomplete owner's destructor body. |
    | `struct_lifetime_tests::destructor_body_precedes_fields` | `DUAL_TEST` | `ARCH_IS_JVM` | Successful objects execute their destructor body before destroying fields in reverse order. |

??? note "`tests/sources/main_test_58_constructor_delegates.qxs` (5)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `constructor_delegate_tests::field_declaration_order` | `DUAL_TEST` | `ARCH_IS_JVM` | Delegate expressions follow field declaration order and omitted scalar fields are default initialized. |
    | `constructor_delegate_tests::delegate_failure_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing delegate prevents later argument evaluation and owner-body execution, then destroys the completed prefix. |
    | `constructor_delegate_tests::base_before_field` | `DUAL_TEST` | `ARCH_IS_JVM` | Direct bases initialize before ordinary fields and survive until after field destruction. |
    | `constructor_delegate_tests::argument_failure_before_construction` | `DUAL_TEST` | `ARCH_IS_JVM` | Throwing arguments prevent the affected constructor, later delegates and owner body from executing. |
    | `constructor_delegate_tests::successful_checked_arguments` | `DUAL_TEST` | `ARCH_IS_JVM` | Successful checked arguments reach every delegate and the owner body without premature field destruction. |

??? note "`tests/sources/main_test_61_atomic_values.qxs` (7)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_value_tests::extended_rmw_constant_values` | `DUAL_TEST` | `ARCH_IS_JVM` | Named operations agree during constant evaluation and native execution. |
    | `atomic_value_tests::extended_rmw_orderings` | `UNIT_TEST` | `ARCH_IS_JVM` | Named RMW operations accept every supported atomic ordering. |
    | `atomic_value_tests::read_modify_write_widths` | `UNIT_TEST` | `ARCH_IS_JVM` | Native atomic operations preserve return and stored values for signed and unsigned 32-bit operands. |
    | `atomic_value_tests::read_modify_write_wide_widths` | `UNIT_TEST` | `ARCH_IS_X86 || ARCH_IS_JVM` | Atomic operations preserve return and stored values for signed and unsigned 64-bit operands. |
    | `atomic_value_tests::compare_exchange_modes` | `UNIT_TEST` | `ARCH_IS_JVM` | Exercises success and failure for every supported pairing of atomic comparison orderings. |
    | `atomic_value_tests::boolean_compare_exchange` | `UNIT_TEST` | `ARCH_IS_JVM` | Boolean compare-exchange writes back observed false and true values and preserves expected on success. |
    | `atomic_value_tests::exchange_constant_values` | `DUAL_TEST` | `ARCH_IS_JVM` | Exchange returns the previous value and updates integer and boolean storage during constant evaluation and native execution. |

??? note "`tests/sources/main_test_62_atomic_threads.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `atomic_thread_tests::release_acquire_publication` | `UNIT_TEST` | `ARCH_IS_JVM` | A release/acquire handshake publishes both payload fields and prevents the next write until the consumer finishes reading. |
    | `atomic_thread_tests::contended_fetch_add_results` | `UNIT_TEST` | `ARCH_IS_JVM` | Concurrent fetch-add operations preserve the final counter and the sum of all returned ticket values. |
    | `atomic_thread_tests::contended_compare_exchange` | `UNIT_TEST` | `ARCH_IS_JVM` | Compare-exchange retries use the observed value written back to expected after a failed attempt. |
    | `atomic_thread_tests::contended_fetch_multiply` | `UNIT_TEST` | `ARCH_IS_JVM` | Compare-exchange multiplication preserves every update and returns each preceding power exactly once. |

??? note "`tests/sources/main_test_63_thread_local_objects.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `thread_local_object_tests::concurrent_owned_state` | `UNIT_TEST` | `ARCH_IS_JVM` | Two simultaneous threads and their parent retain distinct owned strings and each worker destroys its own state once. |
    | `thread_local_object_tests::fresh_state_after_thread_exit` | `UNIT_TEST` | `ARCH_IS_JVM` | Successive threads start from the initializer instead of retaining the previous thread's mutated owned state. |

??? note "`tests/sources/main_test_65_procedure_calls.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `procedure_call_tests::indirect_exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Exceptions from indirect calls preserve the payload and unwind the caller's completed locals. |

??? note "`tests/sources/main_test_69_checked_conversions.qxs` (10)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `checked_conversion_tests::unsigned_endpoints` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks exact narrowing endpoints across standard and non-byte unsigned widths. |
    | `checked_conversion_tests::signed_endpoints` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked conversions preserve signed endpoints and valid signedness changes. |
    | `checked_conversion_tests::signed_byte_above_maximum` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked signed narrowing rejects the first value above the positive bound. |
    | `checked_conversion_tests::signed_byte_below_minimum` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked signed narrowing rejects the first value below the negative bound. |
    | `checked_conversion_tests::negative_to_unsigned` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked conversion rejects a negative source even when its bit width fits the unsigned destination. |
    | `checked_conversion_tests::unsigned_byte_above_maximum` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked unsigned narrowing rejects the first value that loses a high bit. |
    | `checked_conversion_tests::nonbyte_signed_above_maximum` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked narrowing enforces a non-byte signed range rather than its storage-byte range. |
    | `checked_conversion_tests::nonbyte_unsigned_above_maximum` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked narrowing enforces a non-byte unsigned range rather than its storage-byte range. |
    | `checked_conversion_tests::same_width_signedness` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked conversions reject values outside the range after a signedness change. |
    | `checked_conversion_tests::conversion_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Conversion failure unwinds deferred actions before entering the handler. |

??? note "`tests/sources/main_test_70_rotation_counts.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `rotation_count_tests::five_bit_patterns` | `UNIT_TEST` | `ARCH_IS_JVM` | Every signed and unsigned five-bit pattern rotates over in-range and out-of-range counts. |
    | `rotation_count_tests::odd_width_high_bit_patterns` | `DUAL_TEST` | `ARCH_IS_JVM` | Signed high bits and mixed patterns retain their full logical width at the maximum valid rotation count. |
    | `rotation_count_tests::large_rotation_counts` | `DUAL_TEST` | `ARCH_IS_JVM` | Odd-width rotations preserve all count bits before modulo reduction, including both compound forms. |

??? note "`tests/sources/main_test_71_checked_arithmetic.qxs` (15)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `checked_arithmetic_tests::exception_ownership` | `DUAL_TEST` | `ARCH_IS_JVM` | Overflow exceptions retain ordinary object identity across saved handles and rethrows. |
    | `checked_arithmetic_tests::valid_operations` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked and assumed operations agree with ordinary arithmetic inside the permitted range. |
    | `checked_arithmetic_tests::addition_overflow_operands` | `DUAL_TEST` | `ARCH_IS_JVM` | Signed, unsigned, and non-byte additions propagate typed range failures. |
    | `checked_arithmetic_tests::subtraction_overflow_operands` | `DUAL_TEST` | `ARCH_IS_JVM` | Signed bounds and unsigned underflow produce typed subtraction failures. |
    | `checked_arithmetic_tests::multiplication_overflow_operands` | `DUAL_TEST` | `ARCH_IS_JVM` | Signed and unsigned products outside the destination range produce typed failures. |
    | `checked_arithmetic_tests::division_overflow_operands` | `DUAL_TEST` | `ARCH_IS_JVM` | Dividing the minimum signed value by negative one produces a typed failure. |
    | `checked_arithmetic_tests::division_by_zero` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked division by zero throws arithmetic overflow for signed and unsigned operands. |
    | `checked_arithmetic_tests::overflow_exceptions` | `DUAL_TEST` | `ARCH_IS_JVM` | Range failures propagate as typed exceptions for signed and unsigned arithmetic. |
    | `checked_arithmetic_tests::invalid_counts` | `DUAL_TEST` | `ARCH_IS_JVM` | Checked shifts and rotations reject counts equal to or larger than the bit width. |
    | `checked_arithmetic_tests::integer_widths` | `DUAL_TEST` | `ARCH_IS_JVM` | Range contracts cover odd widths and integers wider than a machine register. |
    | `checked_arithmetic_tests::mutating_valid_operations` | `DUAL_TEST` | `ARCH_IS_JVM` | Mutating range operators store the successful result for each arithmetic and bit operation. |
    | `checked_arithmetic_tests::mutating_failures` | `DUAL_TEST` | `ARCH_IS_JVM` | Overflow leaves each destination unchanged and propagates the typed exception. |
    | `checked_arithmetic_tests::overflow_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Arithmetic failure unwinds deferred actions before entering the enclosing handler. |
    | `checked_arithmetic_tests::byte_arithmetic` | `DUAL_TEST` | `ARCH_IS_JVM` | Byte arithmetic uses unsigned range checks and division for high bit patterns. |
    | `checked_arithmetic_tests::rotation_counts` | `DUAL_TEST` | `ARCH_IS_JVM` | Ordinary rotations reduce the full count, including counts larger than a narrow operand can hold. |

??? note "`tests/sources/main_test_74_union_lifetimes.qxs` (6)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `union_lifetime_tests::boxed_replacement` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks boxed union owned alternative replacement in constexpr and native execution. |
    | `union_lifetime_tests::boxed_match_unwinding` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks boxed union match exception cleanup in constexpr and native execution. |
    | `union_lifetime_tests::boxed_failed_payload_copy` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks boxed union failed payload construction in constexpr and native execution. |
    | `union_lifetime_tests::inline_replacement` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks inline union owned alternative replacement in constexpr and native execution. |
    | `union_lifetime_tests::inline_match_unwinding` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks inline union match exception cleanup in constexpr and native execution. |
    | `union_lifetime_tests::inline_failed_payload_copy` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks inline union failed payload construction in constexpr and native execution. |

??? note "`tests/sources/main_test_75_reflection_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `reflection_lifetime_tests::temporary_normal_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Reflection evaluates its temporary receiver once and destroys every field after the consuming call. |
    | `reflection_lifetime_tests::temporary_exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing call cleans up all fields of its reflected temporary receiver before the handler runs. |

??? note "`tests/sources/main_test_76_composite_calls.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `composite_call_tests::owned_normal_return` | `DUAL_TEST` | `ARCH_IS_JVM` | An applied call destroys its moved value parameter on normal return. |
    | `composite_call_tests::owned_exception_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | An applied call destroys its moved value parameter before transferring to an exception handler. |

??? note "`tests/sources/main_test_77_return_lifetimes.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `return_lifetime_tests::selected_return` | `DUAL_TEST` | `ARCH_IS_JVM` | Both early and trailing returns retain one owner until the caller destroys the result. |
    | `return_lifetime_tests::copied_return_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Successful return construction precedes reverse local cleanup and gives the caller independent ownership. |
    | `return_lifetime_tests::failed_return_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Failure while constructing the return value destroys both completed locals and never destroys the incomplete result. |

??? note "`tests/sources/main_test_78_conditional_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `conditional_lifetime_tests::throwing_nested_condition` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing nested condition destroys its temporary before enclosing branch locals and never enters either nested arm. |

??? note "`tests/sources/main_test_79_label_lifetimes.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `label_lifetime_tests::goto_from_handler` | `DUAL_TEST` | `ARCH_IS_JVM` | Leaving a catch handler by GOTO performs deferred cleanup and restores the previous exception state. |

??? note "`tests/sources/main_test_82_nested_exceptions.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `nested_exception_tests::restore_outer_handler` | `DUAL_TEST` | `ARCH_IS_JVM` | A nested handler temporarily replaces the current exception and restores the still-live outer payload afterward. |
    | `nested_exception_tests::replacement_unwinds_handler` | `DUAL_TEST` | `ARCH_IS_JVM` | Throwing a replacement exception unwinds the old handler while an explicitly retained handle keeps the old payload alive. |

??? note "`tests/sources/main_test_83_defer_captures.qxs` (3)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `defer_capture_tests::owned_capture_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | A deferred owned capture is invoked before destruction on both normal and exceptional scope exit. |
    | `defer_capture_tests::move_captured_callable` | `DUAL_TEST` | `ARCH_IS_JVM` | Moving a closure into DEFER CALL transfers its owned capture without copying it or invoking the moved-from closure. |
    | `defer_capture_tests::failed_capture_registration` | `DUAL_TEST` | `ARCH_IS_JVM` | A throwing capture constructor leaves no registered action and unwinds previously registered actions normally. |

??? note "`tests/sources/main_test_84_handler_selection.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `handler_selection_tests::nominal_type_dispatch` | `DUAL_TEST` | `ARCH_IS_JVM` | Handler selection distinguishes signedness and nominal structure identity independently of equal storage layout. |
    | `handler_selection_tests::nonmatching_handler_propagation` | `DUAL_TEST` | `ARCH_IS_JVM` | Nonmatching inner handlers propagate the original exception through local cleanup into an outer typed handler. |

??? note "`tests/sources/main_test_94_default_lifetimes.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `default_lifetime_tests::default_construction_paths` | `DUAL_TEST` | `ARCH_IS_JVM` | Defaults are constructed in order and only completed temporaries are destroyed if a later default throws. |
    | `default_lifetime_tests::explicit_argument_suppresses_default` | `DUAL_TEST` | `ARCH_IS_JVM` | An explicit owned reference suppresses its throwing default and remains alive after the call. |

??? note "`tests/sources/main_test_95_owned_packs.qxs` (1)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `owned_pack_tests::normal_and_exceptional_exit` | `DUAL_TEST` | `ARCH_IS_JVM` | Indexed forwarding preserves mixed pack types and destroys the moved owned payload exactly once on either exit. |

??? note "`tests/sources/main_test_96_delegate_temporaries.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `delegate_temporary_tests::normal_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | Each delegate temporary expires before the next delegate, while completed fields live until owner destruction. |
    | `delegate_temporary_tests::failed_delegate_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | A failing delegate destroys its borrowed temporary and the completed field prefix without running later delegates. |

??? note "`tests/sources/main_test_97_closure_copy_failures.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `closure_copy_tests::capture_failure_prefix` | `DUAL_TEST` | `ARCH_IS_JVM` | A failing capture construction destroys only completed captures in reverse order. |
    | `closure_copy_tests::copy_failure_prefix` | `DUAL_TEST` | `ARCH_IS_JVM` | A failing closure copy destroys only completed captures in reverse order. |

??? note "`tests/sources/main_test_99_nested_unions.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `nested_union_tests::boxed_outer_combinations` | `DUAL_TEST` | `ARCH_IS_JVM` | Boxed outer ownership works with both boxed and inline inner storage. |
    | `nested_union_tests::inline_outer_combinations` | `DUAL_TEST` | `ARCH_IS_JVM` | Inline outer ownership works with both boxed and inline inner storage. |

??? note "`tests/sources/main_test_benchmark_runtime.qxs` (9)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `benchmark_runtime_tests::counts` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::integer_parser` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::string_and_boolean_parser` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::float_parser` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::float_long_integer` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::float_long_fraction` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::float_invalid_suffix` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::float_range` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `benchmark_runtime_tests::integer_overflow` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_big_integer.qxs` (4)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `big_integer_tests::signed_quotients` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `big_integer_tests::generic_integer_types` | `DUAL_TEST` | `ARCH_IS_JVM` | Checks typed construction, arbitrary multiplication, and checked quotients with different integer widths. |
    | `big_integer_tests::quotient_errors` | `DUAL_TEST` | `ARCH_IS_JVM` | Division failures distinguish a zero divisor from a floor quotient outside the requested range. |
    | `big_integer_tests::carry_and_aliasing` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_byte_regex.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `byte_regex_tests::matching` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `byte_regex_tests::invalid_patterns` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_hadix_containers.qxs` (11)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `hadix_container_tests::custom_hash_policy` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::stateful_hash_ownership` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::reported_errors` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::geometric_growth` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::avl_ranks_split_join` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::map_growth_and_shrink` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::map_collisions` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::unordered_bucket_split_join` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::bucket_join_comparison_failure` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::iteration_and_copy` | `UNIT_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `hadix_container_tests::failure_cleanup` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

??? note "`tests/sources/main_test_native_sqrt.qxs` (2)"

    | Test | Kind | Condition | Coverage |
    | --- | --- | --- | --- |
    | `native_sqrt_tests::exact_and_special` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |
    | `native_sqrt_tests::rounding_and_extremes` | `DUAL_TEST` | `ARCH_IS_JVM` | The declaration carries no `DOC` text. |

## Other verified defects and limitations

### Compiler and configuration

| Issue | Description | Evidence |
| --- | --- | --- |
| Target check coverage | CTest registers `qxc check` for seven of the ten bundle targets. `linux-arm32`, `linux-riscv32`, and `linux-riscv64` are compiled only when a contributor builds the bundle. | `quxlang/CMakeLists.txt`, the `qxc_check_test` loop |
| No automated execution | No automated test executes a generated executable. Execution results depend on manual runs. | `quxlang/CMakeLists.txt`; the project rules |
| Unwind-format predicates on a JVM target | Evaluation of any `UNWIND_FORMAT_IS_*` predicate on a JVM target raises an internal compiler error in place of yielding `FALSE`. | `quxlang/include/quxlang/data/machine.hpp`, `current_codegen_unwind_format` |
| `wasm` binary format | The configuration loader accepts `binary: wasm`. The LLVM backend then fails with an internal compiler error, because no target triple exists for the format. | `quxlang/include/quxlang/manipulators/llvm_lookup.hpp` |
| Reserved output kinds | `shared_library`, `static_library`, and `image` pass configuration validation and fail in the output stage. | `quxlang/sources/queries/llvm_output_binary_artifact.cpp` |
| Backend name in the Reference | The `qxcbuild.yml` reference names the backend value `cortado`. The loader accepts only `experimental_cortado`. | `quxlang/sources/loaders/source_loader.cpp` |

### Targets

| Issue | Description | Evidence |
| --- | --- | --- |
| Windows execution | Windows x64 executables are cross-compiled. Their execution is unverified. | [Exception Handling](../reference/exceptions.md#constant-evaluation-and-native-support) |
| RISC-V execution | The RISC-V targets are not tested with real hardware. | Commit message of the change that added the targets |
| ARM32 execution | The repository records no execution result for `linux-arm32`. | Absence of a record |
| Windows file opening | The `io` module has no function that opens a file by path on Windows. | `io/sources/filesystem.qxs` |
| `std::args` and `std::env` | `std::args` has no Windows implementation. `std::env` is implemented for Linux only. | `std/sources/std.qxs` |

### Runtime module

| Issue | Description | Evidence |
| --- | --- | --- |
| Unit-test summary line | The final summary line of a unit-test suite omits the number of known-broken tests and always prints the text `disabled with KNOWN_BROKEN`. With at least one known-broken test, the text is printed twice. | `runtime/sources/unit_test.qxs`, the summary at the end of `UNIT_TEST_MAIN` |
| Frame lookup cost during exception propagation | The unwinder locates the frame description of each frame by a linear scan of the retained unwind section. The cost of propagation grows with the number of frame descriptions in the executable. | `runtime/sources/exception_dwarf.qxs`, `unwind_find_frame` |
| Exceptions in tests on a JVM target | The unit-test driver calls each test without a `TRY` block on a JVM target. | `runtime/sources/unit_test.qxs` |

### Language features

[Language Features](language-features.md) lists the forms that are reserved,
parsed without an implementation, or rejected by the compiler. The following
entries are the restrictions with the widest effect.

| Issue | Description | Reference |
| --- | --- | --- |
| Foreign exceptions | Exception propagation covers frames of the generated executable only. An exception requires handling before control crosses into a foreign frame. | [Exception Handling](../reference/exceptions.md#constant-evaluation-and-native-support) |
| Bound-member procedure pointers | A bound member cannot convert to a runtime procedure pointer. | [Procedure Pointers and Function Values](../reference/procedure-pointers-and-function-values.md) |
| Assembly | `ASM_INLINE_FUNCTION` is not implemented, and structured `EXTERNAL` operands are incomplete for ARM-family assembly. An external procedure declaration represents an external call in those cases. | [Assembly Procedures](../reference/assembly-procedures.md#implementation-status) |
| Wide atomic storage | Atomic operations on storage wider than the native atomic width of the target CPU are rejected. | [Atomics](../reference/atomics.md) |
