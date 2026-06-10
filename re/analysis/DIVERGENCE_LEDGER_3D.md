# 3D divergence ledger — standalone vs original (2026-06-10 reconciliation)

References: `verify/parity3d/orig_race_t01..t05.png` (original Quick Battle,
captured live via `re/frida/race_refs.py` — t01 = in-race wide shot, t03 =
the REAL "Current Standings" screen) vs `verify/parity3d/sa_race_dump.png`
(standalone, same piz, after the fog/sky/alpha gap closures).

Legend: **[GAP]** = renderer feature missing/cheap to add · **[SCAFFOLD]** =
invented behavior that must be REPLACED by an RE'd port · **[DATA]** = needs
more format/RE work · ✅ = closed this pass.

## Ranked divergences

1. **Fog / distance haze** — ✅ CLOSED: `Setup_Fog(start_frac, end, r,g,b)`
   parsed from COURSE.LUA → D3D9 linear vertex fog + fog-colored clear.
   Verified on dump (grey haze matches the reference's depth falloff).
2. **Sky** — ✅ wired (`Sky_Filename` → sky clump drawn first, z-write off,
   unfogged). NOTE: per-track verification pending; the original's sky has
   cloud layers/UV scroll that a static clump won't animate. [GAP residue]
3. **Distinct cars** — ✅ CLOSED 2026-06-10: real selection ported
   (FUN_0040d110): all four cars are LIVERIES of one model (vehicle table
   0x005f37a8, 6 DFF variants per piz, car = (id/6)*6 + livery). Standalone
   AI cars now load Advantage1..3 (sa_liveries_t05.png: green/blue/red pack).
   Residual adapter: livery-per-player default = player index (mode-0xb
   verbatim); DAT_007f1a1c writer untraced.
4. **Camera** — ✅ CLOSED 2026-06-10: the real shared race camera
   (FUN_00446520) verbatim-ported to Race/RaceCamera.cpp — pair framing,
   LED.piz per-gate angles, zoom/distance/pitch laws, springs, sway.
   Validated against a live original trace (zoom 286/286 rows; offset/
   pitch within hmix margin). The orbit/free cameras remain as dev-viewer
   tools only. Residual: countdown/start uses the race cam (original may
   use a start cinematic — unverified).
5. **Scoring/HUD** — ✅ DATA CLOSED 2026-06-10: the real points system is
   ported (FUN_0040eee0 + FUN_0040b290 + Race::EvaluateResult 0x00410510):
   per-elimination awards (4P: −2/−1/+1-capped/+2), score floor 0, signed
   delta + 6000 ms flash, elimination order, match win at score > 11.
   Standalone rounds verified (mashed_re.log: runner-up +1 / winner +2
   accumulation). [residual: PRESENTATION — the standings drawer
   (FUN_0043a610 lobby panel; 0x0041af50.. HUD sprite cluster; badge
   sprites/bars layout) still approximated, not verbatim]
6. **Elimination rule** — ✅ CLOSED 2026-06-10: the real rule is neither
   ">7 gates" nor literally screen-edge: a car dies when the camera's
   REQUIRED ZOOM saturates at 10.0 (cam+0x9a0, checked by fcomp at
   0x00410ee3), and the victim is the lesser-progress member of the
   most-separated pair (wrap-adjusted 80/20/100). Ported verbatim
   (RaceCamera::EliminationCheck); empirically confirmed live (zoom hit
   10.0 → race-phase 6→7).
7. **Handling** — velocity/grip/drag rates harvested from live telemetry but
   the force pipeline is approximated; input-matched diff deferred (needs an
   in-race input injector — the menu input override does not reach the race).
   [SCAFFOLD + DATA]
8. **AI driving** — gate-ribbon + lanes + corner braking is invented; the
   original follows AI.BSP with its own controller. [SCAFFOLD → RE]
9. **Lighting on vehicles** — original applies `Vehicle_Shininess_Range`
   lighting; standalone uses raw DFF prelight (cars look flatter/darker).
   [GAP — needs RW lighting model or baked approximation]
10. **Powerups, shadows, particles, skid marks, UV anims (Sea.uva), Mist/
    windows overlays** — absent entirely. [DATA/feature — out of scope here]

## What is faithful (keep)

World geometry/textures/prelight (13/13 validated), collision, props+MTS
placement, AI gate positions, DFF models, fog parameters, the piz/TXD/DAT
pipeline — i.e. the entire DATA layer. Divergences above live in the
RENDERER and BEHAVIOR layers only.
