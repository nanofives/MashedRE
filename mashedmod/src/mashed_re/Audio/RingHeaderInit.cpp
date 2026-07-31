// Mashed RE - ring/cursor header init.
// Original: 0x005b1160  FUN_005b1160  audio  C2 -> C3
// Plate: re/analysis/bucket_audio_005af070_005b2190/0x005b1160.md
//
// Leaf. Writes an 8-byte header into caller-owned memory and returns nothing.
// Touches no globals and calls nothing.
//
// Disasm at 0x005b1160..0x005b117c (29 bytes), read from the listing in
// Mashed_pool9 rather than from the decompiler:
//   0x005B1160  8B 44 24 04    MOV  EAX,dword ptr [ESP+4]    ; header
//   0x005B1164  8B 54 24 08    MOV  EDX,dword ptr [ESP+8]    ; buffer_base
//   0x005B1168  32 C9          XOR  CL,CL
//   0x005B116A  88 48 01       MOV  byte ptr [EAX+1],CL      ; +1 = 0
//   0x005B116D  88 48 02       MOV  byte ptr [EAX+2],CL      ; +2 = 0
//   0x005B1170  88 08          MOV  byte ptr [EAX],CL        ; +0 = 0
//   0x005B1172  8A 4C 24 0C    MOV  CL,byte ptr [ESP+0xc]    ; capacity, BYTE
//   0x005B1176  88 48 03       MOV  byte ptr [EAX+3],CL      ; +3 = capacity
//   0x005B1179  89 50 04       MOV  dword ptr [EAX+4],EDX    ; +4 = buffer_base
//   0x005B117C  C3             RET
//
// TWO details the decompiler output does not make obvious, both taken from the
// listing:
//
//  1. The THIRD argument is read as a BYTE — `MOV CL,byte ptr [ESP+0xc]` at
//     0x005b1172, not a dword load. It is pushed as a dword by the caller (it
//     occupies a full stack slot) but only the low 8 bits are ever consumed, so
//     the parameter is typed `unsigned char` here and the upper 24 bits of the
//     pushed slot are discarded exactly as the original discards them.
//
//  2. The write ORDER is +1, +2, +0, +3, +4 — not ascending. The port
//     reproduces that order. It is not observable through this function alone
//     (all five writes land before the RET), but it would be observable to
//     anything watching the memory concurrently, and reordering it would be a
//     silent, unforced divergence from the instruction stream.
//
// The plate names the fields head / read-index / spare / capacity / buffer-base.
// Only the LAYOUT is transcribed here; those names come from the matching
// dequeue/advance pair 0x005b1180 / 0x005b11d0 and are not re-derived from this
// body, which contains nothing naming them. [UNCERTAIN U-6836] the element
// stride and payload of the ring itself are unestablished — that is a property
// of 0x005b1180, not of this header init, which is fully transcribed.
//
// __cdecl: arguments read from [ESP+4], [ESP+8], [ESP+0xc]; no ESP adjust
// before RET.
#include "../Core/HookSystem.h"

// 0x005b1160
extern "C" __declspec(dllexport) void __cdecl
RingHeaderInit(unsigned char* header, unsigned int buffer_base, unsigned char capacity)
{
    // 0x005b116a/0x005b116d/0x005b1170 — zeroed in the original's order.
    header[1] = 0;
    header[2] = 0;
    header[0] = 0;
    // 0x005b1176 — byte store of the low 8 bits of the third argument.
    header[3] = capacity;
    // 0x005b1179 — dword store of the second argument.
    *(unsigned int*)(header + 4) = buffer_base;
}

RH_ScopedInstall(RingHeaderInit, 0x005b1160);
