// Mashed RE — promote-round round 26 (worklist batch: single-global leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004cbc90  RwGlobal7d459cGet  — render; getter (companion to 0x004cbc80 setter)
//   0x0040e450  Flag63b908Get      — util;   getter
//   0x0040e460  Flag63b908Set      — util;   setter (same global as 0x0040e450)
//   0x0040e4a0  ElapsedTimeGet     — ai;     getter (DAT_005f29b8 elapsed-time)
//   0x00443080  AiTargetEnableGet  — ai;     getter (DAT_00897ffc mode-6 gate)
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-13.
// All callers confirmed C2 via a round-26 Ghidra reference_to pass.
//
// Analysis notes: render_4_c1_to_c2_s6 / leaderboard / ai_update buckets
// (one-line getter/setter leaves; see hooks.csv rows).

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004cbc90  RwGlobal7d459cGet -- A1 9C 45 7D 00 C3 (mov eax,[0x007d459c]; ret)
//   getter companion of RwGlobal7d459cSet (0x004cbc80, round 1). Caller
//   FUN_00493710 RW_INIT_FN (render, C2).
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwGlobal7d459cGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x007d459cu);  // cited at 0x004cbc90
}
RH_ScopedInstall(RwGlobal7d459cGet, 0x004cbc90);

// 0x0040e450  Flag63b908Get -- A1 08 B9 63 00 C3 (mov eax,[0x0063b908]; ret)
//   race-end gate flag read in STATE_FN case 3. Caller FUN_0045c7f0 (audio, C2).
extern "C" __declspec(dllexport) std::uint32_t __cdecl Flag63b908Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0063b908u);  // cited at 0x0040e450
}
RH_ScopedInstall(Flag63b908Get, 0x0040e450);

// 0x0040e460  Flag63b908Set -- 8B 44 24 04 A3 08 B9 63 00 C3
//   (mov eax,[esp+4]; mov [0x0063b908],eax; ret). Setter paired with
//   Flag63b908Get. Caller FUN_00410860 (util, C2). U-0499 data-semantic.
extern "C" __declspec(dllexport) void __cdecl Flag63b908Set(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0063b908u) = param_1;  // cited at 0x0040e460
}
RH_ScopedInstall(Flag63b908Set, 0x0040e460);

// 0x0040e4a0  ElapsedTimeGet -- A1 B8 29 5F 00 C3 (mov eax,[0x005f29b8]; ret)
//   elapsed-time global, used as a countdown compare. Caller FUN_00418560
//   (ai, C2).
extern "C" __declspec(dllexport) std::uint32_t __cdecl ElapsedTimeGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x005f29b8u);  // cited at 0x0040e4a0
}
RH_ScopedInstall(ElapsedTimeGet, 0x0040e4a0);

// 0x00443080  AiTargetEnableGet -- A1 FC 7F 89 00 C3 (mov eax,[0x00897ffc]; ret)
//   must be non-zero to enable AI mode-6 targeting. Caller FUN_004177b0
//   (ai, C2).
extern "C" __declspec(dllexport) std::uint32_t __cdecl AiTargetEnableGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x00897ffcu);  // cited at 0x00443080
}
RH_ScopedInstall(AiTargetEnableGet, 0x00443080);
