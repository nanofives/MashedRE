// Mashed RE — promote-round round 51 (palette-array initializer).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_004723d0 (decomp-verified, self-contained, no calls): inits the 10-entry
// 0x10-byte palette array at 0x00691500. Each entry i (0..9):
//   [+0]=0xff00ffff|((i+1)<<16)  [+4]=0xff003cff|((i+1)<<16)  [+8]=0  [+0xc]=0
// The diff compares the final 0xa0-byte memory state (range_init), so an
// equivalent loop reimpl is bit-identical. Caller FUN_00402750 boot C2.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004723d0
extern "C" __declspec(dllexport) void __cdecl Init691500(void) {
    for (std::uint32_t i = 0; i < 10u; ++i) {
        std::uint8_t* e = reinterpret_cast<std::uint8_t*>(0x00691500u + i * 0x10u);
        *reinterpret_cast<std::uint32_t*>(e + 0x0) = 0xff00ffffu | ((i + 1u) << 16);  // cited at 0x004723d0
        *reinterpret_cast<std::uint32_t*>(e + 0x4) = 0xff003cffu | ((i + 1u) << 16);
        *reinterpret_cast<std::uint32_t*>(e + 0x8) = 0u;
        *reinterpret_cast<std::uint32_t*>(e + 0xc) = 0u;
    }
}
RH_ScopedInstall(Init691500, 0x004723d0);
