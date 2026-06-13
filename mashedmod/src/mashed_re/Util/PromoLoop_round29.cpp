// Mashed RE — promote-round round 29 (worklist batch: const global setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All bodies byte-verified as `C7 05 <addr32> <imm32> C3` (single immediate
// store + ret, void()). All callers confirmed C2 (round-29 reference_to):
//   0x00493570  Set77196c_1   — [0x0077196c]=1  (caller 0043d7c0 util  C2)
//   0x00493580  Set771970_1   — [0x00771970]=1  (caller 0043d7c0 util  C2)
//   0x00462510  Set603868_0   — [0x00603868]=0  (caller 0040d270 Course::Finish track C2)
//   0x00462500  Set603868_1   — [0x00603868]=1  (caller 0040d270 Course::Finish track C2)
//   0x00487df0  Set703058_0   — [0x00703058]=0  (caller 0040bd80 util  C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00493570  C7 05 6C 19 77 00 01 00 00 00 C3  (mov dword [0x0077196c],1; ret)
extern "C" __declspec(dllexport) void __cdecl Set77196c_1(void) {
    *reinterpret_cast<std::uint32_t*>(0x0077196cu) = 1u;  // cited at 0x00493570
}
RH_ScopedInstall(Set77196c_1, 0x00493570);

// 0x00493580  C7 05 70 19 77 00 01 00 00 00 C3  (mov dword [0x00771970],1; ret)
extern "C" __declspec(dllexport) void __cdecl Set771970_1(void) {
    *reinterpret_cast<std::uint32_t*>(0x00771970u) = 1u;  // cited at 0x00493580
}
RH_ScopedInstall(Set771970_1, 0x00493580);

// 0x00462510  C7 05 68 38 60 00 00 00 00 00 C3  (mov dword [0x00603868],0; ret)
extern "C" __declspec(dllexport) void __cdecl Set603868_0(void) {
    *reinterpret_cast<std::uint32_t*>(0x00603868u) = 0u;  // cited at 0x00462510
}
RH_ScopedInstall(Set603868_0, 0x00462510);

// 0x00462500  C7 05 68 38 60 00 01 00 00 00 C3  (mov dword [0x00603868],1; ret)
//   Set/clear pair with Set603868_0 over the same global 0x00603868.
extern "C" __declspec(dllexport) void __cdecl Set603868_1(void) {
    *reinterpret_cast<std::uint32_t*>(0x00603868u) = 1u;  // cited at 0x00462500
}
RH_ScopedInstall(Set603868_1, 0x00462500);

// 0x00487df0  C7 05 58 30 70 00 00 00 00 00 C3  (mov dword [0x00703058],0; ret)
extern "C" __declspec(dllexport) void __cdecl Set703058_0(void) {
    *reinterpret_cast<std::uint32_t*>(0x00703058u) = 0u;  // cited at 0x00487df0
}
RH_ScopedInstall(Set703058_0, 0x00487df0);
