# PREREG carlod2: the brake byte behind flag 0x800, and the C3 re-earn of FUN_004b5240

Committed before the first measuring run. Base: `race/first-frame-parity` `e825a58c` (carlod merged), branch
`work/carlod2`. Instrument and rules are carlod's (`verify/carlod_20261016/PREREG_carlod.md` section 3, amendments 4-6),
re-used through copies of its harness in this directory (`log/carlod2/`): d1r1's matched-pose per-draw recorder and
`re/tools/drawlist_diff.py` 3D mode on its 9 frames (TRAINING 884/924/959, Arctic 1051/1091/1126, City 761/801/836),
original-side records and traces reused unchanged. Unlike carlod, **no arm passes `MASHED_CAR_BRAKE_ROWS`** (the runner's
`--rows` flag is off by default): that instrument input is what this lane replaces.

## 1. What the original does (static, read before measuring)

All RVAs in `MASHED.exe` (anchor in CLAUDE.md); listings in `evidence/`.

**A. The brake byte.** `FUN_0040da50` @ `0x0040da50`, per car slot esi (`0x0040da80..0x0040db4b`, ebx steps 0x10 over the slot
table at `0x007f1a14`): for an active car it calls `FUN_00420870` (`0x0040dad0`, which uses the flags as the PREVIOUS call
left them), then `FUN_0040d8f0`, then reads the controller block's byte `mov al, byte [ecx + 0x007f103d]` with
`ecx = [ebx] * 0x4c` (`0x0040dadb..0x0040dae0`), i.e. byte `[5]` of the block `0x007f1038 + ctrl*0x4c` (the "brake / reverse"
byte of `Ai/AiState.h`, `[4]` being the accelerator), `setne dl` (`0x0040daed`), kept in ebp and passed as the fourth argument of
`FUN_0041eeb0` (`push ebp` `0x0040db3b`, `call` `0x0040db3f`), which sets or clears `F & 0x800`. An inactive car skips the read
and `FUN_0041eeb0` gets the previous car's ebp (`0x0040da98..0x0040dabe`).

**B. Frame order.** `FUN_0040fc00` calls `FUN_00425a40` at `0x0040fc6c`, which calls the AI tick `FUN_00418860` (decomp of
`0x00425a40`, `evidence/decomp_00425a40_0040d8f0.txt`), and `FUN_0040da50` at `0x0040fc75`. So the byte read for flag 0x800 at
frame K is the AI's output of frame K, and the flags `FUN_00420870` tests at frame K are the ones set at frame K-1: **the plate
shows one frame after the byte.** `FUN_0040d8f0` touches no controller byte (it tests `DAT_007f101c & 2`).

**C. The standalone.** `Ai_Standalone_Tick` runs every race frame and its outputs are the ctrl blocks at
`Ai::kCtrlBlockBase + Ai::I32(Ai::kSlotTableBase + v*Ai::kSlotTableStride) * Ai::kCtrlBlockStride` (`TrackRenderer.cpp`,
the opponents' `blk[k]` that feed `VehiclePhysics_StepCar`), the same block and byte the original reads. The player's byte is
`io.input[5]` (`TrackRenderer.cpp:3919`, `(accel < 0 ? -accel : 0) * 255`), which the standalone keeps in a local. The default
build today leaves `F & 0x800` at 0 (carlod `CarVisFlags(course, false)`).

**D. FUN_004b5240 and its hook.** The binary: `or cl,4` (`0x004b524f`) when the flag is set, else `and cl,0xfb`
(`0x004b5256`), `mov [eax+2],cl`. carlod fixed `Or4b5240` to match and demoted the row to C2 because the old C3 vector
(`hooks_registry.py` `or_4b5240`, `seed_byte` 0xF9, `path1_tests [[1],[0]]`) never exercises the clear: 0xF9 has bit 2 clear
(`0xF9 = 1111 1001`), so `flag = 0` leaves the byte unchanged on both sides and cannot tell a body that clears from one that
does not.

## 2. Work items and fixes

- **F-BRAKE.** `F & 0x800` from the controller bytes: slot 0 from the player's `io.input[5]` (kept in a member where the
  physics step builds it, the same expression for the non-physics path), slots 1..3 from the ctrl block byte `[5]` of their
  slot-table entry, each through a one-frame delay (B). Guards: AI slots only while the ported tick runs (`Vehicle::
  VehiclePhysics_Enabled()` and the AI bridge loaded), else 0. `MASHED_CAR_BRAKE=0` turns the wiring off (the carlod
  behaviour); `MASHED_CAR_BRAKE_ROWS` stays as an override for A/B arms. Diagnostic `MASHED_CAR_BRAKELOG=<csv>` (per row, slot:
  byte, delayed byte, flags), default unset.
- **F-HOOK.** A second registry vector `or_4b5240_set` for `Or4b5240`: `seed_byte` 0xFF (bit 2 set in every byte), observed
  dword at offset 0 (it contains `p[2]`), `path1_tests [[0],[1]]`: with `flag = 0` the original clears bit 2 (0xFF -> 0xFB at
  `p[2]`) and with `flag = 1` leaves it. Accepted when `run_diff.py or_4b5240_set` is GREEN with both sides changed on the
  `flag = 0` vector (non-degenerate), `run_verify_hook.py or_4b5240` confirms the install, and a negative control (the pre-fix
  body, `git show b26d9f3c:mashedmod/src/mashed_re/Util/PromoLoop_sessionB.cpp`) is RED on the same vector. Then `re-classify`.
- **F-DMG** (damage / wreck flags `0x10`, `0x20`, `0x400`: `FUN_0041ede0`, `FUN_0041ef80`) and **F-ORG** (per-slot livery
  part-0x13 origin): done only if small and cited. F-DMG needs the standalone's own state for the setters' conditions and a cited
  row that depends on it (the 9 frames show `F = 0x8 / 0xc8`, never a damage bit); otherwise it is filed with the callers read.
  F-ORG changes `part19_d2`'s operand for slots whose livery model differs; all four cars of the 9 frames are `Advantage`, so it is
  expected to change no row (Pr-3).

## 3. Rules (per fix, on all 9 frames; carlod's, with the arms below)

1. **Targets.** F-BRAKE: the three NEW MISSING `Advantage` rows that the unwired build has on Arctic 1091 and 1126 and City 836
   (the 4-triangle part-0x11 plates, 1 + 1 + 2 draws) are absent.
2. **No new rows** against the arm with the per-part visibility off (`voff`, the pre-carlod behaviour, whose plates matched by
   accident): no row appears on any of the 9 frames, in particular no EXTRA `Advantage` row on the six frames where the original
   draws no plate, and every row stays cited (0 RED).
3. **Matched draws and triangles do not drop** against `voff`, per frame.
4. **No-op controls.** `MASHED_CAR_BRAKE=0` reproduces the `unwired` arm; the new exe with `MASHED_CAR_VIS=0` reproduces `voff`
   and the base exe's rows.
5. **G-CTRL** (`run_gctrl.py`, TRAINING default arm) on the base exe and the final exe, equal.
6. **Pixels (report).** No frame rises above +0.5 against `voff`.

## 4. Arms

On one new exe (knobs): `voff` (`MASHED_CAR_VIS=0`), `unwired` (`MASHED_CAR_BRAKE=0`, no rows: the carlod build's failure on three
frames, re-measured), `wired` (defaults, no rows: the arm that decides), `rows` (`--rows`, the carlod instrument, for
comparison). Base exe: `exe_base5` (clean `e825a58c`), arm `base5` with `--rows` (carlod's final).

## 5. Predictions

- Pr-1: `wired` has no EXTRA `Advantage` row on the six plate-free frames (the standalone's AI brakes on none of them at those rows).
- Pr-2: on the three plate frames `wired` lights the plates only if the standalone's AI output `[5]` at the previous frame equals
  the original's. The standalone's AI runs on its own car state and only the poses are transplanted, so this is **not assured**;
  the diagnostic log decides which of three outcomes it is: (a) match, rules 2-3 pass; (b) the standalone's byte is 0 where the
  original's is set (the AI differs); (c) the standalone sets it elsewhere (EXTRA row). A (b) or (c) is reported with the bytes, not
  hidden, and filed as the AI's fidelity gap; it is not fixed here by transplanting the original's bytes into the default build.

## 6. Amendments

(none yet)

### Amendment 1 (written after the arms on 07526b4e were read; no rule changes)

- Base moved: rebased onto `07526b4e` (clean rebase); baseline `exe_base6` built from a clean `07526b4e` scratch worktree, alone.
  Arms renamed with suffix 6 (`base6`, `voff6`, `unwired6`, `wired6`, `rows6`); G-CTRL is `2b4d47bb / 356821c8 / f7a155e1`.
- Pre-existing RED rows: `base6` already has 1 uncited EXTRA `Headlight` (48 triangles, screen-space) on Arctic 1091 / 1126 and City
  801 / 836, identical in `voff6`; they belong to rfx's beam, not this lane, and rule 2 is read as "no row beyond those".
- Harness for FUN_004b5240: its arg_type `deref_struct_set` has no handler in `diff_template.js` (never had), so path1 is
  `re/frida/early_window_leaf_diff.py` (the harness that earned r115) with `MASHED_EW_ASI` pointing at the build under test, not
  `run_diff.py`. path2 (`run_verify_hook.py`) needs the pointer test shape `{p2bufs, p2args}`; the entry's `path2_tests` uses it.
  path2 runs inside `asi_window.py` (backup, refuse on any running MASHED, deploy, restore in finally, SHA-256 at every step) and is
  checked with `verify/dualcopy4_20261016/check_asi_identity.py` (orchestrator rule of 2026-10-16).
- F-BRAKE default: the decisive arm `wired6` fails rules 1 and 2 (section 3) because the standalone AI's brake output differs from the
  original's at the cited rows (Pr-2 outcomes (b) and (c)), so by section 3 the wiring ships OPT-IN (`MASHED_CAR_BRAKE=1`), and the
  final exe's default arm `final8` must equal `base6` in rows.