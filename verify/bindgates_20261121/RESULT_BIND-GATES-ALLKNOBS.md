# RESULT BIND-GATES-ALLKNOBS (run 55, binding row, top-down kind)

Branch `orch/BIND-GATES-ALLKNOBS` on `orch/integration` head `39ee297f2`. Worker cmwr1c0f60cgrp121fvvlqiyh, lead
cmwr0mi2n088jp121jgz9oroy. Rank script taken with `git checkout orch/GATERANK-ALLKNOBS -- verify/t2gaterank_20261121/t2_gate_rank.py`
(single-file checkout of 5e6ddb4d5's copy, not a cherry-pick), then edited (section 1).

Verdict: 3 of the 4 gates bound (0x00409710, 0x004103a0, 0x00448220), 0x0048ae00 left unbound with a measured reason
(section 4). 0x00440bc0 was a stale rank input, fixed in the rank script. One RULE 4 slip (section 8).

**Merged tree (sections 11 to 13 supersede 6 to 8 for the merge bar).** Hand merge of orch/integration a2843847a (drain 1), plus
the widened step: bulk and raceframe regenerated, which binds 13 more rvas in raceframe (section 12). On the merged tree: both
builds OK; ddkobj 4 new DIFFERENT, all c4=frida-replay rows, classed as replay-C4 bodies; selftest FAIL 14, all audio and all
in the head's own lines (bulk, raceframe, raceload and gates at 0, against the head's FAIL 35); the 4 knob boots rc 0 after 3000
frames with self-test PASS; both ranks re-run, the default view differs from the head's only by the input fix plus this row's binds.

## 1. First step: why 0x00440bc0 still ranked as a stop

The rank's input, not a stale wrapper. `t2_gate_rank.py` (line 152 of the GATERANK copy) read the raceload plan from
`verify/ro_20261116/rltools/plan_rl.json`, the pre-BIND-TOOL copy frozen at PORT-OT-RACE-BIND-PROV4 (260 rvas), where
0x00440bc0 is `stop, cpp-unclean: C2632`. The live raceload plan is `re/tools/bind/state/raceload/plan.json` (411 rvas), where
0x00440bc0 is `bind re/match/tlr/00440bc0.c match` (C unit `Match/PortTop_00440bc0.c`, PORT-OT-RACE-BIND-HANDLER). 157 rvas differ
in state between the two files. Fix (input): the rank reads `re/tools/bind/state/raceload/plan.json`. With that alone, 0x00440bc0
and 0x005a7b60 (rank 1 in GATERANK) leave the ranked gates (section 13: the fixed script on the unmodified head).

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

## 6. Object build vs log/orch/run55_headcheck (pre-merge tree 0fe6ee6a5; merged tree: section 12) (`bytecheck_vs_head.tsv`, `ddkobj_summary.json`)

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

## 7. The three binding numbers (pre-merge; merged tree: section 12)

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

## 8. Checks run (pre-merge tree 0fe6ee6a5; merged tree: section 13)

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

## 11. Hand merge of orch/integration (run 55, granted by L0 through the lead)

- Session moved to claude3 (`move_self`, claude-opus-5-5, medium) because `git merge` prompts on claude2.
- `git -c merge.autoStash=false merge orch/integration` at a2843847a (drain 1: GATERANK-ALLKNOBS, HB-LOCK-ENFORCE, LUA-VENDOR,
  KNOBON-BOOTS-G5, WS-OWNSLOT-CALLERSCAN, FIX-00420420-CRASH ...). One conflict: `verify/t2gaterank_20261121/t2_gate_rank.py`.
  Resolved by taking the head's file and re-applying this row's two input edits (live raceload plan, EXTRA_PLANS glob). The
  result is byte-identical to this branch's copy: the head carries GATERANK's `--all-knobs` / `--out` version unchanged, which
  is the copy this row started from. Both the `--all-knobs` view and the input fix are in. Merge commit f55441fe3.
  `re/orchestrator/UNCERTAIN_REGISTER.md` auto-merged.
- (a) **State-file edit in another profile (raceload).** `re/tools/bind/state/raceload/c_hold.txt` loses one line, no other
  raceload state file changes (profile.json, rules.tsv, roots.txt, cpp_unclean.txt byte-identical to the head):
  ```
  @@ -4,7 +4,6 @@
   00409680	race-owned
  -00409710	race-owned
   00425bf0	race-owned
  ```
  The file's header allows it: "An rva leaves this list when the row that owns it binds it (delete its line, then `write`)".
  The parent gave this row 0x00409710. It is still a state-file edit in a profile this row does not own (RULE 3), made under
  that header and the ownership grant.
- (b) **RULE 4 slip at 21:08:18.** The branch predated HB-LOCK-ENFORCE, so the build wrappers had no lock wait yet. My chained
  command printed `PRIORITY-HOLD` and started the object build anyway. Every build, check, selftest and boot on the merged tree
  ran behind a loop that waits while the status shows PRIORITY-HOLD or SOLO. Each printed `free` or `PRIORITY-LINGER` (no hold)
  and a commit of 17.8 to 19.0 GB.

## 12. Widened step: bulk and raceframe regenerated on the merged tree

The head's own selftest at a2843847a is **FAIL 35** (`selftest_head_a2843847a.txt`, run on a scratch detached checkout of
a2843847a): audio 14 lines plus bulk (1 wrapper, 1 unnamed moved line) and raceframe (17 wrappers). Cause: drain 1's
promotions made bodies bindable and no row regenerated the plans. Regenerated with the tool: `write --profile raceframe`,
`write --profile bulk`, `sync` for both, `refs --profile raceload` (it owns `PortTop_BulkRefs.cpp`; run again after the sync, so
that the index sees the new TUs).

**raceframe: 13 new binds, owned by this row** (plan `plan_raceframe_regen.txt`, match `match_relocheck_regen.txt`):

| rva | source | kind | match.py --bytes | relocheck |
|---|---|---|---|---|
| 0x0041f3d0 | re/match/s04/0041f3d0.c | match | BYTE-IDENTICAL 281 B | OK 13/0 |
| 0x00420870 | re/match/s05/00420870.c | replay (c4=frida-replay, WS-OWNSLOT-CALLERSCAN) | 1172 B, 609 bytes differ (not a byte match) | BAD (replay body, not "matched") |
| 0x00422440 | re/match/s05/00422440.c | match | BYTE-IDENTICAL 45 B | OK 4/0 |
| 0x00422470 | re/match/s05/00422470.c | match | BYTE-IDENTICAL 72 B | OK 4/2 |
| 0x00426dc0 | re/match/s06/00426dc0.c | match | BYTE-IDENTICAL 29 B | OK 2/0 |
| 0x0046b540 | re/match/s11/0046b540.c | replay (c4=frida-replay, WS-OWNSLOT-CALLERSCAN) | 1356 B, 906 bytes differ | BAD (replay body) |
| 0x0046bce0 | re/match/s11/0046bce0.c | match | BYTE-IDENTICAL 54 B | OK 3/0 |
| 0x0046bd20 | re/match/s11/0046bd20.c | match | BYTE-IDENTICAL 54 B | OK 3/0 |
| 0x0046bd60 | re/match/s11/0046bd60.c | match | BYTE-IDENTICAL 50 B | OK 1/0 |
| 0x0046bda0 | re/match/s11/0046bda0.c | match | BYTE-IDENTICAL 104 B | OK 3/0 |
| 0x00540080 | re/match/rwl03/00540080.c | match | BYTE-IDENTICAL 114 B | OK 2/0 |
| 0x00540100 | re/match/rwl03/00540100.c | match | BYTE-IDENTICAL 95 B | OK 1/0 |
| 0x00540160 | re/match/rwl03/00540160.c | match | BYTE-IDENTICAL 97 B | OK 1/0 |

raceframe also moves, without a new body: 0x0040ce80, 0x00422aa0, 0x004260a0, 0x004260b0, 0x00431d70, 0x004c5a60 (new
reuses of existing exe bodies), 0x00420290 and 0x00479100 (new named stops). Wrappers changed: `PortTop_0040da50.cpp`,
`PortTop_0040e590.cpp`, `PortTop_004111c0.cpp`, `PortTop_RaceFrameStops.cpp`, plus the 13 new wrappers. `sync --profile
raceframe`: rsp +13, allowlist +11. `externs`: 0. `samesrc --profile raceframe` lists 26 (for example 0x004922e0, carried by
`Match/MatchS15_004922e0.cpp`). The bind count does not change between plan and write, and this row adds no alias for them.
raceframe state files other than the generated plan.json / newfiles.txt are byte-identical to the head.

**bulk:** no bind. 0x00420420 (FIX-00420420-CRASH's 100% source, now c4=match) stays a stop, because it is outside the bulk cp1
list (`roots_only_reason`), so this row does not own a body of it. Its listing line changes text only. `state/bulk/moved.tsv`
gets 2 appended lines, the other bulk state files are byte-identical:
```
+00420420	00420420 stop  C2 FUN_00420420	00420420 stop outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source re/match/s05/00420420.c C4 FUN_00420420	stop text only: drain 1 (run 55) promoted 0x00420420 by match (FIX-00420420-CRASH), ...; nothing bound (BIND-GATES-ALLKNOBS regeneration)
+totals	Counter({'stop': 227, 'reuse': 181, 'bind': 97}) ...	Counter({'reuse': 204, 'stop': 188, 'bind': 97}) ...	plan totals after the moves (BIND-GATES-ALLKNOBS run 55: 0x00420420 stop text only, totals unchanged; ...)
```
`PortTop_BulkStops.cpp` regenerated (the stop's reason text). `PortTop_BulkRefs.cpp`: 129 -> 128 keep lines (0x00431d70 now
has a bound caller in raceframe).

**Object build vs log/orch/run55_headcheck** (`bytecheck_vs_head.tsv`, `ddkobj_summary.json`):

| | head check | merged tree |
|---|---|---|
| IDENTICAL | 2336 | 2354 |
| DIFFERENT | 72 | 76 |
| not-in-map | 27 | 28 (+0x004103a0, section 6) |
| refusals | 19 | 19 |
| dual body | 33 | 33 |
| provisional units kept VS2022 | 254 | 255 (+0x00448220) |
| units linked | 2322 / 2595 | 2337 / 2611 |

New IDENTICAL rows in BYTECHECK.tsv (16): 0x00409710, 10 of the 11 byte-matched binds (0x00426dc0 was already IDENTICAL in the
head check under another symbol), and the byte-matched file-locals 0x0041f360, 0x0041f4f0, 0x0041f880, 0x00420340
(re/match/s05/00420870.c) and 0x004223f0 (re/match/s05/00422470.c). The summary IDENTICAL count rises by 18, not 16. The 2 are
not traced: no other BYTECHECK row differs from the head check.
**Newly DIFFERENT units (4), each classified:**

| rva | symbol | source | diff bytes | class |
|---|---|---|---|---|
| 0x00420870 | _PortTop_Bound_00420870 | re/match/s05/00420870.c | 602 | **replay-C4 body**: hooks.csv C4 `c4=frida-replay`, verify/wsownslot_20261121/RESULT_WS-OWNSLOT-CALLERSCAN.md (IDENTICAL-FOR-600, install ok, body executed 10876 calls) |
| 0x0046b540 | _PortTop_Bound_0046b540 | re/match/s11/0046b540.c | 961 | **replay-C4 body**: c4=frida-replay, same RESULT |
| 0x0041f590 | ___ddkreg_0041f590 | re/match/s05/00420870.c (file-local) | 136 | **replay-C4 body**: hooks.csv C4 c4=frida-replay (verify/provwsprom_20261112/RESULT_PROV-WRITESET-PROMOTE.md). Here it is the local copy inside the replay-verified 0x00420870 file |
| 0x0041f770 | ___ddkreg_0041f770 | re/match/s05/00420870.c (file-local) | 176 | **replay-C4 body**: c4=frida-replay (verify/provinsitu_20261111/RESULT_PROV-PROMOTE-44.md), ADMITTED.tsv EQUIV row. Here it is the local copy inside the 0x00420870 file |

[UNCERTAIN] Whether the two local copies in re/match/s05/00420870.c are the same text as the bodies their own replay verdicts
ran (re/match/s05/0041f770.c and 0x0041f590's source) rests on reading. They did run inside 0x00420870's replay verdict.
Test: text-diff of the function bodies (register line). No linked DIFFERENT unit is unlisted: all four are replay-C4.

Pairs "listed unit -> 100% neighbour that fell back to its VS2022 body": none. Provisional functions kept VS2022 in linked units
stay at the head's count, and the one newly listed unit (0x00448220) carries no 100% neighbour.

**Binding numbers on the merged tree:** (1) units bound by this row that execute or are reachable from a bound caller: 16
(0x00409710, 0x004103a0, 0x00448220, plus the 13 raceframe binds under the raceframe closure of 0x004111c0). None executed in
the 3000-frame menu boots. (2) pre-bound keep-alive units in `PortTop_BulkRefs.cpp`: 128 (head 129). (3) bindable rows left
unbound: 0x0048ae00 (section 4), 0x00420420 (outside the bulk cp1 list), the six gates-profile ownership stops (section 7).

## 13. Checks on the merged tree

| check | result |
|---|---|
| lock + memory before each | loop until not PRIORITY-HOLD / SOLO. Printed: `free`, `free`, `PRIORITY-LINGER ... wait 95s more` (no hold), `PRIORITY-LINGER ... 91s`. Commit 17.8 to 19.0 GB |
| VS2022 build | Build OK (`build_vs2022_m.log`) |
| object build (MASHED_DDKOBJ=1) | link rc 0 |
| `ddkobj.py check` | IDENTICAL 2354, DIFFERENT 76, not-in-map 28 (section 12) |
| binder selftest | **FAIL 14**. raceload, bulk (listing "60 lines moved, all named"), raceframe, renderframe and gates IDENTICAL. BINDINGS.tsv IDENTICAL. Line by line against the head's own selftest at a2843847a (`selftest_subset.tsv`): 29 failure lines. The 28 audio lines are all in the head. The total line reads FAIL 14 against the head's FAIL 35. 0 new failures (RULE 7) |
| knob boots (4) | knob_vs_base, knob_vs_all, knob_ddk_base, knob_ddk_all: game rc 0, `WinMain 0x00492370 returned 0 after 3000 frames`, `self-test PASS (0 of 260 checks failed)` |
| rank, default view | head a2843847a: gates 165, R 810 / T2 551, ranked 137. Fixed script on the unmodified head: gates 190, R 888 / 628, ranked 161. Merged tree: gates 190, R 889 / 629, ranked 161. So the whole head -> merged shift is the input fix (it reads the 411-rva live raceload plan instead of the 260-rva copy), except one swap that is this row's: 0x00409710 leaves the ranked gates and its stop callee 0x004095a0 enters (R +1). Files `rank_default_rk_head_def.txt`, `rank_default_rk_headfix_def.txt`, `rank_default_rk_m_def.txt` |
| rank, `--all-knobs` | head: 197 ranked (= GATERANK). Fixed script on the head: 220. Merged: 222. Not ranked any more: 0x004103a0, 0x00409710, 0x00448220, 0x00440bc0, 0x005a7b60, 0x00420870, 0x0046b540. Top: 0x00420420 (bulk stop, outside cp1), 0x00446520, 0x0055f800, 0x00418860, 0x00487280, 0x0048ae00 (`T2_GATE_RANK_allknobs_after.tsv`) |
| not run | G-CTRL, B, 12x4 captures (top-down row), menu walk |

Raw data SHA-256: `raw_sha256.txt` (boot logs, both exes, build logs, the head selftest, all six rank outputs, the plan listings).
The crashing 0x0048ae00 boots of section 4 have no kept log (overwritten by the rerun under the same labels).

Unfinished on the merged tree: 0x0048ae00, 0x00420420 (needs a bulk-list or a gates root decision), the six ownership stops,
and executing the 16 bodies in a race-reaching knob boot.
