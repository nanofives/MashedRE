# Video-playback subsystem — standalone scoping decision (2026-07-03)

**Question (RE_MASTER_PLAN M1):** the ~4 C2 video-playback rows — port them, or
`deferred-not-needed`? **Recommendation: `deferred-not-needed` for the standalone
(mashed_re.exe) S-DoD** — the standalone already reproduces the user-visible behavior
via a modern functional equivalent. The verbatim port + `diff-original` remains a
WS-H/`.asi` item (MCP-gated → Fable). One product-choice + one small RE gap noted below.

claude2 (Accenture) worker survey; read-only + build. No Ghidra/Frida. All RVAs cited
from `hooks.csv` + the `video_mci*` notes + `MOVIE_SKIP_ROOTCAUSE_2026-06-22.md`.

## The original's video subsystem (legacy DirectShow)

A DirectShow graph that renders `.mpg` frames through a custom TextureRenderer filter
into two D3D textures (`DAT_00771a10/a14`), blitted each frame:
- `sub_00494a80` (0x00494a80, C2) — movie-start: alloc surface + build graph + set
  `DAT_00771a04=1`.
- `FUN_004944c0` (0x004944c0, C2) — build/run the **indexed**-movie graph (index → the
  `.mpg` name table @ `0x006147c4`); CoCreateInstance + AddFilter TEXTURERENDERER +
  AddSourceFilter + `Run()` on IMediaControl.
- `FUN_00494c80` (0x00494c80, C2) — the **small.mpg** player (a second, fixed graph;
  cdecl/void, plain `ret`).
- `sub_00494460` (0x00494460, C2) — VideoClose (+ latent COM UAF in `FUN_00494320`,
  documented, parked fix `patch_mashed_fix_movie_uaf.py`).
- `IntroSplashOrchestrator` (0x00495350, C2, already impl'd `Frontend/IntroSplash.cpp`)
  — loops the 4 intro logos {1,2,3,4}, keypress-skippable.
- Supporting COM ctors/Release: `FUN_00493c00`, `FUN_00494ac0`, `FUN_0049ec10`,
  `FUN_0049dd60`, `FUN_004a1790`. Getters (`VideoStateFlagGet`, `IntroVideoDimGetter`,
  `ThunkVideoStateGet`, `VideoGetRenderWidth/Height`) are already **C4**.

## Video content actually present (asset survey)

`original/toastart/PC/MOVIES/`: `empire.mpg`, `intro.mpg`, `renderware.mpg`,
`supersonic.mpg` (the 4 intro logos), `frontend.mpg` (menu backdrop), `small.mpg`
(a separate transition/loading clip). Pristine copies in `.../MOVIES/backup/`.

- The **4 intro logos** are **skip-patched** at boot (`patch_mashed_skip_intro.py`
  replaces them with 1-frame empties; originals preserved in `backup/`). The standalone
  shows its own splash phases (`exe_main` `g_frontend_phase` 0 legal / 1 title).
- **`frontend.mpg`** is the animated menu backdrop.
- **`small.mpg`** — purpose not traced this session (its player `FUN_00494c80` is even
  skip-NOP'd by `patch_mashed_skip_movies.py`). [see RESIDUAL 1]

## Why the standalone doesn't need the legacy port

The standalone **already reimplements video playback** with a modern, self-contained
path: `D3d9Render/MpegVideoTexture` (DirectShow **SampleGrabber** → NullRenderer →
copies frames into a D3D9 texture; `qedit` GUIDs declared locally). `exe_main` uses it as
`g_menu_video` and plays `original/toastart/pc/movies/frontend.mpg` as the F1 menu
backdrop (`exe_main.cpp` ~L5523). So the standalone reproduces the **user-visible**
behavior (menu video plays) **without** the legacy TextureRenderer-filter/COM-ctor chain.

Reproducing the other clips (the 4 real intro logos, or `small.mpg`) for a full P-DoD
playthrough is a **trivial reuse of the existing `MpegVideoTexture` path** — point it at
the file — NOT a port of `FUN_004944c0`/`FUN_00494c80`/etc. So those legacy functions have
**no standalone consumer**: `deferred-not-needed` for S-DoD.

## What this does NOT close (no overclaiming)

- **`.asi` diff-original / verbatim fidelity (WS-H, MCP-gated → Fable).** If the project
  wants a verbatim `diff-original` C4 on the video subsystem, `FUN_004944c0` etc. still
  need porting + `RH_ScopedInstall` + a Frida diff. That is a verification-lane item, not
  a standalone-playability blocker. `MpegVideoTexture` is a **functional** equivalent, not
  a bit-identical port of the TextureRenderer filter.
- **DECISION GATE (yours, mirrors the renderer D2 gate):** accept the modern
  `MpegVideoTexture` as the shipping video path (recommended — the D3D9-spike renderer
  already sets this "functional-equivalent standalone" precedent) **vs.** demand a verbatim
  DirectShow-TextureRenderer port for strict fidelity. If accepted, D-11057-adjacent video
  rows can be marked `deferred-not-needed (functional-equivalent shipped)` via `re-classify`.

## Residuals

1. **`small.mpg` purpose** — untraced (its player is skip-NOP'd, no observed gap). Trace
   its invoker + the `0x006147c4` `.mpg`-name table (Ghidra → Fable) to confirm nothing
   in-game/menu needs it. Low priority; no observed behavioral gap.
2. **Real intro logos (product choice)** — the standalone intentionally skips them. If
   P-DoD wants the branded logos, wire `MpegVideoTexture` at the boot splash to play
   `backup/{empire,renderware,supersonic,intro}.mpg` (or the live copies once un-skipped).
   Trivial; deferred as a polish item.
3. **Trackers** — this is a scoping *recommendation*; the DEFERRED/hooks.csv mutation
   (mark the video rows `deferred-not-needed` under the gate decision) is for `re-classify`
   on account3, not a hand-edit.
