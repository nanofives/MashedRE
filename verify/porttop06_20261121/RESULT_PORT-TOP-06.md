# RESULT PORT-TOP-06 (run 53, 2026-11-21)

Row kind: top-down. Branch `orch/PORT-TOP-06` on `orch/integration` `a9e5b4c8a`. Worker claude2, Opus 5.5 medium, lead
cmwpwu26q5v0hnz1cierfryfw. Gates: not run, top-down row (no G-CTRL, no B, no 12x4 captures).

## 0. Verdict

GREEN for what this row binds and checks: both builds green, every bound `c4=match` unit IDENTICAL in ddkobj, binder selftest
unchanged from the head's baseline, all four knob boots (VS2022 and object build, sub-knob OFF and ON) rc 0 with self-test PASS (section 5).

**The bound race-frame closure cannot execute until the top-down race load completes.** 0x004111c0 is called only from
cases 3 / 6 of the frame machine 0x00492d30 (`re/match/s15/00492d30.c:72`, `FUN_004111c0(0x32)`). On the knob path the
phase stays 1 for all 3000 frames (`[ORIG_TOP] frame N: ... phase 1, ..., 0x004111c0 calls 0` in every boot), because the race
load 0x0040d270 does not complete: its closure stops at 0x0047b860 (PORT-OT-RACE-BIND-PROV4). So no bound body of this row
ran in any boot. The boots show that the exe with the closure linked still boots and self-tests as before. They are not
evidence of the bodies' behaviour.

## 1. What changed

- **Sub-knob `MASHED_OT_RACEFRAME`, default OFF.** `Match/PortTop_00492d30.cpp` now binds `FUN_004111c0` to the dispatcher
  `PortTop_RaceFrame_004111c0` (`Match/PortTop_RaceFrameKnob.cpp`). Unset or not `1`: `PortTop_StandIn05_004111c0`, the
  PORT-TOP-05 counting stand-in, unchanged. `=1`: `PortTop_Bound_004111c0`, the byte-matched race frame
  (`re/match/t01g/004111c0.c`, hooks.csv C4 `c4=match`, whole symbol BYTE-IDENTICAL), which calls 0x0040fc00 (Race::Tick)
  and 0x004102f0 as file-local members of that TU (`re/match/t01g/004111c0.c:706-737`). The hand session
  (`PortTop_HandFrameGameFlow`) still runs after the frame machine in both modes.
- **Closure bound by walking** from 0x004111c0 with `re/tools/bind/porttop_bind.py`, new profile `raceframe`
  (stops `Match/PortTop_RaceFrameStops.cpp`, prefix `PortTop_RFStop_`, count mode `MASHED_OT_RF_STOPS=count`).
  240 wrappers `Match/PortTop_<rva>.cpp` (header `// PORT-TOP-06 (2026-11-21): exe-only port body`), state in
  `re/tools/bind/state/raceframe/` (`plan.json`, `stop_reasons.tsv`, `cpp_unclean.txt`, `newfiles.txt`). Listing:
  `plan_listing.txt`.
- `mashedmod/exe_sources.rsp` +242 lines (240 wrappers, stop file, knob TU). `re/tools/dual_copy_allowlist.txt` +128
  lines (sync step, ORIGTOP-ONLY tag) plus 6 untagged held-copy lines (section 4). `verify/porttop_20261108/BINDINGS.tsv`
  +438 rows (`bindings` step) and the 0x004111c0 row text (stand-in to the sub-knob).
- Extern-guard step on 9 bound sources (`a5_00410510.c`, `a5_00422b50.c`, `s01/00403250.c`, `00405460.c`, `004073b0.c`,
  `00408590.c`, `00408610.c`, `00408a70.c`) and a hand guard of the multi-name extern block in `re/match/pilot/a6_00423040.c:38-43`
  (same `#ifdef MATCH_PORT` form the tool writes). `match.py --bytes`: BYTE-IDENTICAL for all except 0x00408590 / 0x00408610
  (`c4=frida-replay`, not byte matches), whose match.py output is identical before and after the guard (12 B at +0x16 and
  32 B at +0x19c both times). 8 other guarded sources ended up unbound (cpp-unclean) and were reverted.
- 0x00492e90 not touched (PORT-TOP-RENDERFRAME-492E90). No ROADMAP.md, no tracker edit, no promotion TSV.

### 1.1 `re/tools/bind/porttop_bind.py` hunks this row owns (for the hand merge with CUNCLEAN / RENDERFRAME)

Line numbers are in the new file of this branch (`git diff -U0 a9e5b4c8a -- re/tools/bind/porttop_bind.py`).

| new lines | hunk | what |
|---|---|---|
| 90-93 | `EXTRA_PRE['00401570']` | MASHED_RE_PORT + address defines for 0x00401570 (as `Match/MatchS01.cpp:42`, `:77-78`) |
| 95-96 | `EXTRA_IN['00401570']` | prototypes of 0x00401da0 / 0x004e6680 the source declares only outside MASHED_RE_PORT |
| 159-199 | `PROFILES['raceframe']` | the profile block (not_local, stop_reasons, static_stop, group_same_source, held_external, thunk_names, bindings_keep_missing, stop_why, alias, hdr, stops_subst, allow text) |
| 261-262 | `build_index` | include regex also takes nested folders (`s01/raw/`) and prefixed names (`h2_<rva>.c`) |
| 310-319 | `Binder.__init__` | load `stop_reasons.tsv`, `group_bound`, `stop_why` |
| 397-402 | `build_plan` | reason stop and static stop before a bind |
| 408-410 | `build_plan` | `held_external`: locals defined only under `#ifndef MATCH_PORT` become external |
| 420-433 | `build_plan` | `group_same_source`: one wrapper per source, other members reuse it |
| 437-466 | `held_defs`, `static_line` | helpers |
| 481 | `listing` | `self.name != 'audio'` (was `== 'raceload'`) |
| 504-505 | `header` | profile `hdr` template |
| 569-570 | `wrapper_text` | `#define FUN_<m> PortTop_Bound_<m>` for a group member |
| 576-581 | `wrapper_text` | `thunk_FUN_<x>` to the target's exe symbol |
| 644-647 | `stops_text` | `stops_subst` applied to the raceload stop text |
| 669 | `write` | refusal tag from `own_tag` (raceload keeps `PORT-OT-RACE`) |
| 855-857 | `cmd_bindings` | `bindings_keep_missing` (a raceframe stop does not overwrite another profile's missing row) |
| 976-977 | `SELFTEST` | `raceframe: 004111c0` |
| 988-991 | `cmd_selftest` | loops raceload, audio, raceframe (skips a profile with no plan.json) |

Every new rule is keyed on a `raceframe` profile flag, so raceload and audio plan and write exactly as before (selftest, section 3).

## 2. Binding numbers

| number | value |
|---|---|
| **units reachable from a bound caller on the top-down path** | **240 bound bodies + 31 file-local members**, all reached from the bound root 0x004111c0, which the matched 0x00492d30 calls under `MASHED_ORIG_TOP=1` + `MASHED_OT_RACEFRAME=1`. With the sub-knob off (default): 0. At run time: 0 executed (phase stays 1, section 0). Kinds: 213 `c4=match`, 26 `c4=frida-replay` (L-128, near-match), 1 L-126 admitted (0x00484de0). Plus 137 reuses of existing exe bodies (raceload / audio wrappers, RwCore, matched TUs) |
| **pre-bound, keep-alive only** | **0** (this row writes no `PortTop_BulkRefs.cpp` entry) |
| **bindable left unbound** | **36**, each with its reason in `unbound_bindable.tsv`: 28 cpp-unclean (first VS2022 error recorded in `state/raceframe/cpp_unclean.txt`), 2 `static` in their only source (0x004103a0 at `re/match/s02/004103a0.c:59`, 0x00421980 at `re/match/s05/004219f0.c:38`), 1 link (0x0041b540 calls `FUN_0041af50_body`, no exe body), 4 owned by RACE rows (0x004c3d90, 0x004e89a0, 0x00538c80, 0x00546b10, `race_owned.txt`), 1 bound by cp1 (0x004e6680) |
| stops with no matched / admitted source | 93 (`stops_nosource.tsv`) |

cp1 (RACE-BIND-C4-BULK @ d4fda5c93, unmerged): its 97 bind keys are in `stop_reasons.tsv`. One is reached here (0x004e6680) and
stays a named stop: listed, not rebound. A re-run of `write --profile raceframe` after cp1 merges reuses cp1's body. No rva in
`race_owned.txt` / `audio_owned.txt` got a new body from this row. An owned rva that already had an exe body is reused, not
rewritten. The two raceload files that drift on this head (`Match/PortTop_00426e10.cpp`, `Match/PortTop_RaceLoadStops.cpp`) were
not rewritten.

Same-source duplicates (samesrc, 23 on the final plan): 0x00401570, 0x00407b00, ... (full list in `samesrc.txt`). Their default TUs
(`Match/MatchS0x_*.cpp`, `Race/SubStateMachine.cpp`) bind the callees to hand bodies or raw-address forwarders, so this row keeps a
separate ORIGTOP-only bound copy of each (allowlisted ORIGTOP-ONLY by `sync`). 0x0045bba0 / 0x0045bf30 alias to the existing
top-down bodies `Orig_0045bba0` / `Orig_0045bf30` (`Match/PortTop_0045bae0.cpp`).

## 3. Checks run

| check | result |
|---|---|
| binder selftest, unmodified tool on `a9e5b4c8a` (baseline) | `FAIL 2`: raceload wrappers+stops DIFFERENT 2 (`Match/PortTop_00426e10.cpp`, `Match/PortTop_RaceLoadStops.cpp`, from the PROV4 merge, known); audio IDENTICAL; BINDINGS IDENTICAL |
| binder selftest, this branch | `FAIL 2`, the same 2 raceload files and nothing else: raceload plan IDENTICAL, audio plan + wrappers IDENTICAL, **raceframe plan IDENTICAL (bind 240 reuse 137 stop 129), wrappers+stops IDENTICAL (241 checked)**, BINDINGS.tsv from the audio plan IDENTICAL. Unchanged from the baseline |
| rva-lint (inside build.bat) | OK, NEW 0 (519 findings, all allowlisted; ORIGTOP-ONLY 507). D4 burn-down count +6 from the held-copy lines (section 4) |
| VS2022 build (`MASHED_NO_DEPLOY=1`) | **OK** (`=== Build OK ===`, pass 5; passes 1-4 were the cpp-unclean / link loop), exe sha256 `0733de40...45da1` |
| object build (`MASHED_DDKOBJ=1`) | **OK** (`=== Build OK ===`, 589 s), exe sha256 `2895fe61...a446cd`. **ddkobj run with the uncommitted decode fix of 296c82aca (cl output decode defect, HB-DDKOBJ-UTF8-DECODE), file restored before commit.** The first object build without it died at `ddkobj.py:1323` (`r.stdout` None, input = cl's Spanish output under PYTHONUTF8=1) |
| `ddkobj.py check` | IDENTICAL 2044/2129, DIFFERENT 57, not-in-map 28, linked 2041/2278, refusals 12 (section 4) |
| `match.py --bytes` + `relocheck.py` on every bound source (`match_bound.tsv`) | all 213 bound `c4=match` rvas BYTE-IDENTICAL, relocheck OK. 3 sources also carry a static held copy that is not byte-identical (0x0041b7a0 in `s04/0041c090.c`, 0x0041c410 in `s04/0041c410.c`, 0x0041ce00 in `s04/0041d830.c`): the bound rva's own symbol is BYTE-IDENTICAL in each |
| knob boots (4: VS2022 / object build x sub-knob OFF / ON) | all rc 0, 3000 frames, self-test PASS 0 of 260 (section 5) |

Build etiquette: test-slot and lock status printed before every build and the check (`status_pre_vs1..5.txt`, `status_pre_ddk1/2.txt`,
`status_pre_chk1.txt`, `status_pre_knob.txt` in the data dir). Every build waited while another Mashed build ran (vs1 waited on
RACE-RWGLOBALS-FRAMELIST's build). No PRIORITY-HOLD or SOLO was held at any build start; the machine lock was held by
PROV-INSITU-REF1 game runs (not a build). Commit 22.5 to 24.6 GB at build starts, under 26 GB.

## 4. ddkobj classification (against the near-baseline)

No ddkobj check of the bare `a9e5b4c8a` was run. The nearest is RACE-RWGLOBALS-FRAMELIST's final check (`a9e5b4c8a` plus that row,
since merged as 34c823375): IDENTICAL 1805/1857, DIFFERENT 28, not-in-map 24, refusals 11. All 28 of its DIFFERENT units are in
this tree's list unchanged. This tree adds:

| delta | units | class |
|---|---|---|
| IDENTICAL +239 | 210 bound `c4=match` bodies + 23 file-local members + others leaving not-in-map | byte-identical |
| DIFFERENT +29 | 25 bound `c4=frida-replay` bodies (L-128 near-match, not byte matches: 0x00408590, 0x00408610, 0x0040c690, 0x00410d10, 0x00411ae0, 0x0041d930, 0x004212b0, 0x00421d20, 0x00422ba0, 0x004252c0, 0x00425ca0, 0x00429310, 0x00474de0, 0x0047cea0, 0x00484f70, 0x00485ef0, 0x004864f0, 0x00486830, 0x00487020, 0x0048ab70, 0x0048b7c0, 0x0048d360, 0x0048d540, 0x00490500, 0x004c1b40) | replay C4 bodies, bound from their one re/match source as `porttop_bind.py` does for `c4=frida-replay`. Neither admitted nor provisional, no new listing |
| | 1 admitted (0x00484de0, `re/match/ADMITTED.tsv`, 8 B at +0x2c) | L-126 admitted, not a byte match |
| | 3 static held copies (0x0041b7a0, 0x0041c410, 0x0041ce00) | `static` copies held in the callers' sources for their DDK codegen (ESI-argument callees, `re/match/s04/0041c090.c:4`, `0041d830.c:3`). Not the rvas' own matched sources; the callers' symbols are IDENTICAL. Neither admitted nor provisional |
| not-in-map +4 | 0x00472740, 0x004762c0, 0x00476320 (bound `c4=match`) and the file-local members of 0x0048b440's source (0x0048aff0, 0x0048b230) | [UNCERTAIN] nothing linked references them (their only planned caller is 0x0048b440 via its file-local members), so /OPT:REF drops them |
| refusals 11 -> 12 | the per-unit `fail:` lines are identical to the baseline's 11 (`fail_mine.txt` = `fail_base.txt`, same sha256); none is a TU of this row | [UNCERTAIN] which unit the 12th count is |
| 0x004704c0 | bound `c4=frida-replay`, `in-set` in `ddkobj_build.json`, no BYTECHECK row | [UNCERTAIN] |

**Held-copy allowlist lines (6, untagged, RWC-CORE-RUN precedent).** `sync` reported 7 UNTAGGABLE findings after grouping. Each is a
`static` copy that two bound sources each hold, file-local: 0x00421980 and 0x004219a0 (`s05/004219f0.c`, `s05/004222d0.c`), 0x004773f0
(`s12/00477730.c`, `s12/00477490.c`, plus the asi-only `Match/MatchS12_004773f0.cpp`), 0x00485c20 (`s14/00485c20.c`, `s14/00485ef0.c`),
0x0048d0f0 (`s15/0048d360.c`, `s15/0048d540.c`). They are allowlisted as plain lines with the `static` line of each source
(`tools/held_copy_lines.py`), not tagged ORIGTOP-ONLY. They raise the D4 burn-down count by 6 and are deleted at the ORIG_TOP flip.
A 7th line (0x00454170) went stale when 0x00454790 became cpp-unclean and was removed. Same-source groups (3) were resolved instead:
one wrapper per source.

## 5. Knob boots

Env `MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000` (cp1's env), root `D:/MashedWT/PORT-TOP-06_root`, harness
`verify/porttop_20261108/knob_boot.py` through `tools/knob_boots.py` (test-slot outside, `mashed_lock.py run` inside).

| boot | exe | result |
|---|---|---|
| `knob_vs_off` | VS2022 | **rc 0**, `WinMain 0x00492370 returned 0 after 3000 frames`, **self-test `[OT_CODEPTR] applied: 188 slots (31 bound to an exe body, 157 to a stop thunk), 72 data seeds (288 B), 0 outside the pad; self-test PASS (0 of 260 checks failed)`**, phase 1 throughout, `0x004111c0 calls 0` |
| `knob_vs_on` | VS2022, `MASHED_OT_RACEFRAME=1` | **rc 0**, 3000 frames, the same self-test line (PASS, 0 of 260), phase 1 throughout, `0x004111c0 calls 0`. The dispatcher reads the sub-knob on its first call, which never comes, so its `[ORIG_TOP] PORT-TOP-06 sub-knob` log line is absent (0 lines): ON is not distinguishable from OFF in this boot, as section 0 says |
| `knob_ddk_off` | object build | **rc 0**, 3000 frames, the same self-test line (PASS, 0 of 260), phase 1 throughout, `0x004111c0 calls 0` |
| `knob_ddk_on` | object build, `MASHED_OT_RACEFRAME=1` | **rc 0**, 3000 frames, the same self-test line (PASS, 0 of 260), phase 1 throughout, `0x004111c0 calls 0`; sub-knob line absent for the same reason as `knob_vs_on` |

The OFF self-test line is the same as cp1's on its tree (188 / 31 / 157 / 72, PASS 0 of 260). No baseline boot of the bare head was
run here.

## 6. Stops under 0x004111c0, ranked by census functions gated

`STOPS_RANK.tsv` (43 gates), from `tools/stops_rank.py`, generated by `tools/make_stops_rank.py` from T2-GATE-RANK's
`t2_gate_rank.py` (orch/T2-GATE-RANK, unmerged; same graph, census `verify/census_20261116/census_T2-RACETICK-CENSUS.tsv`, T2
phases race-load + race-tick, and ranking key) with this row's plan as the last route layer (knob-on view) and the gates restricted to
stop / stand / unbound nodes reached from 0x004111c0 through run nodes. Static graph: the race-frame closure has 161 executed functions
there (all T2 census). `rel_tool` = T2 census functions released if that gate alone bound (with what `porttop_bind.py` binds by itself);
`behind` = T2 census functions it dominates from 0x00492370. `log/orch/audit_20261121/t2_nonc4.tsv` lists the non-C4 census rows by name.

| # | rva | name | class | rel_tool fns / T2 calls | behind fns / calls | this row's plan |
|---|---|---|---|---|---|---|
| 1 | 0x00448220 | Frontend::PostRaceResultCamera | C2 with a source (97.0%, in-situ DIVERGED@1116) | 52 / 2,024,208 | 17 / 117,110 | stop, no matched source |
| 2 | 0x0040e590 | FUN_0040e590 | C2 with a source (84.6%) | 34 / 1,101,325 | 18 / 322,218 | stop, no matched source |
| 3 | 0x00487280 | FUN_00487280 | no source (C2) | 15 / 3,102,578 | 4 / 732 | graph edge only |
| 4 | 0x0040eee0 | FUN_0040eee0 | no source (C2) | 11 / 25,886 | 2 / 18 | stop |
| 5 | 0x004a504f | FUN_004a504f | c4=match bindable | 10 / 25,876 | 5 / 7,000 | graph edge only (CRT, no FUN_ reference in a bound source) |
| 6 | 0x004103a0 | SubStateHeldCountdown5 | c4=match, `static` in its source | 8 / 6,487 | 41 / 940,740 | stop: static at `re/match/s02/004103a0.c:59` |
| 7 | 0x004a5f4a | FUN_004a5f4a | C4 libid, no source | 5 / 16,965 | 1 / 86 | graph edge only |
| 8 | 0x0041b4d0 | FUN_0041b4d0 | c4=match cpp-unclean | 5 / 3,136 | 2 / 5 | stop, cpp-unclean |
| 9 | 0x0045be90 | FUN_0045be90 | c4=match cpp-unclean | 5 / 180 | 2 / 2 | stop, cpp-unclean |
| 10 | 0x0046b540 | VehicleSpawnInit | C3 with a source (59.2%) | 4 / 146,309 | 2 / 40 | stop |
| 11 | 0x00419760 | FUN_00419760 | c4=match cpp-unclean | 4 / 11,058 | 5 / 11,060 | stop, cpp-unclean |
| 12 | 0x004a5fbf | FUN_004a5fbf | C4 libid, no source | 3 / 13,644 | 1 / 86 | graph edge only |
| 13 | 0x004464c0 | CameraEntry::DispatchAll | C4 untagged, no source | 3 / 3,252 | 7 / 7,148 | stop |
| 14 | 0x0045bed0 | PowerupRoundCleanup | c4=match cpp-unclean | 3 / 32 | 3 / 10 | stop, cpp-unclean |
| 15 | 0x0055ab30 | FUN_0055ab30 | C4 untagged, no source | 2 / 24,226 | 1 / 11,934 | stop |

Rows 16-43: `STOPS_RANK.tsv`. Class tally of the 43: C4 no bindable source 15, c4=match cpp-unclean 12, c4=match bindable 5 (CRT /
thunk graph edges and the static 0x004103a0), no source 4, C3 with a source 4, C2 with a source 3. [UNCERTAIN] static reach: the graph
ranks by direct and relocation edges of the census-executed set; the plan walk is by `FUN_` names in sources, so 11 graph gates are not
plan keys ("graph edge only").

## 7. Uncertain (one line each in `re/orchestrator/UNCERTAIN_REGISTER.md`)

- not-in-map binds 0x00472740 / 0x004762c0 / 0x00476320: test = object-build link with /VERBOSE:REF.
- ddkobj refusal count 12 against 11 identical `fail:` lines: test = print the refusal list from `ddkobj_summary.json` producer.
- 0x004704c0 has no BYTECHECK row: test = trace the unit in `ddkobj.py` build (`units` entry for `re/match/s11/004704c0.c`).

## 8. Raw data (D:/MashedWT/PORT-TOP-06_data)

| file | sha256 |
|---|---|
| vs_vs5.log (VS2022 build, green) | 3ebd4061fcd1ea74fe2f2394b97cdf5d58a0e8ab135137aa36514abde3c146f9 |
| ddk_ddk2.log (object build, green) | 2c5469412879c51fb55c37fa00b229e62c3b77c2eabaf1c1e0a17bcc3431d40d |
| check_chk1.log (ddkobj check) | 1e8ca314bebfda3865ba9920433742b827aa2d374ffb91d45f979880fb9d0343 |
| BYTECHECK_chk1.tsv | ed184b5abbee989956901c3d5b83236925b7c6d8afc76d843d5760a851313461 |
| mashed_re_vs5.exe | 0733de404d562476d53eee57461267385fc5096170855849f6b32d1309f45da1 |
| mashed_re_ddk2.exe | 2895fe61f1e84e7f47b82d8b12887775c481981658ce96687f9a6f1ecba446cd |
| knob_vs_on.log / knob_ddk_off.log / knob_ddk_on.log | 015075da66c9d4c77e1040bc0cd458e5a99bac447f8f795d4c0fda71dcd28448 / 876fc8add04b47ed028b0d1775c01187ab1a22e88597e4f43a09e3d1e90deb0b / d887c2ef07b76bf45a66ea11d7d203d07725ce471b8c32414f439dba95232717 |
| knob_vs_off.log / .out | 61a3cd04bd764b0131ae3354a60f7bc4183d2dc3effe905f2bd4f0f0dec29353 / 119cfe869614effac2a5d6aec69a7457ebc38c68f54273cdc57ef02b5d27351d |
| externs_apply.log | d0407ffcfff7c33245d649185e49bb751ff916009298ccb46d53f1eac14275a3 |
| fail_base.txt = fail_mine.txt | 79dd48b3c56ba86d98b5f74091de08d4dc4cfdb2e7d3af8690202cff977b7a05 |
| status_pre_vs5 / ddk2 / chk1 / knob | 7509cc8d... / c4ccd336... / 6054c353... / 9f6ed2d7... |
| baseline: D:/MashedWT/RACE-RWGLOBALS-FRAMELIST_ddkcheck_final.log, _build_ddk_final.log | (another row's files) |

## 9. Next

- Bind order under the race frame: rank 1-2 (0x00448220, 0x0040e590) need a match. 12 cpp-unclean stops go to BIND-CUNCLEAN-AS-C's C-unit
  emit. 0x004103a0 needs a non-static port form of its source, or a register-convention thunk.
- The closure only runs once the top-down race load completes (first stop 0x0047b860). The first ON boot that reaches phase 3 / 6 is the
  real test of this row's bodies.
