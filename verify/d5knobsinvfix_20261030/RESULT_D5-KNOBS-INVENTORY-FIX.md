# RESULT D5-KNOBS-INVENTORY-FIX (2026-10-30)

Text-only correction of the default column of `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md` (class d rows 1-126, table lines 481-606) against `S/Core/FaithDefaults.cpp` `kFaith` (lines 28-70, `S/` = `mashedmod/src/mashed_re/`). Source of the findings: `verify/d5knobsd2_20261030/RESULT_D5-KNOBS-D-TRIAGE-2.md` findings 1-3. No source, script, tracker or build input touched, no build, no game run.

**Verdict: GREEN. 126 of 126 class d names checked against kFaith; 19 defaults corrected (12 seeded by kFaith, 7 derived from a seeded umbrella or a forced-ON).**

## Method

- Names and inventory line numbers parsed from the inventory table (659 rows, 126 with class d). `kFaith` parsed from `FaithDefaults.cpp` as it stands on `orch/integration` (it includes the later PARTMORPH, CAR_STATE0, INTRO_MOVIES entries).
- A name is "seeded" when `kFaith` holds `{ "<name minus MASHED_>", ... }`. Opt-out is `MASHED_NO_<name>=1` (or `MASHED_NO_FAITH=1` for the lot), FaithDefaults.cpp:6-9 and 85-95.
- Sub-knobs: `PosModelKnobs.h:31-36` ORs `POS_WHEELS/TRANS/ORDER/CONTACTS` with `PosModelAll()` (= `MASHED_POS_MODEL`, seeded); `WheelContactSolver.cpp:174-175` ORs `MASHED_RW_WHEELPOS` with `PosModelWheels()`; `WarzonePontoon.cpp:715-717` reads `Env("MASHED_PONTOON_*") || !NoX()`.
- The inventory edit replaces only the default cell of the corrected rows, with the old value kept in the cell. No other cell was changed.

## Corrected rows (old default, new default, kFaith line)

| # | knob | inventory line | old default | new default | kFaith / source line |
|---|---|---|---|---|---|
| 1 | MASHED_A5_DRAFT_LUT | 481 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:38 |
| 2 | MASHED_A6B_ACOS_CRT | 482 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:61 |
| 3 | MASHED_A6B_NORM_LUT | 483 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:61 |
| 6 | MASHED_AI_CONTACT_EVENT | 486 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:30 |
| 8 | MASHED_AI_TICK_FIRST | 488 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 21 | MASHED_CC_PC24 | 501 | OFF (set, not "0") | ON (seeded "1") | S/Core/FaithDefaults.cpp:36 |
| 75 | MASHED_PATHPROG_LATE | 555 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 82 | MASHED_PONTOON_ANIM | 562 | OFF (unset -> off) | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:717) | S/Track/WarzonePontoon.cpp:715-717 (`Env(..) || !NoAnim()`), opt-out MASHED_NO_PONTOON_ANIM |
| 83 | MASHED_PONTOON_CARRY | 563 | OFF (unset -> off) | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:715) | S/Track/WarzonePontoon.cpp:715-717 (`Env(..) || !NoCarry()`), opt-out MASHED_NO_PONTOON_CARRY |
| 84 | MASHED_POS_CONTACTS | 564 | OFF (unset -> off) | ON (via PosModelAll, PosModelKnobs.h:36) | S/Vehicle/PosModelKnobs.h:31,36 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 85 | MASHED_POS_MODEL | 565 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:29 |
| 86 | MASHED_POS_ORDER | 566 | OFF (unset -> off) | ON (via PosModelAll, PosModelKnobs.h:35) | S/Vehicle/PosModelKnobs.h:31,35 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 87 | MASHED_POS_TRANS | 567 | OFF (unset -> off) | ON (via PosModelAll, PosModelKnobs.h:34) | S/Vehicle/PosModelKnobs.h:31,34 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 88 | MASHED_POS_WHEELS | 568 | OFF (unset -> off) | ON (via PosModelAll, PosModelKnobs.h:33) | S/Vehicle/PosModelKnobs.h:31,33 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 96 | MASHED_RING_DBUF | 576 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:32 |
| 99 | MASHED_ROUND_GRID_ORDER | 579 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:30 |
| 102 | MASHED_RW_WHEELPOS | 582 | OFF (unset -> off) | ON (via PosModelWheels, WheelContactSolver.cpp:174-175) | S/Collision/WheelContactSolver.cpp:174-175 with PosModelKnobs.h:33 |
| 112 | MASHED_SPAWN_BASIS_RW | 592 | OFF (unset -> off) | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 114 | MASHED_STEER_ACOS_CRT | 594 | OFF (set, not "0") | ON (seeded "1") | S/Core/FaithDefaults.cpp:34 |

Note: `MASHED_POS_SEED` is also ON through `PosModelAll()` (PosModelKnobs.h:32) but is not a class d row, so it is not corrected here.

## Rows 1-126 with no change

The other 107 class d names are not in `kFaith` and not ORed into a seeded umbrella. Their default column is unchanged.

## All 126 names checked

| # | knob | inventory line | default after fix | kFaith / source |
|---|---|---|---|---|
| 1 | MASHED_A5_DRAFT_LUT | 481 | ON (seeded "1") | S/Core/FaithDefaults.cpp:38 |
| 2 | MASHED_A6B_ACOS_CRT | 482 | ON (seeded "1") | S/Core/FaithDefaults.cpp:61 |
| 3 | MASHED_A6B_NORM_LUT | 483 | ON (seeded "1") | S/Core/FaithDefaults.cpp:61 |
| 4 | MASHED_ABIL_KEYS | 484 | value (parsed) | not in kFaith |
| 5 | MASHED_AILEAF_PC | 485 | OFF (unset -> off) | not in kFaith |
| 6 | MASHED_AI_CONTACT_EVENT | 486 | ON (seeded "1") | S/Core/FaithDefaults.cpp:30 |
| 7 | MASHED_AI_DIFFFLAG | 487 | value (parsed) | not in kFaith |
| 8 | MASHED_AI_TICK_FIRST | 488 | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 9 | MASHED_C4N | 489 | OFF (unset -> off) | not in kFaith |
| 10 | MASHED_C4N_STRIDE | 490 | value (parsed) | not in kFaith |
| 11 | MASHED_CAM_FOV | 491 | value (parsed) | not in kFaith |
| 12 | MASHED_CARPARTS_LAW | 492 | OFF (unset -> off) | not in kFaith |
| 13 | MASHED_CARPARTS_LAW_OUT | 493 | OFF (unset -> off) | not in kFaith |
| 14 | MASHED_CAR_BRAKE_ROWS | 494 | OFF (unset -> off) | not in kFaith |
| 15 | MASHED_CAR_COLOUR | 495 | value (parsed) | not in kFaith |
| 16 | MASHED_CAR_ENV | 496 | OFF (unset -> off) | not in kFaith |
| 17 | MASHED_CAR_ENV_PREVCAM | 497 | OFF (unset -> off) | not in kFaith |
| 18 | MASHED_CAR_LIVERY | 498 | OFF (unset -> off) | not in kFaith |
| 19 | MASHED_CAR_PARTS | 499 | OFF (unset -> off) | not in kFaith |
| 20 | MASHED_CAR_SEL | 500 | value (parsed) | not in kFaith |
| 21 | MASHED_CC_PC24 | 501 | ON (seeded "1") | S/Core/FaithDefaults.cpp:36 |
| 22 | MASHED_CUP | 502 | value (parsed) | not in kFaith |
| 23 | MASHED_D2BSP_KEY0 | 503 | OFF (unset -> off) | not in kFaith |
| 24 | MASHED_D2CHUNK_FRAMEMS | 504 | OFF (unset -> off) | not in kFaith |
| 25 | MASHED_D2LAT_SPAWN_YAW | 505 | OFF (unset -> off) | not in kFaith |
| 26 | MASHED_D2SINK_SM | 506 | OFF (unset -> off) | not in kFaith |
| 27 | MASHED_D3D_NOVSYNC | 507 | OFF (unset -> off) | not in kFaith |
| 28 | MASHED_D3D_REF | 508 | OFF (unset -> off) | not in kFaith |
| 29 | MASHED_DECOUPLE | 509 | OFF (unset -> off) | not in kFaith |
| 30 | MASHED_FONT_FLATRGB | 510 | value (parsed) | not in kFaith |
| 31 | MASHED_FONT_GAMMA | 511 | value (parsed) | not in kFaith |
| 32 | MASHED_FRAME_PROF | 512 | OFF (unset -> off) | not in kFaith |
| 33 | MASHED_FX_BODYPOSE | 513 | OFF (unset -> off) | not in kFaith |
| 34 | MASHED_FX_DEBUG | 514 | value (parsed) | not in kFaith |
| 35 | MASHED_FX_RAWSPEED | 515 | OFF (unset -> off) | not in kFaith |
| 36 | MASHED_GAME_LENGTH | 516 | value (parsed) | not in kFaith |
| 37 | MASHED_GOTO | 517 | value (parsed) | not in kFaith |
| 38 | MASHED_HIDE_SHADOW | 518 | value (parsed) | not in kFaith |
| 39 | MASHED_HOOK_ALLOW_BOOTPATCH | 519 | OFF (unset -> off) | not in kFaith |
| 40 | MASHED_HOOK_HI | 520 | value (parsed) | not in kFaith |
| 41 | MASHED_HOOK_LANE2 | 521 | OFF (unset -> off) | not in kFaith |
| 42 | MASHED_HOOK_LO | 522 | value (parsed) | not in kFaith |
| 43 | MASHED_HOOK_MANIFEST | 523 | OFF (unset -> off) | not in kFaith |
| 44 | MASHED_HOOK_SKIP | 524 | OFF (unset -> off) | not in kFaith |
| 45 | MASHED_INTERP | 525 | OFF (unset -> off) | not in kFaith |
| 46 | MASHED_INTERP_SHADOW | 526 | value (parsed) | not in kFaith |
| 47 | MASHED_INTERP_WHEELS | 527 | value (parsed) | not in kFaith |
| 48 | MASHED_INTRO_PRESS | 528 | value (parsed) | not in kFaith |
| 49 | MASHED_JUMPFIX | 529 | OFF (unset -> off) | not in kFaith |
| 50 | MASHED_L1B_INPUTS | 530 | OFF (unset -> off) | not in kFaith |
| 51 | MASHED_LAPS | 531 | value (parsed) | not in kFaith |
| 52 | MASHED_LIBRW_AFTERWORLDORDER | 532 | OFF (unset -> off) | not in kFaith |
| 53 | MASHED_LIBRW_BINMESHORDER | 533 | OFF (unset -> off) | not in kFaith |
| 54 | MASHED_LIBRW_CLUMPORDER | 534 | OFF (unset -> off) | not in kFaith |
| 55 | MASHED_LIBRW_DUAL | 535 | OFF (unset -> off) | not in kFaith |
| 56 | MASHED_LIBRW_LIFT | 536 | value (parsed) | not in kFaith |
| 57 | MASHED_LIBRW_LINEAR | 537 | OFF (unset -> off) | not in kFaith |
| 58 | MASHED_LIBRW_NODRAW | 538 | OFF (unset -> off) | not in kFaith |
| 59 | MASHED_LIBRW_NOMIP | 539 | OFF (unset -> off) | not in kFaith |
| 60 | MASHED_LIBRW_PAIRORDER | 540 | OFF (unset -> off) | not in kFaith |
| 61 | MASHED_LIBRW_RAINCULL | 541 | OFF (unset -> off) | not in kFaith |
| 62 | MASHED_LIBRW_SECTORWALK | 542 | OFF (unset -> off) | not in kFaith |
| 63 | MASHED_LIBRW_SMOKE | 543 | OFF (unset -> off) | not in kFaith |
| 64 | MASHED_LIBRW_STICKYVA | 544 | OFF (unset -> off) | not in kFaith |
| 65 | MASHED_LIBRW_SUPERG_VA | 545 | OFF (unset -> off) | not in kFaith |
| 66 | MASHED_MISSILE_FORCE | 546 | OFF (unset -> off) | not in kFaith |
| 67 | MASHED_MODE7_PC | 547 | OFF (unset -> off) | not in kFaith |
| 68 | MASHED_MORTAR_FORCE | 548 | OFF (unset -> off) | not in kFaith |
| 69 | MASHED_MSG_IDS | 549 | value (parsed) | not in kFaith |
| 70 | MASHED_MUSIC_NO_RACE_CDAUDIO | 550 | value (parsed) | not in kFaith |
| 71 | MASHED_NEGCTL | 551 | value (parsed) | not in kFaith |
| 72 | MASHED_OBJBSP_ORDER | 552 | OFF (unset -> off) | not in kFaith |
| 73 | MASHED_OBJSHAD_PLANES | 553 | OFF (unset -> off) | not in kFaith |
| 74 | MASHED_P3_MEASURE | 554 | value (parsed) | not in kFaith |
| 75 | MASHED_PATHPROG_LATE | 555 | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 76 | MASHED_PATHPROG_PC | 556 | OFF (unset -> off) | not in kFaith |
| 77 | MASHED_PHYS_NOCONTACT | 557 | OFF (unset -> off) | not in kFaith |
| 78 | MASHED_PHYS_PROF | 558 | OFF (unset -> off) | not in kFaith |
| 79 | MASHED_PICKUPS_PC | 559 | OFF (unset -> off) | not in kFaith |
| 80 | MASHED_PICKUPS_PREGO_NO_REFDIST | 560 | OFF (set, not "0") | not in kFaith |
| 81 | MASHED_PLAYERS | 561 | value (parsed) | not in kFaith |
| 82 | MASHED_PONTOON_ANIM | 562 | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:717) | S/Track/WarzonePontoon.cpp:715-717 (`Env(..) || !NoAnim()`), opt-out MASHED_NO_PONTOON_ANIM |
| 83 | MASHED_PONTOON_CARRY | 563 | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:715) | S/Track/WarzonePontoon.cpp:715-717 (`Env(..) || !NoCarry()`), opt-out MASHED_NO_PONTOON_CARRY |
| 84 | MASHED_POS_CONTACTS | 564 | ON (via PosModelAll, PosModelKnobs.h:36) | S/Vehicle/PosModelKnobs.h:31,36 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 85 | MASHED_POS_MODEL | 565 | ON (seeded "1") | S/Core/FaithDefaults.cpp:29 |
| 86 | MASHED_POS_ORDER | 566 | ON (via PosModelAll, PosModelKnobs.h:35) | S/Vehicle/PosModelKnobs.h:31,35 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 87 | MASHED_POS_TRANS | 567 | ON (via PosModelAll, PosModelKnobs.h:34) | S/Vehicle/PosModelKnobs.h:31,34 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 88 | MASHED_POS_WHEELS | 568 | ON (via PosModelAll, PosModelKnobs.h:33) | S/Vehicle/PosModelKnobs.h:31,33 (POS_MODEL seeded, FaithDefaults.cpp:29) |
| 89 | MASHED_QHULL_OPT | 569 | OFF (unset -> off) | not in kFaith |
| 90 | MASHED_QHULL_PC | 570 | OFF (unset -> off) | not in kFaith |
| 91 | MASHED_R7_INPUT_PAD_PRESS | 571 | value (parsed) | not in kFaith |
| 92 | MASHED_R7_INPUT_PRESS | 572 | value (parsed) | not in kFaith |
| 93 | MASHED_REGISTRY_PC | 573 | OFF (unset -> off) | not in kFaith |
| 94 | MASHED_RENDER_PROF | 574 | OFF (unset -> off) | not in kFaith |
| 95 | MASHED_RFLAME_SORT | 575 | OFF (unset -> off) | not in kFaith |
| 96 | MASHED_RING_DBUF | 576 | ON (seeded "1") | S/Core/FaithDefaults.cpp:32 |
| 97 | MASHED_RING_DBUF_DISCARD | 577 | OFF (unset -> off) | not in kFaith |
| 98 | MASHED_ROUND_COLOURS | 578 | value (parsed) | not in kFaith |
| 99 | MASHED_ROUND_GRID_ORDER | 579 | ON (seeded "1") | S/Core/FaithDefaults.cpp:30 |
| 100 | MASHED_ROUND_SCORES | 580 | value (parsed) | not in kFaith |
| 101 | MASHED_RULE_ENGINE | 581 | OFF (unset -> off) | not in kFaith |
| 102 | MASHED_RW_WHEELPOS | 582 | ON (via PosModelWheels, WheelContactSolver.cpp:174-175) | S/Collision/WheelContactSolver.cpp:174-175 with PosModelKnobs.h:33 |
| 103 | MASHED_SAVE_FIRSTRUN_WRITERS | 583 | OFF (unset -> off) | not in kFaith |
| 104 | MASHED_SAVE_PATH | 584 | OFF (unset -> off) | not in kFaith |
| 105 | MASHED_SAVE_RT_IN | 585 | OFF (unset -> off) | not in kFaith |
| 106 | MASHED_SAVE_RT_OUT | 586 | OFF (unset -> off) | not in kFaith |
| 107 | MASHED_SHADOW_NEED_PAGES | 587 | OFF (unset -> off) | not in kFaith |
| 108 | MASHED_SHADOW_PRIV_HI | 588 | value (parsed) | not in kFaith |
| 109 | MASHED_SHADOW_PRIV_LO | 589 | value (parsed) | not in kFaith |
| 110 | MASHED_SHADOW_STW | 590 | OFF (unset -> off) | not in kFaith |
| 111 | MASHED_SKY_D3D9 | 591 | OFF (unset -> off) | not in kFaith |
| 112 | MASHED_SPAWN_BASIS_RW | 592 | ON (seeded "1") | S/Core/FaithDefaults.cpp:31 |
| 113 | MASHED_SPEED | 593 | value (parsed) | not in kFaith |
| 114 | MASHED_STEER_ACOS_CRT | 594 | ON (seeded "1") | S/Core/FaithDefaults.cpp:34 |
| 115 | MASHED_STEER_AXIS_TERRAIN | 595 | OFF (unset -> off) | not in kFaith |
| 116 | MASHED_SUBSTEP_OUTER_ORDER | 596 | OFF (unset -> off) | not in kFaith |
| 117 | MASHED_SUPERG_REFLECT_FIRST | 597 | OFF (unset -> off) | not in kFaith |
| 118 | MASHED_TEAMS | 598 | value (parsed) | not in kFaith |
| 119 | MASHED_TEAM_KEYS | 599 | value (parsed) | not in kFaith |
| 120 | MASHED_TEAM_PLAY | 600 | OFF (unset -> off) | not in kFaith |
| 121 | MASHED_TERRAIN_NOLIGHT | 601 | OFF (unset -> off) | not in kFaith |
| 122 | MASHED_TEXANIM_MASKTEX | 602 | OFF (unset -> off) | not in kFaith |
| 123 | MASHED_TGT3_PC | 603 | OFF (unset -> off) | not in kFaith |
| 124 | MASHED_THROTTLE_K | 604 | value (parsed) | not in kFaith |
| 125 | MASHED_U9186_GATES | 605 | OFF (unset -> off) | not in kFaith |
| 126 | MASHED_WORLDVEL | 606 | value (parsed) | not in kFaith |

## Outside class d (not edited here)

The same stale OFF default sits in 49 rows of other classes (b, c, a, x), all seeded by kFaith. They are out of this step's scope (class d only) and the inventory file keeps them as written. A follow-up can correct them from this list.

| inventory line | knob | default col | class | kFaith line |
|---|---|---|---|---|
| 33 | MASHED_A5_RUBBER_LIVE | OFF (unset -> off) | b | FaithDefaults.cpp:48 |
| 34 | MASHED_A6A_LC0 | OFF (unset -> off) | b | FaithDefaults.cpp:33 |
| 35 | MASHED_AI_LUT | value (parsed) | b | FaithDefaults.cpp:34 |
| 140 | MASHED_PARTMORPH | OFF (unset -> off) | b | FaithDefaults.cpp:72 |
| 145 | MASHED_WCS_COMBINE_PC24 | OFF (unset -> off) | b | FaithDefaults.cpp:46 |
| 146 | MASHED_WCS_TRINORMAL | OFF (unset -> off) | b | FaithDefaults.cpp:44 |
| 147 | MASHED_A4_DOT_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:62 |
| 148 | MASHED_A4_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:37 |
| 149 | MASHED_A5_AIR_F32 | OFF (unset -> off) | c | FaithDefaults.cpp:62 |
| 150 | MASHED_A5_CONTACT_INT | OFF (unset -> off) | c | FaithDefaults.cpp:62 |
| 151 | MASHED_A5_DOT_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:61 |
| 152 | MASHED_A5_FD0_LIVE | OFF (unset -> off) | c | FaithDefaults.cpp:37 |
| 153 | MASHED_A5_RING_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:61 |
| 154 | MASHED_A5_SPIN_FILD | OFF (unset -> off) | c | FaithDefaults.cpp:62 |
| 155 | MASHED_A6A_DOT_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:58 |
| 157 | MASHED_A6A_GEAR_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:37 |
| 158 | MASHED_A6A_SPIN_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:41 |
| 164 | MASHED_AI_FWD_ATROW | OFF (set, not "0") | c | FaithDefaults.cpp:35 |
| 177 | MASHED_FIXUP_F32 | OFF (unset -> off) | c | FaithDefaults.cpp:41 |
| 178 | MASHED_FIXUP_SLIDE_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:42 |
| 179 | MASHED_FIXUP_SUM_ORDER | OFF (unset -> off) | c | FaithDefaults.cpp:41 |
| 180 | MASHED_FIX_5CEA44 | OFF (unset -> off) | c | FaithDefaults.cpp:38 |
| 186 | MASHED_HEADING_FWD_LIVE | OFF (unset -> off) | c | FaithDefaults.cpp:34 |
| 189 | MASHED_INTRO_MOVIES | value (parsed) | c | FaithDefaults.cpp:77 |
| 194 | MASHED_MODE7 | OFF (unset -> off) | c | FaithDefaults.cpp:36 |
| 195 | MASHED_MODE_LIVE | OFF (unset -> off) | c | FaithDefaults.cpp:51 |
| 197 | MASHED_P3_RESET_PAIR | OFF (unset -> off) | c | FaithDefaults.cpp:66 |
| 199 | MASHED_PATHPROG | OFF (unset -> off) | c | FaithDefaults.cpp:29 |
| 200 | MASHED_PICKUPS_ORIG | OFF (unset -> off) | c | FaithDefaults.cpp:35 |
| 208 | MASHED_REASSERT_KEEP_NORMAL | OFF (unset -> off) | c | FaithDefaults.cpp:31 |
| 209 | MASHED_REGISTRY | OFF (unset -> off) | c | FaithDefaults.cpp:35 |
| 210 | MASHED_REG_CALLBACKS | OFF (unset -> off) | c | FaithDefaults.cpp:36 |
| 211 | MASHED_REG_CARS | OFF (unset -> off) | c | FaithDefaults.cpp:36 |
| 212 | MASHED_REG_LATE | OFF (set, not "0") | c | FaithDefaults.cpp:37 |
| 215 | MASHED_ROTATE_LUT | OFF (unset -> off) | c | FaithDefaults.cpp:33 |
| 216 | MASHED_ROTATE_PC24 | OFF (unset -> off) | c | FaithDefaults.cpp:34 |
| 218 | MASHED_ROUND_GRID | OFF (unset -> off) | c | FaithDefaults.cpp:29 |
| 227 | MASHED_SLOT_TYPE_LIVE | OFF (set, not "0") | c | FaithDefaults.cpp:51 |
| 231 | MASHED_STEER_SCALE_RDATA | OFF (set, not "0") | c | FaithDefaults.cpp:35 |
| 232 | MASHED_SUBSTEP_OUTER | OFF (unset -> off) | c | FaithDefaults.cpp:54 |
| 233 | MASHED_TER_SPIN_F32 | OFF (unset -> off) | c | FaithDefaults.cpp:41 |
| 234 | MASHED_TGT3 | OFF (unset -> off) | c | FaithDefaults.cpp:30 |
| 238 | MASHED_UP_TRINORMAL | OFF (unset -> off) | c | FaithDefaults.cpp:32 |
| 297 | MASHED_CAR_STATE0 | OFF (unset -> off) | a | FaithDefaults.cpp:72 |
| 466 | MASHED_SUBSTATE | OFF (unset -> off) | a | FaithDefaults.cpp:66 |
| 633 | MASHED_FIX_5CEA60 | n/a | x | FaithDefaults.cpp:32 |
| 635 | MASHED_FWD_AT | n/a | x | FaithDefaults.cpp:33 |
| 637 | MASHED_GRID_DIR | n/a | x | FaithDefaults.cpp:30 |
| 679 | MASHED_RW_SSE_POINTS | n/a | x | FaithDefaults.cpp:32 |

Class d rows 6, 8, 21, 1, 2, 3 (`AI_CONTACT_EVENT`, `AI_TICK_FIRST`, `CC_PC24`, `A5_DRAFT_LUT`, `A6B_ACOS_CRT`, `A6B_NORM_LUT`) are seeded too and fall in TRIAGE-1 range (rows 1-63); TRIAGE-2 named only its own six.

## Raw data

None committed. Working script and `d.json` lived in the session temp dir.
