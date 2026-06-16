# AI driver controller — RE map + verbatim-port blueprint (WS-C C1, 2026-06-16)

Anchored to `original/MASHED.exe` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`
(verified this session on `original/MASHED.exe.unpatched`).

This is the **WS-C C1** deliverable: a single consolidated, port-ready spec for the
opponent-driver AI. It does not introduce new decompilation — the whole cluster is
already RE'd at **C2** (every function below has a per-function analysis note; several
leaves are already C3). This document gathers those notes into one call-tree + struct +
control-format + behavior-mode map so the **C2 verbatim port** (into `Ai/`) and the
**C3 wire+diff** can be executed without re-reading 25 scattered files.

Per NO-GUESSING: every field cites the RVA/analysis note it came from; unresolved items
are carried as `[U-####]` rather than invented.

---

## 0. Cluster root correction (read first)

The ROADMAP (`WS-C`) and `SESSION_VERIFICATION_AUDIT_2026-06-16.md` both name the AI
driver root as **"FUN_0040e480 family"**. That is a **mis-citation**: per `hooks.csv`,
`0x0040e480` = `CarSlotStateSet` — an 18-byte frontend double-deref setter
(`*(PTR_PTR_005f2770 + p1*4 + 0x34) = p2`), unrelated to driving. It is *called by* the
AI tick loop only as an entity-state poke (mark cars 1..3 alive), not as the controller.

The **real** opponent-AI driving cluster is the **`FUN_00418860` family** (per-frame AI
tick → per-vehicle decide/steer/throttle → synthetic input). Evidence: the path-following
session (`re/analysis/ai_path_following/SESSION_END.md`) and the `ai_update*` /
`bucket_ai_*` notes. This document supersedes the `0x0040e480` citation; ROADMAP + audit
references corrected alongside it.

---

## 1. Per-frame call tree

```
FUN_00418860  AI tick loop (per frame)                     [ai_path_following/0x00418860.md]
  guard DAT_00801ca0 > 3  (race-line spline count loaded)
  ├─ FUN_0040e480(1..3, 2)        mark AI cars alive (entity poke; NOT driving)
  ├─ FUN_004177b0()               PRE-TICK: race-angle table + difficulty rubber-banding
  │                               [promote_c2_ai_path/0x004177b0.md]
  └─ for v in 0..3 (alive AI vehicles):
       FUN_00418560(v, type)      per-vehicle AI step + control-output writer
                                  [ai_update/0x00418560.md]
         ├─ FUN_00417180(v)       spline-BANK switcher (race/inside/slow/cheat × idx 0..2)
         │                        [ai_update/0x00417180.md]
         ├─ select spline ptr by per-vehicle line-type/index (4 BSS arrays)
         ├─ dispatch by game mode DAT_007f0fd0:
         │    mode 4/9 → FUN_00416a30(spline, v, ctrl, 100.0)   [ai_update/0x00416a30.md]
         │    mode 8   → FUN_00417da0(...)                       [ai_update/0x00417da0.md]
         │    else     → FUN_00416250(spline, v, ctrl, 100.0)    PRIMARY control step
         │                                       [ai_update/0x00416250.md + bucket_ai_…]
         └─ FUN_00417640(v, ctrl) POST-STEP powerup-brake override [ai_update/0x00417640.md]

FUN_00416250  PRIMARY per-vehicle control step (2014 B):
  ├─ getters: FUN_0046d4a0 (own struct ptr), FUN_0046d6a0/d6d0/d510 (speed/rate/vel floats)
  ├─ FUN_00443440(spline, ownXZ, 10.0, &dist, 0)   spline progress + curvature
  ├─ FUN_004161e0(spline, &targetIdx, v)           seed target point  [→ FUN_00443dc0]
  ├─ TARGETING CHAIN (sets behavior mode local_48, each gated by FUN_00416060 LOS):
  │    FUN_00414570  catch car ahead            → mode 1
  │    FUN_00415880  ram from behind (+no-wall FUN_00415d00) → mode 2
  │    FUN_00414a70  closest-vehicle / brake    → mode 3, or early-return full-brake
  │    FUN_00414c30  obstacle avoidance         → mode 3 / mode 7
  │    FUN_004150e0  track-wall lateral recover → mode 9
  │    FUN_00414f00  powerup-seek (gate 0x21)   → mode 10
  │    FUN_00415020  frustration/catch-up       → mode 5  (mode-6 gameplay only)
  │    FUN_004148b0  leader-ranking timer / bomb early-return
  │    FUN_00415220  AI powerup ACTIVATION (fire weapon, writes ctrl+3) → mode 8
  ├─ commit mode to DAT_0089a52c[v*0x74]
  ├─ FUN_00415e20(v, tx, tz)  signed steering-angle error  [ai_update_d2/0x00415e20.md]
  └─ write ctrl bytes [0],[1] (steer), [4] (accel), [5] (brake) + per-mode overrides

FUN_00443dc0  spline lookahead target finder (nearest-point bsearch + 16-step walk + wall)
              [bucket_ai_00415d00_00452ea0/0x00443dc0.md]
```

`FUN_00416a30` (modes 4/9) and `FUN_00417da0` (mode 8) are **near-clones** of
`FUN_00416250` — the port should factor the shared body and parameterize the three
documented deltas (below), not write three copies.

---

## 2. Function inventory (all C2+; analysis notes exist)

| RVA | name | size B | conf | role | note |
|---|---|---|---|---|---|
| 0x00418860 | FUN_00418860 | 279 | C2 | per-frame AI tick loop; 4-vehicle iterator | ai_path_following/0x00418860.md |
| 0x004177b0 | FUN_004177b0 | — | C2 | pre-tick: race-angle table + difficulty rubber-banding | promote_c2_ai_path/0x004177b0.md |
| 0x00418560 | FUN_00418560 | 740 | C2 | per-vehicle step: bank select + dispatch + ctrl write | ai_update/0x00418560.md |
| 0x00417180 | FUN_00417180 | 485 | C2 | spline-bank switcher (type/idx cycle + timer + RNG) | ai_update/0x00417180.md |
| 0x004161e0 | FUN_004161e0 | 69 | C2 | spline target-point init (own XZ → FUN_00443dc0) | bucket_ai_00415d00_00452ea0/0x004161e0.md |
| 0x00416250 | FUN_00416250 | 2014 | C2 | **primary control step** (behavior tree + ctrl out) | ai_update/0x00416250.md + bucket_ai_… |
| 0x00416a30 | FUN_00416a30 | — | C2 | control step, modes 4/9 variant | ai_update/0x00416a30.md |
| 0x00417da0 | FUN_00417da0 | — | C2 | control step, mode 8 variant | ai_update/0x00417da0.md |
| 0x00417640 | FUN_00417640 | 236 | C2 | post-step powerup-brake override | ai_update/0x00417640.md |
| 0x00415e20 | FUN_00415e20 | 566 | C2 | signed steering-angle error (acos bearing vs heading) | ai_update_d2/0x00415e20.md |
| 0x00443440 | FUN_00443440 | — | C2 | spline progress fraction + curvature out | ai_update_d2/0x00443440.md |
| 0x00443dc0 | FUN_00443dc0 | — | C2 | spline lookahead target finder | bucket_ai_00415d00_00452ea0/0x00443dc0.md |
| 0x00416060 | FUN_00416060 | — | C2 | line-of-sight tile ray-march (0=blocked,1=clear) | ai_update_d2/0x00416060.md |
| 0x00415d00 | FUN_00415d00 | — | C2 | wall-ahead 2× velocity ray-march (suppresses mode 2) | ai_update_d2/0x00415d00.md |
| 0x00414570 | FUN_00414570 | — | C2 | targeting: catch car ahead (≤2.0 progress, angle-gate) | ai_update_d2/0x00414570.md |
| 0x00415880 | FUN_00415880 | — | C2 | targeting: ram from behind (latch, dot-align, FUN_0046cc10==4) | ai_update_d2/0x00415880.md |
| 0x00414a70 | FUN_00414a70 | — | C2 | targeting: closest-vehicle (1=chase, 2=brake) | ai_update_d2/0x00414a70.md |
| 0x00414c30 | FUN_00414c30 | — | C2 | targeting: obstacle avoidance (world objs; 1/2) | ai_update_d2/0x00414c30.md |
| 0x004150e0 | FUN_004150e0 | — | C2 | track-wall lateral-zone query (tile type 0/3) | ai_update_d2/0x004150e0.md |
| 0x00414f00 | FUN_00414f00 | — | C2 | powerup-seek targeting (range/flag gate) | ai_update_d2/0x00414f00.md |
| 0x004148b0 | FUN_004148b0 | — | C2 | leader-ranking timer (last-place accumulate; bomb path) | ai_update_d2/0x004148b0.md |
| 0x00415020 | FUN_00415020 | — | C2 | frustration timer (last-place; >72000 → mode 5) | ai_update_d2/0x00415020.md |
| 0x00415220 | FUN_00415220 | — | C2 | AI powerup activation (fire weapon → ctrl+3) | ai_update_d2/0x00415220.md |
| 0x00417cf0 | FUN_00417cf0 | — | C2 | angle-gated targeting (mode-8 variant of FUN_004148b0) | ai_update_d2/0x00417cf0.md |
| 0x00408af0 | AiVehicleFieldPtrGet | — | C3 | mode-2 vector ptr getter | ai_update_d2/0x00408af0.md |
| 0x0046d4a0 | PtrCompute881ec8 | — | C3 | own-vehicle struct ptr (per-car, stride 0xd04) | ai_update_d2/0x0046d4a0.md |
| 0x0046d6a0 | VehTbl8820acGet1 | — | C3 | per-vehicle float (speed-like) | ai_update_d2/0x0046d6a0.md |
| 0x0046d6d0 | VehTbl881f84Get1 | — | C3 | per-vehicle float (rate-like) | ai_update_d2/0x0046d6d0.md |
| 0x0046d510 | FUN_0046d510 | — | C2 | per-vehicle velocity float3 | (cited by 00415e20/00416250) |
| 0x00443080 | AiTargetEnableGet | — | C3 | AI-targeting enable flag | ai_update_d2/0x00443080.md |
| 0x00452ea0 | Table88ff50Get | — | C3 | per-vehicle powerup predicate | ai_update_d2/0x00452ea0.md |
| 0x00414030 | AiSplineBankTimerReset | — | C3 | bank-timer reset (DAT_008032d4[v*5]=1000) | bucket_ai_00407a40_00415880/0x00414030.md |
| 0x00413fe0 | FUN_00413fe0 | — | C3 | AI state reset (zero 12 DWORDs/car @0x89a4f0) | bucket_ai_00407a40_00415880/0x00413fe0.md |
| 0x00407a40 | Table8a9640Get | — | C3 | per-vehicle data getter (stride 0x30c) | ai_update_d2/0x00407a40.md |
| 0x00472650 | FUN_00472650 | — | — | RNG float [0,1) (also used by physics steer) | ai_update_d2/0x00472650.md |

Helper callees of the targeting functions still living only as DEFERRED/plate (resolve
during C2): `FUN_00414300`, `FUN_0046cc10`, `FUN_00484c70`, `FUN_00443d10`,
`FUN_00452160`, `FUN_00452eb0`, `FUN_0046d570`, `FUN_004c3ac0`/`FUN_004c39b0` (RW math —
shared with WS-A2), `FUN_004a3384` (acos), `FUN_004a2c48`, `FUN_00407a20`,
`FUN_0046dd80/dd90`, `FUN_00426c00` (track index).

---

## 3. Per-vehicle AI state (BSS arrays — addresses + strides)

The AI keeps several **parallel per-vehicle arrays**, NOT one struct. Two different
strides are in play; both index by vehicle slot `v ∈ 0..3`. The "stride 0x1d DWORDs"
(= 0x74 bytes) arrays and the "stride 0x74 bytes" arrays alias the same 0x74-byte record
(0x1d DWORDs == 0x74 bytes). The "stride 0x14" arrays are a separate float-history block.

### 3a. Path-following / behavior record — base `DAT_0089a4cc`, stride **0x74 B (0x1d DWORDs)**

| field addr | offset from 0x89a4cc | role | source |
|---|---|---|---|
| DAT_0089a4c0 | −0x0c | slow-line event scratch (zeroed on table-A trigger) | 004177b0 corr.3 |
| DAT_0089a4c4 | −0x08 | per-vehicle scratch DWORD (zeroed pre-tick) | 004177b0 corr.1 |
| DAT_0089a4c8 | −0x04 | per-vehicle scratch DWORD / leader-rank accumulate (FUN_004148b0) | 004177b0, 004148b0 |
| **DAT_0089a4cc** | +0x00 | **AI line TYPE** 0=race 1=inside 2=slow 3=cheat | 00418560, 00417180 |
| DAT_0089a4d0 | +0x04 | **spline INDEX** 0..2 within type | 00418560, 00417180 |
| DAT_0089a4dc | +0x10 | ram-target latch (FUN_00415880) | 00415880 |
| DAT_0089a4e0 | +0x14 | ram-target latch 2 | 00415880 |
| DAT_0089a4e4 | +0x18 | frustration timer (FUN_00415020; >72000 → mode 5) | 00415020 |
| DAT_0089a4e8 | +0x1c | mode-5 speed-threshold float | 00416250 |
| DAT_0089a4ec | +0x20 | mode flag (set 1 or 2 on steering-angle crossing) | 00416250 |
| DAT_0089a4f0 | +0x24 | timestamp (= DAT_007f0ff4 at crossing) | 00416250 |
| DAT_0089a4f4 | +0x28 | data (= FUN_004a2c48() at crossing) | 00416250 |
| DAT_0089a4f8 | +0x2c | mode-5 startup countdown (negative/positive thresholds) | 00418560 |
| DAT_0089a4fc | +0x30 | input-override countdown (replays stored ctrl bytes) | 00418560 |
| DAT_0089a500 | +0x34 | **switch-REQUEST flag** (≠0 triggers bank change) | 00417180 |
| DAT_0089a504 | +0x38 | switch timer (counts to 9000, +DAT_007f1008/frame) | 00417180 |
| DAT_0089a51c/520/524 | +0x50/54/58 | zeroed when DAT_0088fc88==0 (mode-8 reset) | 00416250 |
| DAT_0089a52c | +0x60 | **behavior MODE** (0..10), committed each step | 00416250, 00417640 |

> NOTE on indexing idiom: the decomp writes these as `(&DAT_0089a4cc)[v * 0x1d]` (DWORD
> array, ×0x1d) for the type/index fields and as `(&DAT_0089a500)[v * 0x74]` (byte base,
> ×0x74) for the flag/timer fields. Both resolve to `base + v*0x74`. Port as one
> `struct AiVehState { … }  s_aiState[4];` of size 0x74, indexed by v.

### 3b. Angle/speed history — base `DAT_008032d4`, stride **0x14 B (5 DWORDs)**

| field | offset | role | source |
|---|---|---|---|
| DAT_008032d4 | +0x00 | bank-timer (FUN_00414030 sets 1000) | 00414030 |
| DAT_008032d8 | +0x04 | steering-angle history float (reset 360.0 = 0x43b40000) | 00416250 |
| DAT_008032dc | +0x08 | steering float; compared vs _DAT_005cc320 | 00416250 |
| DAT_008032e0 | +0x0c | speed history (local_38 from FUN_0046d6a0; rate-of-change brake) | 00416250 |

### 3c. Powerup/mode-8 gate — `DAT_0088fc88`, stride **0x2d DWORDs**

Per-vehicle powerup-type holder. `FUN_00415220` switches on it (types
7/9/10/0xb-0xc/0x10-0x13) to fire the held weapon; `==0` zeroes the 0x51c/520/524 fields
and skips the mode-8 path. `[U-0415]` semantic of the 0x2d stride / full field layout.

### 3d. Race-angle / rubber-banding globals (set by FUN_004177b0)

`DAT_0089a880` race-angle array; `_DAT_0089a870..a87c` four candidate slots (sentinel =
`_DAT_005cc33c`); `DAT_0089a360` target-distance float; `DAT_0089a368` difficulty/debug
flag (0/2=race lines, 1=slow lines); `DAT_0089a370/374` slow-line time-ref + category.

---

## 4. AI-line spline data (the "race lines" the AI follows)

Loaded by `FUN_004235b0` from `Common/ai.piz` member `"AI%d.AI"` (track index from
`FUN_00426c00`); record magic `0x13269902`; total `0x11884` B into `DAT_007f1a9c`. Four
line-type arrays at fixed BSS addresses (each: ≤3 splines, ≤64 XZ float2 points, count at
+0x200, stride **0x204**):

| line type | base | count field | meaning |
|---|---|---|---|
| 0 race | 0x00801aa0 | 0x00801ca0 | fast racing line |
| 1 inside | 0x008020ac | 0x008022ac | tighter inside line |
| 2 slow | 0x008026b8 | 0x008028b8 | cautious line |
| 3 cheat | 0x00802cc4 | 0x00802ec4 | shortcut line |

Guard `DAT_00801ca0 > 3` (race-line count) gates the whole tick loop. A bank is valid
only if its count field ≥ 4; `FUN_00417180` cycles type→index, falling back through the
chain on a too-short bank. There is a separate **128×128 tile grid** at `DAT_007f1a9c`
(+8×8 sub-cell at `DAT_007f9a9c`) used by the wall/LOS queries (`FUN_004150e0`,
`FUN_00415d00`, `FUN_00416060`, `FUN_00443d10`); tile type 0 or 3 = wall/boundary.

`.AI` format detail: `re/analysis/ai_path_following/SESSION_END.md`. The standalone does
**not** yet parse `.AI` (the scaffold uses AI*.BSP gate centers instead, §7) — WS-C C2
must add an `.AI` parser (cross-ref `re/prior_art/MashedFileExtractor`), parallel to the
existing track-geometry parsers.

---

## 5. Control output — the synthetic input block (CRITICAL for the port)

`FUN_00418560` computes `ctrl = 0x007f1038 + outSlot * 0x4c`, where
`outSlot = *(int*)(0x007f1a14 + v*0x10)`, then zeroes ctrl bytes [0],[1],[4],[5],[6],[7]
and the step functions fill them.

**`0x007f1038` is the per-player CURRENT-FRAME INPUT block** (stride 0x4c B = 0x13 DWORDs)
— the *same* buffer the DirectInput reader populates for the human:
- `FUN_00496530` snapshots last frame and zeroes `DAT_007f1038 + player*0x13` each frame
  (`bucket_input_dinput_0047b860_0049b300/00496530.md`, `cluster_0049_first_pass/00496530.md`).
- `FUN_004030d0` memsets the whole 0x7f1038 block on reset.
- The vehicle physics input integrator `FUN_00470670` consumes `param_3 = ` this block
  (`vehicle_physics_cluster.md`).

So the AI is a **virtual gamepad**: it writes the same byte block the human controller
writes, and that flows through the identical input→physics path. This is why C3 ("diff AI
positions") is coupled to the physics consumer (§8).

### Byte map — RESOLVED 2026-06-16 (see `ai_ctrl_byte_map_RESOLVED_2026-06-16.md`)

| byte | role | confirmed by |
|---|---|---|
| [0] | steer cmd sign A | writer FUN_00416250 ⇄ steering source FUN_00415e20 |
| [1] | steer cmd sign B (+ mode-2 ram) | writer FUN_00416250 |
| [3] | fire / powerup button | writer FUN_00415220 |
| [4] | **accel** (0/0x40/0xff) | writer FUN_00416250/00418560 + override FUN_00417640 ⇄ consumer **FUN_00467650** (`*(param_4+4)` = forward drive torque) |
| [5] | **brake/reverse** (0/0x40/0xff) | writer FUN_00416250 + override FUN_00417640 ⇄ consumer **FUN_00467650** (`*(param_4+5)`, negative force) + FUN_00470670 accel-curve gate |
| [6],[7] | zeroed scratch | FUN_00418560 |

The AI-side plate was CORRECT; the old "physics-side plate" (`[0]=accel`) was a misread
of FUN_00470670's *secondary* longitudinal-impulse channel — the *primary* throttle/brake
consumer is FUN_00467650, which reads [4]/[5]. **Accel and steer are NOT swapped.**
`FUN_004a2c48` = MSVC `__ftol` (reads x87 ST0). U-0407 closed; U-0413 downgraded to a
non-blocking downstream-physics label question. The verbatim port reproduces the writes
by byte index, so byte semantics do not affect port correctness.

Observed value vocabulary: `0xff` = full, `0x40` = partial (mode-7 throttle, mode-5
brake), `0x00` = off. The final gate forces `[4]=[5]=0` when game mode ∉ {5,6,9,10,11}.

---

## 6. Behavior modes (semantics now derivable from the targeting helpers)

`FUN_00416250` selects an integer mode `local_48 ∈ {0..10}` via the priority chain, each
candidate gated by LOS `FUN_00416060`. Semantics from each helper's mechanical note:

| mode | trigger helper | meaning | ctrl override |
|---|---|---|---|
| 0 | (no target) | follow spline lookahead only | default accel 0xff; steer from FUN_00415e20 |
| 1 | FUN_00414570 | catch car ahead (≤2.0 spline progress, angle-gated) | steer toward, full accel |
| 2 | FUN_00415880 (+ FUN_00415d00==0 no-wall) | ram car from behind (velocity-dot aligned) | dot/cross steer toward target vel |
| 3 | FUN_00414a70==1 / FUN_00414c30==1 | chase closest vehicle / avoid obstacle | steer toward |
| 5 | FUN_00415020 (last-place > 72000) | frustration / catch-up (gameplay mode 6) | conditional accel 0; brake 0x40; blink steer 0xff on DAT_007f0ff8/0x3c & 0x20 |
| 7 | FUN_00414c30==2 | obstacle hard-turn | accel 0x40 (partial) |
| 8 | FUN_00415220 (DAT_0088fc88≠0) | activate held powerup (fire weapon, ctrl+3) | weapon byte + steer |
| 9 | FUN_004150e0==1 | track-wall lateral recovery | local_3c-based brake |
| 10 | FUN_00414f00 (FUN_00426c00()==0x21) | seek powerup pickup | steer toward pickup pos |
| — | FUN_00414a70==2 | (no mode) target inactive+close → **immediate full brake** ([4]=0,[5]=0xff) | early return |

Mode is committed to `DAT_0089a52c[v*0x74]`; `FUN_00417640` post-step can override to full
brake when `FUN_00426c00()==0x21` and the per-vehicle powerup predicate fires.

Steering (`FUN_00415e20`): signed angular error between bearing-to-target and current
heading, both via `acos(normalize(delta)·X)` with z-sign correction, scaled by
`_DAT_005cc970`, wrapped to `[0, 2π=_DAT_005ccac4]`. The error magnitude drives which
`param_3[0/1]` steer band and accel/brake band is selected (float10 thresholds
`_DAT_005ccd6c / 005cd09c / 005cc72c / 005cd0e0`).

---

## 7. Difficulty / rubber-banding (FUN_004177b0)

Per-frame pre-tick, before the per-vehicle loop. Drives catch-up so the pack stays close
(Mashed's signature). Documented deltas (`promote_c2_ai_path/0x004177b0.md`):
- Builds `DAT_0089a880` race-angle table + fills 4 candidate slots (sentinel `_DAT_005cc33c`).
- **Mode 9** speed multiplier by race-angle band: ×0.95 / ×0.9 / ×0.25 / ×1.15
  (thresholds `_DAT_005cc348 / 005cc950 / 005cc9a4 / 005cc574`).
- **Mode 4** multiplier = rubber-band formula in `_DAT_005cc55c − DAT_0089a360` (target
  distance) × `_DAT_005cc56c` × {`_DAT_005cc8f4`,`_DAT_005ccac8`} + offsets.
- Type-2 (AI) vehicles get speed doubled on the powerup-alive path (`FUN_0046dd90(v, 2×speed)`).
- Slow-line toggle every 180000 ticks via probability tables `DAT_005f30a0 / 005f3180`
  → sets `DAT_0089a368` to 1 (slow) or 2.

---

## 8. The scaffold being replaced (C3 target)

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.{h,cpp}` — `struct AiCar { pos[3], yaw,
target, speed, cur_speed, lane, spin, slow }` + the "AI v2" gate-ribbon follower in
`UpdateCar` (≈ TrackRenderer.cpp:1176–1234):
- Follows ordered **AI\*.BSP gate centers** (`gates_`), NOT the `.AI` splines, with a
  per-car lateral `lane` offset.
- Aims at gate center + lane, advances `target` when within 3.0 units.
- Brakes for corner sharpness (dot of consecutive ribbon segments).
- Velocity-shaped `cur_speed` toward `speed*brake`; integrates `pos` by `cos/sin(yaw)`.
- Power-up reactions: `spin` (spun out) / `slow` (shocked) timers.

This is an **invented** lane-follower — no spline banks, no behavior modes, no LOS/ram/
powerup-targeting, no synthetic-input/physics path. It is NOT diff-able against any
original RVA. C3 replaces it with the ported cluster feeding the (eventual) ported physics.

---

## 9. C2 verbatim-port plan (into `Ai/`)

Target files (gta-reversed one-class-per-file, RVA-commented): `Ai/AiController.cpp`
(tick loop + per-vehicle step + dispatch), `Ai/AiControlStep.cpp` (FUN_00416250 + 4/9 + 8
variants, shared body), `Ai/AiTargeting.cpp` (the ~10 targeting/LOS helpers),
`Ai/AiSpline.cpp` (FUN_00443440/00443dc0/004161e0 + `.AI` parser), `Ai/AiState.h`
(the 0x74 / 0x14 / 0x2d records as structs). Suggested order (leaves first, F-DoD "no
stubs"):

1. **Close the byte→axis mapping (§5)** — blocker for any meaningful diff. Decompile
   FUN_00496530 (input write) + FUN_00470670 (physics read); resolve U-0407/U-0413.
2. **`.AI` parser** (data-verified, no Ghidra; cross-ref MashedFileExtractor) — produces
   the 4 line-type arrays + tile grid the controller indexes.
3. **RW math leaves** — reuse WS-A2 ports (FUN_004c3ac0 length, 004c39b0 normalize) +
   FUN_004a3384 (acos), FUN_00472650 (RNG). Shared with physics; coordinate with WS-A.
4. **Spline kernel** — FUN_00443dc0, FUN_00443440, FUN_004161e0, FUN_00415e20 (steering).
5. **Targeting/LOS helpers** — FUN_00416060, 00415d00, 00414570, 00415880, 00414a70,
   00414c30, 004150e0, 00414f00, 004148b0, 00415020, 00415220, 00417cf0 (+ their deferred
   callees FUN_00414300/0046cc10/00484c70/00443d10/00452160/00452eb0/0046d570).
6. **Control step** — FUN_00416250, then factor 00416a30 (no modes 5/10, no bomb path, no
   0x89a52c commit) and 00417da0 (FUN_00417cf0 swaps FUN_004148b0; no DAT_0089a368 debug).
7. **Bank switcher + step dispatcher + tick loop** — FUN_00417180, FUN_00418560,
   FUN_00417640, FUN_00418860.
8. **Tuning constants** — harvest every `DAT_005ccXXX` the cluster reads (these are
   zeroed in the image-padded standalone .bss; see [[standalone-exe-phase-h]]) from the
   original `.rdata`. Build an `Ai/ai_tuning.inc` table, RVA-cited. Parallels WS-A7.

`FUN_00418860` already references `FUN_0040e480` (CarSlotStateSet, C3), `FUN_0040e470`,
`FUN_0046c7b0`, `FUN_0042f6a0`, `FUN_00443080` — all C2/C3, available.

---

## 10. C3 verification plan + its real blocker

Task: "Replace gate-ribbon scaffold; diff AI positions over N frames (dep C2, ideally
A8)." The dependency on **A8 is hard, not "ideal"**: the AI's only output is the synthetic
input block (§5); the cars *move* only when a consumer integrates it. Two diff strategies:

- **(preferred, after WS-A8)** Wire the ported AI → ported physics (FUN_00470670 cluster)
  → diff per-frame car position/velocity vs an original race Frida trace on matched inputs
  (the scenario-attach lane, `re/analysis/scenario_attach`). This is the true C4 (positions
  identical frame-by-frame). Blocked on WS-A/A8.
- **(interim, isolatable now)** `diff-original` the AI sub-functions directly: force-call
  the ported leaves (steering FUN_00415e20, LOS FUN_00416060, spline FUN_00443dc0,
  targeting helpers) against the original via `run_diff.py`, comparing returned
  angles/indices/flags. This gives per-function C3/C4 **without** physics — and is the
  honest acceptance for the controller's *decisions* even before the cars are physically
  driven by them. Add these to `hooks_registry.py`.

So: WS-C C3 splits into **C3a** (per-function decision diffs — runnable as each C2 leaf
lands, via the boot-original lane / scenario-attach race state) and **C3b** (whole-loop
position diff — gated on WS-A8). Record both in `SESSION_VERIFICATION_AUDIT`.

---

## 11. Open uncertainties (carried; resolve during C2)

| U-ID | item | resolution path |
|---|---|---|
| U-0407 / U-0413 | ctrl byte→axis mapping (steer/accel/brake/fire) | decompile FUN_00496530 + FUN_00470670 (§5) |
| U-0414 | FUN_0046d6a0/d6d0 float meaning (speed vs angular rate) | decompile getters; correlate with physics struct +0x9e4/+0x9b0 |
| U-0415 | DAT_0088fc88 0x2d stride / field layout | decompile FUN_00415220 powerup-holder writer |
| U-0416 | behavior-mode names | §6 derives semantics; names cosmetic |
| U-0418/0419/0420 | FUN_00414030 / FUN_00472650 / timer-9000 units | C3 leaves already cover 00414030; RNG = FUN_00472650 |
| U-0422..0426 | FUN_00417640 powerup-brake callees | decompile FUN_00452160/00452ea0/00452eb0/0046d570/004c3ac0 |
| U-1427 | FUN_004150e0 tile-grid semantics | resolved by `.AI` tile-grid parse (§4) |
| U-3612..3619 | tick-loop game-mode codes + rubber-band tables | decompile FUN_0042f6a0; calibrate DAT_005f30a0/005f3180 |
| U-7587..7589 | param_3 mapping / getter floats / DAT_0088fc88 (dup of above) | same as U-0407/0414/0415 |

None block the **C1 deliverable** (this map); all are flagged for C2.

---

## 12. Status

- **C1 (this doc): DONE** — cluster mapped end-to-end; every function C2+ with a note;
  call tree, state, control format, modes, rubber-banding, scaffold, port+verify plan
  recorded. No C-level changes (consolidation only; no diffs run → no re-classify).
- **C2: NOT STARTED** — blocked on the byte-map resolution (§5 step 1) + `.AI` parser +
  RW-math leaves (WS-A2). Multi-session.
- **C3a: ready after first C2 leaves** (per-function decision diffs).
- **C3b: blocked on WS-A8** (whole-loop position diff needs the ported physics consumer).
