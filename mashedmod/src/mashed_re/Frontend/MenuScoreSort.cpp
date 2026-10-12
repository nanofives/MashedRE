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
// 0x00430835 add eax,-2 ; 0x00430838 cmp eax,8 ; ja 0x004308e1 (UNSIGNED):
// modes outside 2..10 return 0 (0x004308e1 xor eax,eax). Jump table 0x004308e4.
// EVERY case computes p*0x30 (lea r,[eax+eax*2] ; shl r,4) and loads one dword:
//   mode 2  : [p*0x30 + 0x7f0a44]   0x00430848..0x00430852
//   mode 3  : [p*0x30 + 0x7f0a44]   0x00430859..0x00430863
//   mode 4  : [p*0x30 + 0x7f0a48]   0x0043086a..0x00430874
//   mode 5  : [p*0x30 + 0x7f0a54]   0x0043087b..0x00430885
//   mode 6  : [p*0x30 + 0x7f0a4c]   0x0043088c..0x00430896
//   mode 7  : [p*0x30 + 0x7f0a60]   0x0043089d..0x004308a7
//   mode 8  : [p*0x30 + 0x7f0a64]   0x004308ae..0x004308b8
//   mode 9  : [p*0x30 + 0x7f0a68]   0x004308bf..0x004308c9
//   mode 10 : [p*0x30 + 0x7f0a44]   0x004308d0..0x004308da
// Re-ported 2026-10-12 (reearn/fstate): the previous port used stride 0xc for
// every mode except 7/8, so any p != 0 read the wrong dword.
// ---------------------------------------------------------------------------

// 0x00430830
extern "C" __declspec(dllexport) std::uint32_t __cdecl SplitScreenTrackAssignment(int param_1) {
    const std::uint32_t mode = *reinterpret_cast<const std::uint32_t*>(0x0067e9fcu);
    std::uintptr_t col;
    switch (mode) {
    case 2:  col = 0x007f0a44u; break;   // 0x00430852
    case 3:  col = 0x007f0a44u; break;   // 0x00430863
    case 4:  col = 0x007f0a48u; break;   // 0x00430874
    case 5:  col = 0x007f0a54u; break;   // 0x00430885
    case 6:  col = 0x007f0a4cu; break;   // 0x00430896
    case 7:  col = 0x007f0a60u; break;   // 0x004308a7
    case 8:  col = 0x007f0a64u; break;   // 0x004308b8
    case 9:  col = 0x007f0a68u; break;   // 0x004308c9
    case 10: col = 0x007f0a44u; break;   // 0x004308da
    default: return 0u;                  // 0x004308e1
    }
    const std::uint32_t off = static_cast<std::uint32_t>(param_1) * 0x30u;   // lea+shl 4, mod 2^32
    return *reinterpret_cast<const std::uint32_t*>(static_cast<std::uint32_t>(col + off));
}

RH_ScopedInstall(SplitScreenTrackAssignment, 0x00430830);  // re-ported 2026-10-12 reearn/fstate (stride 0x30 for every mode)

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
