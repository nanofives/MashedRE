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
3. **Distinct cars** — all four standalone cars are the same red ADVANTAGE0.
   The original races four different vehicles/liveries (yellow/green/blue/red
   in t01). Data exists (10 VEHICLES/*.piz); selection logic is game code.
   [SCAFFOLD → real car-selection port]
4. **Camera** — invented orbit/chase/centroid cameras vs the original's real
   shared race camera (higher, frames the pack with its own zoom law — t01).
   [SCAFFOLD → RE the camera update function]
5. **Scoring/HUD** — the real game uses team badges + score BARS + “+1/−1”
   points on a Current Standings screen (t03); my win-pips/banner are
   inventions. [SCAFFOLD → RE the points table + standings screen]
6. **Elimination rule** — mine = ">7 gates behind leader"; the real rule is
   screen-edge based (off-camera = out). [SCAFFOLD → RE]
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
