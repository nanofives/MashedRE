I now have everything needed. Verified facts: the class-C trigger (column-0 deref initializer) returns **zero matches** across all of Render/ and Rws/, so there are **no class-C files**. All 37 unlinked files are class B. Here is the triage.

---

# D0.7 Triage Part 3 ÔÇö Render/ and Rws/ unlinked `.cpp`

## Scope resolution (from `build.bat` + glob)

- **Render/**: 53 `.cpp` total; 17 on the exe list (build.bat:329-350: BatchAB_s1/s3/s6, RenderLeaves_ae1/ae2/ae3, PaletteQuantizer, RenderStateSettersA, CameraProjCoeffs, SlotObjectAccessors, StateBatchGetters, ParticleEmitterCtors, GlobalByteQuad, GlobalByteQuadAB, RwPluginLinkSet, RwPluginLinkSetAB, Vec3NormalizeScale) ÔåÆ **36 unlinked**.
- **Rws/**: 2 `.cpp`; RwsChunkWalker linked (build.bat:162) ÔåÆ **1 unlinked** (RwsStreamRead).
- **In-scope total: 37 files.**

## Method notes / interpretation calls (settled, stated for the record)

1. **RVA-free = the function *body* holds no MASHED absolute address.** The `RH_ScopedInstall(fn, 0xRVA)` registration argument is **excluded** (integer to the `HookSystemNoOp` stub, never dereferenced ÔÇö finding 1). Confirmed against `Save/FsOpen.cpp` (a known RVA-free/functional Batch-1 file) which carries `RH_ScopedInstall(FsopenSafe, 0x004a4541)` yet has a bare body.
2. **Class-C = zero.** `Grep ^[A-Za-z_].*=\s*\*` over Render/ ÔåÆ *no matches*; `^[A-Za-z_].*=\s*\*` over Rws/RwsStreamRead ÔåÆ *no matches*. No file-scope (column-0) initializer dereferences an absolute address anywhere in scope. The `static ÔÇª const s_X = reinterpret_cast<ÔÇª>(0xRVA);` lines that do occur are **binds without deref** (finding-2 shape) ÔÇö load-safe.
3. **Class A = zero.** Every unlinked file is a real reversed reimpl (some with disabled/deferred `RH_ScopedInstall`), not self-test or `.asi`-only harness code.
4. **hooks.csv file-column quirk** (memory `tracker-schema-gotchas`): the `batch_v`/`wf` pixel-leaf rows put the **analysis `.md` in the file column, not the `.cpp`** ÔÇö I resolved those by RVA (e.g. 004dfa40 ÔåÆ C3). C4 counts were cross-checked against subsystem so mixed-subsystem PromoLoop files weren't missed.

---

## (1) Per-file table

`RVA-free` column: YES/NO + approx count of body absolute-address occurrences (excluding comments and the ScopedInstall arg). All class = **B**; blocking-construct column is empty because class-C count is 0.

| # | File | RVA-free? (count / examples) | C4 | C3 | Cross-TU dep |
|---|------|------|----|----|----|
| **RVA-FREE (functional standalone)** ||||||
| 1 | Rws/RwsStreamRead.cpp | **YES** (0; param-rel only, l.47-50) | 0 | 1 | none |
| 2 | Render/BgraReorder_wf.cpp | **YES** (0; l.53-56) | 0 | 1 | none |
| 3 | Render/PixReadU32_wf.cpp | **YES** (0; l.58-61) | 0 | 1 | none |
| 4 | Render/PixPassthrough16.cpp | **YES** (0; l.44) | 0 | 1 | none |
| 5 | Render/RgbPackEncoder_wfb0f.cpp | **YES** (0; l.55-57) | 0 | 1 | none |
| 6 | Render/RwAtomicDirtyFlag.cpp | **YES** (0; param-rel l.27) | 0 | 1 | none |
| 7 | Render/RwComReleaseThunk.cpp | **YES** (0; param-rel l.32-34) | 0 | 1 | none |
| 8 | Render/StoreEaxAtEcx.cpp | **YES** (0; naked `mov [ecx],eax`) | 0 | 1 | none |
| 9 | Render/ElemArrayCopyAll.cpp | **YES** (0; param-rel l.42) | 0 | 1 | `RenderElemArrayCopy` ÔåÆ **RenderLeaves_ae2.cpp (LINKED Ô£ô)** |
| 10 | Render/RwStreamWriteBytes.cpp | **YES** (0; l.61-66) | 1 | 0 | `RwStreamWrite_s2` ÔåÆ **Save/RwStream.cpp (UNLINKED Ô£ù)** |
| 11 | Render/PromoLoop_round22.cpp | **YES** (0; pure x87 math l.49-56) | 0 | 1 | none ÔÇö *hook `RH_ScopedInstall` commented-out (deferred r23, x87 bit-identity); possible dup-symbol w/ PromoLoop_sessionB.cpp per tracker note* [UNCERTAIN] |
| **RVA-TUNNELLED (load-safe, dead/AV-if-called)** ||||||
| 12 | Render/PerModeRender.cpp | NO (~10; l.119,127,225) | 1 | 0 | callees via RVA ptrs (no C++ dep) |
| 13 | Render/RwPluginHelpers_o3.cpp | NO (~4; l.57,87,155) | 1 | 3 | none |
| 14 | Render/RenderSubmit_o4.cpp | NO (~5; l.61,133,144) | 1 | ~3 | none (1 hook disabled) |
| 15 | Render/D3D9Helpers_q5.cpp | NO (1: 0x00499720 l.151) | 1 | ~1 | `RwStreamWrite_s2` ÔåÆ **Save/RwStream.cpp (UNLINKED Ô£ù)** |
| 16 | Render/PromoLoop_round3.cpp | NO (~6; l.54,80,103) | 1 | 4 | none (mixed vehicle/ai/render) |
| 17 | Render/PromoLoop_round11.cpp | NO (~4; l.49,73,122) | 1 | 3 | none (mixed) |
| 18 | Render/RwPluginHelpers_q1.cpp | NO (~4; l.71,108,195) | 0 | 4 | none |
| 19 | Render/TrackNodeLeaves_o1.cpp | NO (~12; l.76,125,415) | 0 | ~6 | none |
| 20 | Render/RenderStateSettersB.cpp | NO (~30; l.81,182,214) | 0 | 5 | none |
| 21 | Render/D3D9Helpers_p4.cpp | NO (~6; l.68,75,77) | 0 | 4 | none |
| 22 | Render/FrameHelpers_q2.cpp | NO (~15; l.63,140,219) | 0 | 5 | none |
| 23 | Render/LowRvaMixed_q3.cpp | NO (~8; l.72,316,330) | 0 | 5 | callees via RVA ptrs |
| 24 | Render/LowRvaSetters_o2.cpp | NO (~15; l.97,192,324) | 0 | ~4 | callees via RVA ptrs (1 hook disabled) |
| 25 | Render/TextureLoader_q6.cpp | NO (~8; l.254,257,264) | 0 | ~2 | none |
| 26 | Render/TextureLoaderCluster.cpp | NO (~10; l.59,67,151) | 0 | ~? | none |
| 27 | Render/D3d9StateCache.cpp | NO (~9; l.52,71,256) | 0 | ~4 | none |
| 28 | Render/HighAB3Helpers_p6.cpp | NO (~5; l.77,134,135) | 0 | 3 | none |
| 29 | Render/FrameWorldPasses.cpp | NO (~8; l.83,91,141) | 0 | 2 | callees via RVA ptrs |
| 30 | Render/TrackLoaderMicros_p3.cpp | NO (~5; l.53,85,215) | 0 | ~5 | none |
| 31 | Render/PromoLoop_round16.cpp | NO (~10; l.67,79,100) | 0 | 1 | vtable via param |
| 32 | Render/PluginFields_ah4.cpp | NO (~5; l.29,37,61) | 0 | ~1 | none (4 hooks DEFER arg-shape) |
| 33 | Render/RpMaterialNibble_wf1.cpp | NO (1: 0x00911ae4 l.20,59) | 0 | ~1 | none |
| 34 | Render/RwString.cpp | NO (2: 0x007d3ff8,0x005d8d70 l.46,48,60) | 0 | 1 | none |
| 35 | Render/RainCameraScale.cpp | NO (2; l.53-54) | 0 | 1 | none |
| 36 | Render/RainLineWidthRange.cpp | NO (2; l.44-45) | 0 | 1 | none |
| 37 | Render/MixedC3Sweep.cpp | NO (~7; l.113,159,187) | 0 | 0 (C2; hook disabled) | none |

C3 counts for tunnelled files marked `~` are best-effort (function-count / tracker-cross-ref); the `.md`-vs-`.cpp` file-column split makes an exact per-`.cpp` C3 tally unreliable for those, whereas **all C4 counts and all RVA-free-file counts above are verified exact**.

---

## (2) ORDERED ADD-BACK LIST ÔÇö RVA-free class-B only (link now)

These have **no MASHED address in any code path** ÔåÆ functional standalone, not dead exports. Ranked C4 ÔåÆ C3.

**Immediately linkable (no unlinked cross-TU dep):**

1. **Render/ElemArrayCopyAll.cpp** ÔÇö C3 ÔÇö cross-TU on `RenderElemArrayCopy`, already satisfied by linked `RenderLeaves_ae2.cpp`. Safe to add.
2. **Rws/RwsStreamRead.cpp** ÔÇö C3
3. **Render/RwAtomicDirtyFlag.cpp** ÔÇö C3
4. **Render/RwComReleaseThunk.cpp** ÔÇö C3
5. **Render/StoreEaxAtEcx.cpp** ÔÇö C3 (naked; register-ABI)
6. **Render/RgbPackEncoder_wfb0f.cpp** ÔÇö C3
7. **Render/BgraReorder_wf.cpp** ÔÇö C3
8. **Render/PixReadU32_wf.cpp** ÔÇö C3
9. **Render/PixPassthrough16.cpp** ÔÇö C3

**Highest value but ORDER-CONSTRAINED (link only after its dependency):**

10. **Render/RwStreamWriteBytes.cpp** ÔÇö **C4** ÔÇö RVA-free body, **but** its only callee `RwStreamWrite_s2` is defined in **Save/RwStream.cpp (unlinked, out of this scope)**. Linking it before Save/RwStream.cpp ÔåÆ unresolved-external link error. It ranks first by C-level yet must be sequenced **after Save/RwStream.cpp is added** (which itself needs its own RVA-free audit ÔÇö Save is Part-N's scope).

**Excluded from immediate add-back (RVA-free but not ready):**

- **Render/PromoLoop_round22.cpp (Vec3Lerp, C3)** ÔÇö RVA-free and correct as a standalone function, but its `RH_ScopedInstall` is commented out (deferred r23 for x87 bit-identity ÔÇö a *Frida-diff* concern, irrelevant to standalone linking) and the tracker names a second reimpl `Lerp4b4650` in `PromoLoop_sessionB.cpp`. **[UNCERTAIN ÔÇö potential duplicate symbol]**; resolve the dup before linking.

---

## (3) Load-safe-but-RVA-tunnelled BACKLOG (porting work, not a list edit)

Ranked by C4 rows unlocked. Each becomes a dead export whose body AVs if called (exe based at 0x10000; bodies read 0x004xxxxxÔÇô0x009xxxxx). Neutralizing the RVA tunnels is required first.

1. **PerModeRender.cpp** ÔÇö C4 (canonical 808 B hot-path dispatcher `PerModeRenderMachine`) ÔÇö highest-value port.
2. **RwPluginHelpers_o3.cpp** ÔÇö C4 + 3├ùC3 (RW driver-system wrappers).
3. **RenderSubmit_o4.cpp** ÔÇö C4 + ~3├ùC3.
4. **PromoLoop_round3.cpp** ÔÇö C4 + 4├ùC3.
5. **PromoLoop_round11.cpp** ÔÇö C4 + 3├ùC3 (constant-VA getters ÔÇö return raw MASHED addresses).
6. **D3D9Helpers_q5.cpp** ÔÇö C4 ÔÇö *double-blocked*: RVA-tunnelled (0x00499720) **and** cross-TU on `RwStreamWrite_s2` (unlinked Save/RwStream.cpp).
7. C3-only tunnelled (no C4): RwPluginHelpers_q1, RenderStateSettersB, TrackNodeLeaves_o1, D3D9Helpers_p4, FrameHelpers_q2, LowRvaMixed_q3, LowRvaSetters_o2, D3d9StateCache, TrackLoaderMicros_p3, HighAB3Helpers_p6, TextureLoader_q6, TextureLoaderCluster, FrameWorldPasses, PromoLoop_round16, PluginFields_ah4, RpMaterialNibble_wf1, RwString, RainCameraScale, RainLineWidthRange.
8. **MixedC3Sweep.cpp** ÔÇö lowest (C2, `RwFreeListCreate` hook mass-disabled, carries [UNCERTAIN U-4420/U-4421]).

---

## (4) Counts per class

| Class | Count | Notes |
|-------|-------|-------|
| **A** ÔÇö unlinked-by-intent / harness | **0** | none found; all are real reimpls |
| **B** ÔÇö safe-to-add (no file-scope deref) | **37** | = 11 RVA-free + 26 RVA-tunnelled |
| **C** ÔÇö blocked (file-scope absolute deref) | **0** | confirmed: column-0 `= *` grep empty across Render/ + Rws/ |

**Of the 37 class-B:**
- **11 RVA-free / functional** (10 linkable now ÔÇö 1 immediately-linkable cross-TU ElemArrayCopyAll included; RwStreamWriteBytes C4 order-constrained; Vec3Lerp held for dup/deferral).
- **26 RVA-tunnelled / load-safe-only** (6 carry C4 rows; PerModeRender is the standout hot-path).
- **Cross-TU into unlinked files: 2** (RwStreamWriteBytes, D3D9Helpers_q5 ÔÇö both need Save/RwStream.cpp). **1** cross-TU into a linked file (ElemArrayCopyAll ÔåÆ RenderLeaves_ae2.cpp, satisfied).

Net actionable finding: the D0.7 add-back yield from Render/Rws is **9 files linkable immediately** (all C3), plus **1 C4** (RwStreamWriteBytes) gated on Save/RwStream.cpp ÔÇö consistent with the build.bat:129-140 thesis that add-backs are gated on *no MASHED address in any code path*, not on booting.
