# RESULT PORT-TOP-04: HANDOFF (scope inventory and proposed split)

Verdict: HANDOFF. No code on this branch yet. `orch/PORT-TOP-04` is cut from `orch/PORT-TOP-03` @ `2955cb087` (TOP-03 plus
`orch/integration` with PORT-TOP-01). The row as briefed takes AppInitialiseOnBootup `0x00402750` and AppDestroy
`0x00402a40` off stand-ins together with DisplayInit, ViewportInit, `0x004924f0`, `0x00493540`, `0x00493560` and the 36
unbound RW rows. The inventory below puts that at about 108 direct callees, 93 of them with no exe body, under a
pixel-identity gate on 12 frontend captures. That is several rows of work, and the session that did PORT-TOP-03 is
already long, so this file is the handoff: the inventory and a split, each part with its own gate.

## Inventory (direct callees, from BINDINGS.tsv and hooks.csv `port`)

| node | direct callees | no exe body | exe body | called / bound |
|---|---|---|---|---|
| `0x00402750` AppInitialiseOnBootup | 53 | 43 | 5 | 2 called, 2 bound (RW mode queries), 1 under-stand-in |
| `0x00402a40` AppDestroy | 39 | 35 | 4 | 0 |
| `0x004924f0` data init | 6 | 1 | 1 | 3 called, 1 matched (C4 by match on `orch/PORT-TOP-MATCH-01`, in the train) |
| `0x004921d0` DisplayInit | 9 game + 3 RW | 7 | `0x00498bc0`/`bd0` (body) | RW `0x004c2f00`, `0x004c2ed0`, `0x004c5c00` already bound (TOP-03) |
| `0x00428590` ViewportInit | 6 | 5 (`0x00428450` C3, `0x004c1a00`, `0x004c1bb0`, `0x004671a0`, `0x004c1be0`) | `0x004c19f0` (body) | |
| `0x00493540` -> `0x00495150` gate | 6 | `0x00495120` TimerQpfStore, `0x004955b0`, `0x00498510` (C2), `0x004960a0`, `0x004963b0` (not in hooks.csv) | | `0x00499710` called |
| `0x00493560` -> `0x004954f0` HardwareExitApplication | 5 | all | | |
| RW rows still `rw` | 36 | | | each under one of the stand-ins above |

## Proposed split, in order

1. **TOP-04a: boot gate and exit (small).** `0x00493540` -> `0x00495150` with its 6 callees, `0x00493560` ->
   `0x004954f0`. `0x00495120` TimerQpfStore running in the original order retires TOP-01's `0x004950b0` stand-in
   (`MatchS16_00495110_Call004950b0`). Drop the MainLoopInit SEH guard (`PortTop_MainLoopInitGuarded`): on the merged
   build it returns 1 without faulting (`RESULT_PORT-TOP-03.md`, merge section). Gate: knob boot + clean exit, captures.
2. **TOP-04b: DisplayInit + ViewportInit.** 11 game callees without an exe body, plus the RW camera / raster calls in
   ViewportInit (`0x004c1a00`, `0x004c1bb0`, `0x004c1be0`, `0x004671a0`) to bind in `LibRw/RwApiBind.cpp`, each RW 3.x
   identity confirmed from its matched source first. DisplayInit loads `RWTEXDICTIONARY` 404 through `0x004997b0` /
   `0x004b3eb0` and sets `DAT_0077195c`, which makes WinMain reach the bound RwTexDictionaryDestroy `0x004c5930`. Pixel
   risk: it writes render width / height (`0x0042b890` / `0x0042b8a0`) and a default viewport camera
   (`0x0042f660`) that the hand frontend does not read today. Read those consumers before swapping.
3. **TOP-04c: AppDestroy `0x00402a40`.** 39 callees (35 without an exe body). Teardown only, so the gate is clean exit +
   default gates, no capture risk. Its RW rows (`0x004e5700`, `0x004e6e00`, `0x004e45b0`, `0x004e4860`, `0x004e4d90`)
   need their identity checked: `0x004e45b0`, tracked as RwFrameRemoveChild, takes two arguments and indexes a plugin
   offset at `0x007d7174`.
4. **TOP-04d: AppInitialiseOnBootup `0x00402750`** (53 callees, 43 without an exe body) and `0x004924f0` (after
   PORT-TOP-MATCH-01 lands). This is the frontend asset / audio / input / movie bring-up and the main pixel risk. It
   likely needs its own split by subsystem, each part replacing the matching piece of `HandTop_InitRest`.

Each part keeps the TOP-03 rules: callees by name, no raw-address calls, knob default OFF, default gates unchanged,
knob-ON boot to the menu + clean exit + the 12 nav-demo captures identical (reset `mashed_re_settings.bin` before each
run, see RESULT_PORT-TOP-03 method note).

## Decision for the lead

Split PORT-TOP-04 into TOP-04a..d as above (recommended), or keep one row and hand it to a fresh worker session with
this file as the brief.
