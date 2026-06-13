// Mashed RE — promote-round round 39 (first-party multi-store const setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Byte-verified pure 2-store leaves (no calls): C7 05 <g1> 0 C7 05 <g2> 0 C3.
// Diffed via early_window_leaf_diff scalars_to_scattered_globals (observes BOTH
// written globals: fill sentinel -> call -> read both -> compare). Callers C2:
//   0x0041d910  Clear63d584Pair — [0x0063d584]=0,[0x0063d588]=0 (caller 0040dbd0 util C2)
//   0x00429820  Clear8991b0Pair — [0x008991b0]=0,[0x008991b4]=0 (caller 0040e590 util C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041d910  C7 05 84 D5 63 00 00 00 00 00 C7 05 88 D5 63 00 00 00 00 00 C3
extern "C" __declspec(dllexport) void __cdecl Clear63d584Pair(void) {
    *reinterpret_cast<std::uint32_t*>(0x0063d584u) = 0u;  // cited at 0x0041d910
    *reinterpret_cast<std::uint32_t*>(0x0063d588u) = 0u;  // cited at 0x0041d91a
}
RH_ScopedInstall(Clear63d584Pair, 0x0041d910);

// 0x00429820  C7 05 B0 91 89 00 00 00 00 00 C7 05 B4 91 89 00 00 00 00 00 C3
extern "C" __declspec(dllexport) void __cdecl Clear8991b0Pair(void) {
    *reinterpret_cast<std::uint32_t*>(0x008991b0u) = 0u;  // cited at 0x00429820
    *reinterpret_cast<std::uint32_t*>(0x008991b4u) = 0u;  // cited at 0x0042982a
}
RH_ScopedInstall(Clear8991b0Pair, 0x00429820);
