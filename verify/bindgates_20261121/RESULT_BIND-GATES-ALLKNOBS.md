# RESULT BIND-GATES-ALLKNOBS (run 55, binding row, top-down kind)

Branch `orch/BIND-GATES-ALLKNOBS` on `orch/integration` head `39ee297f2`. Worker cmwr1c0f60cgrp121fvvlqiyh, lead
cmwr0mi2n088jp121jgz9oroy. Rank script taken with `git checkout orch/GATERANK-ALLKNOBS -- verify/t2gaterank_20261121/t2_gate_rank.py`
(single-file checkout of 5e6ddb4d5's copy, not a cherry-pick), then edited (section 1).

Verdict: 3 of the 4 gates bound (0x00409710, 0x004103a0, 0x00448220), 0x0048ae00 left unbound with a measured reason
(section 4). 0x00440bc0 was a stale rank input, fixed in the rank script. Both builds OK, ddkobj check 0 new DIFFERENT, binder
selftest 0 new failures (RULE 7), four knob boots rc 0 after 3000 frames with self-test PASS. One RULE 4 slip (section 8).

## 1. First step: why 0x00440bc0 still ranked as a stop

The rank's input, not a stale wrapper. `t2_gate_rank.py` (line 152 of the GATERANK copy) read the raceload plan from
`verify/ro_20261116/rltools/plan_rl.json`, the pre-BIND-TOOL copy frozen at PORT-OT-RACE-BIND-PROV4 (260 rvas), where
0x00440bc0 is `stop, cpp-unclean: C2632`. The live raceload plan is `re/tools/bind/state/raceload/plan.json` (411 rvas), where
0x00440bc0 is `bind re/match/tlr/00440bc0.c match` (C unit `Match/PortTop_00440bc0.c`, PORT-OT-RACE-BIND-HANDLER). 157 rvas differ
in state between the two files. Fix (input): the rank reads `re/tools/bind/state/raceload/plan.json`. With that alone, 0x00440bc0
and 0x005a7b60 (rank 1 in GATERANK) leave the ranked gates (`rank_fixinput_head.txt`).

A second input fix: `EXTRA_PLANS` was the fixed tuple (bulk, raceframe, renderframe); it now globs every
`re/tools/bind/state/*/plan.json` other than audio / raceload, so a new profile (this row's `gates`) is read.

Both edits change the default output as well (the default also read plan_rl.json); the GATERANK RESULT's "default unchanged" no
longer holds after this fix, by design. 0x00440bc0 does not join this row (nothing to regenerate).

## 2. What was bound and how (data only, porttop_bind.py untouched)

New profile `re/tools/bind/state/gates/` (profile.json, rules.tsv, roots.txt, cpp_unclean.txt, generated plan.json, newfiles.txt):
roots only (a matched callee that is not a root is a named stop, `roots_only_reason`), c_emit on, static_stop on, prov_bind
00448220, three alias lines copied from raceload / bulk rules (piz mount / unmount 0x00495280 / 0x004952f0, 0x0040bb30
`Search40bb30`). Stops: `Match/PortTop_GatesStops.cpp` (`PortTop_GTStop_`, 11 stops).

| rva | body | caller now bound to it | how |
|---|---|---|---|
| 0x00409710 | `Match/PortTop_00409710.c` (C unit, `re/match/s01/raw/00409710.c`, c4=match) | 0x00426e10 (raceload root) | `gates` binds it as a C unit (cpp-unclean C2632, line copied into `state/gates/cpp_unclean.txt`); raceload reuses it. Its raceload `c_hold.txt` line was deleted: the file's own header says "An rva leaves this list when the row that owns it binds it (delete its line, then `write`)", and this row owns 0x00409710 |
| 0x004103a0 | `Match/PortTop_004103a0.cpp` (`re/match/s02/004103a0.c`, c4=match) | 0x004111c0 (raceframe root) | the source had `static void FUN_004103a0`, which raceframe's static_stop turned into a named stop. Source edit: the MATCH_STATIC block (`#ifdef __cplusplus` empty, else `static`, the form 30 re/match sources use) and `MATCH_STATIC void FUN_004103a0`. Under the DDK it stays static: match.py --bytes BYTE-IDENTICAL 360 B, relocheck OK 32/0. Src sha256 7b1639eda2acb4ac -> ddcd252548112065: `re/promotions/BIND-GATES-ALLKNOBS-refresh.tsv`, `c4_reclass.py refresh --tsv` dry run: 1 would-refresh, 0 refused |
| 0x00448220 | `Match/PortTop_00448220.cpp` (`re/match/s08/00448220.c`, L-126 provisional body) | 0x0040d470 (raceframe bind) | `prov_bind 00448220` in `state/gates/rules.tsv` (PROVISIONAL.tsv row status `provisional`). Not C4 |

Regenerated with the tool on this tree (rule 3): raceload (`write --roots-only`, its standing mode; plan change = 0x00409710 stop ->
reuse only; wrapper `PortTop_00426e10.cpp` one define), raceframe (`write`; 0x004103a0 and 0x00448220 stop -> reuse;
`PortTop_004111c0.cpp`, `PortTop_0040d470.cpp` one define each), the two stop files, `PortTop_BulkRefs.cpp` (byte-identical to the
head after the final state: 129 keep lines). renderframe and bulk regenerated with 0 files changed. audio not regenerated (not
this row's to touch). `sync --profile gates`: exe_sources.rsp +3 TUs + stop file, allowlist +1 line
(`ORIGTOP-ONLY DUP-IN-TARGET:004103a0:Match/PortTop_004103a0.cpp,Race/SubStateMachine.cpp`, the hand body in the default path).
`externs --profile gates`: 0 sources with unguarded extern data. `samesrc --profile gates`: 0.
No moved.tsv lines: no other profile's moved.tsv applies (bulk's listing is unchanged, selftest "59 lines moved, all named").

A first raceload `write` without `--roots-only` bound 75 extra rvas (Lua range included); it was reverted in full before anything
else ran (files deleted / checked out), and the rerun used `--roots-only`.

## 3. Match / relocheck on the source the exe includes (`match_relocheck.txt`)

| source | match.py --bytes | relocheck |
|---|---|---|
| re/match/s01/raw/00409710.c | BYTE-IDENTICAL 119 B | OK 10/0 |
| re/match/s02/004103a0.c (after the MATCH_STATIC edit) | BYTE-IDENTICAL 360 B | OK 32/0 |
| re/match/s14/0048ae00.c | BYTE-IDENTICAL 357 B | OK 30/0 (not bound, section 4) |
| re/match/s08/00448220.c | 97.0%, 81 bytes differ from +0x3fe (provisional, expected) | BAD (shifted bytes; provisional body, not "matched") |

## 4. 0x0048ae00: left unbound (measured)

Bound in `gates` and wired into its one caller 0x0040bbb0 (one `#undef/#define FUN_0048ae00 PortTop_Bound_0048ae00` line in the
rebound block of `Match/PortTop_0040bbb0.cpp`, the form seven earlier rows used), all four knob boots exited **0xC0000374** after
1.8 to 10.8 s, each log ending at `[ORIG_TOP] [RW_FS] 0x004cc230(2, 1, "Splash.dff")`, `RpClumpStreamRead(...) (1284 bytes, 1
atomics)`, `RwGlobals +0x10c free(00000037)`. Splash.dff is 0x0048ae00's clump (hooks.csv: "Splash.dff clump + 8-slot ring alloc").
0x0040bbb0 runs at boot (from 0x00402750), so the wiring makes the body run in every boot, and its callees 0x00544a70 / 0x00544ad0
/ 0x00544bf0 (C2, 58.1 / 46.0 / 74.3%) are named stops. With the wiring and the root removed, the same four boots exit rc 0 after
3000 frames. Which call frees 0x37 is [UNCERTAIN] (register line). The crashing boots' logs were overwritten by the rerun under
the same labels; the lines above are from the run's console output in this session, not a kept file.

Also: the brief says "C4 by replay since run 53". hooks.csv row 0048ae00 carries c4=match only (BYTE-IDENTICAL 357 B, insn 98.3%,
verify/c4rmatchbatch_20261107), no `c4=frida-replay` tag, and no replay RESULT names 0x0048ae00 (grep of verify/). It would bind
as a matched body. It stays a hand stand-in (`PortTop_StandIn04G_0048ae00`), rank 6 in the re-run.

## 5. 0x00448220 evidence lines (side by side)

| source | verdict |
|---|---|
| 2026-11-11 PROV-INSITU, old source | DIVERGED@1116 (control o448220_b) |
| PROVISIONAL.tsv `replay_after_fix` (const scale operands fix) | IDENTICAL-FOR-2724 (2 runs: f448220_b, fx_00448220) |
| run 53 PROV-INSITU-REF1 (`verify/provinsitu_ref1_20261121/VERDICTS.tsv:95`) | IDENTICAL-FOR-600 (window: boot, menu, load, first ~190 race ticks; "a verdict here does not overturn a long-race one", RESULT_PROV-INSITU-REF1.md:29-31) |
| run 53 PROV-INSITU-REF1-WS2 | stepped run s_00448220 DIVERGED@302 (0x896400); unstepped nw_s_00448220 (`--prov-wlog 0`) IDENTICAL-FOR-600; [UNCERTAIN] register line "diverge only when single-stepped ... the mechanism ... is not known" (UNCERTAIN_REGISTER.md, PROV-INSITU-REF1-WS2) |
| equiv | NOT-PROVEN (call-memory at 0x004485d4, call 0x442e00) |

Bound as a listed provisional body (L-126, knob path only, not C4). No ADMITTED.tsv line (not EQUIV).

## 6. Object build vs log/orch/run55_headcheck (`bytecheck_vs_head.tsv`, `ddkobj_summary.json`)

`py.exe -3.12 re/tools/ddkobj/ddkobj.py check` on the final object build:

| | head | this row |
|---|---|---|
| IDENTICAL | 2336 | **2337** (+0x00409710) |
| DIFFERENT | 72 | **72**, the same set (0 newly DIFFERENT units, nothing to classify) |
| not-in-map | 27 | **28** (+0x004103a0) |
| refusals | 19 (13 externalized-static-register-convention, 6 wrapper-redefines-globals) | 19, same split |
| dual body | 33 | 33 |
| provisional units kept VS2022 | 254 | 255 (+0x00448220, `skip:provisional`) |
| provisional fns in linked units | 7 | 7 |
| units linked | 2322 / 2595 | 2324 / 2598 |

Per bound unit: 0x00409710 IDENTICAL (`_PortTop_Bound_00409710`, 128 B, 0 diff bytes). 0x004103a0: unit status ok, register
convention kept with adapter `___ddkadapt_FUN_004103a0` -> `_PortTop_Bound_004103a0`, BYTECHECK `___ddkreg_004103a0` not-in-map;
the map has `_FUN_004103a0` from `ddk_t01g_004111c0_PortTop_004111c0.obj` (0001:0004cb30), the HELD copy the 0x004111c0 DDK unit
compiles in its own TU, so in the object build 0x004111c0 calls its in-TU copy and the gate copy is unreferenced ([UNCERTAIN] that the
linker dropped it for that reason; register line). 0x00448220 `skip:provisional`.

Pairs "listed unit -> 100% neighbour that fell back to its VS2022 body": none new. The one newly listed unit is 0x00448220; the count
of provisional functions kept VS2022 inside linked units is 7 before and after, so no 100% neighbour fell back.

## 7. The three binding numbers

1. Units executing or reachable from a bound caller on the top-down path: **3** (0x00409710 <- 0x00426e10, 0x004103a0 <- 0x004111c0,
   0x00448220 <- 0x0040d470). None executed in the 3000-frame boots (race paths; [UNCERTAIN] register line).
2. Pre-bound keep-alive units in the generated `PortTop_BulkRefs.cpp`: **129**, unchanged from the head (with 0x0048ae00 wired it was
   128: 0x005449f0 got a bound caller; back to 129 after the removal).
3. Bindable rows left unbound, with reason:
   - 0x0048ae00 (c4=match): boot crash when wired, section 4.
   - c4=match callees outside the roots, named stops by RVA ownership (`roots_only_reason`): 0x00409330, 0x004095a0, 0x0041ef60,
     0x00418860 (also cpp-unclean in raceframe), 0x00441990, 0x00446520 (now rank 2, 26 fns released).
   - stops with no bindable source: 0x00408b00 (C2 97.0%), 0x00434360 (no row), 0x00442e00 (C2 65.1%), 0x0046c7b0 / 0x0046cbb0
     (C4 hand rh bodies in Vehicle/VehicleState.cpp, hand path, same stop raceframe has).

## 8. Checks run

| check | result |
|---|---|
| lock + memory before each build / check / selftest / boot | `mashed_lock.py status` free and commit 19.1 to 21.9 GB (< 26.0) each time, **except one slip**: the second object build (after the alias change) started while the merge train held `PRIORITY-HOLD ... merge-train:20261121_204524 since 21:08:18`; my chained command printed the status but did not wait. Every later step looped until the status was not PRIORITY-HOLD / SOLO. That build was superseded by the final one |
| VS2022 build (final) | Build OK, mashed_re.exe + mashed_re_dev.asi (MASHED_NO_DEPLOY) |
| object build (final, MASHED_DDKOBJ=1) | link rc 0, check as section 6 |
| ddkobj.py check (explicit) | IDENTICAL 2337, DIFFERENT 72, not-in-map 28 |
| binder selftest (final) | `FAIL 14`; gates `plan IDENTICAL (bind 3 reuse 33 stop 11), wrappers+stops IDENTICAL`; raceload, bulk, raceframe, renderframe IDENTICAL; BINDINGS.tsv IDENTICAL. Line by line vs the head (`verify/porttopland_20261121/selftest_cp3.txt`): 29 failure lines, 29 in the head, **0 new** (`selftest_subset.tsv`: the 28 audio lines + `FAIL 14`). RULE 7 met |
| knob boots (final, `tools/knob_boots.py`, test-slot outside, mashed_lock inside, root D:/MashedWT/BIND-GATES-ALLKNOBS_root) | knob_vs_base, knob_vs_all, knob_ddk_base, knob_ddk_all: game rc 0, `WinMain 0x00492370 returned 0 after 3000 frames`, self-test `[OT_CODEPTR] applied: 188 slots (31 bound, 157 stop thunk), 72 data seeds ... self-test PASS (0 of 260 checks failed)`. Base = MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000; all = base + MASHED_OT_RACEFRAME, MASHED_OT_RENDERFRAME, MASHED_RWC_FRAME, MASHED_OT_RL_D110, MASHED_OT_TITLETICK |
| knob boots with 0x0048ae00 wired | 4 of 4 exit 0xC0000374 (section 4); the helper first printed PASS from the wrapper's rc, now fixed to read the game's rc |
| all-knobs rank re-run on this tree | exit 0; ranked 223 (GATERANK 197 on the stale input); 0x004103a0, 0x00409710, 0x00448220, 0x00440bc0, 0x005a7b60 not ranked; 0x0048ae00 rank 6 (stand-in). Top 5: 0x00420420, 0x00446520, 0x0055f800, 0x00418860, 0x00487280 (`T2_GATE_RANK_allknobs_after.tsv`, `rank_after_head.txt`) |
| c4_reclass refresh dry run | 1 would-refresh, 0 refused |
| not run | G-CTRL, B, 12x4 captures (top-down row), menu walk (no menu code changed) |

## 9. Files

Committed: this RESULT, `plan_gates.txt`, `match_relocheck.txt`, `bytecheck_vs_head.tsv`, `ddkobj_summary.json`, `selftest.txt`,
`selftest_subset.tsv`, `knob_boots.txt`, `rank_after_head.txt`, `rank_fixinput_head.txt`, `T2_GATE_RANK_allknobs_after.tsv`,
`tools/knob_boots.py`, `raw_sha256.txt` (SHA-256 of every raw file in D:/MashedWT/BIND-GATES-ALLKNOBS_data cited here: the four
boot logs, both exes, the build logs, selftest, both rank outputs, the plan listing).

## 10. Unfinished

- 0x0048ae00 (needs 0x00544a70 / 0x00544ad0 / 0x00544bf0 bodies or a decision on the crash cause first).
- The six c4=match callees left as gates stops (ownership); 0x00446520 is the next all-knobs gate.
- None of the three bodies ran: a knob-on boot that reaches race load / race frames is needed to see them execute.
