# RESULT carlod2: the brake byte behind flag 0x800, the per-livery part-0x13 origin, and FUN_004b5240 back to C3

**Verdict.** FUN_004b5240 is re-earned at C3 on a vector that actually exercises its clear arm. The brake byte is wired to
flag 0x800 exactly as the original reads it and the plate follows it one frame later, but on the three cited plate frames
the standalone AI brakes in the right place only once, so by the pre-registered rules the wiring ships **opt-in**
(`MASHED_CAR_BRAKE=1`), not default ON. The per-livery part-0x13 origin is default ON and changes no row. Damage and wreck
flags are filed, not ported (no cited row depends on them). Base `race/first-frame-parity` `07526b4e`; pre-registration
`PREREG_carlod2.md` (committed before the first measuring run) with amendment 1.

| item | what | default | result |
|---|---|---|---|
| F-BRAKE | F & 0x800 from the controller byte [5]: player `io.input[5]`, AI ctrl block [5], one frame delayed | OFF (`MASHED_CAR_BRAKE=1` on) | fails rules 1-2: 1 of 4 plates matched, 1 extra plate |
| F-ORG | each livery model's own part-0x13 origin for the LOD state and the env coefficient | ON | 0 rows changed; City 761 pixels 10.32 -> 10.28 |
| F-HOOK | `or_4b5240_set` vector (seed 0xFF) for `Or4b5240` | -- | path1 GREEN non-degenerate, negative control RED, path2 PASS, asi identity OK: C2 -> C3 |
| F-DMG | flags 0x10 / 0x20 / 0x400 | not ported | the 9 frames carry F = 0x8 / 0xc8 only; U-9706 (a) stays open |

Default build: `final8` (defaults) equals `base6` (clean `07526b4e`) in rows, matched draws and triangles on 9/9 frames; rows TRAINING
21/21/21, Arctic 32/35/36, City 20/22/24. G-CTRL `2b4d47bb / 356821c8 / f7a155e1` on the base, the wired and the final exe (`gctrl.txt`).

## 1. What the original does

`FUN_0040da50` reads `mov al, [ctrl*0x4c + 0x007f103d]` (`0x0040dae0`), byte [5] of the controller block, `setne` (`0x0040daed`) and hands it
to `FUN_0041eeb0` (`0x0040db3f`), which sets or clears F & 0x800. `FUN_00420870` (`0x0040dad0`) runs before that read in the same call, so it
tests the flags of the previous frame. The AI tick `FUN_00418860` runs from `FUN_00425a40` at `0x0040fc6c`, before `FUN_0040da50` at
`0x0040fc75`: the byte read at frame K is the AI output of frame K and the plate appears at K+1 (`evidence/dis_0040da50_loop.txt`,
`decomp_00425a40_0040d8f0.txt`).

## 2. F-BRAKE: what the arms show (no instrument input in any arm except `rows6`)

| arm | Arctic 1091 | Arctic 1126 | City 836 | TRAINING 924 | other 5 frames |
|---|---|---|---|---|---|
| `unwired6` (= carlod's default) | plate missing | plate missing | 2 plates missing | ok | ok |
| `wired6` | **matched** | plate missing | 2 plates missing | **EXTRA plate** | unchanged |
| `rows6` (carlod's instrument) | matched | matched | matched | ok | unchanged |

The brake log (`evidence/brakelog_*.csv`, per row and slot: byte, flag used) shows why, and that the delay is right:
- Arctic: slot 3 byte 255 at row 1090, flag 0x8c8 at 1091, the original's plate on slot 3 at 1091 (match). At 1125 slot 1 has byte 0; the
  original braked slot 1.
- City: slots 1 and 2 have byte 0 at rows 833-837; the original braked both at 835.
- TRAINING: slot 3 byte 255 at 923, plate at 924; the original has no plate there.

The AI writes non-zero brake bytes on 164-183 of 1449 frames per AI slot, so the wiring is live; the misses are where the standalone
AI, running on its own state with only the poses transplanted, makes a different braking decision. Rules: 1 (targets) fails (1 of 3
frames), 2 fails (TRAINING 924 EXTRA `Advantage`, cited C02 but new against `unwired6`), 3 passes (matched draws never drop: Arctic 1091
152 -> 153), 4 passes (`unwired6` = `base6`, `brakeon8` = `wired6`), 5 passes, 6 passes (largest rise +0.01). Default therefore OFF.

Found with it: supplying the bit (`rows6`) also removes rfx's `Headlight` MISSING (52/56 triangles) + uncited EXTRA (48) pair on the three
plate frames, so those RED rows on the base are the same missing brake input (U-9711). City 801 keeps its Headlight RED row (no plate
there; cause not mapped here).

## 3. F-HOOK: FUN_004b5240

The r115 vector seeded 0xF9 (bit 2 clear), so `flag = 0` left the byte unchanged on both sides (`evidence/diff_or_4b5240.csv`:
`0xF9F9F9F9` in, `0xF9F9F9F9` out) and could not tell a body without the clear arm. `or_4b5240_set` seeds 0xFF:
- path1 (`early_window_leaf_diff.py`, the handler that earned r115; `diff_template.js` has no `deref_struct_set`): GREEN 2/2, `flag=0`
  `0xFFFFFFFF -> 0xFFFBFFFF` on both sides, `flag=1` unchanged (`evidence/diff_or_4b5240_set.csv`).
- negative control, the pre-fix body built into a scratch `.asi`: RED on `flag=0` (`0xFFFFFFFF` vs the original's `0xFFFBFFFF`,
  `evidence/diff_or_4b5240_set_negctl.csv`).
- path2 (`run_verify_hook.py`, pointer test shape `p2bufs`): JMP installed, rel32 OK, the patched entry runs the port 1x
  (`evidence/path2_or_4b5240_set.txt`), inside `asi_window.py` (refused on any running MASHED, backup / deploy / restore, SHA-256 at every
  step, `evidence/asi_window.log`: deployed = mine, still mine at the end, restored = backup). `check_asi_identity.py`: rva 0x03ae40 =
  export rva, contaminated 0.

## 4. Gaps

U-9706 narrowed: (b) wired but opt-in, with the measured AI divergences; (a) damage / wreck bits, (c) slot-0 state modes and (d) S timing
stay open. Base already carries 4 uncited `Headlight` EXTRA rows (rfx, U-9711), unchanged by this lane.

## 5. Reproduce

```
mashedmod\build.bat                                                    (MASHED_NO_DEPLOY=1, one build at a time)
py -3.12 verify\carlod2_20261016\run_all.py ARM@EXE[@KNOB=V,...] ... gctrl@EXE@NAME
py -3.12 verify\carlod2_20261016\rowdelta.py <a> <b>
set MASHED_EW_ASI=<asi> && py -3.12 re\frida\early_window_leaf_diff.py or_4b5240_set
py -3.12 verify\carlod2_20261016\asi_window.py <asi> <log> -- py -3.12 re\frida\run_verify_hook.py or_4b5240_set
py -3.12 verify\dualcopy4_20261016\check_asi_identity.py <asi> <dir with path2_*.txt>
```
