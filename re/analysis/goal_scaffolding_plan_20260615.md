# Goal: finish frontend-adjacent ports → scaffold all remaining subsystems (2026-06-15)

User goal (2026-06-15): "port all missing elements adjacent to frontend and then
build the scaffolding of all the remaining subsystems in the game." This is the
execution roadmap. Aligns with ROADMAP.md v2 phases R2 (menu completion) → R3+
(race foundations + subsystems).

## Phase A — frontend-adjacent finish (the last menu pieces)

A1. **Challenge Select** (the cup-progress screen, nav 6/7 = `FUN_00434720`).
    Flow fix DONE 2026-06-15 (ActionToScreen 0xff1d: SP colour→7, not →ability;
    FUN_0043dfd0 L303+). REMAINING (user "full port now"):
    - Port `FUN_00434720` layout: the cup TRACK LIST (DAT_0067ecbc order array +
      FUN_0040b6c0 track names), per-challenge TROPHY icons, ANIMATED gold stars
      ("Star" sprite, already loaded → kHandleStar), the right-side map PREVIEW
      VIDEO rect + a semi-transparent black detail panel, and the challenge
      DETAILS text. State: DAT_0067ea6c (screen sub-state), DAT_0067ecdc (cup
      stage), DAT_007f0a50 (cup/track unlock), the trophy/medal arrays.
    - Mirror the campaign/trophy state to the original's defaults (probe like
      re/frida/probe_gamemode_state.py); only the unlocked challenges show named.
    - "select challenge → race" LINK waits for the race layer (Phase B).
A2. **Font processing** ("bolder / not so pixelated", recurring). Needs a fresh
    user side-by-side screenshot to pin the exact defect; candidate levers: the
    MashedFont alpha-gamma LUT (currently 0.72), POINT vs LINEAR sampling, the
    atlas decode. (Disc beige fix 2026-06-15 was the SAME family — was a modulate
    bug, not the decode.)
A3. **Ability/Team select** real content (team/MP modes) — the team-array setup
    (DAT_0067e850/e938 + FUN_0046dc00) + per-player ability cells. Lower priority
    (team/MP path; SP flow now goes to challenge).

## Phase B — subsystem scaffolding (the rest of the game)

The src tree already has per-subsystem dirs (mostly dev-.asi hook reimpls). The
scaffold = a coherent STANDALONE game loop + stubbed subsystem interfaces wired
into mashed_re.exe, so each subsystem can be filled in demand-driven. Order by
dependency:

B0. **GameState / game loop** — the top-level state machine (Frontend ↔ Loading ↔
    Race ↔ Results) + the per-frame RaceSession::Tick. The frontend already runs;
    add the Race state + a stub RaceSession that owns the subsystems below.
B1. **Track** — load a track from .piz (geometry .dff/.bsp, collision, spawn
    points, the cup/challenge track table). Piz/Txd/Rws loaders already exist.
B2. **Render (RW subset)** — 3D scene render for the track + vehicles. The dev
    viewer spike + librw fallback are the ratified path (renderer-gate note).
B3. **Vehicle** — physics/handling integrator + state; the divergence-closure
    work already ported camera/elimination/car-selection bits.
B4. **Camera** — race camera (LED.piz angle format cracked; partly done).
B5. **Ai** — opponent driver controllers (waypoints/racing line).
B6. **Collision / Physics** — car-track + car-car.
B7. **Powerups** — the in-race power-up system (icons already RE'd for game-mode).
B8. **HUD** — in-race HUD (position, lap, timer, powerup) — HUD dir exists.
B9. **Audio / Rws** — RWS sample playback (engine, FX, music).
B10. **Particle / FX** — smoke, sparks, explosions.
B11. **Input (in-race)** — controls during a race (frontend input exists).
B12. **Save** — gamesave read/write (save-restore FUN_00404e80 already patched).

Each B-item: (1) define the standalone interface (header) + a stub impl that the
game loop calls; (2) fill in demand-driven via Ghidra RE of the owning functions
(subsystem_map.md maps RVAs→subsystems). Scaffold first (compiles + runs with
stubs), then port.

## Status snapshot (2026-06-15)

- Frontend: 15/17 non-race screens faithful; s18/s24 game-mode ~12.5% (visual
  parity, power-up icons residual). This round fixed: disc colour, colour-select
  (colours behind + moving cursor + wider keyboard), Select/Back glyphs, load
  modal text+animation, s18 plate overlap, colour→challenge flow.
- Phase A1 (challenge select draw) is the immediate next frontend-adjacent piece.
- Phase B is the demand-driven build-out; B0 (game loop + RaceSession stub) is the
  entry point that makes the rest wireable.
