I now have everything needed. No file-scope `*`-deref exists anywhere in my scope, so **class C = 0** in all 9 directories (the two known offenders are both in `Audio\`, out of scope). Here is the complete part-5 triage.

---

# D0.7 Triage ÔÇö Part 5 (final): Boot/Vehicle/Ai/Collision/Race/Compat/Math/Core/Harness

## Method note
Address counts from a raw grep are dominated by RVA-**citation comments** (gta-reversed style comments every RVA), so "RVA-free" was decided by reading each file and checking for a MASHED absolute address (0x00400000ÔÇô0x00b60000) **in a non-comment code path**. Two multiline greps for a file-scope `= *reinterpret_cast<ÔÇª>(0x00ÔÇª)` / column-0 `= *` initializer (the true class-C trigger, build.bat:101) returned **zero matches** across the whole scope.

## (1) Per-file table

Legend: A = unlinked-by-intent ┬À B = safe-to-add ┬À C = blocked. RVA-free = no MASHED absolute address in any non-comment code path.

| File | Class | RVA-free? (in-code addr count) | C4 | C3 | Blocking construct / notes | Cross-TU deps |
|---|---|---|---|---|---|---|
| **Boot/** | | | | | | |
| Boot.cpp | B | NO (calls 0x004a3258 @47; 3 exit helpers) | ÔÇô | partial | CRT exit trio; RVA callees | HookSystem (stub) |
| CrtCompilerSupport.cpp | B | NO (CrtFastErrorExit @44,46,48,49) | ÔÇô | CrtStackProbe C3 | `__chkstk`/SEH bodies are pure asm (RVA-free) but `CrtFastErrorExit` tunnels | none |
| CrtEnvArgv.cpp | B | NO (0x004a45fbÔÇª @21-53) | ÔÇô | C2 (refused) | calls MASHED heap/strlen | none |
| CrtInit.cpp | B | NO (table 0x005e7b84 @24,30) | ÔÇô | C2 | RH disabled | none |
| CrtStartup.cpp | B | NO (~40 abs globals/callees) | ÔÇô | C2 (a4-refused) | WinMain/exit-core chain | none |
| RwEngineInit.cpp | B | NO (calls 0x00495530 @46) | ÔÇô | DInputInitPredicate C3 | ÔÇô | none |
| PromoLoop_round6.cpp | B | NO (writes 0x007d4134ÔÇª @47,75,76,106) | ÔÇô | C3 | leaf setters | none |
| PromoLoop_round7.cpp | B | NO (0x00773818 @53; 0x00771e70 @91) | ÔÇô | C3 | ÔÇô | none |
| PromoLoop_round12.cpp | B | NO (0x0063cab0 @44; 0x007f0f10 @67; 0x007d6b30 @96) | ÔÇô | C3 | ÔÇô | none |
| **Vehicle/** | | | | | | |
| Damage.cpp | B | NO (RVA callees @35-50; .bss @58-69) | ÔÇô | C2/C3 | ÔÇô | HookSystem(stub) |
| Physics.cpp | B | NO (0x00881f90/f94 @56,58) | ÔÇô | C3 | **SUPERSEDED**: RVA 0x0046cbe0 already linked as `VehicleCarStateSet` in SmallLeaves_o5.cpp (hooks.csv:1461) | none |
| Replay.cpp | B | NO (0x0063bb04ÔÇª globals) | ÔÇô | C3 | ÔÇô | none |
| ReplayCluster.cpp | B | NO (0x0067eca4ÔÇª globals) | ÔÇô | C3 | ÔÇô | none |
| **ReplayRewind_ag.cpp** | **B** | **YES (0)** | ÔÇô | **C3** | pure pointer-offset leaf (`*(p+0x1c)=*(p+0x18)`) | **none** |
| PromoLoop_round2.cpp | B | NO (returns 0x008964fcÔÇª VAs) | ÔÇô | C3 | pointer-return getters | none |
| forceint_selftest.cpp | A | n/a (self-test `main()`) | ÔÇô | ÔÇô | standalone console test | links ForceIntegrator etc. |
| PhysicsChainHooks.cpp | A | NO (.asi C4 lane; ~270 in-code addrs) | **5 rows** | ÔÇô | header:34 "EXCLUDED from exe"; forwards to live RVAs + inline-asm abs | none (.asi only) |
| **PhysicsBodySetFriction.cpp** | **B** | **YES (0)** | ÔÇô | **C3** (hooks.csv:1793) | recursive, struct-relative offsets only, self-recursion | **none** |
| PhysicsBodySetLinearVelocity.cpp | B | NO (calls 0x0055c380 @61,67; 0x005e4fe0) | ÔÇô | C3 | ÔÇô | FUN_0055c380 (RWP C1, unported) |
| **Ai/** | | | | | | |
| PromoLoop_round1.cpp | B | NO (returns 0x008a96dcÔÇª VAs; reads globals) | ÔÇô | C3 | pointer/float getters | none |
| AiController.cpp | A | NO | ÔÇô | C2 | header:14 "dev .asi (hook) target only" | .asi only |
| AiControlStep.cpp | A | NO (Orig-trampolines, live callees) | ÔÇô | C3 | .asi diff-lane orchestrator ports | .asi only |
| AiPreTick.cpp | A | NO | ÔÇô | C3 | .asi diff-lane | .asi only |
| AiControllerAB.cpp | A | NO | ÔÇô | ÔÇô | pure A/B driver harness (gated MASHED_AI_AB) | .asi only |
| AiTargeting.cpp | A | NO | ÔÇô | C3 | WS-C2 .asi leaf cluster | .asi only |
| AiLineOfSight.cpp | A | NO (tile grids 0x007f1a9c; Orig-tramp) | ÔÇô | C3 | .asi diff-lane + self-test | .asi only |
| AiNavHooks.cpp | A | NO | ÔÇô | C3 | .asi diff-lane + self-test | .asi only |
| AiSplineHooks.cpp | A | NO | ÔÇô | C3/C4 | header:12 "**.asi-ONLY**" | .asi only |
| AiWallLateral.cpp | A | NO (tile grids; Orig-tramp) | ÔÇô | C3 | .asi diff-lane | .asi only |
| AiWallAhead.cpp | A | NO (live callee RVAs) | ÔÇô | C3 | .asi diff-lane | .asi only |
| AiLeaderTimer.cpp | A | NO (0x0089a4c8ÔÇª globals) | ÔÇô | C3 | .asi diff-lane | .asi only |
| **Collision/** | | | | | | |
| contact_selftest.cpp | A | n/a (`main()`) | ÔÇô | ÔÇô | standalone console test | ÔÇô |
| b4_chain_selftest.cpp | A | n/a (`main()`) | ÔÇô | ÔÇô | standalone console test | ÔÇô |
| b5b_qhull_selftest.cpp | A | n/a (`main()`) | ÔÇô | ÔÇô | standalone console test | ÔÇô |
| **Race/** | | | | | | |
| ScoringHooks.cpp | A | NO (0x008a94e0ÔÇª @32-58) | ÔÇô | C3/C4 | header:8 ".asi-ONLY ÔÇª excluded from exe" | .asi only |
| CameraClusterHooks.cpp | A | NO (0x005f2770; live callees @28-40) | ÔÇô | C3/C4 | WS-H2 .asi-only hook | .asi only |
| **Compat/** | | | | | | |
| PizOpenBypass.cpp | A | NO (0x00402b70ÔÇª @50-53) | ÔÇô | ÔÇô | dev-only .asi bypass; MASS-DISABLED @76 | .asi only |
| PizWin32Bypass.cpp | A | NO (0x007d3e50ÔÇª @74-108) | ÔÇô | ÔÇô | dev-only .asi compat; MASS-DISABLED @363-365 | .asi only |
| IntroTextNullGuard.cpp | A | YES (0 ÔÇö pure charmap asm) | ÔÇô | ÔÇô | header:16 "**NOT for the shipping exe**"; guards a MASHED-only bug at 0x004277a0 (absent from exe) | .asi only |
| StreamHandlerDispatchGuard.cpp | A | NO (0x00401000/0x00995000 range @50-52) | ÔÇô | ÔÇô | header:40 dev-only; guards 0x005ab148 | .asi only |
| **Math/** | | | | | | |
| devxform_selftest.cpp | A | n/a (`main()`) | ÔÇô | ÔÇô | standalone console test | RwV3dTransformPointsCPU |
| CosineLerp.cpp | B | NO (asm reads 0x005cd310/5cc320/5cc32c @41-45) | ÔÇô | C3 | naked x87 reading live .rdata | none |
| **Core/** | | | | | | |
| HookSystem.cpp | A | YES (0 literal addrs) | ÔÇô | ÔÇô | **the hook engine ÔÇö see below** | replaced by Stubs/HookSystemNoOp.cpp (build.bat:240) |
| **Harness/** | | | | | | |
| HarnessStubs.cpp | A | NO (0x00497190/0046c570/005ac5f0 @28-70) | ÔÇô | ÔÇô | synthetic arg-type harness; "DO NOT promote" (header:16) | .asi only |

## (2) ORDERED ADD-BACK LIST ÔÇö RVA-free class-B, ranked by C4 then C3
No C4 candidates exist (all C4 physics rows live in the class-A `.asi` lanes). Both RVA-free files are C3:

1. **`Vehicle/ReplayRewind_ag.cpp`** ÔÇö C3. 10-byte pure leaf (`*(p+0x1c)=*(p+0x18)`), zero runtime deps, zero cross-TU deps, unique RVA. Frida 12/12 GREEN (hooks.csv:1694). Safest possible add.
2. **`Vehicle/PhysicsBodySetFriction.cpp`** ÔÇö C3. Recursive struct-tree setter, struct-relative offsets only, self-recursion (no external callee), unique RVA. GREEN 5/5 all-branches (hooks.csv:1793). Runtime data dependency only (needs a valid RWP body tree, which the linked Collision/RWP chain builds) ÔÇö not a link dependency.

## (3) Porting backlog ÔÇö load-safe-but-tunnelled class-B (RVA-free = NO), ranked
Clean C3 leaves first (smallest RVA-neutralization surface), boot/CRT last:

- **C3 leaf getters/setters** (a few globals/callees to neutralize each): PromoLoop_round12, PromoLoop_round7, PromoLoop_round6, PromoLoop_round2, PromoLoop_round1, CosineLerp, PhysicsBodySetLinearVelocity *(also needs RWP FUN_0055c380 ported)*.
- **C3, but skip:** Physics.cpp ÔÇö its RVA (0x0046cbe0) is **already linked** via SmallLeaves_o5.cpp; do not add (redundant second dead export for the same RVA).
- **C2/C3 clusters** (bigger call graphs): Damage.cpp, Replay.cpp, ReplayCluster.cpp.
- **Boot/CRT chain** (largest RVA surface; the exe already has its own MSVC CRT, so these are low value): RwEngineInit, Boot.cpp, CrtCompilerSupport *(3 of 4 fns are pure asm; only CrtFastErrorExit tunnels)*, CrtEnvArgv, CrtInit, CrtStartup.

## (4) Counts (my part-5 scope, 44 files)
- **Class A (unlinked-by-intent): 25** ÔÇö 5 self-tests, 3 `.asi` C4/telemetry lanes (PhysicsChainHooks, ScoringHooks, CameraClusterHooks), 11 AI `.asi` diff-lane ports, 4 Compat dev-only hooks, HookSystem, HarnessStubs.
- **Class B (safe-to-add): 19** ÔÇö of which **RVA-free (linkable today) = 2**, tunnelled (backlog) = 17.
- **Class C (blocked): 0** ÔÇö no file-scope `*`-deref anywhere in scope (both known offenders are in `Audio\`).

## The two special-attention files

**`Core/HookSystem.cpp` ÔÇö Class A confirmed; what changes if linked.**
It is the *real* hook engine: `Register` stores `{rva-as-integer, &fn}`; `Install` does `reinterpret_cast<uint8_t*>(h.target_rva)`, reads `target[0]` and writes a 5-byte `E9` JMP at that MASHED RVA via VirtualProtect (lines 65-98). It contains **no literal MASHED address itself** ÔÇö addresses arrive at runtime through the registry. In the exe it is deliberately replaced by `Stubs/HookSystemNoOp.cpp` (build.bat:240). If the real file were linked instead of the stub:
- **At load: still harmless.** The ~200 `RH_ScopedInstall` file-scope ctors would only `push_back` an integer RVA + `&fn` (line 15-22) ÔÇö no deref (this is finding (1) verbatim; `Util/UtilLeaves.cpp` has the identical shape and boots).
- **The fault appears the instant `InstallAll()`/`Install()` runs:** it dereferences and patches `h.target_rva` (e.g. 0x004177b0), which is unmapped in an exe based at 0x10000 ÔåÆ AV (or VirtualProtect fails and the redirect silently no-ops). So linking it would weaponize every linked hook file's RVA at install time.
- This is exactly why the no-op stub is load-bearing: it makes `RH_ScopedInstall` a **guaranteed dead no-op** in the exe, which is the premise the entire triage rests on. `HookSystem.cpp` must stay unlinked.

**`Vehicle/PhysicsChainHooks.cpp` ÔÇö the split is INTENT, not drift, not a cross-TU dependency.**
The physics chain has two parallel implementations by design:
- **Linked standalone bodies** (VehiclePhysicsRun.cpp, ForceIntegrator.cpp, Integrate2.cpp, AeroStabilize.cpp, VehicleControl.cpp) substitute `std::sqrt` for the RW fast-sqrt LUT (the standalone has no RW device ÔåÆ null LUT; WS-PHYS-CRASH-FIX) and are self-contained ÔåÆ they ship in the exe.
- **PhysicsChainHooks.cpp** is the `.asi`-ONLY C4 verification lane (header:34 "EXCLUDED from the exe target"). It forwards **every** chain callee to its ORIGINAL RVA (RW LUT 0x004c3ac0/39b0/3df0/4c4d20, PRNG 0x00472650, rubber-band 0x00442ce0/c80) and reads live MASHED globals (0x007f0fd0, 0x00803340, 0x008815a0ÔÇª), so that *inside the injected MASHED* the live LUT executes and the C transcription is bit-identical ÔÇö the only way to earn C4 (installed inline-JMP, gated by `MASHED_HOOK_ONLY`). It is saturated with absolute addresses in every code path (fwd ptrs @50-59, live-global accessors @110-174, inline-asm `mov eax,0x00881560` @248) and would AV instantly at exe base 0x10000.

So the 5 C4 rows are C4 *because* they were witnessed through this `.asi` lane against the live original; the linked LUT-substituted bodies are the separate thing that ships. It is not drift (deliberate exclusion) and not a cross-TU dependency (it shares no TU with the linked bodies ÔÇö it is a self-contained alternative that forwards to the *original*, whereas the linked bodies call *each other*). **Implication for D2** ("ported physics as default"): the exe already links the standalone physics; PhysicsChainHooks stays `.asi`-only permanently as the C4 witness ÔÇö it is never an add-back candidate.

## (5) GRAND TOTAL ÔÇö reconciliation across parts 1-5
From the committed D0.7 record (build.bat:114-140) plus this part:
- **Total unlinked .cpp project-wide: 235** (433 globbed ÔêÆ 198 in the exe source list; note 5 files ÔÇö Qhull/Rw*Bridge/RwSceneBuild/RwRaceSubmit ÔÇö reach the exe via isolated `.obj`, not the source list).
- **Class C project-wide: exactly 2**, both in `Audio\` (AudioDSound.cpp:95-96, AudioRws.cpp:477-490). My 9 directories add **0** ÔÇö confirmed.
- **RVA-free / linkable-today project-wide:** the 6 already-added batch-1 files (Save/Input/Particle, build.bat:353-358) **+ my 2** (ReplayRewind_ag, PhysicsBodySetFriction) = **8 firmly attested**, plus whatever parts 3-4 surfaced in their directories.
- **This part (5): 44 files triaged ÔåÆ A=25, B=19 (RVA-free 2 / tunnelled 17), C=0.**

[UNCERTAIN] I read only build.bat's committed summary and my own 44 files; the per-file A/B/C tallies for parts 3-4's directories are outside what I verified here, so I cannot give an exact whole-project A/B split ÔÇö only the anchored facts above (235 total, 2 class-C both Audio, ÔëÑ8 RVA-free linkable today including my 2). No files were written.
