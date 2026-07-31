// Mashed RE - recursive physics-body scalar setter.
// Original: 0x0055c4f0  FUN_0055c4f0  PhysicsBodySetFriction  vehicle  C2 -> C3
// Plate: re/analysis/promote_c2_vehicle_dynamics/0055c4f0.md   (76 bytes)
//
// Walks a physics-body tree and stores param_2 into the +0x50 field of every
// LEAF body. A node whose type tag is 8 is a compound: instead of writing, it
// recurses over a child array whose stride is 0x60. hooks.csv records the live
// call site passing 0.45f, i.e. the scalar is a float bit-pattern, but the
// function itself only ever moves the 4 bytes — it never does arithmetic on
// them, so the port keeps it as an untyped dword (no x87 involvement).
//
// PARAM_1 IS A POINTER, not an int. Ghidra's signature says
// `void FUN_0055c4f0(int param_1, ...)` while the same decompilation
// dereferences it three times (+0x5c, +0x40, +0x50). That mismatch is a known
// recurring defect class in this project, so it is called out here rather than
// transcribed: the port takes an unsigned char* and does explicit offset math.
//
// BRANCH STRUCTURE - note it is NOT a plain if/else. Transcribed from the
// disassembly, the tag==8 path with a ZERO child count falls out of the `if`
// WITHOUT writing anything, because the write lives in the `else`:
//     tag != 8              -> *(p+0x50) = param_2
//     tag == 8, count == 0  -> NO WRITE AT ALL   <-- easy to get wrong
//     tag == 8, count  > 0  -> recurse over count children, stride 0x60
// The count is RE-READ from *(p+0x40) on every iteration (0x0055c51e), not
// cached, so a callee that reallocates the child array is observed. The port
// keeps that re-read.
//
// Disasm at 0x0055c4f0..0x0055c53b (76 bytes; integer moves only, no x87):
//   0x0055c4f0  53                  push ebx
//   0x0055c4f1  8B 5C 24 08         mov  ebx, [esp+8]        ; param_1
//   0x0055c4f5  56                  push esi
//   0x0055c4f6  8B 43 5C            mov  eax, [ebx+0x5c]     ; inner ptr
//   0x0055c4f9  66 83 38 08         cmp  word ptr [eax], 8   ; 16-bit tag
//   0x0055c4fd  75 33               jnz  0x0055c532          ; -> leaf write
//   0x0055c4ff  8B 43 40            mov  eax, [ebx+0x40]     ; child descriptor
//   0x0055c502  33 F6               xor  esi, esi            ; i = 0
//   0x0055c504  8B 48 04            mov  ecx, [eax+4]        ; count
//   0x0055c507  85 C9               test ecx, ecx
//   0x0055c509  76 2E               jbe  0x0055c539          ; count==0 -> return
//   0x0055c50b  55                  push ebp
//   0x0055c50c  8B 6C 24 14         mov  ebp, [esp+0x14]     ; param_2
//   0x0055c510  57                  push edi
//   0x0055c511  33 FF               xor  edi, edi            ; byte offset = 0
//   0x0055c513  8B 08               mov  ecx, [eax]          ; children base
//   0x0055c515  55                  push ebp
//   0x0055c516  03 CF               add  ecx, edi
//   0x0055c518  51                  push ecx
//   0x0055c519  E8 D2 FF FF FF      call 0x0055c4f0          ; self-recursion
//   0x0055c51e  8B 43 40            mov  eax, [ebx+0x40]     ; RE-READ descriptor
//   0x0055c521  83 C4 08            add  esp, 8              ; __cdecl cleanup
//   0x0055c524  46                  inc  esi
//   0x0055c525  83 C7 60            add  edi, 0x60           ; child stride
//   0x0055c528  3B 70 04            cmp  esi, [eax+4]
//   0x0055c52b  72 E6               jc   0x0055c513          ; UNSIGNED compare
//   0x0055c52d  5F 5D 5E 5B C3      pop edi/ebp/esi/ebx; ret
//   0x0055c532  8B 54 24 10         mov  edx, [esp+0x10]     ; param_2
//   0x0055c536  89 53 50            mov  [ebx+0x50], edx     ; leaf write
//   0x0055c539  5E 5B C3            pop esi/ebx; ret
//
// The loop compare at 0x0055c52b is JC (unsigned), matching the decompiler's
// `uVar2 < (uint)piVar1[1]` — the count is treated as unsigned, so the port
// uses unsigned types for both the index and the count.
#include "../Core/HookSystem.h"

// 0x0055c4f0
extern "C" __declspec(dllexport) void __cdecl
PhysicsBodySetFriction(unsigned char* param_1, unsigned int param_2)
{
    // 0x0055c4f6 / 0x0055c4f9 — deref +0x5c, then read a 16-bit tag through it
    if (**(short**)(param_1 + 0x5c) == 8) {
        int* piVar1 = *(int**)(param_1 + 0x40);      // 0x0055c4ff
        unsigned int uVar2 = 0;                       // 0x0055c502
        if (piVar1[1] != 0) {                         // 0x0055c504..0x0055c509
            int iVar3 = 0;                            // 0x0055c511
            do {
                // 0x0055c513..0x0055c519: children base + i*0x60
                PhysicsBodySetFriction(
                    (unsigned char*)(*piVar1 + iVar3), param_2);
                piVar1 = *(int**)(param_1 + 0x40);    // 0x0055c51e RE-READ
                uVar2 = uVar2 + 1;                    // 0x0055c524
                iVar3 = iVar3 + 0x60;                 // 0x0055c525
            } while (uVar2 < (unsigned int)piVar1[1]);// 0x0055c528/0x0055c52b
            return;
        }
        // tag==8 with count==0 falls through here and writes NOTHING.
    }
    else {
        *(unsigned int*)(param_1 + 0x50) = param_2;   // 0x0055c536
    }
}

RH_ScopedInstall(PhysicsBodySetFriction, 0x0055c4f0);
