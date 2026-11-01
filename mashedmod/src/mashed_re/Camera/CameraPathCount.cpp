// Mashed RE — FUN_00426bb0 single-global getter: ONE body for both build targets (D4-DC-INC-pathprog, 2026-11-01).
//
// Before this file the RVA had four bodies, the dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md,
// ROADMAP D4):
//   Util/PromoLoop_round31.cpp                 .asi-only, installed, the body hooks.csv's C3 evidence covers
//   PP_426bb0 (Race/PathProgStep_naked.inc)    exe-only, private verbatim naked copy called by FUN_00408610
//   AL_426bb0 (Ai/AiLeafTargets_naked.inc)     exe-only, private verbatim naked copy
//   TG_426bb0 (Ai/AiTargetClosest_naked.inc)   exe-only, private verbatim naked copy
// All four are `mov eax, [0x0066d6d8] / ret`. This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The body
// moved here unchanged; the exe's naked callers now `call` it (re/tools/naked_specs/pathprog_p2.json,
// aileaf_g1.json, target_p3.json). RH_ScopedInstall installs it in the .asi and is a no-op in the exe.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00426bb0  A1 D8 D6 66 00 C3  (mov eax,[0x0066d6d8]; ret) — path-count int
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global66d6d8Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0066d6d8u);  // cited at 0x00426bb0
}
RH_ScopedInstall(Global66d6d8Get, 0x00426bb0);
