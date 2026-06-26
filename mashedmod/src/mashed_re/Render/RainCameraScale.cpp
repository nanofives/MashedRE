// Mashed RE — Rain camera-scale setter reimplementation (C2->C3 promotion worker).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Function in this file:
//   0x00490ff0  RainSetCameraScale — 19B; 2-global setter (_DAT_006146b0/_DAT_006146b4)
//
// Analysis note:
//   re/analysis/breadth_unmapped_0049x/0x00490ff0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RainSetCameraScale  --  0x00490ff0
//
// Original: FUN_00490ff0 (19 bytes, 0x00490ff0..0x00491002; RET at 0x00491003)
// Signature: void FUN_00490ff0(undefined4 param_1, undefined4 param_2)  __cdecl
// Returns: void (eax = param_2 at RET — see disasm below)
//
// Disassembly (cited from 0x00490ff0):
//   00490ff0  MOV EAX,[ESP+0x8]        ; param_2
//   00490ff4  MOV ECX,[ESP+0x4]        ; param_1
//   00490ff8  MOV [0x006146b4],EAX     ; _DAT_006146b4 = param_2
//   00490ffd  MOV [0x006146b0],ECX     ; _DAT_006146b0 = param_1
//   00491003  RET                      ; cdecl plain ret; eax holds param_2
//
// Constants (cited from 0x00490ff0 body):
//   0x006146b0 — first scale slot (set to param_1)
//   0x006146b4 — second scale slot (set to param_2)
//
// Semantics (cited from analysis note re/analysis/breadth_unmapped_0049x/0x00490ff0.md):
//   Rain camera-scale setter. Default values from FUN_00490e70: both 0x3e800000 (0.25f).
//   _DAT_006146b4 is consumed as a velocity-scale multiplier in the per-frame rain
//   update (FUN_004910c0, FUN_00491340). Lua API name "RainSetCameraScale"
//   (string at 0x005cdacc).
//
// The original is declared void but the final store leaves eax = param_2 at RET
// (MOV EAX from [ESP+0x8] at 0x00490ff0 is never overwritten before RET). The
// int_pair A/B harness observes eax (ret='uint32'); declaring this reimpl as
// returning uint32 and `return param_2` reproduces the original's eax bit-for-bit.
// Callers treat FUN_00490ff0 as void and ignore eax, so the extra return is inert.
// ---------------------------------------------------------------------------

// 0x00490ff0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RainSetCameraScale(
    std::uint32_t param_1, std::uint32_t param_2)
{
    // Store order mirrors the original (param_2 first, then param_1). [0x00490ff8/0x00490ffd]
    *reinterpret_cast<std::uint32_t*>(0x006146b4u) = param_2;  // _DAT_006146b4 = param_2
    *reinterpret_cast<std::uint32_t*>(0x006146b0u) = param_1;  // _DAT_006146b0 = param_1
    return param_2;  // eax = param_2 at RET (0x00490ff0 MOV EAX,[ESP+0x8])
}

RH_ScopedInstall(RainSetCameraScale, 0x00490ff0);
