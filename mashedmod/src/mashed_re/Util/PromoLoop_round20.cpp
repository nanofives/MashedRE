// Mashed RE — promote-round round 20 (single-out-ptr class via outbuf_only).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00495270  HWNDGet              — render; out = game window handle
//   0x00484c70  WorldObjectsBaseGet  — ai; *out = count, returns array base
//   0x0041da90  DeltaTimeOutGet      — ai; null-guarded *out = DAT_0063d588
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

// ---------------------------------------------------------------------------
// WorldObjectsBaseGet  --  0x00484c70   (subsystem: ai)
//
// Original: FUN_00484c70 (18 bytes, 0x00484c70..0x00484c81)
// Bytes: 8B 44 24 04 / 8B 0D D8 70 6E 00 / 89 08 / B8 B8 CC 6D 00 / C3
//   (mov eax,[esp+4]; mov ecx,[0x006e70d8]; mov [eax],ecx;
//    mov eax,0x006dccb8; ret)
// Signature: undefined4* FUN_00484c70(undefined4* param_1)
//
// Writes the world-object count to *param_1 and RETURNS the array base. The
// diff folds the return (fold_ret) so both the out (count) and the returned
// base are verified.
//
// Constants (cited from function body at 0x00484c70):
//   0x006e70d8 — world-object count (written to *param_1)
//   0x006dccb8 — world-objects array base (returned; stride 0x8c per caller)
//
// Caller: FUN_00414c30 (ai obstacle-avoidance, C2). Leaf.
// ---------------------------------------------------------------------------

// 0x00484c70
extern "C" __declspec(dllexport) std::uint32_t __cdecl WorldObjectsBaseGet(std::uint32_t* param_1) {
    // 0x006e70d8 count + 0x006dccb8 base cited at 0x00484c70 body.
    *param_1 = *reinterpret_cast<const std::uint32_t*>(0x006e70d8u);
    return 0x006dccb8u;
}

RH_ScopedInstall(WorldObjectsBaseGet, 0x00484c70);

// ---------------------------------------------------------------------------
// DeltaTimeOutGet  --  0x0041da90   (subsystem: ai)
//
// Original: FUN_0041da90 (16 bytes, 0x0041da90..0x0041da9f)
// Bytes: 8B 44 24 04 / 85 C0 / 74 08 / 8B 0D 88 D5 63 00 / 89 08 / C3
//   (mov eax,[esp+4]; test eax,eax; jz +8; mov ecx,[0x0063d588];
//    mov [eax],ecx; ret)
// Signature: void FUN_0041da90(undefined4* param_1)
//
// Null-guarded: if param_1 != NULL, copies DAT_0063d588 to *param_1.
//
// Constants (cited from function body at 0x0041da90):
//   0x0063d588 — accumulating time value (compared to a lap-complete threshold
//                by caller FUN_004103a0)
//
// Caller: FUN_004103a0 TimeTrial::LapFinishProcessor (util, C2). Leaf.
//
// Uncertainties (non-blocking):
//   U-3585: whether DAT_0063d588 is real-time / frame-count / elapsed
//           (data-semantic). The diff seeds it (seed_global) so the copy is
//           deterministic regardless.
// ---------------------------------------------------------------------------

// 0x0041da90
extern "C" __declspec(dllexport) void __cdecl DeltaTimeOutGet(std::uint32_t* param_1) {
    // null guard + 0x0063d588 cited at 0x0041da90 body.
    if (param_1 != nullptr) {
        *param_1 = *reinterpret_cast<const std::uint32_t*>(0x0063d588u);
    }
}

RH_ScopedInstall(DeltaTimeOutGet, 0x0041da90);
