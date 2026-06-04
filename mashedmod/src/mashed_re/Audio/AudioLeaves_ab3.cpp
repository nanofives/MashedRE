// Mashed RE — Audio pure-leaf C2->C3 cluster (c3_batch_ab session 3).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Six audio leaves promoted C2 -> C3 in this session. Every reimpl is the
// verbatim Ghidra decompilation (pool8, 2026-06-04); RVA cited above each.
//
//   0x005b73b0  AudioFindExtension        — find-file-extension string helper
//   0x005b9410  AudioSourceLoopSet        — source loop-enable setter (sw+hw paths)
//   0x005baf40  AudioRendererField3cSet   — renderer +0x3c setter w/ hw mirror
//   0x005bb5b0  AudioPcmSaturatedAdd      — 16-bit PCM saturated additive mixer
//   0x005bc450  AudioSlotPairZero         — zero first two dwords of a block
//   0x005bcb80  AudioMediaSubtypeFromTag  — DirectShow MEDIASUBTYPE GUID builder
//
// Analysis notes:
//   re/analysis/bucket_audio_005b2220_005b8570/0x005b73b0.md
//   re/analysis/bucket_audio_005b8be0_005bcb80/0x005b9410.md
//   re/analysis/bucket_audio_005b8be0_005bcb80/0x005baf40.md
//   re/analysis/bucket_audio_005b8be0_005bcb80/0x005bb5b0.md
//   re/analysis/bucket_audio_005b8be0_005bcb80/0x005bc450.md
//   re/analysis/bucket_audio_005b8be0_005bcb80/0x005bcb80.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x005b73b0  FUN_005b73b0  AudioFindExtension   (37 bytes)
//
// char * FUN_005b73b0(char *param_1)
//   Forward-walk to the NUL terminator, then back-walk toward param_1: if a '.'
//   is found return the pointer just after it; if the walk reaches param_1
//   without a dot, return param_1. Note: param_1 itself is NOT tested for '.'
//   (the loop breaks when the decrement lands on param_1).
//
// Decompilation (verbatim, 0x005b73b0):
//   cVar2 = *param_1; pcVar3 = param_1;
//   while (cVar2 != '\0') { pcVar3 = pcVar3 + 1; cVar2 = *pcVar3; }
//   cVar2 = *pcVar3;                       // = '\0' at terminator
//   while (true) {
//     if (cVar2 == '.') return pcVar3 + 1;
//     pcVar3 = pcVar3 + -1;
//     if (pcVar3 == param_1) break;
//     cVar2 = *pcVar3;
//   }
//   return param_1;
//
// Constants: '.' (0x2e), '\0' (0x00) — cited at 0x005b73b0.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) char* __cdecl
AudioFindExtension(char* param_1)
{
    char* pcVar3 = param_1;
    char  cVar2  = *pcVar3;
    while (cVar2 != '\0') {
        ++pcVar3;
        cVar2 = *pcVar3;
    }
    cVar2 = *pcVar3;  // terminator
    while (true) {
        if (cVar2 == '.') {
            return pcVar3 + 1;
        }
        --pcVar3;
        if (pcVar3 == param_1) {
            break;
        }
        cVar2 = *pcVar3;
    }
    return param_1;
}

RH_ScopedInstall(AudioFindExtension, 0x005b73b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005b9410  FUN_005b9410  AudioSourceLoopSet   (73 bytes)
//
// void FUN_005b9410(int param_1, int param_2)
//   param_1 = audio source; param_2 = loop flag (nonzero = looping).
//   Software path (caps[+0x50] & 8 == 0): set/clear flag bit 0x800 in +0x28.
//   Hardware path (else): set/clear bit 0x8 in *(*(int*)(src+0x11c) + 0xcc).
//
// Decompilation (verbatim, 0x005b9410):
//   if ((*(byte*)(*(int*)(param_1+0x94) + 0x50) & 8) == 0) {
//     if (param_2 != 0) { *(uint*)(param_1+0x28) |= 0x800; return; }
//     *(uint*)(param_1+0x28) &= 0xfffff7ff; return;
//   }
//   if (param_2 != 0) { puVar1 = *(int*)(param_1+0x11c)+0xcc; *puVar1 |= 8; return; }
//   puVar1 = *(int*)(param_1+0x11c)+0xcc; *puVar1 &= 0xfffffff7; return;
//
// Constants (cited 0x005b9410..0x005b9442):
//   +0x94 caps owner ptr; caps+0x50 bit 0x8 = hardware-mode select.
//   +0x28 source flags, bit 0x800 = software loop-enable; mask 0xfffff7ff.
//   +0x11c hwvoice ptr; hwvoice+0xcc control word, bit 0x8 = hw loop-enable;
//          mask 0xfffffff7.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioSourceLoopSet(int param_1, int param_2)
{
    if ((*reinterpret_cast<std::uint8_t*>(*reinterpret_cast<int*>(param_1 + 0x94) + 0x50) & 8u) == 0u) {
        if (param_2 != 0) {
            *reinterpret_cast<std::uint32_t*>(param_1 + 0x28) |= 0x800u;
            return;
        }
        *reinterpret_cast<std::uint32_t*>(param_1 + 0x28) &= 0xfffff7ffu;
        return;
    }
    std::uint32_t* puVar1 = reinterpret_cast<std::uint32_t*>(
        *reinterpret_cast<int*>(param_1 + 0x11c) + 0xcc);
    if (param_2 != 0) {
        *puVar1 |= 8u;
        return;
    }
    *puVar1 &= 0xfffffff7u;
}

RH_ScopedInstall(AudioSourceLoopSet, 0x005b9410);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005baf40  FUN_005baf40  AudioRendererField3cSet   (28 bytes)
//
// void FUN_005baf40(int param_1, undefined4 param_2)
//   Unconditionally writes param_2 to *(param_1+0x3c). If (*(byte*)(param_1+0x78)
//   & 8) the renderer is in hardware mode, so also mirror param_2 into the
//   hardware voice at *(*(int*)(param_1+0x11c) + 0x34).
//
// Decompilation (verbatim, 0x005baf40):
//   *(undefined4*)(param_1+0x3c) = param_2;
//   if ((*(byte*)(param_1+0x78) & 8) != 0)
//     *(undefined4*)(*(int*)(param_1+0x11c) + 0x34) = param_2;
//
// Constants (cited 0x005baf43..): +0x3c renderer field; +0x78 flags bit 0x8 =
//   hardware; +0x11c hwvoice ptr; hwvoice+0x34 mirror target.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioRendererField3cSet(int param_1, std::uint32_t param_2)
{
    *reinterpret_cast<std::uint32_t*>(param_1 + 0x3c) = param_2;
    if ((*reinterpret_cast<std::uint8_t*>(param_1 + 0x78) & 8u) != 0u) {
        *reinterpret_cast<std::uint32_t*>(
            *reinterpret_cast<int*>(param_1 + 0x11c) + 0x34) = param_2;
    }
}

RH_ScopedInstall(AudioRendererField3cSet, 0x005baf40);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005bb5b0  FUN_005bb5b0  AudioPcmSaturatedAdd   (381 bytes)
//
// void FUN_005bb5b0(int param_1, int param_2, int param_3, uint param_4)
//   param_1 = output int16 buffer; param_2/param_3 = two int16 source buffers;
//   param_4 = byte count.  For each sample i in [0, param_4>>1):
//     out[i] = saturate( (int)srcA[i] + (int)srcB[i] )
//   where saturate clamps to [-0x7fff, +0x7fff] (lower bound -32767, NOT -32768):
//     if (sum < 0x8000) { if (sum < -0x7fff) sum = -0x7fff; } else sum = 0x7fff;
//
// The original unrolls 4 samples/iteration with interleaved src/out pointer
// deltas plus a 1-sample tail loop; the per-sample arithmetic is identical, so
// a straight 0..n-1 loop is bit-identical on non-overlapping buffers (the only
// way this mixer is ever called: distinct out/srcA/srcB).
//
// Decompilation (verbatim, 0x005bb5b0): param_4 = param_4 >> 1; main loop over
//   (param_4 & 0xfffffffc) processes iVar1/iVar3/iVar6/iVar5 = srcB[k]+srcA[k]
//   for k=i..i+3 with the saturate ladder above; tail loop handles the
//   remaining (param_4 & 3) samples the same way.
//
// Constants (cited repeatedly 0x005bb5fb..): 0x8000 (32768) overflow compare,
//   0x7fff (+32767) positive clamp, -0x7fff (-32767) negative clamp, >>1 byte
//   ->sample, & 0xfffffffc 4-sample block mask.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioPcmSaturatedAdd(int param_1, int param_2, int param_3, std::uint32_t param_4)
{
    const std::uint32_t nSamples = param_4 >> 1;
    short* const out  = reinterpret_cast<short*>(param_1);
    short* const srcA = reinterpret_cast<short*>(param_2);
    short* const srcB = reinterpret_cast<short*>(param_3);

    for (std::uint32_t i = 0; i < nSamples; ++i) {
        int sum = static_cast<int>(srcA[i]) + static_cast<int>(srcB[i]);
        if (sum < 0x8000) {
            if (sum < -0x7fff) {
                sum = -0x7fff;
            }
        } else {
            sum = 0x7fff;
        }
        out[i] = static_cast<short>(sum);
    }
}

RH_ScopedInstall(AudioPcmSaturatedAdd, 0x005bb5b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005bc450  FUN_005bc450  AudioSlotPairZero   (17 bytes)
//
// void FUN_005bc450(undefined4 *param_1)
//   Clears the first two dwords (the interface-pointer pair at the head of the
//   capture-device block) and returns.
//
// Decompilation (verbatim, 0x005bc450): *param_1 = 0; param_1[1] = 0;
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioSlotPairZero(std::uint32_t* param_1)
{
    param_1[0] = 0;
    param_1[1] = 0;
}

RH_ScopedInstall(AudioSlotPairZero, 0x005bc450);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005bcb80  FUN_005bcb80  AudioMediaSubtypeFromTag   (36 bytes)
//
// void FUN_005bcb80(undefined4 param_1, undefined4 *param_2)
//   Fabricates the well-known DirectShow audio MEDIASUBTYPE GUID from a wave
//   format tag: {0000pppp-0000-0010-8000-00AA00389B71}, pppp = param_1.
//
// Decompilation (verbatim, 0x005bcb80):
//   *param_2 = param_1;                          // Data1 = wFormatTag
//   *(undefined2*)(param_2+1) = 0;               // Data2 = 0x0000  (byte +4)
//   *(undefined2*)((int)param_2+6) = 0x10;       // Data3 = 0x0010  (byte +6)
//   param_2[2] = 0xaa000080;                     // Data4[0..3] = 80 00 00 AA
//   param_2[3] = 0x719b3800;                     // Data4[4..7] = 00 38 9B 71
//
// Constants (cited 0x005bcb8?..): Data3 0x10; Data4 dwords 0xaa000080,
//   0x719b3800 → byte sequence 80 00 00 AA 00 38 9B 71.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioMediaSubtypeFromTag(std::uint32_t param_1, std::uint32_t* param_2)
{
    param_2[0] = param_1;
    *reinterpret_cast<std::uint16_t*>(reinterpret_cast<char*>(param_2) + 4) = 0;
    *reinterpret_cast<std::uint16_t*>(reinterpret_cast<char*>(param_2) + 6) = 0x10;
    param_2[2] = 0xaa000080u;
    param_2[3] = 0x719b3800u;
}

RH_ScopedInstall(AudioMediaSubtypeFromTag, 0x005bcb80);
