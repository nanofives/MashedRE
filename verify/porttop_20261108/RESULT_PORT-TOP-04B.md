# RESULT PORT-TOP-04B: HANDOFF (DisplayInit 0x004921d0 + ViewportInit 0x00428590 under MASHED_ORIG_TOP)

Verdict: HANDOFF. No code yet. Branch `orch/PORT-TOP-04B` cut from `orch/PORT-TOP-04A` @ `a90ed1970`. The session
that did TOP-03 / 04A is long enough that a fresh worker is cheaper. This file is its brief.

## What the two functions do (matched sources)

- **DisplayInit `0x004921d0`** (re/match/s15/004921d0.c, 100%): mode info (RW `0x004c2f00` / `0x004c2ed0`, already
  bound in `LibRw/RwApiBind.cpp`), render size from `0x00498bc0` / `0x00498bd0` (exe bodies, return the exe
  resolution stand-ins), `0x00467110` (main RW camera at `DAT_006905b0`: camera + frame + camera raster (type 2) +
  z raster (type 1), view window 0.8 / 0.8, near 0.1, far 180, plus `0x004c2bf0` into `DAT_006905b4`),
  `0x0042b890` / `0x0042b8a0` (store width / height at `DAT_0067ea54` / `+2`), `0x0042d560` and `0x0042f660` (two
  more cameras through `0x0042d420` into `DAT_0067f18c` / `DAT_0067f190`, then `0x0042d4a0` / `0x0042f530`), then the
  loading-disc texture: `0x004997b0` (FindResource / LoadResource / LockResource of RWTEXDICTIONARY 404 on the input
  hinst, matched), `0x004b3eb0` (C2, **no matched source**: TXD from memory into `DAT_0077195c`), `0x004c5c00`
  RwTexDictionaryFindNamedTexture("LoadIcon") into `DAT_00771960` (bound, TOP-03). Setting `DAT_0077195c` makes
  WinMain reach the bound RwTexDictionaryDestroy `0x004c5930` at exit.
- **ViewportInit `0x00428590`** (re/match/s06/00428590.c, 100%): one loading frame. Clear the main camera black
  (`0x004c1bb0`), BeginUpdate (`0x004c1a00`), draw the loading disc (`0x00428450` -> Im2D quad `0x00450b10`, texture
  `*DAT_00771960`), EndUpdate (`0x004c19f0`), show the raster (`0x004c1be0` -> `0x004c7730`, the Present).

## Callee tree (50 nodes; from re/match sources + hooks.csv, walked to RW leaves)

Game nodes with a matched source and no exe body: `0x0042b890 0x0042b8a0 0x0042d560 0x0042d420 0x0042d4a0
0x0042f660 0x0042f530 (C2) 0x00467110 (+ static 0x004670a0 in the same file) 0x00467020 0x004671a0 0x004997b0
0x00499720 0x00428450 (C3) 0x00450b10 (C3)`. Exe bodies already present: `0x0042b8b0 0x0042b8c0 0x0042f510
0x0042b930 0x00498bc0 0x00498bd0 0x004c1a70`. Wrappers that exist asi-only and must join the exe list with a
MASHED_STANDALONE branch (TOP-02 pattern) rather than a second exe-only body: `MatchS07_0042b890`,
`MatchS07_0042b8a0`, `MatchS16_004997b0`, `MatchS19_004c1a00`, `MatchS19_004c1bb0`, `MatchS11_004671a0`.

RW core nodes to bind in `LibRw/RwApiBind.cpp` (each RW 3.x identity to be confirmed from its matched source first,
NO-GUESSING; the names below are the reading of the sources, not yet checked against librw behaviour):

| rva | matched source reads as | librw candidate |
|---|---|---|
| `0x004c1d30` | camera create | `rw::Camera::create` |
| `0x004c0b30` | frame create | `rw::Frame::create` |
| `0x004c0740` | object set frame (via `0x004c0e50`) | `Camera::setFrame` |
| `0x004c77c0` | raster create (w, h, 0, type 2 / 1) | `rw::Raster::create` (CAMERA / ZBUFFER) |
| `0x004c1c80` | camera view window | `Camera::setViewWindow` |
| `0x004c1a40` / `0x004c1b10` | near / far clip (calls `0x004c1a70`) | `Camera::setNearPlane` / `setFarPlane` |
| `0x004c2bf0` | camera-derived object into `DAT_006905b4` | to identify |
| `0x004c7760` | raster call from `0x0042d4a0` | to identify |
| `0x00467020` -> `0x004c0c20` / `0x004c1cf0` / `0x004c7650` | camera teardown (frame destroy, camera destroy, raster destroy) | `Frame::destroy`, `Camera::destroy`, `Raster::destroy` |
| `0x004c1bb0` | camera clear through RwGlobals `+0x9c` | `Camera::clear` |
| `0x004c1a00` / `0x004c19f0` | camera vtable `+0x18` / `+0x1c` (BeginUpdate / EndUpdate) | `Camera::beginUpdate` / `endUpdate` |
| `0x004c1be0` -> `0x004c7730` | show raster | `Camera::showRaster` / `Raster::show` |

No matched source: `0x004b3eb0` (TXD from memory: needs the decomp, then librw `StreamMemory` +
`TexDictionary::streamRead`, or a listed stand-in) and the RW leaves under `0x00450b10` (not walked: check its source).

## Risks to measure, not assume

1. **librw state after ViewportInit's frame.** The hand renderer draws on the same adopted device through its own
   D3D9 path and librw. A librw Clear / Im2D / Present at boot can leave librw's cached render state or
   `engine->currentCamera` out of step with what the hand path expects. The 12 captures are the gate. If they move,
   compare against a run that skips only ViewportInit's present.
2. **Extra Present before the title.** The frame limiter and any frame counters see one more Present. The
   deterministic nav demo counts its own frames, but check `NAV_DEMO` phases and `walk_*` identity.
3. **Camera rasters at render size.** Three librw cameras with their own rasters (type 2 / 1) on the adopted device.
   Memory only, unless one of them becomes `currentCamera` for the hand path's librw submit.
4. **The hand path never runs these.** Today `DAT_006905b0` / `DAT_0067f18c` / `DAT_0067f190` / `DAT_0077195c` stay 0
   in the exe. Grep the exe for readers of those addresses before turning them non-zero.

## Method (as 04A)

Private roots `D:/MashedWT/PORT-TOP-04B_root{,K}` (robocopy of `original/`), `knob_boot.py` with `PORTTOP_OUT`,
`mashed_lock.py run --label PORT-TOP-04B`, test-slot outside the lock. Nav demo four runs knob ON / OFF / OFF / ON with
`mashed_re_settings.bin` restored from a baseline before each (copy `D:/MashedWT/PORT-TOP-03_data/settings_baseline.bin`).
Default gates `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` with `A4_ROOT` / `A4_OUT` (create `A4_OUT` first).
Keep comment lines above functions from starting with a `.text` RVA (rva-lint binds them as bodies; write
`// rva 0x...`). After any merge run `backfill_exe_file.py` **and then** `backfill_port.py` (the first alone drops the
`port` column).
