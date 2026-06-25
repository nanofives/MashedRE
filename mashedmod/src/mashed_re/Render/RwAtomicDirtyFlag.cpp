// Mashed RE - RenderWare atomic frame-dirty-flag getter.
// Original: 0x004c0b10  FUN_004c0b10  render_3_c1_to_c2_s6  C2 -> C3
//
// Pure nested-deref leaf: returns the low 2 bits of a flag byte one pointer
// indirection inside the passed object (the "dirty-flag field read" pattern
// shared with FUN_004c0e50 (writer) and FUN_004c0ed0 (bit-0 test)).
//   ret = *(byte*)(*(int*)(this + 0xa0) + 3) & 3
//
// CALLER-GATE (2026-06-25 identified-caller clause, CONFIDENCE.md): the sole
// promotable anchor is RpAtomicGetWorldBoundingSphere (0x004e6100) — a NAMED
// RenderWare API. It stays C1 by library-skip policy but is NOT an island, so it
// satisfies the C2->C3 caller-half. (Other caller FUN_00533ec0 is anonymous.)
//
// Disasm at 0x004c0b10..0x004c0b20 (16 bytes; integer-only, no x87 -> plain C is
// bit-identical):
//   0x004c0b10  8B 44 24 04         mov eax, [esp+4]        ; this
//   0x004c0b14  8B 88 A0 00 00 00   mov ecx, [eax+0xa0]     ; inner = *(this+0xa0)
//   0x004c0b1a  8A 41 03            mov al,  [ecx+3]         ; byte at inner+3
//   0x004c0b1d  83 E0 03            and eax, 3               ; low 2 bits (upper zeroed)
//   0x004c0b20  C3                  ret
#include "../Core/HookSystem.h"

// 0x004c0b10
extern "C" __declspec(dllexport) int __cdecl
RwAtomicDirtyFlagGet(int param_1)
{
    return *(const unsigned char*)(*(const int*)(param_1 + 0xa0) + 3) & 3;
}

RH_ScopedInstall(RwAtomicDirtyFlagGet, 0x004c0b10);
