// Mashed RE — promote-round round 68 (strided 30-global zero-init + 64-dword constant fill).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00406370 (0x00406370): zeroes +0/+4/+8 of each of 10 records (stride 0x20) at 0x0063a494.
//   XOR EAX,EAX ; 30x MOV [global],EAX  (records 0x63a494 + r*0x20, r=0..9; fields +0,+4,+8)
//
// FUN_00418e50 (0x00418e50): REP STOSD 64 dwords of 0x3fe00000 starting at 0x0063c508.
//   PUSH EDI ; MOV ECX,0x40 ; MOV EAX,0x3fe00000 ; MOV EDI,0x63c508 ; REP STOSD ; POP EDI

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00406370
extern "C" __declspec(dllexport) void __cdecl Clear10x3_63a494(void) {
    for (std::uint32_t r = 0; r < 10u; ++r) {
        std::uint32_t b = 0x0063a494u + r * 0x20u;
        *reinterpret_cast<std::uint32_t*>(b + 0) = 0;
        *reinterpret_cast<std::uint32_t*>(b + 4) = 0;
        *reinterpret_cast<std::uint32_t*>(b + 8) = 0;
    }
}
RH_ScopedInstall(Clear10x3_63a494, 0x00406370);

// 0x00418e50
extern "C" __declspec(dllexport) void __cdecl Fill64_63c508(void) {
    for (std::uint32_t k = 0; k < 0x40u; ++k)
        *reinterpret_cast<std::uint32_t*>(0x0063c508u + k * 4u) = 0x3fe00000u;
}
RH_ScopedInstall(Fill64_63c508, 0x00418e50);
