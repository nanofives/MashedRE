// Mashed RE — promote-round round 66 (per-player flag-bit toggle + gated switch predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_0041ef80 (0x0041ef80): per-player flag-bit toggle.
//   MOV EAX,[ESP+4] ; MOV ECX,[ESP+8] ; IMUL EAX,0x2ac ; ADD EAX,0x63d9e0
//   TEST ECX,ECX ; MOV ECX,[EAX+0x294] ; JZ clear
//   OR ECX,0x400 ; MOV [EAX+0x294],ECX ; RET          ; set != 0
//   clear: AND ECX,0xfffffbff ; MOV [EAX+0x294],ECX   ; set == 0
//   -> flag = *(u32*)(0x0063dc74 + idx*0x2ac) (0x63d9e0+0x294); set ? |=0x400 : &=~0x400
//
// FUN_0041f360 (0x0041f360): gated membership predicate.
//   if (*(int*)0x00636ad0 == 0) switch(arg){ 4,5,8,10,0xb,0xe,0xf,0x25,0x26,0x29,0x2b,0x2c,0x2f,0x30: return 1; }
//   return 0;

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041ef80
extern "C" __declspec(dllexport) void __cdecl FlagToggle63dc74(std::uint32_t idx, int set) {
    std::uint32_t* f = reinterpret_cast<std::uint32_t*>(0x0063dc74u + idx * 0x2acu);   // 0x63d9e0 + idx*0x2ac + 0x294
    if (set) *f |= 0x400u;                                                              // OR ECX,0x400
    else     *f &= 0xfffffbffu;                                                         // AND ECX,0xfffffbff
}
RH_ScopedInstall(FlagToggle63dc74, 0x0041ef80);

// 0x0041f360
extern "C" __declspec(dllexport) std::uint32_t __cdecl GatedSwitch636ad0(std::uint32_t a) {
    if (*reinterpret_cast<std::int32_t*>(0x00636ad0u) == 0) {                           // gate
        switch (a) {
        case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf:
        case 0x25: case 0x26: case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30:
            return 1;
        }
    }
    return 0;
}
RH_ScopedInstall(GatedSwitch636ad0, 0x0041f360);
