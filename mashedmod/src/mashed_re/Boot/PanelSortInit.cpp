// PanelSortInit — 0x00420d00
//
// One-file class per gta-reversed convention. Reimplementation of FUN_00420d00.
//
// Original (0x00420d00..0x00420d3a, 59 bytes). `void __cdecl FUN_00420d00(void)`.
// Zero-fills 0x90 (144) bytes at DAT_0063e4b8, then writes the constants 0/1/2/3
// into 4 dwords at stride 0x24 within that block:
//   [0x0063e4d0] = 0   (offset +0x18 from base)
//   [0x0063e4f4] = 1   (offset +0x3c from base)
//   [0x0063e518] = 2   (offset +0x60 from base)
//   [0x0063e53c] = 3   (offset +0x84 from base)
//
// Pure integer / dword copy — no x87 arithmetic. Verbatim transcription; the one
// non-verbatim substitution: CALL relative -> `mov eax, 0x004b6520; call eax`
// (EAX is dead across it). `ds:` prefix required on ALL absolute-address operands
// (both reads and writes) in MSVC inline asm — bare [imm] is a C2415 error.
//
// DEV-HOOK TU only (asi_sources.rsp); NOT the greenfield exe (absolute MASHED VAs).
// Callee FUN_004b6520 ZeroFillWrapper C3.
// listing: 0x00420d00..0x00420d3a
// ---------------------------------------------------------------------------

#include "../Core/HookSystem.h"

// 0x00420d00
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl PanelSortInit()
{
    __asm {
        push 0x90                                // 00420d00  PUSH 0x90
        push 0x0063e4b8                          // 00420d05  PUSH &DAT_0063e4b8
        mov  eax, 0x004b6520                     // 00420d0a  CALL FUN_004b6520
        call eax
        add  esp, 8                              // 00420d0f
        mov  dword ptr ds:[0x0063e4d0], 0        // 00420d12  [0x0063e4d0] = 0
        mov  dword ptr ds:[0x0063e4f4], 1        // 00420d1c  [0x0063e4f4] = 1
        mov  dword ptr ds:[0x0063e518], 2        // 00420d26  [0x0063e518] = 2
        mov  dword ptr ds:[0x0063e53c], 3        // 00420d30  [0x0063e53c] = 3
        ret                                      // 00420d3a
    }
}

RH_ScopedInstall(PanelSortInit, 0x00420d00);
