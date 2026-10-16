// Mashed RE — RenderWare material-effects (MatFX) material extension and geometry
// user-data reader, shared by TrackWorld (BSP) and DffModel (DFF). [rtex 2026-10-16]
//
// MatFX: rwID_MATERIALEFFECTSPLUGIN chunk 0x120 inside a material EXTENSION (0x03).
// Layout (librw src/matfx.cpp readMaterialMatFX, the same stream RW 3.x writes):
//   u32 type                          rpMatFXMaterialFlags (0 none, 1 bump, 2 env,
//                                     3 bump+env, 4 dual, 5 uvxform, 6 dual+uvxform)
//   2 x { u32 effect, body }          body by effect:
//     1 BUMP : f32 bumpiness, i32 hasBumpTex [TEXTURE], i32 hasTex [TEXTURE]
//     2 ENV  : f32 coef, i32 fbAlpha, i32 hasTex [TEXTURE]
//     4 DUAL : i32 srcBlend, i32 dstBlend, i32 hasTex [TEXTURE]
//     5 UVX  : (nothing)
// Shipped assets (verify/rtex_20261016/evidence/matfx_census.txt, every DFF/BSP): DUAL with
// srcBlend 5 / dstBlend 6 (SRCALPHA / INVSRCALPHA) on 1518 materials -- TRAINING ROAD.DFF +
// GRAPH.BSP material 0, the vehicle DFFs (CarDirt / WheelDirt) -- and 1 / 2 on 48 (Atmos);
// plus UVTRANSFORM (57), ENVMAP (7, dump REFLECTIONS.DFF), BUMPENVMAP (1, SFX CUP.DFF) and
// DUALUVTRANSFORM (1, Storm WATER.DFF). The original renders DUAL in FUN_005405c0.
//
// User data: rpUSERDATA chunk 0x11F inside a geometry EXTENSION. i32 numArrays, then
// per array: i32 nameLen + name, i32 format (1 int, 2 real, 3 string), i32 count,
// values. Vehicle geometries carry "0.tv_part_id" (int) as array 0 -- the part code
// FUN_004b5190 reads (array 0, element 0 of atomic+0x18 = the GEOMETRY).
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace Track {

struct MatFx {
    std::uint32_t type = 0;         // rpMatFXMaterialFlags; 0 = no MatFX extension
    bool          has_dual = false; // a DUAL (effect 4) slot was present
    std::int32_t  dual_src = 0;     // RwBlendFunction
    std::int32_t  dual_dst = 0;
    char          dual_tex[33] = {};
};

namespace matfx_detail {
inline bool rd32(const std::uint8_t* d, std::size_t len, std::size_t off, std::uint32_t* v) {
    if (off + 4 > len) return false;
    std::memcpy(v, d + off, 4);
    return true;
}
// TEXTURE chunk (0x06) at `off`: STRUCT then STRING name. Returns the chunk end.
inline std::size_t read_tex(const std::uint8_t* d, std::size_t len, std::size_t off, char* name) {
    name[0] = '\0';
    std::uint32_t id = 0, sz = 0;
    if (!rd32(d, len, off, &id) || !rd32(d, len, off + 4, &sz) || id != 0x06) return 0;
    const std::size_t end = off + 12 + sz;
    if (end > len) return 0;
    std::size_t q = off + 12;
    std::uint32_t sid = 0, ssz = 0;
    if (!rd32(d, len, q, &sid) || !rd32(d, len, q + 4, &ssz)) return end;
    q += 12 + ssz;                                     // skip STRUCT (filter/addressing)
    std::uint32_t nid = 0, nsz = 0;
    if (!rd32(d, len, q, &nid) || !rd32(d, len, q + 4, &nsz) || nid != 0x02) return end;
    std::size_t n = 0;
    while (n < 32 && n < nsz && q + 12 + n < len && d[q + 12 + n]) {
        name[n] = static_cast<char>(d[q + 12 + n]);
        ++n;
    }
    name[n] = '\0';
    return end;
}
}  // namespace matfx_detail

// Scan a material EXTENSION payload for chunk 0x120 and decode it. Leaves `out`
// default (type 0) when absent or malformed.
inline void ParseMatFxExt(const std::uint8_t* d, std::size_t len,
                          std::size_t ext_payload, std::size_t ext_size, MatFx* out) {
    using namespace matfx_detail;
    *out = MatFx{};
    std::size_t o = ext_payload;
    const std::size_t end = ext_payload + ext_size;
    while (o + 12 <= end) {
        std::uint32_t id = 0, sz = 0;
        if (!rd32(d, len, o, &id) || !rd32(d, len, o + 4, &sz)) return;
        if (id == 0x120) {
            std::size_t q = o + 12;
            const std::size_t ce = q + sz;
            std::uint32_t type = 0;
            if (!rd32(d, len, q, &type)) return;
            q += 4;
            MatFx fx;
            fx.type = type;
            for (int slot = 0; slot < 2 && q + 4 <= ce; ++slot) {
                std::uint32_t eff = 0;
                if (!rd32(d, len, q, &eff)) break;
                q += 4;
                std::uint32_t a = 0, b = 0, has = 0;
                char scratch[33];
                if (eff == 1) {                               // BUMP
                    q += 4;
                    if (!rd32(d, len, q, &has)) break;
                    q += 4;
                    if (has) { const std::size_t e2 = read_tex(d, len, q, scratch); if (!e2) break; q = e2; }
                    if (!rd32(d, len, q, &has)) break;
                    q += 4;
                    if (has) { const std::size_t e2 = read_tex(d, len, q, scratch); if (!e2) break; q = e2; }
                } else if (eff == 2) {                        // ENV
                    q += 8;
                    if (!rd32(d, len, q, &has)) break;
                    q += 4;
                    if (has) { const std::size_t e2 = read_tex(d, len, q, scratch); if (!e2) break; q = e2; }
                } else if (eff == 4) {                        // DUAL
                    if (!rd32(d, len, q, &a) || !rd32(d, len, q + 4, &b) ||
                        !rd32(d, len, q + 8, &has)) break;
                    q += 12;
                    fx.has_dual = true;
                    fx.dual_src = static_cast<std::int32_t>(a);
                    fx.dual_dst = static_cast<std::int32_t>(b);
                    if (has) {
                        const std::size_t e2 = read_tex(d, len, q, fx.dual_tex);
                        if (!e2) break;
                        q = e2;
                    }
                }
                // 0 (none) and 5 (uv transform) carry no body
            }
            *out = fx;
            return;
        }
        o += 12 + sz;
    }
}

// Scan a geometry EXTENSION payload for chunk 0x11F; return array 0 element 0 when
// it is an int array (format 1), else -1.
inline std::int32_t ParseUserDataPartId(const std::uint8_t* d, std::size_t len,
                                        std::size_t ext_payload, std::size_t ext_size) {
    using namespace matfx_detail;
    std::size_t o = ext_payload;
    const std::size_t end = ext_payload + ext_size;
    while (o + 12 <= end) {
        std::uint32_t id = 0, sz = 0;
        if (!rd32(d, len, o, &id) || !rd32(d, len, o + 4, &sz)) return -1;
        if (id == 0x11F) {
            std::size_t q = o + 12;
            std::uint32_t narr = 0, nlen = 0, fmt = 0, cnt = 0, v = 0;
            if (!rd32(d, len, q, &narr) || narr == 0) return -1;
            q += 4;
            if (!rd32(d, len, q, &nlen)) return -1;
            q += 4 + nlen;
            if (!rd32(d, len, q, &fmt) || !rd32(d, len, q + 4, &cnt)) return -1;
            q += 8;
            if (fmt != 1 || cnt == 0 || !rd32(d, len, q, &v)) return -1;
            return static_cast<std::int32_t>(v);
        }
        o += 12 + sz;
    }
    return -1;
}

}  // namespace Track
}  // namespace mashed_re
