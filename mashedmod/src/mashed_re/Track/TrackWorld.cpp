// Mashed RE — track world geometry parser implementation. See TrackWorld.h.
// C++ twin of re/tools/track_dump.py (validated 13/13 tracks 2026-06-10).

#include "TrackWorld.h"

#include <cstring>

namespace mashed_re {
namespace Track {

namespace {

struct Rd {
    const std::uint8_t* d;
    std::size_t         len;
    bool ok(std::size_t off, std::size_t n) const { return off + n <= len; }
    std::uint32_t u32(std::size_t off) const {
        std::uint32_t v; std::memcpy(&v, d + off, 4); return v;
    }
    float f32(std::size_t off) const {
        float v; std::memcpy(&v, d + off, 4); return v;
    }
};

struct Chunk { std::uint32_t id, size; std::size_t payload; };

bool read_chunk(const Rd& r, std::size_t off, Chunk* c) {
    if (!r.ok(off, 12)) return false;
    c->id = r.u32(off); c->size = r.u32(off + 4); c->payload = off + 12;
    return r.ok(c->payload, c->size);
}

}  // namespace

bool World::Parse(const std::uint8_t* data, std::size_t len) {
    materials.clear(); sectors.clear();
    total_tris = total_verts = 0;
    Rd r{data, len};
    Chunk world;
    if (!read_chunk(r, 0, &world) || world.id != 0x0B) {
        err_ = "root is not WORLD(0x0B)"; return false;
    }
    const std::size_t wend = world.payload + world.size;

    // ---- world header STRUCT (0x40 bytes) ----
    Chunk st;
    if (!read_chunk(r, world.payload, &st) || st.id != 0x01 || st.size != 0x40) {
        err_ = "world header STRUCT malformed"; return false;
    }
    const std::uint32_t hdr_tris   = r.u32(st.payload + 0x10);
    const std::uint32_t hdr_verts  = r.u32(st.payload + 0x14);
    const std::uint32_t hdr_planes = r.u32(st.payload + 0x18);
    const std::uint32_t hdr_worlds = r.u32(st.payload + 0x1C);
    for (int i = 0; i < 6; ++i) bbox[i] = r.f32(st.payload + 0x28 + i * 4);
    std::size_t p = st.payload + st.size;

    // ---- MATLIST ----
    Chunk ml;
    if (!read_chunk(r, p, &ml) || ml.id != 0x08) { err_ = "MATLIST missing"; return false; }
    {
        Chunk ms;
        if (!read_chunk(r, ml.payload, &ms) || ms.id != 0x01) { err_ = "matlist struct"; return false; }
        const std::int32_t n = static_cast<std::int32_t>(r.u32(ms.payload));
        std::size_t q = ms.payload + ms.size;
        for (std::int32_t i = 0; i < n; ++i) {
            Chunk mat;
            if (!read_chunk(r, q, &mat) || mat.id != 0x07) { err_ = "material chunk"; return false; }
            Material out{}; out.tex_name[0] = '\0';
            std::size_t qq = mat.payload;
            const std::size_t qe = mat.payload + mat.size;
            while (qq + 12 <= qe) {
                Chunk ch;
                if (!read_chunk(r, qq, &ch)) break;
                if (ch.id == 0x01 && ch.size >= 8) {
                    std::memcpy(out.rgba, data + ch.payload + 4, 4);
                } else if (ch.id == 0x06) {
                    Chunk fs, ns;
                    if (read_chunk(r, ch.payload, &fs) &&
                        read_chunk(r, fs.payload + fs.size, &ns)) {
                        std::size_t cn = 0;
                        while (cn < ns.size && cn < 32 &&
                               data[ns.payload + cn] != 0) {
                            out.tex_name[cn] = static_cast<char>(data[ns.payload + cn]);
                            ++cn;
                        }
                        out.tex_name[cn] = '\0';
                    }
                }
                qq = ch.payload + ch.size;
            }
            materials.push_back(out);
            q = mat.payload + mat.size;
        }
    }
    p = ml.payload + ml.size;

    // ---- sector tree (iterative stack; PLANESECTOR 0x0A inner, 0x09 leaf) ----
    std::uint32_t plane_count = 0;
    // explicit recursion via a small lambda-less stack of offsets
    struct Frame { std::size_t off; };
    std::vector<std::size_t> stack;
    stack.push_back(p);
    while (!stack.empty()) {
        const std::size_t off = stack.back(); stack.pop_back();
        Chunk c;
        if (!read_chunk(r, off, &c)) { err_ = "sector chunk truncated"; return false; }
        if (c.id == 0x0A) {
            ++plane_count;
            Chunk ps;
            if (!read_chunk(r, c.payload, &ps) || ps.id != 0x01 || ps.size != 0x18) {
                err_ = "plane sector struct"; return false;
            }
            const std::size_t left = ps.payload + ps.size;
            Chunk lc;
            if (!read_chunk(r, left, &lc)) { err_ = "left child truncated"; return false; }
            const std::size_t right = lc.payload + lc.size;
            stack.push_back(right);
            stack.push_back(left);
            continue;
        }
        if (c.id != 0x09) { err_ = "unexpected sector chunk id"; return false; }
        Chunk as;
        if (!read_chunk(r, c.payload, &as) || as.id != 0x01) { err_ = "atomic struct"; return false; }
        const std::uint32_t base = r.u32(as.payload + 0x00);
        const std::uint32_t nt   = r.u32(as.payload + 0x04);
        const std::uint32_t nv   = r.u32(as.payload + 0x08);
        constexpr std::size_t kFixed = 0x2C;
        if (as.size < kFixed) { err_ = "atomic struct too small"; return false; }
        const std::size_t payload = as.size - kFixed;
        // solve arrays: payload = nv*12 + [nv*12 normals] + [nv*4 prelit]
        //               + uv*nv*8 + nt*8
        const std::size_t base_need = static_cast<std::size_t>(nv) * 12 +
                                      static_cast<std::size_t>(nt) * 8;
        if (payload < base_need) { err_ = "sector payload too small"; return false; }
        const std::size_t rem = payload - base_need;
        bool have_n = false, have_p = false; int nuv = -1;
        for (int normals = 0; normals < 2 && nuv < 0; ++normals)
            for (int prelit = 0; prelit < 2 && nuv < 0; ++prelit)
                for (int uv = 0; uv < 3; ++uv)
                    if (rem == static_cast<std::size_t>(normals) * nv * 12 +
                               static_cast<std::size_t>(prelit)  * nv * 4 +
                               static_cast<std::size_t>(uv)      * nv * 8) {
                        have_n = normals != 0; have_p = prelit != 0; nuv = uv;
                        break;
                    }
        if (nuv < 0) { err_ = "sector array layout unsolved"; return false; }

        Sector s;
        std::size_t q = as.payload + kFixed;
        s.verts.resize(static_cast<std::size_t>(nv) * 3);
        std::memcpy(s.verts.data(), data + q, static_cast<std::size_t>(nv) * 12);
        q += static_cast<std::size_t>(nv) * 12;
        if (have_n) q += static_cast<std::size_t>(nv) * 12;
        if (have_p) {
            s.prelit.resize(nv);
            std::memcpy(s.prelit.data(), data + q, static_cast<std::size_t>(nv) * 4);
            q += static_cast<std::size_t>(nv) * 4;
        }
        if (nuv > 0) {
            s.uvs.resize(static_cast<std::size_t>(nv) * 2);
            std::memcpy(s.uvs.data(), data + q, static_cast<std::size_t>(nv) * 8);
            q += static_cast<std::size_t>(nuv) * nv * 8;
        }
        s.tris.resize(static_cast<std::size_t>(nt) * 4);
        // on-disk order = (v0,v1,v2,mat); store as (mat,v0,v1,v2)
        for (std::uint32_t i = 0; i < nt; ++i) {
            std::uint16_t t4[4];
            std::memcpy(t4, data + q + static_cast<std::size_t>(i) * 8, 8);
            const std::uint16_t mat = static_cast<std::uint16_t>(t4[3] + base);
            if (t4[0] >= nv || t4[1] >= nv || t4[2] >= nv ||
                mat >= materials.size()) {
                err_ = "triangle index out of range"; return false;
            }
            s.tris[i * 4 + 0] = mat;
            s.tris[i * 4 + 1] = t4[0];
            s.tris[i * 4 + 2] = t4[1];
            s.tris[i * 4 + 3] = t4[2];
        }
        total_tris  += nt;
        total_verts += nv;
        sectors.push_back(std::move(s));
        (void)wend;
    }

    if (total_tris != hdr_tris || total_verts != hdr_verts ||
        sectors.size() != hdr_worlds || plane_count != hdr_planes) {
        err_ = "sector sums do not match world header"; return false;
    }
    err_ = "";
    return true;
}

}  // namespace Track
}  // namespace mashed_re
