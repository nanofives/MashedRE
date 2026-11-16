# RESULT ROADMAP-T1-STANDIN (text row, no build, no gates)

Branch orch/ROADMAP-T1-STANDIN from orch/integration 7630e53ca. Changed: `ROADMAP.md` (T1 section), `re/orchestrator/UNCERTAIN_REGISTER.md` (3 lines), this file.

## 1. L-168 written into ROADMAP T1
Source: `log/orch/DECISIONS.md` L-168 (answered 2026-11-16 Mariano: both in parallel), `.claude/skills/roadmap-loop/SKILL.md` "Run policy" item 4.
The hand stand-in for `0x0043dfd0` (RWC-FRAME-CLUMP phase-ladder stand-in on the RW core frame route, ledger RWC-FRAME-LADDER-CHECK) is allowed in T1 while MATCH-43DFD0-RAISE raises `re/match/s08/0043dfd0.c` (19.2% per PORT-TOP-BODY-43DFD0 ledger note). No runtime debugging of the body. T1 Exit now says "except the 0x0043dfd0 exception".

## 2. Ranked T1 checklist (ledger stage read 2026-11-16 with roadmap.ps1 get/list)
| # | Item | Row id | Stage | Source |
|---|---|---|---|---|
| 1 | TOP NON-0x0043dfd0 ITEM: font read chain 3 (`0x00427880` to `0x00554390` 33.5%, `0x00554200` 69.5%, `0x00554940` 12.5%, `0x005551d0`, `0x005c4c60`, `0x005c4ad0`, `0x004cc790`, `0x004c5ca0`; stand-ins `0x004d8480`, `0x00553e80`, `0x005547f0`); acceptance `0x00555830` gets non-zero `DAT_0067d838` | HUDINIT-C | open, unstaffed | ROADMAP T1 Rows list; ledger note (RW_FUNCS.tsv state, run 37) |
| 2 | Strict draw parity: 42 of 82 menu draws match only with `--tol-anim 8`; align jitter counter | RWC-FRAME-JITTER-STRICT | open | ledger |
| 3 | Check ladder stand-in vs original (write watch `0x0067eca4`/`0x0067eca8`) | RWC-FRAME-LADDER-CHECK | open | ledger |
| 4 | `0x00555ff0` first live `+0x60` consumer; `0x004cdd60` C2 79.2%; U-9803 | RASTER-555FF0 | open | ledger note run 37 |
| 5 | Swap `0x00403160`, FastSqrt, Vec2Length (knob + PREREG) | PORT-UNBLOCK-RWGLOBALS | open | ledger note run 37 |
| 6 | Remove 10 null guards on routed Destroy bindings | RWC-NULL-DESTROY-GUARD | open | ledger |
| 7 | Destroyer of Bomb.dff (1 clump alive at RwEngineStop, `DAT_00636c00`) | RWC-FRAME-BOMB-DESTROY | open | ledger |
| 8 | Stage of core bring-up | RWC-CORE-RUN | blocked, note ends run 38 | ledger; see register T1-RWCORERUN-STAGE |
| 9 | 21 DirectShow renderer methods behind `0x00494c80` | BIND-D3D-REST | blocked (option c, 2026-11-12) | ledger |
| 10 | `0x0043dfd0` body | MATCH-43DFD0-RAISE (working), PORT-TOP-BODY-43DFD0 (handoff), PORT-TOP-BODY-43DFD0-AV, NAVDEMO-KNOBPATH, NAVDEMO-KNOBPATH-FADE (blocked, L-168) | per row | ledger |

Not on the list, with reason: HUDINIT-A/B, NEARMATCH-555360, PORT-TOP-08, PORT-TOP-04K, PORT-OT-SEED-CODEPTR, BIND-CRT-STDIO, BIND-CRT-STRING, MATCH-RT2D-FONT, RWC-DEVICE, RWC-DRIVER, RWC-CENSUS, RWC-MATCH are merged. PORT-BIND-RWCORE-2, PORT-OT-BIND-41DEF0, PORT-OT-SCREENSHOT are blocked, superseded by L-124. RWC-IMGREAD-DEPTH, RWC-FRAME-CROSSFADE-PHASE, RWC-FRAME-MOVIE-TEARDOWN are parked in UNCERTAIN_REGISTER (run-51 policy), promoted only if they block T1.

Why HUDINIT-C is the top non-0x0043dfd0 item: it is the only row in the ROADMAP T1 Rows list that is neither merged, blocked nor superseded and removes menu-path stand-ins; rows 2 to 7 are verification or cleanup of what already runs on the frame route. Its dependency on RWC-CORE-RUN (ledger note) is [UNCERTAIN], see below. This ranking is a judgement from the ledger text, not a measurement.

## 3. [UNCERTAIN], one register line each (no ledger rows)
- T1-STANDIN-COUNT: current menu-path stand-in count unknown (59 + 32 is the v5 figure). Test: recount from `verify/porttop_20261108/BINDINGS.tsv` and PROVISIONAL.tsv.
- T1-PORTTOP08-PHASE34: RWC-FRAME-LADDER-CHECK says the phase 3 to 4 write stays PORT-TOP-08 work while PORT-TOP-08 is merged. Test: read PORT-TOP-08 RESULT.
- T1-RWCORERUN-STAGE: whether the "after RWC-CORE-RUN" prerequisite of rows 1, 4, 5 is met, given RWC-FRAME-* merges. Test: RESULT_RWC-FRAME-CLUMP knob-ON boot, check `0x007d3ff8` filled by the core engine open.

No ledger row added: no unfinished scope of this row blocks its goal.

## 4. Not done / limits
No build, no gates, no Ghidra. Ledger notes quoted, not re-verified against code.
