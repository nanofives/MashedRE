// Mashed RE — promote-round round 20 (single-out-ptr class via outbuf_only).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00495270  HWNDGet              — render; out = game window handle
//   [MOVED 2026-11-01, D4-DC-INC-ai] WorldObjectsBaseGet (FUN_00484c70) -> Util/WorldObjectsBase.cpp
//   [MOVED 2026-10-09, D-11073 leg 1a] DeltaTimeOutGet (FUN_0041da90) -> Race/SubStateCallees.cpp
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12.
// Diffed via the round-19 outbuf_only handler (round-20 fold_ret/seed_global
// extensions).
//
// Analysis:
//   re/analysis/skeleton_prep_boot_winmain_a/00495270.md
//   re/analysis/bucket_ai_00452eb0_004c3df0/00484c70.md
//   re/analysis/ai_update_d5/0x0041da90.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// HWNDGet  --  0x00495270   (subsystem: render)
//
// Original: FUN_00495270 (12 bytes, 0x00495270..0x0049527b)
// Bytes: E8 9B 44 00 00 / 8B 4C 24 04 / 89 01 / C3
//   (call 0x00499710; mov ecx,[esp+4]; mov [ecx],eax; ret)
// Signature: void FUN_00495270(undefined4* param_1)
//
// Calls FUN_00499710() (the HWND getter) and stores the result via the
// out-pointer. The reimpl calls THROUGH to the original-image 0x00499710 so
// the returned handle is byte-identical.
//
// Constants (cited from function body at 0x00495270):
//   0x00499710 — HWND getter (E8 rel32; render C2)
//
// Caller: FUN_00493710 RW_INIT_FN (render, C2). Callee: FUN_00499710 (C2).
// ---------------------------------------------------------------------------

// 0x00495270
extern "C" __declspec(dllexport) void __cdecl HWNDGet(std::uint32_t* param_1) {
    // call 0x00499710 cited at 0x00495270 body (E8 rel32 -> 0x00499710).
    using HwndFn = std::uint32_t(__cdecl*)();
    *param_1 = reinterpret_cast<HwndFn>(0x00499710u)();
}

RH_ScopedInstall(HWNDGet, 0x00495270);

