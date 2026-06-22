# Frontend faithfulness verification — full navigation run (2026-06-13)

Goal (user): verify all frontend screens (except pause/race-gated) are visually
faithful in a full navigation run; complete unblock of everything frontend;
then build race foundations.

Method: capture the ORIGINAL (`re/frida/capture_orig_screens.py` — boots
MASHED.exe via Frida, drives the real nav SM `FUN_0043d2a0` to each screen,
on-demand backbuffer dump through the d3d9-shim Present hook → `verify/orig_screens/`)
and the STANDALONE (`MASHED_GOTO=<scr>` + `MASHED_DBG_BBDUMP` → `verify/std_screens/`)
for the same 17 frontend screens; side-by-side composites in `verify/cmp/`
(`scripts/make_frontend_pairs.py`). Both channels are the post-Present client
backbuffer (no window chrome / DWM scaling) so the comparison is apples-to-apples.

Verification set: {1,2,3,4,6,7,8,15,16,18,19,24,29,30,31,32,33}.
Excluded per goal: pause/race-gated (0,5,20,21,23,25,26), network (11–14),
NOT-USED (9,10,17).

## Two blocking regressions fixed first (both were silent)
1. **Original boot** — PCA auto-added `DWM8And16BitMitigation` to MASHED.exe's
   AppCompat layer; it conflicts with our d3d9 proxy → 0xC0000005 pre-CreateDevice.
   Fixed by re-running `setup_mashed_compat.ps1` (cleans HKCU). See
   memory `project-dwm-mitigation-breaks-shim`.
2. **Standalone init-hang** — the d3d9-shim build emitted `d3d9.dll` into
   `mashedmod/build/`, the same dir as `mashed_re.exe`; the standalone then loaded
   the shim as its d3d9 import and deadlocked in the shim's DllMain (LoadLibrary of
   a `d3d9_real.dll` absent from that dir). Fixed: `build_d3d9_shim.bat` now emits
   to `build/d3d9shim/`; removed the stray DLL. Won't recur.

## Per-screen verdict
FAITHFUL (chrome + title + item list + highlight bars + prompt strip + sliders/
toggles all match the original):
- s1  Game Type Select (main menu)   — orange selected bar, blue item bars, greyed Bonus Features, "Select" prompt, MASHED watermark ✓
- s2  Single Player                  — ✓
- s3  Multi Player                   — ✓
- s8  Options                        — greyed Load/Save Game ✓
- s19 Sound                          — 3 volume sliders + Insults, "Confirm/Back" ✓
- s30 Gamma Correction               — slider, "Back" ✓
- s31 Bonus Features (4 cam toggles) — Off values ✓
- s32 Autosave                       — (value text, see below)
- s33 Bonus Features (2 cam toggles) — ✓
- s29 Controllers                    — items + bars ✓ (help line, see below)

FRONTEND DIVERGENCES — addressed by the ENGLISH.DAT switch (below), pending
display-up re-capture:
- s4  title "Player Color Select" (USA.DAT) vs original "Player Colour Select"
- s1  missing credit "Designed and Developed By Supersonic Software Ltd."
- s29 missing help line "Normal controls - one player per controller"
- s32/s18/s24 missing setting VALUE text (On/Off/Standard/…)

  Root cause: the standalone fed `GetMenuMessage` from Font36.piz **USA.DAT**
  (American, and a sparse placeholder missing the credit/help/value strings); the
  original loads **ENGLISH.DAT** (British, full). Fix applied in `exe_main.cpp`
  (LoadMessageTable now reads ENGLISH.DAT — a sibling piz entry in the same
  message-table format, so descriptor-table string ids resolve identically; only
  the language text changes). RE-VERIFY pending (display asleep blocked the final
  standalone re-capture; the original references already in `verify/orig_screens/`).

CONTENT-PENDING = the frontend→race boundary (render chrome+title; their content
is drawn by campaign/player/race-setup dispatchers keyed on state a frontend-only
build lacks — this is exactly the "build race foundations" next phase):
- s4  Colour Select — original shows a palette row of 6 colored car-head icons +
      numbered player slots; standalone shows one cycling car livery.
- s6/s7 Challenge Select — original: track list (Angel Peak…) + large track
      preview + player car row; standalone: empty.
- s15 Ability Select — original: Elite/Pro/Amateur/Rookie difficulty grid × player
      rows; standalone: empty.
- s16 Team Select — original: Team 1/Team 2 columns × player rows; standalone: empty.
- s18/s24 Game Mode — original: per-setting VALUE column + vehicle preview + player
      car row, and a state-built item list; standalone: label-only list (and the
      item set differs because the original builds it from the chosen mode).

## Bottom line
The navigable frontend chrome / font (FGDC20) / item layout / highlight bars /
prompt strips / sliders / toggles are FAITHFUL across every reachable screen. The
remaining gaps are (a) language/aux-text — fixed via the ENGLISH.DAT switch,
pending display-up re-capture; (b) the content-heavy state-dependent screens,
which are the natural handoff into race foundations (they need campaign/player/
race-setup state to populate). Captures: `verify/{orig_screens,std_screens,cmp}/`.
