# Local split-screen multiplayer — v1.1 plan + scaffold (2026-06-19)

Scope: **local split-screen 2-player** (split-screen render + 2nd input + MP frontend).
**Online = wontfix** (no netcode). Grounded in the spike (`mashed_re.exe`:
`D3d9Render/TrackRenderer.cpp`, `Race/`, `Input/`, `Frontend/`). Turnkey for the v1.1
effort — deliberately a plan, not a session-tail rewrite, because the core change
refactors the **verified-working** scene-draw and needs per-view screenshot verification
on a clean display (the spike's race render is currently GREEN; don't regress it blind).

## 1. Split-screen render — the core (TrackRenderer::Render)

Today `Render` (≈2386) computes ONE camera at line **2468** (`eye/at` from
`race_cam_`/chase `car_pos_`/orbit) and draws the scene ONCE: view/proj setup
**2470-2482**, then world + cars + AI-car loop + wheels + copters + particles through
**≈2790**.

Change (lowest-risk = lambda, captures locals/members by ref — no param threading):
1. Wrap the scene-view (2470 → end of the copter/scene draw, **before** the particle
   block) in `auto drawView = [&](const float ve[3], const float va[3], float aspect){ … };`
   Substitute inside: `eye`→`ve`, `at`→`va`, the hardcoded `800.f/600.f`→`aspect`.
2. **Particle pool advances ONCE per frame**, not per view: pull `parts_.Update(dt,…)`
   OUT of the lambda to before the view loop; the lambda keeps `parts_.Render(dev,ve,va)`
   (drawn in each viewport).
3. View loop:
   - 1P: `dev->SetViewport(full 0,0,W,H)`; `drawView(eye, at, (float)W/H)`.
   - 2P: top `SetViewport(0,0,W,H/2)` + `drawView(p1eye,p1at, (float)W/(H/2))`; bottom
     `SetViewport(0,H/2,W,H/2)` + `drawView(p2eye,p2at, …)`; then restore the full viewport
     (before the 2D/HUD pass, which must stay full-screen or be drawn per half).
4. **P1 camera** = the existing one (line 2442-2455). **P2 camera** = mirror the chase-cam
   math (2451-2455) on `ai_cars_[0].pos`/`.yaw` (car slot 1 becomes player 2). When the
   real race camera is wanted per player, run a 2nd `RaceCamera` instance keyed on car 1.

Verify: `MASHED_SPLITSCREEN=1` + the race-demo; screenshot must show two stacked race
views (top P1 / bottom P2). Gate the whole thing behind the player count so 1P is byte-identical.

## 2. Second input (Input/)

The input layer drives player 0. Add a **P2 device**:
- Simplest: a 2nd keyboard map (e.g. P1 = WASD/arrows, P2 = numpad / IJKL) read alongside P1.
- Better: enumerate a 2nd DirectInput gamepad (device index 1) for P2.
Feed P2's `DriveInput` (ctrl bytes [0]/[1]=steer, [4]=accel, [5]=brake — see
`project_wsc_ai_port`) to **car slot 1** (`ai_cars_[0]`), replacing its AI controller while
`g_playerCount==2`. The per-car drive step (`TrackRenderer::UpdateCar` / the AI loop)
already runs per slot — route slot 1 from P2 input instead of `Ai_Standalone_Tick`.

## 3. MP frontend (Frontend/)

The nav state machine is the ported `FUN_0043d2a0` + the kT* descriptor tables
(`project_standalone_menu_state_machine`). Add a **player-count / "2 Players" entry** to
the mode/game-type screen; on confirm set a shared `g_playerCount = 2`. `RaceSession` /
`TrackRenderer` read it to enable split-screen (§1) + P2 input (§2). No new screens are
strictly required for a first cut — a toggle on the existing Game-Type select suffices.

## 4. Race logic

4 entrants already exist (player `car_pos_` + 3 `ai_cars_`). 2P = car0 (P1) + car1 (P2,
human) + car2/3 (AI). Scoring/laps/elimination are already per-car (`ScoreAward`/lap
sequence), so they need no change beyond counting 2 humans.

## Status: IMPLEMENTED (2026-06-19, branch ws-visual-polish)

All three components are built + verified in the isolated worktree:
- **§1 Split-screen render — DONE, visually verified.** `TrackRenderer::Render` scene-view
  wrapped in a `drawView` lambda; `MASHED_PLAYERS>=2` renders P1 (top) + P2 (bottom, chase
  car slot 1), each its own viewport/aspect; particle pool advances once, renders per view;
  1P byte-identical. Evidence: `verify/mp/vp_split.png`, `verify/mp/vp_split2.png` (two stacked
  race views; relocated from verify/ root at salvage-merge 2026-07-02 per verify/ subfolder convention).
- **§2 2nd input — DONE (wired + builds).** In the AI control loop, when 2-player, car slot
  1's controller is overridden by P2 keyboard (arrows → steer/accel/brake `ctrl[]`).
- **§3 MP frontend — DONE.** Shared runtime `MashedPlayerCount()` (env-default, accessor in
  TrackRenderer.h/.cpp); **F2** cycles it 1↔2; the window title shows `PLAYERS:N (F2)`
  (verified). The toggle drives §1 + §2 live.

Coherent: F2 in the frontend → 2P → split-screen + P2 keyboard on car slot 1.
**Online netcode: wontfix** (as scoped). Polish backlog (not blocking): per-half HUD,
a 2nd `RaceCamera` instance per player (currently P2 uses a chase cam), an in-menu
selector widget instead of the F2/title control.
