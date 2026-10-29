# PREREG D5-ICONCUBE-GLOW (pickup glow on the rfx emitters)

Branch `orch/D5-ICONCUBE-GLOW` off `orch/integration` `327fd83c6`. DEFERRED D-11076, decision L-45 (glow after
the rfx emitters). Committed alone, before any arm runs.

STANDALONE-ONLY. The original does not boot this run (no audio endpoint, crash at `0x005baf48`). Original
behaviour comes from static reading of `original/MASHED.exe.unpatched` (decomp_pc.py, capstone) and from
captures already on disk. Any original-side parity leg is written up as PENDING, not run.

## 1. Change under test

- `Gameplay/PickupGlow.{h,cpp}` (new, exe-only): six cdecl shims for the sprite-queue callees the verbatim
  FUN_00459000 calls (`0x004769f0` colour, `0x004769a0` position, `0x004769d0` size, `0x00476a30` rotation,
  `0x00476d00` submit, `0x00476df0` flush), on the `Rfx::` core, plus the glow batch
  (`FUN_004770c0(&DAT_0068b968, 0x817, 0x19, puglow)`, `0x004587c5..0x004587d2`). The colour pointer is an
  address in the original's .data, so its index selects a table transcribed from the image
  (`DAT_005f9838`, 22 dwords + the zero dword after it).
- `re/tools/naked_specs/pickups_m2a.json` + regenerated `PickupTick_naked.inc`: those six calls bind
  `PK_Glow*` instead of `PK_Nop` (11 call sites, no other line changes).
- `LibRw/RwRaceFx.cpp`: `DrawPickupGlow` in `RaceFx_DrawWorldFx` between `DrawLensSmall` and `DrawHeadlights`
  (FUN_00410b30 order: `0x00410c23` FUN_004891f0, `0x00410c46` FUN_0045b990 -> `0x0045b996` FUN_00458b10,
  `0x00410c53` FUN_00413a00): Z-write 0 / Z-test 1, `RaceFx_DrawEmitter`, Z-write 1 / Z-test 1
  (`0x00458b22..0x00458b56`). Texture `PUGlow` from SFX.piz FX.TXD, loaded on first use.
- Knob: `MASHED_RFX_EMITTERS=1` (existing, default OFF, not in FaithDefaults) AND `PickupsOrig_Enabled()`.
  `MASHED_NO_PICKUP_GLOW=1` turns the glow off under the knob. Knob off: every shim returns at once (as
  `PK_Nop`), the draw sets no state. Diagnostic `MASHED_DBG_PUGLOW=<file>` (pool snapshots + particles).
- Not in scope: the IconCube clump draw and re-skin (D5-ICONCUBE-SPIN), retiring the PickupField orbs.

## 2. Arms and pass rules

| arm | what | pass |
|---|---|---|
| G0 | `mashedmod\build.bat` with `MASHED_NO_DEPLOY=1`; `scripts/lint_rva_bodies.py` inside it | build exit 0, lint adds no FAIL |
| A1 | default gates `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (private root, mashed_lock, test-slot) | the six sha8 equal `merge_train.json` `gctrl.ref` / `b.ref` |
| A2 | default 13-track table `run13.py run` + `score` vs `run13_reference` | 0 differing lines |
| A3 | knob ON gates: the same four arms with `MASHED_RFX_EMITTERS=1` (copied runner, stack `GLOW`) | the six sha8 equal the refs (the glow writes no sim state) |
| A4 | wiring check: one knob-ON H recipe run with `MASHED_DBG_PUGLOW`; `check_glow.py` recomputes each particle from the PRE / POST pool snapshot by the static formula (section 3) | 0 mismatching particles and 0 count mismatches over every tick; state-2 coverage reported (a run with 0 state-2 particles passes A4 for state 1 only and says so) |
| A5 | draw: 13-track run with `MASHED_RFX_EMITTERS=1` (knob ON) on Training rows 884 / 924 / 959; read the standalone draw records | ON: one `PUGlow` draw per row with `n = [4k, 2k]`, k >= 1, Z-write off; OFF (A2's run): no `PUGlow` draw. Report k, rs, bb next to the original's capture event (`training_orig_row_00884.draws.json` event 225: n [20, 10], rs [1,0,4,1,0,5,1,5,6,2,0,0]). k equal to the original's is reported, not required (pool state is not matched) |
| A6 | screenshots: crops of the ON vs OFF frames of A5 around the glow bb, plus the original's `orig_row_00884.bmp` crop | pixels differ ON vs OFF only inside the projected glow quads (imgdiff), report |

## 3. A4 formula (static, from the disassembly)

Entries `i = n-1 .. 0` (`0x0045901f..0x00459033`, n = `[0x0068b9a8]`). Type 0x15 forces state 0 (`0x00459036`).
- State 1 (`0x0045912b..0x00459226`): pos = POST frame+0x40 (FUN_00458a40 runs first, `0x004591e8`);
  d2 = ((dy*dy) + (dx*dx)) + (dz*dz) with d = PRE +0x2c - PRE +0x38 (float32 ops at PC_24); size =
  1.25 * clamp(1.0 - d2, 0, 1) (both axes, `[0x005ce540]` / `[0x005ce544]`; clamp `0x0045916c..0x004591b9`),
  size = 1.25 when d2 <= 0; colour = table[type]; rot = POST +0x04 * 0.017453292f (`0x0045920d..0x00459219`).
- State 2 (`0x00459057..0x004590dd`): only when PRE timer +0x1c > 0 and (PRE +0x18 - PRE +0x1c) < 1.0;
  pos = +0x38; size = 1.25 * sin(pi * (resp - timer)) (x87 `fsin`; checked within 1 ulp); colour =
  table[type]; rot = 0.
- Count = number of particles above, capped at 25 (`Rfx::Submit`).

## 4. Pending (not run, BLOCKED: needs the audio endpoint)

- P1: original-side PUGlow draw at a matched pose with a matched pool (live capture), against A5.
- P2: original-side `FUN_00459000` sprite-queue call log vs the A4 dump on one replayed pool.
