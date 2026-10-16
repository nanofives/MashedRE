// Mashed RE - FUN_0042d3e0 (0x0042d3e0): the menu-entry array reset, ONE body for both targets (dualcopy4, 2026-10-16, ROADMAP D4).
//
// The RVA had two bodies: Frontend/MenuInit.cpp:MenuEntryArrayInit (.asi installed, zeroing the original's array at 0x00898ac0) and
// Frontend/MenuNavSM.cpp:RecordsZero (exe, `memset(g_records, 0, ...)`). The first did not match the bytes either (it wrote the dword before the
// entry, bytes +9..+12 instead of +12..+15, +32 instead of +28 and +44), so this body is written from the disassembly, not from either copy.
//
// Original (unpatched MASHED.exe; verify/dualcopy4_20261016/disasm_42d3e0.txt), eax = first_entry + 4, entry = eax - 4, stride 0x34, 30 iterations:
//   0x0042d3e7  mov [eax-4],ecx ; 0x0042d3ea mov [eax],ecx ; 0x0042d3ec mov [eax+4],ecx            dwords at entry +0, +4, +8
//   0x0042d3ef  mov [eax+8],cl ; 0x0042d3f2 [eax+9] ; 0x0042d3f5 [eax+0xa] ; 0x0042d3f8 [eax+0xb]   bytes  at entry +12, +13, +14, +15
//   0x0042d3fb  mov [eax+0xc],ecx ; 0x0042d3fe [eax+0x10] ; 0x0042d401 [eax+0x14] ; 0x0042d404 [eax+0x18]   dwords at entry +16, +20, +24, +28
//   0x0042d407  mov [eax+0x20],ecx ; 0x0042d40a [eax+0x24] ; 0x0042d40d [eax+0x28]                  dwords at entry +36, +40, +44
//   0x0042d410  add eax,0x34 ; 0x0042d413 cmp eax,0x8990dc ; 0x0042d418 jl 0x0042d3e7
// NOT written: entry +32 (the dword at eax+0x1c) and anything past entry 29. A do-while: the body runs at least once.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Frontend {

inline void MenuEntryZeroBody(std::uint8_t* first_entry, int count) {
    std::uint8_t* eax = first_entry + 4;                                 // 0x0042d3e0 mov eax, 0x898ac4
    int i = 0;
    do {
        *reinterpret_cast<std::uint32_t*>(eax - 4)    = 0u;              // 0x0042d3e7  entry +0
        *reinterpret_cast<std::uint32_t*>(eax)        = 0u;              // 0x0042d3ea  entry +4
        *reinterpret_cast<std::uint32_t*>(eax + 4)    = 0u;              // 0x0042d3ec  entry +8
        eax[8] = 0u; eax[9] = 0u; eax[10] = 0u; eax[11] = 0u;            // 0x0042d3ef..0x0042d3f8  entry +12..+15 (four byte stores)
        *reinterpret_cast<std::uint32_t*>(eax + 0xc)  = 0u;              // 0x0042d3fb  entry +16
        *reinterpret_cast<std::uint32_t*>(eax + 0x10) = 0u;              // 0x0042d3fe  entry +20
        *reinterpret_cast<std::uint32_t*>(eax + 0x14) = 0u;              // 0x0042d401  entry +24
        *reinterpret_cast<std::uint32_t*>(eax + 0x18) = 0u;              // 0x0042d404  entry +28
        *reinterpret_cast<std::uint32_t*>(eax + 0x20) = 0u;              // 0x0042d407  entry +36   (entry +32 = eax+0x1c is NOT written)
        *reinterpret_cast<std::uint32_t*>(eax + 0x24) = 0u;              // 0x0042d40a  entry +40
        *reinterpret_cast<std::uint32_t*>(eax + 0x28) = 0u;              // 0x0042d40d  entry +44
        eax += 0x34;                                                     // 0x0042d410
    } while (++i < count);                                               // 0x0042d413 cmp / 0x0042d418 jl
}

}  // namespace Frontend
}  // namespace mashed_re
