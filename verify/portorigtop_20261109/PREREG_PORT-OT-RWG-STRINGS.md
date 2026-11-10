# PREREG PORT-OT-RWG-STRINGS: RwGlobals string slots on MASHED_ORIG_TOP_RWMOD

Committed alone, before any arm. Branch `orch/PORT-OT-RWG-STRINGS`, stacked on orch/PORT-OT-RWG-MODULES `07a9a01c3`
(PREREG_PORT-OT-RWG-MODULES.md defines the knob).

## Change

Same knob, `MASHED_ORIG_TOP_RWMOD=1` (default OFF, read only on the MASHED_ORIG_TOP path). `RegisterRwModules`
(`LibRw/RwApiBind.cpp`) also fills seven RwGlobals string slots. The MASHED.exe values come from the original-side probe
(RwGlobals block at 33.02 s, `D:/MashedWT/PORT-OT-RWG-MODULES_data/orig/orig_probe.json`):

| slot | original | binding |
|---|---|---|
| +0xc4 | 0x004a2b60 (libcmt `_sprintf`, libid) | `vsprintf` wrapper |
| +0xd0 | 0x004a34b0 (`_strncpy`) | `strncpy` |
| +0xdc | 0x004d8750 (RwStrrchr, C4) | exe body `RwStrrchr` (Render/RwStrSearch.cpp) |
| +0xe8 | 0x004af340 (CRT strcmp shape, 0x004af340..0x004af38b) | unsigned byte compare, returns -1 / 0 / +1 as 0x004af384..0x004af38b |
| +0xf4 | 0x004a9410 (`_strlen`) | `strlen` |
| +0xf8 | 0x004d86d0 (RwStrupr, C4) | exe body `RwStrupr`, returns s (eax = s through the original body) |
| +0xfc | 0x004d8700 (RwStrlwr, C4) | exe body `RwStrlwr`, returns s |

Not in this step: the format registrations of 0x00493600. Its four read / write arguments (0x004935a0, 0x0052d530,
0x004935d0, 0x00513f90) have no exe body. Registering the formats with null functions would make the image list
non-empty without the behaviour behind it.

## Arms and pass criteria

| arm | what | pass |
|---|---|---|
| A0 default gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3 |
| A1 knob boot | MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 MASHED_DET_FRAMES=3000 (`ot_measure.py probe`) | exit 0; the registration log line names the string slots |
| A2 measure | `ot_measure.py verdict --orig` | Prediction: RwGlobals OK 65 / needs binding 7 (was 61 / 11). Released: 0x004113b0, 0x00427840, 0x00555910, 0x005adf60. 0x004ce790 and 0x004ce8e0 stay blocked on the empty image format list (+0x21c). |
| A3 captures | `capflake_runs.py` OFF@0 ON@0 OFF@0 ON@0, RWMOD=1 in every run's environment | all 12 hashes equal across OFF and ON, and equal to the PORT-OT-RWG-MODULES runs (walk_04 = 4d53d93f87f0332d, the BAR_Y default) |
| A4 lint | `py -3.12 scripts/lint_rva_bodies.py` | `[rva-lint] OK` |
