// Mashed RE — Frontend menu-state writers (c3-batch-u, targeted promotions).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0042bfb0  MenuStateParamStore  — 6-param guarded global-block writer
//
// Analysis note:
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042bfb0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declaration for the alt-init callee (guard == 0 path).
// 0x0042bf30  FUN_0042bf30 — C2; called only when DAT_0067eab0 == 0.
// ---------------------------------------------------------------------------
static auto* const s_AltInit =
    reinterpret_cast<void(__cdecl*)()>(0x0042bf30);

// ---------------------------------------------------------------------------
// MenuStateParamStore  --  0x0042bfb0
//
// Original: FUN_0042bfb0 (0x0042bfb0..0x0042c002)
// Signature: void FUN_0042bfb0(undefined4 p1..p6)  __cdecl
//
// Body (mechanical transcript from analysis note):
//   if (DAT_0067eab0 == 0) { FUN_0042bf30(); return; }   // alt-init path [0x0042bfb0]
//   DAT_0067e918 = p1;   // [0x0042bfb0 store block]
//   DAT_0067e91c = p2;
//   DAT_0067e920 = p3;
//   DAT_0067e924 = p4;
//   DAT_0067e928 = p5;
//   DAT_0067e92c = p6;
//   DAT_0067e930 = 1;    // flag
//
// Constants (cited from analysis note):
//   0x0067eab0 — guard global (== 0 → alt path)
//   0x0067e918..0x0067e92c — 6-word param block (stride 4)
//   0x0067e930 — flag, set to 1 on the store path
//
// Callee FUN_0042bf30 (0x0042bf30) is C2. Caller FUN_00432450 (0x00432450) is C2.
// arg_type: 'multi_arg_global_write' (guard_global=0x0067eab0,
//   out_base=0x0067e918, out_count=7 — 6 params + flag).
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bfb0.md
// ---------------------------------------------------------------------------

// 0x0042bfb0
extern "C" __declspec(dllexport) void __cdecl MenuStateParamStore(
    std::uint32_t param_1, std::uint32_t param_2, std::uint32_t param_3,
    std::uint32_t param_4, std::uint32_t param_5, std::uint32_t param_6)
{
    // Guard: DAT_0067eab0 == 0 → alt-init and return. [0x0042bfb0]
    if (*reinterpret_cast<const std::int32_t*>(0x0067eab0u) == 0) {
        s_AltInit();
        return;
    }

    // Store the 6-tuple into the contiguous block. [0x0042bfb0 store path]
    *reinterpret_cast<std::uint32_t*>(0x0067e918u) = param_1;
    *reinterpret_cast<std::uint32_t*>(0x0067e91cu) = param_2;
    *reinterpret_cast<std::uint32_t*>(0x0067e920u) = param_3;
    *reinterpret_cast<std::uint32_t*>(0x0067e924u) = param_4;
    *reinterpret_cast<std::uint32_t*>(0x0067e928u) = param_5;
    *reinterpret_cast<std::uint32_t*>(0x0067e92cu) = param_6;
    *reinterpret_cast<std::uint32_t*>(0x0067e930u) = 1u;  // flag
}

RH_ScopedInstall(MenuStateParamStore, 0x0042bfb0);
