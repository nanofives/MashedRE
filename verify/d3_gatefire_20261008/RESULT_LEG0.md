# RESULT: D-11073 leg 0 (measure and predict, NO CODE)

Date 2026-10-08. Brief: `re/BRIEF_D11073.md` §2 leg 0. Pre-registration: `PREREG_LEG0.md`, committed
`f30cb571` before the run. No port body, no default flip, no C-level, no tracker change.

Raw:
- `o_leg0.csv` (`.log` kept local, gitignored like the cadence logs): original, 2,481 samples at 60 Hz, PID 22076, not killed by this tool.
  Scenario `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 30 --statediff-out <scratch>`.
- `sa_leg0_inputs.csv`: standalone, 694 samples, 480 with the clock advancing (1,306 frames).
  Own PID 10452, self-exited rc 0 on `MASHED_DET_FRAMES=1500`. Build 18:56, no source newer.
  Env = `run_h4.ps1` baseline, with no lane knobs (so `SLOT_PLAYER`/`SLOTSTATE_SEED` at their default-ON).
  Poller `sa_inputs_poll.py` (this folder): read-only `ReadProcessMemory` at the original addresses.
  Its `slot_tbl`/`slot<v>` columns are a wrong double dereference, so ignore them. `slotb<v>` is the
  `FUN_004103a0` read (`*(0x005f2770) + 0x34 + v*4`), the same as `orig_rampwatch`'s `e470_<v>`.
  The exe maps the original data range (`exe_main.cpp:7617` B7 wedge).

## 0. Verdict first

> **Leg 1 cannot start as scoped.** A faithful `FUN_004103a0`, fed the port's current inputs,
> **holds phase 5 forever**. Its exit test reads `DAT_0063d588` (via `FUN_0041da90`). In the
> standalone that timer is 0.0 on 480/480 live samples and its step `_DAT_007f100c` is also 0.0.
> In the original the timer advances 0.0166667 per frame, from `FUN_0041d930`'s tail
> (`fadd [0x7f100c]` at `0x0041da6a`, `fstp [0x63d588]` at `0x0041da76`). The step itself is
> stored by `FUN_0040fc00` (`mov [0x7f100c],eax` at `0x0040fc57`).
>
> The exe has neither write:
> - `Ai_AdvanceClock` (`AiStandalone.cpp:1901-1906`) models `0x007f1008`/`0ff4`/`0ff8` only.
> - `HudSlideBillboardTick` (`ScenarioLeaves_sa2.cpp`, port of `0x0041d930`, in `exe_sources.rsp:136`)
>   has no exe-side caller.
> - It cannot simply be called, either: it makes raw calls to `0x004c1480`/`0x004c13e0`
>   (`ScenarioLeaves_sa2.cpp:70-75`), and the exe maps only `0x00420000..0x0047ffff` of text
>   (`exe_main.cpp:8346`).
>
> Three more inputs disagree. Each is listed in §3 with what leg 1 must add.

All nine pre-registered gates PASS. Two findings change the brief:
1. **The timer is not a phase-5 timer.** It runs through phases 3, 5 and 6 and is zeroed only on
   the phase-4 frame.
2. **Phase 3's length does not reproduce.** It was 730 frames here, against 651/652 on the two
   cadence runs. Leg 2 cannot gate on a hold length.

## 1. Gates (PREREG_LEG0.md)

| id | verdict | measured |
|---|---|---|
| G-NOPRESS | PASS | `in0..in7` = 0 on 684/684 sub-state-3 samples; phase 3 = 729 frames by clk (50 -> 36,500) |
| G-TRK | PASS | `DAT_00644158` = **30 (0x1e)** on 1,710/1,710 samples with `n994` = 7 (0 before the race loads) |
| G-GUARD | PASS | `DAT_0089898c` = 1 on 1,710/1,710 samples with `n994` = 7; 0 on all 771 earlier |
| G-TBL | PASS | first `n994`=7 sample (clk 50, the first phase-3 frame): entry 0 = (1.500016, 9.699181, 34.498360) vs row 0 of `0x005f8d50` = (1.5, 9.7, 34.5). Types e0..e6 = 0,1,1,1,2,2,3 = column 7 of rows 0..6 |
| G-STEP | PASS | median `d(t_d588)/d(n_d584)` = 0.01666665 over 1,685 same-phase pairs |
| G-RUN3 | PASS | non-decreasing over phase 3, 0.0167 -> **12.1667** (`n_d584` 1 -> 730) |
| G-RESET | PASS | first sub-state-5 sample: `n_d584` = 1, `t_d588` = 0.0167. The phase-4 sample reads 12.1833 / 731, i.e. the reset had not yet run (see §2) |
| G-EXIT | PASS | sub-state 5: `n_d584` 1..**112**, max `t_d588` = **1.866665**. First sub-state-6 sample 1.883332 |
| G-RUN6 | PASS | sub-state 6 runs 1.8833 -> 12.3833 and carries into 7 (17.9999). No reset at 5 -> 6 |

The brief's third prediction (entry 0 starts at `FUN_00441b30(row 0)`) holds to 0.002. The static
basis: `FUN_00441b30` calls `FUN_004c51a0(entry+0xc, row[0..2], 0)`, and combine 0 writes
`param_1[0xc..0xe]`, which is entry `+0x3c/+0x40/+0x44`. The track id is a live-only fact:
`DAT_00644158` is BSS.

## 2. The timeline, frame-exact (`n_d584` is the frame witness)

| sub-state | frames | `DAT_0063d588` | `DAT_005f29b8` |
|---|---|---|---|
| 3 | 730 | 0.0167 -> 12.1667 | 72,520 / 34,048 / -169,304 ... (not a countdown) |
| 4 | 1 | 12.1833 (sampled after the 3->4 frame) | **12000** |
| 5 | **112** | 0.0167 -> 1.8667 | 12000 -> 6450, -50/frame |
| 6 | 631 | 1.8833 -> 12.3833 | **0** |

Phase 5 is **exactly 112 frames**, as predicted: the handler exits on its 112th frame because
112 x 0.0166667 = 1.8667 >= 1.86. The brief's "~108-112" was a sample count.

**Phase-3 length is run-dependent.** Entry 0's z approach matches across runs to 0.001 (frame 200:
20.530 here, 20.529 in run 3). What differs is the target it closes on (`0x00897fe0` +4..+0xc).
That target starts at a different point per run and drifts:
- this run, x: -0.03 -> -0.40 -> -0.11;
- run 3, x: +0.08 -> +0.41.

Release is still site (a) on all three runs. Here entry 0 is 0.037 from the target in xz on the
last flag-1 sample, and the flag drops on the sample where sub-state 3 ends (clk 36,500).
Measured lengths: **651, 652, 730 frames.** [UNCERTAIN] what moves the target. It is the struct
`FUN_00446520` copies from `+0x40..+0x48` at `0x004481c9`. Its source was not traced this session.

## 3. Item 3: the port's live inputs, and what a faithful port would do with them

Standalone values are at the original addresses, and every one was constant over 480 live samples.

| handler input | original (o_leg0) | standalone | faithful-port output with the standalone's value |
|---|---|---|---|
| `DAT_0063d588` via `FUN_0041da90` | 0 -> 1.8667 in phase 5 | **0.0** | `0 >= 1.86` never true: **phase 5 never exits** (BLOCKER 1) |
| `_DAT_007f100c` (step) | 0.0166667 | **0.0** | even with `FUN_0041d930`'s tail ported, the timer stays 0 (BLOCKER 1) |
| `DAT_005f29b8` | 12000 at phase 4, -50/frame | **0** | 0, -50, -100 ... The mode-5 branch at `AiStandalone.cpp:1765` (`elapsed < (0x4a - cd) * 100`, under `MASHED_MODE5_RESET`) then sees negative elapsed instead of 12000..6450 (BLOCKER 2) |
| car alive `0x008815a4 + v*0xd04` (`FUN_0046c7b0`) | 1/1/1/1 on every sample | **0/0/0/0** | a transcription reading the address holds **no** car and copies nothing at exit. The port's alive is `g_aib.alive[v]` (`TrackRenderer.cpp:98`), which `RESULT_CALLWISE.md` measured uneven (BLOCKER 3) |
| `0x00882194`/`0x00882198 + v*0xd04` | not polled on the original | 0 | the exit arm copies 0 into `0x007f0a04`/`0x007f0a08` (stride 12). [UNCERTAIN] the original's values |
| `DAT_0067e9fc` (`FUN_0042f6a0`) | **10** | 0 (`aib_round_type()` returns 3, `TrackRenderer.cpp:101`) | all three values miss the `== 2` arms of `FUN_004103a0`, so no difference there |
| `DAT_0067ea64` (`FUN_0042f500`) | **not polled** | 0 (only `MASHED_TEAM_PLAY` writes it, `exe_main.cpp:8663-8664`) | `FUN_0040dbd0`: with 0, cases 3, 10 and default all reach `FUN_0041b520`. [UNCERTAIN] the original's value, so which helper the original calls is unmeasured |
| `DAT_007f0fd0` | 0 | 0 | agree |
| `DAT_007f1018` | not polled | 0 | `DAT_007f1014 = (0 < clk)` = 1. [UNCERTAIN] the original's value and the readers of `0x007f1014` |
| slot table `*(0x005f2770)+0x34+v*4` | 1,2,2,2 | 1,2,2,2 | agree (`SLOTSTATE_SEED` default-ON) |
| `DAT_0063ba78` | not polled | 0 | `FUN_00426c90` operand; that function is not ported (comment only, asi) |
| `DAT_00644158` (leg 2) | 30 | **0** | a faithful `FUN_00442600` takes the `default` table `0x005f82a8`, not Training's `0x005f8d50`. It must read `aib_track_index()` (= `g_aib.course`, 30 for `kAreas[12]`, `GameFlow.cpp:52`) |

Port status of what leg 1 would execute (worker survey, spot-checked; E = `exe_sources.rsp`, A = `asi_sources.rsp`):

| RVA | what | exe today |
|---|---|---|
| `0040dbd0`, `004103a0` | the two handlers | no body (comments only) |
| `0041d910` | zero d584/d588 (21 B) | **A only** (`PromoLoop_round39.cpp:17-22`) |
| `0041da90` | read d588 (17 B) | **A only** (`PromoLoop_round20.cpp:105-111`) |
| `0041d930` | timer tail + RW billboard | E body, no caller, raw calls out of the mapped text window |
| `0041b520` | `FUN_0041ae20` over 4 records `0x0063c8d0..0x0063caa0` stride 0x74 (`0x0041b520..0x0041b535`) | **A only** naked (`PromoLoop_sessionB.cpp:3653`). `FUN_0041ae20` not surveyed |
| `0045b350` | team-play arm only | A only |
| `0046baa0` | per-car re-init, 571 B | **A only** (`PromoLoop_round80.cpp:73-250`) |
| `0046d7f0`, `0046d780` | per-car field `+0x2194` | E+A (`LaunchRevCharge.cpp`) |
| `0046c750`, `0046c730` | getters | **A only**, duplicated in `PromoLoop_round10.cpp` and `round38.cpp` |
| `00418860` | AI per-frame tick | asi body `AiController.cpp:308-347`. The E copy `ScenarioLeaves_sa2.cpp:437` is **not installed** (`:474` commented) |
| `00426c90` | `FUN_0041ea80` / `FUN_0041e960` | not ported |

**Readers of `aib_game_sub_mode` in the exe** (all through `s_host.game_sub_mode`):
- `AiStandalone.cpp:890` (`ControlStep`), `:1090` (`ControlStepM49`), `:1209` (`ControlStepM8`): branch selectors.
- `:1655` (`AiPreTickRubberBand`, `!= 6` -> return).
- `:1761` (`Ai_ResetVehicleStates`, `== 5` under `MASHED_MODE5_RESET`).
- `TrackRenderer.cpp:4596`: diagnostic only.

**Every executable reader or writer of `DAT_0063ba8c` is asi-only.** No port-side variable models it.

## 4. What leg 1 must add to its scope, before any code

1. **The timer (BLOCKER 1).** Store `_DAT_007f100c = units * (1/3000)` alongside `Ai_AdvanceClock`
   (`0x0040fc57`). Also port `FUN_0041d930`'s tail (`0x0041da5f..0x0041da76`: d584 += 1, d588 +=
   step), called every frame after the sub-state handler, as `FUN_004111c0` orders it.
   Split it from the RW billboard half and name the split partial.
   Both writes go behind `MASHED_SUBSTATE`. `0x007f100c` has exe readers only in that same TU's
   uncalled body, plus `PromoLoop_sessionB.cpp:4016` (asi only). Re-check before flipping anything.
2. **The `DAT_005f29b8 = 12000` seed (BLOCKER 2).** Leg 1 enters at state 4, so it must perform
   `FUN_004111c0` case 3's exit write itself.
3. **Alive routing (BLOCKER 3).** `FUN_0046c7b0` must read the port's `s_host.car_alive`, not
   `0x008815a4`. The port's vector is not the original's 1/1/1/1. Register that as a named
   deviation on the phase-5 gate, or close `SCOPE_CALLWISE`'s alive split first.
4. **Callee moves.** `0041d910`, `0041da90`, `0041b520` (+ `FUN_0041ae20`, unsurveyed), `0046baa0`,
   `0046c750`/`0046c730` live in asi-only TUs. Each needs an exe-built home at its RVA. The two
   getters are duplicated, so pick one body (memory `duplicate-rva-implementations-drift`).
5. **Two columns owed on the next original capture:** `DAT_0067ea64` (which `FUN_0040dbd0` helper
   runs) and `0x00882194/98 + v*0xd04` (what the exit arm copies).

For leg 2: gate on the release rule (site (a), entry 0 within 0.02 of the target), not on a hold
length. Route `FUN_00442600`'s switch through `aib_track_index()`.

## 5. Residuals

- [UNCERTAIN] the source of the phase-3 target's per-run drift (§2).
- [UNCERTAIN] the original's `DAT_0067ea64`, `DAT_007f1018`, `DAT_0063ba78`, `0x00882194/98`. Not polled.
- [UNCERTAIN] `FUN_0041ae20` and `FUN_00426c90`'s callees: not surveyed.
- [UNCERTAIN] which row column `FUN_00441b30`'s `FUN_004a2c48` (ftol) converts into `entry+4`.
  Column 7 is the only one that matches the live types, but the operand is not visible in the decomp.
- The worker survey cost $3.28 (Opus 5). Its claim that `FUN_004103a0`/`FUN_0040dbd0` were absent
  from the three decomp files is right. Their callee sets came from `decomp_pc.py` this session instead.
