# Vehicle physics struct — consolidated canonical map (A1, 2026-06-16)

Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(`.unpatched` backup matches the anchor; Ghidra pool8, read-only).

This is the **WS-A / A1** deliverable: one canonical layout for the per-car vehicle
record, merging and **correcting** the two earlier fragments
([[vehicle_dynamics.md]] and [[vehicle_damage.md]]) and adding the spawn/init site.
Source functions decompiled this session: `FUN_00470670` (control-input integrator),
`FUN_0046ddb0` (per-wheel physics core), `FUN_00470c70` (16-car dispatcher),
`FUN_0046b540` (per-vehicle suspension/spring/mass init), `FUN_0046c5c0` (destroy).

The C++ port of this layout is `mashedmod/src/mashed_re/Vehicle/VehicleStruct.h`.

---

## 0. CRITICAL CORRECTIONS to the prior docs (read first)

Two systematic errors in `vehicle_dynamics.md` are corrected here. Anything that
ports off the old table must re-check against this one.

### 0.1 The struct base is `DAT_008815a0`, **not** `DAT_00881000`
`vehicle_dynamics.md` states "Vehicle slot array base: `DAT_00881000`, stride 0xD04".
**The real base of the record that `FUN_00470670`/`FUN_0046ddb0` operate on is
`DAT_008815a0` (car 0), stride 0xd04.** The pointer passed in EAX/EDI to the
per-frame cluster points at `DAT_008815a0 + car*0xd04`. `DAT_00881000` is 0x5a0
bytes *before* car 0 and is not the physics record base.

Three independent cross-checks pin the base at `DAT_008815a0`:
- **Cars 1..3** appear in `FUN_0046ddb0`'s contact loop as the literals
  `DAT_008822a8`, `DAT_00882fac`, `DAT_00883cb0` — these are exactly
  `0x8815a4 + 1/2/3 × 0xd04` (the active-flag field at base+0x4). RVA 0x0046df48+.
- **Mass field** `EDI[0x14]` (byte +0x50) read by `FUN_0046ddb0` at 0x0046de9c is
  the same memory the init writes as `*(&DAT_008815f0 + car*0xd04) = 1000.0`
  (`FUN_0046b540` @ 0x0046b7..); `0x8815f0 − 0x8815a0 = 0x50`. ✓
- **Self-index compare** `iVar11 != *EDI` (0x0046df3a) treats `EDI[0]` (byte +0x0)
  as the per-car index; `DAT_008815a0` is the slot the dispatcher writes the car
  index into (`FUN_00470c70` `piVar14[-0x27c]`, 0x0047103x). Active flag is the
  *next* int at base+0x4 = `DAT_008815a4` (init writes `= 1`). ✓

`vehicle_damage.md` already uses the correct base symbol `0x008815a4`; note its
"base" is the **active-flag** field (= our base+0x4), so its byte offsets are
**+0x4 higher** than this document's. (i.e. damage `+0x000` = our `+0x4`.)

### 0.2 `vehicle_dynamics.md` mixes int-index and byte units
The decompiler shows `EDI[N]` (an `int*` index ⇒ byte `N×4`). Several rows in the
old table copied the int index into a "+0x.." byte column **without multiplying**,
so they are 4× too small. Confirmed mislabels:
- "wheel-0 right dir `+0x60`" — the access is `EDI[0x60..0x62]` ⇒ byte **+0x180**
  (wheel-1's was correctly listed `+0x244` = `0x91×4`). RVA 0x0046e4b0.
- "gear torque consts `+0x54..+0x5C`, `ESI[0x54]*[0x55]*[0x56]`" — the access is
  `EDI[0x54..0x56]` ⇒ byte **+0x150..+0x158**. RVA 0x0046de7e.

Every offset in §2/§3 below is a **byte offset from base `DAT_008815a0`**, and the
int index it came from is shown so the unit is unambiguous.

---

## 1. The per-car array

| Property | Value | Evidence |
|---|---|---|
| Base (car 0) | `DAT_008815a0` | §0.1 |
| Stride | `0xd04` bytes (`0x341` ints) | `FUN_00470c70` `piVar14 += 0x341`; `FUN_0046ddb0` contact loop `+= 0xd04`, bound `< 0x3410` (= 4×0xd04) |
| Count | 16 slots (`FUN_00470c70` loops `local_18 = 0x10`); the contact loop scans only the first **4** | 0x00470f.. / 0x0046df.. |
| Car N base | `DAT_008815a0 + N*0xd04` | — |

Per-frame entry is `FUN_00470c70(carCount, &dt)` → for each active slot
`FUN_00470670(idx, subSteps, inputDesc, &dt)` → `FUN_0046ddb0` → `FUN_00467650`
→ `FUN_00468980`. (`FUN_00467650`/`FUN_00468980` are A6, not yet mapped.)

---

## 2. Main record fields (byte offset from `DAT_008815a0`)

Type column: `f`=float, `i`=int/dword, `vec3`=3 contiguous floats, `?`=raw.

| Byte | int idx | T | Field (mechanical description) | RVA(s) |
|---|---|---|---|---|
| +0x00 | [0] | i | Car index / self-id (skip-self compare) | 0046df3a |
| +0x04 | [1] | i | Active flag (init `=1`; destroy `=0`; `==1` gate) | 0046b7c1 (w), 0046df48 (r), 0046c5dd (w0) |
| +0x10 | [4] | i | State/destroyed flag (init `=0`; `==1` ⇒ wheel friction branch) | 0046b7c8 (w), 0046e6b8 (r) |
| +0x2c | [0xb] | i | (read in 2nd wheel loop as `EDI[0xb]` scalar mult on slip) | 0046e7e0 |
| +0x50 | [0x14] | f | **Mass** (init `=1000.0`); torque ×dt, airborne damp | 0046b7..(w), 0046de9c, 0046e98e |
| +0x54 | [0x15] | f | Mass factor B (init computed inertia sum) | 0046b9.. (w) |
| +0x58 | [0x16] | f | 1 / inertia (init `= 1.0 / +0x54`) | 0046ba.. (w) |
| +0x150 | [0x54] | f | Gear torque const 0 (`[0x54]*[0x55]*[0x56]`×throttle) | 0046de7e |
| +0x154 | [0x55] | f | Gear torque const 1 | 0046de7e |
| +0x158 | [0x56] | f | Gear torque const 2 | 0046de7e |
| +0x16c | [0x5b] | — | **Wheel[0] block** base; 4 wheels, stride 0xC4 (see §3) | 0046de0a (loop), 0046b7.. (init) |
| +0x190 | — | f | **Per-vehicle accel/force magnitude** (`input×this`) | 0047079e (byte off in FUN_00470670) |
| +0x1a8 | — | f | Drive-torque output (this frame; cleared then set) | 004706.., 0047086x |
| +0x1ac | — | f[16] | Drive-torque ring; phase `DAT_007f101c & 0xf` | 0047087x |
| +0x26c | — | f | Angular/steer-torque output (this frame) | 004706.., 00470875 |
| +0x270 | — | f[16] | Angular-torque ring | 0047088x |
| +0x330 | — | i | Cleared each tick [UNCERTAIN semantic U-V01] | 004706cd |
| +0x3f4 | — | i | Cleared each tick [UNCERTAIN semantic U-V02] | 004706c4 |
| +0x928 | — | mat | Wheel-transform matrix blocks (0x40 each), indexed by +0x9a8 | 004706f? |
| +0x958 | [0x256] | vec3[] | This-car wheel/contact world positions, stride 0x40 (`[i*0x10+0x256]`) | 0046e0.. |
| +0x9a4 | — | i | Contact double-buffer index A (init `=0`) | 0046b8.. (w), 0046e1.. (r) |
| +0x9a8 | [0x26a] | i | Wheel-matrix set selector / contact ring (init `=1`; `×0x40 + 0x928`) | 0047069a, 0046e0.. |
| +0x9b0 | [0x26c] | vec3 | **Linear velocity** (world); drag×, +impulse, parked-damp | 004706.., 0046e3.., 0046e4.. |
| +0x9d4 | [0x275] | vec3 | **Forward direction** (world); transformed from model const `DAT_00614708` | 0046ddc1, 00470730 |
| +0x9e0 | [0x278] | f | **Grounded-wheel count** (+1.0 per contact; `==4.0`/`0x40800000` all-grounded) | 0046ddd? |
| +0x9e4 | [0x279] | f | **Speed magnitude** (= max-speed denom in FUN_00470670; throttle-ish in FUN_0046ddb0) [resolves U-3722: one field] | 004706a3, 0046de.. |
| +0x9f0 | — | i | Motion state (`==2` ⇒ damp velocity by `_DAT_005cc9c8`) | 00470960 |
| +0xacc | — | i | (init `=0`) [UNCERTAIN] | 0046b8.. |
| +0xad0 | [0x2b4] | i | Pos/ang-vel history ring index (`(idx-1)&1`) | 0046de3? |
| +0xad4 | [0x2b5] | f[6] | History buffer: 2 × vec3 of `+0x9b0` snapshots | 0046de4? |
| +0xb00 | [0x2c0] | vec3 | Random per-tick impulse (`FUN_00472650(-r,r)`) | 0046e8.. |
| +0xb0c | — | f | Slide measure = `(1 − |fwd·vel|/speed) × speed` | 0047074x |
| +0xb14 | — | vec3 | Boost/accumulated force vec (cleared here; written by FUN_00467650) | 004706.. |
| +0xb18 | [0x2c6] | f | Airborne velocity-damp counter (`−= dt·mass·_DAT_005ccd08`) | 004706bf, 0046e9.. |
| +0xb1c | — | i | scratch (cleared) | 004706b? |
| +0xb20 | [0x2c8] | i | Airborne flag (set 1 when above speed/contact thresholds) | 004706.., 0046e96x |
| +0xb24 | — | i | Filtered accel input = `FUN_004a2c48(inputDesc[0])` | 0047078x |
| +0xb28 | — | i | Filtered brake input = `FUN_004a2c48(inputDesc[1])` | 0047079x |
| +0xbec | [0x2fb] | i | Per-tick ring counter (`(n+1)&0xf`); indexes `+0xb34` ring | 0046e88x |
| +0xb34 | [0x2cd] | f[…] | ring written from `+0xbec` phase (×3 stride) | 0046e8a? |
| +0xbf0 | — | i | Boost gate (≠0 ⇒ drive force ×`_DAT_005cc950`) | 0047080x |
| +0xd00 | [0x340] | i | Slip/hover state flag (`!=0` ⇒ wheel slip factor 0.2 vs 0.01) | 0046e7b? |

> RVA suffixes shown as `xxxx?`/`xx` are the byte region within the cited function;
> exact instruction addresses are recoverable from the decomp listing if needed for
> a hook plate. The function-level RVA (e.g. 0x0046ddb0) is exact.

### Fields from `vehicle_damage.md` (base there = our +0x4); carried forward
Re-based to `DAT_008815a0` (add +0x4 to that doc's offsets):
- +0x0B10 control_source (destroy writes `DAT_007f1030`) — `FUN_0046c5c0`.
- +0x924 transform sub-object pointer region (`DAT_00881ec8` family; getter
  `FUN_0046d4a0` returns `*(ptr + subIdx*0x10)`, then reads +0x30/+0x34/+0x38 =
  world X/Y/Z, +0x80 = subIdx). The position consumer for camera/HUD.
- +0xA50 spinout flag (0=alive, 2=slide) — `FUN_0046cbb0`.

---

## 3. Wheel sub-block (4 × 0xC4 bytes; first at +0x16c)

Offsets are **relative to the wheel block base** (`wheelN = +0x16c + N*0xC4`, i.e.
int `0x5b + N*0x31`). Per-frame wheel loop @ `FUN_0046ddb0` 0x0046ddf0
(geometry/contact) and 0x0046e6.. (suspension force). Init geometry @ `FUN_0046b540`.

| W-byte | W-int | T | Field | RVA |
|---|---|---|---|---|
| +0x00 | [0] | f | Contact/mount component X (→ scratch `DAT_00881560`) | 0046de0a |
| +0x08 | [2] | f | Contact/mount component Z (→ scratch) | 0046de1? |
| +0x14 | [5] | vec3 | Lateral/right axis (world; transformed by wheel matrix) | 0046de2?, 0046e4b0 (steer use) |
| +0x2c | [0xb] | i | **Contact flag** (≠0 ⇒ wheel grounded; gates steer/force) | 0046ddf?, 0046e4a? |
| +0x3c | [0xf] | f | Steer angle (0 ⇒ no rotation; else `FUN_004c4d20` matrix) | 0046de5? |
| +0x88 | [0x22] | f | 2nd-loop base (pfVar14 = wheel+0x88) [dynamics scratch] | 0046e6.. |
| +0x98 | [0x26] | f | Spring compression / load input (`[6]·[4]` ⇒ +0xa4) | 0046e7.. |
| +0xa0 | [0x28] | f | **Steer-torque output** (`EDI[0x83]` wheel0); aliases load | 0046e4f? |
| +0xa8 | [0x2a] | vec3 | Suspension force vector (`[8..10]`) | 0046e8.. |
| +0xb4 | [0x2d] | vec3 | Steered forward axis (world; from steer matrix) | 0046de6? |

Per-wheel right-axis & steer outputs for all four wheels (the §0.2-corrected set):

| Wheel | right-axis (int / byte) | steer out (int / byte) | contact gate (int / byte) |
|---|---|---|---|
| 0 | [0x60..62] / +0x180 | [0x83] / +0x20c | [0x66] / +0x198 |
| 1 | [0x91..93] / +0x244 | [0xb4] / +0x2d0 | [0x97] / +0x25c |
| 2 | [0xc2..c4] / +0x308 | [0xe5] / +0x394 | [0xc8] / +0x320 |
| 3 | [0xf3..f5] / +0x3cc | [0x116] / +0x458 | [0xf9] / +0x3e4 |

(Each is the previous + 0x31 ints / +0xC4 bytes — confirms the 0xC4 wheel stride.)

---

## 4. Init / spawn chain

### 4.1 `FUN_0046b540` — per-vehicle suspension/spring/mass init (A1/A3 lead)
Signature `undefined4 FUN_0046b540(float carIdx)`; returns 0 if idx ≥ 16 else 1.
Writes (car-indexed `[idx*0x341]` / `+ idx*0xd04`):
- base+0x4 active flag `=1`; base+0x10 state `=0`.
- Mass `+0x50 = 1000.0` (`0x447a0000`); `+0x54`/`+0x58` computed inertia & 1/inertia.
- Wheel geometry block(s) at +0x16c onward (the same wheels §3 reads): direction
  consts `0x3f19e83e`, `0.397094`, spring-rate `0x42080000`(=34.0), etc.
- Contact double-buffer indices `+0x9a4 = 0`, `+0x9a8 = 1`.
- Several contact-point distance tables (loops of 4 / 8 / 6, stride 0x10) at
  +0x4b8 / +0x5b8 / +0x7b8 using wheel mount positions (+0x60 / +0xf0) vs the
  reference point (+0x158/+0x15c/+0x160).

**Handling-data source** (consume in A3): the init seeds defaults
`DAT_00613108=100.0`, `DAT_00613114=40000.0`, `DAT_00613130=1.0`, `DAT_0061313c=1.5`,
then overrides them from a **static keyed table at `0x00613140`** (5-int stride,
`-1`-terminated): each entry = `[paramA, spring=40000, k1=1000, k2=1500, nextKey]`,
keys observed `0, 6, 12, …`. The key for the current car is
`FUN_0040ce80(carIdx) = *(*(PTR_PTR_005f2770 + carIdx*4) + 4)` — i.e. the car's
object pointer table, deref, field +4 = handling id. (`DAT_00613130`/`0061313c`
are scaled by `_DAT_005cc558` before use.)

### 4.2 `FUN_00470c70` — 16-car per-frame dispatcher
Inits physics-scale globals `_DAT_0088e610 = dt × _DAT_005cea80`,
`_DAT_0088e5f0 = _DAT_005ccd08 / _DAT_0088e610`; selects the per-car input
descriptor (`&DAT_007f1038 + map*0x13`, AI vs human via `&DAT_007f19b8`); calls
`FUN_00470670` per active car; then a contact-resolve pass.

### 4.3 `FUN_0046c5c0` — destroy (zeroes active flag, writes control_source).

### 4.4 [UNCERTAIN] Allocation site
The code base for the array (`DAT_008815a0`) is a fixed `.bss` region (16 slots ×
0xd04 are statically reserved, not heap-allocated). The DFF/model→wheel-count and
the *per-instance object pointer* table `PTR_PTR_005f2770` are populated by an
earlier spawn/load step not yet traced. **U-V03**: where `PTR_PTR_005f2770[idx]`
and the wheel **count/layout** (vs the fixed 4 here) are filled. Resolution path:
xref `PTR_PTR_005f2770` writers; trace the race-load entity spawn.

---

## 5. Parallel per-car arrays (separate `.bss`, same 0xd04 stride, car-indexed `[i*0x341]`)
These are referenced by their own absolute symbols (not offsets within the §2
record). Carried from the cluster/damage docs with this session's confirmation:
- `DAT_00881f48` / `DAT_00881f4c` — contact write double-buffer ptr A/B
  (`FUN_0046b540` init `=0`/`=1`; `FUN_0046ddb0`/`FUN_00470c70` flip via `&1`).
- `DAT_00881ec8` family — per-car transform sub-object (see §2 +0x924 note).
- Score / position arrays `0x008a94e0`, `0x008a9640` (stride 0x30c), spectator
  slots `DAT_0063c018` (stride 0x6c) — see `vehicle_damage.md` §"score/position".

---

## 6. Open uncertainties
| ID | Where | Question |
|----|-------|----------|
| U-V01 | +0x330 | Cleared each tick by FUN_00470670; never read in the 4 mapped fns |
| U-V02 | +0x3f4 | Same — cleared, no read site found in mapped fns |
| U-V03 | alloc | `PTR_PTR_005f2770[idx]` writer + wheel count/layout source (DFF) unmapped |
| U-V04 | +0x9e4 | Resolves dynamics U-3722 to a single "speed magnitude" field; confirm sign/units at C4 with a live diff |
| U-3577/3721/3723 | — | Inherited from `vehicle_dynamics.md`; re-evaluate once §2 ported |

## 7. What this unblocks
- **A4** (`FUN_00470670` port) — fields +0x190/+0x1a8/+0x26c/+0xb24/+0xb28/+0xbf0
  and the drive/reverse force formula are mapped; harvest `_DAT_005cea..`/`_DAT_005cc..`
  consts (A7) then port.
- **A5** (`FUN_0046ddb0` port) — wheel sub-block (§3) + velocity/grounded-count +
  contact-array offsets mapped; still needs WS-B contact source (`DAT_008815a4`
  active-flag scan + contact buffers).
- **A3** (init/spawn port) — `FUN_0046b540` + the `0x00613140` handling table are
  the entry points; U-V03 (object-ptr/wheel-count) is the remaining gap.
