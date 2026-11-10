# RESULT PORT-OT-TRACKLOAD (2026-11-10): STOPPED AT SCOPE, no swap. 0x00426e10 is a 425-node closure with 47 C0..C3 rows and no caller on the knob path

L2 worker `cmwb01mfy0tx5sc1bvktph5uy` for lead `cmw7xu1960f37p41557tefpnd`, L-111 step 1. Branch `orch/PORT-OT-TRACKLOAD`
from orch/integration `5557d67b5`. No code change, so no build, no run and no BINDINGS or port-column change. The lead's
brief says to stop and report the scope when the loader is too large for one subsystem-sized commit. It is.

Static numbers come from `verify/portorigtop_20261109/tl_closure.py` (new, read-only). It uses the edges of
origtop_size.py (E8 rel32 plus relocated function pointers and tables, from MASHED.exe.unpatched) and its classes,
against BINDINGS.tsv at `5557d67b5` (453 lines):

```
py -3.12 verify/portorigtop_20261109/tl_closure.py 0x00426e10 0x0040cfd0 0x0040d270 0x0040d440
```

## 1. Who calls 0x00426e10 on the exe path: nobody

- The original's chain is 0x004929d0 (game-mode machine, case 2) -> 0x0040d440 -> 0x0040d270 (`Course::Finish`) ->
  0x0040d020 -> 0x00426e10 (re/match/s02/0040d020.c:35, `FUN_00426e10(**(param_1 + param_2 * 4))`).
- On the knob path 0x004929d0 is `PortTop_Port_004929d0 -> GameModeMachine_Tick` (BINDINGS.tsv). That is a no-op unless
  MASHED_PORTSTATE_GAMEFLOW_FU=1, and under it 0x0040d440 binds to `GameModeMachine_Case2Unreached`
  (Race/GameModeMachine.cpp:9-14: "the tick never calls the machine in phase 2"). BINDINGS row 0x0040d440 is `missing`.
- 0x00492d30 is the stand-in `HandTop_FrameDetTick + HandTop_FrameUpdate`, so the race on the knob path is the hand
  game flow.
- The only body of 0x00426e10 in mashedmod is Match/MatchS06_00426e10.cpp, listed in asi_sources.rsp only. The one
  call site, Track/CourseLoadDispatch.cpp:35 (a raw-address call), is also in asi_sources.rsp only.

So compiling a PortTop copy of 0x00426e10 would add a body with no caller. It would not set 0x0066d704 in any run.

## 2. What the hand loader does in its place

The exe's course load is D3d9Render/TrackRenderer.cpp (course load around :3500). It opens the track piz itself and
builds the librw world and texture dictionaries (`RaceSubmit_OnTrackLoaded`, :3592). It also seeds pieces of the
original's state:

| hand owner (exe TU) | original state it writes | the loader's own writer of that state |
|---|---|---|
| `Ai_BridgeLoad` (TrackRenderer.cpp, default ON) | AI controller image, 0x005f2770 (Track/CourseTableState.cpp:12) | 0x004235b0 / 0x00423630 (AI piz) |
| `CourseTableState_OnCourseLoad` (MASHED_PORTSTATE_COURSE) | 0x005f2570 / 0x005f2728 / 0x005f33f8 / 0x005f37a0 tables, then the matched 0x0041e870 | 0x0041e870 at 0x00426eaf (same call) |
| Race/PathNodes.cpp (D-11072 P1) | course block 0x00657498 (+0x0c1c0 nodes, +0x159c0 routes, +0x16240.. counts) | 0x0047a0f0 "LAPDATA.LUA", then 0x00478660 -> 0x004783f0 (PathNodes.cpp:3-5) |
| `GameModeState_OnCourseLoad` (MASHED_PORTSTATE_GAMEFLOW) | 0x0067f17c / 0x0063ba7c / 0x0063ba78 | course-index words of the 0x0040d270 chain |
| Ai/AiControllerAB.cpp:300 / :336 | 0x00644158 (the 0x00426c00 getter) | 0x0047a020 / 0x00479330 (COURSE.LUA) |

Every row is shared state. Running the matched loader next to the hand loader would give each of them two owners. That
is the PICKUPPOOL first-try crash shape.

## 3. What 0x00426e10 needs to run

`root 0x00426e10: 425 nodes`. 274 are `match` (C4 with a matched body and no exe binding yet), 13 are already
`bind:matched`, and 29 are other bindings. The blockers:

| class | n | rows |
|---|---|---|
| game-c0c3 (needs matching first) | 47 | direct: **0x00462950, 0x00478660, 0x00479330, 0x0047a020** (all C2). Deeper: 0x0045c030 0x0045de80 0x004783f0 0x004785e0 0x0047a320 0x0047be80 0x0047f6d0 0x004854e0 0x00487280 0x00491590 0x004a3384 0x004b4550 0x004b53b0 0x004c39b0 0x004c4a50 0x004c4d20 0x004c4dc0 0x004c4eb0 0x004c51a0 0x004c52f0 0x004cc050, plus 22 rows 0x005a6d60..0x005b3b80 |
| game-c4, no matched body | 2 | 0x00409680 `LedArrayInit`, 0x00409710 (both direct) |
| norow | 10 | 0x00440bc0 0x00474f90 0x0047f7d0 0x00487180 0x004b3f20 0x004b4eb0 0x00562ac0 0x005aaaa0 0x005ac770 0x005ac930 |
| stand-in | 2 | 0x004d8480, 0x0053fbb0 |
| families, not bound | | lua (7), rwphysics (13), rt2d (2), qhull (1) |

Of the 36 direct callees, 23 have no exe body and no BINDINGS row (hooks.csv `port=none`). The .asi TU sends 16 direct
callees through raw-address forwarders (`MatchS06_00426e10_Call<rva>`), which point at original code the exe does not
have. So each of the 23 needs an exe-only copy, and so does its subtree.

## 4. What the 0x0040cfd0 subtree touches once the flag is set

`root 0x0040cfd0: 120 nodes`, 10 direct callees:
- 6 are already bound matched bodies: 0x00405400, 0x004114e0, 0x00426c00, 0x00471df0, 0x004725f0, 0x00484c90.
- 4 are stand-ins: 0x00426b40, 0x0045bed0, 0x0045bf30, 0x00490000. 0x0045e040 is deeper.
- The rest of the subtree: 4 game-c0c3 (0x00485b30, 0x005a6d60, 0x005a75b0, 0x005ae920) and 4 norow (0x004827a0,
  0x004827c0, 0x00482800, 0x004b3f20).

So even a set flag would run only 6 small matched bodies. 0x004114e0 would read zero records (no exe writer of
0x0063bb04 / 0x0063bb08, RESULT_PORT-OT-OTHER-2.md). The builders of the records that 0x004725f0 and 0x00471df0 read
were not checked in this step [UNCERTAIN: needs the builder lookup of the PICKUPPOOL lesson before any flag change].

0x0040d270 also calls 0x0040cfd0 directly (direct callee list above). So the original runs that tree at course finish
as well as in AppDestroy.

## Pieces kept, with the reason

| piece | reason |
|---|---|
| 0x00426e10 (whole body) | 4 direct C2 callees with no matched body, plus 2 direct C4 callees with no matched body. No caller on the knob path (section 1). |
| 0x0066d704 = 1 alone, from hand code | No original code writes 1 there without the load. AppDestroy (matched 0x00402a40, :93-97) would then run 0x0040cfd0 against course state that no matched builder made. Same failure class as PICKUPPOOL. |
| 0x0040d020 / 0x0040d270 / 0x0040d440 | 0x0040d270's closure is 557 nodes with 53 game-c0c3, and it calls 0x00426e10 through 0x0040d020. |

## Proposed split (each one subsystem-sized)

1. **TL-OWN (an L-n decision, needed first).** Who owns the course state on the knob path: the hand loader (table in
   section 2) or the matched chain. If it is the matched chain, the hand pieces need knob-path skips (`Ai_BridgeLoad`,
   CourseTableState, PathNodes, GameModeState, the 0x00644158 forcing). This touches default-ON hand code, so it is a
   lead or user decision.
2. **TL-MATCH-LUA.** MATCH slice for the four direct C2 callees 0x00479330 (2893 B), 0x00478660 (1605 B), 0x00462950
   (1136 B) and 0x0047a020, then 0x004783f0 / 0x004785e0 / 0x0047a320 / 0x0047be80 / 0x0047f6d0. Also bind the lua
   family (7 nodes), since COURSE.LUA and LAPDATA.LUA run through it.
3. **TL-MATCH-REST.** 0x00409680 / 0x00409710 (C4, no matched body), the RW-math rows 0x004c39b0..0x004c52f0 and
   0x004cc050, the 22 rows 0x005a6d60..0x005b3b80, the 10 norow entries, and the rwphysics family (13).
4. **TL-PORT-n.** Exe-only PortTop copies of the 274 matched rows, split by subsystem: track nodes 0x0041e8xx,
   AI data 0x004235b0 / 0x00423630, the Lua runners 0x0047xxxx, and the camera / vehicle rows.
5. **TL-CALLER.** 0x0040d440 -> 0x0040d270 -> 0x0040d020 on the knob path, a superset of 0x00426e10 (558 nodes). This
   is the step that finally writes 0x0066d704. Only after it does the 0x0040cfd0 tree run, and then its 4 stand-ins
   (section 4) become the next swaps.

## Acceptance

| check | result |
|---|---|
| code change | none (`git diff 5557d67b5 -- mashedmod` empty) |
| build, knob-ON boot, 12 captures, G-CTRL, B, rva-lint | not run. Nothing in mashedmod/ or the allowlist changed, so the binary is the base build's |
| BINDINGS.tsv / port column | unchanged. No updater script added, so the POWERUPINIT chain is not affected |
