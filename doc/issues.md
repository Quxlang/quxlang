This record tracks potential bugs reported during Quxlang code reviews. The review record through 2026-11-20 lists **30 current findings** (1 P1, 27 P2 and 2 P3), **1 partially fixed finding pending backend follow-up**, **6 fixed in source**, and **1 withdrawn report**. Four earlier memory investigations and the unverified JVM recovery residue appear separately. The working tree revalidation records the disposition of each open finding and the backend follow-up.

Dates are UTC. **First found** means the earliest recovered review report date, not the date the code was introduced; exact discovery times are not recorded for every issue. **Current** means unresolved in the review record as of the stated last verification, not freshly reproduced at current HEAD. **Fixed** means the source correction was verified; it does not assert a passing runtime test. Fix date is the correcting commit timestamp, shown separately from verification date.

The last confirmed consecutive review checkpoint is [636244af](https://github.com/Quxlang/quxlang/commit/636244af6b0ac8b3518299fb74d6c947bf079bd1), after the starting baseline [244e9fdd](https://github.com/rnichollx/quxlang/commit/244e9fdd3086dc4e58a091bbf0914f4ed9fe7a28). The November 17 records disagree on whether [36b922d](https://github.com/Quxlang/quxlang/commit/36b922d704d878a4e41388b94d852a646bd17032) was fully reviewed; this commit remains pending for checkpoint purposes. Its two reported findings are recorded below, with their original verification dates. On 2026-10-26, both commits after [8dab8815](https://github.com/rnichollx/quxlang/commit/8dab881511ed1181c6fbd416a7bfcf83b5087649) were statically reviewed: 58 file appearances across 55 paths, with complete patches and relevant callers, tests and dependencies. That HEAD was rechecked unchanged at 22:37 UTC. On 2026-10-27, the sole new commit [9a7d4ff1](https://github.com/rnichollx/quxlang/commit/9a7d4ff1e3402ac0d5ad03ce49ba9e220c47ae77) after [1d5242ab](https://github.com/rnichollx/quxlang/commit/1d5242aba500675729763b9987e62098cb22fe96) was statically reviewed in full: all 13 changed paths, 4,274 additions and 91 deletions, with relevant callers, tests and dependencies. Default branch main and HEAD were verified at 22:42 UTC; comparison showed one commit ahead and none behind, and both commit enumeration and changed-file pagination were exhausted. No coverage remains pending. No new credible actionable issue was identified. QXR-004, QXR-023 and QXR-024 were last reverified as current on October 26; historical findings were not fully re-audited at the October 27 HEAD, and their individual source-verification dates and statuses remain unchanged. On 2026-10-31, all three new commits [f5150e36](https://github.com/rnichollx/quxlang/commit/f5150e36881deb826616521704f44168a3843a55), [24a42fd3](https://github.com/rnichollx/quxlang/commit/24a42fd36faba5b96dc83de805a6998548e3f971) and [06f3c2c3](https://github.com/rnichollx/quxlang/commit/06f3c2c33ebf55876ce54d07820938c4c97f031d) were statically reviewed in full: 28 changed-file appearances across 26 distinct paths, with complete diffs and relevant surrounding code, callers, tests and dependencies. Comparison and commit/file pagination were exhausted; main remained at that HEAD at 23:06 UTC. No new credible actionable issue was identified and no coverage remains pending. Existing QXR-016 was reverified in source at that HEAD; other historical findings retain their individual verification dates and were not fully re-audited. On 2026-11-01, all eight commits after 06f3c2c3 through 471aea1 were reviewed: 331 changed-file appearances across 314 distinct paths, 5,277 additions and 5,828 deletions. Commit enumeration and all per-commit file pagination were exhausted; patches matched addition/deletion metadata. All eight commits form a linear descendant chain, with no merged ancestry omitted. The 266-file formatting commit was checked across all 2,597 exact line-joining edit groups; relevant behavior-changing diffs, surrounding code, callers, tests and dependencies were inspected. Default branch main and HEAD were reverified at 23:55 UTC. Two new P2 findings, QXR-025 and QXR-026, remain at that HEAD. QXR-024 was reverified Current; QXR-004 has a verified native correction and a separately labeled unverified JVM residue. Other historical findings retain their individual source-verification dates; they were not fully re-audited. No commit/file coverage remains pending. The starting baseline itself was not reviewed. These are static source findings, not claims of executed reproductions, builds or tests. On 2026-11-08, all 13 new commits after 471aea1 through 2e8e33e were statically reviewed: 403 changed-file appearances across 251 distinct paths, with 27,546 additions and 5,067 deletions. The commits form a linear descendant chain. Commit enumeration and file pagination were exhausted; file statistics and all available patch line counts matched, and three omitted large patches were recovered from complete pinned source versions. Relevant surrounding code, callers, tests and dependency exports were inspected. Four new P2 findings are recorded as QXR-027 through QXR-030. QXR-025’s original merge mechanism changed, but resolution of its original example remains unverified; it is not marked fixed. Other historical findings retain their verification dates. No commit/file coverage remains pending. Builds, tests and reproducers were not executed. On 2026-11-10, the single new commit [cf1d9ae](https://github.com/rnichollx/quxlang/commit/cf1d9ae9bef50c4d5d046c1385e774b9544176c1) after 2e8e33e was statically reviewed in full: all 40 changed paths, 1,506 additions and 333 deletions. Comparison and file pagination were exhausted; all patch line counts matched metadata. The commit has one parent, the prior reviewed checkpoint, so no merged ancestry was omitted. Review covered I/O module migration, coroutine/controller and request lifetimes, platform completion backends, epoll syscall layouts, tests, benchmark staging and relevant callers. No new credible actionable issue was established. QXR-024 and QXR-026 remain current in source; other historical findings retain their individual verification dates and were not fully re-audited. No commit/file coverage remains pending. No builds, tests or reproducers were executed. On 2026-11-11, all four linear commits after cf1d9ae through 71279dd were reviewed: 44 changed-file appearances across 36 paths, with 1,102 additions and 72 deletions. The range includes 551f1a8, cdc41203, 81b54d4 and 71279dd. Comparison and per-commit file pagination were exhausted, all patch counts matched, and relevant source, tests and guidance were inspected. The new member-coroutine forwarding defect is QXR-031. QXR-028 and QXR-030 remain current after source reinspection; the hook rename and attached-member metadata lookup change do not fix their original failures. Other historical findings retain their individual verification dates. No commit/file coverage remains pending. Review was static; builds, tests and reproducers were not executed. For the November 12 review, completed on November 13, all four linear commits after 71279dd through a4df88b were statically reviewed: eight file appearances across six paths, 13 additions and 14 deletions. Complete patches and relevant build context were inspected, with matching patch counts and exhausted pagination. Repository identity 949539346 confirms the move to Quxlang/quxlang; main and ancestry were verified. The changes remove obsolete cow build dependencies and update repository links and guidance. All three relocated GitHub dependency URLs resolve their unchanged pinned revisions. QXR-027 is fixed in source by ae89490; no new credible actionable issue was identified. Other findings retain their individual verification dates. No coverage remains pending; builds and tests were not executed. On 2026-11-14, both linear commits b0432fa and 592603c after a4df88b were statically reviewed in full: 36 changed paths and file appearances, 4,504 additions and 117 deletions. Complete patches matched metadata; comparison and per-commit pagination were exhausted. Review covered interface documentation parsing, network socket ownership and operations, platform syscall ABI wrappers, native asynchronous completion and drain lifetimes, the standalone io_uring processor, tests, build registration and documentation. No new credible actionable issue was identified. QXR-026 remains current after source reinspection; other historical findings retain their individual verification dates. No coverage remains pending. Builds, tests and reproducers were not executed. On 2026-11-15, both linear commits 95ebb60 and 636244a after 592603c were statically reviewed in full: 36 changed-file appearances across 35 paths, 1,446 additions and 124 deletions. Complete patches matched metadata; comparison and per-commit pagination were exhausted. Review covered Linux ARM32 target lowering, ELF relocations, assembly symbol literals, syscall and calling conventions, TLS and native threads, DWARF exception unwinding, Release procedure preoptimization and component LTO, tests, build registration and documentation. No new credible actionable finding or unresolved review candidate was identified. Historical findings retain their individual verification dates; they were not exhaustively re-audited. No commit or file coverage remains pending. Builds, tests and reproducers were not executed. On 2026-11-17, 36b922d was identified as the sole new commit after 636244a (39 changed paths). The original log entry reported full static review, but the contemporaneous chat update reported incomplete compiler/runtime coverage. Full coverage is unconfirmed, and the checkpoint has not advanced past 636244a. QXR-032 and QXR-033 were reported and recorded; their original source verification dates are retained. Builds, tests and reproducers were not executed.

# Working tree revalidation

The audit used commit `cde605fd` as its baseline. Commit `66800906` recorded the initial corrections.

| Finding | Revalidation and disposition |
| --- | --- |
| QXR-001, QXR-002 | Benchmark control-flow validation now recognizes deferred action exits and runtime targets. Compile-time loops introduce no runtime target. |
| QXR-003 | Commit `d6bd2298` corrected quotient range checking. The test suite covers the reported negative floor quotient. |
| QXR-004 | The native buffered writer preserves completed transfers. The Cortado backend currently lacks a local catch path for a Java I/O exception, so the proposed JVM catch-and-retry sequence could not be established. |
| QXR-006 | Published local static bindings carry the declared alignment into native snapshots. |
| QXR-007 | Address-sanitized builds of `qxc` use the sanitizer's C++ allocation operators. |
| QXR-011 | The privacy test names the existing module-private `layer5_mapping_size` function. |
| QXR-013 | The IntelliJ plugin declares typed loop bindings in the iteration scope and resolves their type expressions in the enclosing scope. |
| QXR-015 | Checked integer conversion contributes exceptional destructor dependencies. |
| QXR-016 | Erasure of a specialized generic resolves its signatures in the instance context. |
| QXR-017 | Floating-point placement rounds an unsigned byte count without multiplying it back into bits. |
| QXR-018 | The default loop step uses the same value construction path as `BY(1)`. |
| QXR-019 | Copy-specific constructor selection checks whether the caller actually supplied `OTHER`. |
| QXR-020 | LLVM lowering records that destruction has started before emitting a destructor call's exceptional path. Completed cleanup retires the slot metadata. |
| QXR-021 | Withdrawn after clarification of the contract. Lifecycle traits report the selected operation's exception guarantee. Argument preparation takes place in the caller's exception context. |
| QXR-022 | Constructor condition normalization strips nested source locations from lifecycle traits. |
| QXR-023 | Unsupported native exception storage alignment selects the allocation-failure sentinel. |
| QXR-024 | The integration harness uses the current CLI, locates the prefix documentation for the benchmark clock, and recognizes the coroutine benchmark cases. |
| QXR-025 | Commit `bec50742` removed the incorrect merge from ordinary calls. Exceptional transfer restores the pre-`TRY` ownership state. A focused test confirmed that a handler can initialize the destination again after rollback. |
| QXR-026 | The worker pool retains the stop count across posting failures and transfers that count with pool ownership. |
| QXR-028 | The direct coroutine benchmark forwards the current named pack arguments. |
| QXR-029 | The formatter passes its writer positionally, allowing a public `output` keyword field. |
| QXR-030 | Coroutine descriptor rebinding imports logical parameters while retaining their concrete types. |
| QXR-031 | Forwarding to `CO_COROUTINE_CALL` moves a bound receiver into the adapter's argument pack. |
| QXR-032 | RISC-V epoll records use the Linux layout, including alignment and array stride. |
| QXR-033 | RV32 passes the caller's page offset directly to `mmap2`. |
| QXR-034 | Both scripts invoke the `qxc build` subcommand. |
| QXR-035 | The dynamic allocator routes an empty request through a size class that satisfies its alignment. |
| QXR-036 | The optional static Linux collector can decline to start after stack or TLS setup fails. Partial setup is released. |
| QXR-037 | Background collection enforces the purged byte limit. |
| QXR-038 | The lifetime test examines pool-owned extents under the pool mutex. |

For the initial audit, the Release compiler built successfully. The macOS testbundle passed 1,865 static tests and 1,436 native tests. Compilation checks passed for all nine cross targets, and full builds linked the RV32, RV64 and Windows x64 outputs. The targeted C++ and IntelliJ suites passed 62 and 24 tests respectively. The benchmark integration checks passed in stages, including a separate run of the parser-exception fixture after the harness timed out during concurrent builds. The direct coroutine benchmark matched its C++ reference for all nine verification cases.

# Current findings

## [current] QXR-035 Zero-byte dynamic allocation can ignore requested alignment

**P2 · First found 2026-11-20 · Current at [d6bd2298](https://github.com/Quxlang/quxlang/commit/d6bd22981980fe815b6eb9b4c1cae17179eb0644); source verified 2026-11-20.**

[b750fb4a](https://github.com/Quxlang/quxlang/commit/b750fb4a29db0f526de2060f20b01ea5de236003) rounds bytes before selecting a slab, so 0 bytes rounds to 0 at any valid alignment and selects the 8-byte class. The native [public virtual allocator](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/memory.qxs#L1515-L1532) forwards that pair unchanged, and a zero-element typed allocation can also reach the router. For a zero-byte request with 16-byte alignment, [adjacent slots](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_regions.qxs#L645-L661) are 8 bytes apart, so the allocator can return storage only 8-byte aligned. Preserve the requested alignment in [both dynamic routes](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/memory.qxs#L1246-L1296), including when [rounding zero](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/memory.qxs#L1158-L1168). Static review; no runtime test was run.

## [current] QXR-036 Optional collector startup can abort a static Linux free

**P2 · First found 2026-11-20 · Current at [d6bd2298](https://github.com/Quxlang/quxlang/commit/d6bd22981980fe815b6eb9b4c1cae17179eb0644); source verified 2026-11-20.**

[b750fb4a](https://github.com/Quxlang/quxlang/commit/b750fb4a29db0f526de2060f20b01ea5de236003) lets [the release path](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_regions.qxs#L375-L395) start the collector after retaining enough idle backing. The [static Linux startup path](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_collector.qxs#L216-L244) calls helpers that panic on [TLS mapping](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/static_linux_thread.qxs#L375-L387) or [stack mapping/guard protection](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/static_linux_thread.qxs#L448-L473) failure, before the recoverable clone result is checked. Under allocation pressure, a free that triggers this optional background worker can terminate the process instead of continuing under synchronous limits. Use fallible stack/TLS setup for the collector and clean up partial setup. Static review; no failure injection was run.

## [current] QXR-037 Background collection bypasses the purged byte limit

**P2 · First found 2026-11-20 · Current at [d6bd2298](https://github.com/Quxlang/quxlang/commit/d6bd22981980fe815b6eb9b4c1cae17179eb0644); source verified 2026-11-20.**

In [b750fb4a](https://github.com/Quxlang/quxlang/commit/b750fb4a29db0f526de2060f20b01ea5de236003), [the foreground path](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_regions.qxs#L375-L392) trims purged mappings by bytes, but [the collector](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_collector.qxs#L65-L86) only checks their age after adding newly purged extents. With the supported purged cap configured below the retained cap, the collector can age a retained pool into a purged pool larger than its [documented limit](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocator_budget.qxs#L21-L26), without a subsequent free to trim it. This is retained mappings, not necessarily resident memory. Apply the same byte budget in both paths. Static review; duration not measured.

## [current] QXR-038 Allocator lifetime test can read metadata after final release unmaps it

**P3 · First found 2026-11-20 · Current at [d6bd2298](https://github.com/Quxlang/quxlang/commit/d6bd22981980fe815b6eb9b4c1cae17179eb0644); source verified 2026-11-20.**

The [lifetime test](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_regions_tests.qxs#L130-L139) was extended in [b750fb4a](https://github.com/Quxlang/quxlang/commit/b750fb4a29db0f526de2060f20b01ea5de236003) saves the owner, returns the region, and then dereferences the owner under the pool lock. With valid retention and purge limits smaller than one extent and this being its last issued region, the [release path](https://github.com/Quxlang/quxlang/blob/d6bd22981980fe815b6eb9b4c1cae17179eb0644/quxlang/tests/testdata/testbundle/modules/runtime/sources/allocation_regions.qxs#L375-L392) can unmap the owner before the test takes the lock. Inspect pool-owned state instead of a released pointer, or isolate the test pool and control its limits. Test-only finding; no runtime test was run.

## [current] QXR-034 Two scripts still use the old qxc command line

**P2 · First found 2026-11-19 · Current at [b750fb4a](https://github.com/Quxlang/quxlang/commit/b750fb4a29db0f526de2060f20b01ea5de236003); source verified 2026-11-19.**

The [benchmark scaling runner](https://github.com/Quxlang/quxlang/blob/b750fb4a29db0f526de2060f20b01ea5de236003/quxlang/benchmarks/benchmarksgame/run_scaling.py#L45-L51) and [native failure-test runner](https://github.com/Quxlang/quxlang/blob/b750fb4a29db0f526de2060f20b01ea5de236003/quxlang/tests/run_fusion_native_failure_tests.sh#L30-L35) pass the bundle path as the first argument. [qxc now requires a subcommand there](https://github.com/Quxlang/quxlang/blob/b750fb4a29db0f526de2060f20b01ea5de236003/quxlang/sources/app/qxc.cpp#L608-L628), so both scripts can stop with an unknown-command error before compiling. Add `build` before the bundle path in both callers. Static finding; neither script was executed.

Tracking key: `qxc-build-subcommand-stale-callers`.

## [current] QXR-001 Benchmark validation rejects a DEFER local BREAK

**P2 · First found 2026-10-04 · Current; last substantive verification 2026-10-04. Later resolution unverified.**

A BREAK local to a DEFER block is rejected by benchmark validation even though it does not escape the measurement scope. The validator should distinguish a local control-flow target from an escape out of the measured block.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/4746fbb28d49073ff0dd9de9e22d270750c02a25/quxlang/sources/queries/benchmark_function.cpp#L219-L220)

Fix date and commit: not verified.

Tracking key: `benchmark-defer-local-break-rejection`.

## [current] QXR-002 STATIC_WHILE can let control flow escape benchmark measurement

**P2 · First found 2026-10-04 · Current; last substantive verification 2026-10-04. Later resolution unverified.**

STATIC_WHILE falsely treated as runtime target; BREAK/CONTINUE can escape MEASURE and skip recording. Static while is compile-time expansion, so BREAK legitimately targets outer BENCHMARK_LOOP; validator should reject escape.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/4746fbb28d49073ff0dd9de9e22d270750c02a25/quxlang/sources/queries/benchmark_function.cpp#L167-L171)

Fix date and commit: not verified.

Tracking key: `benchmark-static-while-escape`.

## [current] QXR-003 Big integer quotient estimate can wrap before checking overflow

**P2 · First found 2026-10-04 · Current; last substantive verification 2026-10-04. Later resolution unverified.**

For quotient_i32 with numerator −8589934591 and denominator 2, the U32 quotient estimate increments past its maximum and wraps to zero before the overflow check. The range check therefore sees the wrapped value rather than the out-of-range estimate.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/4746fbb28d49073ff0dd9de9e22d270750c02a25/quxlang/tests/testdata/testbundle/modules/std/sources/big_integer.qxs#L148-L153)

Fix date and commit: not verified.

Tracking key: `big-integer-quotient-estimate-wrap`.

## [current] QXR-006 Local static alignment is lost in captured snapshots

**P2 · First found 2026-10-05 · Current; last substantive verification 2026-10-05. Later resolution unverified.**

Local STATIC ALIGN metadata is lost through lambda and SERIALOID snapshot paths. Proposed source-only address-alignment assertions can pass accidentally from physical alignment; no runtime execution.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/6639a7cf9f338f2b1206ae864f23d7d113af4ca1/quxlang/include/quxlang/co_vmir_generator2.hpp#L11744-L11747)

Fix date and commit: not verified.

Tracking key: `local-static-alignment-lambda-snapshot-loss`.

## [current] QXR-007 Mimalloc override bypasses expected ASAN heap tracking

**P2 · First found 2026-10-05 · Current; last substantive verification 2026-10-05. Later resolution unverified.**

qxc unconditionally redirects C++ new and delete through mimalloc, but the pinned mimalloc configuration leaves MI_TRACK_ASAN off. ASAN therefore lacks the expected tracking for those compiler heap allocations. This finding is limited to qxc’s C++ heap: it does not establish missing stack or global checks, a gtest failure, or a TSAN defect.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/0e7865bb2bd72f33c941b9fd24abc196023f838e/quxlang/sources/app/qxc.cpp#L46-L47) · [Build configuration](https://github.com/rnichollx/quxlang/blob/0e7865bb2bd72f33c941b9fd24abc196023f838e/misc/build/cbuild_workspace.yml#L105-L116)

Fix date and commit: not verified.

Tracking key: `mimalloc-asan-tracking-disabled`.

## [current] QXR-011 Privacy negative test fails for the wrong reason

**P3 · First found 2026-10-11 · Current; last substantive verification 2026-10-11. Later resolution unverified.**

The EXPECT_COMPILATION_FAILURE privacy test calls memory_region_size, which was renamed to layer5_mapping_size. It can pass because the old name does not exist, without testing whether access to the actual helper is correctly prohibited. This is a false-positive test, not a demonstrated privacy breach.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/56035cb7d516f9ca44ecc61e220713238c20cf1c/quxlang/tests/testdata/testbundle/modules/tests/sources/main_test_23_privacy.qxs#L167-L170)

Fix date and commit: not verified.

Tracking key: `privacy-test-renamed-missing-helper`.

## [current] QXR-013 IntelliJ typed loop binding resolves to an outer name

**P2 · First found 2026-10-12 · Current, partially fixed. Untyped loop bindings were corrected in [25d6c916](https://github.com/rnichollx/quxlang/commit/25d6c916551a0036ff153440604806b4f5e644ab) on 2026-10-15 at 00:35:10 UTC; typed bindings remained affected when rechecked 2026-10-15 at 22:57:30 UTC.**

The original plugin did not register or suppress VALUE, ITEM, INDEX or ITER loop bindings, allowing a same-name global to win navigation inside the loop body. The partial fix recognizes untyped bindings, but typed bindings still fail the end == i + 2 guard in QuxlangSymbols.java, lines 381–382. The typed case remains part of this original issue.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/32a588301a0aed4eaaaaaff1182fe8f1c4c61940/idea/syntax_plugin/src/main/java/dev/quxlang/intellij/QuxlangSymbols.java#L191-L198)

Full fix date and commit: not verified. The partial fix is recorded above.

Tracking key: `intellij-loop-binding-outer-resolution`.

## [current] QXR-015 Checked integer conversion omits exceptional destructor dependencies

**P2 · First found 2026-10-15 · Current; last substantive verification 2026-10-15. Later resolution unverified.**

A checked I16-to-I8 narrowing conversion can require an exceptional cleanup destructor that dependency discovery never instantiates. A concrete trigger is a default-constructed local owner with primitive fields and a custom destructor, followed by the checked conversion and an unconditional LOOP DO block with no normal exit. Constructor scan sees output before alive (state_engine.hpp447–469); co_gen_defer_dtor disabled (generator4022–4034); co_generate_dtor_references15950–15971 only populates map. Checked iconv lacks .overflow and is classified wraparound, excluded from exceptional cleanup discovery. LLVM8011–8030 emits unwindable failure and landing-pad destructor. Scanner does not instantiate every destructor-map entry. Ordinary caller-owned argument or LOOP WHILE(TRUE) false edge can mask the issue; use local owner and LOOP DO.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/e205d22675eef2dac91934588d3e25e67a97859a/quxlang/sources/vmir2/routine_requirements.cpp#L706-L715)

Fix date and commit: not verified.

Tracking key: `checked-iconv-exceptional-destructor-dependency`.

## [current] QXR-016 Instantiated generic signatures lose template bindings

**P2 · First found 2026-10-15 · Current; last source verification 2026-10-31 at 06f3c2c3 (static inspection, not executed).**

An inherited signature in an instantiated generic can lose its T binding. For example, reader#I32 with a .read function returning T can fail to resolve T or resolve an unrelated outer alias. type_parent(instance) returns inst.temploid, while subtag_binding requires the instanciation_reference. The earlier generic::\_\_INTERFACE lookup retained that instance context.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/e205d22675eef2dac91934588d3e25e67a97859a/quxlang/sources/queries/generic_interface.cpp#L72-L83) · [Current affected lines](https://github.com/rnichollx/quxlang/blob/06f3c2c33ebf55876ce54d07820938c4c97f031d/quxlang/sources/queries/generic_interface.cpp#L72-L83) · [Instance context discarded](https://github.com/rnichollx/quxlang/blob/06f3c2c33ebf55876ce54d07820938c4c97f031d/quxlang/sources/manipulators/typeutils.cpp#L1222-L1225) · [Binding requires instance context](https://github.com/rnichollx/quxlang/blob/06f3c2c33ebf55876ce54d07820938c4c97f031d/quxlang/sources/queries/subtag_binding.cpp#L7-L12)

Fix date and commit: not verified.

Tracking key: `instantiated-generic-signature-context`.

## [current] QXR-017 Large floating type layout calculation overflows

**P2 · First found 2026-10-15 · Current; last substantive verification 2026-10-17. Later resolution unverified.**

The parser accepts a 64-bit floating width, but layout calculation still uses signed int sz and sz \* 8. For SIZEOF(F4294967296E8), this multiplication overflows when sz reaches 268435456, before reaching the expected x64 size of 536870912 bytes. Undefined behavior could produce an incorrect size or a hang; the exact symptom remains unverified. This is an incomplete widening fix, not a regression of previously defined input: the old parser also overflowed. The same issue remained in FLOAT(4294967296,8) at 92deb762.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/e205d22675eef2dac91934588d3e25e67a97859a/quxlang/sources/queries/class_placement_info.cpp#L146-L160)

Fix date and commit: not verified.

Tracking key: `numeric-width-float-layout-overflow`.

## [current] QXR-018 Default LOOP step uses invalid raw constant construction

**P2 · First found 2026-10-15 · Current; last substantive verification 2026-10-17. Later resolution unverified.**

With existing mutable I32 start/end variables, VALUE(i MUT& I32) keeps the reference type as sequence_type. Omitted BY emits load_const_int directly into a reference-typed VMIR local; LLVM lowering attempts an invalid IntegerType downcast of PointerType.

Among 24 helper calls in the full generator, only omitted LOOP BY takes an unrestricted type. Other callers use BYTE, uintptr or validated UINTANY of at least 16 bits. F32, F64 and custom numeric-protocol structs are also affected; non-native floating storage represented as integers can produce wrong bits rather than a cast failure. Unsupported types may be semantically rejected first.

Invalid VMIR operand/type combination and invalid backend downcast; an assertion is expected with checked LLVM. No execution, emitted LLVM IR verifier failure, or runtime dangling-reference undefined behavior was demonstrated. Earlier semantic rejection remains possible. This narrows the finding and corrects the prior verifier-rejection claim; it does not withdraw the finding.

Suggested correction: Create numeric literal 1 and construct the target using the same path as explicit BY 1. Preserve qualifiers; reject an invalid reference default semantically instead of blindly applying remove_ref. Guard the raw helper by language int_type/BYTE, not merely LLVM representation.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/e205d22675eef2dac91934588d3e25e67a97859a/quxlang/include/quxlang/co_vmir_generator2.hpp#L10973-L10994)

Fix date and commit: not verified.

Tracking key: `typed-sequence-reference-default-step`.

## [current] QXR-019 ARG constructor selection reads an absent defaulted OTHER

**P2 · First found 2026-10-16 · Current; last substantive verification 2026-10-16. Later resolution unverified.**

A valid constructor with @ARG I32 and @OTHER I32 DEFAULT(0), invoked as item(source), supplies ARG and THIS but no explicit OTHER. Candidate initialization supplies the default, yet selection reads OTHER from the original input map with .at("OTHER"). That entry is absent, leading to std::out_of_range before the copy-specific type comparison. OTHER is I32 rather than the owner type, so this is not an invalid copy-constructor signature. The unchecked access predates the commit, but newly legal ARG/default-OTHER combinations expose it. Guard actual supplied parameters before reading their types; the exact compiler diagnostic was not executed.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/efb75286e41757c466eb2e0a027cb8a02ef68fc0/quxlang/sources/queries/functum_select_function.cpp#L254-L263)

Fix date and commit: not verified.

Tracking key: `arg-constructor-default-other-selection`.

## [current] QXR-020 Throwing destructor lowering retains invalid dead slot metadata

**P2 · First found 2026-10-16 · Current; last substantive verification 2026-10-16. Later resolution unverified.**

Consider Outer containing an Inner field whose destructor is declared NOEXCEPT_IF(FALSE), even with an empty body. Generated field cleanup marks the slot dead but retains its nontrivial destructor metadata. Outer’s implicit noexcept contract takes the unwindable call path, whose exception-exit state validation then asserts because the dead slot still has that metadata. No actual runtime throw is needed to reach this compiler assertion. The reported failure applies to assertion-enabled compiler builds; NDEBUG removes this assertion.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/efb75286e41757c466eb2e0a027cb8a02ef68fc0/quxlang/sources/llvm-backend.cpp#L5617-L5620)

Fix date and commit: not verified.

Tracking key: `llvm-throwing-destructor-dead-slot-metadata`.

## [current] QXR-022 Noexcept trait source locations break constructor pairing

**P2 · First found 2026-10-16 · Current; last substantive verification 2026-10-16. Later resolution unverified.**

strip_source_locations does not recurse through the new expression_is_noexcept_operation node. Matching full-object and subobject constructors of a VIRTUAL_POLYMORPHIC type, each using ENABLE_IF(IS_NOEXCEPT_COPYABLE([1]I32)), retain different source locations on their array-count literals. Those otherwise matching conditions compare unequal and fail constructor pairing. Include the new trait in the existing recursive normalization visitor. The exact emitted diagnostic remains unverified.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/efb75286e41757c466eb2e0a027cb8a02ef68fc0/quxlang/sources/manipulators/typeutils.cpp#L2398-L2400)

Fix date and commit: not verified.

Tracking key: `noexcept-trait-source-location-normalization`.

## [current] QXR-023 Overaligned exception storage panics instead of using OOM fallback

**P2 · First found 2026-10-17 · Current at [1d5242ab](https://github.com/rnichollx/quxlang/commit/1d5242aba500675729763b9987e62098cb22fe96); source reverified 2026-10-26.**

Throwing a valid STRUCT ALIGN(8192) reaches allocator_allocate_static, which panics for alignment above 4096 bytes. The exception-storage caller instead expects a null allocation result so it can use its permanent out-of-memory exception frame. The earlier guard returning null for unsupported alignment was removed. Restore that early guard so this case follows the intended exception-allocation failure path.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/92deb7625c34bae863966463bcb95a77f5e01b2b/quxlang/tests/testdata/testbundle/modules/runtime/sources/exception_memory.qxs#L18-L19) · [Current exception caller](https://github.com/rnichollx/quxlang/blob/1d5242aba500675729763b9987e62098cb22fe96/quxlang/tests/testdata/testbundle/modules/runtime/sources/exception_memory.qxs#L18-L19) · [Current alignment rejection](https://github.com/rnichollx/quxlang/blob/1d5242aba500675729763b9987e62098cb22fe96/quxlang/tests/testdata/testbundle/modules/runtime/sources/memory.qxs#L236-L245)

Fix date and commit: not verified.

Tracking key: `overaligned-exception-storage-panic`.

## [current] QXR-024 Benchmark harness still searches old DOC markers

**P2 · First found 2026-10-23 · Current at [cf1d9ae](https://github.com/rnichollx/quxlang/commit/cf1d9ae9bef50c4d5d046c1385e774b9544176c1); source reverified 2026-11-10.**

Standalone benchmark harness searches obsolete ::BENCHMARK_CLOCK DOC and ::BENCHMARK_OPERATION_COUNT DOC markers. Prefix DOC migration makes FIND return -1 and SUBSTRING abort before deterministic-clock validation. No default CTest registration found; not an ordinary CI failure claim. Update the extraction boundaries for prefix DOC and reject missing markers before taking substrings.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/8dab881511ed1181c6fbd416a7bfcf83b5087649/quxlang/tests/benchmark_integration.cmake#L131-L134) · [Changed declaration](https://github.com/rnichollx/quxlang/blob/8dab881511ed1181c6fbd416a7bfcf83b5087649/quxlang/tests/testdata/testbundle/modules/runtime/sources/benchmark_clock.qxs#L61-L94) · [Current obsolete searches](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/benchmark_integration.cmake#L131-L134) · [Current platform clock declarations](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/runtime/sources/benchmark_clock.qxs#L9-L52) Restoring suffix-DOC parsing in 46fa6b2b does not change the prefix-DOC declarations searched by this standalone harness.

Fix date and commit: not verified.

Tracking key: `benchmark-integration-prefix-doc-anchors`.

## [current] QXR-025 NOEXCEPT calls create impossible NVALUE exception-state conflicts

**P2 · First found 2026-11-01 · Unresolved; source rechecked at [2e8e33e](https://github.com/rnichollx/quxlang/commit/2e8e33ebe65f1698469fa2f61475cb47af542d52) on 2026-11-08. Original error mechanism changed; original regression not established fixed.**

At the originally reviewed revision, within an NVALUE generator, a TRY containing a nonthrowing call before INIT and a throwing call after INIT is rejected: the first call incorrectly establishes a dead output state at the catch handler, then the real throwing call merges a full state and raises “NVALUE initialization state differs across control-flow paths.” For example, a NOEXCEPT noop; INIT := 1; then an I32-throwing call caught by the handler has only a post-initialization exceptional path. emit() nevertheless merges all invoke kinds without checking their noexcept contract. The LLVM backend correctly omits exceptional edges for noexcept calls, so this frontend rejection is inconsistent. Restrict catch-state merging to possible exceptional edges. Static source analysis; conceptual reproducer not executed.

Introduced in [9caf0473](https://github.com/rnichollx/quxlang/commit/9caf0473d17611f213ea847480764b4e40b25d29). [Affected unconditional merge lines 2840–2843](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/include/quxlang/co_vmir_generator2.hpp#L2840-L2843) · [State conflict check](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/include/quxlang/co_vmir_generator2.hpp#L581-L587) · [Backend noexcept handling](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/sources/llvm-backend.cpp#L6152-L6159)

Commit [bec50742](https://github.com/rnichollx/quxlang/commit/bec50742f76f70aa4374e38db73cc49bc09c032e), committed 2026-11-03 05:12:14 UTC, removes per-invoke NVALUE state merging into catch handlers. Source verification on 2026-11-08 confirms that change, but not resolution of the original program: catch dispatch retains the pre-TRY initialization snapshot, so INIT performed inside TRY is not carried into an empty CATCH, and a later output-initialization check can still reject the example. No executed regression result is available. Keep this issue open until the intended rollback contract and original example are resolved. [Changed invoke handling](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/include/quxlang/co_vmir_generator2.hpp#L3246-L3255) · [Catch snapshot](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/include/quxlang/co_vmir_generator2.hpp#L12987-L12993).

Tracking key: `nvalue-catch-impossible-noexcept-edge`.

## [current] QXR-026 Retrying a partially failed worker stop leaves stale stop packets

**P2 · First found 2026-11-01 · Current at [592603c](https://github.com/Quxlang/quxlang/commit/592603c90787bb86555bde3da4719fe5f8b002ad); source reverified 2026-11-14.**

On Windows, stopping a two-worker pool can successfully post one stop packet and fail the second. The new recoverable worker_stop_failed exception loses that partial progress. If the caller catches it and retries join(), the helper posts two more packets. Each old worker consumes one and exits, leaving one stop packet in the still-open context. join() permits a new pool to attach; a subsequent one-worker pool consumes the stale packet and exits immediately, stranding later I/O callbacks and potentially hanging drain/join. Persist accepted stop progress or otherwise reconcile the queue before retry/context reuse. This requires a native posting failure after partial success and a retry; no runtime failure injection was performed.

Introduced in [4b5551f8](https://github.com/rnichollx/quxlang/commit/4b5551f80c58be3c14d4b6cfef7f9f7d233de4e6). [Affected posting loop and new throw, lines 192–201](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/std/sources/async_io_windows.qxs#L192-L201) · [Stop-packet interpretation](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/std/sources/async_io_windows.qxs#L156-L160) · [Untracked retry and pool detachment](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/std/sources/async_io.qxs#L556-L565) Source recheck at cf1d9ae: [posting loop](https://github.com/rnichollx/quxlang/blob/cf1d9ae9bef50c4d5d046c1385e774b9544176c1/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_windows.qxs#L193-L205), [stop interpretation](https://github.com/rnichollx/quxlang/blob/cf1d9ae9bef50c4d5d046c1385e774b9544176c1/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_windows.qxs#L158-L164), and [untracked retry and detachment](https://github.com/rnichollx/quxlang/blob/cf1d9ae9bef50c4d5d046c1385e774b9544176c1/quxlang/tests/testdata/testbundle/modules/io/sources/async_io.qxs#L579-L594). Source recheck at 592603c: [posting loop](https://github.com/Quxlang/quxlang/blob/592603c90787bb86555bde3da4719fe5f8b002ad/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_windows.qxs#L201-L212), [stop interpretation](https://github.com/Quxlang/quxlang/blob/592603c90787bb86555bde3da4719fe5f8b002ad/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_windows.qxs#L158-L164), and [untracked retry and detachment](https://github.com/Quxlang/quxlang/blob/592603c90787bb86555bde3da4719fe5f8b002ad/quxlang/tests/testdata/testbundle/modules/io/sources/async_io.qxs#L624-L639) remain unchanged in behavior.

Fix date and commit: not verified.

Tracking key: `async-worker-stop-partial-retry`.

## [current] QXR-028 Direct coroutine benchmark retains obsolete controller arguments

**P2 · First found 2026-11-08 · Current at [71279dd](https://github.com/rnichollx/quxlang/commit/71279dda3ee01ba38eef74d60f74a3060480f266); source reverified 2026-11-11.**

The direct coroutine benchmark still declares CO_RUN and CO_COROUTINE_CALL (renamed from CO_CALL_CORT on November 11) with an ordinary @ARGUMENTS AUTO& AUTO parameter and treats it as a composite. The compiler now supplies a field pack, and argument matching rejects a pack passed to an ordinary value/reference formal. Building the benchmark through its documented runner therefore fails when its ON(execution) RUN calls are instantiated. Migrate both hooks to named pack parameters and use the pack specialization and forwarding operations. Static source analysis; reproducer or clean build not executed.

Introduced in [0ea79223](https://github.com/rnichollx/quxlang/commit/0ea792233403eb87df1a39810959cd8cd25494af). [Unmigrated controller hooks](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/benchmarks/coroutines/modules/main/sources/algorithms.qxs#L26-L39) · [Compiler creates hook argument pack](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/include/quxlang/co_vmir_generator2.hpp#L17693-L17695) · [Hook argument dispatch](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/include/quxlang/co_vmir_generator2.hpp#L17738-L17745) · [Pack/formal mismatch rejection](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/sources/queries/ensig_initialize.cpp#L253-L256) The hook-name migration did not change the obsolete argument form: [current benchmark hooks](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/benchmarks/coroutines/modules/main/sources/algorithms.qxs#L26-L39) and [current pack/formal mismatch rejection](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/sources/queries/ensig_initialize.cpp#L253-L256).

Fix date and commit: not verified.

Tracking key: `coroutine-benchmark-obsolete-hook-pack`.

## [current] QXR-029 Formatting keyword output collides with the internal writer

**P2 · First found 2026-11-08 · Current at [2e8e33e](https://github.com/rnichollx/quxlang/commit/2e8e33ebe65f1698469fa2f61475cb47af542d52); source verified 2026-11-08.**

A previously valid call such as std::fmt#"{output}"(@output 42) now fails during compilation. fmt expands user keyword fields beside the helper’s internal @output writer argument, creating duplicate @output arguments. printf has the same regression. The public formatting interface does not reserve that keyword. Preserve user fields in a separate named pack rather than expanding them into the helper’s parameter namespace. Static source analysis; reproducer or clean build not executed.

Introduced in [0ea79223](https://github.com/rnichollx/quxlang/commit/0ea792233403eb87df1a39810959cd8cd25494af). [fmt forwarding](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/tests/testdata/testbundle/modules/std/sources/format.qxs#L428-L432) · [printf forwarding](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/tests/testdata/testbundle/modules/std/sources/format.qxs#L441-L445) · [Duplicate argument rejection](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/include/quxlang/co_vmir_generator2.hpp#L3988-L3990)

Fix date and commit: not verified.

Tracking key: `format-output-keyword-collision`.

## [current] QXR-030 Coroutine descriptor rebinding imports hidden pack parameters

**P2 · First found 2026-11-08 · Current at [71279dd](https://github.com/rnichollx/quxlang/commit/71279dda3ee01ba38eef74d60f74a3060480f266); source reverified 2026-11-11.**

Re-specializing a coroutine descriptor with a nonempty named pack copies its expanded runtime parameters back into the source argument list. These include generated \_*PACK*\* names that are not declared formals. A coroutine such as accept COROUTINE(@args ...) can first be specialized with a nonempty pack and @CONTROLLER AUTO, but rebinding that descriptor to a concrete controller fails because the hidden name is rejected as an unknown argument. The standard thread-pool and C++ coroutine adapters use this two-stage rebinding. Import only the descriptor’s source parameter keys, or restrict hidden-field expansion to runtime lowering. Static source analysis; reproducer or clean build not executed.

Introduced in [0ea79223](https://github.com/rnichollx/quxlang/commit/0ea792233403eb87df1a39810959cd8cd25494af). [Hidden-field expansion](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/sources/queries/instanciation_concrete_params.cpp#L183) · [Expanded parameters reused for specialization](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/sources/queries/lookup.cpp#L493-L507) · [Unknown-name rejection](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/sources/queries/ensig_initialize.cpp#L311-L318) · [Thread-pool rebinding](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/tests/testdata/testbundle/modules/std/sources/thread_pool_coroutine.qxs#L288) The November 11 attached-member lookup change does not alter [coroutine descriptor rebinding](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/sources/queries/lookup.cpp#L492-L506); the hidden-parameter issue remains.

Fix date and commit: not verified.

Tracking key: `coroutine-named-pack-rebinding-hidden-parameters`.

## [current] QXR-031 Functum hook forwarding loses a usable member coroutine receiver

**P2 · First found 2026-11-11 · Current at [71279dd](https://github.com/rnichollx/quxlang/commit/71279dda3ee01ba38eef74d60f74a3060480f266); source verified 2026-11-11.**

The new CO_FUNCTUM_CALL route binds a member coroutine’s THIS receiver into the callable and removes it from the argument pack. A hook that forwards to the existing CO_COROUTINE_CALL adapter can no longer specialize that callable with a child controller: lookup only unwraps bindings with a void carrier, and the pack no longer contains RECEIVER for the adapter to supply as @THIS. For example, derive a controller from coroutine_call_tests::controller, add a CO_FUNCTUM_CALL hook that delegates to .CO_COROUTINE_CALL(@COROUTINE callee, @ARGUMENTS FORWARD PACK(arguments)), and call source.value() where value is a zero-argument member coroutine. Frame construction fails. The same member call without the new hook forwards RECEIVER and is supported by the existing member-call test. Preserve a receiver representation usable by both specialization and frame initialization, and add this regression case. This does not require a named-pack child parameter and is distinct from QXR-030. Static source analysis; no reproducer was executed.

Introduced in [71279dd](https://github.com/rnichollx/quxlang/commit/71279dda3ee01ba38eef74d60f74a3060480f266). [New receiver-binding route](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/include/quxlang/co_vmir_generator2.hpp#L2223-L2224) · [THIS removed from the argument pack](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/include/quxlang/co_vmir_generator2.hpp#L17755-L17760) · [Specialization only unwraps void-carrier bindings](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/sources/queries/lookup.cpp#L483-L509) · [Existing adapter specialization](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/tests/testdata/testbundle/modules/tests/sources/main_test_coroutine_calls.qxs#L5-L15) · [Existing member-call test](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/tests/testdata/testbundle/modules/tests/sources/main_test_coroutine_calls.qxs#L321-L339) · [New hook forwarding pattern](https://github.com/rnichollx/quxlang/blob/71279dda3ee01ba38eef74d60f74a3060480f266/quxlang/tests/testdata/testbundle/modules/tests/sources/main_test_coroutine_functum_calls.qxs#L71-L80)

Fix date and commit: not verified.

Tracking key: `functum-hook-bound-coroutine-receiver`.

## [current] QXR-032 RISC-V epoll records allow out of bounds kernel accesses

**P1 · First found 2026-11-17 · Current at [36b922d](https://github.com/Quxlang/quxlang/commit/36b922d704d878a4e41388b94d852a646bd17032); source verified 2026-11-17.**

On RV32 and RV64, epoll_event omits the required padding and has a 12-byte size with data at offset 4. Linux requires 16 bytes with data at offset 8. A successful epoll_wait writes four bytes beyond the declared record, and epoll_ctl reads beyond it. The epoll async backend passes one local record and decodes the misplaced data as a request pointer, creating stack-corruption and incorrect-dispatch risks when a completion arrives. Include both RISC-V targets in the padded layout and verify size, offsets and multi-record stride. Static source analysis; no reproducer was executed.

Introduced in [36b922d](https://github.com/Quxlang/quxlang/commit/36b922d704d878a4e41388b94d852a646bd17032). [Record layout](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/syscall/sources/epoll.qxs#L5-L11) · [New wait wrappers](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/syscall/sources/epoll.qxs#L281-L300) · [Local event and pointer dispatch](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_epoll.qxs#L123-L140) · [Linux epoll ABI](https://github.com/torvalds/linux/blob/v6.12/include/uapi/linux/eventpoll.h) · [RISC-V alignment rules](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc)

Fix date and commit: not verified.

Tracking key: `riscv-epoll-event-layout`.

## [current] QXR-033 RV32 io_uring mapping offsets are divided twice

**P2 · First found 2026-11-17 · Current at [36b922d](https://github.com/Quxlang/quxlang/commit/36b922d704d878a4e41388b94d852a646bd17032); source verified 2026-11-17.**

The new RV32 mmap wrapper shifts the offset right by 12, but both io_uring mapping helpers already divide byte offsets by 4096 on 32-bit targets. Constructing io_uring_processor maps its SQE region at byte offset 0x10000000: the helper converts this to 65536 page units, then the wrapper converts it to 16. The kernel therefore receives an effective byte offset of 0x10000 instead of the SQE offset. Initialization can fail or map the wrong ring region, depending on kernel validation. Keep offset units consistent between the wrapper and all callers, including the async-context backend. Static source analysis; no reproducer was executed.

Introduced in [36b922d](https://github.com/Quxlang/quxlang/commit/36b922d704d878a4e41388b94d852a646bd17032). [RV32 mmap conversion](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/syscall/sources/syscall.qxs#L993-L1005) · [Standalone helper](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/io/sources/io_uring_processor.qxs#L43-L51) · [SQE mapping](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/io/sources/io_uring_processor.qxs#L181-L184) · [Async-context helper](https://github.com/Quxlang/quxlang/blob/36b922d704d878a4e41388b94d852a646bd17032/quxlang/tests/testdata/testbundle/modules/io/sources/async_io_linux.qxs#L46-L80) · [Linux mmap2 units](https://github.com/torvalds/linux/blob/v6.12/arch/riscv/kernel/sys_riscv.c#L31-L44)

Fix date and commit: not verified.

Tracking key: `riscv32-io-uring-mmap-offset`.

# Partial fixes and unresolved verification

## [current] QXR-004 Buffered writer state is unsafe after recoverable write failure

**P2 · First found 2026-10-04 · Partially fixed in source; native correction verified at [471aea1](https://github.com/rnichollx/quxlang/commit/471aea1ef195412caa097cf2ba8c8e4ac4c01079) on 2026-11-01. JVM recovery residue is unverified, not a newly confirmed current-HEAD bug.**

A recoverable underlying write failure can leave the buffered writer in a full-buffer state. A subsequent operation can write past the buffer end or resend bytes already accepted during a partial write. Recovery needs to preserve the unwritten remainder and a valid buffer position. This describes the original failure. The native buffered-writer path now persistently advances .flushed after each successful transfer and preflushes a full buffer before appending, correcting both reported failure modes for reported native transfer progress.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/4746fbb28d49073ff0dd9de9e22d270750c02a25/quxlang/tests/testdata/testbundle/modules/std/sources/buffered_io.qxs#L35-L47) · [Previously verified failure and append paths](https://github.com/rnichollx/quxlang/blob/1d5242aba500675729763b9987e62098cb22fe96/quxlang/tests/testdata/testbundle/modules/std/sources/buffered_io.qxs#L52-L69) · [Corrected buffer state and flush/append paths](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/std/sources/buffered_io.qxs#L41-L77)

Verified native correcting commit: [471aea1](https://github.com/rnichollx/quxlang/commit/471aea1ef195412caa097cf2ba8c8e4ac4c01079), committed 2026-11-01 21:34:18 UTC; verified 2026-11-01. The added macOS regression test was inspected but not executed. JVM file_ref.write accumulates byte progress locally and assigns the result only after the loop; a Java IOException after a prefix can therefore leave buffered progress unchanged. Practical catch/retry semantics for that Java exception have not been established, so this is an investigation candidate, not a verified remaining recovery bug. [JVM transfer source](https://github.com/rnichollx/quxlang/blob/471aea1ef195412caa097cf2ba8c8e4ac4c01079/quxlang/tests/testdata/testbundle/modules/std/sources/filesystem.qxs#L267-L281). Do not mark the original generic report wholly fixed until that scope is resolved.

Tracking key: `buffered-writer-recoverable-failure`.

# Fixed findings

## [fixed] QXR-027 Clean builds still require the removed cow package

**P2 · First found 2026-11-08 · Fixed in source 2026-11-12 at 02:21:43 UTC; verified 2026-11-13 through [a4df88b](https://github.com/Quxlang/quxlang/commit/a4df88b8bae9f38608b8f845ba1d1f7d82918e7a). Clean build not executed.**

A clean build using the checked-in cbuild workspace stopped downloading or building cow while Quxlang still required RPNXCow and linked RPNX::cow. The pinned datastructures revision supplies cow\.hpp through RPNX::datastructures, without an RPNXCow package or RPNX::cow target. Commit ae89490 removes the obsolete package and target references from both the compiler and debugger UI, retaining the datastructures dependency. The reported source-level configuration mismatch is corrected; a clean build was not executed.

Introduced in [0c0df94b](https://github.com/rnichollx/quxlang/commit/0c0df94b87157213d092ac6907728b224ec49ef3). [Remaining required package](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/CMakeLists.txt#L26) · [Remaining link target](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/quxlang/CMakeLists.txt#L730) · [Workspace dependency list](https://github.com/rnichollx/quxlang/blob/2e8e33ebe65f1698469fa2f61475cb47af542d52/misc/build/cbuild_workspace.yml#L117-L131) · [Pinned dependency exports](https://gitlab.com/rpnx/datastructures/-/blob/7628c68b381cebf738eac8ad917ce56769817236/CMakeLists.txt#L45-67)

Fix: [ae89490](https://github.com/Quxlang/quxlang/commit/ae89490ad6c2bb2b1b1e708f1230d7d2e0d1f130), committed 2026-11-12 at 02:21:43 UTC. [Compiler package declarations](https://github.com/Quxlang/quxlang/blob/a4df88b8bae9f38608b8f845ba1d1f7d82918e7a/quxlang/CMakeLists.txt#L13-L26) · [Compiler link target](https://github.com/Quxlang/quxlang/blob/a4df88b8bae9f38608b8f845ba1d1f7d82918e7a/quxlang/CMakeLists.txt#L729) · [Debugger dependencies](https://github.com/Quxlang/quxlang/blob/a4df88b8bae9f38608b8f845ba1d1f7d82918e7a/quxlang-debugger-ui/CMakeLists.txt#L6-L35). Source correction verified 2026-11-13.

Tracking key: `build-cow-dependency-removed`.

## [fixed] QXR-005 Native quad precision SQRT selects an unsupported x64 libcall

**P2 · First found 2026-10-04 · Fixed in source**

Native F128E15 SQRT on x64 chooses intrinsic while LLVM22.1.6 needs unsupported libcall instead of runtime hook. Exact runtime diagnostic unverified.

**Fix date:** 2026-10-17 18:11:34 UTC. **Fix commit:** [81f8a297](https://github.com/rnichollx/quxlang/commit/81f8a2971ba76b3eb34d7e2b0fcb471453ba261c). **Source fix verified:** 2026-10-17 22:16:00 UTC.

Generator lines 4384–4402 and LLVM backend 6350–6397 use runtime fallback; hardware path limited to supported F32/F64.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/4746fbb28d49073ff0dd9de9e22d270750c02a25/quxlang/include/quxlang/co_vmir_generator2.hpp#L4376-L4378). Runtime validation remains unverified.

Tracking key: `native-f128e15-sqrt-x64-libcall`.

## [fixed] QXR-008 Buffered reader requires unsupported fixed array SIZE

**P2 · First found 2026-10-08 · Fixed in source**

buffered_reader .bytes.SIZE() on fixed [16384]BYTE lacks lowering; read_byte/read_line compilation fails, affecting regex_redux, reverse_complement and k_nucleotide. Exact diagnostic unverified.

**Fix date:** 2026-10-09 04:01:42 UTC. **Fix commit:** [db38e6bb](https://github.com/rnichollx/quxlang/commit/db38e6bb24088aa949926ccc576bf5897f294786). **Source fix verified:** 2026-10-09 23:08:58 UTC.

Adds fixed-array SIZE overloads, dispatch and lowering, two DUAL_TEST cases and documentation. The missing support described above applies to the original reviewed revision, 67e3a6f, not the corrected source.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/67e3a6f30174f786d4fbb3c97395ed5dc82d1d75/quxlang/tests/testdata/testbundle/modules/std/sources/buffered_io.qxs#L163). Runtime validation remains unverified.

Tracking key: `buffered-reader-fixed-array-size-unsupported`.

## [fixed] QXR-009 Allocator transition test assumes obsolete capacity

**P2 · First found 2026-10-10 · Fixed in source**

The allocator’s default capacity calculation gives 341 slots and rounds down to 340, but the transition test still expects the old capacity of 84. Its 169-allocation sequence and 85-entry drain checks therefore use obsolete expected values; the first actual value is 168 rather than 84. The native guard and runtime test registration are active.

**Fix date:** 2026-10-11 16:07:27 UTC. **Fix commit:** [77da2aff](https://github.com/rnichollx/quxlang/commit/77da2afffb0081a08bc324ca0bfd638d80b240aa). **Source fix verified:** 2026-10-11 23:09:19 UTC.

Replaces hard-coded expectations with a capacity-derived transition test in memory_tests, lines 181–232 at the verified fix.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/393deb7be08277cb6eaadd98bdd0099c38b6aef9/quxlang/tests/testdata/testbundle/modules/runtime/sources/memory.qxs#L1798-L1830). Runtime validation remains unverified.

Tracking key: `allocator-transition-test-obsolete-capacity`.

## [fixed] QXR-010 Mixed benchmark rejects valid non GHz counter frequencies

**P2 · First found 2026-10-10 · Fixed in source**

The mixed benchmark runner requires a 1 GHz counter frequency. A valid Darwin ARM64 environment reporting a legacy 24 MHz counter is rejected during warmup rather than having its elapsed ticks scaled to time.

**Fix date:** 2026-10-11 20:02:28 UTC. **Fix commit:** [2e4d57b6](https://github.com/rnichollx/quxlang/commit/2e4d57b67fecc427c3f3adc78bda88d9c494397d). **Source fix verified:** 2026-10-11 23:09:19 UTC.

Accepts positive counter frequencies and scales timings in run_mixed.py, lines 137–146 at the verified fix.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/393deb7be08277cb6eaadd98bdd0099c38b6aef9/quxlang/benchmarks/allocators/run_mixed.py#L105-L106). Runtime validation remains unverified.

Tracking key: `mixed-benchmark-fixed-counter-frequency`.

## [fixed] QXR-012 IntelliJ AS casts create a phantom declaration

**P2 · First found 2026-10-12 · Fixed in source**

Every word after AS is registered as an UNKNOWN declaration, shadowing the real type; subsequent constructor call loses navigation/color.

**Fix date:** 2026-10-15 00:35:10 UTC. **Fix commit:** [25d6c916](https://github.com/rnichollx/quxlang/commit/25d6c916551a0036ff153440604806b4f5e644ab). **Source fix verified:** 2026-10-15 22:57:30 UTC.

Removes the AS-triggered phantom declaration while retaining real type lookup.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/32a588301a0aed4eaaaaaff1182fe8f1c4c61940/idea/syntax_plugin/src/main/java/dev/quxlang/intellij/QuxlangSymbols.java#L157-L160). Runtime validation remains unverified.

Tracking key: `intellij-as-cast-phantom-shadow`.

# Withdrawn reports

## [withdrawn] QXR-021 Lifecycle noexcept trait excludes argument preparation

**Originally P2 · First found 2026-10-16 · Withdrawn 2026-11-21**

The report identified a `NOEXCEPT` swap overload with a throwing default argument. The trait correctly reports the selected overload as nonthrowing. Default arguments, conversions, and by-value adaptations execute in the caller before the overload begins. A `NOEXCEPT` wrapper that performs those operations is responsible for its own exception contract. The argument-preparation check introduced in `66800906` was reverted.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/efb75286e41757c466eb2e0a027cb8a02ef68fc0/quxlang/sources/queries/type_operation_is_noexcept.cpp#L24-L33)

Tracking key: `noexcept-trait-argument-preparation`.

## [withdrawn] QXR-014 Ordinary atomic integer swap rejection

**Originally P2 · First found 2026-10-13 · Withdrawn 2026-10-14 at 01:42 UTC**

The October 13 report treated rejection of ordinary swap between two ATOMIC#I24 values as a regression. Ryan clarified that ordinary atomic integer swap is unsupported by design. The rejection is therefore intended, not a bug requiring a fix; EXCHANGE is a distinct operation. No fixing commit or fix date applies.

[Original source and affected lines](https://github.com/rnichollx/quxlang/blob/12f16bd9ac8aa32e11594f0a48e771c9bc1426f2/quxlang/sources/llvm-backend.cpp#L9177-L9179).

Tracking key: `atomic-widened-ordinary-swap-storage-mismatch`.

# Earlier memory investigations

First reported **2026-09-30 at 21:03:36 UTC**. These four mechanisms were identified in source, but their memory contributions were not measured and no ownership cycle leak was established. Status: **investigation, not a confirmed defect**; later resolution and fix dates are unverified. They are excluded from the current and fixed bug counts.

- **QXM-001 Cached intermediate representations.** Graph-lifetime AST, symbol and VMIR copies can remain retained. [VMIR cache source](https://github.com/rnichollx/quxlang/blob/823e12ef5b28ff7ce1d14de4f35476e542e81a61/quxlang/sources/queries/vm_procedure3.cpp#L12-L26).

- **QXM-002 Completed coroutine frames.** Completed frames are retained by `m_cort` with `final_suspend suspend_always` and dependency sets. Ordinary coroutine locals unwind; this is not a claim that every local remains retained. [QueryGraph source at the reviewed dependency revision](https://gitlab.com/rpnx/querygraph/-/blob/7c0cbbc6d410332bc2c2055128ba5c24329e3a2e/include/rpnx/querygraph/querygraph.hpp#L1314-1337).

- **QXM-003 Quick optimization duplication.** Per-root depth-two callee imports duplicate IR, while cached stages and parse plus CloneModule can coexist during optimization. [Imports](https://github.com/rnichollx/quxlang/blob/823e12ef5b28ff7ce1d14de4f35476e542e81a61/quxlang/sources/queries/llvm_preoptimize.cpp#L443-L501) · [Cloning](https://github.com/rnichollx/quxlang/blob/823e12ef5b28ff7ce1d14de4f35476e542e81a61/quxlang/sources/llvm-backend.cpp#L368-L432).

- **QXM-004 Constexpr allocation retention.** Deallocation marks allocations freed while vectors retain strong owners until evaluation ends. [Interpreter source](https://github.com/rnichollx/quxlang/blob/823e12ef5b28ff7ce1d14de4f35476e542e81a61/quxlang/sources/ir2_constexpr_interpreter.cpp#L3158-L3220).

Serialized logical-size statistics are not RSS or a strict memory lower bound: they can omit frames, container capacity and LLVM working state, and can count shared values repeatedly. LLVM contexts use RAII. No measured allocation totals or runtime leak diagnosis were established by this review.

# Status maintenance

Keep stable issue IDs and first-found dates when an issue changes status. Record a partial fix without closing a remaining failure mode. Mark an issue fixed only when the correcting commit and relevant source behavior have been checked; retain both the commit date and the separate verification date. Preserve withdrawn reports and earlier qualifications instead of silently deleting them. Record new uncertainty or a reopened regression explicitly.

This record includes all recovered reported findings from the daily reviews through 2026-10-23 and the four September 30 memory investigations. Older open issues need individual revalidation before their status can be asserted at a newer HEAD. Exact discovery times for the earliest October reports were not retained, so those use date-only first-found fields.
