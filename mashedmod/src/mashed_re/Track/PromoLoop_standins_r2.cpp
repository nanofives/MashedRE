// Mashed RE: stand-in promotion round 2 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x0047bcc0  TriEdgePairsBuild  (FUN_0047bcc0, 434 B; caller 0x0047bf70)
//   ABI: EAX = n (triangle count, signed: `jle` at 0x0047bd0a), stack cdecl (out, tris); EBX/EBP/ESI/EDI preserved,
//   frame `sub esp, 0x10` + 4 pushes, `add esp, 0x10; ret` at 0x0047be6e. No meaningful return value (EAX is
//   whatever rwfree leaves).
//   tris: 8-byte records, u16 vertex indices at +0 / +2 / +4 (movzx at 0x0047bd10 / 0x0047bd16 / 0x0047bd35).
//   out:  out[0] = block of 0x18-byte edge records, out[1] = record count (0x0047be27 / 0x0047be29).
//
//   0x0047bccd  ebp = 3n; 0x0047bcd1 esi = ebp*3; 0x0047bcd5 shl esi, 3    bytes = 72n (32-bit)
//   0x0047bce3  tmp = rwmalloc(bytes, 0)                                     *(0x007d3ff8)+0x108
//   0x0047bcf4  rep stosd / 0x0047bcfb rep stosb                             zero bytes of tmp
//   0x0047bd10..0x0047bd71  per triangle i, three records (stride 0x18):
//       {+0 tri, +4 0, +8 i, +0xc -1, +0x10 v0, +0x14 v1}, then (v1, v2), then (v2, v0)
//   0x0047bd73..0x0047bde0  for a = 0 .. 3n-2 (k = a+1): if tmp[a].+0 != 0 and k < 3n, for each b in k .. 3n-1 with
//       tmp[b].+0 != 0 and 0x0047bc90(tmp[a], tmp[b]) != 0: tmp[a].+4 = tmp[b].+0, tmp[a].+0xc = tmp[b].+8,
//       tmp[b].+0 = 0, tmp[b].+8 = -1 (0x0047bdaf..0x0047bdbe). The inner loop does not stop at a match.
//   0x0047bde2..0x0047be04  cnt = records with +0 != 0 and +4 != 0
//   0x0047be18  p = rwmalloc(cnt * 0x18, 0); out[0] = p, out[1] = cnt (no fill)
//   0x0047be2c..0x0047be54  copy those records (6 dwords, rep movsd) into p in order
//   0x0047be61  rwfree(tmp)                                                  *(0x007d3ff8)+0x10c
//
// 0x0047bc90 (C3, ESI = a, EDX = b -> EAX) is called through its existing C body Match47bc90(s, e) in
// Util/PromoLoop_sessionB.cpp (one body per RVA, rva-lint): 1 when a.v0 is b.v0 or b.v1 AND a.v1 is b.v0 or b.v1.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

extern "C" std::uint32_t __cdecl Match47bc90(std::uint8_t* s, std::uint8_t* e);   // 0x0047bc90

namespace {
using RwMallocFn = void*(__cdecl*)(std::uint32_t size, std::uint32_t hint);
using RwFreeFn = void(__cdecl*)(void* p);

inline std::uintptr_t RwGlobals() { return *reinterpret_cast<const std::uintptr_t*>(0x007d3ff8u); }

struct EdgeRec {                   // 0x18 bytes
    const std::uint8_t* tri;       // +0x00
    const std::uint8_t* mate;      // +0x04
    std::int32_t triIdx;           // +0x08
    std::int32_t mateIdx;          // +0x0c
    std::uint32_t v0;              // +0x10
    std::uint32_t v1;              // +0x14
};
static_assert(sizeof(EdgeRec) == 0x18, "edge record is 0x18 bytes");

void __cdecl TriEdgePairsBuildBody(std::int32_t n, std::uint32_t* out, const std::uint8_t* tris) {
    const std::uint32_t e3 = static_cast<std::uint32_t>(n) * 3u;                  // 0x0047bccd
    const std::uint32_t bytes = (e3 * 3u) << 3;                                     // 0x0047bcd1 / 0x0047bcd5
    EdgeRec* tmp = static_cast<EdgeRec*>(
        (*reinterpret_cast<RwMallocFn*>(RwGlobals() + 0x108u))(bytes, 0));          // 0x0047bce3
    std::memset(tmp, 0, bytes);                                                     // 0x0047bcf4 / 0x0047bcfb

    EdgeRec* e = tmp;
    for (std::int32_t i = 0; i < n; ++i) {                                          // 0x0047bd04 / 0x0047bd6f
        const std::uint8_t* t = tris + 8 * i;
        const std::uint32_t a = *reinterpret_cast<const std::uint16_t*>(t);
        const std::uint32_t b = *reinterpret_cast<const std::uint16_t*>(t + 2);
        const std::uint32_t c = *reinterpret_cast<const std::uint16_t*>(t + 4);
        e[0] = {t, nullptr, i, -1, a, b};
        e[1] = {t, nullptr, i, -1, b, c};
        e[2] = {t, nullptr, i, -1, c, a};
        e += 3;
    }

    const std::int32_t se3 = static_cast<std::int32_t>(e3);
    if (se3 - 1 > 0) {                                                              // 0x0047bd76
        EdgeRec* E = tmp;
        std::int32_t k = 1;
        for (std::int32_t rem = se3 - 1; rem != 0; --rem, ++E, ++k) {             // 0x0047bdd3..0x0047bde0
            if (E->tri == nullptr || k >= se3) continue;                           // 0x0047bd90 / 0x0047bd95
            EdgeRec* F = E + 1;
            for (std::int32_t c2 = se3 - k; c2 != 0; --c2, ++F) {                 // 0x0047bd9e / 0x0047bdc8
                if (F->tri == nullptr) continue;                                    // 0x0047bda2
                if (!Match47bc90(reinterpret_cast<std::uint8_t*>(E),
                                 reinterpret_cast<std::uint8_t*>(F))) continue;     // 0x0047bda6
                E->mate = F->tri;                                                   // 0x0047bdaf
                E->mateIdx = F->triIdx;                                             // 0x0047bdb5
                F->tri = nullptr;                                                   // 0x0047bdb8
                F->triIdx = -1;                                                     // 0x0047bdbe
            }
        }
    }

    std::uint32_t cnt = 0;
    for (std::int32_t j = 0; j < se3; ++j)                                          // 0x0047bde2..0x0047be04
        if (tmp[j].tri != nullptr && tmp[j].mate != nullptr) ++cnt;

    std::uint8_t* p = static_cast<std::uint8_t*>(
        (*reinterpret_cast<RwMallocFn*>(RwGlobals() + 0x108u))(cnt * 0x18u, 0));    // 0x0047be18
    out[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p));       // 0x0047be27
    out[1] = cnt;                                                                   // 0x0047be29
    std::uint32_t off = 0;
    for (std::int32_t j = 0; j < se3; ++j) {                                        // 0x0047be2c..0x0047be54
        if (tmp[j].tri == nullptr || tmp[j].mate == nullptr) continue;
        std::memcpy(p + off, &tmp[j], 0x18);                                        // 0x0047be4b rep movsd x6
        off += 0x18;
    }
    (*reinterpret_cast<RwFreeFn*>(RwGlobals() + 0x10cu))(tmp);                       // 0x0047be61
}
}  // namespace

// 0x0047bcc0  EAX = n, [esp+4] = out, [esp+8] = tris
extern "C" __declspec(dllexport) __declspec(naked) void TriEdgePairsBuild() {
    __asm {
        push dword ptr [esp + 8]      // tris
        push dword ptr [esp + 8]      // out (one push later, still at +8)
        push eax                      // n
        call TriEdgePairsBuildBody
        add esp, 12
        ret
    }
}
RH_ScopedInstall(TriEdgePairsBuild, 0x0047bcc0);
