// Mashed RE — car-slot state setter: ONE body for both build targets (fwd494fd0, 2026-10-16).
//
// Before this file the RVA had two bodies that could drift apart, which is the dual-copy class
// (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md, ROADMAP D4):
//   Frontend/RaceResults.cpp  .asi-only, installed, the body hooks.csv's C3 evidence covers
//   Ai/AiStandalone.cpp       exe+asi, private inline copy with a null-base guard (the exe's)
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. RH_ScopedInstall installs it in the .asi and
// is a no-op in the exe, so the exe now runs the very body the .asi evidence judged.
//
// The guard is the standalone's, kept in the one body: in the standalone the image-pad owns the original's
// RVA range ZERO-FILLED, so `*(uint32*)0x005f2770` reads 0 there (the original's .data holds 0x005f2728 at
// that address; read from original/MASHED.exe.unpatched file offset 0x1f2770, see the note at
// AiStandalone.cpp's forwarder). The original has no guard because the pointer is never 0 once the process has
// loaded; the guarded path is therefore unreachable inside MASHED.exe and cannot change a result there.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// CarSlotStateSet  --  FUN_0040e480  (18 bytes, 0x0040e480..0x0040e492)
//
// Signature: void FUN_0040e480(int param_1, undefined4 param_2)
//   param_1: car slot index (0-3)
//   param_2: value to write
// Writes param_2 to *(PTR_PTR_005f2770 + param_1*4 + 0x34). No branches, no callees in the original.
// (`mov edx,[0x5f2770] / mov [edx+ecx*4+0x34], eax`, same shape the A/B drivers cite in AiControllerAB.cpp W7.)
//
// Uncertainties:
//   U-3431: struct type at 0x005f2770 -- does not affect the reimplementation.
//
// ref: re/analysis/c0_promotion_frontend_a/0x0040e480.md
// ---------------------------------------------------------------------------

// 0x0040e480
extern "C" __declspec(dllexport) void __cdecl CarSlotStateSet(int param_1, std::uint32_t param_2)
{
    // PTR_PTR_005f2770: base pointer read from 0x005f2770; 0x34 is the fixed offset after param_1*4.
    const std::uintptr_t base = *reinterpret_cast<const std::uint32_t*>(0x005f2770u);
    if (base == 0) return;   // standalone image-pad only, see the header (never taken in MASHED.exe)
    *reinterpret_cast<std::uint32_t*>(base + static_cast<std::uintptr_t>(static_cast<unsigned>(param_1)) * 4u + 0x34u)
        = param_2;
}

RH_ScopedInstall(CarSlotStateSet, 0x0040e480);
