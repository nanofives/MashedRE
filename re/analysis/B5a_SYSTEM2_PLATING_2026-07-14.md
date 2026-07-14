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
