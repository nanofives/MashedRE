# PREREG: D-11073 leg 1a (exe substrate, INERT in the default build)

Written 2026-10-09 BEFORE any code or run. Brief: `re/BRIEF_D11073.md` §2 "Leg 1a".
Leg 0 result: `RESULT_LEG0.md` §0, §4. No default flip. No `FUN_0040dbd0` / `FUN_004103a0`, no
`aib_game_sub_mode` change (leg 1b). C3 max. Trackers only via `re-classify`.

## 1. Static reads this session (capstone on `MASHED.exe.unpatched` via `re/tools/pedisasm.py`,
## decomp via `re/tools/decomp_pc.py`, pool `Mashed_pool0`, read-only)

**`FUN_0041ae20` (52 B, `0x0041ae20..0x0041ae53`), the one callee no survey covered.** `__thiscall`
(this in ECX), no calls:
- `0x0041ae20` `mov edx,[ecx+0x64]`; `0x0041ae24` `lea edx,[edx+edx*2]`; `0x0041ae2b`
  `lea edx,[edx*4+0x5f3304]`: row = `0x005f3304 + this[0x64]*12`.
- `0x0041ae32..0x0041ae41`: `this[0x50..0x58]` = the row's three dwords.
- `0x0041ae44` `fld [eax]` / `0x0041ae46` `fadd st0,st0` / `0x0041ae51` `fstp [eax]`:
  `this[0x50]` (float) doubled.
- `0x0041ae49`: `this[0x70] = 0`.
- Preserves ESI/EDI, leaves ECX unchanged (needed by `FUN_0041b520`'s loop).

It is NOT unported: `Copy41ae20` (`Util/PromoLoop_sessionB.cpp:718-730`, asi only, C3 r103 via
`early_window` `thiscall_struct_from_table`) matches the listing above store for store.

**`FUN_0041b520` (22 B)**: `mov ecx,0x63c8d0` (`0x0041b520`); `call 0x41ae20` (`0x0041b525`);
`add ecx,0x74`; `cmp ecx,0x63caa0`; `jl` (`0x0041b533`). Four records. The existing asi body
(`PromoLoop_sessionB.cpp:3653`) calls the RAW address `0x0041ae20`, which the exe cannot execute
(text mapped only `0x00420000..0x0047ffff`, `exe_main.cpp:8346`). The exe home calls the C body
`Copy41ae20` directly instead.

**`FUN_0041d910`** (21 B): `[0x63d584]=0` (`0x0041d910`), `[0x63d588]=0` (`0x0041d91a`).
**`FUN_0041da90`** (17 B): if `[esp+4]` != 0, `*[esp+4] = [0x63d588]` (`0x0041da98`).
**`FUN_0046c750` / `FUN_0046c730`** (25 B each): `cmp eax,0x10; jl` (SIGNED) then
`imul eax,0xd04; mov eax,[eax+0x882194]` (`0x0046c762`) / `[eax+0x882198]` (`0x0046c742`), else 0.

**Choice of body for the two duplicated getters.** `round10` (`EntityVelocityCounterGet`,
`EntityDamageStateGet`) and `round38` (`Table882194Get`, `Table882198Get`) are semantically
identical (both signed `< 0x10`). hooks.csv rows `0046c750`/`0046c730` record the 2026-09-09 dual-
install audit: round10 is the INSTALLED copy, round38 is refused at install (`DUAL-INSTALL
REFUSED`). **Keep round10's bodies; delete round38's.** The two `table_88219x_get` registry
entries (the table-seeded vectors) are repointed to the round10 exports.

**Timer step and tail, and their order in one frame.**
- `FUN_0040fc00`: `fild [esp+4]` of ESI (units) at `0x0040fc09`, unsigned fixup `fadd [0x5cc94c]`
  (2^32) at `0x0040fc0f` only if negative, `fmul [0x5cc948]` at `0x0040fc1b`, stored to
  `0x007f100c` at `0x0040fc57`. `_DAT_005cc948` = `0x39aec33e` (1/3000).
- `FUN_0041d930`'s only caller is `FUN_0040fc00` (decomp callers list), at `0x0040fd11`,
  unconditionally, AFTER the step store and after the AI/physics calls in `FUN_0040fc00`.
- Tail: `fld [0x63d588]` `0x0041da5f`; `mov eax,[0x63d584]` `0x0041da65`; `fadd [0x7f100c]`
  `0x0041da6a`; `inc eax` / `mov [0x63d584],eax` `0x0041da71`; `fstp [0x63d588]` `0x0041da76`.
  Nothing between `0x0041da5f` and the `ret` at `0x0041da7f` depends on the billboard half's
  result (the preceding instruction is `call 0x4c13e0` at `0x0041da5a`, whose return is unused).
- `FUN_004111c0` cases 3, 4, 5, 6 each run the handler, THEN `FUN_0040fc00`.

**Predicted values (numpy float32, bit-exact):** step = `float(50.0f * 0x39aec33e)` =
**`0x3c888888` = 0.016666666**. Accumulated from 0: 112 frames -> **1.8666651**, the first value
>= `_DAT_005ccdf4` (`0x3fee147b`, 1.86) is reached at frame **112**, which is leg 0's measured
phase-5 length and its measured max (1.8667). The port's units are
`(int)(dt*3000+0.5)` (`TrackRenderer.cpp:3534`); under `MASHED_DETERMINISTIC` dt = 1/60 -> 50.

## 2. Exe census (this session, `grep` over `mashedmod/src`, membership from both `.rsp`)

Readers/writers of `0x007f100c`, `0x0063d584`, `0x0063d588`, and the computed bases
`0x0063d580`, `0x0063d55c` (offset-grep trap):
- `HUD/ScenarioLeaves_sa2.cpp` (E+A) `HudSlideBillboardTick` (whole `0x0041d930`): reads/writes all
  three. **No exe caller** (the export name appears nowhere else in `src`).
- `HUD/HudBatch.cpp` `0x0041db80` reads `0x0063d55c+0x2c` = `0x0063d588`: **A only**.
- `Util/PromoLoop_round20.cpp`, `round39.cpp`: A only, being moved by this leg (no caller anywhere).
- Every other hit is a comment (`TrackRenderer.cpp:5566`, `Powerup*`, `RaceCamera*`, `RuleEngine.h`).

=> With the knob ON, **no exe code reads what it writes.** Prediction for the knob arm: the three
control files stay byte-identical to the default ones.

Callers of the seven moved exports anywhere in `src` (excluding definitions): **none**. So the
moved bodies are uncalled in the exe in both arms.

## 3. What this leg writes

1. New TU `Race/SubStateCallees.cpp`, in BOTH `exe_sources.rsp` and `asi_sources.rsp`. One body
   per RVA, RVA comment, `RH_ScopedInstall`, same export names as today (so registry keys and the
   installed copies keep their identity): `0x0041d910` `Clear63d584Pair`, `0x0041da90`
   `DeltaTimeOutGet`, `0x0041ae20` `Copy41ae20`, `0x0041b520` `Update41b520` (now C, calling
   `Copy41ae20` directly), `0x0046baa0` `VehiclePhysicsStateReset`, `0x0046c750`
   `EntityVelocityCounterGet`, `0x0046c730` `EntityDamageStateGet`. The old copies are removed
   (`round80.cpp` and `round38.cpp` become empty and leave `asi_sources.rsp`).
2. `HUD/ScenarioLeaves_sa2.cpp`: the tail of `HudSlideBillboardTick` becomes a named PARTIAL,
   `HudSlideBillboardTimerTail()` (`0x0041da5f..0x0041da76`), which the whole body calls as its
   last statement. One copy of the tail, not two. No `RH_ScopedInstall` on the partial (it is
   not a function entry).
3. `Ai_AdvanceClock` (`AiStandalone.cpp:1901`): under `MASHED_SUBSTATE`, store
   `_DAT_007f100c = (float)units * asFb(0x39aec33e)` (`0x0040fc57`).
4. `TrackRenderer.cpp`: under `MASHED_SUBSTATE`, call `HudSlideBillboardTimerTail()` once per
   frame after `Ai_Standalone_Tick()` and its two dumps, inside the same `faithful_nav` block as
   `Ai_AdvanceClock` (so the step store precedes the tail in the same frame, as `0x0040fc57`
   precedes `0x0040fd11`). There is no port sub-state handler yet, so "after the handler" is
   trivially satisfied. One `[substate] MASHED_SUBSTATE=1` line goes to `mashed_re.log` at first use.

## 4. Gates

| id | prediction | PASS | what it CAN see | what it CANNOT see |
|---|---|---|---|---|
| G-BUILD | both targets build | `build.bat` exit 0, both artifacts newer than every edited source | compile + link | behaviour |
| G-LINT | no new duplicate body | `lint_rva_bodies.py` reports `NEW=0`, AND for each of the 7 RVAs `grep -rn "RH_ScopedInstall(.*0x<rva>)"` over `src` returns exactly 1 line | static install registrations | which copy wins at runtime (G-PATH2 covers that) |
| G-CTRL | default build unchanged | `run_h4.ps1 L1A_A` (the H4_A recipe, new arm name so the committed H4_A files stay as the reference) on the new build: step sha8 `c741a4c5`, gates `642af0ac`, ptrace `05984e30` | the AI step dump, the U-9186 gate dump and the player trace for 14400 frames | anything not dumped (render, HUD, audio) |
| G-INERT | knob writes have no exe reader | a new arm `L1A_SUB` = H4_A + `MASHED_SUBSTATE=1` gives the SAME three sha8, AND its `mashed_re.log` carries the `[substate]` line (knob took) | that the knob's writes do not reach any dumped state | readers outside the dumps (the census in §2 covers those statically) |
| G-STEP | `DAT_0063d588` advances 0.0166667 per frame under the knob | poller arm `SUB` (knob ON, `MASHED_DET_FRAMES=1500`): (a) `dt_100c` bits == `0x3c888888` on every sample after the first with `n_d584 > 0`; (b) on every sample with `n_d584 = n > 0`, `t_d588` == float32 sum of n steps (bit-exact), or the n-1 sum (a torn read, counted and reported); exact share >= 90%; (c) median `d(t_d588)/d(n_d584)` within 1e-6 of 0.0166667 | per-sample values at the original addresses (`ReadProcessMemory` on the own PID) | intra-frame order; the poll is not atomic, hence `n_d584` as the frame witness and the n-1 allowance |
| G-STEP-CTL | the knob is what writes them | poller arm `OFF` (same env, no knob): `t_d588`, `n_d584`, `dt_100c` all 0 on every sample | that the default build writes none of the three | n/a |
| G-PATH1 | each moved body still matches the original | per RVA, GREEN on its registry key: `clear_63d584_pair`, `delta_time_out_get`, `entity_velocity_counter_get` + `table_882194_get`, `entity_damage_state_get` + `table_882198_get` via `run_diff.py`; `copy_41ae20`, `update_41b520` via `early_window_leaf_diff.py` (their arg_types exist only there; that is the harness that produced their C3); `vehicle_physics_state_reset_obs` (NEW key, see below) via `early_window_leaf_diff.py`; `hud_slide_billboard_tick` (body refactored) via `run_diff.py` | the export vs the original on the registry vectors | the export's behaviour in the exe (uncalled in 1a) |
| G-PATH2 | the moved copy is the one installed | `run_verify_hook.py <key>` OK for each of the 8 RVAs above (E9 rel32 to the new export) | that the inline JMP lands on the new TU's export | n/a. A 0-arg call-through "bad argument count" failure is the known harness gap (memory `stale-demotion-and-0arg-verify-gap`); the E9 check alone then decides, and is reported as such |
| G-ORIG | the two owed columns | one `orig_rampwatch.py --cam --hz 60` capture WITH `--statediff-out`, scenario as `RESULT_LEG0.md`: G-NOPRESS repeated (`in0..in7` = 0 in phase 3 and phase 3 >= 600 frames); report `team_ea64` on the phase-4 sample (or the last 3 / first 5 samples) and which `FUN_0040dbd0` arm it selects with `submode_e9fc` = 10; report `c2194_v`/`c2198_v` (v=0..3) on the last phase-5 and first phase-6 samples; report records `0x0063c8d0+k*0x74` `+0x64` (int) and `+0x50` (float), k=0..3, before and after phase 4 | values per poll | the exact phase-4 frame (one frame long; a 60 Hz poll may miss it) |

**New registry key `vehicle_physics_state_reset_obs`.** The existing `vehicle_physics_state_reset`
key is `int_scalar`, i.e. it compares the RETURN value only (1 or 0), so its GREEN never saw the
~90 stores (memory `inherited-green-may-be-guard-only`). The new key uses
`scalars_to_scattered_globals`: windows `0x008815a0 + v*0xd04`, length `0xd04`, fill `0xAA`, for
v in {0,1,2,3,15}, `fold_ret`, tests `[0],[1],[2],[3],[15],[16],[0xffffffff]`. Every store of
`FUN_0046baa0` for slot v lands in `[0x008815a0+v*0xd04, 0x00882194+v*0xd04)`, inside its window
(lowest `0x008815a0` at `0x0046bab8`; highest the loop-1 `puVar2[1]` of its last pass,
`0x008820d0 + 15*12 + 4` = `0x00882190`). Prediction: GREEN; the slot-16 and
`0xffffffff` tests (unsigned guard `> 0xf`, `0x0046baa4`) return 0 and touch no window.

**`hud_slide_billboard_tick`** (`void_write_observe`, scenario race) is re-run because its body
changes shape (the tail moves into a called partial). Prediction GREEN, same as its C3.

## 5. Rules

- Any FAIL is reported as measured. G-CTRL or G-INERT FAIL stops the leg: the change is not inert.
- No re-run to fix a FAIL, except a harness void (process never readable, G-NOPRESS fail), which
  gets one re-run, reported.
- Own PIDs only. `MASHED_GF1` stays unset. `SLOT_PLAYER`/`SLOTSTATE_SEED` at their default-ON in
  every arm.
- C-levels: the seven moved rows stay C3 (file column repointed via `re-classify`), `0041d930`
  stays C3. No promotion above C3 from this lane.
