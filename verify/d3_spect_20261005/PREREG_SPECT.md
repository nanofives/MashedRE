# PRE-REGISTRATION — port `FUN_00442a60` (`Spectator::ComputeDistances`) to unblock U-9186's 64-call branch

**Committed UNRUN. 2026-10-05.** D3 criterion (b)/(e), U-9186 + U-9187.

Version anchor verified before arming: `original/MASHED.exe.unpatched` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, `original/launch.exe`
`01506209E42C79A4E5BEDB43DCE9FB953F0CA628B26AF9FCD49EE2522DF78DA2`. Both equal `CLAUDE.md`'s anchor.

## 0. Why this leg leads with a kill test instead of with code

Two findings were filed and retracted earlier today, both because a number was scored over a
population the claim was not about. The chosen target here rests on a chain of inferences, and
**one link is unverified and cheap to test**, so it is tested first and the gate is written so it can
**kill the whole path**.

## 1. What is ESTABLISHED, with citations — not re-derived here

- **U-9186 holds the carrier** of the AI over-speed: **149 of 660** window calls where the original
  lifts or brakes and the port holds full throttle, **0 of 149 unexplained**, split cell-identical
  across `o_t1`/`o_t2`/`o_t3` (`verify/d3_elim_20261003/RESULT_STEP2.md`).
- **Per-car split** (`RESULT_STEP2.md`): the `FUN_004148b0` + LOS branch is **64 calls, 31 of them
  car 1's**; mode 7 via `FUN_00414c30` is 49 (car 1's largest, structurally blocked on the
  world-object list); `FUN_00414a70` is 36 (cars 2 and 3 only).
- **U-9187** measured wiring `FUN_004148b0` as **inert AND unsafe** on 2026-10-03, with
  `Prog[0..3]` **0.0 in all 1089 port samples** against non-zero on **464 / 125 / 486 / 461 of 512**
  original calls (max 9.988 / 3.790 / 4.466 / 2.750).
- **`Prog(i)` is a CALL to `FUN_00442cc0`**, not a direct global read (`AiLeaderTimer.cpp:72`), and
  `FUN_00442cc0` returns `*(float*)(&DAT_008989b0 + i*4)` for `i < 4`, else `DAT_005d757c` = 0.0
  (`re/analysis/ai_update_d3/0x00442cc0.md`).
- **`FUN_00442a60` is the ONLY writer of `0x008989b0`.** Measured this session by an authoritative
  Ghidra dataref sweep (`re/tools/decomp_pc.py 0x008989b0 --datarefs`, read-only pool slot
  `Mashed_pool0`; Ghidra MCP unreachable, `decomp_pc.py` is the sanctioned no-MCP path): **2 write
  sites, both inside `FUN_00442a60`** — `0x00442a65` `MOV [0x008989b0],EAX` and `0x00442c53`
  `FSTP float ptr [EAX*0x4 + 0x8989b0]`. Readers are three indexed getters, `FUN_00442c80`
  (`0x00442ca2`), `FUN_00442cc0` (`0x00442cc9`) and `FUN_00442ce0` (`0x00442d0f`).
- **The function's shape** is already C2-plated (`re/analysis/bucket_util_0042f7a0_004764e0/00442a60.md`,
  530 bytes, fully read): zero the four floats, `FUN_0040e180` fills two player ids, metrics via
  `FUN_00408ad0` adjusted by `_DAT_005cc568` against `_DAT_005cc730`/`_DAT_005ccd6c`, `FUN_0046cbb0`
  for both, chosen ids to `_DAT_008989a8`/`DAT_008989c8`, then gated on
  `FUN_0040e370(id) != 0 && FUN_0046c7b0(id) == 1`: position via `FUN_0046d4a0`, and for `i` in
  `[0,4)` the planar delta magnitude `FUN_004c3ac0` scaled by `_DAT_005cc9bc` into
  `*(&DAT_008989b0 + i*4)`.
- **All seven depth-1 callees already have bodies**, so this is **no new callee reversing**:

  | callee | confidence | status | file |
  |---|---|---|---|
  | `0x004c3ac0` `Vec3Magnitude` | C4 | verified | `Math/Vec3.cpp` |
  | `0x0046cbb0` `CarStatePairGet` | C4 | impl | `Util/VehicleState.cpp` |
  | `0x0046c7b0` `VehicleSlotGetter` | C4 | impl | `Vehicle/VehicleState.cpp` |
  | `0x0040e370` | C3 | mapped | `Util/UtilLeaves.cpp` |
  | `0x0046d4a0` `PtrCompute881ec8` | C3 | impl | `Util/PromoLoop_round58.cpp` |
  | `0x0040e180` `MostSeparatedPair` | C2 | verified | `Race/CameraClusterHooks.cpp` |
  | `0x00408ad0` `RaceScoreFloatGetBySlot` | C2 | impl | `Frontend/SmallLeaves_t1.cpp` |

## 2. THE UNVERIFIED LINK, stated plainly

**`FUN_00442a60` has exactly ONE caller, and the only path to it is a post-race camera.** Measured
this session (`decomp_pc.py --callers`, same read-only slot):

```
FUN_0040d470  ->  FUN_00448220  ->  FUN_00446520  ->  FUN_00442a60
                  Frontend::       "race-result camera      the ONLY writer
                  PostRaceResult-   state-machine; 3        of 0x008989b0
                  Camera            branches: DM-spectator
                                    (mode5)/result-cam/..."
```

`0x00448220` is `Frontend::PostRaceResultCamera` (C2 `mapped`, `frontend`) and `0x00446520` is the
race-result camera state machine (C2 `mapped`, `util`).

**So it is NOT established that the producer runs during a race at all.** U-9187's non-zero `Prog`
readings are consistent with at least three different worlds, and they imply different work:

| world | what the array does during the driving phase | what the fix actually is |
|---|---|---|
| **LIVE** | non-zero **and changing** as the cars move | port `FUN_00442a60` + reach it — this leg's premise holds |
| **FROZEN** | non-zero but **constant** | the values are written once outside the driving phase and persist; porting the producer alone does **not** make it live, and the scope becomes `FUN_00446520`/`FUN_00448220`'s race-phase branch |
| **DEAD** | **zero** throughout | `Prog`'s non-zero readings did not come from this array during the window; U-9187's premise is mis-scoped and this path is **abandoned** |

## 3. KA-SPECT — the kill test. No Interceptor, and it needs no new hook.

**Why no hook is needed, and this is the leg's one real insight:** because `FUN_00442a60` is the
**only** writer of `0x008989b0` (§1, measured), **the array's value history is a complete proxy for
the producer's call history.** A `--peek` read answers the question with a plain `Memory` read.

**Instrument.** `re/frida/scenario_launch.py --peek`, which is explicitly *"No Interceptor, no hook,
no write"* and therefore exempt from the hot-path rule (`scenario_launch.py:2338-2343`). The
`--statediff-out` capture in the same run supplies car 1's position, which defines the driving phase.

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_spect_20261005/s1.msd \
    --statediff-car 1 --statediff-aistep \
    --peek 008989b0:f,008989b4:f,008989b8:f,008989bc:f --hold 60
```

Unchanged from the committed (b)/(e) original recipe except for the added `--peek`. One original-side
run; its PID is the only process this leg may kill, and no blanket kill by name is permitted.

**THE DENOMINATOR, spelled out and checked against the population the claim is about** — this is the
clause that was got wrong twice today. Let `S_all` be every peek sample in the run and `S_drive` the
subset taken while car 1's position is changing (the driving phase; on the committed captures that is
**222 of 3622** frames, frame_idx 845..1064 plus the spawn settle). **Every count below is scored over
`S_drive`, and both the `S_drive` and `S_all` figures are printed side by side.** A figure over
`S_all` decides nothing here, because car 1 is eliminated partway through every one of these captures
and the post-elimination frames are ~70 % of the file.

**Gates, each with its threshold and its denominator on the same line:**

- **LIVE — the premise holds, and only this licenses §4.** At least one of the four slots is non-zero
  on **>= 0.90 × |S_drive|** samples, **AND** the 4-tuple takes **>= 10 distinct values** across
  `S_drive`. Both counts printed with `|S_drive|`.
- **FROZEN — the premise fails and the scope changes.** Non-zero on **>= 0.90 × |S_drive|** samples
  **AND** the 4-tuple takes **<= 2 distinct values** across `S_drive`. The second clause is what
  separates this from LIVE: the stored quantity is a distance between moving cars, so a recomputing
  producer **must** produce a changing value while the cars move.
- **DEAD — the path is abandoned.** All four slots are `0.0` on **>= 0.90 × |S_drive|** samples.
- Anything else: **INCONCLUSIVE**, with the full distinct-value histogram reported and **no port
  started**.

**Registered limitation, so a PASS is not over-read:** `--peek` samples periodically and cannot
distinguish "never called" from "called and the value coincidentally re-sampled identically". The
distinct-value clause is what makes that immaterial — a live recompute over moving cars cannot hold
`<= 2` distinct tuples across the driving phase.

**Registered prediction, recorded before running so it can be wrong:** I expect **FROZEN or DEAD**,
because the only call path is a post-race camera. If it is LIVE, my reading of the call chain is
wrong and I will say so.

## 4. G-PORT — runs ONLY if KA-SPECT returns LIVE

Port `FUN_00442a60` to `mashedmod/src/mashed_re/Race/SpectatorDistances.cpp`, one file for one
original function, RVA-cited per line, registered through `InjectHooks` with `RH_ScopedInstall` and
runtime-toggleable, per the hook-author conventions.

**Acceptance is the project's C3 gate, not "it compiles and does not crash":**

- a `hooks_registry.py` entry (the single source of truth for test vectors — **no new one-off `.js` or
  `.py` harness**), then
- **path1** `py -3.12 re/frida/run_diff.py <name>` bit-identical A/B against the original, and
- **path2** `py -3.12 re/frida/run_verify_hook.py <name>` to prove the inline-JMP actually installed,
  because **path1 GREEN does not prove install** (U-9065).

**Promotion ceiling, registered now:** synthetic A/B with the hook bypassed is **C3 at best**. A C4
claim needs a canonical-scenario run with the hook live, and **no C4 is claimed by this leg.**

## 5. G-NOREG — the default build must not move

(e) **PASSES 3/3 today** and the speed-reducing option is already **rejected by a recorded user
decision** (`ROADMAP.md:2400`, commit `e0b35ff9`: KEEP the seed). `MASHED_NO_START_BOOST` is the
precedent: it cut the window excess to +31/+21/+15 % and **regressed (e) 6 of 6 by -46 % to -90 %**.

So, measured on this build's own baseline (`verify/d3_arm_20261005/BASELINE_DISTANCE.md`):

- (e)'s two **gated** stats must be unchanged on all three cars: `launch` **1426.4 / 2053.0 / 2055.2**
  and `ft_median_m0` **2550.6 / 2053.0 / 2278.2**, scored by `ai_speed_env.py --check` over the
  **same 220-call window**. Any movement in either, on any car, and the port is reverted, not argued.
- (b) must not regress past **13 of 30** failing bands (`ai_ctrl_window.py --check`).
- `git diff --stat` and `git status --porcelain` on `re/tools/ai_ctrl_window.py` and
  `re/tools/ai_speed_env.py` must both be **empty** — the bands are not to be moved, and this is
  proven rather than asserted, per every prior session.

## 6. What a success here does and does NOT license

**It does not land the 64-call branch, and must not be reported as doing so.** U-9187 names four
missing inputs; this leg addresses **one**. Still owed afterwards: exe-side bodies for `0x0040e470`,
`0x00442cc0` and `0x0046d4a0` (all C3 `impl` with an **empty** `exe_file`, so the three TUs are in
`asi_sources.rsp` only), and the two upstream disagreements that decide whether the 64 calls are
**reproduced** or merely made **reachable** — `idx364` (original **-1** vs port **0**) and `bias374`
(**0** vs **{0,1,2,3}**).

**It does not close (b).** The 2026-10-02 counterfactual matrix had **no arm passing (b) on any car**.

**It does not touch U-9191.** Car 1's heading residual is stable across the whole speed-tolerance
ladder (0.790 / 0.8918 / 0.9764 deg at 1/5/10 % matching) and its `c0_distinct` is bound by the `err`
**envelope** — port (1.0, 4.23 deg] against the original's (1.0, 86.56 deg] — which no speed change
manufactures. Speed and heading are mechanically coupled by `m = err * speed * 0.0030034`
(`0x00416648`/`0x00416656`) but they are **not the same defect**.

**Do NOT seed `0x008989b0`.** `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203`'s rule applies
unchanged: *"seeding the globals would not be a port."* If KA-SPECT returns FROZEN, the answer is to
reverse the real call-site context, not to write plausible numbers into the array.

## 7. Process

Frida is used for **one** original-side run, with **no Interceptor at all** (`--peek` only). No entry
hooks are installed by KA-SPECT. The port's own verification in §4 uses the standard registry path.
PID hygiene: track the spawned PID, kill only that, never by name.
