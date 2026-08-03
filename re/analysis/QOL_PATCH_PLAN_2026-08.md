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

**Stage 3 — camera render interpolation (SHIPPED 2026-08-01, `MASHED_INTERP`):**
The camera's rendered pose lives in the director struct `DAT_00897fe0` (position
`+0x40..0x48`, Euler angles elevation/azimuth/roll `+0x34/+0x38/+0x3c`), and
`FUN_00441760(camStruct)` (cdecl) is the single function that commits those fields
into the camera's RW frame (`*(*(cam+0x84)+4)`, matrix `+0x10`). The director
(`FUN_00446520` via `FUN_0040d470`) runs in the per-frame race tick BEFORE the
render. So the fix wraps the main-loop render call at `0x004922b8`
(`E8 D3 0B 00 00` → `FUN_00492e90`): each rendered frame it lerps the camera pose
between the last two tick snapshots by `alpha = DAT_007719d4/50` (the quantizer's
sub-tick accumulator), re-runs `FUN_00441760`, renders, then restores the true
pose. Position = linear lerp; the three angles = shortest-arc angle lerp; a
>100-unit position jump (respawn/scene cut) snaps instead of interpolating. Only
active in race phases (`DAT_00771968` ∈ {3,6}); at 60 fps `alpha≈0` so it renders
the true pose = bit-identical to stock.

**Measured acceptance (Camera::Apply pose-commit rate during a driven race, cap
165):** interp OFF = **60/s** (stepped at tick rate); interp ON = **389/s**
(render-rate pose commits, ~6× — the smoothing). No crash across 30 s+ driven
races either way. Interpolates the CAMERA only → the world flows smoothly; the
player car is ~screen-centred so its residual step is minimal.

**Car interpolation — SHIPPED (2026-08-01, `MASHED_INTERP` covers cars too).**
After three investigation rounds (below), landed via the car RwFrame subtree:
- renderable = `*(DAT_0063da18 + i*0xd04-slot i*0x2ac)`; a car atom frame =
  `*(renderable+0x4)`; its ROOT = `*(frame+0xa0)` (RW 3.x: child +0x98, next +0x9c,
  root +0xa0). Walk the root's subtree; for each frame, snapshot its LTM (+0x50)
  prev/curr per tick and write the lerp — **LTM only, never modelling** (+0x10 is
  the fixed local part-offset; writing it wedged the car, vel=0). Restore true
  LTM after render. Car count `DAT_008a94d0`; per-frame snapshots keyed by frame
  address (stable within a race), reset on leaving race phase.
- Verified via the trustworthy BBDUMP channel: at 165 fps with real interpolation
  the cars render **clean and intact** at interpolated positions, no tear / freeze
  / crash, across a full race (`verify/qol_asi_20260801/bb_interp165_f3000.png`).
  A crude +80 cartest confirmed the body LTM is read by the render
  (`bb_sub_f1500.png`). At 60 fps alpha≈0 → bit-identical to stock.
- Nuance: the car BODY interpolates (its LTM is render-read); some parts (e.g.
  wheels) recompute their LTM from modelling and micro-step, but per-frame interp
  deltas are a fraction of one tick's motion so it's imperceptible.

**Car interpolation — earlier investigation rounds (ATTEMPTED and DROPPED).** Extended the wrapper
to lerp each car's active render matrix (`0x00881ec8 + i*0xd04 + active*0x40`,
`FUN_0046d4a0`; car count `DAT_008a94d0`) with row+pos linear lerp + restore.
Decisive empirical test — a constant **+80-unit Y lift** applied to every car's
`+0x928` matrix before the render call — left the cars **on the ground**
(`verify/qol_asi_20260801/cartest_lift.png`). Conclusion: the car MESH's RwFrame
is committed **before** the render pass (at tick time); the render matrix at
`+0x928` is a position source for camera/HUD/effects, NOT the transform the car
clump draws from. So interpolating `+0x928` at render time is a no-op for the car
mesh, and the code was reverted (camera-only ships).
Why the camera works but cars don't: `FUN_00441760` is a distinct, callable
pose→frame commit I can re-run at render time; the car clump has no equivalent
re-appliable step in the render path.

**Deeper-hook investigation (2026-08-01, second attempt).** Traced the render
dispatch and did a live memory scan for a car's world-position triplet
(`car_frame_scan.py`). Findings:
- RW matrix layout confirmed: right `+0x00`, up `+0x10`, at `+0x20`, pos `+0x30`.
- The car's transform is replicated in **30+ copies at per-boot heap addresses**
  (a `0x10d3xxxx`-region cluster — the car clump's per-atomic/part frame LTMs;
  a Mashed car is a multi-part `RpClump`), plus fixed-segment copies: the
  per-player render struct `DAT_0063dc38 + i*0x2ac` (passed to `FUN_004c1b40`
  in `FUN_00420050`), a clean matrix at `0x0063d910`, and the `+0x928` render
  matrix already tried.
- The clump-frame LTMs are positioned at **tick time** (the +80 lift on `+0x928`
  at render had no effect), at **heap addresses that change every boot**.
- **What car interpolation actually requires:** at render, for each car, walk to
  its clump root frame (dynamic ptr — via the per-player renderable
  `DAT_0063da18[i]` or `DAT_0063dc38[i]` → clump → frame), override the root
  frame LTM with the interpolated matrix, force the child-frame LTM rebuild
  (RW dirty-flag / `RwFrameUpdateObjects`), render, then restore the true LTM.
  This is RW frame-hierarchy manipulation with real crash risk on the play
  binary, and the frame walk must be re-derived each boot. A dedicated session,
  not a quick extension.
- **Decision (first pass):** not landed. Camera-only ships.

**Dedicated session (2026-08-01, third attempt).** Located a writable car RwFrame
and established a trustworthy verification method, but still could not identify
the frame the car atoms actually DRAW from. Progress:
- Car RwFrame located via live BFS (`car_frame_bfs.py`): `*(renderable+0x4)`,
  standard RW 3.x layout — modelling `+0x10`, **LTM `+0x50`** (pos `+0x80`);
  independently confirmed by a worker notes survey. Implemented render-time
  interpolation of that frame's modelling+LTM (rebuilt `mashed_qol.cpp`).
- `phase_check.py` confirmed the write REACHES the frame LTM (a +80 lift showed
  LTM.y ≈ 85.6 during phase-3 racing) and the game ran a full race **healthy,
  no crash**.
- **Trustworthy verification via the d3d9 shim `MASHED_ORIG_BBDUMP`** (window
  screenshots are untrustworthy for D3D9 on this machine — CLAUDE.md). The
  in-race backbuffer dump (`verify/qol_asi_20260801/bb_cartest_f1700.png`)
  renders **cleanly** (no corruption) with the racing cars **on the ground** —
  the +80 lift to `*(rend+0x4)`'s LTM did NOT move the drawn cars. So that frame
  is another non-render copy, like `+0x928`.
- **Net:** two strongest candidates eliminated with trustworthy evidence
  (`+0x928`, `*(rend+0x4)`); the car transform lives in 30+ copies and the actual
  atom-draw frame is still unidentified. My writes are proven **non-corrupting**,
  which de-risks future work.
- **Now-cheap iteration method for next time:** for each remaining candidate
  frame (`*(rend+0x8)` [matrix @+0x74], `*(rend+0xbc)` [+0x50], or the deeper
  clump-child frames in the `0x10d3xxxx` cluster), apply the +80 cartest lift and
  BBDUMP an in-race frame — the one where the cars float is the draw frame. Then
  interpolate it (mind child frames if it's a clump root). This is a bounded
  search now that the trustworthy capture loop works.
- Scratch probes: `car_frame_find.py`, `car_frame_scan.py`, `car_frame_bfs.py`,
  `phase_check.py`; lift evidence `verify/qol_asi_20260801/bb_cartest_f1700.png`. Opponents currently step at 60 Hz relative to the smooth
camera; the player car is ~screen-centred so its residual step is minimal.

**Residue for a later session:** car-mesh interpolation (above); aspect-ratio
audit at non-4:3.

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
**INVESTIGATION (2026-08-01) — strong root cause, not yet live-confirmed.**

Confirmed it is a NATIVE bug (the owner reported it before any QoL work existed;
boot patches don't touch physics). RE survey (worker, cited from C1 notes) points
to a single culprit:

- **`FUN_0046EF70`** (wheel contact spring/damper resolver, `0x0046ef70`, C1
  `re/analysis/vehicle_dynamics/0046ef70.md`; called from broadphase
  `FUN_004709a0` only when a contact is present). On a contact frame it:
  1. **overwrites** the car linear velocity `+0x9B0/9B4/9B8` with the summed
     contact-spring force, directed along the contact **face normal**; and
  2. **clamps** it on the LAST active contact: `vel *= clampFactor`,
     `clampFactor = min((1.0 − spring_depth/speed)·k, cap)`.
- The contact normal is the contacted triangle's face normal from
  **`FUN_0046CC40`** (classifier). A ramp-**lip** triangle has a near-vertical /
  overhanging normal. Its per-wheel-flag **reset path is a documented unknown
  (U-3629)** — exactly the hole that lets a stale lip contact linger one frame
  too long. If that stale lip contact is the last active contact on the takeoff
  frame, the spring force redirects/kills forward velocity and/or `clampFactor`
  shrinks it → **forward velocity → ~0, car drops straight down.** Intermittent
  because it needs a stale lip contact to land exactly on the takeoff frame.
- The ported A3–A6 force chain is airborne-clean (A6a 20/20 ndiff=0, A6b 12/12),
  so the defect is in this **C1 contact layer**, not the force chain.

**Repro built:** `--track 11` (Warzone) is the only track whose racing line
crosses a real jump. Probes `re/…/scratchpad/jump_probe{,2}.py` classify takeoffs
(grounded `+0x9E0` 4→0) by horizontal-velocity retention. In one 80s run: 42
takeoffs captured; the player's clean jumps were all NORMAL (velocity retained) —
no *dead* jump surfaced this run (intermittent). AI-car velocity field reads 0
(they store velocity elsewhere), so use render-pos delta for AI hspeed.

**Player-in-the-loop capture tool (BUILT + verified 2026-08-01):**
`re/frida/capture_jump_bug.py` — the owner plays normally and this logs the
smoking gun on every dead jump. Headless grinding was proven futile first: 5+
Warzone races produced many low-speed airborne bumps (13–40 grounded→0/run) but
ZERO high-speed ramp launches (the bug's trigger) — the AI don't take the ramp at
racing speed headlessly, so no dead jump can occur. The tool sidesteps this: it
hooks `FUN_0047eb30` (per-tick, detects a dead takeoff via render-transform: fast
in → forward speed collapses + falling) AND `FUN_0046EF70` (captures the car
linear velocity `+0x9B0` BEFORE vs AFTER the resolver, implicit-EDI = car record).
On a dead jump it writes to `log/jump_capture.txt`: the tick window, the
0046EF70 before/after velocity (CONFIRMED if |horiz| collapses across it), and the
per-wheel contact normals/loads (to see the bad lip normal). Usage:
  `py -3.12 re/frida/capture_jump_bug.py`   (auto-attaches to the running game;
  `--pid N` if several). Verified: attaches, both hooks install clean.
Hygiene: attach-only, never spawns/kills, refuses to guess among multiple MASHED.

**FIX VERIFIED + SHIPPED 2026-08-02 (`MASHED_JUMPFIX`, on by default).**
Player-in-the-loop confirmation (`re/frida/capture_jump_bug.py`, deceleration
detector): the violent ramp dead jump — fast approach, forward render-speed
collapses from ~0.18 to ~0.03 the instant grounded goes 4→0 at the lip, tilted
contact normal (−0.061, 0.991, −0.12) with only the front wheels loaded (10,10,0,0)
— reproduced twice in ~1900 ticks with the fix OFF (car0 t1376, car3 t1372, same
ramp) and did NOT reproduce across ~10000 ticks (5× longer) with the fix ON. The
owner independently "couldn't recreate it." Residual logged events with the fix on
are round-restart teleports (detector false-positives, all 4 cars jump at one
tick) or genuine wall-collision stops on flat normals (correct). Root cause
mechanism confirmed exactly as predicted: the ramp-lip contact normal's backward
tilt cancels forward velocity at takeoff. Launcher enables it by default; opt out
with `-NoJumpFix`. Evidence: `verify/qol_asi_20260801/jump_capture_CONFIRMED.txt`.

**Fix DRAFTED 2026-08-01 (`MASHED_JUMPFIX`).** In
`mashed_qol.asi`: retarget the sole call to the once-per-tick bridge FUN_0047eb30
(`E8 @0x00470e15`) to a wrapper that, before the bridge runs, restores each car's
horizontal velocity (`+0x9B0`/`+0x9B8`) to the previous tick's value IF the car is
leaving the ground (`+0x9E0 ≤ 1.5`) AND its horizontal speed collapsed >60% vs
last tick from a real speed — the takeoff-kill signature. Vertical (`+0x9B4`) is
untouched (upward launch + gravity intact). Clean ABI (avoids FUN_0046EF70's
implicit-EDI); corrects the kill one tick later (car dips imperceptibly instead of
dropping dead). Guard is inert during normal driving (grounded=4), wall crashes
(stay grounded), and normal airborne flight (no >60% single-tick horizontal loss
airborne). Smoke-tested: installs clean, cars race normally at speed (12/tick, 63
ground transitions, no freeze/crash). Launcher opt-in: `-JumpFix`.
**NOT verified to actually fix a dead jump** — needs the capture-tool log
(`log/jump_capture.txt`) from a real dead jump to confirm the restore fires on it,
plus a normal-jump corpus to confirm no regression. Off by default until then.

**Remaining: live confirmation + fix.** Confirm by hooking `FUN_0046EF70` and
logging `+0x9B0` before/after on takeoff frames until a dead jump is caught
(needs many runs, or a **player-in-the-loop capture** — the owner triggers it
while the hook logs, the natural repro for an intermittent bug). **Fix design**
(toggleable `mashed_qol.asi` hook, off by default): on the takeoff frame (grounded
→0 / last contact lost), preserve the horizontal velocity from the last grounded
frame instead of letting the lip-contact spring overwrite/clamp it. NOT shipped —
a physics guard must be verified against a real dead jump AND a normal-jump corpus
before touching the play binary (a wrong guard would break normal jumps).

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

## Item 6 — Interp residue: tick-cached overlay/effect elements (investigated 2026-08-03)

Owner report after the 165Hz interp shipped: powerup icons over cars, the "!"
indicator, car shadows, wheels, fire still step at tick rate against the
interpolated car body.

**First attempt (shipped 2d186b9a, INSUFFICIENT):** lerp the car record render
matrix (`0x00881ec8 + i*0xd04 + act*0x40`, active index at record `+0x9A8`;
layout cited from the id->matrix lookup `FUN_0046d4a0` @0x0046d4a0) inside the
interp wrapper, restore after render. Harmless and alpha-correct, but the owner
retested: no visible change.

**Root cause of the no-change (STATIC, decisive):** reachability classification
of all 48 callers of `FUN_0046d4a0` against the two main-loop roots
(render `FUN_00492e90`, logic `FUN_00492d30`, sets computed on pool13):
**47/48 callers are logic-side; the only render-side caller is `FUN_00444b60`
(map/spline debug draw).** Every element that positions itself from the car
transform does so DURING THE TICK and caches the result — a render-time lerp of
the source matrix cannot move them. The fix must interpolate each element's
CACHED state (or re-run its positioning step) at render time — per-element RE.

**Empirical probes** (`re/frida/carpos_source_probe.py`, `carpos_root_test.py`,
runs under `verify/carpos_probe/`):
- Consecutive-frame lift pairs with a logic freeze (force frame-time source
  `FUN_00493390` to return 0 -> 0-tick frames, native under the decouple) and
  on-demand BBDUMP pairs. Self-validating triple (on/off/off2) added after
  scene-motion noise polluted early rounds.
- Root RwFrame single-matrix lifts (modelling +0x40 / LTM +0x80): both at noise
  level -> the lagging elements do NOT derive from the root frame matrices at
  render; the RW dirty-resync theory is dead.
- Record scan: two matrix buffers at +0x928/+0x968 (pos +0x958/+0x998) plus a
  per-wheel-looking triplet run at +0x9f8..+0xac4 stride 0xc. Fixed globals
  echoing car0 pos: 0x63d910, 0x63dc38 (camera-feed), transient 0x7ec/0x7ed
  arrays.

**Environment lessons (cost most of the session):**
- Force-killed MASHED instances make Windows PCA re-add `DWM8And16BitMitigation`
  to original\MASHED.exe (breaks the d3d9 proxy -> next spawns wedge before
  render). Remedy: clear PCA Store + re-run setup_mashed_compat.ps1. THIS
  SESSION'S KILLS DID IT TWICE.
- MASHED stops rendering when unfocused: `FUN_00499690` (0x00499690) blocks in
  WaitMessage while the active flag `DAT_0077391c`==0. Forcing the flag to 1
  (+ one WM_NULL PostThreadMessage to wake a parked pump) keeps it rendering in
  background — now built into the probe agent. Candidate future QoL toggle
  (play/capture while unfocused).

**Status: OPEN.** Next lane (dedicated session): pick ONE element (car shadow is
the most visible), find its draw under `FUN_00492e90`, trace where its
world-space inputs were cached during the tick, and lerp THAT cache in the
interp wrapper (same snapshot/restore pattern). Repeat per element (powerup
icon, "!" marker, wheel transforms, fire emitters). Wheels: their clump-frame
LTMs ARE lerped and still step -> they draw from another copy (candidates: the
record +0x9f8 wheel array, or per-wheel heap frames written by suspension at
tick).

### Item 6 progress — shadow (2026-08-03, session 2)

The car shadow is a **render-to-texture pass**: `FUN_0041f8f0` (0x0041f8f0, per-
slot pre-pass in PerPlayerViewportRender) parks a projector camera at
`sunDir(*(DAT_0063d850+4) +0x30..38) * DAT_005cca00 + *(slot +0x258..0x260)`
and renders the car clump into the shadow raster (slot = 0x0063dc38 + i*0x2ac).
The xyz at **slot+0x258 is the tick-cached car position anchor** — the shadow
stepped because this anchor stepped. `FUN_0041faf0` (VehicleShadowRender,
gate byte slot+0x294 bit 0x40, frame at slot+0x64) is the AIRBORNE billboard
variant — gate observed CLEAR in normal driving.

**Shipped:** interp wrapper now lerps slot+0x258..0x260 per car (SEH-guarded
snapshot/lerp/restore, ShadowLerpOne/ShadowRestoreOne). The clump rendered into
the shadow raster already uses the interpolated frames, so shape+position both
follow the smooth car.

**Crash lesson:** ALSO lerping the slot+0x64 airborne-billboard frame crashed
race load twice — that object pointer is stale outside airborne states and the
writes corrupt a live heap block (SEH cannot catch a successful bad write). Do
NOT write through slot+0x64; if the airborne shadow ever needs interp, gate on
+0x294 bit 0x40 first.

Verified: full stack (decouple+interp+jumpfix, 165 race cap) races 90s+ through
round transitions, no crash. Visual smoothness pending owner check at 165Hz.
