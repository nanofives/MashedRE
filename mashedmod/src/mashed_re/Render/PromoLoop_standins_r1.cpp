// Mashed RE: stand-in promotion round 1 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Rows that allocate through the RwGlobals memory slot *(0x007d3ff8) + 0x108 (rwmalloc(size, hint), 2 cdecl args;
// re/analysis/rwglobals_layout_20260911.md). A/B-tested under the heap stand-in (re/frida/js/standins.js), which
// carves both arms' blocks from one arena, so the slot must be read through *(0x007d3ff8) as the original does.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using RwMallocFn = void*(__cdecl*)(std::uint32_t size, std::uint32_t hint);
using ZeroFillFn = void(__cdecl*)(void* p, std::uint32_t n);   // 0x004b6520 ZeroFillWrapper (C3)

inline RwMallocFn RwMallocSlot() {
    const std::uintptr_t rwg = *reinterpret_cast<const std::uintptr_t*>(0x007d3ff8u);
    return *reinterpret_cast<RwMallocFn*>(rwg + 0x108u);
}
}  // namespace

// ===========================================================================
// 0x00474db0  RwMallocArrayZeroFill  (FUN_00474db0, 42 B, cdecl(count, stride) -> block)
//   0x00474db0  mov eax, [0x007d3ff8]
//   0x00474db6  mov esi, [esp+8]             count
//   0x00474dba  imul esi, [esp+0xc]          n = count * stride (low 32 bits)
//   0x00474dc0  push 0 / push esi
//   0x00474dc3  call [eax+0x108]             p = rwmalloc(n, 0)
//   0x00474dcb  push esi / push edi
//   0x00474dcd  call 0x004b6520              ZeroFillWrapper(p, n), also when p is NULL
//   0x00474dd5  mov eax, edi                 return p
// ===========================================================================
// 0x00474db0
extern "C" __declspec(dllexport) void* __cdecl RwMallocArrayZeroFill(std::uint32_t count, std::uint32_t stride) {
    const std::uint32_t n = count * stride;                       // 0x00474dba
    void* p = RwMallocSlot()(n, 0);                               // 0x00474dc3
    reinterpret_cast<ZeroFillFn>(0x004b6520u)(p, n);              // 0x00474dcd
    return p;                                                     // 0x00474dd5
}
RH_ScopedInstall(RwMallocArrayZeroFill, 0x00474db0);

// ===========================================================================
// 0x004759b0  Arrays691600Alloc  (FUN_004759b0, 68 B, cdecl(n) -> last block; caller 0x0040bbb0)
//   0x004759b8  mov [0x00691600], esi        n
//   0x004759be  call 0x00474db0 (n, 0x40)  -> [0x00691608]   (0x004759c6)
//   0x004759cb  call 0x00474db0 (n, 4)     -> [0x00691604]   (0x004759d3)
//   0x004759d8  call 0x00474db0 (n, 4)     -> [0x00691610]   (0x004759e0)
//   0x004759e5  call 0x00474db0 (n, 4)     -> [0x00691614]   (0x004759ed)
//   EAX on return is the fourth block (nothing overwrites it after 0x004759e5).
// ===========================================================================
// 0x004759b0
extern "C" __declspec(dllexport) void* __cdecl Arrays691600Alloc(std::uint32_t n) {
    *reinterpret_cast<std::uint32_t*>(0x00691600u) = n;                          // 0x004759b8
    *reinterpret_cast<void**>(0x00691608u) = RwMallocArrayZeroFill(n, 0x40u);    // 0x004759c6
    *reinterpret_cast<void**>(0x00691604u) = RwMallocArrayZeroFill(n, 4u);       // 0x004759d3
    *reinterpret_cast<void**>(0x00691610u) = RwMallocArrayZeroFill(n, 4u);       // 0x004759e0
    void* last = RwMallocArrayZeroFill(n, 4u);
    *reinterpret_cast<void**>(0x00691614u) = last;                               // 0x004759ed
    return last;
}
RH_ScopedInstall(Arrays691600Alloc, 0x004759b0);

// ===========================================================================
// 0x00484170  Rec18Create  (FUN_00484170, 82 B, cdecl(a1, a2, a3) -> 0x18-byte block or NULL; caller 0x00402750)
//   0x00484175  push 0 / push 0x18 / call [eax+0x108]    r = rwmalloc(0x18, 0)
//   0x00484182  test eax, eax / je 0x004841bf             NULL -> return 0 (0x004841bf xor eax, eax)
//   0x00484186  ecx = a3 (esp+0xc): r[0] = a3[0] (0x0048418c), r[1] = a3[1] (0x00484195)
//   0x00484191  edx = a2 (esp+8):   r[2] = a2[0]          (0x0048419e)
//   0x0048419a  edx = a1 (esp+4):   fld [a1+0x80] / fdivr [0x005cc320] (= 1.0f, 0x3f800000)
//   0x004841ad  r[4] = 0, 0x004841b4 r[5] = 0, 0x004841bb fstp dword [r+0xc]   r[3] = 1.0f / a1.f[0x80]
// A single x87 division rounded once to float equals the SSE single division (64-bit intermediate >= 2*24+2).
// ===========================================================================
// 0x00484170
extern "C" __declspec(dllexport) void* __cdecl Rec18Create(const std::uint8_t* a1, const std::uint32_t* a2,
                                                           const std::uint32_t* a3) {
    std::uint32_t* r = static_cast<std::uint32_t*>(RwMallocSlot()(0x18u, 0));   // 0x00484179
    if (!r) return nullptr;                                                     // 0x00484184
    r[0] = a3[0];                                                               // 0x0048418c
    r[1] = a3[1];                                                               // 0x00484195
    r[2] = a2[0];                                                               // 0x0048419e
    const float x = *reinterpret_cast<const float*>(a1 + 0x80);                 // 0x004841a1
    r[4] = 0;                                                                   // 0x004841ad
    r[5] = 0;                                                                   // 0x004841b4
    *reinterpret_cast<float*>(&r[3]) = 1.0f / x;                                // 0x004841a7 / 0x004841bb
    return r;
}
RH_ScopedInstall(Rec18Create, 0x00484170);
