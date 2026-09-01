// MenuOrdinalToPlayerSlot — 0x00430670
//
// One-file class per gta-reversed convention. Reimplementation of FUN_00430670.
//
// Original (0x00430670..0x00430754, 226 bytes). `int __cdecl FUN_00430670(int param_1)`
// (ordinal position 1..3 -> player slot index, or -1). param_1 is the sole stack arg,
// read at [ESP+0x1c] after the prologue (SUB ESP,0x10 + PUSH ESI + PUSH EDI).
//
// Two paths, gated on DAT_0067e9fc (listing 0x43067c CMP ECX,0xa / 0x430691 JNZ):
//   * CLEAN path  (DAT_0067e9fc == 10): pure reads of the frontend-state global block.
//     Loops ECX=0..3 counting non-zero (&DAT_0067ea98)[ECX] (0x4306a4); when the running
//     count EDX == param_1, returns (&DAT_0067ea94)[ECX] - 1 (0x4306be/0x4306c6); else -1.
//     Synthetically verifiable: seed DAT_0067e9fc=10 + the dword window 0x0067ea94..0x0067eaa8,
//     compare the scalar return (arg_type seed_globals_fold_ret).
//   * ELSE path (!= 10): CALLs FUN_00413f90 (0x4306d5) and derefs its returned live table
//     pointer + DAT_0067f17c*0x30 (+0x10 if ==4, +0x20 if ==5), then a table loop against
//     DAT_007f1a1c. Requires a live table -> NEEDS-BOOTED-RACE, not path1-observable.
//
// Pure integer transform (no x87). The naked body below is a verbatim transcription of
// the original instruction stream, so the clean-path return is bit-identical by
// construction. The one non-verbatim substitution is the ELSE-path CALL (0x4306d5
// `e8 rel32` -> `mov ecx,0x00413f90; call ecx`, functionally identical: ECX is reloaded
// immediately after at 0x4306da, and the callee takes no stack args); the ELSE path is
// unexercised on path1 (all vectors seed ==10) and is queued for booted-race verification.
//
// DEV-HOOK TU only (asi_sources.rsp), NOT the greenfield exe: it references absolute
// MASHED addresses (globals + the FUN_00413f90 call), valid only when injected into
// MASHED.exe. Callers: FUN_004306... menu ordinal paths (frontend).
// ref: re/analysis/frontend_round5_frontier.md ; listing 0x00430670..0x00430754
// ---------------------------------------------------------------------------

#include "../Core/HookSystem.h"

// 0x00430670
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl MenuOrdinalToPlayerSlot()
{
    __asm {
        sub  esp, 0x10                                  // 0x430670
        mov  ecx, dword ptr ds:[0x0067e9fc]             // 0x430673  (ds: forces a mem load; bare [imm] assembles as mov-immediate in MSVC inline asm)
        or   eax, 0xffffffff                            // 0x430679
        cmp  ecx, 0xa                                   // 0x43067c
        push esi                                        // 0x43067f
        push edi                                        // 0x430680
        mov  dword ptr [esp+0x8], eax                   // 0x430681
        mov  dword ptr [esp+0xc], eax                   // 0x430685
        mov  dword ptr [esp+0x10], eax                  // 0x430689
        mov  dword ptr [esp+0x14], eax                  // 0x43068d
        jnz  L6cc                                       // 0x430691
        mov  esi, dword ptr [esp+0x1c]                  // 0x430693  param_1
        xor  ecx, ecx                                   // 0x430697
        xor  edx, edx                                   // 0x430699
        jmp  L6a0                                       // 0x43069b
    L6a0:
        cmp  edx, esi                                   // 0x4306a0
        jge  L6b8                                       // 0x4306a2
        mov  edi, dword ptr [ecx*4 + 0x0067ea98]        // 0x4306a4
        test edi, edi                                   // 0x4306ab
        jz   L6b0                                       // 0x4306ad
        inc  edx                                        // 0x4306af
    L6b0:
        inc  ecx                                        // 0x4306b0
        cmp  ecx, 0x4                                   // 0x4306b1
        jl   L6a0                                       // 0x4306b4
        cmp  edx, esi                                   // 0x4306b6  (loop fall-through only)
    L6b8:
        jnz  L74f                                       // 0x4306b8
        mov  eax, dword ptr [ecx*4 + 0x0067ea94]        // 0x4306be
        pop  edi                                        // 0x4306c5
        dec  eax                                        // 0x4306c6
        pop  esi                                        // 0x4306c7
        add  esp, 0x10                                  // 0x4306c8
        ret                                             // 0x4306cb
    L6cc:
        mov  eax, dword ptr ds:[0x007f1a1c]             // 0x4306cc
        mov  dword ptr [esp+0x8], eax                   // 0x4306d1
        mov  ecx, 0x00413f90                            // 0x4306d5  CALL FUN_00413f90
        call ecx
        mov  ecx, dword ptr ds:[0x0067f17c]             // 0x4306da
        lea  ecx, [ecx + ecx*2]                         // 0x4306e0
        shl  ecx, 0x4                                   // 0x4306e3
        add  eax, ecx                                   // 0x4306e6
        mov  ecx, dword ptr ds:[0x0067e9fc]             // 0x4306e8
        cmp  ecx, 0x4                                   // 0x4306ee
        jnz  L6f8                                       // 0x4306f1
        add  eax, 0x10                                  // 0x4306f3
        jmp  L700                                       // 0x4306f6
    L6f8:
        cmp  ecx, 0x5                                   // 0x4306f8
        jnz  L700                                       // 0x4306fb
        add  eax, 0x20                                  // 0x4306fd
    L700:
        push ebp                                        // 0x430700
        mov  ebp, dword ptr [eax]                       // 0x430701
        add  eax, 0x4                                   // 0x430703
        xor  edi, edi                                   // 0x430706
        test ebp, ebp                                   // 0x430708
        mov  edx, eax                                   // 0x43070a
        jle  L746                                       // 0x43070c
        push ebx                                        // 0x43070e
        mov  ebx, dword ptr ds:[0x007f1a1c]             // 0x43070f
    L715:
        mov  esi, dword ptr [eax]                       // 0x430715
        add  eax, 0x4                                   // 0x430717
        cmp  esi, ebx                                   // 0x43071a
        jnz  L73c                                       // 0x43071c
        xor  ecx, ecx                                   // 0x43071e
    L720:
        cmp  ecx, ebx                                   // 0x430720
        jz   L732                                       // 0x430722
        cmp  ecx, dword ptr [edx]                       // 0x430724
        jz   L732                                       // 0x430726
        cmp  ecx, dword ptr [edx+0x4]                   // 0x430728
        jz   L732                                       // 0x43072b
        cmp  ecx, dword ptr [edx+0x8]                   // 0x43072d
        jnz  L73a                                       // 0x430730
    L732:
        inc  ecx                                        // 0x430732
        cmp  ecx, 0x6                                   // 0x430733
        jl   L720                                       // 0x430736
        jmp  L73c                                       // 0x430738
    L73a:
        mov  esi, ecx                                   // 0x43073a
    L73c:
        mov  dword ptr [esp + edi*4 + 0x14], esi        // 0x43073c
        inc  edi                                        // 0x430740
        cmp  edi, ebp                                   // 0x430741
        jl   L715                                       // 0x430743
        pop  ebx                                        // 0x430745
    L746:
        mov  edx, dword ptr [esp+0x20]                  // 0x430746
        mov  eax, dword ptr [esp + edx*4 + 0xc]         // 0x43074a
        pop  ebp                                        // 0x43074e
    L74f:
        pop  edi                                        // 0x43074f
        pop  esi                                        // 0x430750
        add  esp, 0x10                                  // 0x430751
        ret                                             // 0x430754
    }
}

RH_ScopedInstall(MenuOrdinalToPlayerSlot, 0x00430670);
