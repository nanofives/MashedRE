# RESULT PORT-OT-BOOT (2026-11-09): GREEN, 9 boot rows on their matched bodies under MASHED_ORIG_TOP

Second worker, from the handoff `a682f4e14` (scoping by the first worker, kept below). orch/integration `3fbf77214`
was already contained (`git -c merge.autoStash=false merge orch/integration`: up to date). One commit on
`orch/PORT-OT-BOOT`.

## Verdict

9 of the 10 planned rows run their matched bodies on the MASHED_ORIG_TOP path. 0x0041e0d0 stays a stand-in (reason
below). Knob-ON boot exits 0, the 12 captures are equal knob ON vs OFF (and equal to the 04E list) in all four runs,
default gates are unchanged, lint D4 burn-down unchanged at 2, ORIGTOP-ONLY = 4. Default path: no change (every new
TU is exe-only, reached only from the matched 0x00402750 / 0x0040cfd0, and the gates confirm it).

## What changed (one commit)

| rva | before | now | TU |
|---|---|---|---|
| 0x004026d0 | `PortTop_StandIn04E_004026d0` | `Orig_004026d0` | new Match/PortTop_004026d0.cpp, **ORIGTOP-ONLY** (DUP-IN-TARGET vs Match/MatchS01.cpp) |
| 0x00402f50 | `PortTop_StandIn04E_00402f50` | `BootDefaultParamsInit` (Match/MatchS01.cpp) | rebind only |
| 0x00404830 | `PortTop_StandIn04E_00404830` | `Orig_00404830` | new Match/PortTop_00404830.cpp, marker (see the lint note) |
| 0x00412890 | `PortTop_StandIn04E_00412890` | `Orig_00412890` (callee 0x0040bb30 = `Search40bb30`) | new Match/PortTop_00412890.cpp |
| 0x0047ba00 | `PortTop_StandIn04E_0047ba00` | `Orig_0047ba00` -> `Orig_00496940` | new Match/PortTop_0047ba00.cpp + Match/PortTop_00496940.cpp (RwRasterCreate bound) |
| 0x00420d00 | `PortTop_StandIn04E_00420d00` | `Orig_00420d00` -> `Orig_004b6520` -> `MemsetInline` | new Match/PortTop_00420d00.cpp (**ORIGTOP-ONLY** CROSS-TARGET vs Boot/PanelSortInit.cpp) + Match/PortTop_004b6520.cpp |
| 0x00471eb0 | `PortTop_StandIn04E_00471eb0` | `Orig_00471eb0` -> `Orig_00471df0` | new Match/PortTop_00471eb0.cpp |
| 0x00471df0 | `PortTop_StandIn04C_00471df0` (in 0x0040cfd0) | `Orig_00471df0` | new Match/PortTop_00471df0.cpp (**ORIGTOP-ONLY** CROSS-TARGET 0x00471530 vs Match/MatchS11_00471530.cpp) |
| 0x004723d0 | `PortTop_StandIn04E_004723d0` | `Orig_004723d0` | new Match/PortTop_004723d0.cpp (**ORIGTOP-ONLY** CROSS-TARGET vs Match/MatchS11_004723d0.cpp) |

Also: the 8 `SI(...)` stand-ins removed from Match/PortTop_00402750_Pieces.cpp, the 0x00471df0 stand-in removed from
Match/PortTop_StandIns04C.cpp, 10 TUs added to `mashedmod/exe_sources.rsp`, 4 tagged lines in
`re/tools/dual_copy_allowlist.txt`, and BINDINGS.tsv through the new `update_bindings_otboot.py` (runs the 04J updater
first): 431 rows, matched 145 -> 156, stand-in 105 -> 96, 2 rows added (0x00496940, 0x004b6520).

Departures from the handoff plan, each with its reason:
- **0x004026d0 is not a rebind but a second exe copy.** The exe body `BootQueueFlush` (Match/MatchS01.cpp:50, :101)
  calls its five callees at absolute original addresses (`#define FUN_004671a0 ((void*(__cdecl*)(int))0x004671a0u)`
  and four more, MatchS01.cpp:83-91). The exe maps no `.text` there: Compat/StandaloneRvaThunks gets exactly five
  thunks installed (exe_main.cpp:9841-9845), none of them these. So the rebind would call unmapped memory. The new TU
  compiles the same source with the callees bound by name to their BINDINGS exe symbols (`BackBuffRasterOpen`,
  `MashedRw_CameraClear_004c1bb0`, `IntroSplashVtableSlot6`, `RwVtableSlot07Call`, `Orig_004c1be0`). That is a
  DUP-IN-TARGET pair, which is the ORIGTOP-ONLY class. The knob log shows the four passes (knob_on_1.log:362-377:
  RwCameraClear, begin / end update, RwRasterShowRaster Present 0x00000000, 4 times).
- **0x0041e0d0 left out (stand-in kept).** Its body reads `*(puVar3[i] + 4)` for each of the 66 pointers of the table at
  0x0063d610 with no null check (re/match/s04/0041e0d0.c). The only writer of that table on this path is 0x0041def0
  (re/match/s04/0041def0.c:63-71: `FUN_004b6520(DAT_0063d610, 0x1d4)`, then the clone rows), and 0x0041def0 is still the
  stand-in `PortTop_StandIn04E_0041def0` (knob_on_1.log:262 `rva 0x0041def0: stand-in, not run (Panel family)`). With
  the table zero, the first read is at address 4. 0x0041def0 is not a boot row (absent from boot_base.tsv). It is in the
  Panel family. Re-pickup: swap 0x0041e0d0 in the same commit as 0x0041def0 (PORT-OT-FRONTEND scope).
- **0x00404830's source got a `#ifndef MATCH_PORT` guard** around its two plain `extern` lines
  (re/match/s01/00404830.c), so the port TU can bind the arrays to their original addresses. Under the DDK MATCH_PORT
  is undefined. `match.py --bytes` re-run after the edit: `100.0%  214/214 ins`, `1474 B whole symbol, 0 differing
  unmasked bytes -> BYTE-IDENTICAL`.
- **0x00420d00 binds its callee through a C++ overload.** The raw g2s source passes `&DAT_0063e4b8` to an `undefined4`
  parameter, which C accepts and C++ refuses. `FUN_004b6520` maps to a file-static inline taking `void*` that forwards
  to `Orig_004b6520`. The source text is unchanged.

## Acceptance

Machine rules: the build waited for the merge train (PRIORITY-HOLD pids 6248 / 3380, label
`merge-train:20261109_210953`, read from `%TEMP%\mashed_machine*`) to clear, at 22:39. Every game launch ran as
`test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-BOOT -- ...'`.

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed`. mashed_re.exe sha256 `3135ba6d781615c2bd7b9d10c7a71a11edb6c843d51c457c53c690497ce790c3` |
| knob-ON boot (`knob_boot.py knob_on_1 <rootK> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | log ends `0x00402a40 AppDestroy (matched) returned`, `HardwareExitApplication (matched) returned 0`, `WinMain 0x00492370 returned 0 after 3000 frames` (knob_on_1.log:605-615). knob_boot's own `rc=` line fell outside my console tail. The two knob-ON nav runs below use the same driver with MASHED_ORIG_TOP=1 and record `rc=0` |
| 12 captures, OFF / ON / OFF / ON, pin default (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0`, `pinned pos=55 hash=28538c74`, class E, 12/12 equal to the 04E list in every run |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json ref |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = ref |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=4 (class deleted at the ORIG_TOP flip; 5 marked TU(s))`, `OK -- no NEW duplicate bodies (6 known)`. Before: burn-down 2 (the two untagged allowlist lines), ORIGTOP-ONLY 0 |
| port column (`backfill_exe_file.py`, then `backfill_port.py`) | below |

Lint note: 5 TUs carry the marker and 4 pairs are counted. PortTop_00404830.cpp carries it because the scoping found
the hand body (Vehicle/CarMaterialTables.cpp + CarMaterialTables_dirt.inc), but the rva scanner reports no second
body for 0x00404830, so there is no finding to tag. The marker there is inert. The exe's hand readers use that TU's
own storage (`Tables()`, CarMaterialTables.cpp:95-96), not 0x00639cc8 / 0x00636c08, so the matched writes change no
value the default path reads.

### Port column deltas

`backfill_port.py`: `none=5182 body=551 called=418` (04J: none 5195, body 554, called 402), 33 rows changed.

This step (11 rows):

| rva | port |
|---|---|
| 0x004026d0 | body -> called (exe_file now `Match/MatchS01.cpp` and `Match/PortTop_004026d0.cpp`) |
| 0x00412890, 0x0047ba00, 0x00496940, 0x00420d00, 0x004b6520, 0x00471eb0, 0x00471df0, 0x00471530, 0x004723d0 | none -> called |
| 0x004b64e0 (MemsetInline, callee of 0x004b6520) | body -> called |

Not moved: 0x00402f50 was already `called` (BootDefaultParamsInit has another reached caller). 0x00404830: its
hooks.csv record has 13 fields (an unquoted comma in its notes cell), and both backfills print `left untouched`.
Fixing that record is a tracker edit (re-classify), not done here. Three more records are malformed the same way
(0x004464c0, 0x0041f360, 0x00420870), all pre-existing.

The other 22 changed rows are drift on this tree: bodies merged by earlier steps whose column was not regenerated
here. The eight 04J rows (0x00401000, 0x004014b0, 0x004c1800, 0x004c1840, 0x004e4900, 0x004e4b90, 0x004e48e0,
0x004b5320), 0x00401630, 0x00401690, 0x004025f0, 0x00489250, 0x0048bc10, 0x004b3bf0, 0x004e7e30, 0x004e8090,
0x004880a0, 0x004881d0, and PortBatch_HUD 0x0041d410, 0x0041d870, 0x0041e630, 0x0041e850. All none -> called.

### Raw files (D:/MashedWT/PORT-OT-BOOT_data, not committed)

| file | sha256 |
|---|---|
| knob_on_1.log | 34a6cefc2f9ea52267b784a457c5a96c86a5a975248184b6d83cceda47db09a2 |
| runs.tsv | 139c064ed273684c74b0c8c902eac3ca8bfb0dc5e6892918ec850866b0452501 |
| nav1/r01_OFF_pindflt.log | 532fb74e695e67389f2eb18bb55798d911b9314d1d67c992996681a84c404f1b |
| nav1/r02_ON_pindflt.log | 986a9d2c0fbd275d5534c647540df6c27ce2bdc9b67f616aba1a0fd07a25ee48 |
| nav1/r03_OFF_pindflt.log | b5fe9026f783fcfb6df628d074795a169a8597e043479f9495fe9df14ffad5c5 |
| nav1/r04_ON_pindflt.log | c4fab724f5fd5dc51da90691403b4494037dbc6ac37e0981dfa536f551a12b5e |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

---

# Scoping (first worker, handoff a682f4e14)

Branch `orch/PORT-OT-BOOT` from `orch/HB-CAPTURE-MPGPIN` (`351b22d91`), with `orch/HB-LINT-ORIGTOP-CLASS`
(`024ac3986`) merged in (`git -c merge.autoStash=false`). orch/integration was already contained (`3fbf77214`).
Why a handoff: the first worker's context was heavy after CAPFLAKE + HB-CAPTURE-MPGPIN. A merge train held the machine
(PRIORITY-HOLD from 21:09, 13-track SOLO from 21:33, ttl up to 4 h), so no build or game run was possible.
This file fixes the scope and the per-row plan. The next worker does the swap in one commit.

## How the scope was taken

- `verify/portorigtop_20261109/origtop_base_dump.py` = PORT-ORIGTOP-SIZE's `origtop_size.py` (ade6191a1) plus
  `--dump-base <tsv>`. It writes the base runnable set (today's bindings, no new family) with subsystem, BINDINGS kind,
  matched files, exe_file and the two MEASURE caveats per row (`rwglobals` = body names `DAT_007d3ff8`,
  `nonzero_data` = body reads nonzero original .data/.rdata).
  Run: `py -3.12 verify/portorigtop_20261109/origtop_base_dump.py --dump-base <out.tsv>`. It also rewrites
  `SIZE.tsv` next to itself. That file belongs to PORT-ORIGTOP-SIZE and is not committed here.
- On this tree the base set is **1,001** rows (SIZE.tsv at ade6191a1: 996), 129 already running. **65 are subsystem
  boot**: 45 already `matched` in BINDINGS, 15 `stand-in`, 5 with no BINDINGS row. Table: `boot_base.tsv`.
- `closure_04c.py <rva>` on the 20 non-matched rows: `boot_closures.txt`. States: exe / rw / asitu / gen / none.

## Per-row plan (the 20 non-matched boot rows)

### Swap in this commit (9 rows)

| rva | name | caller TU (stand-in define) | closure | what to do |
|---|---|---|---|---|
| 0x004026d0 | FUN_004026d0 | PortTop_00402750.cpp:73 `PortTop_StandIn04E_004026d0` | exe (MatchS01.cpp) | rebind the define to the exe symbol `FUN_004026d0` (matched body already compiled), delete the 04E stand-in |
| 0x00402f50 | FUN_00402f50 | PortTop_00402750.cpp:72 `PortTop_StandIn04E_00402f50` | exe (MatchS01.cpp) | same as 0x004026d0 |
| 0x00404830 | FUN_00404830 | PortTop_00402750.cpp:70 | gen re/match/s01/00404830.c | new `PortTop_00404830.cpp`. **Dual**: the hand body is Vehicle/CarMaterialTables_dirt.inc, so mark the TU `// RVA-LINT: ORIGTOP-ONLY` and `lint_rva_bodies.py --seed` |
| 0x00412890 | FUN_00412890 | PortTop_00402750.cpp:68 | gen re/match/s03/h2_00412890.c + exe 0x0040bb30 | new TU, callee 0x0040bb30 = exe `FUN_0040bb30` (MatchS02_0040bb30.cpp) |
| 0x0041e0d0 | FUN_0041e0d0 (04K family-2 teardown) | PortTop_00402a40.cpp:25 `PortTop_StandIn04C_0041e0d0` | gen + 3 rw + 1 exe | new TU, RW callees via LibRw/RwApiBind.cpp, delete the stand-in from PortTop_StandIns04C.cpp:20 |
| 0x0047ba00 | FUN_0047ba00 | PortTop_00402750.cpp:54 | gen x2 + 1 rw | new TUs for both gen nodes |
| 0x00420d00 | PanelSortInit | PortTop_00402750.cpp:67 | asitu + gen + exe | new TU(s). The asitu node is CROSS-TARGET against an asi-only hand body, so the matched TU gets the ORIGTOP-ONLY marker (the class covers CROSS-TARGET, RESULT_HB-LINT-ORIGTOP-CLASS.md:23-24) |
| 0x00471eb0 | thunk_FUN_00471df0 | PortTop_00402750.cpp:69 | gen x2 + asitu | with 0x00471df0 below |
| 0x00471df0 | FUN_00471df0 | PortTop_0040cfd0.cpp:18 `PortTop_StandIn04C_00471df0` | gen + asitu | **note at the define:** "its source also holds static 0x00471530". Read it first, the source TU carries a second body. Delete PortTop_StandIns04C.cpp:72 |
| 0x004723d0 | Init691500 | PortTop_00402750.cpp:52 | asitu | matched TU with ORIGTOP-ONLY marker |

(10 lines: 0x00471df0 and 0x00471eb0 are one family.) For each gen node, take the macro map from BINDINGS `exe_symbol` the
way PortTop_004014b0.cpp does. `gen_top04c_tus.py --check` lists declared callees that have no binding. Write
`update_bindings_otboot.py` on the 04J pattern (run the 04J updater first, then set these rows to `matched` /
`Orig_<rva>`). Add every new TU to `mashedmod/exe_sources.rsp`.

### Left out of this commit, with the reason

| rva | name | reason |
|---|---|---|
| 0x00484170 | Rec18Create | PORT-OT-MEASURE: body reads RwGlobals 0x007d3ff8 and nonzero original data |
| 0x004841d0 | FUN_004841d0 | PORT-OT-MEASURE: body reads nonzero original data |
| 0x005c4d50 | FUN_005c4d50 | PORT-OT-MEASURE: body reads RwGlobals. It is also off the path (row below) |
| 0x004924f0 | sub_004924f0 | not MEASURE. PORT-TOP-04D measured that the matched DataZeroFill changes walk_04 (0x007f0a40 defaults survive the hand init, exe_main.cpp:189). Stays behind `MASHED_TOP04D_DATAZERO=1` (PortTop_00492370.cpp:123-138) until 0x007f0a40 is restored from the save on this path |
| 0x00418980 | thunk_FUN_0041a060 | closure is 15 gen + 7 asitu + 3 none, 19 of its nodes outside the base set. It also reaches 0x004770c0, which reads nonzero data (MEASURE). The 04K family-2 powerup loader, so PORT-OT-POWERUPS |
| 0x0041b660 | FUN_0041b660 | closure 15 gen + 2 asitu + 1 none, 9 nodes outside the base set. The 04K family-2 panel loader, so PORT-OT-FRONTEND |
| 0x005c4d20 / 0x005c4d30 / 0x005c4df0 / 0x005c4e10 | | no BINDINGS row and no caller define in any PortTop TU: reached only through an address-taken edge, not on the executed path |

## Acceptance plan as handed off (run above)

- build `MASHED_NO_DEPLOY=1`, only when `mashed_lock.py status` shows no PRIORITY-HOLD / SOLO
  - `status` hung on 2026-11-09 21:40 (Bash and pwsh). Reading `%TEMP%\mashed_machine*` in PowerShell worked: the
    files are JSON.
- knob-ON boot exits 0 (`knob_boot.py knob_on_1 <rootK> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`)
- 12 captures, knob OFF / ON / OFF / ON with the pin. `knob_boot.py` now sets `MASHED_DET_MPG_PIN=0` itself
  (HB-CAPTURE-MPGPIN). Reference = the 04E list.
- default gates G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3
- lint: D4 burn-down count unchanged, ORIGTOP-ONLY count reported (the lint line prints both)
- `backfill_exe_file.py`, then `backfill_port.py`, port-column deltas listed
- Roots ready: `D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_rootK` (nav / knob, `original` junction to the 04J rootK copy)
  and `D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_root` (gates). `verify/porttop_20261108/capflake_runs.py` drives nav sets
  with `CAPFLAKE_DATA` / `CAPFLAKE_ROOT`.
