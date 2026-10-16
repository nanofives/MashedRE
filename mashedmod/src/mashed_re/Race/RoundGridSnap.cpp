// Mashed RE - FUN_00409290, the round-start node snap. SHARED TU: listed in BOTH exe_sources.rsp and
// asi_sources.rsp, so mashed_re.exe and mashed_re_dev.asi run this one body (hooks.csv exe_file == file).
//
// Brief: verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md section 6 (promotion leg).
// Plate: re/analysis/game_state_d5_cont2/0x00409290.md
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (preserved in original\MASHED.exe.unpatched).
//
// The body replaces the verbatim naked copy that lived in RoundGrid_naked.inc (exe-only). Its two callees bind to the
// C3 exports in Frontend/GlobalGetters_s5.cpp, a shared TU as well.

#include "../Core/HookSystem.h"

#include <cstdint>

extern "C" std::uint32_t __cdecl GetDat0066d6e0();                                                // 0x00426bc0
extern "C" void __cdecl GetTableEntry0066d658(int i, std::uint16_t* lo, std::uint16_t* hi);      // 0x00426bd0

// ---------------------------------------------------------------------------
// SnapNodeToSafeStartLines  --  0x00409290
//
// Original: FUN_00409290 (110 bytes, 0x00409290..0x004092fd), __cdecl, one stack arg `int* p`.
// Reads  : count = FUN_00426bc0() (0x00409295, DAT_0066d6e0);
//          per range i, FUN_00426bd0(i, &lo, &hi) (0x004092bb): lo = u16 [0x0066d658 + i*4], hi = u16 [0x0066d65a + i*4].
//          Both are used as int16 (movsx at 0x004092cf / 0x004092d6 / 0x004092e4 / 0x004092f1).
// Writes : *p only (0x004092e7, 0x004092f5). No other memory, no FPU.
// Flow   : count == 0 (0x0040929e/a0)          -> return, *p untouched.
//          count < 0 (jle 0x004092ac)          -> *p = 0 (ebp is zeroed at 0x0040929c).
//          range 0 sets first = lo (0x004092c7..cb, taken only while i == 0).
//          *p < lo (signed, jl 0x004092d4)     -> *p = lo, return.
//          *p <= hi (signed, jle 0x004092dd)   -> return, *p untouched.
//          next range (inc esi, signed cmp esi, count at 0x004092df..e2).
//          past the last range                 -> *p = first (0x004092e4..e7).
// Registers: eax/ecx/edx are scratch, ebx/esi/edi/ebp are saved and restored, as for any cdecl body.
// Callers: FUN_0040e590 (FUN_00409290(&DAT_0063ba80)), FUN_00424eb0.
// ---------------------------------------------------------------------------

// 0x00409290
extern "C" __declspec(dllexport) void __cdecl SnapNodeToSafeStartLines(std::int32_t* p) {
    const std::int32_t count = static_cast<std::int32_t>(GetDat0066d6e0());   // 0x00409295
    if (count == 0) return;                                                    // 0x0040929e
    std::int16_t first = 0;                                                    // ebp, 0x0040929c
    for (std::int32_t i = 0; i < count; ++i) {                                 // 0x004092aa, 0x004092e0
        std::uint16_t lo = 0, hi = 0;
        GetTableEntry0066d658(i, &lo, &hi);                                    // 0x004092bb
        const std::int16_t lo16 = static_cast<std::int16_t>(lo);
        const std::int16_t hi16 = static_cast<std::int16_t>(hi);
        if (i == 0) first = lo16;                                              // 0x004092c7
        if (*p < lo16) { *p = lo16; return; }                                  // 0x004092d2, 0x004092f5
        if (*p <= hi16) return;                                                // 0x004092db
    }
    *p = first;                                                                // 0x004092e7
}

RH_ScopedInstall(SnapNodeToSafeStartLines, 0x00409290);
