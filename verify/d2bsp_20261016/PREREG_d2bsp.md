# PREREG d2bsp: port the BSP sphere walk FUN_00538c80 + the outer chunk loop of FUN_00470c70

Branch `work/d2bsp`, base `9420dde2` (race/first-frame-parity HEAD). Model claude-opus-5-5 (account2).
Written 2026-10-16 before any measuring run. Closes the two remaining port-only items of DEFERRED D-11071.
Sibling `d2engage` (A6a drive gate, Training wall trap, ReassertContacts cache) runs in parallel: every change
here is knob-gated.

## 0. Facts from the bytes (established before this file, decomp_pc / capstone on `MASHED.exe.unpatched`)

- **Call site** `0x0046f7d3..0x0046f822` in `FUN_0046f6c0`: sphere `{ring +0x30, +0x34, +0x38, r = rec +0x4a4}`,
  type 3 at +0x18; `DAT_0088e60c = 0`; `DAT_0088e654 = EBX`; `FUN_00538c80([EBX+8], &sphere, FUN_00468b80,
  0x00828320)` at `0x0046f817`, then `FUN_0046cc40(EDI, 0x00828320)` at `0x0046f822` (cdecl, vehicle first).
- **`FUN_00538c80`** is a 5-way dispatcher on `isect+0x18` (jump table `0x00538d4c`); case 3 calls
  `FUN_00539ec0(world, isect, isect, cb, data)` (`0x00538ccc`), cdecl.
- **`FUN_00539ec0`** (2033 bytes): plane-sector descent from `world+0x1c` (64-deep stack, left first; go right
  when `rightValue <= sup[type]`, also left when `inf[type] <= leftValue`), then per world sector the collision
  tree at `sector + [0x007dc5b4]`: sphere-vs-tree-box squared distance, a 6-bit "box crosses the inner box"
  mask (inner half-size = r * `_DAT_005e4578` = 0x3f11eb85 = 0.57), split descent; a node whose mask is 0 emits
  its whole triangle range with a direct normal (`FUN_004c3b90` LUT inv-sqrt) and distance `(c - v0).n`, any
  other leaf runs `FUN_00547450` (sphere vs triangle). Distance is scaled by `1.0 / r` (`_DAT_005cc320`). The
  callback is `call [esp+0x328]` at `0x0053a643`; a 0 return stops the walk.
- **Callback `FUN_00468b80`** (509 bytes): entry `count*0x90 + data`; `+0x30` = `(int)` sector triangle
  `[index].matIndex` (`movzx` at `0x00468ba5`, int store at `0x00468c27`); `+0x34` = the RGBA dword of
  `materials[matIdx]` from `[[[0x0088e654]+8]+0x10]` (`0x00468baa..0x00468bbc`); vertices, normal, 100000.0,
  zeros, three SAT edge normals; `DAT_0088e60c += 1` at `0x00468d6c..0x00468d73`. Returns EAX = the
  collTriangle pointer (`0x00468b99`, not overwritten), so the walk never stops early.
- **Stream layout** (reader `FUN_0053d2b0` -> `FUN_0053d820` -> `FUN_0053d5a0`): plugin 0x11D on world sectors
  (registered `FUN_0053d0b0`, offset global `0x007dc5b4`), version 0x37002 -> COLLTREE chunk 0x2C -> STRUCT
  {u32 flags, 6 floats (stream inf, sup), u32 numTris, u32 numSplits, 16-byte splits, u16 map if flags & 1}. In
  memory: +0 flags, +4 sup, +0x10 inf (swapped at `0x0053d860..`), +0x1c u16 numTris, +0x1e u16 numSplits, +0x20
  splits, +0x24 map. Measured on all 13 COLLI*.BSP: every non-empty sector carries one, every empty sector none,
  every matListWindowBase is 0; sizes match (Training sector 3: 7072 = 36 + 348*16 + 734*2).
- **The port's stand-in** (`ContactProducer.cpp:72`, called at `WheelContactSolver.cpp:153`): every triangle
  whose AABB grown by r contains the centre, in sector / triangle-index order, normal from `std::sqrt`, and
  **surface key 0 on every entry** (`VehiclePhysicsRun.cpp:271`).
- **The key is consumed.** `0x0046cc40` (`CarWorldContacts.cpp:158-165`) never takes an entry whose key is
  `0xff010101` for a wheel and runs a 32-slot history match on it (`0x00468b40`); the wheel's `+0x1f0` = key
  (`:172`) selects A6a's grip threshold (`Integrate2.cpp:206-218`; -1 / 0xffc8c8c8 also damp the velocity).
  Training's collision materials are `ffc88080 ff008000 ffb48080 ff010101 ffff0080`: material 3 is the
  never-a-wheel-contact colour. The stand-in hands those triangles to the classifier with key 0.
- **Chunk loop.** `FUN_00470c70` takes its budget as `param_1`; its only caller pushes the constant `0x32`
  (`0x0042c980` -> `0x00425a8d`), so `remaining -= min(remaining, 0x32)` at `0x00471143..0x00471151` reaches 0
  after one pass on the original. The port's `remI = (int)(dt*3000)` is 50 at the fixed 60 Hz step.

## 1. Changes (all default OFF in this file)

- **Fix W, `MASHED_D2BSP=1`.** A replica of the RW collision world built from the COLLI*.BSP stream at track
  load (world `+0x10` materials, `+0x1c` root; plane sectors `{type, value, left, right, leftValue,
  rightValue}`; world sectors `{-1, triangles, vertices, coll}`; coll trees in the in-memory layout above).
  `FUN_00539ec0`, `FUN_00547450`, `FUN_004c3b90` are verbatim naked asm generated from the bytes
  (`re/tools/naked_specs/collworld_d2bsp.json`, generator extended to pass the one register-indirect call),
  run under `_controlfp_s(_PC_24)` (memory `track-load-runs-at-pc24`). `FUN_00538c80` is C++ (case 3 only; the
  other cases stay with their existing callers). `FUN_00468b80` = the existing `FillBatchEntry` + key = material
  RGBA + return the collTriangle pointer. No batch cap is added (the original has none); storage is sized well
  above any measured count and an overflow is counted, never silently truncated.
- **Diagnostic sub-knob `MASHED_D2BSP_KEY0=1`** (report only): fix W with the key forced to 0, to separate the
  set / order effect from the key effect.
- **Fix L, `MASHED_D2CHUNK=1`.** The outer loop `for (rem = remI; rem; rem -= chunk)` around the per-car A4 /
  refresh / substep block, `chunk = min(rem, 0x32)`. Stated residual: the original interleaves all 16 cars per
  chunk; the port steps one car at a time. Identical while `remI <= 50`.
- **Diagnostics** (write nothing when unset): `MASHED_BATCH_DUMP=<path>` (port, player slot, classifier entry:
  query, count, entries, wheel positions), `MASHED_BSP_REPLAY=<orig csv>` + `MASHED_BSP_REPLAY_OUT=<path>`
  (after the COLLI world is built: run fix W, the stand-in, and the port's classifier on every captured query).

## 2. Original-side capture

`scenario_launch.py --batch-probe` (new, entry hooks only): site 0 = `0x0046cc40` entry, `args[0] == rec`
(query from the record's ring `+0x30..+0x38` and `+0x4a4`, `[0x0088e60c]`, entries at `0x00828320`: dwords
0..0xd, 0xf, 0x1b..0x23; the four wheel positions `0x0088e620..`; the classifier's record inputs `+0x9c8..+0x9d0`,
the per-wheel `+0x194/+0x1ec/+0x1f0` it starts from, and the active history slots `+0xbfc/+0xc7c`); site 1 =
`0x0046f6c0` entry, `EDI == rec` (per wheel `+0x194, +0x1ec, +0x1f0, +0x200..+0x208` = the previous classifier's
output); site 2 = A6a `0x00467650` frame marker. A count-only pass runs first (prediction ~2 classifier calls per
frame for the player, ~120/s). Recipe = the D2 reference (`orig_solo3`): `--statediff-drive --statediff-drive-late
--statediff-steer 1 --hold 38 --poke-ctrl-slots`. A piz-collision crash is re-run, never a finding.

## 3. Gates

- **G-WALK** (bit-exact): for every site-0 sample, fix W on the sample's (centre, radius) gives the same count
  and the same entries in the same order, every captured dword identical. **Prediction PASS on 100 %.**
  Reported beside it (no gate): the stand-in on the same queries (same set / same order / count histogram).
- **G-WHEEL** (per wheel per call): the port's classifier on the fix-W batch, the captured wheel positions and
  record inputs, gives the original's site-1 values of the next solver entry (`+0x194, +0x1ec, +0x1f0`, normal)
  on >= 99 % of wheel-calls. Reported beside it: the same with the stand-in batch.
  **Prediction: fix W >= 99 %; the stand-in below it on wheel-calls whose batch holds a `ff010101` triangle.**
- **G-KEY** (knob took): `MASHED_BATCH_DUMP` on the D2 arm shows Training colours in `+0x34` under fix W and 0
  without it.
- **No-op control**: the new build with no knob = the base build's G-CTRL hashes (parent's base
  `716cb2fd / 2a2afda1 / ba90ed88`, re-measured here on a build of `9420dde2`). **Prediction PASS.**
- **G-CHUNK**: fix L alone = the base G-CTRL hashes bit-exactly, and D2 arm motion_diag identical to the base
  arm's. **Prediction PASS** (one pass while remI = 50).
- **Determinism**: every G-CTRL arm that is a default candidate runs twice, hashes identical.
- **D2** (bounds `d81a8df6`, unchanged; `verify/d2settle_20261016/score_d2.py`, 3 runs per arm): slip 1500-2000
  and slip 2000-2600 inside their bounds 3/3, driving-median reported against U-9180 (base ~1860, bound floor
  1904.70) and not below the base arm's own 3-run minimum.
- **Recovery H1** (`a8_launch.py`): >= 50 % of the 400 post-trough frames >= 100 and median >= 900, 3/3.
- **D3 (b)** per regime (`B_*_K{0,1,26}`, `score_regimes.py`): no car-regime pass lost against the base.
- **Collateral** (`collateral.py --mode banded`, `cut600.py`, A = `o_p2b`, floor `o_p0a`): fields past the floor
  not more than 2 above the base arm's.
- **Watch list** (report, no gate): `wcs_drift`, airborne drift, Training wall pins (memory `d2-arm-wall-pin-
  signature`), `+0x9e0` distribution.

No direction is predicted for D2, H1, D3 (b) or collateral under fix W. The bounds decide.

## 4. Default rule (decided now)

- **Fix W** is wired default ON (opt-out `MASHED_NO_D2BSP=1`) and the stand-in's call is removed from the default
  path only if ALL of: G-WALK PASS, G-WHEEL >= 99 %, no-op PASS, determinism PASS, D2 slips in bounds 3/3 and
  driving-median not below the base minimum, H1 3/3, D3 (b) no loss, collateral <= base + 2. Otherwise it stays
  default OFF and RESULT says which gate failed. A driving-median move into its bound is reported first.
- **Fix L** is wired default ON (opt-out `MASHED_NO_D2CHUNK=1`) if G-CHUNK PASSES; it changes nothing measured
  at remI = 50, so it cannot fail a D2 gate.
- Tracker changes are proposals for `re-classify`; nothing here promotes a C-level (the walk has no run_diff
  path in this lane, and fix W is exe-side).
