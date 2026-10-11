// Mashed RE: stand-in promotion round 5 (plan item #3 step 2, 2026-10-11): two RW geometry builders.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x00401fe0  Geom3Vert1TriCreate  (FUN_00401fe0, 600 B, cdecl() -> geometry or NULL; caller 0x00402240)
// Frame F = esp after `sub esp, 0x4c; push esi; push edi`. Locals: F+0x8 four bytes 0xff (0x00401fe7..0x00401ff3);
// F+0x30..F+0x50 nine floats (0x00401ff7..0x00402037) = (1,0,-1) (0,0,2) (-1,0,-1).
//   0x0040203f  mat = 0x004e7fa0();  0x00402054 [mat+4] = 0xffffffff (the F+0x8 bytes)
//   0x00402057  geom = 0x004e8c70(3, 1, 0x4b);  NULL -> return NULL (0x00402063; mat is not released on that path)
//   0x0040206e..0x0040209c  [mat+0xc] = 0x3e99999a (0.3f), [mat+0x10] = 0, [mat+0x14] = 0x3f333333 (0.7f)
//   0x0040209f  tris = [geom+0x2c], prelit = [geom+0x30], uv = [geom+0x34], morph = [geom+0x5c],
//               verts = [morph+0x14], normals = [morph+0x18]
//   per vertex k = 0, 1, 2 (unrolled 0x004020b8..0x004021d1): verts[k] = position k; then, each only when its array
//               pointer is non-NULL (and that pointer then advances): normal = (0, 1.0f, 0) (F+0x14..F+0x1c),
//               prelit = bytes ff ff ff ff, uv = (1.0f, 1.0f)
//   0x004021e4  0x004e8910(geom, tris, 0, 1, 2)
//   0x004021f0  0x004e8940(geom, tris, mat)
//   0x004021fb  0x004e8640(morph, &sphere)   sphere = 16-byte local at F+0x20
//   0x00402204  [morph+4 .. morph+0x10] = sphere (0x00402207..0x0040221d)
//   0x00402220  0x004e8a10(geom)
//   0x00402226  0x004e8030(mat)
//   0x0040222b  add esp, 0x30 (12 pushes, all caller-cleaned); return geom (0x00402231)

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using Fn0 = void*(__cdecl*)();
using Fn1 = void*(__cdecl*)(void*);
using Fn2 = void*(__cdecl*)(void*, void*);
using Fn3 = void*(__cdecl*)(void*, void*, void*);
using GeomCreateFn = void*(__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t);
using TriSetFn = void*(__cdecl*)(void*, void*, std::uint32_t, std::uint32_t, std::uint32_t);

template <typename T> T& At(void* base, std::uint32_t off) {
    return *reinterpret_cast<T*>(static_cast<std::uint8_t*>(base) + off);
}
}  // namespace

namespace {
// Shared by 0x00401fe0 and 0x004022a0: the two bodies run the same sequence with different vertex / triangle counts,
// positions and triangle index lists (cited per row below).
void* GeomBuild(const std::uint32_t (*pos)[3], std::uint32_t nVerts, const std::uint32_t (*tri)[3], std::uint32_t nTris) {
    void* mat = reinterpret_cast<Fn0>(0x004e7fa0u)();
    At<std::uint32_t>(mat, 4) = 0xffffffffu;
    void* geom = reinterpret_cast<GeomCreateFn>(0x004e8c70u)(nVerts, nTris, 0x4b);
    if (!geom) return nullptr;
    At<std::uint32_t>(mat, 0xc) = 0x3e99999au;
    At<std::uint32_t>(mat, 0x10) = 0;
    At<std::uint32_t>(mat, 0x14) = 0x3f333333u;
    std::uint8_t* tris = At<std::uint8_t*>(geom, 0x2c);
    std::uint8_t* prelit = At<std::uint8_t*>(geom, 0x30);
    std::uint32_t* uv = At<std::uint32_t*>(geom, 0x34);
    void* morph = At<void*>(geom, 0x5c);
    std::uint32_t* verts = At<std::uint32_t*>(morph, 0x14);
    std::uint32_t* normals = At<std::uint32_t*>(morph, 0x18);
    for (std::uint32_t k = 0; k < nVerts; ++k) {
        verts[3 * k + 0] = pos[k][0];
        verts[3 * k + 1] = pos[k][1];
        verts[3 * k + 2] = pos[k][2];
        if (normals) { normals[0] = 0; normals[1] = 0x3f800000u; normals[2] = 0; normals += 3; }
        if (prelit) { prelit[0] = 0xff; prelit[1] = 0xff; prelit[2] = 0xff; prelit[3] = 0xff; prelit += 4; }
        if (uv) { uv[0] = 0x3f800000u; uv[1] = 0x3f800000u; uv += 2; }
    }
    for (std::uint32_t t = 0; t < nTris; ++t) {                    // triangle records are 8 bytes (0x0040252d)
        reinterpret_cast<TriSetFn>(0x004e8910u)(geom, tris + 8 * t, tri[t][0], tri[t][1], tri[t][2]);
        reinterpret_cast<Fn3>(0x004e8940u)(geom, tris + 8 * t, mat);
    }
    std::uint32_t sphere[4];
    reinterpret_cast<Fn2>(0x004e8640u)(morph, sphere);
    for (int i = 0; i < 4; ++i) At<std::uint32_t>(morph, 4 + 4 * i) = sphere[i];
    reinterpret_cast<Fn1>(0x004e8a10u)(geom);
    reinterpret_cast<Fn1>(0x004e8030u)(mat);
    return geom;
}
}  // namespace

// 0x00401fe0  (sequence addresses in the header above)
extern "C" __declspec(dllexport) void* __cdecl Geom3Vert1TriCreate() {
    static const std::uint32_t kPos[3][3] = {                     // F+0x30..F+0x50 (0x00401ff7..0x00402037)
        {0x3f800000u, 0x00000000u, 0xbf800000u},
        {0x00000000u, 0x00000000u, 0x40000000u},
        {0xbf800000u, 0x00000000u, 0xbf800000u},
    };
    static const std::uint32_t kTri[1][3] = {{0, 1, 2}};          // 0x004021dc..0x004021e0
    return GeomBuild(kPos, 3, kTri, 1);
}
RH_ScopedInstall(Geom3Vert1TriCreate, 0x00401fe0);

// 0x004022a0  Geom4Vert2TriCreate  (FUN_004022a0, 744 B, cdecl() -> geometry or NULL; caller 0x00402590)
// Same sequence as 0x00401fe0 with the frame grown to `sub esp, 0x58`:
//   F+0x30..F+0x5c twelve floats (0x004022b7..0x0040230f) = (-1,0,-4) (-1,0,4) (1,0,4) (1,0,-4)
//   0x00402317  mat = 0x004e7fa0(); 0x0040232c [mat+4] = 0xffffffff
//   0x0040232f  geom = 0x004e8c70(4, 2, 0x4b); NULL -> return NULL (0x0040233b)
//   0x00402346..0x0040236f  material floats 0x3e99999a / 0 / 0x3f333333 at +0xc / +0x10 / +0x14
//   0x00402390..0x00402507  four vertices, each with the optional normal (0,1,0) / prelit ff ff ff ff / uv (1,1)
//   0x00402516  0x004e8910(geom, tris, 0, 1, 3);  0x00402522 0x004e8940(geom, tris, mat)
//   0x00402532  0x004e8910(geom, tris+8, 3, 1, 2); 0x0040253a 0x004e8940(geom, tris+8, mat)
//   0x0040254c  0x004e8640(morph, &sphere); 0x00402558..0x0040256d [morph+4..+0x10] = sphere
//   0x00402570  0x004e8a10(geom); 0x00402576 0x004e8030(mat); 0x00402581 return geom
// 0x004022a0
extern "C" __declspec(dllexport) void* __cdecl Geom4Vert2TriCreate() {
    static const std::uint32_t kPos[4][3] = {
        {0xbf800000u, 0x00000000u, 0xc0800000u},
        {0xbf800000u, 0x00000000u, 0x40800000u},
        {0x3f800000u, 0x00000000u, 0x40800000u},
        {0x3f800000u, 0x00000000u, 0xc0800000u},
    };
    static const std::uint32_t kTri[2][3] = {{0, 1, 3}, {3, 1, 2}};
    return GeomBuild(kPos, 4, kTri, 2);
}
RH_ScopedInstall(Geom4Vert2TriCreate, 0x004022a0);
