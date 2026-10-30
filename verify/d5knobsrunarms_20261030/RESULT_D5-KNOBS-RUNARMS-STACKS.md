# RESULT D5-KNOBS-RUNARMS-STACKS (2026-10-30)

Verdict: **GREEN**. The 9 report-only opt-out stacks are retired from the gate runner, and the 10 knobs they were the
last setters of are deleted from the exe. The exe builds, rva-lint 0 NEW, G-CTRL and B unchanged, D2 DEFH 3/3 in the 42 s band. No setter of a deleted knob remains (section 3).

Step: follow-up of D5-KNOBS-B1 / B2 / B3 (`verify/d5knobsb1_20261030`, `verify/d5knobsb2_20261030`,
`verify/d5knobsb3_20261030`), scope as widened by Mariano in run 26: retire `verify/savelink_20261023/run_arms.py` stacks
NOPGIC, OPT, NOP, NOPC, NOCL5, NOL5, NOCC, NCC and NOD2, then delete the knobs they were the last setters of.
Branch `orch/D5-KNOBS-RUNARMS-STACKS` from `orch/integration` @ `e23f260b7`, change commit `8bf1ba734`.
No default flipped: every deleted read was an opt-out of a path that runs when the variable is unset (inventory class b,
`verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md`), so the default build executes the same code before and after.

## 1. The set, derived from the stacks

The 9 stacks set 12 names (`run_arms.py` at `e23f260b7`, line numbers there):

| Stack | Line | Names |
|---|---|---|
| OPT (= `OPTOUT`) | :19, :22 | NO_HEADING_ASSIST, NO_PONTOON_CARRY, NO_PONTOON_STATE, NO_PONTOON_ANIM |
| NOP / NOPC | :23 | NO_PONTOON_ANIM / NO_PONTOON_CARRY |
| NOCC | :57 | NO_AI_COUNTDOWN_CLOCK |
| NOD2 | :158 | NO_D2LAT_REFRESH, NO_GRID_DIR, NO_A5_RACE_TIMER |
| NOCL5 / NOL5 | :160 | NO_AI_CLOCK_LATE, NO_AI_LUT / NO_AI_LUT |
| NCC | :439 | NO_AI_COUNTDOWN_CLOCK=1 |
| NOPGIC | :446 | NO_PICKUPS_PREGO=1, NO_ICONCUBE=1 |

Two of the 12 keep a setter after the retirement and stay: `MASHED_NO_HEADING_ASSIST` (stack `NOHA`, kept at the
same line) and `MASHED_NO_A5_RACE_TIMER` (stack `NRTR`, `run_arms.py:453` at `e23f260b7`, the D3-SUBSTATE-ROW315
record-dump arm). That leaves **10** knobs, not the 8 the ledger title named. The title's 8 (ICONCUBE, the three
PONTOON, the four B3 leftovers) plus `MASHED_NO_PICKUPS_PREGO` (NOPGIC's other name) and `MASHED_NO_D2LAT_REFRESH`
(NOD2's first name). Both extra names are class (b) in the inventory (rows 102 and 78) and had no setter but these
stacks, so they are deleted too.

## 2. Change (commit `8bf1ba734`, 11 files, 67 insertions, 148 deletions)

`TR` = `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`, `AS` = `mashedmod/src/mashed_re/Ai/AiStandalone.cpp`.

- `Track/WarzonePontoon.cpp` / `.h`: `NoAnim` / `NoState` / `NoCarry`, `Pontoon_AnimOn` / `Pontoon_StateOn` /
  `Pontoon_CarryOn`, `StateLive` / `CarryLive`, `g_replay` and the file-local `Env` deleted. These also read the
  positive `MASHED_PONTOON_ANIM` / `_STATE` / `_CARRY`. With the opt-outs gone those positive reads had no effect, so they
  are gone as well. Trigger / Coupling / the carry and the record branch run unconditionally. The carry writes the car
  matrix in place (it used a scratch matrix only with CARRY off). `Pontoon_Init` loses its early return. The
  `MASHED_PONTOON_TRACE` init line keeps its format and prints `state=1 carry=1`. `TR:3035`, `TR:3079` drop the
  `Pontoon_AnimOn()` term.
- `TR:280` `PickupsPreGo` deleted (it read `MASHED_NO_PICKUPS_PREGO` and `MASHED_PICKUPS_PREGO=0`). Both callers run
  `PreGoRegistryTick` with their remaining condition (`TR:5368` unconditionally, `TR:5508` on `CamReplayRowRunsFc00`).
  The diagnostic `MASHED_PICKUPS_PREGO_NO_REFDIST` stays.
- `D3d9Render/PickupIconCube.cpp:60` `IconCube_Enabled` = `Gameplay::PickupsOrig_Enabled()` (the
  `MASHED_NO_ICONCUBE` / `MASHED_ICONCUBE=0` reads deleted).
- `AS` `AiLutMask`: `MASHED_AI_LUT=<mask>` if set, else 5. The `MASHED_NO_AI_LUT` read is deleted. `MASHED_AI_LUT=0`
  gives what it gave (exact sqrt).
- `AS` `Ai_ClockLateEnabled` deleted (decl in `Ai/AiStandalone.h`). `Ai_AdvanceClockLate` adds unconditionally, and
  `Ai_AdvanceClock` loses its knob-off add. `TR:5359` calls `Ai_RaceTimerCountdownTick(units, false)` unconditionally
  (`!subClock || true`).
- `AS` `Ai_CountdownClockEnabled` deleted (decl in the header). `TR:5365` is `if (subClock)`, and `TR:5413` keeps the
  spline-count gate only.
- `Race/RoundGrid.cpp` `GridDir_Enabled` deleted (decl in `RoundGrid.h`). `GridDir_Anchor` loses its early return, and
  `TR:7610` calls `GridDir_YawAt` unconditionally.
- `Vehicle/VehiclePhysicsRun.cpp` `s_d2latRefresh` deleted. The A4-skipped refresh takes `Math::LutLength` only, and
  the per-frame refresh `else if` drops the term.
- Comments that named a deleted knob as a live opt-out now say it was deleted.
- `verify/savelink_20261023/run_arms.py`: `OPTOUT` and the 9 stack definitions deleted, with a header comment naming
  this RESULT. `ast.parse` OK. No other stack is built from a retired one (grep for `STACK['<tag>']` reads and
  `OPTOUT` = definitions only).

Built exe check (`grep -ao` on the new `mashed_re.exe`, sha256 `d3fd1b1e`): `MASHED_NO_PONTOON_ANIM / _STATE / _CARRY`,
`MASHED_PONTOON_ANIM / _STATE / _CARRY`, `MASHED_NO_PICKUPS_PREGO`, `MASHED_NO_ICONCUBE`, `MASHED_ICONCUBE`,
`MASHED_NO_AI_CLOCK_LATE`, `MASHED_NO_AI_LUT`, `MASHED_NO_AI_COUNTDOWN_CLOCK`, `MASHED_NO_D2LAT_REFRESH` and
`MASHED_NO_GRID_DIR` occur 0 times. The remaining `MASHED_PICKUPS_PREGO*` string is `MASHED_PICKUPS_PREGO_NO_REFDIST`.
`MASHED_AI_LUT`, `MASHED_NO_HEADING_ASSIST` and `MASHED_NO_A5_RACE_TIMER` occur once each (kept).

## 3. Proof: no setter, no dangling reader

Reader check = `git grep -n 'getenv("MASHED_<name>")' -- mashedmod`. Live-setter check =
`git grep -lE 'MASHED_<name>\b' -- scripts re/tools re/frida re/orchestrator verify/savelink_20261023 '*.ps1' '*.bat'`
minus `*.md` and `roadmap.json` (text, not setters). Closed copies = `git grep -lE` over `verify/*.{py,sh,ps1,bat}`
outside `verify/savelink_20261023`.

| Knob | getenv readers | Live setters | Closed runner copies |
|---|---|---|---|
| MASHED_NO_PONTOON_ANIM | 0 | 0 (1 hit: the new `run_arms.py:20` header comment) | 90 |
| MASHED_NO_PONTOON_STATE | 0 | 0 | 90 |
| MASHED_NO_PONTOON_CARRY | 0 | 0 | 91 |
| MASHED_NO_PICKUPS_PREGO (+ `PICKUPS_PREGO=0`) | 0 | 0 (header comment `:21`) | 2 |
| MASHED_NO_ICONCUBE (+ `ICONCUBE=0`) | 0 | 0 (header comment `:22`) | 3 |
| MASHED_NO_AI_CLOCK_LATE | 0 | 0 (header comment `:21`) | 87 |
| MASHED_NO_AI_LUT | 0 | 0 (header comment `:21`) | 87 |
| MASHED_NO_AI_COUNTDOWN_CLOCK | 0 | 0 (header comment `:20`) | 89 |
| MASHED_NO_D2LAT_REFRESH | 0 | 0 (header comment `:21`) | 86 |
| MASHED_NO_GRID_DIR | 0 | 0 (header comment `:21`) | 86 |

No caller is left dangling: `git grep -nE 'Pontoon_(Anim|State|Carry)On|StateLive|CarryLive|Ai_ClockLateEnabled|Ai_CountdownClockEnabled|GridDir_Enabled|PickupsPreGo\('`
over `mashedmod` = 0, and the build links. No live script names a retired arm (`git grep -nE '\b[HBDW]L?_(NOCC|NOD2|NOCL5|NOL5|NCC|NOPGIC|OPT|NOPC|NOP)(_|-|\b)'`
over `scripts re/tools re/orchestrator .claude verify/savelink_20261023` = 0).
`re/orchestrator/DECISIONS.md` and `re/orchestrator/ledger/roadmap.json` name `MASHED_NO_ICONCUBE` / `_PONTOON_ANIM` /
`_AI_LUT` in decision and step text only. The closed copies are frozen evidence of lanes closed before 2026-10-30 (same
treatment as B1 / B2 / B3). Re-running one would make its opt-out arm identical to its default arm. None is an open
ledger step's runner (open steps at 2026-10-30: none names a runner copy).

Not changed, noted for the inventory:
- `Core/FaithDefaults.cpp:29-30` seeds the positive `AI_COUNTDOWN_CLOCK` and `GRID_DIR`. The exe read neither
  positive name before this step either (only the `NO_` forms), so the seeds were already inert. Untouched.
- `MASHED_SPEED_REFRESH=1` (`Vehicle/VehiclePhysicsRun.cpp:1743`, stack `SR`) feeds the per-frame refresh's second
  `else if`. That branch was already unreachable with the D2 refresh ON (default since d2ref 2026-10-17), and it stays
  unreachable now. It is a candidate for the next batch together with the `SR` stack.

## 4. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` = `=== Build OK ===` (shared `.asi` not deployed). No new
warning in a touched file (no C4101 / C4189 on the edited TUs).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py` from this branch, private root `D:/MashedWT/D5-KNOBS-RUNARMS-STACKS_root`
(robocopy of `original\`), under `mashed_lock.py run` (D2: `run --solo`) and `test-slot`. Refs from
`re/orchestrator/merge_train.json`: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9 x3`.

| Gate | This exe (`mashed_re.exe` sha256 `d3fd1b1e`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1917.10 / 1917.10 / 1920.48, slip1500 0.1933 / 0.1930 / 0.1930, all `in`, H1 398/400, motion_diag 1639 / 1641 / 1646 lines (min 1600, no re-run) | 3/3 in band (B3 2026-10-30: 1917.10 / 1919.62 / 1919.62) | same (3/3) |

The B step files are byte-identical to B3's (full sha256 `1d5bf4f980bf...` on all three, the value in
`verify/d5knobsb3_20261030/RESULT_D5-KNOBS-B3-AI-RACE.md` section 4), so `vsum.py` (b) and `ai_speed_env.py --check` (e),
both pure functions of those files, give B3's 3/3 and 3 `(e) PASS` with launch 1425.7 / 2052.5 / 2055.0. They were not
re-run. All six gate arms and the three D2 arms ran on the first attempt (no re-run, no piz collision). The gate arms
queued behind merge train `20261030_164237` for the machine lock. Committed copy: `score_d2.txt` in this folder.
The 13-track table was not run. The pontoon and IconCube edits remove opt-out branches only, and the train runs the
table under `-Full`.

## 5. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-RUNARMS-STACKS_data/` (31 MB, sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `arms/D2_DEFH_2.motion_diag.log` e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258
- `arms/D2_DEFH_3.motion_diag.log` 25850da6561def896b6932ff925541fc39c8ee4e452636875dfa51fd322d90b1
- `score_d2.txt` ed3407776a141666257f42d3ddf4ccbd6a84c78729d5b325e8630d56886d0da6
- `build.log` 8879db739465e4f27064b43bb05e08d3d8c035003192ed3a000fa69f88fc7ad9
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` 76eec78d61dae2c3163d4998a1bb9a1494e4c83fdfbce4a944d8f603fe718698
- `d2_run.log` ca1c5eab6d8a4f266eba2385c452ee629414b34a32d7270d70dbfd7c132a04b8
- exe `mashedmod/build/mashed_re.exe` d3fd1b1efd1bdf9f22fa99c398bbe20f63cfd752ad8bdc2785275f21cf6a1d97

## 6. For the lead

- No `.ps1` / `.bat` edit needed.
- 10 knobs deleted instead of the 8 in the brief (section 1). The 2 extra are the stacks' own other names.
- Next batch candidates: `MASHED_SPEED_REFRESH` + stack `SR` (section 3). `MASHED_NO_HEADING_ASSIST` goes once stack
  `NOHA` is retired, and `MASHED_NO_A5_RACE_TIMER` once `NRTR` is.
