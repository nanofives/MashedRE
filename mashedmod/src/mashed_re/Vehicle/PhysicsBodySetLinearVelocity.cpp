// Mashed RE - init a physics body record, then store a 3-dword vector at +0x40.
// Original: 0x0055c810  FUN_0055c810  PhysicsBodySetLinearVelocity  vehicle  C2 -> C3
// Plate: re/analysis/vehicle_update_d3_cont/0055c810.md   (44 bytes)
//
//     FUN_0055c380(body, &DAT_005e4fe0);       // RW-Physics shape-record init
//     *(u32*)(body + 0x40) = vec[0];
//     *(u32*)(body + 0x44) = vec[1];
//     *(u32*)(body + 0x48) = vec[2];
//     return body;
//
// TWO CORRECTIONS to the screening brief, both found in the disassembly:
//  1. It is NOT void. `mov eax, esi` at 0x0055c838 returns param_1, and the
//     brief recorded the return as void/RETURN_UNVERIFIED. The port returns it.
//  2. The brief described the function as writing only through the passed
//     pointer with no callee of note; there IS a direct callee, FUN_0055c380,
//     which does the bulk of the work. It is safe for a scratch buffer — it
//     writes constants only through its own first argument, touches no globals
//     and calls nothing (0x0055c380..0x0055c3d7) — but it is not absent.
//
// The three moves are untyped dwords (`mov ecx,[eax]` / `mov [esi+0x40],ecx`
// and so on), never x87, so the values are copied bit-exactly whatever they
// represent. Nothing here names them, so no meaning is assigned; the hooks.csv
// name PhysicsBodySetLinearVelocity predates this session and is not evidence.
//
// FUN_0055c380 is deliberately NOT ported: hooks.csv tags it
// third-party-library[RenderWare-Physics-3.7], held C1 by library-skip policy.
// The port calls the original at its RVA, which is the verbatim behaviour.
//
// Note FUN_0055c380 does NOT touch +0x40/+0x44/+0x48 (it writes indices
// 0,1,2,3,4,5,6,8,9,0xa,0xc,0xd,0xe,0x13,0x14,0x15,0x16,0x17 plus a u16 at
// +0x5a). So the init and the vector store are independent, and a port that ran
// them in the wrong order would still produce the same memory. Order is
// preserved here because it is what the original does, not because a test
// distinguishes it.
//
// Disasm at 0x0055c810..0x0055c83b (44 bytes):
//   0x0055c810  56                  push esi
//   0x0055c811  8B 74 24 08         mov  esi, [esp+8]        ; body
//   0x0055c815  68 E0 4F 5E 00      push 0x5e4fe0            ; &DAT_005e4fe0
//   0x0055c81a  56                  push esi
//   0x0055c81b  E8 60 FB FF FF      call 0x0055c380          ; RW-Physics init
//   0x0055c820  8B 44 24 14         mov  eax, [esp+0x14]     ; vec (after 2 pushes)
//   0x0055c824  83 C4 08            add  esp, 8              ; __cdecl cleanup
//   0x0055c827  8B 08               mov  ecx, [eax]
//   0x0055c829  89 4E 40            mov  [esi+0x40], ecx
//   0x0055c82c  8B 50 04            mov  edx, [eax+4]
//   0x0055c82f  89 56 44            mov  [esi+0x44], edx
//   0x0055c832  8B 40 08            mov  eax, [eax+8]
//   0x0055c835  89 46 48            mov  [esi+0x48], eax
//   0x0055c838  8B C6               mov  eax, esi            ; return body
//   0x0055c83a  5E                  pop  esi
//   0x0055c83b  C3                  ret
//
// The `mov eax,[esp+0x14]` at 0x0055c820 reads the SECOND argument: two pushes
// are still on the stack at that point, so [esp+0x14] is the original [esp+0xc].
#include "../Core/HookSystem.h"

// 0x0055c380 — RenderWare-Physics 3.7 shape-record init (C1, library-skip)
typedef void(__cdecl* RwpShapeRecordInitFn)(void* record, const void* param2);
static const RwpShapeRecordInitFn RwpShapeRecordInit =
    (RwpShapeRecordInitFn)0x0055c380;

// 0x0055c810
extern "C" __declspec(dllexport) unsigned char* __cdecl
PhysicsBodySetLinearVelocity(unsigned char* param_1, const unsigned int* param_2)
{
    RwpShapeRecordInit(param_1, (const void*)0x005e4fe0);   // 0x0055c81b
    *(unsigned int*)(param_1 + 0x40) = param_2[0];          // 0x0055c829
    *(unsigned int*)(param_1 + 0x44) = param_2[1];          // 0x0055c82f
    *(unsigned int*)(param_1 + 0x48) = param_2[2];          // 0x0055c835
    return param_1;                                          // 0x0055c838
}

RH_ScopedInstall(PhysicsBodySetLinearVelocity, 0x0055c810);
