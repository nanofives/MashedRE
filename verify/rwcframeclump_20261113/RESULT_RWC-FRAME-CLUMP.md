# RESULT RWC-FRAME-CLUMP (partial, HANDOFF at L0 STOP run 43)

Run 43, 2026-11-13. Branch `orch/RWC-FRAME-CLUMP`, based on `orch/integration` 69cd03777. Top-down row. Stopped by the
L0 STOP before the closing arms. **Verdict: HANDOFF.** The cup.dff clump read on the core is fixed, and the boot gets
past every clump read in AppInitialiseOnBootup's TXD / DFF loads. It now stops later, in the powerup init (section 3).

**Self-test: not run on the committed tree.** The last full boot (`d6`) ran the RWC_CORE path up to an AV in the
powerup init, before any self-test line. The RwTexDictionaryCreate binding self-test and RWC-DEVICE self-test of
RWC-FRAME-492E90 were not re-run. Not run: `ddkobj.py check`, the object build, vs_off / ddk_off / vs_frame /
ddk_frame. G-CTRL / B / 12 captures: not run (top-down row, knob off for those hashes).

## 1. Root cause of the AV at EIP 1 (RWC-FRAME-492E90 section 5, item 3)

One defect class, found in five executed provisional sources. Ghidra splits a stack buffer into separate scalar
locals. The source passes the first one's address to a reader (or clears / copies through a pointer from it), and the
port's VS2022 frame does not keep the pieces contiguous, so the read overruns the frame. The symptoms were a `ret` into
a stack value (EIP 1, [esp] = 0x37002 = the stream version) and a stack-cookie failure (0xC0000409). Each fix was found
by a boot with a temporary log trace in 0x004e7420 (removed before commit) and checked against the original's listing.

| rva | function (RW) | buffer in the original | fix | score before / after |
|---|---|---|---|---|
| 0x004e7420 | RpClumpStreamRead | 12-byte header at [esp+0x48] (0x004e7493) | rewritten from the disassembly (section 2) | 37.9 (ddk) / 52.3 (rw9044) |
| 0x004e9330 | RpGeometryStreamRead | 0x10 header [esp+0x18] (0x004e9377), 0x18 morph target [esp+0x28] (0x004e953b) | `int hdr[4]`, `int mt[6]` | 51.5 / 52.2 |
| 0x004e7060 | atomic stream read | 16-byte struct [esp+0x20], zeroed 0x004e70a8..0x004e70bb, read 0x004e70bf | `int ai[4]` | 39.5 / 49.8 |
| 0x0053fec0 | material texture-pair installer | 32-byte name [esp+0x18] (0x0053ff84 / 0x0053ff89 rep stosd) | `char local_20[32]` | 50.9 / 67.3 |
| 0x004e8120 | RpMaterialStreamRead | 7-dword struct [esp+0x1c] (0x004e816c rep stosd x7, read 0x004e8179) | `int local_1c[7]` | 50.2 / 53.8 |

All five stay provisional (L-126), not C4. `split_local_scan.py` (this folder) lists 80 candidates of this shape in
30 provisional sources of the closure. Most are separate size / version out-parameters and harmless. They were not
all checked.

## 2. 0x004e7420 source

`re/match/rwl02/004e7420.c` is rewritten from the disassembly under `TOOLCHAIN rw9044`: the original's 0x44 frame,
one RwError per error site, the geometry list read as an inlined helper (rw9044 inlines it and reuses the stream
parameter home for the count, as the original does at 0x004e76f0), the geometry list free inlined at five sites and
called (0x004e7010) at two, as in the original. Same call sequence as the original on every path. Residual (in the
source header): the ebx / edi swap (zero / counter vs stream), and the original's unfolded `lea eax,[esp+0x28]; test`
at 0x004e77d2 with the fail block placed first. `equiv.py`: NOT-PROVEN call-regargs (the register swap is visible to
callees). The same source with all six frees inlined scores 63.2%.

## 3. Boot progress (vs_frameN, MASHED_RWC_FRAME_N=120, private root D:/MashedWT/RWC-FRAME-CLUMP_root)

| build | where it stopped |
|---|---|
| vs1 (new 0x004e7420 only) | AV EIP 1, now inside 0x004e9330 (the clump exists) |
| diag3 (+ 0x004e9330, 0x004e7060) | AV in 0x0053fec0 <- lifted 0x0053f3c0 <- 0x004e1b60 <- 0x004e8120 |
| d4 (+ 0x0053fec0) | 0xC0000409 stack cookie in the material read |
| d5 (+ 0x004e8120) | **cup.dff and Tri.dff clump reads return** (1 geometry, 1 atomic). AV in `Orig_0048fef0` from `PortTop_TriPool_Call0048ff20`: the librw atomic view applied to a core atomic |
| d6 (+ `MashedRw_AtomicView_0048fef0` returns the atomic on the core) | past the tri pool. AV in `RwCore_004e69a0` (RpAtomicClone) from the powerup init `Orig_004548e0`: the atomic comes from `MashedRw_ClumpFirstAtomic_004b3f90`, a librw binding run on a core clump; `MashedRw_AtomicPartCode_004b5190` also ran on core atomics |
| d7 / d8 (+ game-side helpers on the core, section 4) | d7 rva-lint 3 ORIGTOP-ONLY pairs (allowlisted); d8 build OK, **boot not run** (L0 STOP) |

## 4. Game-side RW helpers on the core (not yet booted)

`gen_rwcore_tus.py` takes 0x004b3f90, 0x004b5190, 0x004b52f0, 0x004b5300 as roots (game RVAs followed like RW code,
with 0x004b4070, 0x004b52c0, 0x0055deb0). `route_bindings.py` then routes their four bindings to the core bodies
(`[RWC-FRAME]` guards, routed 71 -> 75). New sources in `re/match/rwfc/`, all 100% BYTE-IDENTICAL with relocheck OK:
0x004b3f90 and 0x004b4070 (ddk /O2 /Ob1 /G5), 0x004b5300 (ddk /O2 /GS /Ob1), 0x0055deb0 (rw9044). `match_ledger.tsv` has
no rows for them, so `RwCore_Bodies.tsv` labels the three game ones "provisional" with no score until the ledger is
rebuilt. `MashedRw_AtomicView_0048fef0` returns its argument under the frame route (no librw use, no new binding).
L-124 holds: no librw binding added.

Promotions: `re/promotions/RWC-FRAME-CLUMP.tsv` = 0x004b3f90 route match (dry run: 1 pass). 0x004b4070 and 0x004b5300
were refused by the dry run because they have no hooks.csv row (C1 by identity first, through re-classify). 0x0055deb0
is already C4.

## 5. Next step

1. Boot d8's tree (`bash verify/rwcframeclump_20261113/iter.sh <tag>`) and continue the crash-driven loop from the
   powerup init. Each stop so far was either a split-local source or a librw binding without a frame-route branch.
2. Then the closing arms: `run_arms.sh vs_off vs_frame vs_frameN`, the object build with `ddk_off ddk_frame
   ddk_frameN`, `ddkobj.py check`, and the self-test lines. Last, the draw compare once `RwCore_00492e90` runs.
3. Strict members (L-144) still open on this path: 0x004e7420 52.3, 0x004e9330 52.2, 0x004e7060 49.8, 0x004e8120 53.8,
   0x0053fec0 67.3, 0x004e8c70 28.9, 0x004cc5e0 59.8, 0x005c47e0 80.0. Lifted executed members (L-141 says they need
   a source): 0x004f3e90 (material list read), 0x0053f3c0.

## 6. Raw data

`D:/MashedWT/RWC-FRAME-CLUMP_data` (build logs, vs_frameN logs per iteration; `vs_frameN_d8.log` is a copy of d6, d8
did not boot), sha256 in `RAW_SHA256_RWC-FRAME-CLUMP.txt`. Every game launch went through `test-slot` ->
`mashed_lock.py run`. My own test-slot waiter PIDs for the unrun d8 boot (3940, 17172, 20408) were stopped at the STOP.
The committed tree is d8's tree minus the temporary trace in 0x004e7420. The clean 0x004e7420 source was built (vs1)
but this exact tree was not rebuilt after removing the trace.
