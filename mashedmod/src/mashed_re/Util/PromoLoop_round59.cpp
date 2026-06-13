// Mashed RE — promote-round round 59 (two get-and-return-with-ptrout accessors).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
// Same record base 0x0063e4b8 + idx*0x24 (idx*9 dwords), different field offsets.
//
// FUN_00420da0 (0x00420da0):
//   8b442404      MOV EAX,[ESP+4]          ; idx
//   8b4c2408      MOV ECX,[ESP+8]          ; out
//   85c9          TEST ECX,ECX
//   8d04c0        LEA EAX,[EAX+EAX*8]      ; idx*9
//   8d0485b8e46300 LEA EAX,[EAX*4+0x63e4b8] ; addr = 0x63e4b8 + idx*0x24
//   7404          JZ  +4                    ; out==0 -> skip store
//   8b10 8911     MOV EDX,[EAX] ; MOV [ECX],EDX   ; *out = *(u32*)(addr+0)
//   8b4008        MOV EAX,[EAX+8]           ; return *(u32*)(addr+8)
//
// FUN_00420dc0 (0x00420dc0): identical, but *out = *(addr+4) and return *(addr+0xc).

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00420da0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TblRetPtrout63e4b8_0(std::uint32_t idx, std::uint32_t* out) {
    std::uint32_t a = 0x0063e4b8u + idx * 0x24u;                  // LEA idx*9 ; LEA *4 + 0x63e4b8
    if (out) *out = *reinterpret_cast<std::uint32_t*>(a + 0);     // JZ guard ; MOV [out],[addr+0]
    return *reinterpret_cast<std::uint32_t*>(a + 8);             // MOV EAX,[addr+8]
}
RH_ScopedInstall(TblRetPtrout63e4b8_0, 0x00420da0);

// 0x00420dc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TblRetPtrout63e4b8_4(std::uint32_t idx, std::uint32_t* out) {
    std::uint32_t a = 0x0063e4b8u + idx * 0x24u;
    if (out) *out = *reinterpret_cast<std::uint32_t*>(a + 4);     // *out = *(addr+4)
    return *reinterpret_cast<std::uint32_t*>(a + 0xc);          // return *(addr+0xc)
}
RH_ScopedInstall(TblRetPtrout63e4b8_4, 0x00420dc0);
