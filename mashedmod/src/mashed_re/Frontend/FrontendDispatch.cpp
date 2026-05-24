// Mashed RE - Frontend game-mode dispatch reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042ee40.md
//   re/analysis/c0_promotion_frontend_a/0x0042ee40.md (prior plate)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: ma3-frida-s7 (game-mode / settings UI dispatch).
// Other ma3-frida-s7 candidates REFUSED via callee-gate:
//   0x0042f0c0 (4/8 callees still C1)
//   0x0042fb70 (3/10 callees C1)
//   0x0042fe90 (3/10 callees C1)
//   0x00439210 (8/21 callees C1 + 2 C0; large dispatcher)
// Only 0x0042ee40 passed the callee gate (sole callee 0x0040bb90 is C4).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: FUN_0040bb90 - sprite lookup variant B (SpriteLookupTableB, C4)
// Analysis: re/analysis/frontend_promote_menus_b/0040bb90.md
// Signature confirmed via hooks_registry.py entry 'sprite_lookup_table_b':
//   pointer __cdecl FUN_0040bb90(int32 key)
// The dispatcher forwards its own param_1 through (Ghidra renders the call as
// FUN_0040bb90() with no args because the call is a tail-call/stack-reuse;
// the actual asm forwards [esp+4]).
// ---------------------------------------------------------------------------
typedef void* (__cdecl *SpriteLookupB_t)(int param_1);
static constexpr std::uintptr_t kFun0040bb90 = 0x0040bb90u;
static inline void* CallFun0040bb90(int key) {
    return reinterpret_cast<SpriteLookupB_t>(kFun0040bb90)(key);
}

// ---------------------------------------------------------------------------
// FrontendModeDispatch  --  0x0042ee40
//
// Original: FUN_0042ee40 (204 bytes, 0x0042ee40..0x0042ef0c)
// Signature: undefined4 FUN_0042ee40(int param_1)
//   Returns: result of FUN_0040bb90 forwarding, or 0 on default/miss.
//
// Decomp (cited from re/analysis/promote_c2_frontend_menus/0x0042ee40.md):
//   switch (DAT_0067e9fc) {
//   case 2:
//     return FUN_0040bb90();
//   case 3: case 4: case 5:
//     if (DAT_0067f17c > 9) {
//       if (param_1 == 0 || param_1 == 1 || param_1 == 2)
//         return FUN_0040bb90();
//     }
//     if (param_1 > 999) param_1 -= 1000;
//     if (param_1 == 0 || param_1 == 1 || param_1 == 2)
//       return FUN_0040bb90();
//     break;
//   case 6: case 7: case 8: case 9:
//     if (param_1 == 0) return FUN_0040bb90();
//     break;
//   case 10:
//     return FUN_0040bb90();
//   }
//   return 0;
//
// Globals (cited at analysis note):
//   0x0067e9fc  outer mode/screen selector (switched 2..10)
//   0x0067f17c  animation frame counter (threshold check > 9)
//
// Constants (cited at decomp):
//   999  param_1 offset cutoff for -1000 adjustment
//   1000 subtracted from param_1 when > 999
//   9    DAT_0067f17c threshold (>9 enables early dispatch for cases 3-5)
//
// Note on FUN_0040bb90 forwarding semantics:
//   Ghidra's decomp omits the param. The C1 plate of FUN_0040bb90 confirms it
//   takes one undefined4 arg used as a sprite-table key. The dispatcher
//   forwards its own (possibly -1000-adjusted) param_1 through, which is the
//   only ABI-consistent interpretation given that FUN_0040bb90 dereferences
//   the arg via the sprite table at DAT_0063b904.
// ---------------------------------------------------------------------------

// 0x0067e9fc: outer mode switch (cited at decomp).
static constexpr std::uintptr_t kModeSelector = 0x0067e9fcu;
// 0x0067f17c: animation frame counter (cited at decomp).
static constexpr std::uintptr_t kFrameCounter = 0x0067f17cu;

// 0x0042ee40
extern "C" __declspec(dllexport) void* __cdecl FrontendModeDispatch(int param_1) {
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(kModeSelector);

    switch (mode) {
    case 2:
        return CallFun0040bb90(param_1);

    case 3:
    case 4:
    case 5: {
        const std::int32_t frame = *reinterpret_cast<const std::int32_t*>(kFrameCounter);
        if (frame > 9) {
            if (param_1 == 0 || param_1 == 1 || param_1 == 2) {
                return CallFun0040bb90(param_1);
            }
        }
        if (param_1 > 999) {
            param_1 -= 1000;
        }
        if (param_1 == 0 || param_1 == 1 || param_1 == 2) {
            return CallFun0040bb90(param_1);
        }
        break;
    }

    case 6:
    case 7:
    case 8:
    case 9:
        if (param_1 == 0) {
            return CallFun0040bb90(param_1);
        }
        break;

    case 10:
        return CallFun0040bb90(param_1);

    default:
        break;
    }
    return nullptr;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(FrontendModeDispatch, 0x0042ee40);
