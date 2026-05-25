// Mashed RE — Frontend score-sort and player-color helpers.
// Analysis notes: re/analysis/frontend_promote_menus_b/
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// NOT INCLUDED here (callee gate failures):
//   0x0040b460  SlotSortByScoreWithModeOverride — refused: callee 0x00417740 is C0 (STUB S-0491)
//   0x00429a30  LapTimeStoreToPlayerArrays      — blocked: callee 0x00430790 is C1 with open U-3470

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// SplitScreenTrackAssignment  --  0x00430830
//
// Original: FUN_00430830 (179 bytes, 0x00430830..0x004308e3)
// Signature: undefined4 FUN_00430830(int param_1)
//   param_1: slot index
// Pure leaf; no callees.
//
// Switch on DAT_0067e9fc (game mode) — same global as GetRaceSubMode.
// Returns a value from one of several per-slot layout arrays, indexed
// by param_1, at stride 0xc or 0x30 depending on the mode.
// Returns 0 for unsupported/default modes.
//
// Per-mode array addresses (all cited from 0x00430830..0x004308e3):
//   mode 2/3/10 : DAT_007f0a44  (stride 0xc)  — read at 0x00430848, 0x00430856, 0x004308d0
//   mode 4      : DAT_007f0a48  (stride 0xc)  — read at 0x00430862
//   mode 5      : DAT_007f0a54  (stride 0xc)  — read at 0x0043087e
//   mode 6      : DAT_007f0a4c  (stride 0xc)  — read at 0x00430870
//   mode 7      : DAT_007f0a60  (stride 0x30) — read at 0x0043088c
//   mode 8      : DAT_007f0a64  (stride 0x30) — read at 0x0043089a
//   mode 9      : DAT_007f0a68  (stride 0xc)  — read at 0x004308a6
//
// ref: re/analysis/frontend_promote_menus_b/00430830.md
// ---------------------------------------------------------------------------

// 0x00430830
extern "C" __declspec(dllexport) std::uint32_t __cdecl SplitScreenTrackAssignment(int param_1) {
    int mode = *reinterpret_cast<const int*>(0x0067e9fcu);
    switch (mode) {
    case 2:  // stride 0xc
    case 3:  // stride 0xc  (same array as case 2)
    case 10: // stride 0xc  (same array as cases 2/3)
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a44u + static_cast<unsigned>(param_1) * 0xcu);
    case 4:  // stride 0xc
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a48u + static_cast<unsigned>(param_1) * 0xcu);
    case 5:  // stride 0xc
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a54u + static_cast<unsigned>(param_1) * 0xcu);
    case 6:  // stride 0xc
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a4cu + static_cast<unsigned>(param_1) * 0xcu);
    case 7:  // stride 0x30
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a60u + static_cast<unsigned>(param_1) * 0x30u);
    case 8:  // stride 0x30
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a64u + static_cast<unsigned>(param_1) * 0x30u);
    case 9:  // stride 0xc
        return *reinterpret_cast<const std::uint32_t*>(0x007f0a68u + static_cast<unsigned>(param_1) * 0xcu);
    default:
        return 0u;
    }
}

RH_ScopedInstall(SplitScreenTrackAssignment, 0x00430830);  // re-enabled 2026-05-24 c3-frontend-b

// ---------------------------------------------------------------------------
// PlayerColorTableGet  --  0x0040e3a0
//
// Original: FUN_0040e3a0 (141 bytes, 0x0040e3a0..0x0040e42d)
// Signature: void FUN_0040e3a0(undefined4 param_1, undefined1 *param_2)
//   param_1: player color index 0..5
//   param_2: RGBA byte[4] output pointer
//
// 6-way switch writing 4 RGBA bytes to param_2.
// For the default case, calls FUN_004a332b(0xfffffff2) — non-returning abort.
//
// Color table (all bytes cited from body 0x0040e3a0..0x0040e42d):
//   case 0: (0x98, 0x3a, 0x3d, 0xff)  — red-ish
//   case 1: (0x4e, 0x89, 0xae, 0xff)  — blue-ish
//   case 2: (0x61, 0x76, 0x56, 0xff)  — green-ish
//   case 3: (0xdb, 0xc3, 0x62, 0xff)  — yellow-ish
//   case 4: (0xeb, 0xa7, 0xa7, 0xff)  — pink-ish
//   case 5: (0x00, 0x00, 0x00, 0xff)  — black
//   default: FUN_004a332b(0xfffffff2) — abort (0xfffffff2 = -14 signed)
//
// FUN_004a332b is an abort/assert — non-returning cold path; C-level irrelevant.
// Declared extern as an abort helper to match original behavior exactly.
//
// ref: re/analysis/frontend_promote_menus_b/0040e3a0.md
// ---------------------------------------------------------------------------

// 0x004a332b — non-returning abort helper (called via function pointer; not linked as extern).
// Invoked only on the unreachable default case (invalid player index).
// Error code 0xfffffff2 = -14 signed, cited at 0x0040e427.
typedef void (__cdecl *AbortFn_t)(std::int32_t);
static const AbortFn_t s_AbortFn = reinterpret_cast<AbortFn_t>(0x004a332bu);

// 0x0040e3a0
extern "C" __declspec(dllexport) void __cdecl PlayerColorTableGet(int param_1, std::uint8_t* param_2) {
    switch (param_1) {
    case 0:
        param_2[0] = 0x98u; param_2[1] = 0x3au; param_2[2] = 0x3du; param_2[3] = 0xffu;
        break;
    case 1:
        param_2[0] = 0x4eu; param_2[1] = 0x89u; param_2[2] = 0xaeu; param_2[3] = 0xffu;
        break;
    case 2:
        param_2[0] = 0x61u; param_2[1] = 0x76u; param_2[2] = 0x56u; param_2[3] = 0xffu;
        break;
    case 3:
        param_2[0] = 0xdbu; param_2[1] = 0xc3u; param_2[2] = 0x62u; param_2[3] = 0xffu;
        break;
    case 4:
        param_2[0] = 0xebu; param_2[1] = 0xa7u; param_2[2] = 0xa7u; param_2[3] = 0xffu;
        break;
    case 5:
        param_2[0] = 0x00u; param_2[1] = 0x00u; param_2[2] = 0x00u; param_2[3] = 0xffu;
        break;
    default:
        s_AbortFn(static_cast<std::int32_t>(0xfffffff2u)); // -14; original abort at 0x004a332b
        break;
    }
}

RH_ScopedInstall(PlayerColorTableGet, 0x0040e3a0);  // re-enabled 2026-05-24 phase-a2 GREEN (12/12 6 distinct)
