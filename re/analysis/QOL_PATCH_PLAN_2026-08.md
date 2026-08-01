# QoL patch plan — playable "any-FPS" Mashed on the original exe (2026-08-01)

**Direction:** separate from the RE/standalone lanes. Goal is a *player-facing* patched
`MASHED.exe` setup: framerate-decoupled physics (165 Hz PC / 120 Hz TV), a one-click
launcher that bypasses all startup config, everything unlocked, no savegame writes, and
the jumping bug fixed. Executed in its own session; the RE session continues in parallel
(multi-session etiquette applies — kill only your own PIDs, no master-Ghidra writes
without a pool slot).

**Ground rules (inherit from the existing patch discipline):**
- Every binary change is either (a) an idempotent, self-checking, `--restore`-able
  `scripts/patch_mashed_*.py` following the existing pattern, or (b) a runtime hook in a
  new **QoL .asi** (separate from `mashed_re_dev.asi`; same dinput8 autoload path) so it
  is env-toggleable for A/B. Prefer the .asi for anything with tuning knobs.
- `original/MASHED.exe.unpatched` stays untouched. Verify the SHA-256 anchor before any
  new patch authoring.
- Acceptance for behavior changes = measured evidence (statediff / replay harness /
  captured runs), not "feels right". The NO-GUESSING rule applies to every RVA cited.

---

## Item 1 — Decouple physics/game speed from framerate (the hard one)

### What we already know (project evidence, cite before reuse)
- Game speed is **frame-coupled**: uncapped it runs ~360 fps ≈ 6× fast; the current fix
  is the d3d9 shim's Present-side limiter (`MASHED_FPS_CAP`, default 60). So today the
  game is only correct *at* 60 fps.
- The game's logic clock **is** the render-frame counter (replay work anchored it at
  `FUN_004c1be0`; replays are deterministic frame-for-frame).
- We ported the physics: fixed timestep constant 0.05 reconciled in B5a; per-tick
  integrator helpers live in `mashedmod/src/mashed_re/.../RwpIntegrator.cpp` (B5c) and
  the solver island K1..K24 (B5e). **This is the key asset** — we know from our own port
  exactly which laws are per-tick (damping factors, friction per step, impulse clamps),
  so we know what breaks under a different dt.

### Approach options
- **(A) dt-scaling** — find the per-frame dt / tick plumbing and scale physics by real
  frame delta. True any-FPS smoothness, but every `k`-per-tick constant must become
  `k^(dt/dt0)` (damping) or `k*(dt/dt0)` (accumulation), and integration error changes →
  jump heights / collision feel drift. High risk of subtle divergence everywhere.
- **(B) fixed 60 Hz logic, free-running render** — safe but pointless: at 165 Hz the
  world still animates at 60 Hz; no smoothness gained over `MASHED_FPS_CAP=60`.
- **(C) fixed-tick + render interpolation (RECOMMENDED)** — keep the game's logic +
  physics at its native fixed tick (unchanged behavior, replays stay deterministic),
  drive ticks from a QPC accumulator (the standalone already proved this pattern:
  RenderFrame→UpdateCar accumulator locks 60 Hz), and render at display rate by
  **interpolating the RW transforms** (car/camera frames) between the previous and
  current tick. Physics behavior is bit-identical to a 60 fps run; only presentation is
  smoother. 120 Hz is exactly 2× tick so it degrades gracefully even before
  interpolation lands.

### EXECUTED 2026-08-01 — loop map + the actual mechanism (supersedes the staging below)

Ghidra (pool12, anchored binary) mapping found the game **already contains a
fixed-tick quantizer**; the decouple shipped as a 4-byte call retarget instead of
any new accumulator/interpolation machinery:

- Main loop `FUN_00492290`: `while (!DAT_00828300 && FUN_00499690()==0) {
  FUN_004929d0 (session-phase SM); FUN_00492d20 (intro shim); FUN_00492d30
  (logic tick); FUN_00492e90 (render+flip); FUN_004926c0 (audio tick);
  FUN_00493480 (tick quantizer); }`
- `FUN_00493390` (frame-time source, sole caller = the CALL instruction AT
  `0x00493480`): measures real elapsed (`_DAT_007f0ffc`, 3e6 units/s, cap
  200000) but **pins `DAT_007f1000 = 0x32` @0x004933d5** (one 60 Hz tick) and
  `DAT_007f1004 = 1/60f` @0x0049341e. This constant is the entire frame-coupling.
- `FUN_00493480` (quantizer): snap bands 47..53→50, 97..103→100, 147..153→150,
  197..203→200; sub-frame accumulator `DAT_007719d4`; emits `DAT_007f1000 =
  N*0x32` and `DAT_007f1004 = N*50 * (1/3000f @0x005cc948)`.
- Consumers run updates **per tick**: `FUN_00492d30` cases 3/6 and `FUN_0042c960`
  do `N=(DAT_007f1000-1)/0x32+1; do FUN_004111c0(0x32); while(--N)`;
  `FUN_0040fc00` has an explicit multi-tick catch-up (`0x3c < DAT_007f1000`) and
  advances the race clocks (`DAT_007f0fec/0ff4/0ff8`) by tick units. Console
  heritage: the machinery natively supports 0..4 ticks/frame.
- `DAT_007f0ff0` (+0x32/frame @0x00493453) drives cosmetic scroll clocks
  (e.g. `FUN_00401340`) — corrected alongside.
- Menu phases (1/4/7) tick per-frame unconditionally (`FUN_0043dfd0`/`FUN_0043d7c0`
  run outside the tick loop), so menus must stay at 60 fps.

**Shipped implementation:**
- `mashed_qol.asi` `MASHED_DECOUPLE=1`: retarget the E8 at 0x00493480 to a stub
  that runs the original then writes `DAT_007f1000 = measured/1000` (remainder
  carried; cap 200) and fixes `DAT_007f0ff0`. At 60 fps this is bit-identical to
  stock (snap band); at any other fps the native quantizer emits the right tick
  count, including render-only 0-tick frames.
- d3d9 shim `MASHED_FPS_CAP_RACE`: phase-aware Present cap — race phases
  (DAT_00771968 ∈ {3,6}) run at the profile target, all other phases stay at
  `MASHED_FPS_CAP` (60) so menu speed is unchanged.
- Launcher: pc → race 165 / tv → race 120, menus 60, decouple on.

**Measured acceptance (speed_probe, race clock `DAT_007f0ff4` vs wall clock,
phase 3, driving):** decouple + cap165 → present_fps=165.0, tick-units seen
{0,50}, clock rate median 2994 units/s = **0.998× real time** (pre-fix build
without the remainder carry measured 0.965 — the probe detects small speed
errors). **Negative control (decouple OFF, cap 165):** tick-units pinned {50}
(one full tick EVERY frame — the stock constant), and the 40 s hold churned
through 3+ round/intro cycles where decoupled runs stayed in one round — the
documented frame-coupled fast-forward (BOOT_PATCHES.md: ~6× at ~360 fps). A
clean in-race clock-rate number for the control was not captured (rounds cycle
too fast to hold a driving window); the {50}-vs-{0,50} mechanism split is the
control evidence.

**Residue for a later session:** stage 3 render interpolation (at 165 Hz motion
is still 60 Hz-stepped with 3-3-2-3 cadence; at 120 Hz cadence is a clean 2:1).

### Staged execution (C, with A as a fallback experiment)
1. **Map the main loop** (Ghidra, pool slot): from `FUN_004c1be0` outward, identify
   (a) the once-per-frame game-update call, (b) where render/Present happens, (c) every
   read of the frame counter that gates logic. Deliverable: a loop map note with RVAs.
2. **QoL .asi hook of the loop**: accumulator drives 0..N update calls per rendered
   frame at fixed 60 Hz (`MASHED_TICK_HZ` env). Raise/remove the shim's Present cap
   (`MASHED_FPS_CAP=165/120/0`). Milestone 1 acceptance: game speed correct (stopwatch a
   lap-timer minute vs wall clock) at 60/120/165/uncapped; replay determinism harness
   still +0 drift at 60.
3. **Interpolation pass**: snapshot car + camera RwMatrix per tick (we know the vehicle
   record layout, 0xd04 @ `DAT_008815a0`, and the chase cam global `DAT_00897fe0`);
   before Present on non-tick frames, write lerped/slerped transforms, restore after.
   Start with camera + player car only, extend to opponents. Acceptance: capture at
   165 Hz shows distinct interpolated positions between ticks; statediff at tick
   boundaries identical to stock-60.
4. **(optional) A-experiment** behind `MASHED_DT_SCALE=1` if C's 60 Hz tick feel isn't
   enough — but only after C works, and gated by statediff evidence.

**Risks:** input sampling may be inside the update (fine — sampled per tick); some
animation/audio may be driven per render frame (audit in step 1); UI/menu code paths
also frame-coupled (menus can simply tick 1:1 — decouple only in-race if simpler).

## Item 5 — Borderless native-resolution play mode (added 2026-08-01)

The classic play window is 640×480 (the d3d9 shim's forced backbuffer, coupled to
`patch_mashed_fix_camera_res.py`'s 640×480 screen-dim getters). This item makes the
game render at the monitor's native resolution in a borderless-fullscreen window.

**Mechanism** — the two coupled sizes are moved together at runtime (no on-disk
change, so RE boots are unaffected):
- d3d9 shim: `ForcedBackBufInit` reads `MASHED_RES=WxH` (bounded 320..7680 ×
  240..4320) and sizes the D3D9 backbuffer to it; `MASHED_QOL_BORDERLESS=1` gives
  the device window a `WS_POPUP` style pinned to (0,0) at the backbuffer size.
- `mashed_qol.asi` `ApplyRes()`: retargets the screen-dim getters `FUN_00498bc0`
  (width) / `FUN_00498bd0` (height) @0x00498bc0/0x00498bd0 to return the same W/H
  (accepts pristine `A1 <glob> C3` or already-patched `B8 imm32 C3`). This keeps
  the camera frameBuffer raster == device backbuffer, avoiding the null-raster boot
  AV documented in `patch_mashed_fix_camera_res.py` /
  `re/analysis/BOOT_CRASH_ROOTCAUSE_2026-06-13.md`.
- Launcher profiles: pc → 2560×1440, tv → 1920×1080; `-Res WxH` overrides,
  `-Res 0` restores the classic 640×480 titled window.

**Measured acceptance (2026-08-01):** launched pc profile → qol log shows all five
patches (NO_SAVE/UNLOCK/DECOUPLE/RES) applied; window is a borderless popup at
(0,0) covering the full screen; game booted to Game Type Select and rendered the
video backdrop + menu at native res; process survived 30 s (no null-raster AV).
Screenshots in `verify/qol_asi_20260801/screen_1440p*.png`.

**Note:** internal render resolution scales with the backbuffer, so the game is
genuinely sharper (not upscaled 640×480). At 4:3 source aspect on a 16:9 panel the
image fills the width; pillarboxing/stretch behavior is whatever the engine's own
viewport math produces at the new dims — not yet separately audited.

## Item 2 — Launcher that bypasses control/video settings

Mostly assembly of existing pieces:
- Startup dialogs are already patch-silenced (`show_windowed`, `skip_selector`,
  `skip_controller_dialog`, `skip_audio_com`); patched exe boots straight to menu in
  ~5 s, no `launch.exe` (copy-protection wrapper) needed. CLI params exist
  (`-VS/-CS/-L`, parser `FUN_00493900`; MashedRunner documents the full set).
- Build `scripts/mashed_launch.ps1` (or a tiny C++ exe later) with **profiles**:
  - `-Profile pc` → `MASHED_FPS_CAP=165`, videocfg for the desktop monitor
  - `-Profile tv` → `MASHED_FPS_CAP=120`, videocfg for the TV
- Per profile it: verifies/applies patches (`repatch_original.py` idempotent), writes
  the canonical `videocfg.bin` variant, sets env (`MASHED_FPS_CAP`, `MASHED_TICK_HZ`,
  `MASHED_NO_SAVE=1`, unlock template), starts `MASHED.exe`, records the PID.
- Resolution: current shim forces windowed 640×480 backbuffer + `fix_camera_res` 640×480
  (COUPLED — see BOOT_PATCHES.md). If the user wants fullscreen-native on the TV, that's
  a follow-on: teach the shim a `MASHED_RES=WxH` override and move `fix_camera_res` to
  read the same values. Keep out of v1 unless asked.

## Item 3 — Unlock everything + no savegame by default

- **Unlock:** `patch_mashed_unlock_restore.py` is the proven fix (save-restore
  `FUN_00404e80`); Bonus stayed locked — carry that as a known limitation or extend the
  unlock template. Launcher restores a pristine fully-unlocked `gamesave.bin` template
  each boot.
- **No-save:** the gamesave writer is known (`FUN_00404ee0`, WS-G / Save/GameSaveFormat.h).
  QoL .asi hook: short-circuit the writer (return success, write nothing) behind
  `MASHED_NO_SAVE=1` (launcher default). Prefer the .asi hook over an on-disk NOP patch
  so saving can be re-enabled per run. Acceptance: play a round, confirm
  `gamesave.bin` mtime unchanged; disable the env var, confirm saving works.

## Item 4 — Jumping bug fix

**Owner confirms (2026-08-01): the bug reproduces even at the 60 fps cap** — it is a
genuine native bug, NOT framerate coupling, so Item 1 will not fix it (though its
fixed-tick design must not *change* it either).

**Symptom (owner, 2026-08-01):** intermittent — most jumps work normally, but sometimes
at the *moment of takeoff* the car's forward motion just stops and it drops straight
down off the ramp lip. Untested hypotheses to discriminate (NO-GUESSING — these are
starting points for instrumentation, not conclusions): the contact solver killing
horizontal velocity on the last lip contact (an edge-contact normal pointing wrong),
airborne-state transition zeroing/clamping velocity, or a broadphase/edge special case
at the ramp boundary. We ported the relevant code (B5b contact, B5e solver K1..K24,
B5c integrator) — read our C first.
Investigation lane:
1. Reproduce deterministically with `scenario_launch.py` (the A6b airborne lane already
   produces NATURAL full-air states) + deterministic capture mode
   (`MASHED_DETERMINISTIC`/`MASHED_DET_FRAMES`). Because the bug is intermittent, sweep
   approach speed/steering micro-variations until one seed produces a "dead" jump and a
   near-identical seed produces a normal one.
2. statediff the good-vs-dead pair around the takeoff frame — the frame where the
   velocity vector collapses identifies the writer. Then root-cause in the ported
   contact/solver/integrator source (faster to read than Ghidra decomp — we own
   C-level for this subsystem), citing the original RVAs those files carry.
3. Fix as a QoL .asi hook (toggleable), A/B with the statediff lane: dead-jump seed now
   jumps; a corpus of normal-jump seeds is bit-unchanged.

## Order of execution
1. Item 3 (hours — pieces exist) → 2. Item 2 v1 launcher (hours) → 3. Item 1 stages 1–3
(the bulk; milestone-gated) → 4. Item 4 (needs repro; may fall out of Item 1).

## Kickoff prompt for the NEXT session (post-2026-08-01: Items 1-3 shipped)

> Items 1–3 of `re/analysis/QOL_PATCH_PLAN_2026-08.md` are shipped and committed
> (mashed_qol.asi decouple/unlock/no-save + mashed_launch.ps1 + phase-aware fps
> cap; read the EXECUTED section for the loop map). Two work items remain:
> 1. **Item 4, jump bug** (primary): intermittent dead jumps — at the moment of
>    takeoff the car's forward motion stops and it drops off the ramp lip;
>    reproduces even at 60 fps. Follow the plan's investigation lane: seed-sweep
>    `scenario_launch.py` micro-variations until you capture a dead/normal jump
>    pair, statediff at the takeoff frame to find which write kills the velocity,
>    then root-cause in our ported contact/solver source (B5b Collision/, B5e
>    K-island, B5c RwpIntegrator) citing their RVAs. Fix = env-gated hook in
>    mashed_qol.asi; acceptance = dead seed now jumps + normal-seed corpus
>    bit-unchanged.
> 2. **Stage 3 render interpolation** (secondary, only if Item 4 closes): at
>    165 Hz motion is 60 Hz-stepped (0-tick frames re-render the same state).
>    Interpolate car (0xd04 records @ DAT_008815a0) + chase cam (DAT_00897fe0)
>    transforms on 0-tick frames before Present, restore after.
> Multi-session rules apply (kill only your own PIDs, pool slots via the skill).

## Original kickoff prompt (2026-08-01 planning session)
> Read `re/analysis/QOL_PATCH_PLAN_2026-08.md` and execute it in order. Start with
> Item 3 (unlock + MASHED_NO_SAVE hook in a NEW QoL .asi, not mashed_re_dev.asi), then
> the Item 2 launcher script with pc/tv profiles, then Item 1 stage 1 (main-loop map
> from FUN_004c1be0 via a ghidra-pool slot). Multi-session rules apply: another RE
> session is active — kill only PIDs you spawn, use worktree + pool-slot skills, don't
> hand-edit trackers. Item 4 is a native 60 fps bug (owner-confirmed): get my symptom
> description if I haven't given one yet, else characterize it via the scenario_launch
> airborne lane + statediff before touching any fix.
