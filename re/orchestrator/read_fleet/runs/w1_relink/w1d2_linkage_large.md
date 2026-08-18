I now have everything needed. Compiled findings below.

---

# ROADMAP D0.7 triage ÔÇö part 2: `Util/` + `Audio/` unlinked-source classification

## Method & the load-time model (read this first ÔÇö it reframes every verdict)

- **Scope confirmed from `build.bat` + `asi_sources.rsp`:** `Util/` = 84 `.cpp`, 12 linked into `mashed_re.exe` (`build.bat:268-279`), **72 unlinked**. `Audio/` = 25 `.cpp`, 4 linked (`build.bat:201,202,315,324` = `RwsBank`, `AudioEngine`, `AudioQueuePop`, `AudioVecLength`), **21 unlinked**. Total in-scope: **93 unlinked**.
- **`call_original_0xÔÇª`: ZERO matches** anywhere under `mashed_re/`. That trigger is empty for this whole codebase.
- **`RH_ScopedInstall` is a FALSE trigger here ÔÇö do not treat it as class C.** Every one of the 84 Util + 25 Audio files contains it, *including all 12 Util + 4 Audio files already linked into the booting exe*. `Core/HookSystem.h:37-43` shows the macro expands to a file-scope static object whose ctor calls `HookSystem::Register(RVA, &Method, name)` ÔÇö it passes the RVA as an **integer value** and takes the address of **our own** function; it **never dereferences a MASHED address**. In the exe, `Register` is the no-op from `Stubs\HookSystemNoOp.cpp` (`build.bat:212`). So file-scope `RH_ScopedInstall` cannot cause `STATUS_DLL_INIT_FAILED`. The empirical proof is that `Util/UtilLeaves.cpp` (linked, boots) has the identical shape.
- **The real class-C trigger is a file-scope initializer that *dereferences* an absolute MASHED address at load** ÔÇö the `static const GUID g = *reinterpret_cast<const GUID*>(0x005d09dc)` shape build.bat:101 names as the boot-crash root cause. The discriminator is a leading `*` on a column-0 initializer.
- **Column-0 (file-scope) RVA-construct sweep result:** `Util/` = **zero** file-scope RVA constructs (every RVA touch is inside a function body ÔåÆ runtime-only ÔåÆ boot-safe). `Audio/` = exactly two offenders: `AudioDSound.cpp` (deref-at-load) and `AudioRws.cpp` (bind-only).
- **Architectural caveat that dominates the whole exercise:** because `HookSystem` is no-op'd in the standalone, these reimpls are **never invoked** in `mashed_re.exe` ÔÇö linking one adds a *dead exported symbol*. Any function that runtime-derefs a MASHED global (0x00400000-0x00b60000, unmapped in the standalone since the exe bases at 0x10000, `build.bat:330`) would fault **only if called**. So "SAFE-TO-ADD" here means "will not crash boot," **not** "functional." "C4 rows unlocked" = compiled-in and available to wire up, not executing.

---

## (1) Family table ÔÇö the repeated `Util/` shape

| Family | Members | Class | Shared blocking construct | Deviating members |
|---|---|---|---|---|
| **PromoLoop** (`PromoLoop_round{8,9,10,15,17,18,20,21,24-35,37-51,53-78,80}.cpp` + `PromoLoop_sessionB.cpp`) | **63** | **B** (all) | **None.** Uniform shape: `#include "../Core/HookSystem.h"` + `<cstdint>`; `extern "C" __declspec(dllexport) __cdecl` leaf getters/setters; all RVA derefs **inside** function bodies (runtime); file-scope `RH_ScopedInstall` only (proven inert). Verified on `round8.cpp` (full read) and by the column-0 grep finding **0** file-scope RVA constructs across all of `Util/`. | **None deviate in class.** Structural outliers only: `PromoLoop_sessionB.cpp` is a ~4,400-line mega-bucket (~130 functions incl. most of the family's C4s) vs the small round files; `round53/54/56.cpp` add file-scope `static inline` helpers (`StridedClear`, `VehTblGet`, `Idx2WheelGet`) ÔÇö still no RVA init, still B. |

PromoLoop family confidence weight: **27 C4** + the bulk of the Util C3 rows.

---

## (2) Per-file table ÔÇö everything not in a family

Unlinked non-family `Util/` (9) + all unlinked `Audio/` (21). C4/C3 from `hooks.csv`.

### Util (non-family)
| File | Class | Blocking construct (file:line) | C4 | C3 |
|---|---|---|---|---|
| `Util/UtilBatch.cpp` | **B** (boot) ÔÜáthunk | none at load; runtime-calls MASHED fn-ptrs (`UtilBatch.cpp:138,173-175,209-210,242-243`, e.g. `kPrintfFn_VA`, gates `0x0041ea40`) | **7** | 0 |
| `Util/TimeRecordWrite_ag.cpp` | **B** | none | 0 | 1 |
| `Util/RtFSHandler.cpp` | **B** | none | 0 | 1 |
| `Util/RtFSHandlerCancel.cpp` | **B** | none | 0 | 1 |
| `Util/UtilMid.cpp` | **B** | none (all derefs in-function; file-scope items are plain `struct`/data tables of address *values*, no deref) | 0 | 0* |
| `Util/CameraEntryDispatch.cpp` | **B** | none | 0 | 0* |
| `Util/UtilRandIntRange_wfb0f.cpp` | **B** | none | 0 | 0* |
| `Util/Vec3ValidateMixed_cfd622.cpp` | **B** | none | 0 | 0* |
| `Util/MixedC3Sweep.cpp` | **B** | none | 0 | 0* |

\* **[UNCERTAIN]** these show 0 rows under their own `hooks.csv` path. UtilMid especially is a large multi-function file, so its rows are likely tracked under stale/alternate paths or `sub_RVA` names elsewhere ÔÇö I report the literal grep result, not an inferred count. Also **`Util/MixedC3Sweep.cpp` is an orphan**: present on disk but in **neither** the exe list **nor** `asi_sources.rsp` (only `Audio\MixedC3Sweep.cpp` is in the rsp, line 155) ÔÇö it compiles into no target today.

### Audio (all unlinked)
| File | Class | Blocking construct (file:line) | C4 | C3 |
|---|---|---|---|---|
| `Audio/AudioDSound.cpp` | **C ÔÇö CONFIRMED boot crash** | `AudioDSound.cpp:95` `static const GUID s_IID_005d09dc = *reinterpret_cast<const GUID*>(0x005d09dc);` and `:96` same for `0x005d09bc` ÔÇö file-scope deref of MASHED addresses at CRT dynamic-init ÔåÆ `STATUS_DLL_INIT_FAILED`. Exact build.bat:101 shape. | 0 | 8 |
| `Audio/AudioRws.cpp` | **C ÔÇö flagged** ([UNCERTAIN] on boot) | `AudioRws.cpp:477,481,485,490` file-scope globals **bound** to MASHED RVAs (`0x005aea00`, `0x004522d0`, `0x005ae920`, `0x007ddab0`). Matches your "global bound to a MASHED RVA at load" trigger. **Mechanism note:** bind-only (no leading `*`) ÔåÆ stores an integer ÔåÆ will **NOT** itself crash the loader; the hazard is a foreign-address **call** at runtime + it thunks to the original RW-audio engine. Classed C conservatively. | 0 | 20 |
| `Audio/AudioMemory.cpp` | **B** | none (`:27` is an in-function cast of a local) | **1** | 4 |
| `Audio/AudioLeaves_ab1.cpp` | **B** | none | 0 | 6 |
| `Audio/AudioLeaves_ab3.cpp` | **B** | none | 0 | 6 |
| `Audio/AudioLeaves_ab4.cpp` | **B** | none (all derefs off `param_1`) | 0 | 6 |
| `Audio/TimerGetters_ah5.cpp` | **B** | none | 0 | 7 |
| `Audio/RwsStream.cpp` | **B** ÔÜáthunk | none at load; in-function fn-ptr thunks (`kStreamRead` etc.) | 0 | 4 |
| `Audio/AudioList_ag.cpp` | **B** (cleanest ÔÇö param-only) | none | 0 | 4 |
| `Audio/AudioPoolFreeGuards.cpp` | **B** | none | 0 | 3 |
| `Audio/AudioTableDispatch.cpp` | **B** | none | 0 | 2 |
| `Audio/RwsFmt.cpp` | **B** ÔÜáthunk | none at load; in-function `kKeyCmp` thunk | 0 | 2 |
| `Audio/AudioMusic.cpp` | **B** | none | 0 | 1 |
| `Audio/AudioCharacterBankPaths.cpp` | **B** | none | 0 | 1 |
| `Audio/AudioChainWalk.cpp` | **B** | none | 0 | 1 |
| `Audio/RingHeaderInit.cpp` | **B** | none | 0 | 1 |
| `Audio/AudioLeaves_ab2.cpp` | **B** | none | 0 | 1 |
| `Audio/AudioListSearch_wfb0f.cpp` | **B** | none | 0 | 0 |
| `Audio/AtomicExchangeStore.cpp` | **B** | none | 0 | 0 |
| `Audio/AudioVoiceQueueSet.cpp` | **B** | none | 0 | 0 |
| `Audio/MixedC3Sweep.cpp` | **B** | none (derefs off `node_base`/`param`, in-function) | 0 | 0 |

Audio unlinked C3+C4 total = **78** (77 C3 + 1 C4), which matches your stated figure exactly ÔÇö validating the extraction.

---

## (3) Ordered add-back list ÔÇö class-B files by C4 rows unlocked (desc)

Only files with ÔëÑ1 C4 shown ranked; all remaining class-B files (0 C4, C3-only) are equally boot-safe to add below the cut. ÔÜá = boot-safe but runtime-thunks to original code (broken if the standalone ever calls it directly).

| Rank | File | C4 | C3 | Note |
|---|---|---|---|---|
| 1 | `Util/UtilBatch.cpp` | 7 | 0 | ÔÜá thunks to MASHED `printf`/gates |
| 2 | `Util/PromoLoop_sessionB.cpp` | 4 | ~60 | mega-bucket; also unlocks the most C3 |
| 3 | `Util/PromoLoop_round41.cpp` | 3 | 5 | |
| 4 | `Util/PromoLoop_round42.cpp` | 2 | 1 | |
| 5 | `Util/PromoLoop_round47.cpp` | 2 | 1 | |
| 6 | `Util/PromoLoop_round59.cpp` | 2 | 0 | |
| 7 | `Util/PromoLoop_round9.cpp` | 1 | 4 | |
| 8 | `Util/PromoLoop_round33.cpp` | 1 | 4 | |
| 9 | `Util/PromoLoop_round29.cpp` | 1 | 4 | |
| 10 | `Util/PromoLoop_round40.cpp` | 1 | 4 | |
| 11 | `Util/PromoLoop_round17.cpp` | 1 | 3 | |
| 12 | `Util/PromoLoop_round27.cpp` | 1 | 5 | |
| 13 | `Util/PromoLoop_round35.cpp` | 1 | 2 | |
| 14 | `Util/PromoLoop_round56.cpp` | 1 | 3 | |
| 15 | `Util/PromoLoop_round58.cpp` | 1 | 2 | |
| 16 | `Util/PromoLoop_round70.cpp` | 1 | 3 | |
| 17 | `Util/PromoLoop_round24.cpp` | 1 | 1 | |
| 18 | `Util/PromoLoop_round25.cpp` | 1 | 0 | |
| 19 | `Util/PromoLoop_round37.cpp` | 1 | 2 | |
| 20 | `Util/PromoLoop_round78.cpp` | 1 | 1 | |
| 21 | `Audio/AudioMemory.cpp` | 1 | 4 | only Audio file with a C4 |

Below the cut (0 C4, boot-safe class B, add for C3 coverage): the remaining ~44 PromoLoop rounds, the non-family Util leaves (`TimeRecordWrite_ag`, `RtFSHandler`, `RtFSHandlerCancel`, `UtilMid`, `CameraEntryDispatch`, `UtilRandIntRange_wfb0f`, `Vec3ValidateMixed_cfd622`, `MixedC3Sweep`), and the Audio leaves (`AudioLeaves_ab1/2/3/4`, `TimerGetters_ah5`, `AudioList_ag`, `AudioPoolFreeGuards`, `AudioTableDispatch`, `RwsStream`ÔÜá, `RwsFmt`ÔÜá, `AudioMusic`, `AudioCharacterBankPaths`, `AudioChainWalk`, `RingHeaderInit`, `AudioListSearch_wfb0f`, `AtomicExchangeStore`, `AudioVoiceQueueSet`, `MixedC3Sweep`).

---

## (4) Verdict on `Audio/` ÔÇö intent vs drift

**MIXED, but predominantly DRIFT for the leaf files, over a small genuine INTENT core.**

- **Genuine intent (must stay out as-is):** `AudioDSound.cpp` is a hard boot-crash blocker ÔÇö it dereferences the DirectSound COM IID GUIDs at `0x005d09dc`/`0x005d09bc` at load (`:95-96`), the exact pattern build.bat cites. `AudioRws.cpp` + the `ÔÜáthunk` files (`RwsStream`, `RwsFmt`, and `AudioRws`'s pool/alloc thunks at `0x005aea00`/`0x005ae920`/ÔÇª) depend on the original RenderWare-audio engine + DirectSound being mapped, which the standalone does not initialize. This part of the "wraps DirectShow/RW audio" story is real.
- **But 18 of 21 unlinked Audio files are class-B leaves** (`AudioLeaves_ab1-4`, `TimerGetters_ah5`, `AudioList_ag`, `AudioMemory`, `AudioPoolFreeGuards`, `AudioTableDispatch`, `AudioMusic`, `AudioCharacterBankPaths`, `AudioChainWalk`, `RingHeaderInit`, etc.) ÔÇö same param-only / runtime-deref shape as the Util PromoLoop family, **no file-scope RVA construct, no boot risk**. `AudioList_ag.cpp` operates purely on caller-supplied pointers. And two audio files (`RwsBank`, `AudioEngine`) *are* already linked, so audio is not deliberately absent wholesale.
- **Conclusion:** the 78 C3+C4 rows are **not** uniformly blocked by a DirectShow carve-out. Only `AudioDSound` (8 rows) is a true boot blocker and `AudioRws` (20 rows) a thunk blocker; the remaining **~50 C3 rows across the leaf files are boot-safe** and their non-linkage is the same "frozen minimal exe set" drift (`build.bat:106-110`) that stranded the Util leaves ÔÇö not a deliberate audio-subsystem exclusion. The single Audio C4 (`AudioMemory.cpp`) is class B. Recommend: keep `AudioDSound`/`AudioRws` out (fix the static init / thunks first), reclassify the rest as drift and fold them into the add-back list.

---

## Counts per class (93 unlinked in scope)

- **Class A (unlinked-by-intent harness/self-test): 0.** None of these are harness or self-test code; they are hook reimplementations frozen out by the minimal-exe-set decision.
- **Class B (safe-to-add, boot-wise): 91** = 72 Util (all unlinked Util) + 19 Audio. (Of these, `UtilBatch`, `RwsStream`, `RwsFmt` and other ÔÜá files are boot-safe but non-functional-if-called runtime thunks; `Util/MixedC3Sweep.cpp` is a compile-into-nothing orphan.)
- **Class C (blocked): 2** ÔÇö `Audio/AudioDSound.cpp` (CONFIRMED boot crash, `:95-96`) and `Audio/AudioRws.cpp` (RVA-bound thunk, `:477/481/485/490`; boot-safe by mechanism but blocked by your stated "global bound to RVA at load" trigger and the foreign-call hazard ÔÇö flagged [UNCERTAIN] on the boot-crash claim specifically: it will not fault the loader).

**Load-bearing caveat for D0.7:** adding class-B files unlocks C4/C3 rows *for linkage only*. Because `HookSystem::Register` is no-op'd in the standalone, none of these run unless `exe_main` and the standalone call graph invoke them **by name** ÔÇö that is separate wiring work, not achieved by adding them to the source list.
