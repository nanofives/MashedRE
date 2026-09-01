// MenuSlotInit — 0x004224d0
//
// One-file class per gta-reversed convention. Reimplementation of FUN_004224d0.
//
// Original (0x004224d0..0x0042256d, 158 bytes). `void __cdecl FUN_004224d0(int slot,
// uint32_t* src4, uint32_t p3)`. Initialises one 0xf40-byte slot of the array rooted at
// DAT_006403e8 (stride 0xf40): zero-fills the slot via FUN_004b6520 (ZeroFillWrapper, C3)
// then writes ~15 fields at fixed offsets within the slot:
//   +0xf00..+0xf0c = src4[0..3]           (the 4 dwords behind the pointer arg)
//   +0xf1c,+0xf24  = 0x3f000000 (0.5f)
//   +0xf28         = DAT_005f6154          (a global scalar, bare-absolute load)
//   +0xf2c         = slot                  (the index itself)
//   +0xf10         = 0xbfe66666            +0xf14 = 0x3f800000 (1.0f)
//   +0xf18         = 0xbf59999a            +0xf20,+0xf34 = 0
//   +0xf38         = p3
//
// Pure integer/dword-copy transform (no x87 arithmetic; the float constants are copied as
// raw dwords). The naked body is a verbatim transcription of the original instruction
// stream, so the written slot is bit-identical by construction. Two non-verbatim but
// functionally identical substitutions: the CALL to FUN_004b6520 (0x4224ea `e8 rel32` ->
// `mov eax,0x004b6520; call eax`, ECX/EAX are dead across it) and the bare-absolute load
// at 0x422512 needs `ds:` (MSVC inline asm assembles `mov edx,[imm]` as mov-IMMEDIATE, the
// r6 0x00430670 lesson).
//
// DEV-HOOK TU only (asi_sources.rsp), NOT the greenfield exe: absolute MASHED addresses.
// Callee FUN_004b6520 ZeroFillWrapper C3. ref: re/analysis/frontend_round5_frontier.md
// (r7 addendum) ; listing 0x004224d0..0x0042256d
// ---------------------------------------------------------------------------

#include "../Core/HookSystem.h"

// 0x004224d0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MenuSlotInit()
{
    __asm {
        push esi                                        // 0x4224d0
        push edi                                        // 0x4224d1
        mov  edi, dword ptr [esp+0xc]                   // 0x4224d2  slot
        mov  esi, edi                                   // 0x4224d6
        imul esi, esi, 0xf40                            // 0x4224d8
        add  esi, 0x006403e8                            // 0x4224de  slot base ptr
        push 0xf40                                      // 0x4224e4
        push esi                                        // 0x4224e9
        mov  eax, 0x004b6520                            // 0x4224ea  CALL FUN_004b6520(esi,0xf40)
        call eax
        mov  ecx, dword ptr [esp+0x18]                  // 0x4224ef  src4
        mov  dword ptr [esi+0xf2c], edi                 // 0x4224f3  slot -> +0xf2c
        mov  edx, dword ptr [ecx]                       // 0x4224f9  src4[0]
        lea  eax, [esi+0xf00]                           // 0x4224fb
        mov  dword ptr [eax], edx                       // 0x422501  -> +0xf00
        mov  edx, dword ptr [ecx+0x4]                   // 0x422503  src4[1]
        mov  dword ptr [eax+0x4], edx                   // 0x422506  -> +0xf04
        mov  edx, dword ptr [ecx+0x8]                   // 0x422509  src4[2]
        mov  dword ptr [eax+0x8], edx                   // 0x42250c  -> +0xf08
        mov  ecx, dword ptr [ecx+0xc]                   // 0x42250f  src4[3]
        mov  edx, dword ptr ds:[0x005f6154]             // 0x422512  DAT_005f6154 (ds: forces mem load)
        mov  dword ptr [eax+0xc], ecx                   // 0x422518  -> +0xf0c
        mov  eax, 0x3f000000                            // 0x42251b
        add  esp, 0x8                                   // 0x422520  clean up the call args
        mov  dword ptr [esi+0xf1c], eax                 // 0x422523  0.5f -> +0xf1c
        mov  dword ptr [esi+0xf24], eax                 // 0x422529  0.5f -> +0xf24
        mov  eax, dword ptr [esp+0x14]                  // 0x42252f  p3
        xor  ecx, ecx                                   // 0x422533
        pop  edi                                        // 0x422535
        mov  dword ptr [esi+0xf28], edx                 // 0x422536  DAT_005f6154 -> +0xf28
        mov  dword ptr [esi+0xf34], ecx                 // 0x42253c  0 -> +0xf34
        mov  dword ptr [esi+0xf10], 0xbfe66666          // 0x422542  -> +0xf10
        mov  dword ptr [esi+0xf14], 0x3f800000          // 0x42254c  1.0f -> +0xf14
        mov  dword ptr [esi+0xf18], 0xbf59999a          // 0x422556  -> +0xf18
        mov  dword ptr [esi+0xf20], ecx                 // 0x422560  0 -> +0xf20
        mov  dword ptr [esi+0xf38], eax                 // 0x422566  p3 -> +0xf38
        pop  esi                                        // 0x42256c
        ret                                             // 0x42256d
    }
}

RH_ScopedInstall(MenuSlotInit, 0x004224d0);
