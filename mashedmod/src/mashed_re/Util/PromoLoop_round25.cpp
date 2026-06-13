// Mashed RE — promote-round round 25 (L5 int2out: two-out-ptr getter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0046cbb0  CarStatePairGet — vehicle; fn(i, out_state*, out_secondary*)
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-13. Diffed via
// the new int2out handler.
//
// Analysis: re/analysis/vehicle_damage_d2/0x0046cbb0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// CarStatePairGet  --  0x0046cbb0   (subsystem: vehicle)
//
// Original: FUN_0046cbb0 (47 bytes, 0x0046cbb0..0x0046cbde)
// Bytes: 8B 44 24 04 / 83 F8 10 / 72 03 / 33 C0 / C3 /
//        8B 54 24 08 / 69 C0 04 0D 00 00 / 8B 88 90 1F 88 00 / 89 0A /
//        8B 80 94 1F 88 00 / 8B 4C 24 0C / 89 01 / B8 01 00 00 00 / C3
//   (mov eax,[esp+4]=idx; cmp eax,0x10; jb read; xor eax,eax; ret 0;
//    read: mov edx,[esp+8]=out_a; imul eax,eax,0xd04;
//          mov ecx,[eax+0x00881f90]; mov [edx],ecx;      <- *out_a = state
//          mov eax,[eax+0x00881f94]; mov ecx,[esp+0xc]=out_b; mov [ecx],eax;
//          mov eax,1; ret)                                <- *out_b = secondary, ret 1
// Signature: undefined4 FUN_0046cbb0(uint idx, undefined4* out_a, undefined4* out_b)
//
// idx >= 0x10 returns 0 (no writes). Else writes the per-car state (+0) and
// secondary (+4) fields and returns 1. Per-car struct at 0x00881f90, stride
// 0xd04 -> requires scenario:'race' with in-bounds car indices 0..3.
//
// Constants (cited from function body at 0x0046cbb0):
//   0x10       — exclusive bound (cars 0..15; unsigned jb)
//   0x00881f90 — per-car state field base (out_a)
//   0x00881f94 — per-car secondary field base (out_b; = base + 4)
//   0xd04      — per-car stride
//   1 / 0      — success / OOB return
//
// Callers: FUN_00410d10, FUN_0040e180 (both vehicle, C2). Leaf.
//
// Uncertainties (non-blocking):
//   U-1855/U-1856/U-1857: field-value/struct semantics (data-semantic).
// ---------------------------------------------------------------------------

// 0x0046cbb0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarStatePairGet(
        std::uint32_t idx, std::uint32_t* out_a, std::uint32_t* out_b) {
    // 0x10 bound, 0x00881f90/94 bases, 0xd04 stride cited at 0x0046cbb0 body.
    if (idx >= 0x10u) {
        return 0u;
    }
    const std::uint32_t off = idx * 0xd04u;
    *out_a = *reinterpret_cast<const std::uint32_t*>(0x00881f90u + off);
    *out_b = *reinterpret_cast<const std::uint32_t*>(0x00881f94u + off);
    return 1u;
}

RH_ScopedInstall(CarStatePairGet, 0x0046cbb0);
