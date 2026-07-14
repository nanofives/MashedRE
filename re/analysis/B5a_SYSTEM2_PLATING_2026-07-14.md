# B5a — system-2 call-surface plating (STARTED 2026-07-14)

Lane B5a (ROADMAP §WS-B): *demand-closure + plating of what `0x0047eb30`/`0x0047f840`
actually reach in the 0x55 band + island caller closure (brief Open-Unknowns #3/#4).*
This is the first lane of the D1-Option-A system-2 reconstruction that is the faithful
fix for WS-PHYS-DRIVE-STABILIZE (see `INITD3D9_HANG_AND_REMEASURE_2026-07-14.md` +
RE_MASTER_PLAN §7 item 10). Read-only Ghidra; no port yet (that's B5b..B5d).

## System-2 call surface (from `COLLISION_GATE_BRIEF_D1_2026-07.md:60`)

| Piece | RVAs | Role |
|---|---|---|
| qhull bridge | `FUN_0057ca30` (← single caller `FUN_00481e00`) | wrap vendored qhull-2002.1; LOAD-TIME only, not per-frame (brief:18-19) |
| physics-world init | `FUN_0047f840`, `FUN_00480340` | build physics world `DAT_006ce274` + 4 bodies `DAT_006c9a78[0..3]`; sets build flag `DAT_0086caa0=0x7b` |
| 4-body build chain | `FUN_0047d3c0`→`FUN_004826d0`→`FUN_00481e00` | 120-ray cone-cast → qhull → mesh; runs once when `DAT_0086caa0==0x7b`, flag cleared at `0x0047eba7` (brief:20) |
| per-tick integrator | `FUN_0055dff0`, `FUN_0055ac00`, `FUN_0055deb0`, `FUN_0055c000` (0x55-band) | steps the 4 proxy bodies each frame |
| body accessor | `FUN_0057c210` | read a body transform: `FUN_0057c210((&DAT_006c9a78)[i])` |
| coupling bridge | `FUN_0047eb30` | per-tick step: advances bodies via the integrator, reads via `FUN_0057c210`, never re-enters qhull (brief:21) |

## Demand-closure — direct callees (Ghidra pool0 read-only, this session)

**Coupling driver `FUN_0047eb30` — 19 callees.** System-2 members present:
`FUN_0047d3c0` (4-body build root), `FUN_0055ac00` / `FUN_0055deb0` / `FUN_0055dff0`
(0x55-band integrator), `FUN_0057c210` (body accessor), `FUN_0047ea40` (post-step),
`FUN_00562460`. Vehicle/RW-math helpers also called: `FUN_0046cb30`,
`FUN_0046d4a0/d4d0/d510`, `FUN_004c0e50/c3df0/c45f0` (0x4c RW-matrix), `FUN_004233e0`,
`FUN_00426060`, `FUN_0040e340/e350`, `FUN_004a3384`.

**Physics-world init `FUN_0047f840` — 13 callees.** Mostly 0x55-band:
`FUN_0055ad20`, `FUN_0055bac0`, `FUN_0055c490/c4a0/c4f0`, `FUN_0055ddd0`,
`FUN_0055deb0`, `FUN_0055dec0`, `FUN_0055fe10/fe30/fe40`, plus `FUN_0047f4c0`
(0x47 helper) and `FUN_00562600` (0x56-band). `FUN_0055c000` per brief:60 is in the
integrator set — confirm its call-site vs the `c490/c4a0/c4f0` cluster during plating.

Observation: the surface concentrates in the **0x55-band** (the RW-Physics integrator
island, hooks.csv band ~`0x0057c5b0..0x005a5820`, 246 rows / 100% C1-C2 / 0 ported,
~165 KB per `[[qhull-rwphysics-island]]`). qhull's own bytes are "free" (vendored,
B5b); the 0x55-band integrator that steps the bodies is NOT — it must be ported.

## Already mapped — do NOT redo in B5a

- `re/analysis/vehicle_coupling.md` — **definitive** RE of coupling driver `FUN_0047eb30`
  (`0x0047eb30..0x0047f1db`) + accessor `FUN_0057c210`: per-car struct (stride 0x341/0xd04),
  field offsets, `FUN_0057c210((&DAT_006c9a78)[i])` body-fetch, transform/velocity slot
  chains, PD-gain constants (`_DAT_005ccd6c=20.0`, 0.06 s lookahead). ⇒ B5d port source.
- `WS_A8_REALPHYS_2026-07-01.md` — root-cause diagnosis (unbounded speed → 1500 clamp;
  dead steer) tied to the missing two-body loop.
- `D1_SPIKE_PROXY_BYPASS_2026-07-06.md` — behavioral proof system 2 is load-bearing;
  enumerates the per-frame surface removed on bypass (`FUN_0055deb0` step, `FUN_0046d4d0`
  readback to record `+0x928`, `FUN_0047ea40` post-step).
- Leaf notes exist: `FUN_0047f840` (bucket_physics_smplfzx…), `FUN_00480340`
  (render_2_c1_to_c2_s3), `FUN_0057ca30` + `0x0057c5b0` (bucket_util…).

## B5a remaining work (this lane's actual deliverable)

1. **Plate the 0x55-band integrator** — decomp + demand-map `FUN_0055dff0`, `FUN_0055ac00`,
   `FUN_0055deb0`, `FUN_0055c000` (+ the init-side 0x55 cluster from `FUN_0047f840`):
   inputs (which body fields), outputs (which body fields), constants, callees. No existing
   dedicated note.
2. **Plate the 4-body build chain** — `FUN_0047d3c0`→`FUN_004826d0`→`FUN_00481e00`
   (120-ray cone-cast → qhull → mesh): what it reads (track collision), what it writes into
   the 4 bodies `DAT_006c9a78[0..3]` + world `DAT_006ce274`.
3. **Open-Unknown #3 — island caller closure.** Sweep the ~165 KB island
   (`~0x0057c5b0..0x005a5820`, 246 rows) for callers OUTSIDE system 2. Only `FUN_0057ca30`
   ←`FUN_00481e00` is proven. Goal: confirm the island is entered only via the system-2
   surface above (so vendoring qhull + porting the 0x55 integrator is a closed set).
4. **Open-Unknown #4 — integrator entanglement.** Prove the 0x55-band integrator touches
   only the 4 proxy bodies `DAT_006c9a78[0..3]` + world `DAT_006ce274` per-frame (no hidden
   global/render-state writes). Currently "assumed isolated; not proven."

## B5a exit criteria

A demand-closed static map of system 2's per-frame + load-time call surface such that B5b
(qhull vendor) and B5c (integrator port) have a complete, caller-closed target list, with
Open-Unknowns #3 and #4 resolved (island closed; integrator isolation proven or its extra
entanglements enumerated). No behavioral claims (that's B5e).

---

# B5a PLATING RESULTS (DONE 2026-07-14 — Ghidra pool3, read-only, MASHED.exe anchor)

All decomp/constants below cite the live Ghidra address. NO-GUESSING: mechanical only.
All 9 target functions already had leaf notes (delegate cross-read, `claude2`); this section
supplements them with the B5a-specific findings and resolves the two Open-Unknowns.

## Global-identity reconciliation (was the map's chief ambiguity)

The B5a map and the leaf notes disagreed on `DAT_007dc8d8` vs `DAT_006ce274`. Resolved by
`FUN_0047d3c0` (build root) + `FUN_0057c210` (accessor) decomp — **they are three distinct
globals**, all correct:

| Global | Role (cited) | Evidence |
|---|---|---|
| `DAT_006ce274` | the physics **world** object | `FUN_0047eb30:0047eb30` gate `if (DAT_006ce274 != 0)`; passed as `param_1` to `FUN_0047d3c0(DAT_006ce274,uVar5)` and to `FUN_0055dec0(param_1,body,-1,2)` which registers each body into it |
| `DAT_006c9a78[0..3]` | 4-entry array of **body-handle keys** (byte offsets) | `FUN_0047d3c0`: `(&DAT_006c9a78)[i] = FUN_004826d0(...)` stores the collision-body handle per body i |
| `DAT_007dc8d8` | base of the **body-pointer lookup table** | `FUN_0057c210(key) = *(DAT_007dc8d8 + key)`; setter `FUN_0057c220(key,ptr)` links them in `FUN_0047d3c0`. (The stale "animation table" label in `fun_00471ec0_callees/0x0057c210.md` is WRONG — it is the physics body table; `vehicle_coupling.md` is ground truth.) |
| `puVar3` (local) | material/scene builder from `FUN_0057c500()` | its first field `*puVar3` = the RWP scene handle `uVar1` fed to `FUN_0055c810/c4f0/c4a0/bab0/b940` |

## Per-function plates (0x55-band integrator quartet + accessor)

### `FUN_0055dff0(body,0)` — body world-matrix refresh gate  (C1, RWP lib-tag)
- Reads `*(body+0x24)` (owner ptr, `0x0055dff3`); if owner!=0, tests the shape-active bit:
  `*(owner+0x60 + (*(ushort*)(body+0x20) >> 5)*4) & (1 << (*(ushort*)(body+0x20) & 0x1f))`.
- If set → `FUN_0055b800()` (args in regs; see below). Pure gate; **no absolute writes**.
- Call-site identity (`FUN_0047eb30`): `param_1 = iVar6 = FUN_0057c210((&DAT_006c9a78)[i])`
  = the resolved **body pointer**.

### `FUN_0055b800(container,srcMtx)` — refresh one body's transform  (callee of dff0)
- `base = **(int**)(*container+0x10)`; `idx = container[1]` (body index).
- Writes body matrix slot `idx*0x40 + base` (0x40-byte RwMatrix) via `FUN_004c52f0` (mul,
  `0x0055b811`) + `FUN_004c51a0`; writes 0x10-byte vector slot `idx*0x10 + (*(int**)(*container+0x10))[4]`
  via `FUN_00546b10`. All targets are **pointer-indirect** (base derived from `container`).
  **No absolute writes** (verified). RW-math callees `004c52f0/004c51a0/00546b10` also abs-write-free.

### `FUN_0055ac00(bitsetOwner, body, set)` — shape-active bit set/clear  (C1, RWP lib-tag)
- `uVar2 = *(ushort*)(body+0x20)` (shape index). `p = *(int*)(bitsetOwner+0x5c) + (uVar2>>5)*4`.
  `set!=0`: `*p |= 1<<(uVar2&0x1f)`; else `*p &= ~(...)`. Leaf, **no absolute writes**.
- **Call-site identity resolved** (`FUN_0047eb30`): `param_1 = *(undefined4*)(iVar6+0x24)`
  = `*(body+0x24)` = the body's **owner/broadphase container** (holds the contact-active bitset
  at `+0x5c`). Called with `set=1` on the active bodies; with `set=0` in the sleep branch
  (`iVar6 <= iVar4`, `0047f0??`). So `+0x5c` bitset ≠ the `+0x60` bitset read by `dff0` — different
  owners (body-owner vs world-broadphase); report both raw.

### `FUN_0055deb0(world, dt)` — world-handle getter / step entry  (C1, RWP lib-tag)
- Body is a 7-byte leaf: `return *(undefined4*)(param_1+4)` (`0x0055deb0`). The `dt` 2nd arg is
  passed at the call but **ignored by this getter's body**.
- Call-site (`FUN_0047eb30`, after the per-body loop): `FUN_0055deb0(DAT_006ce274, DAT_0061331c)`
  → result feeds `FUN_0047ea40(result)`. So this reads `world+4` (the solver/island handle) and
  hands it to the post-step. `DAT_0061331c = 0.05f` (`0x3d4ccccd`) is the **fixed physics timestep**
  (set in the build branch at `0x0047ebc0`). `[UNCERTAIN]` whether `dt` is consumed by a caller
  variant — not by `0055deb0` itself.

### `FUN_0055c000(shape, mtx, dir, out)` — support mapping (GJK extreme point)  (C1, RWP lib-tag)
- If `mtx!=0`: rotate `dir` into shape-local by the 3×3 of `mtx` (`mtx[0,1,2,4,5,6,8,9,10]`,
  `0x0055c01b..0x0055c046`). Then `pt = (**(code**)(*(int*)(shape+0x5c)+0x14))(shape,dir,param_4)`
  — the shape's **support/extent virtual** (vtable slot `+0x14`; indirect, impl varies by shape
  primitive — NOT statically resolvable to one function). If `pt!=0 && mtx!=0`: transform `pt` back
  to world by the full 4×3 affine (rotation + translation `mtx[0xc,0xd,0xe]`). **Returns the
  world-space support point (float[3]) written in place at `out`, or NULL on miss.**
- **Per-frame vs load-time**: callers = `FUN_00481e00` (build) + `FUN_00578f20`(0 callers),
  `FUN_00579d50`(←`57a250`/`57a660`), `FUN_0057a9a0`(←`57adb0`) — the latter three are all inside
  the 0x578–0x57a physics/GJK band (island-internal contact generation, per-frame contact is OFF
  per `[[qhull-rwphysics-island]]`). So on the **live per-frame path `FUN_0055c000` is reached only
  via the load-time build** (`FUN_00481e00`). No absolute writes.

## Per-function plates (4-body build chain — load-time, gated `DAT_0086caa0==0x7b`)

### `FUN_0047d3c0(world, carState)` — VehiclePhysicsWorldCreate / 4-body build root  (C2, vehicle)
- `puVar3 = FUN_0057c500()` scene/material builder. Gravity `puVar3[2]=0xbdcccccd (-0.1f)`,
  `puVar3[0xd]=1.0f`, `puVar3[0xe]=0x40200000 (2.5f)`. Material params on `uVar1=*puVar3`:
  friction `FUN_0055c4f0(_,0x3ecccccd=0.4)`, restitution `FUN_0055c4a0(_,0x3f000000=0.5)`,
  damping `FUN_0055bab0(_,0x3c23d70a=0.01)`, mass `FUN_0055b940(puVar3,0x3fc00000=1.5,1)`.
- CoM offsets `local_18/14/10` from car bbox `_DAT_00881630/163c/1664/1634/1638/1650`
  × `_DAT_005cc32c(0.5)` × `_DAT_005cc9dc(0.95)`; broadcast to `DAT_008816d8/dc/e0` and 3 sibling
  slots `DAT_008823dc / DAT_008830e0 / DAT_00883de4` (stride ≈ 0xd04 = the 4 car records).
- **Loop i=0..3**: `iVar4=FUN_0057c300(puVar3)`; `(&DAT_006c9a78)[i]=FUN_004826d0(uVar1,1)`;
  `FUN_00482730(handle,&color{0x40,0x40,0x60,0xff})`; attach `FUN_004e7e30`; link
  `FUN_0057c220((&DAT_006c9a78)[i], iVar4)`; place `FUN_0047d240(i,0,0.5,-10.0f,i*_DAT_005cd074)`;
  `*(iVar4+8) &= 0xfffffff3`; register into world `FUN_0055dec0(world,iVar4,-1,2)`;
  collision group `FUN_0055ae70(*(iVar4+0x24),iVar4, (FUN_00426c00()==0x26 ? i+2 : 1))`.
- Finalize `FUN_0057c550(puVar3)`. Body count fixed at **4**.

### `FUN_004826d0(scene, linkFlag)` — PhysicsCollisionBodyCreate  (C2, vehicle)
- `iVar3 = linkFlag==0 ? 0 : scene`; `hull = FUN_00481e00(scene, 1.0f, iVar3)`;
  `mesh = FUN_00482140(hull, DAT_005d757c(0.0) < *(float*)(scene+0x4c))` (RpAtomic collision-mesh
  strip builder, per the note's correction); activate `FUN_00563810(hull)`. **Returns `mesh`** —
  this is the value stored into `DAT_006c9a78[i]` by `FUN_0047d3c0`.

### `FUN_00481e00(shape, scale, mtx)` — cone-cast hull builder  (C2, vehicle) — B5b interface
- Lazy-inits the ≤120-dir cone sample table `DAT_006ce278` (guard `DAT_006ce818==0`, cap `0x77`),
  using `fsin/fcos` (float10). Cone constants (all `.rdata`, init=True, read via `flat_api.getFloat`):
  `_DAT_005cf23c=1.78 (0x3fe3d70a)`, `_DAT_005ce2f4=π=3.14159 (0x40490fdb)` latitude max,
  `_DAT_005cd2c0=2π=6.28319 (0x40c90fdb)` longitude span, `DAT_005d757c=0.0`.
  `_DAT_005cf240` reads `0.0 (0x00000000)` and `_DAT_005cf238` reads `0x047a0001` (denormal) —
  both **[UNCERTAIN]/anomalous**; the `_`-overlap warning suggests a mistyped/misaligned operand
  at these two addresses. Report raw; resolve at runtime for B5b.
- Radius: `local_5b4 = *(float*)(shape+0x4c) * scale`. Shape-type at `**(short**)(shape+0x5c)`:
  type `1` → `scale *= _DAT_005cf238`; type `4` && `local_5b4 < _DAT_005cd0ec(0.005)` → clamp
  `local_5b4 = 0.005`.
- Loop each cone dir: `pt = FUN_0055c000(shape, mtx, dir, &out)`; **NULL → early `return 0` (miss)**.
  Scale `pt` by `scale` and add `local_5b4*dir`; accumulate into `local_5a0[≤360]`.
- `return FUN_0057ca30(count, local_5a0, 0, 0)` — the qhull result handle (convex hull of the
  support cloud). **This return handle is what a body's collision hull is.**

### `FUN_0057ca30(count, pts, opt, x)` — qhull bridge  (C1, qhull-2002.1 lib-tag) — B5b root
- `opt = opt ? opt : "qhull s Pp" (0x0062406c)`. Drives qhull mode 3 via
  `FUN_0058f520(3,count,pts,0,opt,x,x)`. On success walks facet list `DAT_0091459c` (link `+0x1c`,
  data `+0x28`), sums `FUN_005834a0` counts, allocs output `FUN_00563840(DAT_009145d4, total,
  DAT_009145d0)`, builds table `FUN_0057c670(out)`. Cleanup `FUN_00589dc0/FUN_0058f0a0`.
  **Sole caller `FUN_00481e00`** (verified). This is the single external door into the qhull unit.

## OPEN-UNKNOWN VERDICTS

### OU#3 — island caller closure: **RESOLVED — CLOSED to a single external entry.**
Static direct-call sweep of the qhull unit `[0x0057c5b0, 0x005a5910)` (306 functions; range
extended to include the boundary parser `FUN_005a5820`, body_end `0x005a5902`): **exactly one
function has any caller outside the range — `FUN_0057ca30`, called only by `FUN_00481e00`**
(the system-2 load-time build path). The three apparent extra entries in the first pass
(`FUN_0057c5b0`, `FUN_0058edf0`, `FUN_0058f6a0`) were all internal — their sole caller
`FUN_005a5820` is qhull's own halfspace-input parser (reads `qh.halfspace` global `DAT_0091400c`,
qhull error strings), reached via `FUN_0058f520 ← FUN_0057ca30`; it merely sits one function past
the original exclusive bound. ⇒ The qhull island is **caller-closed**: vendoring qhull-2002.1
(B5b) is a self-contained unit whose only interface point is `FUN_0057ca30`.
*Scope caveat:* sweep is over **direct** call references. Indirect (vtable/fn-ptr) entry into the
island was not exhaustively checked, but qhull sets up its own callback tables internally and no
external fn-ptr store into the range was observed on the build path. `[C1-static]`.

### OU#4 — 0x55-band integrator per-frame isolation: **RESOLVED — isolated; footprint enumerated.**
- **Integrator proper writes ZERO absolute globals.** Direct instruction-write scan of
  `FUN_0055dff0, FUN_0055b800, FUN_0055ac00, FUN_0055deb0, FUN_0055c000, FUN_00562460` and the
  solve entry `FUN_0047ea40 → FUN_0047e9c0` — every one returns an empty absolute-write set; all
  state changes are **pointer-indirect** through the body/world handles passed as params.
- **The per-frame physics-state footprint (absolute) is confined to system-2/vehicle-owned data**,
  positively enumerated by transitive scan from the per-frame roots:
  - 4 proxy-body state arrays `0x007e9de0 / 0x007ea1e4 / 0x007ea5e4 / 0x007ea9e4` (stride 0x404)
    + aux block `0x007ec9e4`, written by `FUN_0047def0` / `FUN_0047d640` — **both called only by
    the per-frame solve `FUN_0047e9c0`** (via `FUN_0047ea40`). These fixed arrays ARE the 4 proxy
    bodies' integration state.
  - vehicle-struct readback `0x008815ac` (`DAT_008815a0` region) — `FUN_0047d640`.
  - body-table allocator metadata `0x007dc8c0/c8/cc` (adjacent to the `DAT_007dc8d8` table) —
    `FUN_0055a1f0` / `FUN_005646c0`.
  - coupling-driver bookkeeping (`FUN_0047eb30`): tick counter `DAT_006cad48` (reset+INC,
    `0x0047eb5f`/`0x0047ebca`), timestep config `DAT_0061331c=0.05` (`0x0047ebc0`), float export
    `DAT_007f0f8c` (`FSTP`, `0x0047ebed`), build flag `DAT_0086caa0` cleared (`0x0047eba7`).
  - one physics global `0x00913284` (`FUN_00560260`, 0x56-band; single write).
- **Not integrator leakage:** the transitive closure (261 funcs) also reaches input/joypad
  globals (`0x00773xxx`, `0x008aa6xx`, `0x008ab6cc` via `FUN_004a3258`/`004ac45c`/`004aa*`) and a
  CRT/system write (`0xffdff000` via `FUN_004a5984`). These are **off-path conditional helper
  branches** (input/mode/CRT), not the physics integrator, and do not touch body/world state.
  ⇒ The integrator + solve step mutate only the 4 proxy bodies, the world/body-table, and the
  vehicle readback — no hidden cross-subsystem physics writes. `[C1-static]`.

## PD-coupling law constants observed in the driver (for B5d, not B5a scope)
`_DAT_005ccd6c` (PD position gain), `_DAT_005cf014`, `_DAT_005cc33c`/`_DAT_005cc320` (clamp band),
`_DAT_005cc98c`, `_DAT_005cd09c`/`_DAT_005ccac4` (angle wrap 2π family), timestep `DAT_0061331c=0.05`.
Full law is in `vehicle_coupling.md`; not re-derived here.

## B5a status: **DONE.** Target list is complete + caller-closed for B5b (vendor qhull; sole entry
`FUN_0057ca30`) and B5c (port the pointer-indirect integrator + the `FUN_0047ea40→0047e9c0` solve;
4-body state at `0x007e9de0` stride 0x404). OU#3 CLOSED, OU#4 ISOLATED. No behavioral claims (B5e).
