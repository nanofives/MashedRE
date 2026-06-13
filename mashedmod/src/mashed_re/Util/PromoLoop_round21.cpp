// Mashed RE — promote-round round 21 (out-ptr survey: int_copy_outbuf).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0041f030  TriggerStructRead — ai; fn(i, out*) copies 4 dwords
//   0x0041f260  WorldMatrixCopy   — util; fn(i, out*) copies a 4x4 matrix
//
// Both bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12.
// Diffed via the existing int_copy_outbuf handler (fn(int idx, T* dst)).
//
// Analysis:
//   re/analysis/ai_update_d3/0x0041f030.md
//   re/analysis/random_rng_d2/0x0041f260.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TriggerStructRead  --  0x0041f030   (subsystem: ai)
//
// Original: FUN_0041f030 (0x2e bytes, 0x0041f030..0x0041f05e)
// Bytes (head): 8B 44 24 04 / 8B 4C 24 08 / 69 C0 AC 02 00 00 /
//               05 E0 D9 63 00 / 05 58 02 00 00 / 8B 10 / 89 11 / ...
//   (mov eax,[esp+4]=i; mov ecx,[esp+8]=out; imul eax,eax,0x2ac;
//    add eax,0x0063d9e0; add eax,0x258;  <- base 0x0063dc38 + i*0x2ac
//    then 4x: mov edx,[eax+k]; mov [ecx+k],edx)
// Signature: void FUN_0041f030(int param_1, undefined4* param_2)
//
// Copies 4 consecutive dwords from 0x0063dc38 + i*0x2ac into param_2[0..3]
// (the type-0x15 trigger struct, per the round-3 decomp transcript). Reads
// live trigger state -> scenario:'race'.
//
// Constants (cited from function body at 0x0041f030):
//   0x0063dc38 — trigger struct field base (= 0x0063d9e0 + 0x258)
//   0x2ac      — per-trigger stride
//
// Caller: FUN_00414c30 (ai obstacle-avoidance, C2). Leaf.
//
// Uncertainties (non-blocking):
//   U-1894: semantic of the 4-dword struct (data-semantic).
// ---------------------------------------------------------------------------

// 0x0041f030
extern "C" __declspec(dllexport) void __cdecl TriggerStructRead(int param_1, std::uint32_t* param_2) {
    // 0x0063dc38 base + 0x2ac stride cited at 0x0041f030 body.
    const std::uint32_t off = static_cast<std::uint32_t>(param_1) * 0x2acu;
    param_2[0] = *reinterpret_cast<const std::uint32_t*>(0x0063dc38u + off);
    param_2[1] = *reinterpret_cast<const std::uint32_t*>(0x0063dc3cu + off);
    param_2[2] = *reinterpret_cast<const std::uint32_t*>(0x0063dc40u + off);
    param_2[3] = *reinterpret_cast<const std::uint32_t*>(0x0063dc44u + off);
}

RH_ScopedInstall(TriggerStructRead, 0x0041f030);

// ---------------------------------------------------------------------------
// WorldMatrixCopy  --  0x0041f260   (subsystem: util)
//
// Original: FUN_0041f260 (42 bytes, 0x0041f260..0x0041f28a)
// Bytes (head): 8B 44 24 04 / 69 C0 AC 02 00 00 / 56 / 05 E0 D9 63 00 /
//               8B 80 6C 02 00 00 / 8B 70 04 / 57 / 8B 7C 24 10 / ...
//   (mov eax,[esp+4]=i; imul eax,eax,0x2ac; add eax,0x0063d9e0;
//    mov eax,[eax+0x26c];  <- p0 = *(0x0063dc4c + i*0x2ac)
//    mov esi,[eax+4];      <- sub = *(p0+4)
//    mov edi,[esp+0x10]=out;  then copy 16 dwords from esi+0x10)
// Signature: void FUN_0041f260(int param_1, undefined4* param_2)
//
// DOUBLE-DEREF: src = *(*(0x0063dc4c + i*0x2ac) + 4) + 0x10; copies the 16
// dwords (4x4 matrix) to param_2. The per-slot object pointers are NULL at
// menu -> scenario:'race' with in-bounds slots.
//
// Constants (cited from function body at 0x0041f260):
//   0x0063dc4c — per-slot object pointer array base (= 0x0063d9e0 + 0x26c)
//   0x2ac      — per-slot stride; +4 sub-object; +0x10 matrix data; 16 dwords
//
// Callers: FUN_00421d20 (util C2), FUN_0040d8f0 (gameplay C2), and others. Leaf.
// ---------------------------------------------------------------------------

// 0x0041f260
extern "C" __declspec(dllexport) void __cdecl WorldMatrixCopy(int param_1, std::uint32_t* param_2) {
    // 0x0063dc4c base + 0x2ac stride, +4 sub, +0x10 matrix cited at 0x0041f260 body.
    const std::uint32_t off = static_cast<std::uint32_t>(param_1) * 0x2acu;
    const std::uint32_t p0  = *reinterpret_cast<const std::uint32_t*>(0x0063dc4cu + off);
    const std::uint32_t sub = *reinterpret_cast<const std::uint32_t*>(p0 + 4u);
    const std::uint32_t* src = reinterpret_cast<const std::uint32_t*>(sub + 0x10u);
    for (int k = 0; k < 16; k++) {
        param_2[k] = src[k];
    }
}

RH_ScopedInstall(WorldMatrixCopy, 0x0041f260);
