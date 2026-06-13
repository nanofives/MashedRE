// Mashed RE — promote-round round 77 (strided -1.0f fill + trailing global zero; opportunistic, beyond 200).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_00421080 (0x00421080): __cdecl, self-contained (zero callees), byte-verified.
//   MOV EAX,0x63e5a4 ; loop: MOV [EAX],0xbf800000 ; ADD EAX,0x2c ; CMP 0x63fba4 ; JL
//   MOV [0x0063fb88],0
//   -> strided fill 0xbf800000 (-1.0f) over [0x0063e5a4, 0x0063fba4) step 0x2c; then *(u32*)0x0063fb88 = 0
//      (0x0063fb88 lies inside the filled range, so the trailing store overwrites that slot to 0)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00421080
extern "C" __declspec(dllexport) void __cdecl Fill63e5a4(void) {
    for (std::uint32_t p = 0x0063e5a4u; static_cast<std::int32_t>(p) < 0x0063fba4; p += 0x2cu)
        *reinterpret_cast<std::uint32_t*>(p) = 0xbf800000u;     // MOV [EAX],0xbf800000
    *reinterpret_cast<std::uint32_t*>(0x0063fb88u) = 0;         // MOV [0x0063fb88],0
}
RH_ScopedInstall(Fill63e5a4, 0x00421080);
