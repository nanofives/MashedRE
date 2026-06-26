// Mashed RE — Render: rain line-width range setter (C2->C3 promotion worker).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00491010  RainLineWidthRangeSet  — render; two-param two-global setter
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-25
// (file offset = RVA - 0x400000).
//
// Analysis: re/analysis/breadth_unmapped_0049x/0x00491010.md
//           re/analysis/render_2_c1_to_c2_s5/FUN_00491010.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RainLineWidthRangeSet  --  0x00491010   (subsystem: render)
//
// Original: FUN_00491010 (20 bytes, 0x00491010..0x00491023)
// Bytes: 8B 44 24 04 / 8B 4C 24 08 / A3 B8 46 61 00 / 89 0D BC 46 61 00 / C3
//   (mov eax,[esp+4]; mov ecx,[esp+8];
//    mov [0x006146b8],eax; mov [0x006146bc],ecx; ret)
// Signature: void FUN_00491010(undefined4 param_1, undefined4 param_2)
//
// Constants (cited from function body at 0x00491010):
//   0x006146b8 — destination global dword for param_1
//   0x006146bc — destination global dword for param_2 (adjacent, +4)
//
// Companion init FUN_00490e70 seeds these defaults: 0x40400000 (3.0f) and
// 0x40c00000 (6.0f). Together with FUN_00490ff0 (0x006146b0/b4) they form the
// 4-float parameter block at 0x006146b0..bc.
//
// Uncertainties (non-blocking, data-semantic): the two globals' rain meaning.
// ---------------------------------------------------------------------------

// 0x00491010
extern "C" __declspec(dllexport) void __cdecl RainLineWidthRangeSet(std::uint32_t param_1,
                                                                    std::uint32_t param_2) {
    // 0x006146b8 / 0x006146bc cited at 0x00491010 body.
    *reinterpret_cast<std::uint32_t*>(0x006146b8u) = param_1;
    *reinterpret_cast<std::uint32_t*>(0x006146bcu) = param_2;
}

RH_ScopedInstall(RainLineWidthRangeSet, 0x00491010);
