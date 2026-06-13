// Mashed RE — promote-round round 69 (60-dword contiguous zero-fill + Copter linear-scan find).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00413f20 (0x00413f20): contiguous zero-fill of [0x0063bc60, 0x0063bd50) (60 dwords).
//   MOV EAX,0x63bc60 ; loop: MOV ECX,5 ; inner: [EAX+8]=0 [EAX]=0 [EAX+4]=0 ADD EAX,0xc DEC ECX JNZ ;
//   CMP EAX,0x63bd50 ; JL loop
//
// FUN_00407640 (0x00407640): linear scan for a key over the Copter record array.
//   EDX = count @ 0x0063a5d0 ; if (count<=0) return -1 ; ESI = key (arg) ; ECX = 0x639dc4
//   loop: CMP [ECX],ESI ; JZ -> return EAX(index) ; INC EAX ; ADD ECX,0xec ; CMP EAX,EDX ; JL loop
//   not found -> OR EAX,-1 (return -1)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00413f20
extern "C" __declspec(dllexport) void __cdecl ZeroFill60_63bc60(void) {
    for (std::uint32_t p = 0x0063bc60u; static_cast<std::int32_t>(p) < 0x0063bd50; p += 4)
        *reinterpret_cast<std::uint32_t*>(p) = 0;
}
RH_ScopedInstall(ZeroFill60_63bc60, 0x00413f20);

// 0x00407640
extern "C" __declspec(dllexport) int __cdecl CopterFind639dc4(int key) {
    int count = *reinterpret_cast<int*>(0x0063a5d0u);                       // MOV EDX,[0x63a5d0]
    char* p = reinterpret_cast<char*>(0x00639dc4u);                        // MOV ECX,0x639dc4
    for (int k = 0; k < count; ++k) {
        if (*reinterpret_cast<int*>(p) == key) return k;                   // CMP [ECX],ESI / JZ
        p += 0xec;                                                          // ADD ECX,0xec
    }
    return -1;                                                              // OR EAX,-1
}
RH_ScopedInstall(CopterFind639dc4, 0x00407640);
