# PREREG PORT-OT-RWG-MODULES: register the four RW modules on the MASHED_ORIG_TOP path (knob MASHED_ORIG_TOP_RWMOD)

Committed alone, before any arm runs. Branch `orch/PORT-OT-RWG-MODULES` (orch/integration `1be11e86d` +
orch/PORT-OT-RWG-SLOTS `f4b994948`).

## Knob

`MASHED_ORIG_TOP_RWMOD=1`, default OFF. Read once in `LibRw/RwApiBind.cpp` (`RwModKnob`). It acts only inside
`MashedRw_InstallRwGlobals`, which runs only on the MASHED_ORIG_TOP path (`Match/PortTop_00492370.cpp`,
`Match/PortTop_00402750_Pieces.cpp`). Without MASHED_ORIG_TOP the knob is never read.

ON, at the RwGlobals install after piece 4 (the first install that sees the B15 device):
- RwGlobals record grows from 0x218 to 0x440 bytes (always; the bytes past +0x114 stay zero with the knob OFF).
- The four module-offset globals get MASHED.exe's values (original-side probe, `ORIG_RWG.json` "mods"):
  `*(0x007d3ffc) = 0x134`, `*(0x007d4028) = 0x14c`, `*(0x007d4054) = 0x3f4`, `*(0x007d4628) = 0x170`.
- Module data in the record, each slot bound to an exe body, a table equal to the original's, or MASHED.exe's value:
  - vector +0x134: +0x0 / +0x4 `RwLutSqrtTable` / `RwLutInvSqrtTable` (original heap LUT roots, sentinels of
    `Math/RwLutGuard.h`); +0x8 point x1 (0x004dc4a0), +0xc `RwV3dTransformPointsCPU_C` (0x004dc370), +0x10 vector x1
    (0x004dc530), +0x14 `RwV3dTransformVectorsCPU_C` (0x004dc410).
  - matrix +0x14c: +0x0 0x007d4004, +0x4 0x00020000, +0x8 `RwMatrixMultiplyCPU_C` (0x005cb2a0), +0xc..+0x14 0x3c23d70a.
  - image +0x170: +0x218 0x007d4604, +0x21c 0 (empty format list: the registrations of 0x00493600 are unported).
  - texture +0x3f4: +0x0 / +0x4 empty list (self-linked), +0x8 0x007d405c, +0xc 0x007d4030, +0x1c 0.
- The freelist headers at 0x007d4004 / 0x007d4030 / 0x007d405c / 0x007d4604 get MASHED.exe's words +0x0 / +0x4 / +0x8 /
  +0xc / +0x18 (original-side probe dumps), block list empty.
- RwGlobals +0x118 / +0x11c (original 0x004cca80 / 0x004ccba0, freelist alloc / free): per-entry allocation through
  the librw memory functions, size and alignment from the header; free of an entry it did not hand out returns 0.

Behaviour this changes with the knob ON (the reason for the knob): the exe readers of those globals stop seeing
offset 0. `Math/RwLutGuard.h` callers find the LUT roots and take the LUT path; the module call slots are no longer
null.

## Arms and pass criteria

| arm | what | pass |
|---|---|---|
| A0 default gates | `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this branch's exe | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3 (`merge_train.json`) |
| A1 knob boot | MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 MASHED_DET_FRAMES=3000 | exit 0; log has the `modules registered` line |
| A2 measure | `ot_measure.py probe` with RWMOD=1, `verdict --orig` (the PORT-OT-RWG-SLOTS original probe) | RwGlobals OK count reported per row. Prediction: 61 OK / 11 needs binding (was 47 / 25): the 7 vector rows, 0x004c4600, 0x004c5770, 0x004c5890 and the four +0x11c rows (0x00554050, 0x00555280, 0x00555830, 0x00556e40) released; 0x004ce790 (+0xd0 +0xe8 +0xf8 +0xfc) and 0x004ce8e0 (+0xdc +0xe8, empty format list) stay |
| A3 captures | `capflake_runs.py` OFF ON OFF ON, MASHED_ORIG_TOP_RWMOD=1 in the environment of every run | all four runs class E (12/12 equal to the E list) |
| A4 lint | `py -3.12 scripts/lint_rva_bodies.py` | `[rva-lint] OK` |

A0 or A4 failing is RED. A1 failing is RED for the knob. A3 with an ON run not class E is reported with the
differing walks; the knob stays OFF either way (no flip is proposed by this step).
