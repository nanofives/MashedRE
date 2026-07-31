// Mashed RE - guarded pool-free wrappers for three audio object pools.
// Originals:
//   0x005ae380  FUN_005ae380  flag +0x00  anchor 0x007dda50   (23 bytes)
//   0x005a6c90  FUN_005a6c90  flag +0x10  anchor 0x007dc9e8   (24 bytes)
//   0x005ad8b0  FUN_005ad8b0  flag +0x1c  anchor 0x007dd6c4   (24 bytes)
// All three: audio, C2 -> C3.
//
// One shape, three instances. Each returns a node to its own pool unless bit 0
// of a per-pool flag byte is set, in which case it does nothing:
//     if ((*(byte*)(node + flag_off) & 1) == 0)
//         AudioPoolFree(&pool_anchor, node);
// The callee 0x005ae920 is AudioPoolFree, already C3 (bitmap-tracked
// fixed-size block pool; hooks.csv records a Frida GREEN 10/10 from
// c3-batch-f-s6), so the callee-half of the C2->C3 gate is satisfied by a
// genuinely promoted function rather than by a clause.
//
// Bit 0 reads as a "do not free" / externally-owned marker. That is a
// DESCRIPTION OF THE TEST, not of the field's meaning — nothing here names the
// bit, so it is reported as raw bit 0 and no semantics are assigned.
//
// The three differ ONLY in the flag offset and the anchor immediate, so they
// share this file rather than three near-identical ones. Each is transcribed
// separately anyway; no shared helper, because a helper would hide the two
// constants that are the entire content of each function.
//
// Disasm (0x005ae380; the other two differ only in the TEST offset and the
// pushed anchor immediate):
//   0x005ae380  8B 44 24 04      mov  eax, [esp+4]       ; node
//   0x005ae384  F6 00 01         test byte ptr [eax], 1  ; bit 0 only
//   0x005ae387  75 0E            jnz  0x005ae397         ; set -> do nothing
//   0x005ae389  50               push eax                ; node
//   0x005ae38a  68 50 DA 7D 00   push 0x7dda50           ; &pool anchor
//   0x005ae38f  E8 8C 05 00 00   call 0x005ae920         ; AudioPoolFree
//   0x005ae394  83 C4 08         add  esp, 8             ; __cdecl cleanup
//   0x005ae397  C3               ret
// 0x005a6c90: F6 40 10 01 (test [eax+0x10]) / push 0x7dc9e8
// 0x005ad8b0: F6 40 1C 01 (test [eax+0x1c]) / push 0x7dd6c4
//
// NOTE the mask is `test byte ptr [...], 1` — bit 0 alone, not the whole byte.
// A port that tested the byte for non-zero would pass every seed except one
// with the low bit clear and other bits set, which is why 0xfe is in the test
// vectors.
#include "../Core/HookSystem.h"

typedef void(__cdecl* AudioPoolFreeFn)(void* anchor, void* node);
static const AudioPoolFreeFn AudioPoolFree = (AudioPoolFreeFn)0x005ae920;

// 0x005ae380
extern "C" __declspec(dllexport) void __cdecl
AudioPoolFreeGuardA(unsigned char* param_1)
{
    if ((*(param_1 + 0x00) & 1) == 0)
        AudioPoolFree((void*)0x007dda50, param_1);
}

// 0x005a6c90
extern "C" __declspec(dllexport) void __cdecl
AudioPoolFreeGuardB(unsigned char* param_1)
{
    if ((*(param_1 + 0x10) & 1) == 0)
        AudioPoolFree((void*)0x007dc9e8, param_1);
}

// 0x005ad8b0
extern "C" __declspec(dllexport) void __cdecl
AudioPoolFreeGuardC(unsigned char* param_1)
{
    if ((*(param_1 + 0x1c) & 1) == 0)
        AudioPoolFree((void*)0x007dd6c4, param_1);
}

RH_ScopedInstall(AudioPoolFreeGuardA, 0x005ae380);
RH_ScopedInstall(AudioPoolFreeGuardB, 0x005a6c90);
RH_ScopedInstall(AudioPoolFreeGuardC, 0x005ad8b0);
