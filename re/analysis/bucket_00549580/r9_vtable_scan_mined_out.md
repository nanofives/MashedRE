# area-render r9 — Systematic RW vtable/fn-ptr-table scan

Date: 2026-09-01. Tool: XrefRange.java on Mashed_pool3. Scope: enumerate every RW
function-pointer table reachable in the global data segment, classify each slot as
pure-CPU-backed vs device/allocator/live-state-backed, find pure-CPU slots not yet at C3.

## Method

XrefRange.java scanned three data ranges for WRITE operations (slot assignments):

| range | refs | static (imm) fn-ptr writes | register-based writes |
|---|---|---|---|
| 0x007d3ec8–0x007d4200 | 2992 | **4** (0x007d3fe0: 0x4c35b0/0x4cca80; 0x007d3fe4: 0x4c35d0/0x4ccba0) | 47 unique slots |
| 0x007d4160–0x007d5800 | 678 | 0 | all register-based |
| 0x007d5800–0x007d7500 | 1347 | 0 | all register-based |

## Classification

### Main RW device table (0x007d3ec8–0x007d4200)

**4 static assignments decoded (all NOT pure-CPU):**

| slot | address | fn stored | verdict |
|---|---|---|---|
| +0x118 from 0x007d3ec8 | 0x007d3fe0 | 0x004c35b0 | thunk: load first field of arg, JMP *(DAT_007d3ff8+0x108) — dispatches through device alloc slot; NOT pure CPU |
| +0x118 from 0x007d3ec8 | 0x007d3fe0 | 0x004cca80 | MISS in Ghidra (no function defined), linked-list alloc body; NOT pure CPU |
| +0x11c from 0x007d3ec8 | 0x007d3fe4 | 0x004c35d0 | thunk: CALL *(DAT_007d3ff8+0x10c) — dispatches through device free slot; NOT pure CPU |
| +0x11c from 0x007d3ec8 | 0x007d3fe4 | 0x004ccba0 | FreeList item free: bitmap bit-clear + conditional block release; calls *(DAT_007d3ff8+0x10c); NOT pure CPU |

**Slots +0x118/+0x11c function:** freelist alloc/free callbacks that dispatch through the RW device vtable alloc (+0x108) and free (+0x10c) slots. Already DEMOTED (D-11016) and in parent's booted lane.

**All other register-based writes in main table:**

| writer | slots | classification |
|---|---|---|
| FUN_004c7a70 | 0x007d40b0–0x007d40bc, 0x007d4100–0x007d410c | D3D9 device installer: raster lock/unlock/copy/destroy + alloc/free trampolines |
| FUN_004c8650, 004c9ad0, 004caea0, 004cb190 | 0x007d40c0–0x007d40d8 | Raster type registrations: per-type constructor/destructor callbacks, D3D9-backed |
| FUN_004c8c70 | 0x007d4110, 0x007d4120, 0x007d4128, 0x007d4130, 0x007d4144, 0x007d4148, 0x007d414c | D3D9 device teardown: confirmed per decomp (calls IDirect3DDevice9 methods) |
| FUN_004c8800 | 0x007d4124 | D3D9 state init |
| FUN_004c9f50, 004c9f60 | 0x007d4134, 0x007d413c | Conditional setters on D3D9 mode state |
| FUN_004c9cd0, 004fb0e0 | 0x007d4140, 0x007d4150 | D3D9 global state |
| (none) sites 004c3e84..004c79e6 | 0x007d3ffc, 0x007d4000, 0x007d4028, 0x007d402c, 0x007d4054, 0x007d4058, 0x007d4080, 0x007d40a8, 0x007d40ac | Anonymous code within RW engine init body — all D3D9/device context slots |

**MOVSD.REP bulk copy:** FUN_004c3040 (site 0x004c307a) and FUN_004c30b0 (site 0x004c31cd) copy
`*(DAT_007d3ff8)` (runtime device structure) into DAT_007d3ec8 at engine restart. Source is
RUNTIME-filled, not a static initializer — no statically-encoded function pointers discovered.

### Gap range (0x007d4160–0x007d5800): D3D9 device state, allocator, freelist

Writers: FUN_004c7a70 (D3D9 device), FUN_004c8690/9ad0/aea0 (raster types),
FUN_004cc820/cce20/ccf20 (allocator/freelist), FUN_004cfe20/fe40 (RW subsystem teardown).
**All D3D9 or allocator-backed. 0 static fn-ptr assignments.**

### Plugin/render-state range (0x007d5800–0x007d7500): D3D9 dirty-queue setters

From hooks.csv, all writers in this range are D3D9 render-state caching functions
(FUN_004d5bc0 RS cold init, FUN_004d6200 RS reset, FUN_004d55b0 material setup,
FUN_004d6ce0 SetTexture binder, etc.). Many already at C3 (D3d9State_Flush,
SetRenderState setters). **0 static fn-ptr assignments. All D3D9-backed.**

## Verdict: RENDER MINED-OUT on the synthetic vtable-scan lane

**ZERO pure-CPU slots exist** in any of the three RW table ranges that are not already C3
or explicitly D3D9/allocator-backed. The r8 discriminator, applied systematically:

- The ONLY pure-CPU vein in the render function-pointer tables was the RW string vtable
  (DAT_007d3ff8 slots +0xc4..+0x104: strncpy/strlen/stricmp/strupr/strlwr/strchr/strrchr),
  already exhausted in rounds r3–r6 (7 C3s: 004d8680/86d0/8700/8730/8750 + 004c5ae0/4c5b50).
- Every other slot in the surveyed 0x007d3ec8–0x007d7500 range is one of:
  (a) D3D9 device-backed (raster lock/unlock/copy/destroy, device vtable dispatch)
  (b) Allocator/heap-backed (freelist alloc/free, calls RW vtable +0x108/+0x10c)
  (c) Live render-state management (dirty-queue setters, D3D9 state caching)

Two consecutive dry rounds (r8 + r9 = dry #1 + dry #2) confirm MINED-OUT.
Remaining ~739 render residue are all in the parent's booted-race lane.
