# RESULT PLAYTEST-PAUSE-MENU (2026-11-22, branch fix/playtest-pause-menu)

Playtest report (Mariano): "pause menu is completely different". Verdict: FIXED on the default path. The paused
screen's draw list is GREEN against the original's screen 0 (117/117), and every entry does what the original's
code does.

## Gap

The standalone drew a hand-written stand-in over the frozen race (`exe_main.cpp` "PAUSED / [ESC] Resume / [ENTER]
Quit to Menu", marked `[SCOPED]` since 2026-09-06). No part of it was the original's screen. ESC toggled pause, ENTER quit.

The original (citations from the matched sources `re/match/s07/00432080.c`, `re/match/s08/0043dfd0.c`,
`re/match/s08/0043d7c0.c`, `re/match/gffu/004929d0.c`, `re/match/s15/00492d30.c`, `re/match/s15/00492e90.c`,
plus headless Ghidra decomp of 0x00432080 / 0x005c9d00 / 0x0043c5b0 / 0x0042d3a0 on Mashed_pool0):

- Entry: 0x004929d0 case 3. FUN_0042f6a0 (DAT_0067e9fc) != 0xb and FUN_0040e350 (DAT_0063ba8c) != 0xb, != 9, then
  FUN_00432080(FUN_005c9d00()) moves to mode 5. 0x00432080 needs DAT_0067eca4 == 0 and the pause-action edge (pad byte +9),
  sets DAT_0067eca4 = 1, clears the nav stack (FUN_0042d3a0) and DAT_0067e9f8 / e844, and stores DAT_0067ecac = 0 (0x005c9d00 returns 0).
- Screen: frontend screen 0 (kT0): header 0x47 "Transmission Interrupted", items 0x18 Continue / 0x27 Options /
  0x1c Restart Race / 0x1d Quit Race / 0x1e Quit Game (ENGLISH.DAT in Font36.piz). It is pushed by 0x0043dfd0's
  DAT_0067eca4 1->2 arm. Mode 4 renders it as a full frontend frame (0x00492e90 case 4: 0x00492e60 + 0x0043c5b0 +
  0x00433f40). The race is not drawn (screenshot `verify/screen_id/race_fire6_settled.png`, in-race pause of the original).
- Fade: 0x0043dfd0 ramps DAT_0067eca8 (alpha of 0x0042aae0's full-screen black quad) per frame by (int)(DAT_007f1004*k).
  k = +1000 [0x005cc9fc] for phase 1 and phase 4, -500 [0x005cda88] for phase 2, -800 [0x005cd8f8] for phase 5.
  DAT_007f1004 = 0x32/3000 every frame (0x004933d5, 0x0049351f). Modes go 5 -> 4 at eca4 2, 4 -> 6 at 5, 6 -> 3 at 0.
  The race tick runs only in modes 3 and 6 (0x00492d30). Mode 6 draws nothing while DAT_0067eca8 >= 200.
- Input: kT0's flag run has ff0b / ff0c / ff10 / ff12 and no ff11. So Back does nothing on screen 0. The pause key is
  read only in mode 3, so ESC does nothing on the pause menu. Only Continue resumes.
- Select arms (0x0043dfd0):
  - 0xff150000 Continue: FUN_0043d2a0(0,2) + DAT_0067e914 = 1.
  - 0xff500000 Options: push 8.
  - 0xff1e0000 / 0xff1f0000 / 0xff200000: FUN_0042bf30(0x32 / 0x33 / 0x34, code, 2, 0x2e, 0x2f, 0), the Yes/No confirms.
  - Yes on those runs 0x0043d7c0 case -0xe20000 (reload, FUN_0040de10, e914 = 1), -0xe10000 (FUN_00493570,
    FUN_0043d2a0(7,0), FUN_0040de10) or -0xe00000 (FUN_00493580, FUN_0042b950, FUN_0043d2a0(1,0), FUN_0040de10).

## Change (default path, `mashedmod/src/mashed_re/`)

- `exe_main.cpp`:
  - `PauseMenuState` block: PauseAllowed / PauseOpen / PauseTick / PauseDrawFade / PauseConfirmYes.
  - The InRace input runs the menu nav in phase 3 only.
  - The race render branch is skipped in phases 2..4, so the frontend frame draws. Records are hidden from phase 4.
  - The fade quad is drawn after the chrome and before the records.
  - ModalGo steps 60 / 61 / 62.
  - Screen-0 select arms.
  - No ESC / Backspace pop on screens 0 and 1 (no 0xff110000).
  - The stand-in is removed.
  - `MASHED_PAUSE_DEMO` capture driver.
- `Frontend/MenuNavSM.{h,cpp}`: `Nav_PauseEnter` (0x00432080 + push), plus `Nav_PauseSnapshotSave/Restore/Drop`.
  The snapshot is standalone flow: it puts the pre-race stack back on Continue / Restart, because the post-race
  frontend flow resumes from it.
- `Race/GameFlow.{h,cpp}`: `GameFlow_EndRaceForRestart`.
- `D3d9Render/DrawStreamDump.{h,cpp}`: `DrawStreamDump_FrameIndex` (capture driver).

Standalone differences, by design:
- Restart Race relaunches the race through the launch path with the same config, which reloads the course piz.
  The original's 0x0040de10 re-inits in place.
- The close fade holds at phase 4 until the relaunch is back in InRace.

## Evidence

| what | path | verdict |
|---|---|---|
| draw list, settled pause screen | `diff_scr0_final.txt` (orig `orig/scr0_burst.json` `--screen 0`, standalone `r4/nav/drawstream_re.json` f940..f943) | GREEN 117/117 each frame vs orig f0 and f3 (`--scale-b 1 --exclude-tex 9 --rotate-a 0x42e65a --tol-anim 4`) |
| behaviour, all 5 entries + ESC/Backspace + modal No | `r4/<scenario>/pause/pause_demo.txt` (nav, options, no, restart, quitrace, quitgame) | Continue resumes (paused 0, pre-race stack 6 back). Options pushes 8, Back returns to 0. Restart relaunches (mode 1 -> 2). Quit Race lands on 7 (depth 2). Quit Game lands on 1. ESC / Backspace on 0 and ESC on 1 are no-ops. No keeps the menu. |
| text raster | `imgdiff/` header and footer bands vs `verify/screen_id/race_fire6_settled.bmp` | same strings, positions and sizes (0x47, 0x41, 0x42). Mean diff 9.05 / 7.68 is backdrop movie frame + checker phase through the translucent bands. |
| nav_coverage | `nav_coverage.txt` | screen 0 not statically reachable from root 1 (entered by the pause key), so its 5 actions are covered by the dynamic walk above |
| gates | `gates_exe12a81d33.txt` | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3, equal to `re/orchestrator/merge_train.json` refs |

The gates ran on exe 12a81d33: the merged tree, before two capture-driver-only edits (the MASHED_PAUSE_DEMO
pre-launch wait and log fields). Those edits are inside `RunPauseDemoStep`, which runs only with MASHED_PAUSE_DEMO set.

## Open

- Confirm modals (`diff_modal33.txt`, orig `--post-modal 0x33,0xff1f0000,2,0x2e,0x2f,0` vs standalone `r3_ds_modal/no`):
  RED, and every row sits in the standalone's shared modal renderer (hand code in WinMain, not a port of 0x00433f40).
  - Dim quad alpha: 0xa6 against the original's 0xbf (0x0042abfa via 0x00434009).
  - Border: 1.5 px quads against the original's line primitives (0x004274c4, called from 0x0042c0ae..0x0042c147).
  - The remaining rows are the highlighted row (cursor 3 against cursor 0 in the forced capture) and checker phase.
  - Shared with the load-dialog fix, not changed here.
- U-9804: what the original's frontend shows after Quit Race -> Back (7 -> 0) -> Continue / Restart. The standalone
  only reloads screen 0 there.
