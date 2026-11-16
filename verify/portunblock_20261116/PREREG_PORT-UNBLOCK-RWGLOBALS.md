# PREREG PORT-UNBLOCK-RWGLOBALS (committed alone, before any arm)

Knob: `MASHED_RWC_RWGSWAP=1`, default OFF, exe only (`mashedmod/src/mashed_re/Match/PortTop_RwgSwap.cpp`). Live only when
`RwCore_FrameRoute()` is on (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1`) and `[0x007d3ff8]` equals the
engine instance the core recorded after RwEngineStart (`RwCore_GlobalsAtInit()`, `RwCore/RwCore_Knob.cpp`). When live:
FastSqrt 0x004c3b30 -> `RwCore_004c3b30` (re/match/s19/004c3b30.c), Vec2Length 0x004c3bf0 -> `RwCore_004c3bf0`
(re/match/s19/004c3bf0.c), and the matched HUD dispatch's 0x00403160 callee -> `RwCore_00403160` (re/match/s01/00403160.c).

Default path: the knob reads one global (`g_portTopRwgSwapEnv`, 0 after the first call) in FastSqrt and Vec2Length.
No value changes with the knob off. The default gates (G-CTRL, B, 12 captures) are the merge train's (WORKER_RULES, top-down
row); the worker does not run them.

## Arms (worker)

1. Builds: VS2022 and object build (`MASHED_DDKOBJ=1`), both `=== Build OK ===`. `ddkobj.py check`: no new DIFFERENT unit
   against the base (`RESULT_RWC-FRAME-LADDER-CHECK.md`: IDENTICAL 1561/1599, DIFFERENT 22).
2. Knob boots, private root, mashed_lock: `knob_boot.py` with `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1
   MASHED_RWC_FRAME_N=120 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`, knob OFF and ON, on both builds.
3. Closed-set tool `re/tools/port_closed_sets.py` on the VS2022 build before and after.

## Predictions and pass rule

- P1 (step 1 of the row): the RwEngineInit log line shows `[0x007d3ff8] = 007D3EC8` (the template, 0x004c32b0 at
  re/match/s19/004c32b0.c:106), and after RwEngineOpen `[0x007d3ff8]` is a heap instance (re/match/rwl01/004c30b0.c:60).
- P2: knob ON logs `[RWGSWAP] self-test PASS`: RwGlobals nonzero and equal to the core instance, sqrt table
  `*(RwGlobals + [0x007d3ffc])` nonzero, matched FastSqrt / Vec2Length bit-identical to the exe bodies on 31 fixed inputs.
- P3: knob ON and OFF both end `boot ends after present 120 on the RW core`, with the existing self-tests PASS.
- P4: knob OFF boot log identical in its RWC lines to the base recipe (no `[RWGSWAP]` line).
- P5: after the change the closed-set tool lists none of 0x00403160 / 0x004c3bf0 with the RWGLOBALS blocker (their
  matched files are compiled by the exe).

FAIL of P2 or P3 = the knob does not ship (RED). 0x00403160 is a race-HUD callee (sub-mode 0xb of 0x0040dfc0): the
120-frame menu route does not reach it, so its execution is not measured here (stated limit, not a pass).
