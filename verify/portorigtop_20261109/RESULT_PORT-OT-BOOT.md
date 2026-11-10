# RESULT PORT-OT-BOOT (2026-11-09): HANDOFF after scoping, no exe change yet

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

## Acceptance still to run (next worker)

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
