# RESULT PORT-TOP-RENDERFRAME-492E90 (run 53, checkpoint 1)

Row kind: top-down. Branch `orch/PORT-TOP-RENDERFRAME-492E90` from `orch/integration` @ e9ea31d6d.
Worker session cmwq48iuj6n2bnz1ch8hs26zx (claude2, Opus 5.5), lead cmwpwu26q5v0hnz1cierfryfw.

## Verdict

**Checkpoint 1 (0x00492e90) bound behind the default-OFF sub-knob `MASHED_OT_RENDERFRAME`.** The closure was walked from
0x00492e90 with `re/tools/bind/porttop_bind.py --profile renderframe`. Both builds `=== Build OK ===`, rva-lint OK
(424 known, NEW=0). `ddkobj.py check`: IDENTICAL 1897/1958, DIFFERENT 37, not-in-map 24. Every byte-matched bound unit is
IDENTICAL (87 of 87). The 9 DIFFERENT units that are new against the base's 28 are classified in section 4. Binder selftest:
renderframe IDENTICAL, audio IDENTICAL, BINDINGS IDENTICAL, raceload DIFFERENT 2 = the known PROV4 baseline (section 5).
Knob boots: section 6. **cp2 (0x0040d110) and cp3 (0x00429290) were not started.** The lead ended run 53 after
checkpoint 1, so they lead run 54. G-CTRL / B / 12 captures: not run (top-down row, the knob is off for those hashes).
L-124 holds: no librw binding added. The RW callees of the closure bind to RW core bodies (`RwCore_*`, 43 reuses) or
to matched `re/match/rwl*` sources.

## 1. Binding numbers (log/orch/run53_lead_rules.md "Binding numbers")

| number | value | note |
|---|---|---|
| reachable from a bound caller on the top-down path | **181** units (93 bind incl. the root, 88 reuse) | all reached from 0x00492e90 through bound bodies. They run only under `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_OT_RENDERFRAME=1` |
| pre-bound, keep-alive only | **0** | this row writes no `PortTop_BulkRefs.cpp` entry |
| bindable left unbound, with a reason | **39** | 30 cpp-unclean, 6 PORT-TOP-06's 0x004111c0 closure, 1 RACE-BIND-C4-BULK cp1, 1 owned by RACE rows (race_owned.txt), 1 `static` in its source |
| stops with no bindable source | 13 | 8 C2 below 100%, 2 C3 below 100%, 1 C4 without a c4=match tag (0x0041db80), 2 with no re/match source (0x00484360, 0x005f99b8) |

The plan has 52 stops; 0x00404320 PerModeRenderMachine (C4 by frida, no re/match source, hand body
`Render/PerModeRender.cpp`, not reused) is one of the 13. Per-stop list: `STOPS.tsv` (from `tools/binding_numbers.py`).
Plan listing: `plan_listing_cp1.txt`.

Bodies bound by unmerged branches are listed, not rebound (named stops with the reason, `state/renderframe/stop_reasons.tsv`
from `tools/make_stop_reasons.py`): RACE-BIND-C4-BULK cp1 (`plan_cp.json` @ d4fda5c93: 0x004b5260, 0x004e6680 reached here)
and PORT-TOP-06's 0x004111c0 closure (snapshot of its uncommitted `state/raceframe/plan.json`,
`tools/porttop06_raceframe_plan_snapshot.json`, sha256 60ae1fa7e153241af5a1cbff1e442bdf041ddbf046b9cfb366b9b79d44f9f603;
reached here: 0x0040b890, 0x0041e9b0, 0x0041f260, 0x0041f290, 0x00443080, 0x00467210, 0x00476df0, 0x004b4a80, 0x004c1b40).
Run-53 ownership: 0x004b4080 and 0x004c3d90 are in race_owned.txt, stops here. No audio_owned rva was reached.

## 2. Binding shape (sub-knob and the one exe body of 0x00492e90)

- **Sub-knob `MASHED_OT_RENDERFRAME`, default OFF** (unset or `0`). It acts only on the RW core frame route
  (`RwCore_FrameRoute()`, `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1`), where `PortTop_StandIn_00492e90`
  (Match/PortTop_00492370.cpp:312) already calls the matched 0x00492e90 as `RwCore_00492e90`
  (RwCore/RwCore_00492e90.cpp, row RWC-FRAME-492E90). Off that route the hand RenderFrame runs, knob or not.
- **One exe body of 0x00492e90.** A second wrapper `PortTop_00492e90.cpp` was rva-lint `UNTAGGABLE DUP-IN-TARGET` against
  `RwCore_00492e90.cpp` (first sync). So the binder got a `root_external` option: the RW core TU stays the only body and
  its direct callees whose walked symbol differs from the TU's own go through selectors
  (`Match/PortTop_RenderFrameSel.cpp`, generated): knob on = the walked body or named stop, knob off = the TU's previous
  symbol, so the knob-off path is unchanged. 15 of the root's 31 callees got a selector: 12 replace an `RwCoreMissing_`
  stop with a bound body (0x0040de30, 0x0040df60, 0x0040dfc0, 0x00410b30, 0x00426670, 0x004266b0, 0x0042a9f0, 0x0042d390,
  0x00430b90; reuse 0x0042b8f0 StatePhaseIsFinal, 0x0042f510 Vehicle0HandleGet, 0x004671c0 GetOverlayCamera), 3 replace it
  with a named stop of this closure (0x00404320, 0x0040df20 cpp-unclean, 0x00426030 cpp-unclean). A callee the TU
  already binds to an `RwCore_` body keeps it (L-124): 0x0042f530 and 0x004c1be0 are not switched to their `Orig_` copies.
  The TU's 15 changed `#define` lines carry `// was <old>; PORT-TOP-RENDERFRAME-492E90 ...`. Note: a regeneration of the
  TU by `verify/rwccorerun_20261111/gen_rwcore_tus.py` would drop them; `porttop_bind.py write --profile renderframe` puts
  them back and the selftest checks them.
- Named stops: `Match/PortTop_RenderFrameStops.cpp` (`PortTop_RRStop_*`, env `MASHED_OT_RR_STOPS`, counter
  `PortTop_RR_StopHits`), 52 stops.
- 92 new wrappers `Match/PortTop_<rva>.cpp` (header `// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body`),
  `exe_sources.rsp` +94, `dual_copy_allowlist.txt` +33 ORIGTOP-ONLY lines, `BINDINGS.tsv` +178 rows (0 updated).
- samesrc: 10 planned binds whose source another TU also compiles (0x0040dfc0 `MatchS02_0040dfc0.cpp`, 0x0041d870,
  0x00426d20, 0x00426d90, 0x00429620, 0x00429660, 0x00429b70, 0x00429e10, 0x0042c7c0, 0x00432ad0) are accepted as separate ORIGTOP-only bound copies, so the walked
  closure keeps its own callee bindings. One exception: 0x00401340 is aliased to its exe body `CupSpinSpeedAndColor`
  (Match/MatchS01.cpp), since its source compiles on the port only with `MASHED_RE_PORT` (the wrapper failed LNK2019 on
  4 externs, build cp1b).
- Extern-guard step: 6 bound sources got `#ifdef MATCH_PORT` address defines (re/match/s01/004011f0.c, 004013f0.c,
  00401f10.c, 004036a0.c, 00404650.c, 004072e0.c), each `match.py --bytes` BYTE-IDENTICAL after the edit.
- cpp-unclean: 35 rvas from build cp1a (`cpp_unclean_cp1.txt`), named stops with the first error, not hand-fixed (the C
  emit of BIND-CUNCLEAN-AS-C is not on this branch); 30 of them are reached in the final plan.

### porttop_bind.py hunks owned by this row (for the hand merge with PORT-TOP-06 and BIND-CUNCLEAN-AS-C)

The branch's porttop_bind.py = PORT-TOP-06's working copy (uncommitted on orch/PORT-TOP-06 @ a9e5b4c8a, sha256
950722fa93407320641e0aa7ed5f79fc9d22dc59c99620fd36c1ce5309b123e5: profile `raceframe`, stop_reasons, static_stop,
group_same_source, held_external, thunk_names, bindings_keep_missing, hdr / stops_subst, the index regex) plus these
hunks of this row (line numbers in this branch's file; `diff --strip-trailing-cr -U0` against that copy):

| lines | hunk |
|---|---|
| 200-239 | `PROFILES['renderframe']` block (incl. `alias['00401340']` and `root_external`) |
| 512-513 | `Binder.sym()`: a `root_external` root returns its TU symbol |
| 517-595 | `Binder.write_selectors()`: selector generator and the TU define rewrite |
| 780 | `write()` prune: a profile wrapper of a `root_external` rva is removed |
| 789-792 | `write()`: a `root_external` root writes selectors instead of a wrapper |
| 1102-1103 | `SELFTEST['renderframe'] = '00492e90'` |
| 1114 | selftest profile list adds `renderframe` |
| 1127-1139 | selftest: a `root_external` root is checked by regenerating the selectors and restoring the files |

## 3. Builds (status before each: `status_pre_cp1a.txt`, `status_pre_cp1b.txt`, `status_pre_cp1c.txt`)

| build | result |
|---|---|
| VS2022 cp1a | compile errors in 35 matched sources (cpp-unclean), recorded with `unclean` |
| VS2022 cp1b | LNK2019: 4 externs of re/match/s01/00401340.c (alias fix) and `RwCore_Log` declared with C++ linkage in the selector file (fixed: `extern "C"`) |
| VS2022 cp1c (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK 424 known NEW=0 |
| object build cp1c (`MASHED_DDKOBJ=1`) | `=== Build OK ===`, `[ddkobj] link rc=0` |

Every build and check waited for: no PRIORITY-HOLD / SOLO in `mashed_lock.py status`, no other Mashed build.bat or
ddkobj.py running, commit at or below 26 GB (`tools/wait_build.py`, copied from PORT-TOP-06). Cp1a waited about 29 min
behind the merge train's PRIORITY-HOLD / SOLO window (merge-train:20261121_045343). Status at cp1c: slots 0, 1, 3 held by
PROV-INSITU-REF1, RACE-RWLAYER-00543D40, RACE-RWGLOBALS-FRAMELIST; memory 23.1 GB of 28.5 GB; `mashed_machine` HELD by
PROV-INSITU-REF1 (a game run, does not block a build).

## 4. ddkobj check (`py -3.12 re/tools/ddkobj/ddkobj.py check`)

IDENTICAL 1897/1958 (base 1803/1855), DIFFERENT 37 (base 28), not-in-map 24 (base 24). Base list: the integration head's
check in `D:/MashedWT/RACE-WRITER-0067EA74_ddkcheck.log`. `comm`: no base unit dropped out of DIFFERENT. 9 newly DIFFERENT:

| rva | unit | class |
|---|---|---|
| 0x00420050, 0x00429bd0, 0x004b61c0 | bound, ADMITTED.tsv bodies | admitted source below 100% by definition (L-126), as the base classifies 0x005bce80 / 0x005bbdb0 |
| 0x004770a0, 0x00477e60, 0x00477f50 | bound, c4=frida-replay bodies | replay C4 source below 100%, as the base classifies 0x005ae3a0 |
| 0x0041fcc0 | `___ddkreg_0041fcc0`, co-resident in re/match/s05/00420050.c | C4 frida-replay, second function of the admitted source above |
| 0x004036a0 | `___ddkreg_004036a0`, co-resident in re/match/s01/004036a0.c (bound rva 0x00403910 is IDENTICAL) | C2 function of a bound source, below 100% |
| 0x004969a0 | `___ddkreg_004969a0`, co-resident in re/match/s16/00496c10.c (bound rva 0x00496c10 is IDENTICAL) | C2 function of a bound source, below 100% |

Per bound unit: match 87 IDENTICAL, admitted 3 DIFFERENT, replay 3 DIFFERENT. **ddkobj.py decode defect:** the decode-only
diff of commit 296c82aca (orch/BIND-CUNCLEAN-AS-C) was applied to the working tree for the object build and the check
(`git show 296c82aca -- re/tools/ddkobj/ddkobj.py | git apply`) and reverted with `git checkout -- re/tools/ddkobj/ddkobj.py`
before the commit. ddkobj.py is not changed on this branch.

## 5. Self-tests

- `py -3.12 re/tools/bind/porttop_bind.py selftest`: renderframe roots 1: plan IDENTICAL (bind 93 reuse 88 stop 52),
  wrappers+stops IDENTICAL (94 checked); audio IDENTICAL (385 checked); BINDINGS.tsv IDENTICAL; raceframe skipped (no
  plan on this branch); **raceload: plan IDENTICAL, wrappers+stops DIFFERENT 2 (Match/PortTop_00426e10.cpp,
  Match/PortTop_RaceLoadStops.cpp) = the known PROV4 baseline on orch/integration, not this row, files not rewritten.**
  Exit `FAIL 2` = that baseline only.
- Game self-tests: section 6.

## 6. Knob boots (`tools/run_arms.py`, root `D:/MashedWT/PORT-TOP-RENDERFRAME-492E90_root`, test-slot outside, mashed_lock inside)

**Not run at commit time: queued.** The arm batch (vs_off, vs_rfoff, vs_frameT, vs_rfT, ddk_off, ddk_frameT, ddk_rfT)
waited 10 min on test-slot memory (23.8 GB committed + 8 GB per run over the 24.0 GB budget), took slot 2/8, then queued
on `mashed_machine`, held by PROV-INSITU-REF1 (pid 1736, a game run), for the rest of the session. The lead asked for
the report within about 30 minutes, so the knob boots and the game self-tests (portstate-image, OT_CODEPTR, OT_TIMER,
RWC-DEVICE) are **not measured** here. To check in run 54 (`py -3.12 verify/porttoprenderframe_20261121/tools/run_arms.py
vs_off vs_rfoff vs_frameT vs_rfT ddk_off ddk_frameT ddk_rfT` inside test-slot + mashed_lock): vs_off = vs_rfoff, vs_frameT
matches the RWC-FRAME-MOVIE-BACKDROP vs_frameT arm (exit 0, 600 frames, self-tests PASS), vs_rfT logs
`[PORT-TOP-RENDERFRAME-492E90] MASHED_OT_RENDERFRAME on` and its stop hits. [UNCERTAIN] knob-off unchanged rests on
reading the selector code (each selector calls the TU's previous symbol when the knob is unset); test: the vs_frameT /
ddk_frameT arms above (registered in re/orchestrator/UNCERTAIN_REGISTER.md).

## 7. Stops ranked by census functions gated (`STOPS_RANK.tsv`, `tools/stops_rank.py`)

Generated by `tools/make_stops_rank.py` from T2-GATE-RANK's `t2_gate_rank.py` (same graph, census and ranking key), with
this row's plan as the last layer and the gates taken under the frame-render closure (from 0x00492e90 following run
nodes: 173 functions, 160 T2 census). 57 gates: stop 36, stand 1, unbound 20. **19 of the 20 `unbound` gates have an RW
core TU body** (`RwCore/RwCore_<rva>.cpp`) that the reused RW core bodies call; the ranking's route layers do not read RW
core TU defines, so those 19 are gates of the ranking model, not stops on the knob path (0x00456c90 is the one without).
Top 20 (rel_tool = census functions released by binding the gate with the tool, behind = census functions it dominates):

| rank | rva | name | class | rel_tool | behind (calls) | state / reason |
|---|---|---|---|---|---|---|
| 1 | 0x0045b990 | FUN_0045b990 | c4=match cpp-unclean | 4 | 4 (10900) | stop: C2440 |
| 2 | 0x0041ebb0 | FUN_0041ebb0 | c4=match cpp-unclean | 3 | 1 (2725) | stop: C2197 |
| 3 | 0x00426030 | FUN_00426030 | c4=match cpp-unclean | 2 | 3 (8175) | stop: C2660 (arg count of bound 0x0041e8f0) |
| 4 | 0x0042e5b0 | MenuChromeShellB | C3 54.3% | 2 | 1 (11993) | unbound, RW core TU body |
| 5 | 0x0042e3a0 | MenuChromeShellA | c4=match bindable | 2 | 0 | unbound, RW core TU body |
| 6 | 0x004d8350 | FUN_004d8350 | c4=match bindable | 1 | 2 (1949833) | unbound, RW core TU body |
| 7 | 0x00406130 | FUN_00406130 | c4=match cpp-unclean | 1 | 1 (2725) | stop: C2632 |
| 8 | 0x0041f290 | FUN_0041f290 | c4=match bindable | 1 | 1 (21351) | stop: PORT-TOP-06 closure |
| 9 | 0x004725c0 | FUN_004725c0 | c4=match cpp-unclean | 1 | 1 (2725) | stop: C2440 |
| 10 | 0x00490490 | FUN_00490490 | c4=match cpp-unclean | 1 | 1 (5450) | stop: C2197 |
| 11 | 0x004e6680 | RpClumpRender | c4=match bindable | 1 | 1 (41774) | stop: RACE-BIND-C4-BULK cp1 |
| 12 | 0x0041b630 | HudSlotLoopB630 | c4=match cpp-unclean | 1 | 2 (12580) | stop: C3861 Orig_004b3fc0 |
| 13 | 0x00413a00 | FUN_00413a00 | c4=match cpp-unclean | 1 | 2 (7287) | stop: C2065 DAT_00828320 |
| 14 | 0x004189c0 | FUN_004189c0 | c4=match cpp-unclean | 1 | 1 (2725) | stop: C2632 |
| 15 | 0x00487de0 | FUN_00487de0 | c4=match cpp-unclean | 1 | 2 (5450) | stop: C2660 |
| 16 | 0x0048ad50 | FUN_0048ad50 | c4=match cpp-unclean | 1 | 1 (2725) | stop: C2197 |
| 17 | 0x004914b0 | RainRender | c4=match cpp-unclean | 1 | 1 (2725) | stop: C2197 |
| 18 | 0x00428760 | ViewportScaledRectDraw_Arg7Z | c4=match bindable | 1 | 2 (624) | unbound, RW core TU body |
| 19 | 0x00404320 | PerModeRenderMachine | C4, no re/match source | 0 | 1 (2725) | stop |
| 20 | 0x0040df20 | sub_0040df20 | c4=match cpp-unclean | 0 | 1 (2725) | stop: C2664 (RwCore_004c1c80 arg type) |

Class tally of the 57: c4=match cpp-unclean 26, c4=match bindable 21, C3 with a source 3, C4 frida-replay with a
source 3, C4 no bindable source 2, C2 with a source 1, hand stand-in 1. The cpp-unclean class (26 gates) is what the C
emit of BIND-CUNCLEAN-AS-C would release.

## 8. Checkpoints 2 and 3

Not started (lead, run 53 wind-down). They lead run 54: cp2 = 0x0040d110 (gate 4, topm01/0040d110.c 100%, the caller swap
under a sub-knob; RACE-BIND-C4-BULK cp1 already binds its body), cp3 = 0x00429290 (gate 3, s06/00429290.c 100%).

## 9. Files

Committed: this RESULT, `plan_listing_cp1.txt`, `STOPS.tsv`, `STOPS_RANK.tsv`, `cpp_unclean_cp1.txt`, `status_pre_*.txt`,
`tools/` (make_stop_reasons.py, porttop06_raceframe_plan_snapshot.json, make_stops_rank.py, stops_rank.py (generated),
binding_numbers.py, wait_build.py, run_arms.py), `re/tools/bind/state/renderframe/` (plan.json, newfiles.txt,
cpp_unclean.txt, stop_reasons.tsv), the binder, wrappers, selector and stop files, the RW core TU, rsp, allowlist,
BINDINGS.tsv, 6 guarded sources.

Raw (not committed), `D:/MashedWT/PORT-TOP-RENDERFRAME-492E90_data/`:

`raw_sha256.txt` (committed) lists the sha256 of every raw file: vs_cp1a.log, vs_cp1b.log, vs_cp1c.log, ddk_cp1c.log,
check_cp1c.log, arms_cp1.out, diff_base.txt, diff_mine.txt, diff_rows.tsv, and both exes.

Exes: mashed_re.exe sha256 1b6f96d23d82f326..., mashed_re_ddk.exe 436d49a09a800f3a... (full values in the raw list).
Kills: none (knob_boot.py kills only the PID it spawns, on its own timeout). No `original\` write, no deploy of
`original\mashed_re_dev.asi`, no tracker edit, no merge.
