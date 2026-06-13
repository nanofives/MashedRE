// Mashed RE — promote-round round 63 (three container-record setters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All three are __cdecl, self-contained (zero callees), byte-verified from the listing.
// Common shape: base = container[0]; idx = container[2]; addr = base + idx*0x30
// (LEA idx*3 ; SHL 4 -> idx*0x30). Each writes its args into addr + a (negative) field offset.
//
// FUN_00489450 (container, in2*): MOV [addr-0x20],in2[0] ; MOV [addr-0x1c],in2[1]
// FUN_00489480 (container, float): FLD [ESP+8] ; FSTP [addr-0x18]   (single float store)
// FUN_004894a0 (container, in2a*, in2b*): MOV [addr-0x10],in2a[0] ; [addr-0xc],in2a[1] ;
//                                          MOV [addr-0x8],in2b[0] ; [addr-0x4],in2b[1]

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00489450
extern "C" __declspec(dllexport) void __cdecl ContRecSet450(int* cont, const std::uint32_t* in2) {
    std::uint32_t addr = static_cast<std::uint32_t>(cont[0]) + static_cast<std::uint32_t>(cont[2]) * 0x30u;
    *reinterpret_cast<std::uint32_t*>(addr - 0x20) = in2[0];   // MOV [addr-0x20],ESI
    *reinterpret_cast<std::uint32_t*>(addr - 0x1c) = in2[1];   // MOV [addr-0x1c],EAX
}
RH_ScopedInstall(ContRecSet450, 0x00489450);

// 0x00489480
extern "C" __declspec(dllexport) void __cdecl ContRecSet480(int* cont, float val) {
    std::uint32_t addr = static_cast<std::uint32_t>(cont[0]) + static_cast<std::uint32_t>(cont[2]) * 0x30u;
    *reinterpret_cast<float*>(addr - 0x18) = val;             // FSTP [addr-0x18]
}
RH_ScopedInstall(ContRecSet480, 0x00489480);

// 0x004894a0
extern "C" __declspec(dllexport) void __cdecl ContRecSet4a0(int* cont, const std::uint32_t* in2a, const std::uint32_t* in2b) {
    std::uint32_t addr = static_cast<std::uint32_t>(cont[0]) + static_cast<std::uint32_t>(cont[2]) * 0x30u;
    *reinterpret_cast<std::uint32_t*>(addr - 0x10) = in2a[0];
    *reinterpret_cast<std::uint32_t*>(addr - 0xc) = in2a[1];
    *reinterpret_cast<std::uint32_t*>(addr - 0x8) = in2b[0];
    *reinterpret_cast<std::uint32_t*>(addr - 0x4) = in2b[1];
}
RH_ScopedInstall(ContRecSet4a0, 0x004894a0);
