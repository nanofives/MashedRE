// Mashed RE - PowerupTypeLookup (0x0045baa0), the one body in both targets (dcmix 2026-10-22).
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// Disasm (anchored binary, 0x0045baa0..0x0045babf):
//   0x0045baa0  mov edx, [0x5f9bd8]      ; count
//   0x0045baa6  xor ecx, ecx
//   0x0045baa8  test edx, edx
//   0x0045baaa  jle 0x45babd             ; count <= 0 -> return 0
//   0x0045baac  mov eax, 0x5f9998        ; base
//   0x0045bab1  cmp esi, [eax]           ; key (ESI) vs entry +0x00
//   0x0045bab3  je 0x45babf              ; match -> return the entry
//   0x0045bab5  inc ecx
//   0x0045bab6  add eax, 0x40            ; stride
//   0x0045bab9  cmp ecx, edx
//   0x0045babb  jl 0x45bab1
//   0x0045babd  xor eax, eax
//   0x0045babf  ret
//
// The table base, count and stride are the target's inputs:
//   .asi  Util/PromoLoop_sessionB.cpp Search45baa0: 0x005f9998, [0x005f9bd8], 0x40 (the original's data).
//   exe   Powerup/PowerupSystem.cpp Lookup: s_table, 9, sizeof(TypeEntry); TypeEntry::code is at +0x00.
#include "PowerupTypeSearch.h"

// 0x0045baa0
const std::uint8_t* PowerupTypeSearch(std::uint32_t key, const std::uint8_t* base, int count, int stride) {
    if (count <= 0) return nullptr;                                   // 0x0045baaa
    for (int i = 0; i < count; ++i, base += stride)                   // 0x0045bab5..0x0045babb
        if (*reinterpret_cast<const std::uint32_t*>(base) == key)     // 0x0045bab1
            return base;
    return nullptr;                                                   // 0x0045babd
}
