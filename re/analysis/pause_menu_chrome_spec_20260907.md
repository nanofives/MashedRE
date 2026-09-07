# Implementation Spec — Playtest #3: full in-race pause-menu chrome

Scoping only (read-only, produced 2026-09-07 by orchestrator child D on account2 —
NO Ghidra). For a later account3 / Ghidra-capable session. Every RVA below is cited
from the port's transcribed RVA maps + on-disk sources; the four "NOT reversed"
functions are named as decomp targets, not decompiled here.

## 0. Current state (confirmed, do not re-derive)

Faithful pause CORE is committed (`10d737cc`):
- **State model** — `Race/GameFlow.{h,cpp}`: `GameFlow_IsPaused()` / `GameFlow_SetPaused(bool)`.
  Original mode 3<->7 map in `GameFlow.h:31-46` (master `FUN_004929d0` 0x004929d0;
  pause-arm `FUN_0042c220` 0x0042c220 -> `FUN_0042c280` -> `FUN_0042bf30` 0x0042bf30
  raising event 0xff210000 into `DAT_0067eab0`, read by `FUN_0042c1c0` 0x0042c1c0;
  sim-freeze gate `FUN_00492d30` runs race tick `FUN_004111c0` only in case 3; render
  tick `FUN_00492e90` draws case 7 identically to case 3).
- **Input toggle** — `exe_main.cpp:1880-1913`: ESC toggles pause; ENTER-while-paused
  calls `GameFlow_RequestExit()`. Demo/capture/`g_det_clock` keep exit-on-ESC.
- **Sim-freeze** — `exe_main.cpp:2932-2936`: physics step gated on `!paused`.
- **Placeholder overlay (REPLACE THIS)** — `exe_main.cpp:3832-3847`, guarded by
  `if (paused && g_font.ready() && g_bridge_installed)`. Dim panel + three hand-written
  lines "PAUSED" / "[ESC] Resume" / "[ENTER] Quit to Menu". Marked `[SCOPED]` — invented.
- **Real pause descriptor already transcribed** — `Frontend/MenuNavSM.cpp:55` `kT0[]` IS
  screen 0, the in-race pause menu (`PTR_DAT_005f7638[0]`); `MenuNavSM.cpp:924-931`
  glosses it "Continue / Options / Restart Race / Quit Race / Quit Game". Nav stack,
  `TableForScreen(0)`, record builder, dispatcher all present. Missing = the arm+dispatch
  mapping kT0 action codes to Resume/Restart/Quit, and rendering screen 0 over frozen race.

## 1. RVAs needing Ghidra decomp

| RVA | Role | Must contribute |
|---|---|---|
| `FUN_0043d7c0` (0x0043d7c0) | Pause-menu build/dispatch (tracker-named handler) | action-code -> behavior ladder (Resume/Restart/Options/Quit-Race/Quit-Game). `GameFlow.h:42-43` records -0xce0000 -> `FUN_0043d2a0(1,0)` (frontend screen 1) and -0xe00000 -> `FUN_0040de10` (restart). Resolve the §discrepancy. |
| `FUN_0040de10` (0x0040de10) | Restart-race flow | exact teardown/re-init to restart same config/track (not frontend round-trip). Needed for a new `GameFlow_RequestRestart()`. |
| `FUN_0042bf30` (0x0042bf30) 0xff210000 arm | pause event raise | whether the menu is a pushed Nav screen 0 or an event-layered overlay; frame ordering of freeze+show. |
| `FUN_0043dfd0` (0x0043dfd0) + `FUN_0042ac90` (0x0042ac90) | frontend input/dispatch + action-code reader | how a screen-0 select resolves its action; whether pause reuses `Nav_Select` or a pause-specific handler. |
| kT0 msgids | item labels via ENGLISH.DAT (Font36.piz copy, memory `two-copies-of-english-dat`) | dump strings for 0x47,0x18,0x27,0x1c,0x1d,0x1e to confirm gloss + get verbatim Resume/Restart/Quit. |

`kT0` item stream (`MenuNavSM.cpp:55`, verbatim): title msgid 0x47, kind 5; items
(label,action): (0x18, 0xff150000), (0x27, 0xff500000), (0x1c, 0xff1e0000),
(0x1d, 0xff1f0000), (0x1e, 0xff200000).

### DISCREPANCY (NO-GUESSING flag — decomp must resolve)
Tracker/`GameFlow.h` record Restart = -0xe00000 = 0xff200000 and Quit-to-frontend =
-0xce0000 = 0xff320000. In kT0, 0xff200000 is the LAST item (label 0x1e), and 0xff320000
does not appear at all. So the "Restart Race = item 3" gloss and the action map don't line
up, and the quit action the input code uses (`exe_main.cpp:1905-1912`, -0xce0000) is not in
the table. Decompiling `FUN_0043d7c0` + `FUN_0043dfd0` reconciles action->item->behavior.
Do not guess from label order.

## 2. Placeholder -> real chrome (reuse existing infra)
- Placeholder draw `exe_main.cpp:3832-3847`; placeholder input `exe_main.cpp:1900-1913`.
- Screen 0 (`kT0`) reachable via `TableForScreen(0)` / `Nav_DevGoto(0)` (`MenuNavSM.cpp:1034`);
  record builder/cursor/slide (`Nav`,`BuildRecords`,`PlaceCursor`,`Nav_MoveCursor`
  `MenuNavSM.cpp:1055`) already handle screen 0 -> geometry+strings for free.
- **New seam:** render the pause menu over the frozen 3D scene (original draws case7==case3
  then layers menu, `exe_main.cpp:3834`). Invoke the frontend menu-record render path gated on
  `GameFlow_IsPaused()`, near `exe_main.cpp:2929-2936`.
- `Frontend/MenuModal.cpp` (`MenuModalRender` 0x00433f40) is the generic posted DIALOG, not
  the pause list — use only for a Restart/Quit confirm.
- Input: replace ESC/ENTER shim with Up/Down/Select/Back through `Nav_MoveCursor` + the select
  dispatcher (`Nav_Select`/`FUN_0043dfd0`). Keep the demo/capture bypass (`demo` bool
  `exe_main.cpp:1893`) so headless parity captures still exit on ESC.

## 3. Per-item behavior — wired vs new
| Item | Behavior | Status |
|---|---|---|
| Resume (0x18) | mode7->3, `GameFlow_SetPaused(false)` | wired; drive from menu select not just ESC |
| Options (0x27) | push Options over pause | new/optional; `[UNCERTAIN]` until decomp; default defer |
| Restart Race (-0xe00000 -> `FUN_0040de10`) | re-run current race same config | NEW; needs `FUN_0040de10` + `GameFlow_RequestRestart()` resetting live `RaceSession` |
| Quit Race (-> frontend, `FUN_0043d2a0(1,0)`) | teardown -> frontend screen 1 | wired (`GameFlow_RequestExit`); re-bind from ENTER to menu item |
| Quit Game | exit to desktop/title | new/verify; lowest priority |

Minimum faithful set: Resume (wired) + Quit Race (wired) + Restart (new, blocked on
`FUN_0040de10`). Options/Quit-Game may be listed-but-deferred `[UNCERTAIN]`.

## 4. Acceptance gate (parity harness, not screenshots — `re/analysis/parity_tooling.md`)
1. Draw-list diff of the pause screen: original-side burst (`re/frida/race_hud_burst.py` /
   `race_draw_burst.py`; caveats memories `race-hud-burst-harness-gotchas`,
   `no-driving-hud-race-ui-is-standings`) pausing original in a race vs standalone
   `MASHED_DBG_DRAWSTREAM` paused frame via `re/tools/drawlist_diff.py`. GREEN or RED-with-cited-rows.
2. Text/raster via `re/tools/imgdiff.py` vs an original pause-screen reference.
3. Behavioral: Resume/Quit via `GameFlow` state on a desktop run (checklist step 5); Restart via
   a `diff-original` Frida check on `FUN_0040de10` once hooked (inline-JMP live, not compile-and-run).
4. Capture-safety regression: headless parity capture still exits on ESC, never enters menu
   (preserve `demo` bypass `exe_main.cpp:1893`).

### Files a later session touches
- `exe_main.cpp` — remove placeholder (:3832-3847), rework paused input (:1900-1913), add
  paused menu-render near :2929-2936.
- `Race/GameFlow.{h,cpp}` — add `GameFlow_RequestRestart()`; RVA map already `GameFlow.h:31-46`.
- `Frontend/MenuNavSM.cpp` — screen 0 (`kT0` :55) ready; wire pause select dispatch (reuse
  `Nav_MoveCursor` :1055, `FUN_0043dfd0` ladder :106+).
- `Frontend/MenuModal.cpp` — only if a Restart/Quit confirm dialog is wanted.

### Ghidra work order (read-only pool slot)
`FUN_0043d7c0` (dispatch+action map, resolves §discrepancy) -> `FUN_0043dfd0`/`FUN_0042ac90`
(select->action) -> `FUN_0040de10` (restart) -> dump ENGLISH.DAT for kT0 msgids
0x47,0x18,0x27,0x1c,0x1d,0x1e. `FUN_0042bf30`/`FUN_0042c1c0`/`FUN_004929d0` only if pause is
event-layered rather than a pushed Nav screen.
