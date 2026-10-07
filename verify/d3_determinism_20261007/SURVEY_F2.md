# SURVEY — leg F2 static determinism survey (worker account, 2026-10-07)

Produced by a read-only delegation to the account2 worker (Opus, Read/Grep/Glob only) over
`mashedmod/src/mashed_re/` alone. Verbatim worker output below, header line and raw-JSON pointer
stripped. This is **static evidence for F2's design**, not a measurement: PREREG_F1 §2 records that
it cannot substitute for F1's own measurement, and PREREG_F2 §4 does not let any gate pass on it.

---


Static survey complete. All findings below are from `mashedmod/src/mashed_re/` only, with `file:line` citations and verbatim snippets.

---

# Mashed leg F2 — determinism source survey

## (1) THE FRAME TIMEBASE

**Verdict: the physics/AI car step runs on a FIXED timestep by default** (`di.dt = kSimStep = 1/s_simHz`, default `1/60 s`). A **real-QPC accumulator** decides *how many* fixed steps run per render frame; that accumulator input is the only measured quantity, and it is pinned when `MASHED_DETERMINISTIC` is set. Setting `MASHED_SIM_HZ=0` reverts to a single **measured** `GetTickCount` dt step.

### 1a. The two clock sources

`exe_main.cpp:404-408` — the display/scheduling clock (`DetTicks`), synthetic when deterministic else `GetTickCount()`:
```cpp
inline DWORD DetTicks() {
    return g_det_clock
        ? static_cast<DWORD>(static_cast<unsigned long long>(g_det_frame) * 1000ull / 60ull)
        : GetTickCount();
}
```

`exe_main.cpp:2835-2841` — the per-frame `dt` used for camera/demo/`t`, also copied into `ci.dt`/`di.dt`. `GetTickCount` granularity (~15.6 ms):
```cpp
static DWORD s_t0 = DetTicks();
static DWORD s_prev = 0;
const DWORD now = DetTicks();
const float t = static_cast<float>(now - s_t0) * 0.001f;
const float dt = (s_prev == 0) ? 0.f
               : static_cast<float>(now - s_prev) * 0.001f;
s_prev = now;
```

`exe_main.cpp:2852-2867` — the **high-resolution physics delta** (`sim_real_dt`, QPC) + its clamp + the det pin + the rate env:
```cpp
static LARGE_INTEGER s_simPrev = []{ LARGE_INTEGER q;
    QueryPerformanceCounter(&q); return q; }();
LARGE_INTEGER s_simNow; QueryPerformanceCounter(&s_simNow);
double sim_real_dt = (double)(s_simNow.QuadPart - s_simPrev.QuadPart)
                   / (double)s_qf.QuadPart;
s_simPrev = s_simNow;
if (sim_real_dt > 0.25) sim_real_dt = 0.25;  // clamp: no spiral after a stall
...
if (g_det_clock) sim_real_dt = kDetStep;
static const int s_simHz = []{ char b[16];
    DWORD n = GetEnvironmentVariableA("MASHED_SIM_HZ", b, sizeof b);
    return n ? atoi(b) : 60; }();
```
- `kDetStep = 1.0/60.0` (`exe_main.cpp:403`); `s_simHz` default **60**.

### 1b. Fixed-step accumulator (the actual step)

`exe_main.cpp:2887` / `2907`: `ci.dt = dt;` … `di.dt = dt;` (the GetTickCount dt is the *initial* value).

`exe_main.cpp:3036-3057` — the dispatch:
```cpp
if (s_simHz <= 0) {
    // A/B hatch (MASHED_SIM_HZ=0): original variable-dt single step.
    steerHoldApply();
    g_track.UpdateCar(di);   // di.dt already == the GetTickCount dt
    simStepsThisFrame = 1;
} else {
    const float kSimStep = 1.0f / (float)s_simHz;
    static double s_simAccum = 0.0;
    s_simAccum += sim_real_dt;
    while (s_simAccum >= (double)kSimStep && simStepsThisFrame < 6) {
        di.dt = kSimStep;
        steerHoldApply();
        g_track.UpdateCar(di);
        s_simAccum -= (double)kSimStep;
        ++simStepsThisFrame;
    }
}
```
- **Fixed path (default):** `di.dt = kSimStep` (exact `1/60`). Clamps/guards: spiral clamp `sim_real_dt ≤ 0.25` (`2858`), substep cap **6** (`3050`).
- **Variable path (`MASHED_SIM_HZ=0`):** `di.dt` stays the measured `GetTickCount` dt — a true measured timestep.
- **Step COUNT per render frame** = `floor(s_simAccum / kSimStep)`, driven by `sim_real_dt` (real QPC) unless `g_det_clock` pins it.

### 1c. dt fan-out into the sim (all consume `in.dt`/`dt`)

- Player physics: `TrackRenderer.cpp:3203` `VehiclePhysics_StepPlayer(in.dt, io)`; AI physics `3553` `VehiclePhysics_StepCar(v, in.dt, io)`; pre-race `3125` `LaunchRev_PreRaceTick(in.dt, …)`.
- Race/rule/HUD accumulators: `countdown_ -= in.dt` (`3115`); `race_time_ += dt` (`4919`); `cam_ticks_ += (double)dt * 3000000.0` (`5105`); `delta_timer_[i] -= dt * 1000.f` (`5111`); `RE::Rule10Tick(rulep_, dt)` (`5141`).
- Whole `Vehicle/*` integrator chain takes `dt` as its first/second arg (ForceIntegrator, Integrate2, AeroStabilize, VehicleControl, BodyOrientationIntegrate).
- **AI clock** — frame-rate-dependent integer accumulator (see §4).

Frontend-only (not race sim): `g_wall_dt` (`exe_main.cpp:1745` default `0.016f`, set `4438`), used for a slide counter (`4804`).

---

## (2) EXISTING DETERMINISM KNOBS

| Env var | Default | What it changes | Reaches AI/physics step? |
|---|---|---|---|
| `MASHED_DETERMINISTIC` (`g_det_clock`) | **OFF** (`exe_main.cpp:394,8109`) | `DetTicks`→frame counter (`404-408`); **pins `sim_real_dt = kDetStep`** (`2864`); suppresses live/ambient input (`2935` gate `!g_det_clock`); pins MPEG frame index (`MpegVideoTexture.cpp:167,182`) | **Yes** — via the `sim_real_dt` pin (exactly 1 fixed step/frame) and via `dt` |
| `MASHED_DET_FRAMES` (`g_det_frames_max`) | 0 = off (`exe_main.cpp:402,8112`) | Quits after N rendered frames; makes run *length* a function of frame index | No computed value; removes the wall-clock-kill reintroduction of nondeterminism (`395-401`) |
| `MASHED_SIM_HZ` (`s_simHz`) | **60** (`2865-2867`) | Sets `kSimStep=1/s_simHz`; `=0` → single variable-dt step | **Yes** — defines the fixed step, or disables it |
| `MASHED_SLOTSTATE_SEED` | OFF (`TrackRenderer.cpp:379-380`) | `Ai::I32(0x005f2770)=0x005f2728` (A/B diagnostic torque seed) | Diagnostic only |
| `MASHED_REAL_PHYSICS` (`=0` revert) | ON (ported) (`TrackRenderer.h:306`, `cpp:3457-3458,3535`) | `=0` swaps ported physics chain for the Option-B kinematic AI model | Changes the model, not a determinism knob |
| `MASHED_FPS_CAP` | — (`RwBridge.cpp:239`) | Lives in the **dev d3d9 shim**, not this path; the standalone paces via its own QPC 60 Hz accumulator (`RwBridge.cpp:237-241`) | No (standalone doesn't bind the shim) |

No match for env names containing `TIMESTEP`, `FIXED`, or a `..._SEED` that reseeds a PRNG. The only `*SEED` knob is the diagnostic `MASHED_SLOTSTATE_SEED`.

**On "MASHED_RACE_DEMO DETERMINISTIC":** these are **two separate things**. `MASHED_RACE_DEMO` (`g_race_demo`, `exe_main.cpp:8117`) is the scripted race-demo *driver* (fixed car input, used with `MASHED_GOTO=6`); it does **not** touch the clock or PRNG. The determinism mode proper is `MASHED_DETERMINISTIC`. What `MASHED_DETERMINISTIC` precisely does (per its own header `exe_main.cpp:377-402`): it makes a **scripted capture** bit-reproducible by (a) replacing the wall clock with a frame counter so `dt`/scheduling are a function of frame index, (b) pinning the physics accumulator to one fixed step per frame (`2859-2864`), and (c) suppressing ambient/unfocused input (`2888-2895`).

**Why it is not sufficient for leg F2:**
- It is **OFF by default**, so the normal/interactive race is non-deterministic by construction (variable `dt`, variable substep count).
- It only pins the **clock and input**. It does **not** reseed or re-order any PRNG (the ring's draw index is explicitly called non-reproducible, `AiStandalone.cpp:653-655`), and it presupposes a scripted driver (it suppresses live input), so a free match is not covered.
- It needed `MASHED_DET_FRAMES` added afterward because a wall-clock kill otherwise lands at a different synthetic instant each run (`395-400`).
- The pinnable carriers (§5 items 1–2) are all it covers; the tracker-recorded failure that full 240 s matches still diverge (memory `round-level-outcomes-do-not-reproduce`) is **not** explained by any wall-clock or PRNG read visible in this source tree. `[UNCERTAIN]` — the residual carrier is not locatable statically here.

---

## (3) PRNG (exhaustive)

**No `srand`/`std::srand` anywhere** (grep: no match). **No time-derived seed anywhere.** Every PRNG is fixed-seeded.

1. **`ParticleSystem::Frand`** — xorshift32, cosmetic weather/dust.
   - `ParticleSystem.cpp:22-26`:
     ```cpp
     float ParticleSystem::Frand() {
         rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5;
         return static_cast<float>(rng_ & 0xFFFFFF) / static_cast<float>(0x1000000);
     }
     ```
   - Seed: `ParticleSystem.h:82` `std::uint32_t rng_ = 0x9e3779b9u;` (fixed, per-instance).

2. **`PickupField::Frand`** — same xorshift32. `PickupField.cpp:94-97`; seed `PickupField.h:98` `rng_ = 0x51ed270bu;` (fixed).

3. **`RaceCamera` jitter PRNG** — xorshift32, `RaceCamera.cpp:473-477`; seed `RaceCamera.h:109` `prng_ = 0x12345678;`. **Inert** in the standard race (`jitter_amp == 0` in the live probe, `RaceCamera.cpp:470-472`).

4. **RenderWare ring (`RwRandomNext`/`RwRandomOpen`)** — the real AI PRNG, ported verbatim, **fixed seed, no entropy, opened once, never reseeded**.
   - `AiStandalone.cpp:677-693`:
     ```cpp
     void RwRandomOpen() {                                          // FUN_00534990
         s_rw_ring[0] = 0x9a319039u;
         for (int i = 1; i < 31; ++i) s_rw_ring[i] = s_rw_ring[i - 1] * 0x41c64e6du + 0x3039u;
         s_rw_p = s_rw_ring + 3; s_rw_q = s_rw_ring; s_rw_end = s_rw_ring + 31;
         for (int i = 0; i < 0x136; ++i) RwRandomNext();
     }
     float AiRand(float lo, float hi) {
         if (!s_rw_p) RwRandomOpen();
         const std::uint32_t u = RwRandomNext();
         return (hi - lo) * static_cast<float>(u & 0x7fffffffu) * 4.656612873e-10f + lo;
     }
     ```
   - Stream is deterministic; the **call index is not** (`AiStandalone.cpp:653-655`). Consumers: AI fire decision `AiFireDecision` (`746` `AiRand(0.0f,1.0f)`), bank-switch variation (`1324` `RandUnit()` → `AiRand`, def `1245`), `AiFireDecision` mortar/etc.

5. **`std::rand()` — no longer used in standalone AI.** The header comments (`AiStandalone.cpp:36-37, 1239`) describe a *former* `std::rand()` substitute that was **replaced by `AiRand`** (`1244-1245`). The only live CRT `rand` is the original's static-linked one:
   - `RwpSolverLeaves1.cpp:48` `static int (__cdecl* const s_rand_orig)(void) = (int(__cdecl*)(void))0x005c229bu;`
   - `RwpSolverLeaves1.cpp:253` `uVar7 = s_rand_orig();` — an octree/collision leaf. This is an **absolute-address call into mapped MASHED.exe**, i.e. valid on the **dev `.asi`** side, not the greenfield standalone. CRT `rand` has no `srand` → default seed 1 (deterministic), but its draw index is shared with the original image.

6. **`Fi_RandRange` — deterministic stand-in (NOT random).**
   - `ForceIntegratorStubs.cpp:71` `float Fi_RandRange(float lo, float /*hi*/) { return lo; }  // deterministic stand-in`
   - Used by the standalone physics surface-jitter at `ForceIntegrator.cpp:261,263` (`vF(self,0x2c0/0x2c2) = Fi_RandRange(-r, r)`), so the random-surface impulse is **pinned to `lo`** in the standalone build.

7. **`Live_00472650` / `RandFloat` / live `FUN_00534870`** — dev `.asi`-only live-original PRNG calls (absolute RVAs):
   - `PhysicsChainHooks.cpp:82` `Live_00472650 = (…)(0x00472650);`, used at `813,815` for the random-surface impulse (hook side).
   - `AiPreTick.cpp:66` `RandFloat … (0x00472650)`, used `154,311,320`.
   - `UtilRandIntRange_wfb0f.cpp:65-71` calls live `0x00534870`.

---

## (4) OTHER wall-clock / frame-rate-dependent reads reachable from race logic

- **AI clock (frame-rate-dependent integer accumulator)** — the single biggest non-dt-direct path:
  - `TrackRenderer.cpp:3460` `Ai::Ai_AdvanceClock(static_cast<int>(in.dt * 3000.0f + 0.5f));`
  - `AiStandalone.cpp:1730-1735`:
    ```cpp
    void Ai_AdvanceClock(int units) {
        I32(kOverrideStep) = units;          // DAT_007f1008
        I32(0x007f0ff4u) += units;           // 0x0040fe5e
        I32(kFrame0ff8)  += units;
    }
    ```
  - Read by AI timers everywhere: `AiFireDecision` (`AiStandalone.cpp:730-734`, `dt=I32(0x007f1008)`), `BankSwitch` (`1305`, `1321` `period = I32(0x007f0ff8)/3000`), `AiControlStep.cpp:237,247,272,282,325`, `AiPreTick.cpp:137,151,264,274`. The original measured `DAT_007f1008 == 50` every frame (= `1/60 · 3000`); any `in.dt` jitter moves it off 50 and the **`+0.5f` rounding flips integer gates**.

- **Per-frame dt accumulators in rule/HUD/camera** (listed in §1c): `race_time_`, `delta_timer_`, `cam_ticks_`, `countdown_`, `Rule10Tick`.

- **`QpcTimeScaledTo3Mhz` (UtilMid.cpp:115-130)** — a faithful port of the original's QPC→3 MHz frame timer (`0x004950b0`), but **MASS-DISABLED / not installed** (`UtilMid.cpp:132`) and not called by the standalone loop. Informative (explains the `·3000`), not a live carrier.

- **Particle dt** (`TrackRenderer.cpp:6528` `const float dt = … t - last_t_`) — cosmetic, feeds nothing into sim.

- **Profiler-only, env-gated, never in the default path:** `MASHED_FRAME_PROF` QPC (`exe_main.cpp:2824-2830, 3034, 3162, 3933`), `MASHED_RENDER_PROF` QPC (`TrackRenderer.cpp:5635-5640`), `MASHED_PHYS_PROF` `std::chrono` (`VehiclePhysicsRun.cpp:37-42`).

- **Not race logic (watchdog/self-test):** `GetTickCount64` CreateDevice watchdog, deliberately real (`exe_main.cpp:2472`, rationale `388-390`); `ShadowTrack.h:95,112,519` `GetTickCount` (shadow-exec A/B watchdog); `RwBridge.cpp:210-234` QPC (librw self-test probe); `PromoLoop_round7.cpp:87-89` QPF return ignored.

---

## (5) Most plausible carriers of run-to-run divergence (ranked)

1. **Variable frame `dt` driving the fixed-step accumulator's step COUNT** — `exe_main.cpp:2855-2857` (`sim_real_dt` = QPC delta) feeding `3049-3056` (`while (s_simAccum >= kSimStep …)`). With `g_det_clock` OFF (the default), the number of fixed sub-steps integrated per real interval is a function of CPU load. This is the documented root cause the whole R10 mode exists to kill (`exe_main.cpp:377-384`).
2. **AI clock `round(in.dt·3000)`** — `TrackRenderer.cpp:3460` → `AiStandalone.cpp:1732`. `dt` jitter perturbs the integer `0x007f1008` off 50, and the `+0.5f` rounding flips every AI timer threshold (fire cooldowns `AiStandalone.cpp:730-734`, bank-switch period `1321`, steer-refresh `el>=200`).
3. **Shared `RwRandom` ring call-index** — `AiStandalone.cpp:661-693`; draws at `746` and `1324`. The seed/stream is fixed, but the stream *position* depends on how many `AiRand` calls occurred, which depends on branch outcomes gated by (2). A dt-driven branch flip desyncs the ring for the rest of the match (amplifier, `AiStandalone.cpp:653-655`).
4. **Off-mesh recovery halving `car_speed_`** — cited at `exe_main.cpp:2988-2989` (`TrackRenderer.cpp:2805-2829`): a chaotic map that turns a one-step phase error into 11-vs-71 recoveries over the same window. Amplifier of (1)/(2).
5. **`MASHED_SIM_HZ=0` A/B hatch** — `exe_main.cpp:3036-3040`: physics runs on the raw `GetTickCount` dt (15.6 ms granular, occasional 31 ms double-steps, `dt==0` early-returns). Off by default, but a true measured-timestep path if ever set.

Not carriers: the fixed-seed cosmetic PRNGs (`ParticleSystem`/`PickupField`/`RaceCamera`, §3.1–3.3) and `Fi_RandRange` (`return lo;`, §3.6) — deterministic and/or non-sim.

**Note for F2:** under `MASHED_DETERMINISTIC`, carrier 1 is pinned (`sim_real_dt = kDetStep`, `exe_main.cpp:2864`) and carrier 2 becomes exactly 50 (`dt = 1/60`). The code therefore covers every *statically visible* wall-clock/PRNG carrier. If full-match reproduction still fails under it (per memory `round-level-outcomes-do-not-reproduce`), the remaining source is **not** a wall-clock or PRNG read present in `mashedmod/src/mashed_re/` — `[UNCERTAIN]`, outside what this static survey can locate.

