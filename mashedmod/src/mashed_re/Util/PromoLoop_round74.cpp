// Mashed RE — promote-round round 74 (strided fill + 2 index->ptr-array getters + 2 multi-bit flag setters).
// This round brings the promote-round total to 200 C2->C3.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All five are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_00453f30 (0x00453f30): strided fill of 0xffffffff.
//   MOV EAX,0x6870b4 ; loop: MOV [EAX],0xffffffff ; ADD EAX,0x24 ; CMP 0x687ec4 ; JL
//
// FUN_00404e00 (0x00404e00): index->ptr-array getter (1 index).
//   idx=*(int*)(0x639cc8 + p1*4) ; if(idx==-1) return 0 ; return *(u32*)(0x5ea0b8 + idx*4)
//
// FUN_00404e20 (0x00404e20): index->ptr-array getter (2 indices, comp=p1*0x4e+p2).
//   idx=*(int*)(0x636c08 + (p1*0x4e+p2)*4) ; if(idx==-1) return 0 ; return *(u32*)(0x5ea0c8 + idx*4)
//
// FUN_0041ede0 (0x0041ede0): per-player flag multi-bit setter (3 args; bits 0x10/0x20 + cascades 0x1000/0x2000).
// FUN_0041eeb0 (0x0041eeb0): per-player flag multi-bit setter (4 args; bits 0x40/0x80/0x800 + 3 clear-cascades).

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00453f30
extern "C" __declspec(dllexport) void __cdecl Fill6870b4(void) {
    for (std::uint32_t p = 0x006870b4u; static_cast<std::int32_t>(p) < 0x00687ec4; p += 0x24u)
        *reinterpret_cast<std::uint32_t*>(p) = 0xffffffffu;
}
RH_ScopedInstall(Fill6870b4, 0x00453f30);

// 0x00404e00
extern "C" __declspec(dllexport) std::uint32_t __cdecl IdxPtr404e00(std::uint32_t p1) {
    std::int32_t idx = *reinterpret_cast<std::int32_t*>(0x00639cc8u + p1 * 4u);   // [EAX*4+0x639cc8]
    if (idx == -1) return 0;                                                       // CMP -1 / JNZ ; XOR EAX,EAX
    return *reinterpret_cast<std::uint32_t*>(0x005ea0b8u + static_cast<std::uint32_t>(idx) * 4u);  // [EAX*4+0x5ea0b8]
}
RH_ScopedInstall(IdxPtr404e00, 0x00404e00);

// 0x00404e20
extern "C" __declspec(dllexport) std::uint32_t __cdecl IdxPtr404e20(std::uint32_t p1, std::uint32_t p2) {
    std::int32_t idx = *reinterpret_cast<std::int32_t*>(0x00636c08u + (p1 * 0x4eu + p2) * 4u);  // IMUL 0x4e ; ADD ; [EAX*4+0x636c08]
    if (idx == -1) return 0;
    return *reinterpret_cast<std::uint32_t*>(0x005ea0c8u + static_cast<std::uint32_t>(idx) * 4u);  // [EAX*4+0x5ea0c8]
}
RH_ScopedInstall(IdxPtr404e20, 0x00404e20);

// 0x0041ede0
extern "C" __declspec(dllexport) void __cdecl Flag0041ede0(std::int32_t p1, std::int32_t p2, std::int32_t p3) {
    std::uint32_t* f = reinterpret_cast<std::uint32_t*>(0x0063dc74u + static_cast<std::uint32_t>(p1) * 0x2acu);
    std::uint32_t v;
    v = (p2 == 0) ? (*f & 0xffffffefu) : (*f | 0x10u); *f = v;
    v = (p3 == 0) ? (*f & 0xffffffdfu) : (*f | 0x20u); *f = v;
    if (v & 0x10u) *f = v | 0x1000u;
    if (*f & 0x20u) *f = *f | 0x2000u;
}
RH_ScopedInstall(Flag0041ede0, 0x0041ede0);

// 0x0041eeb0
extern "C" __declspec(dllexport) void __cdecl Flag0041eeb0(std::int32_t p1, std::int32_t p2, std::int32_t p3, std::int32_t p4) {
    std::uint32_t* f = reinterpret_cast<std::uint32_t*>(0x0063dc74u + static_cast<std::uint32_t>(p1) * 0x2acu);
    std::uint32_t v;
    v = (p2 == 0) ? (*f & 0xffffffbfu) : (*f | 0x40u);  *f = v;
    v = (p3 == 0) ? (*f & 0xffffff7fu) : (*f | 0x80u);  *f = v;
    v = (p4 == 0) ? (*f & 0xfffff7ffu) : (*f | 0x800u); *f = v;
    if (v & 0x400u) *f = v & 0xfffff73fu;
    if (*f & 0x10u) *f = *f & 0xffffffbfu;
    if (*f & 0x20u) *f = *f & 0xfffff77fu;
}
RH_ScopedInstall(Flag0041eeb0, 0x0041eeb0);
