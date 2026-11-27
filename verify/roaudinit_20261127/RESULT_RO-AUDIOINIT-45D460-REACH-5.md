# RESULT RO-AUDIOINIT-45D460-REACH-5 (run 56, G2): not met, stop moved from 0x005B8910 to 0x005C78C0 (5 functions still unbound before the 0x00604604 write, 1 of them reached by the port)

Branch `orch/RO-AUDIOINIT-45D460-REACH-5`, worktree `.worktrees/RO-AUDIOINIT-45D460-REACH-5`, created from orch/integration
`affd19dd4`. The worker ran on claude3, Opus 5.5 high (moved from claude2 at the start because the row needs Ghidra MCP).
Row kind: race-path binding row. It ran both builds, the ddkobj check (in the object build), the knob boot, the binder
selftest and the RO arm. No G-CTRL, no B, no 12 captures.

## Verdict: RED (G2 not met, decision B)

**0x00604604 was not compared with ref1 at record 0.** On the object build, the `RO_ROAUDINIT-pr1` environment now ends
with an execute access violation at **0x005C78C0** after about 3 s (2 runs). The call comes from
`_PortTop_Bound_005a6d60+0x21` (ddk map 0x00270341), inside `_PortTop_Bound_005a71f0+0x14a`, inside 0x0045d460. The RO arm
itself (`run_arms.py`) exits 0xC0000005 after 2 s with 0 ticks (`ticks.bin` sha8 e3b0c442). There is no record 0, so
`detorig_firstdiv.py` was not run.

**Where the stop moved, measured.** Entry counters on the bound bodies (`count_av.py`, Frida `Interceptor` on the ddk
map symbols, 2 runs, the same both times) at the fault:

| rva | port calls before the fault | original, RO-AUDIOINIT-DEPTH (before + in window) |
|---|---|---|
| 0x005b8910 | 28 | 1 + 27 = 28 |
| 0x005b8c40 | 28 | 0 + 28 = 28 |
| 0x005c6f50 | 28 | 0 + 28 = 28 |
| 0x005c6bb0 | 28 | 0 + 28 = 28 |
| 0x005aeb90 | 28 | 0 + 28 = 28 |
| 0x005a71f0 | 1 | 0 + 1 |
| 0x005a6d60 | 1 | 0 + 1 |
| 0x005a65d0 / 0x005a66c0 / 0x005a7420 | 0 / 0 / 0 | 0 + 200 each |
| 0x005aa560 | 60 | 5 + 255 |
| 0x005b8680 | 1 | not in the window table |
| 0x004ccef0 | 899 | 220 + 34 (the port also calls the RW memory slot outside this lane; not comparable) |

0x005b8910 (the REACH-4 stop) and its callees now run with the original's call counts. The port faults at the 0x005a6d60
table call before 0x005a65d0 / 0x005a66c0 / 0x005a7420 run.

**The new stop, 0x005c78c0.** It has no Ghidra function, no `re/console/pc_funcs.csv` extent, no hooks.csv row and no source.
The listing of `MASHED.exe.unpatched` (`listing_005c78c0.txt`) runs from 0x005c78c0 to the `ret` at 0x005c792c (109 B).
It reads 0x007de1b0 / 0x007de1b4 and calls 0x005a6dc0 in a loop (0x005c78e4). It stores `*(arg3+4)` / `*arg3` at
0x007de1b0 / 0x007de1b4 (0x005c7902 / 0x005c790a), or 0 into both (0x005c7918 / 0x005c7922), and returns arg1. Its address
is stored in .data at **0x006350cc** and **0x00636024** (byte search of the exe). Neither slot is in
`re/tools/image/CODEPTR_IMAGE.tsv`, so the image code-pointer fix-up leaves the original address there and the port calls
0x005c78c0 directly. [UNCERTAIN] which of the two slots 0x005a6d60 read (it reads `*(param_1[1]+4) + index*8`). The test
is a `--watch-write` or a read probe on both slots in the same arm. 0x005c78c0 is outside DEPTH's 52: it has no counter,
which is blind spot 2 of RO-AUDIOINIT-DEPTH.

**Functions still unbound between the new stop and the 0x00604604 write: 5.**
- 1 reached by the port: 0x005c78c0 (no row, 109 B).
- 4 not reachable from a bound body: 0x004a4554 `__heap_alloc`, 0x004a45cf `__nh_malloc`, 0x004a59bf `CrtSehEpilog` and
  0x004a5984 `CrtSehProlog`. They are CRT internals under 0x004a45fb `_malloc`, which every profile reuses as
  `MashedCrt_malloc_004a45fb` (Match/PortTop_CrtHeap.cpp). No bound body calls them.
- Of DEPTH's 13 not-bound functions, 9 are bound now (section 1). `_pow` 0x004a2d30 is bound by an alias line.
- Not counted: Ghidra-missed functions with no counter. 0x005c78c0 is one, so the count can rise.

## 0. First step: the two D: worktrees (cleanup hook, first end-to-end run)

`Get-ChildItem <wt> -Recurse -Force -Attributes ReparsePoint -Depth 3` found no reparse point in either worktree, so
nothing was unlinked. `py -3.12 scripts/diag.py wt-remove` printed (`wt_remove.out`):

```
wt-remove: D:\MashedWT\RO-AUDIOINIT-DEPTH_wt: removed
wt-remove: root D:\MashedWT\RO-AUDIOINIT-DEPTH_root: refused 0.000 GB: root written 0.0 d ago (< 7.0)
wt-remove: D:\MashedWT\HB-MASHEDWT-ROOT-CLEANUP_wt: removed
```

Each call also printed `diag.py:439: SyntaxWarning: invalid escape sequence '\M'` (a docstring with `D:\MashedWT`). That is
harmless, but the docstring wants a raw string.

- Roots deleted: **none**. The hook refused `RO-AUDIOINIT-DEPTH_root` because it was written less than 7 days ago. No root
  line was printed for HB-MASHEDWT-ROOT-CLEANUP (that row has no `_root`).
- D: free space went from 552.71 GB to 559.93 GB to **567.14 GB, +14.43 GB**. All of it is the two worktree checkouts, 0 GB
  from roots. Other sessions write to D: too, so the figure is gross.

## 1. Binds (what changed)

| rva | source | how | evidence |
|---|---|---|---|
| 0x005b8910 | re/match/s27/005b8910.c (new, REACH-4's draft) | c1_add, PROVISIONAL.tsv P3, `prov_bind` | 71.0%; equiv NOT-PROVEN branch-condition (operand order of one compare); calls checked against the listing (0x005b8954..0x005b895f, 0x005b897b..0x005b8995, 0x005b89c0, both 0x005b8c40 sites) |
| 0x005b8c40 | re/match/s27/005b8c40.c (**rewritten**) | PROVISIONAL.tsv P3, `prov_bind` | the MATCH-S27 h1 source (30.0%) declared the record a `float`, read the device pointer before loading it, and dropped the arguments of 0x005aeb90 / 0x005bc020 / 0x005c75b0. It was rewritten from the listing 0x005b8c40..0x005b92cb. 26.2% bytes / 74.6% ops; equiv NOT-PROVEN ret-memory at the early return (+0x88 stored from the +0x84 register, 0 on that path) |
| 0x005c6f50 | re/match/s28/005c6f50.c (new) | c1_add, PROVISIONAL.tsv P3, `prov_bind` | 91.1%; equiv NOT-PROVEN call-args (stack temp under the no-argument call 0x005ab040) |
| 0x005a65d0 | re/match/s25/005a65d0.c (existing C2) | PROVISIONAL.tsv P3, `prov_bind` | 84.6%; checked against the listing 0x005a65d0..0x005a663f; equiv NOT-PROVEN ret-memory |
| 0x004ccef0 | re/match/s20/004ccef0.c (new) | PROVISIONAL.tsv P3, `prov_bind` | the DDK and rw9044 builds emit a tail `jmp` for every flag set tried (/O1, /O2, /Ox, /G6, /Oy-, no /Og); equiv NOT-PROVEN call-regargs (eax only). The hand-body row `RwCoreHand_004ccef0` stays: a second, re/match-keyed row (ddkobj `load_provisional` keys by (rva, source)) |
| 0x005a66c0 | re/match/s25/005a66c0.c (new) | c1_add, match promotion | BYTE-IDENTICAL 15 B, relocheck OK 0/0, equiv EQUIV |
| 0x005a7420, 0x005c6bb0 | existing C4 match | bound by the planner (callees of the new binds) | BYTE-IDENTICAL, relocheck OK 4/0 and 2/0 |
| 0x005aeb90 | re/match/s26/005aeb90.c (C4 frida-replay) | `force 005aeb90` (instead of reusing the hand body `AudioLinearToLog`, as BIND-AUDIO-ONE-GENERATOR did for 0x005b10a0) | PROV-INSITU-REF1 PROMOTIONS.tsv line 34: IDENTICAL-FOR-600, same source file |
| 0x004a2d30 `_pow` | `RwCoreCrt_pow` (RwCore/RwCore_Crt.cpp) | `alias` line in state/audio/rules.tsv | CRT C4 libid, exe CRT body (called by 0x005b8c40 at 0x005b91c2) |

- equiv verdicts come from `re/tools/match/equiv.py` at tool commit 1ed7ceb1a (the merged one). Each P3 row carries its
  verdict line. One register line was added for the five P3 bodies (re/orchestrator/UNCERTAIN_REGISTER.md).
- hooks.csv changed only through `re/tools/c1_add.py`: 3 rows (0x005b8910, 0x005c6f50, 0x005a66c0), none -> C1. The
  CHANGELOG lines were written by the tool.
- Promotions: `re/promotions/RO-AUDIOINIT-45D460-REACH-5.tsv`, 1 row (0x005a66c0, route match). `c4_reclass.py batch --tsv`
  dry run: **1 pass, 0 refused, 0 skipped**. No `--apply`.
- `state/audio/rules.tsv`: 5 `prov_bind`, 1 `alias`, 1 `force`, with comments. The 0x00551460 line (RACE-VFS-COURSELUA) is
  not in this file on the head and was not touched. No other profile's state changed.
- Generated, by the tool only: `porttop_bind.py write --profile audio` (bind 450 -> 459, reuse 82 -> 85, stop 79 -> 74),
  `sync --profile audio` (rsp +9, allowlist +3), `bindings --profile audio` (updated 6, added 7), `externs` (0 unguarded),
  `samesrc --profile audio` (4, all pre-existing: 0x00407600 / 0x00411350 / 0x0044b000 / 0x0047d150), and
  `refs --profile raceload`. The last was needed because 0x005c6b40 now has a bound caller (0x005c6f50), so the
  `Match/PortTop_BulkRefs.cpp` keep-alive list lost one entry.
- `prov_drop.py` (dry run): PROVISIONAL.tsv rows 989, DROP 0.

## 2. Binding numbers

- **Executing or reachable, new in this row:** 9 binds. 6 are measured executing on the knob-on RO environment:
  0x005b8910, 0x005b8c40, 0x005c6f50, 0x005c6bb0 and 0x005aeb90 28 calls each, 0x004ccef0 899 calls. 3 are bound but not
  reached before the stop: 0x005a65d0, 0x005a66c0, 0x005a7420 (0 calls each). There are 3 new reuse lines: 0x004a2d30
  (`RwCoreCrt_pow`), 0x005c75b0 (`AudioVoiceField8cGet`) and 0x005c6b40 (`PortTop_Bound_005c6b40`).
- **Measured execution count of the audio body on a knob-on arm:** 0x005b8910 runs **28** times (the original runs it 28
  times). No race frame body executed: 0 ticks.
- **Pre-bound keep-alive (`Match/PortTop_BulkRefs.cpp`):** 126 caller-less bound bodies (head 127, foreign 92 -> 91).
- **Bindable left unbound, with reason:**
  - 0x005a9280: a named stop (C2 source). It is not executed in the original's window (no window row).
  - 0x004a4554 / 0x004a45cf / 0x004a59bf / 0x004a5984: CRT internals of the reused malloc body.
  - 0x005c78c0: the new stop (no row or source; its slot is not in the image list).

## 3. Object build (ddkobj check, in the object build)

**IDENTICAL 2363/2530, DIFFERENT 79, not-in-map 88**, units linked 2409/2703, refusals 34
(externalized-static-register-convention 13, wrapper-redefines-globals 21), provisional kept VS2022 260 units + 7 fns in
linked units, dual body 33.

Against `log\orch\run56_headcheck\` (`cmp_head1.txt`):
- IDENTICAL +3: 0x005a66c0, 0x005a7420, 0x005c6bb0.
- DIFFERENT +1: **0x005aeb90**, classified as a **replay-C4 body**: c4=frida-replay, and its source is the replay-verified
  file re/match/s26/005aeb90.c (PROV-INSITU-REF1 PROMOTIONS.tsv line 34, IDENTICAL-FOR-600). 49 differing bytes.
- not-in-map, refusals and dual body: unchanged.
- Provisional units +5: the five new P3 wrappers (`skip:provisional`).
- **Listed unit -> 100% neighbour that fell back to its VS2022 body: none.** Provisional functions in linked units stay at
  7 = 7, the same list as the head, and each new wrapper binds one rva.

rva-lint (VS2022 build): 661 findings, all allowlisted, **NEW=0**, ORIGTOP-ONLY 648, ORIGTOP-HELD 9 (head 658 known). The
+3 allowlist lines are DUP-IN-TARGET 0x004ccef0 (`RwCoreHand_004ccef0` on the RWC knob) and 0x005aeb90 (`AudioLinearToLog`
on the hand path), and CROSS-TARGET 0x005a7420 (`Util/PromoLoop_sessionB.cpp`).

## 4. RULE 9: match.py --bytes and relocheck on every source the exe now includes

(`rule9_match_reloc.txt`)
- **match (byte-identical + relocheck OK): 3.** 0x005a66c0 (new promotion, 15 B, 0/0), 0x005a7420 (64 B, 4/0) and
  0x005c6bb0 (36 B, 2/0).
- **replay-C4: 1.** 0x005aeb90 (35 differing bytes, relocheck BAD 2/1 positional, `__ftol` unchecked).
- **provisional (neither): 5.**
  - 0x005b8910: 482 differing bytes, relocheck BAD.
  - 0x005b8c40: 1340 differing bytes, relocheck BAD.
  - 0x005c6f50: 57 differing bytes, relocheck OK 3/2.
  - 0x005a65d0: 49 differing bytes, relocheck OK 1/0.
  - 0x004ccef0: 1 differing byte (tail jmp), relocheck BAD.
  - Every BAD relocheck lists the same named targets on both sides at shifted offsets (layout), not a different callee.

## 5. Builds, self-tests, arms

- Every build, selftest and arm ran after `mashed_lock.py status` (free, or PRIORITY-LINGER only). C: had 43.7 GB free
  before the build and 65.4 GB before the arms. test-slot was free (3 of 8 slots held by SimGolf).
- Both builds rc 0: VS2022 254 s, object build 1002 s.
- **Binder selftest (RULE 7): PASS, 0 failures, every profile** (`bind_selftest2.txt`):
  - audio plan IDENTICAL (bind 459 reuse 85 stop 74).
  - raceload refs IDENTICAL.
  - bulk, raceframe, renderframe, gates and BINDINGS.tsv IDENTICAL.
  - The first run (`bind_selftest1.txt`) gave FAIL 1, the stale `PortTop_BulkRefs.cpp`. It was fixed with `refs`.
- **Knob-on arms** (REACH-3's set, `audiotick_stops_arms.py vs_init_audio ddk_init_audio`) and the **RO arm**:
  - **Pass 1** (`pass1/`): both builds faulted executing 0x00000000 in `_PortTop_Bound_005a5f60+0x1a`, which reads table slot
    0x00632ba8 = 0. Cause: the private root had no `original\mashed_image.bin`. `scripts/mashedwt_cleanup.py prep` copies
    `original\` from the install, which does not hold that file (REACH-4's root inherited it from REACH-3's), and the log
    says `[portstate-image] REFUSED(missing)`. It was staged with `re/tools/image/extract_image.py`; the sha256 81cc7775...
    equals REACH-4's copy. This is a harness gap of the prep tool: it is named in the follow-up row, not fixed here.
  - **Pass 2**: portstate-image self-test PASS (0 of 8), OT_CODEPTR PASS (0 of 367), 107 image slots written, `[REPLAY] init
    self-test: 66 checks, 0 failed`. VS2022 faulted at 2.1 s, the object build at 2.4 s and the RO arm at 2 s, each
    0xC0000005 after `printf: numpermwaves 43`. The fault address was caught with `catch_av.py` (spawn under Frida,
    `Process.setExceptionHandler`): execute at 0x005c78c0 from `PortTop_Bound_005a6d60+0x21`, on VS2022, on the object build
    and on the RO environment. Some catcher runs saw no exception (2 of 7), so they were re-run.
  - `[code-trap] NOT ARMED: range 0x00401000..0x005CC000 is outside the B17 pad 0x0041ABE0..`. This was already so in
    REACH-4's final pass (pad 0x00411C40), so stops show as raw access violations, not named code-trap stops.
- Not run: G-CTRL, B, 12 captures (race-path row), menu walk (no menu code changed), the `--watch-write` leg on +0xc (no
  time left).

## 6. Frida temp folders

`%LOCALAPPDATA%\Temp\frida-*`:
- before 6 (`frida_before.txt`), after 15, **deleted 9**, remaining 6 (the before set).
- Every deleted folder is new against the before list and was created inside one of this row's own run windows
  (`run_windows.txt`, `frida_new.txt`). There was one per catcher or counter run.

## 7. Files

Committed: this RESULT, `mk_sources5.py`, `mk_005b8c40.py`, `add_prov5.py`, `catch_av.py`, `count_av.py`, `build_both.py`,
`arms_driver.py`, `cmp_head.py`, `rule9_match_reloc.txt`, `cmp_head1.txt`, `equiv2.txt`, `av_rocount1.json`,
`wt_remove.out`.

Raw (`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-5_data/`, full sha256 in `sha256.txt` there), first 16 hex:

| file | sha256 |
|---|---|
| wt_remove.out | 0adde139857facf0 |
| build1_vs.log | 6f94bf52a2890ffc |
| build1_ddk.log | d5b4b0e4493f7ee6 |
| ddkobj_summary1.json | 5f8b7f8ca1b6a0d6 |
| BYTECHECK1.tsv | 27a0ee05294a1bd4 |
| bind_selftest1.txt | 8573822f2ebdd8af |
| bind_selftest2.txt | 403f30e04fd9b554 |
| bind_write1.txt / bind_write2.txt | 28dd86bb890abb90 / 9b5e4d24fa713f9a |
| equiv2.txt | 67eca41538c08050 |
| arms_knob.out / roarm.out | eacf864f2c3d440a / bfabf18e4449dd1e |
| vs_init_audio.log / ddk_init_audio.log | 5e28f0f28bdafa87 / 806ca4eb7f8dffe9 |
| ro/RO_ROAUDINIT-pr1/meta.json | 12c9bf2a06e10d78 |
| av_vs.json (pass 1) / av_vs3.json / av_ddk4.json / av_ro2.json | 01d99653c30f51c5 / 0af58c1a9126f10d / 0d05b0cd72502bcd / d90860f7db02257a |
| av_rocount1.json / av_rocount2.json | ce6f338abfd09ec3 / c5b929f50cca82b0 |
| listing_005b8c40.txt / listing_005b8910.txt / listing_005c78c0.txt | 402f118308f5dcbd / 7c914f9e67b37150 / eb5affdbfe2e9ddb |
| frida_before.txt / frida_new.txt / run_windows.txt | 4f3081fc22b82a80 / 2eaefc838a2b129e / b5487736b39d3259 |

## Next (new open row RO-AUDIOINIT-45D460-REACH-6)

1. 0x005c78c0: a pc_funcs extent (0x005c78c0, 0x005c792c, 109), c1_add, a source from the listing, match / equiv.
2. Its code-pointer slot or slots (0x006350cc / 0x00636024, not in CODEPTR_IMAGE.tsv). The image list is generated by
   `re/tools/image/codeptr_image.py` from the Ghidra data-reference export, so this may need its own tool or data row.
3. Then the RO arm again, for 0x00604604 against ref1 at record 0.
4. Harness, outside the binding: `mashedwt_cleanup.py prep` should stage `original\mashed_image.bin`, and the code trap is
   not armed while the B17 pad starts above 0x00401000.
5. The `--watch-write` leg on +0xc stays open.
