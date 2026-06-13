// Mashed RE — promote-round round 33 (byte-scan batch: getters + const setter).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified; callers confirmed C2 (round-33 reference_to):
//   0x004075a0  Global63a5d0Get — A1 [0x0063a5d0]   (caller 00464a50 audio C2)
//   0x0041e140  Global63d7e0Get — A1 [0x0063d7e0]   (caller 00429e10 render C2)
//   0x0047ce70  Global6c6eb0Get — A1 [0x006c6eb0]   (caller 00459620 gameplay C2)
//   0x00496910  Global772ffcGet — A1 [0x00772ffc]   (caller 004332a0 frontend C2)
//   0x0042b950  Set7f1a0c_1000  — [0x007f1a0c]=0x1000 (caller 004030d0 util C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004075a0  A1 D0 A5 63 00 C3  (mov eax,[0x0063a5d0]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global63a5d0Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0063a5d0u);  // cited at 0x004075a0
}
RH_ScopedInstall(Global63a5d0Get, 0x004075a0);

// 0x0041e140  A1 E0 D7 63 00 C3  (mov eax,[0x0063d7e0]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global63d7e0Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x0063d7e0u);  // cited at 0x0041e140
}
RH_ScopedInstall(Global63d7e0Get, 0x0041e140);

// 0x0047ce70  A1 B0 6E 6C 00 C3  (mov eax,[0x006c6eb0]; ret)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global6c6eb0Get(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x006c6eb0u);  // cited at 0x0047ce70
}
RH_ScopedInstall(Global6c6eb0Get, 0x0047ce70);

// 0x00496910  A1 FC 2F 77 00 C3  (mov eax,[0x00772ffc]; ret) — timer-table base dword
extern "C" __declspec(dllexport) std::uint32_t __cdecl Global772ffcGet(void) {
    return *reinterpret_cast<const std::uint32_t*>(0x00772ffcu);  // cited at 0x00496910
}
RH_ScopedInstall(Global772ffcGet, 0x00496910);

// 0x0042b950  C7 05 0C 1A 7F 00 00 10 00 00 C3  (mov dword [0x007f1a0c],0x1000; ret)
extern "C" __declspec(dllexport) void __cdecl Set7f1a0c_1000(void) {
    *reinterpret_cast<std::uint32_t*>(0x007f1a0cu) = 0x1000u;  // cited at 0x0042b950
}
RH_ScopedInstall(Set7f1a0c_1000, 0x0042b950);
