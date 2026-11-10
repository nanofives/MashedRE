# RESULT PORT-OT-RWG-SLOTS: the 17 rows on RwGlobals +0x18 / +0x20 / +0x30 are released, with no exe change

Branch `orch/PORT-OT-RWG-SLOTS`: the handoff `66beb44a3`, plus orch/integration `e9fce5b22` merged in (`70d23bd65`),
so PORT-OT-MEASURE and PORT-OT-SEED-2 are both in. Worker cmwa3cvc21im9le1c8iibrc85 picked up the handoff from
cmw9srv5g098tle1c2ba4475l.

**No mashedmod change. No build, and no gate was needed: the default path is untouched.** The step is measurement plus
tooling.

## Verdict

All 17 rows the brief targeted read RwGlobals values that match the original, or are non-null call slots, on the
current ORIG_TOP exe. They are listed in `RWG_RELEASED.tsv`.
- Re-scored with the original's values: RwGlobals **OK 47, needs binding 25**. PORT-OT-MEASURE had OK 30, needs
  binding 42.
- No row went from OK to needs binding.

The 17 rows were blocked for two reasons, and neither one was a missing binding.

1. **+0x20 / +0x30 were already filled.** The PORT-OT-MEASURE probe (run 2) found the RwGlobals record
   (`0x00ff8290`) with +0x20 / +0x30 = 0 at all 8 samples up to 28 s.
   - That run started memory-starved: 10 min in the test-slot memory queue, and it had not finished 3000 frames
     30 s after its last sample.
   - The SEED-2 probe (`D:/MashedWT/PORT-OT-SEED-2_data/probe/probe.json`, record `0x00ff9290`) shows +0x20 =
     `0x00152bb0` and +0x30 = `0x00152760` from t = 3.03 s through 28.01 s. Those are the B15 bridge's
     `Bridge_SetRenderState` / `Bridge_DrawPrimitive` (`D3d9Render/RwIm2DBridge.cpp`).
   - The B15 fields are copied into the record by `MashedRw_InstallRwGlobals` after piece 4
     (`Match/PortTop_00402750_Pieces.cpp`, `PortTop_Piece_00403640` -> `LibRw/RwApiBind.cpp`). The SEED-2 run's exe
     log shows the copy: `RwGlobals: 0x007d3ff8 00F25170 (B15 device) -> record 00FF9290; Im2D +0x18/+0x20/+0x30
     copied`.
   - So MEASURE's zeros came from that run's timing: piece 4 had not run by its last sample. They were not a gap in
     the exe.
2. **+0x18 is 0.0f in the original.** The verdict counted any zero data field as missing. The new original-side
   probe reads RwGlobals +0x18 = `0x00000000` (0.0f) at all 7 samples where RwGlobals is set (1.53 s to 33.39 s).
   - The B15 device writes `z_field = 0` (`RwIm2DBridge.cpp`, `RwIm2DBridge_Install`), so the exe already holds the
     original's value.
   - The rows use it as the Im2D vertex Z. Example: `re/match/s06/0042aae0.c:69` `DAT_0067ec30[i].z =
     DAT_007d3ff8->f18`.

## Original-side probe (new: `ot_orig_probe.py`, extract `ORIG_RWG.json`)

**Setup:**
- Binary: `D:/MashedWT/PORT-BATCH-TOOL_root/original/MASHED.exe`, SHA-256 `b9977dab...fe87`. This is the patched
  install, byte-equal to `original\MASHED.exe`. No `mashed_re_dev.asi` in that folder, and MASHED_RE_NO_AUTO_HOOK=1.
- Run inside test-slot and `mashed_lock.py run --label PORT-OT-RWG-SLOTS`.
- 8 samples from 0.51 s to 33.39 s, then the probe killed its own PID 724 (rc 1). It only reads memory.

**Values at the last sample:**
- `*(0x007d3ff8)` = `0x02b499a8` (heap).

| RwGlobals + | original | exe ORIG_TOP (SEED-2 probe, 28 s) |
|---|---|---|
| +0x10 | `0x3f800000` (1.0f) | (not read) |
| +0x14 | `0x004c7a70` | (not read) |
| +0x18 | `0x00000000` (0.0f) | 0 |
| +0x1c | `0x3f800000` (1.0f) | (not read) |
| +0x20 | `0x004d7480` | `0x00152bb0` (Bridge_SetRenderState) |
| +0x24 | `0x004d6910` | 0 |
| +0x28 / +0x2c | `0x004db5b0` / `0x004db840` | (not read) |
| +0x30 | `0x004dba60` | `0x00152760` (Bridge_DrawPrimitive) |
| +0x34 | `0x004dbc90` | 0 |

- None of `0x004d7480`, `0x004dba60`, `0x004d6910`, `0x004dbc90` has a hooks.csv row.
- The other slots the rows use, as the original holds them (`ORIG_RWG.json` "slots"):

  | offset | value | offset | value | offset | value |
  |---|---|---|---|---|---|
  | +0x0 | 0 | +0x60 | `0x004de1b0` | +0xbc | `0x02b49a64` |
  | +0xc4 | `0x004a2b60` | +0xd0 | `0x004a34b0` | +0xdc | `0x004d8750` |
  | +0xe8 | `0x004af340` | +0xf4 | `0x004a9410` | +0xf8 | `0x004d86d0` |
  | +0xfc | `0x004d8700` | +0x118 | `0x004cca80` | +0x11c | `0x004ccba0` |

- +0x0 is 0 in the original as well. Row `0x0047ade0` uses it as a call slot, so it stays at needs binding until
  someone reads that row's use. [UNCERTAIN] Whether +0x0 is set later than 33 s or on another path: nothing past the
  menu was sampled.

**Module-offset globals:** all four read 0 at 0.51 / 1.53 / 3.10 s and are set from 6.01 s on.

| global | offset | module data at RwGlobals + offset (dwords) |
|---|---|---|
| `0x007d3ffc` | `0x134` | +0 `0x0cc00048`, +4 `0x0cc04050` (heap), +8 `0x004dc4a0`, +0xc `0x004dc370`, +0x10 `0x004dc530`, +0x14 `0x004dc410`, +0x18 `0x007d4004`, +0x1c `0x00020000` |
| `0x007d4028` | `0x14c` | +0 `0x007d4004`, +4 `0x00020000`, +8 `0x005cb2a0`, +0xc / +0x10 / +0x14 `0x3c23d70a`, +0x18 `0x007d3e78`, +0x1c `0x007d45a0` |
| `0x007d4054` | `0x3f4` | +0 `0x0e6252c4`, +4 `0x0cc14c34`, +8 `0x007d405c`, +0xc `0x007d4030`, +0x10 0, +0x14 `0x004c61b0`, +0x18 `0x004c7490` |
| `0x007d4628` | `0x170` | +0 `0x007d4630`, +4 `0x02b4edb0`, +8 `0x00000100`, then bytes 00 01 02 ...; +0x218 `0x007d4604`, +0x21c `0x0cc0d5ec` |

- `0x134 + 0x18 = 0x14c`: the 0x007d3ffc module's +0x18 / +0x1c are the 0x007d4028 module's +0 / +4.
- The +8..+0x14 values of the 0x007d3ffc module agree with `Math/RwV3dTransformPointsCPU.cpp:61` (batch probe o_bq3).

## Module offsets: not registered in this step

The brief asked to register 0x007d3ffc and the other three. I did not, for two reasons.
- **Exe code reads these globals today, so writing them changes behaviour.**
  - `Math/RwLutGuard.h` (`Resolve`, which reads `0x007d3ffc`) and the RwSqrt / Vec3 / RwV2d / RwV3dNormalize
    callers: a registered math module whose LUT roots point at `RwLutSqrtTable()` / `RwLutInvSqrtTable()` passes
    the sentinels, so those callers move from the CPU fallback to the LUT path.
  - `Collision/RwpSolverMath2.cpp:90` `RWP_MATMUL = *(DAT_007d4028 + 8 + DAT_007d3ff8)` is a call through the
    0x007d4028 module.
  - Also reading them: `Math/RwMatrixRotateInner.cpp:45`, `Render/RwMatrixInvert.cpp:215`,
    `Render/Vec3NormalizeScale.cpp:49`, `Math/RwV3dTransformPoints.cpp:37`, `Render/TextureLoaderCluster.cpp:60`.
  - An offset with zeroed module data behind it would send those calls to address 0.
- **The record is too small.** `MashedRwGlobals` ends at +0x218 (`LibRw/RwApiBind.cpp`). The 0x007d4628 module needs
  +0x170 + 0x21c = 0x38c, and 0x007d4054 sits at +0x3f4.

So registration is real port work, and it changes knob-path behaviour. It needs its own default-OFF knob on top of
MASHED_ORIG_TOP plus a PREREG, with every module slot bound to an exe body, a librw equivalent, or an original-equal
table:
- `Math/RwLutTables.h`: the LUTs equal the original's
- `Math/RwV3dTransformPointsCPU.cpp`: forms of +0xc and +0x14

The 12 module rows stay at needs binding until that happens.

## Remaining 25 needs-binding rows (`OT_MEASURE.tsv`)

| blocker | rows |
|---|---|
| module 0x007d3ffc | 7: 0x004c3b30, 0x004c3b90, 0x004c3bf0, 0x004c3d60, 0x004c3d90, 0x004c3dc0, 0x004c3df0 |
| call +0x11c | 4: 0x00554050, 0x00555280, 0x00555830, 0x00556e40 |
| other call slots | 0x004113b0 (+0xc4), 0x00413a40 (+0x24 +0x34), 0x00427840 (+0xf4), 0x004430e0 (+0x34), 0x0047ade0 (+0x0), 0x004d52d0 (+0x60), 0x00555910 (+0xe8 +0xf4), 0x005adf60 (+0xe8) |
| data +0xbc | 0x004c1680 (original `0x02b49a64`, a heap pointer) |
| modules 0x007d4028 / 0x007d4054 / 0x007d4628 | 0x004c4600, 0x004c5770, 0x004c5890, 0x004ce790, 0x004ce8e0 (some also need call slots) |

## Tooling changes

- `ot_orig_probe.py` (new): the original-side twin of `ot_measure.py probe`. It uses the same row offsets, and also
  dumps the RwGlobals block 0x0..0x200 and 0x40 bytes of each module.
- `ot_measure.py verdict --orig <orig_probe.json>`: a zero data field counts as OK when the original also holds 0
  there. Each `rwg_detail` now ends with the original's value per offset.
- `OT_MEASURE.tsv` is regenerated from the SEED-2 probe plus `--orig`. The seed columns changed too (111 OK / 13 needs
  seed rule), because the SEED-2 seed is in: it is the current state, not this step's work.

## Evidence (raw data in `D:/MashedWT/PORT-OT-RWG-SLOTS_data`, not committed)

| file | SHA-256 |
|---|---|
| orig/orig_probe.json (original-side probe, 8 samples) | 0974155d26974e855ca3755664baf5d19d0a80ef9e77784dd91034f7f75e90b0 |
| orig/orig_probe_run.log | a2dac43f900255d7b1b8848f792eaf47b61ffd59ef4768be3e4ca72c88b6ed59 |
| orig/ot_rows.json (= PORT-OT-MEASURE / SEED-2 ot_rows.json) | a6dc8309f8ceaf08fc4df5b1694d27c9909422c8ed64383920d2c5e43bd87641 |
| D:/MashedWT/PORT-OT-SEED-2_data/probe/probe.json (exe ORIG_TOP probe, verdict input) | a6daa9a9fd451d5e6d6d1606d3c345d558f25b04118b2f1b1c282415a9785a5a |
| OT_MEASURE_before.tsv (the committed MEASURE table, before re-scoring) | de4277b431b088c833ad97273ad6e1ee161f57fd0b23e3c0451e3a1d9d1b0cc4 |
| PORT-BATCH-TOOL_root/original/MASHED.exe (the probed binary) | b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87 |

Committed: `ORIG_RWG.json` (last-sample extract plus the per-sample +0x18 / +0x20 / +0x30), `RWG_RELEASED.tsv`,
`OT_MEASURE.tsv`, `ot_orig_probe.py`, `ot_measure.py`.

## REPORT

```
REPORT PORT-OT-RWG-SLOTS
verdict: GREEN
branch: orch/PORT-OT-RWG-SLOTS @ <see commit>
evidence: verify/portorigtop_20261109/RESULT_PORT-OT-RWG-SLOTS.md
gates: not run: no mashedmod change (measurement + verify tooling only)
decision: none
next: PORT-OT-RWG-MODULES (new row): register the 4 RW module offsets on ORIG_TOP behind a default-OFF knob + PREREG, record grown past +0x3f4, every module slot bound (LUTs RwLutTables, +0xc/+0x14 RwV3dTransform*CPU); releases up to 12 rows
```
