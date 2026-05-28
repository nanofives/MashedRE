// Mashed RE — Compat: NULL-out-buffer guard for stock FUN_004277a0 (string charmap copy).
//
// 2026-05-28 DEV WORKAROUND. The stock FUN_004277a0 crashes ~90 s in the modded boot when
// a caller in the intro/title text-render path (system-DLL/callback stack, "Empire Interactive"
// UTF-16 text) invokes it with EBX=0 (NULL out buffer). The original always writes the null
// terminator at out[count] (and chars in the loop), so EBX=0 → NULL write → AV. This crash
// reproduces with no .asi loaded (no-`.asi` control), i.e. it is a STOCK / modded-boot bug,
// not a port regression — but it blocks reaching the menu/race for runtime port verification.
//
// This guard hook adds a single instruction at entry: `test ebx,ebx; jnz do_work; ret`.
// When EBX≠0 it executes the original behavior byte-for-byte (PUSH ESI; MOV SI,[EAX];
// ADD EAX,2; TEST SI,SI; JLE null_term_count_le0; … charmap loop … null_term; RET). So this
// hook is faithful for valid (EBX≠0) calls and only skips the broken NULL-buffer case.
//
// Goal: unblock the dev .asi past the stock title/intro-text crash so the remaining
// ~248 menu/race ports can be runtime-exercised for the verification campaign. NOT for the
// shipping greenfield exe (mashed_re.exe) — this is a dev-only compat hook.
//
// Original (from listing 0x004277a0..0x00427832, 0x93 bytes):
//   PUSH ESI; MOV SI,[EAX]; ADD EAX,2; TEST SI,SI; JLE null_term_count_le0
//   MOV EDX,EAX; PUSH EDI; MOV ECX,EBX; SUB EDX,EBX; MOVZX EDI,SI
//   LAB: MOV AX,[EDX+ECX]; CMP AX,0x8 → 0x81 / 9→0x7f / a→0x81 / b→0x8d / c→0x80 / d→0x87 / e→0x8f
//        MOV [ECX],AX; ADD ECX,2; DEC EDI; JNZ LAB
//   MOVSX EAX,SI; POP EDI; MOV [EBX+EAX*2],0; POP ESI; RET
//   null_term_count_le0: MOVSX ECX,SI; MOV [EBX+ECX*2],0; POP ESI; RET

#include "../Core/HookSystem.h"
#include <cstdint>

// 0x004277a0  — dev-only guard reimpl. __cdecl with custom non-standard register convention
// (EAX = ushort* in (length-prefixed UTF-16), EBX = short* out). Faithful to FUN_004277a0
// except for the EBX==0 early-return at entry.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FUN_004277a0_NullOutGuard()
{
    __asm {
        // DEV GUARD: original would NULL-write if called with EBX=0.
        test ebx, ebx
        jnz  do_work
        ret

do_work:
        push esi                                 // orig 0x4277a0
        mov  si, word ptr [eax]                  // orig 0x4277a1
        add  eax, 0x2                            // orig 0x4277a4
        test si, si                              // orig 0x4277a7
        jle  null_term_count_le0                 // orig JLE 0x427828

        mov  edx, eax                            // orig 0x4277ac
        push edi                                 // orig 0x4277ae
        mov  ecx, ebx                            // orig 0x4277af
        sub  edx, ebx                            // orig 0x4277b1
        movzx edi, si                            // orig 0x4277b3

charmap_loop:                                    // orig LAB_004277b6
        mov  ax, word ptr [edx + ecx]            // orig 0x4277b6
        cmp  ax, 0x8                             // orig 0x4277ba
        jnz  c9
        mov  eax, 0x81                           // orig 0x4277c0
        jmp  do_write                            // orig 0x4277c5 JMP 0x427813
c9:
        cmp  ax, 0x9                             // orig 0x4277c7
        jnz  ca
        mov  eax, 0x7f                           // orig 0x4277cd
        jmp  do_write
ca:
        cmp  ax, 0xa                             // orig 0x4277d4
        jnz  cb
        mov  eax, 0x81                           // orig 0x4277da
        jmp  do_write
cb:
        cmp  ax, 0xb                             // orig 0x4277e1
        jnz  cc
        mov  eax, 0x8d                           // orig 0x4277e7
        jmp  do_write
cc:
        cmp  ax, 0xc                             // orig 0x4277ee
        jnz  cd
        mov  eax, 0x80                           // orig 0x4277f4
        jmp  do_write
cd:
        cmp  ax, 0xd                             // orig 0x4277fb
        jnz  ce
        mov  eax, 0x87                           // orig 0x427801
        jmp  do_write
ce:
        cmp  ax, 0xe                             // orig 0x427808
        jnz  do_write                            // orig JNZ 0x427813
        mov  eax, 0x8f                           // orig 0x42780e

do_write:                                        // orig LAB_00427813
        mov  word ptr [ecx], ax                  // orig 0x427813
        add  ecx, 0x2                            // orig 0x427816
        dec  edi                                 // orig 0x427819
        jnz  charmap_loop                        // orig JNZ 0x4277b6

        movsx eax, si                            // orig 0x42781c
        pop  edi                                 // orig 0x42781f
        mov  word ptr [ebx + eax*2], 0           // orig 0x427820
        pop  esi                                 // orig 0x427826
        ret                                      // orig 0x427827

null_term_count_le0:                             // orig LAB_00427828
        movsx ecx, si                            // orig 0x427828
        mov  word ptr [ebx + ecx*2], 0           // orig 0x42782b
        pop  esi                                 // orig 0x427831
        ret                                      // orig 0x427832
    }
}

RH_ScopedInstall(FUN_004277a0_NullOutGuard, 0x004277a0);
