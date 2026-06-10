// Mashed RE — DFF clump parser implementation. See DffModel.h and
// re/analysis/formats/vehicle_dff.md (librw-cross-referenced layout).

#include "DffModel.h"

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
    std::int32_t i32(std::size_t off) const {
        std::int32_t v; std::memcpy(&v, d + off, 4); return v;
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

bool find_child(const Rd& r, std::size_t off, std::size_t end,
                std::uint32_t want, Chunk* out) {
    while (off + 12 <= end) {
        Chunk c;
        if (!read_chunk(r, off, &c)) return false;
        if (c.id == want) { *out = c; return true; }
        off = c.payload + c.size;
    }
    return false;
}

struct Frame {
    float rot[9];   // column-major 3x3
    float pos[3];
    std::int32_t parent;
};

// model-space transform of a point through the frame parent chain
void FrameXform(const std::vector<Frame>& frames, int idx, const float in[3],
                float out[3]) {
    float v[3] = {in[0], in[1], in[2]};
    int i = idx;
    while (i >= 0) {
        const Frame& f = frames[static_cast<std::size_t>(i)];
        const float x = f.rot[0]*v[0] + f.rot[3]*v[1] + f.rot[6]*v[2] + f.pos[0];
        const float y = f.rot[1]*v[0] + f.rot[4]*v[1] + f.rot[7]*v[2] + f.pos[1];
        const float z = f.rot[2]*v[0] + f.rot[5]*v[1] + f.rot[8]*v[2] + f.pos[2];
        v[0] = x; v[1] = y; v[2] = z;
        i = f.parent;
    }
    out[0] = v[0]; out[1] = v[1]; out[2] = v[2];
}

}  // namespace

bool DffModel::Parse(const std::uint8_t* data, std::size_t len) {
    materials.clear(); batches.clear();
    total_tris = total_verts = 0;
    Rd r{data, len};
    Chunk clump;
    if (!read_chunk(r, 0, &clump) || clump.id != 0x10) {
        err_ = "root is not CLUMP(0x10)"; return false;
    }
    const std::size_t cend = clump.payload + clump.size;
    Chunk st;
    if (!read_chunk(r, clump.payload, &st) || st.id != 0x01) {
        err_ = "clump struct"; return false;
    }
    const std::int32_t num_atomics = r.i32(st.payload);

    // FRAMELIST
    Chunk fl;
    if (!find_child(r, st.payload + st.size, cend, 0x0E, &fl)) {
        err_ = "FRAMELIST missing"; return false;
    }
    Chunk fst;
    if (!read_chunk(r, fl.payload, &fst) || fst.id != 0x01) {
        err_ = "framelist struct"; return false;
    }
    const std::int32_t nframes = r.i32(fst.payload);
    if (nframes <= 0 ||
        4 + static_cast<std::size_t>(nframes) * 0x38 > fst.size) {
        err_ = "framelist size"; return false;
    }
    std::vector<Frame> frames(static_cast<std::size_t>(nframes));
    {
        std::size_t q = fst.payload + 4;
        for (std::int32_t i = 0; i < nframes; ++i) {
            Frame& f = frames[static_cast<std::size_t>(i)];
            for (int k = 0; k < 9; ++k) f.rot[k] = r.f32(q + k * 4u);
            for (int k = 0; k < 3; ++k) f.pos[k] = r.f32(q + 36 + k * 4u);
            f.parent = r.i32(q + 48);
            if (f.parent >= i) { err_ = "frame parent out of order"; return false; }
            q += 0x38;
        }
    }

    // GEOMETRYLIST
    Chunk gl;
    if (!find_child(r, fl.payload + fl.size, cend, 0x1A, &gl)) {
        err_ = "GEOMETRYLIST missing"; return false;
    }
    Chunk gst;
    if (!read_chunk(r, gl.payload, &gst) || gst.id != 0x01) {
        err_ = "geolist struct"; return false;
    }
    const std::int32_t ngeo = r.i32(gst.payload);

    struct Geo {
        std::vector<float> verts;            // model-space AFTER atomic bake
        std::vector<float> uvs;
        std::vector<std::uint32_t> prelit;
        std::vector<std::uint16_t> tris;     // mat,v0,v1,v2
        std::uint32_t mat_base = 0;          // into this->materials
        std::uint32_t nmats = 0;
    };
    std::vector<Geo> geos;
    std::size_t q = gst.payload + gst.size;
    for (std::int32_t gi = 0; gi < ngeo; ++gi) {
        Chunk g;
        if (!read_chunk(r, q, &g) || g.id != 0x0F) { err_ = "geometry chunk"; return false; }
        Chunk gs;
        if (!read_chunk(r, g.payload, &gs) || gs.id != 0x01) { err_ = "geometry struct"; return false; }
        const std::uint32_t flags  = r.u32(gs.payload);
        const std::int32_t  ntris  = r.i32(gs.payload + 4);
        const std::int32_t  nverts = r.i32(gs.payload + 8);
        const std::int32_t  nmorph = r.i32(gs.payload + 12);
        std::size_t p = gs.payload + 16;
        Geo geo;
        if (!(flags & 0x01000000u)) {  // !NATIVE
            if (flags & 0x08u) {       // PRELIT
                geo.prelit.resize(static_cast<std::size_t>(nverts));
                std::memcpy(geo.prelit.data(), data + p,
                            static_cast<std::size_t>(nverts) * 4);
                p += static_cast<std::size_t>(nverts) * 4;
            }
            std::uint32_t nuv = (flags >> 16) & 0xFFu;
            if (nuv == 0) nuv = (flags & 0x80u) ? 2 : ((flags & 0x04u) ? 1 : 0);
            if (nuv) {
                geo.uvs.resize(static_cast<std::size_t>(nverts) * 2);
                std::memcpy(geo.uvs.data(), data + p,
                            static_cast<std::size_t>(nverts) * 8);
                p += static_cast<std::size_t>(nuv) * nverts * 8;
            }
            geo.tris.resize(static_cast<std::size_t>(ntris) * 4);
            for (std::int32_t i = 0; i < ntris; ++i) {
                const std::uint32_t w0 = r.u32(p), w1 = r.u32(p + 4); p += 8;
                const std::uint16_t v0 = static_cast<std::uint16_t>(w0 >> 16);
                const std::uint16_t v1 = static_cast<std::uint16_t>(w0);
                const std::uint16_t v2 = static_cast<std::uint16_t>(w1 >> 16);
                const std::uint16_t m  = static_cast<std::uint16_t>(w1);
                if (v0 >= nverts || v1 >= nverts || v2 >= nverts) {
                    err_ = "geometry tri index"; return false;
                }
                geo.tris[static_cast<std::size_t>(i) * 4 + 0] = m;
                geo.tris[static_cast<std::size_t>(i) * 4 + 1] = v0;
                geo.tris[static_cast<std::size_t>(i) * 4 + 2] = v1;
                geo.tris[static_cast<std::size_t>(i) * 4 + 3] = v2;
            }
            for (std::int32_t mt = 0; mt < nmorph; ++mt) {
                p += 16;  // bounding sphere
                const std::int32_t hv = r.i32(p), hn = r.i32(p + 4); p += 8;
                if (hv) {
                    if (mt == 0) {
                        geo.verts.resize(static_cast<std::size_t>(nverts) * 3);
                        std::memcpy(geo.verts.data(), data + p,
                                    static_cast<std::size_t>(nverts) * 12);
                    }
                    p += static_cast<std::size_t>(nverts) * 12;
                }
                if (hn) p += static_cast<std::size_t>(nverts) * 12;
            }
        }
        if (geo.verts.size() != static_cast<std::size_t>(nverts) * 3 ||
            geo.tris.size() != static_cast<std::size_t>(ntris) * 4) {
            err_ = "geometry counts"; return false;
        }
        // material list for this geometry
        Chunk ml;
        geo.mat_base = static_cast<std::uint32_t>(materials.size());
        if (find_child(r, gs.payload + gs.size, g.payload + g.size, 0x08, &ml)) {
            Chunk ms;
            if (read_chunk(r, ml.payload, &ms) && ms.id == 0x01) {
                const std::int32_t nm = r.i32(ms.payload);
                std::size_t mq = ms.payload + ms.size;
                for (std::int32_t i = 0; i < nm; ++i) {
                    Chunk mat;
                    if (!read_chunk(r, mq, &mat) || mat.id != 0x07) break;
                    DffMaterial out{};
                    std::size_t qq = mat.payload;
                    const std::size_t qe = mat.payload + mat.size;
                    while (qq + 12 <= qe) {
                        Chunk ch;
                        if (!read_chunk(r, qq, &ch)) break;
                        if (ch.id == 0x01 && ch.size >= 8) {
                            std::memcpy(out.rgba, data + ch.payload + 4, 4);
                        } else if (ch.id == 0x06) {
                            Chunk fs2, ns2;
                            if (read_chunk(r, ch.payload, &fs2) &&
                                read_chunk(r, fs2.payload + fs2.size, &ns2)) {
                                std::size_t cn = 0;
                                while (cn < ns2.size && cn < 32 &&
                                       data[ns2.payload + cn] != 0) {
                                    out.tex_name[cn] =
                                        static_cast<char>(data[ns2.payload + cn]);
                                    ++cn;
                                }
                                out.tex_name[cn] = '\0';
                            }
                        }
                        qq = ch.payload + ch.size;
                    }
                    materials.push_back(out);
                    ++geo.nmats;
                    mq = mat.payload + mat.size;
                }
            }
        }
        geos.push_back(std::move(geo));
        q = g.payload + g.size;
    }

    // ATOMICs: bake frame transforms, emit per-(atomic,material) batches.
    bbox[0] = bbox[1] = bbox[2] =  1e9f;
    bbox[3] = bbox[4] = bbox[5] = -1e9f;
    std::int32_t found_atomics = 0;
    std::size_t a = gl.payload + gl.size;
    while (a + 12 <= cend) {
        Chunk c;
        if (!read_chunk(r, a, &c)) break;
        if (c.id == 0x14) {
            Chunk as;
            if (!read_chunk(r, c.payload, &as) || as.id != 0x01) {
                err_ = "atomic struct"; return false;
            }
            const std::int32_t fi = r.i32(as.payload);
            const std::int32_t gi = r.i32(as.payload + 4);
            if (fi < 0 || fi >= nframes || gi < 0 ||
                gi >= static_cast<std::int32_t>(geos.size())) {
                err_ = "atomic indices"; return false;
            }
            ++found_atomics;
            const Geo& geo = geos[static_cast<std::size_t>(gi)];
            const std::size_t nverts = geo.verts.size() / 3;
            // transformed vertex positions for this atomic
            std::vector<float> w(nverts * 3);
            for (std::size_t v = 0; v < nverts; ++v) {
                float out[3];
                FrameXform(frames, fi, &geo.verts[v * 3], out);
                w[v * 3 + 0] = out[0]; w[v * 3 + 1] = out[1]; w[v * 3 + 2] = out[2];
                for (int k = 0; k < 3; ++k) {
                    if (out[k] < bbox[k])     bbox[k] = out[k];
                    if (out[k] > bbox[3 + k]) bbox[3 + k] = out[k];
                }
            }
            total_verts += static_cast<std::uint32_t>(nverts);
            total_tris  += static_cast<std::uint32_t>(geo.tris.size() / 4);
            // split into per-material batches
            for (std::uint32_t mi = 0; mi < geo.nmats || mi == 0; ++mi) {
                DffBatch b;
                b.material = geo.mat_base + (geo.nmats ? mi : 0);
                for (std::size_t i = 0; i < geo.tris.size() / 4; ++i) {
                    if (geo.nmats && geo.tris[i * 4] != mi) continue;
                    for (int k = 1; k <= 3; ++k) {
                        const std::uint16_t vi = geo.tris[i * 4 + k];
                        b.tris.push_back(static_cast<std::uint16_t>(
                            b.verts.size() / 3));
                        b.verts.push_back(w[vi * 3 + 0]);
                        b.verts.push_back(w[vi * 3 + 1]);
                        b.verts.push_back(w[vi * 3 + 2]);
                        b.uvs.push_back(geo.uvs.empty() ? 0.f : geo.uvs[vi * 2 + 0]);
                        b.uvs.push_back(geo.uvs.empty() ? 0.f : geo.uvs[vi * 2 + 1]);
                        if (!geo.prelit.empty())
                            b.prelit.push_back(geo.prelit[vi]);
                    }
                }
                if (!b.tris.empty()) batches.push_back(std::move(b));
                if (geo.nmats == 0) break;
            }
        }
        a = c.payload + c.size;
    }
    if (found_atomics != num_atomics) { err_ = "atomic count mismatch"; return false; }
    if (batches.empty()) { err_ = "no renderable batches"; return false; }
    err_ = "";
    return true;
}

}  // namespace Track
}  // namespace mashed_re
