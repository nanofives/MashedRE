# PREREG D4-DC-INC-0046d4a0-KNOB: the bump arm's record-pointer call through the one shared body

Committed alone, before any knob-ON arm. Ledger rows: D4-DC-INC-0046d4a0-KNOB (this), D4-DC-INC-0046d4a0 (blocked burn-down,
closes with the flip).

## 0. What is being changed

The exe has two bodies for `FUN_0046d4a0` (allowlist key `DUP-IN-TARGET:0046d4a0`):

- `PK_46d4a0`, `Gameplay/PickupCollect_naked.inc` (spec `re/tools/naked_specs/pickups_m3.json`), verbatim: `idx >= 0x10`
  returns 0 at `0x0046d4a9` without writing, else `*out = 0x00881ec8 + idx*0xd04 + [0x00881f48 + idx*0xd04]*0x40`
  (`0x0046d4ac..0x0046d4c6`), returns 1 (`0x0046d4c8`). In the exe both absolutes are blank image pad (the record array is
  `g_vehicleArrayBase`, `Vehicle/VehicleRecordBase.h:9-13`).
- `PtrCompute881ec8`, `Vehicle/VehicleRecordPtr.cpp`, the one shared body (both targets). Same arithmetic on
  `VehRecord(idx)`, so in the exe it reads the rebound array. It returns 0 without writing when the array is not allocated.

`PK_46d4a0`'s only caller is `PK_458570` (`FUN_00458570`, call at `0x00458581`), reached only from the bump arm of
`PK_458920` (`FUN_00458920`, call at `0x004589dd`).

The knob, `MASHED_PK_ONEBODY_46D4A0` (default OFF; `=1` ON): the spec maps the `0x00458581` call to `PK_46d4a0_sel`
(`Gameplay/PickupTick.cpp`). OFF calls `PK_46d4a0`, ON calls `PtrCompute881ec8`. The regenerated `.inc` differs from the
committed one by that one call line. `MASHED_PK_ONEBODY_46D4A0_WIT=<path>` logs, per call, both forms' return, pointer
and the `+0x30/+0x34/+0x38` dwords each reads (diagnostic, either knob state).

## 1. Why the result is predicted to be unchanged

From the transcription in `Gameplay/PickupCollect_naked.inc`:

- `FUN_00458570`'s ST0 is popped by the caller at `0x004589e2` (`fstp st`).
- Its out pointer is `ESI = [esp+0x18]` (`0x004589c1`). The caller then reads `[esp+0xc]`, `[esp+0x10]`, `[esp+0x14]`
  (`0x004589e4..0x004589ec`), which it set itself at `0x004589c5..0x004589d5` to `0, 0x3d23d70a, 0`, and writes them to
  entry `+0x44..+0x4c` (`0x004589f6..0x004589fb`). `[esp+0x18]` is not read after the call.
- `FUN_00458570` writes only `[esi]` and its own frame. Its callees `RwLutLength_cd` / `RwLutNormalize_cd` write only the
  out buffer (PREREG_M3.md section 0.3, `verify/d3_mode7_m3_20261010/`).

So the pointer `FUN_0046d4a0` returns changes the values `FUN_00458570` computes, and nothing it computes is read.

## 2. Arms (all on the worktree exe, `verify/savelink_20261023/run_arms.py`, private root, under `mashed_lock.py`)

| Arm | Stack | Knob |
|---|---|---|
| `H_DEF`, `B_DEF_K0`, `B_DEF_K1`, `B_DEF_K26` | `DEF` | OFF (default) |
| `H_PK46`, `B_PK46_K0`, `B_PK46_K1`, `B_PK46_K26` | `PK46` | ON |
| `H_PK46W` | `PK46W` | ON + witness |
| `H_DEFW` | `DEFW` | OFF + witness |

## 3. Gates (predictions, fixed now)

| Gate | Prediction |
|---|---|
| **G-DEF** (knob OFF, default) | `H_DEF` = `8284be46 / ba407a16 / 31fe043e`, `B_DEF_K0/K1/K26` = `1d5bf4f9` x3 (`re/orchestrator/merge_train.json` refs) |
| **G-ON** (knob ON) | `H_PK46` and `B_PK46_K*` = the same refs, byte for byte |
| **G-EXEC** | `H_PK46W.pk46.tsv` has at least 1 call row. If it has 0, G-ON is vacuous for this scenario and the RESULT says so (no flip filed on a vacuous G-ON) |
| **G-READS** | every witness row with `0 <= idx < 0x10`: `naked_ret = 1`, `naked_ptr = 0x00881ec8 + idx*0xd04 + t*0x40`, `one_ret = 1`, `one_ptr != naked_ptr`. Report only: the naked form's `+0x30..+0x38` dwords and the one-body form's |
| **G-WIT-INERT** | `H_PK46W` = `H_PK46` hashes, `H_DEFW` = `H_DEF` hashes (the witness reads only) |
| lint | `scripts/lint_rva_bodies.py` NEW=0 with the knob in (the key stays until the flip) |

Any G-DEF miss is RED for this branch (the knob commit must not move the default). A G-ON miss is RED for the flip: the
RESULT then lists the first divergent frame and no flip is filed.

## 3a. Amendment 1 (committed before its arms): G-EXEC failed on the gate scenario

Section 2's ten arms ran (exe `5e0df7b4`, commit `910c5e6f2`). G-DEF, G-ON and G-WIT-INERT matched the refs, but neither
witness file was created: the selector opens it on its first call, so `H_PK46W` and `H_DEFW` made 0 calls. The gate
recipe (Training, round rule 0, game mode 10, `[fflip 2026-10-21] recipe F`) never reaches the bump arm, so G-ON is
vacuous there. The 12 live bumps of `M3B_ON` (`verify/d3_mode7_m3_20261010/RESULT_M3.md` section 3, ticks 3691..3702,
car 3 holding 9 over pool 3) ran on the earlier recipe, round rule 4 without game mode 10.

Added arms (`_R4` = `MASHED_ROUND_RULE=4`, `-MASHED_GAME_MODE`, both with the witness, so the knob is the only difference):

| Arm | Stack | Knob |
|---|---|---|
| `H_DEFR4W` | `DEFW` + `_R4` | OFF + witness |
| `H_PK46R4W` | `PK46W` + `_R4` | ON + witness |

| Gate | Prediction |
|---|---|
| **G-EXEC-R4** | `H_PK46R4W.pk46.tsv` and `H_DEFR4W.pk46.tsv` have at least 1 call row each, the same count, the same `idx` sequence. If 0, G-ON stays vacuous: the RESULT reports it and no flip is filed |
| **G-ON-R4** | `H_PK46R4W` step / gates / ptrace hashes = `H_DEFR4W`'s, byte for byte (no ref exists for this recipe) |
| **G-READS-R4** | as G-READS, on every row of both files |

## 4. The flip (filed as a decision, not done here)

On G-DEF + G-ON + G-EXEC PASS (or, per amendment 1, G-DEF + G-ON + G-EXEC-R4 + G-ON-R4 PASS): map `"0x0046d4a0"` in `pickups_m3.json` to `PtrCompute881ec8`, drop `PK_46d4a0` from the
spec's `funcs`, regenerate the `.inc`, delete `PK_46d4a0_sel` and the knob, and remove the allowlist key
`DUP-IN-TARGET:0046d4a0`. Expected default hashes after the flip: unchanged, G-CTRL `8284be46 / ba407a16 / 31fe043e`,
B `1d5bf4f9` x3. D4-DC-INC-0046d4a0 closes with the flip.
