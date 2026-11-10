# RESULT PORT-OT step-1 sweep (2026-11-10): AI, RENDER, AUDIO, UTIL, SAVE, POWERUPS, OTHER

L2 worker `cmwarkkjt0qrple1mu1pz2s1w` for lead `cmw7xu1960f37p41557tefpnd` (L-111 (a) step 1). One pre-scan across the
seven subsystems, then one commit per subsystem with swappable rows: orch/PORT-OT-RENDER, then orch/PORT-OT-OTHER stacked
on it. Base: orch/integration `dabf91efe` + orch/PORT-OT-VEHICLE `4883ee9ee`.

## Pre-scan

`origtop_base_dump.py --dump-base` on bde8b3774 (D:/MashedWT/PORT-OT-SWEEP_data/base.tsv, sha256 below; SIZE.tsv restored
afterwards): 1,008 rows in the base runnable set. A row is a candidate when its BINDINGS kind is `stand-in`. `-` rows have
no BINDINGS row (reached through a matched or stand-in ancestor), `under-stand-in` rows sit under a stand-in parent that is
not this step's.

| row | base rows | stand-in | under-stand-in | swapped | callees added | result |
|---|---|---|---|---|---|---|
| PORT-OT-AI | 35 (34 `-`, 1 matched) | 0 | 0 | 0 | 0 | **0 rows** |
| PORT-OT-RENDER | 166 (121 `-`, 26 matched) | 9 | 10 | 6 | 2 | RESULT_PORT-OT-RENDER.md |
| PORT-OT-AUDIO | 89 (all `-`) | 0 | 0 | 0 | 0 | **0 rows** |
| PORT-OT-UTIL | 116 (108 `-`, 6 matched) | 0 | 2 | 0 | 0 | **0 rows** |
| PORT-OT-SAVE | 9 (all `-`) | 0 | 0 | 0 | 0 | **0 rows** |
| PORT-OT-POWERUPS | 5 (all `-`) | 0 | 0 | 0 | 0 | **0 rows** |
| PORT-OT-OTHER | 107 (particle 37, others 70) | 13 (particle) | 0 | 10 | 3 | RESULT_PORT-OT-OTHER.md |

- UTIL's two `under-stand-in` rows (0x00496920 TimerTable772ffcGet, 0x00496930 Table773030Get) sit under the 0x00492d30
  stand-in (the frame tick, PORT-OT-RACE step 3). They are not swaps.
- RENDER's ten `under-stand-in` rows are under 0x00492e90 / 0x00492d30, the same.
- Stand-ins of these subsystems that are in BINDINGS but **not** in the base runnable set, so out of step-1 scope: SAVE
  0x004963e0, 0x00496400 (config log), UTIL 0x00492d30, 0x00494c80, 0x004950b0, AUDIO 0x004669b0, 0x0045e040, 0x00466a50,
  POWERUPS 0x0045bed0, 0x0045bf30, OTHER input 0x004967e0, particle 0x0048af70 / 0x0048ae00 / 0x0048e820 / 0x0048ff20, RW
  0x00558240 / 0x0053fbb0, and the RENDER list in RESULT_PORT-OT-RENDER.md.
- OTHER = every hooks.csv subsystem no PORT-OT row names (list in RESULT_PORT-OT-OTHER.md). Only particle has stand-ins.

## What landed

- RENDER: 0x004759b0, 0x00476390 (+ 0x00475ef0), 0x00477e40 (+ 0x00477b40), 0x00475ea0, 0x0041a980, 0x004d7ff0.
- OTHER (particle): 0x00489290, 0x0048a460, 0x00487e00 (+ 0x00487150), 0x00487140, 0x00485d90 (+ 0x004892c0, 0x004893a0),
  0x00485e10, 0x004862d0, 0x00486350, 0x0048eac0, 0x0048fdd0.
- Left, each with its reason in the subsystem RESULT: RENDER 0x00475d90, 0x00490000, 0x004768c0 (HUD-2's row); particle
  0x0048cf70, 0x0048ffd0, 0x0048a830.

Unlike the GAMEPLAY / VEHICLE swaps, most of these bodies **run** in the knob-ON boot: the inits are called by 0x0040bbb0
at startup and the teardowns by 0x0040bd00 at AppDestroy (per-row detail in each RESULT, section "Executed").

## Verification

Top of the stack (orch/PORT-OT-OTHER, RENDER its parent): knob-ON boot rc=0 after 3000 frames with 24 PTank records
created and the same 24 destroyed, 12 captures OFF/ON/OFF/ON class E (walk_04 ba34030d1b7e2fe4), G-CTRL
8284be46/67931834/31fe043e, B 1d5bf4f9 x3, rva-lint OK (ORIGTOP-ONLY 15 -> 23). Detail: RESULT_PORT-OT-OTHER.md.
Port column: 21 rows none -> called (8 RENDER, 13 OTHER), no drift.

## Merge

- `git merge-tree --write-tree` of orch/PORT-OT-OTHER against orch/integration `d89e5da1c` (PORT-OT-VEHICLE merged since
  this base): rc 0, no conflict.
- Against orch/PORT-OT-HUD-2 `9bb08df4b`: CONFLICT in mashedmod/exe_sources.rsp and re/tools/dual_copy_allowlist.txt.
  orch/integration against HUD-2 alone gives the same two conflicts (HUD-2 appends at the tails GAMEPLAY / VEHICLE also
  changed), so they are not from this stack. Match/PortTop_0040bbb0.cpp, which both edit, auto-merges.

## Lessons for the next rows

- 0x004768c0 dereferences `record + 4` with no check, so every teardown in 0x0040bd00 that calls it swaps only together
  with the init that builds its records. A teardown whose init is a stand-in (0x00475d90 / 0x00475a00, 0x0048cf70) stays.
- `update_bindings_otgameplay.py` and `update_bindings_otvehicle.py` each re-run `update_bindings_othud.py`, which resets
  the other one's rows. `update_bindings_otrender.py` runs the othud chain once and applies both tables.
- A comment line `//  - 0xRVA Name` above a declaration of `Name` in the same TU is an RVA header to rva_body_scan.py (a false
  body). Write `0xRVA -> Name`.
- PORT-OT-HUD-2 (still `working`) adds Match/PortTop_004b6480.cpp and edits the 0x004210b0 lines of
  Match/PortTop_0040bbb0.cpp. The rebinds here sit in one block below that list so the two branches do not touch the same
  lines.

## Raw files (D:/MashedWT/PORT-OT-SWEEP_data, not committed)

| file | sha256 |
|---|---|
| base.tsv (origtop_base_dump --dump-base on bde8b3774) | 2c307d9225fa3e82daa1cbd04093fadd7d29466abcc9b19a2819a7f61dd53314 |
| OT_MEASURE.rerun.tsv (ot_measure.py verdict on this tree) | d25bcf4adb584e2525d036fcefaf77451b306afe73f9defb4916d853510989c2 |
| match_bytes.txt (match.py --bytes + relocheck on the 20 swapped sources) | 90151d993646cde93dafd6ccfc8996f64a244082efbf76393a8691d5406616b4 |
| knob_on_1.log, captures, arms | in RESULT_PORT-OT-OTHER.md |
