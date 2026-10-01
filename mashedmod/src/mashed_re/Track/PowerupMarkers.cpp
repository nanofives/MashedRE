// PowerupMarkers impl — see PowerupMarkers.h for the RVA chain this reproduces.
//
// Deliberately self-contained rather than an extension of Track::DffModel:
// DffModel bakes frame transforms into vertices, discards the frame and
// geometry indices, and walks EXTENSION only at material level. Teaching it
// user data would put a new code path under every vehicle, prop and world DFF
// the port loads, which the pickup fix's "no collateral pixels" rule explicitly
// forbids. This file is reached only by the POWERUPS_GOLD.DFF load.
#include "PowerupMarkers.h"

#include <cstring>
#include <string.h>     // _stricmp

namespace mashed_re {
namespace Track {
namespace {

// RW stream chunk ids (RenderWare 3.x)
const std::uint32_t kStruct       = 0x01;
const std::uint32_t kExtension    = 0x03;
const std::uint32_t kClump        = 0x10;
const std::uint32_t kFrameList    = 0x0E;
const std::uint32_t kGeometryList = 0x1A;
const std::uint32_t kGeometry     = 0x0F;
const std::uint32_t kAtomic       = 0x14;
const std::uint32_t kUserData     = 0x011F;

struct Rd {
    const std::uint8_t* d;
    std::size_t len;
    bool u32(std::size_t off, std::uint32_t* v) const {
        if (off + 4 > len) return false;
        std::memcpy(v, d + off, 4);
        return true;
    }
    bool i32(std::size_t off, std::int32_t* v) const {
        return u32(off, reinterpret_cast<std::uint32_t*>(v));
    }
    bool f32(std::size_t off, float* v) const {
        if (off + 4 > len) return false;
        std::memcpy(v, d + off, 4);
        return true;
    }
};

struct Chunk { std::uint32_t id, size; std::size_t payload; };

bool ReadChunk(const Rd& r, std::size_t off, Chunk* c) {
    std::uint32_t id = 0, sz = 0;
    if (!r.u32(off, &id) || !r.u32(off + 4, &sz)) return false;   // [+8] = version
    if (off + 12 > r.len || sz > r.len - (off + 12)) return false;
    c->id = id; c->size = sz; c->payload = off + 12;
    return true;
}

bool FindChild(const Rd& r, std::size_t off, std::size_t end,
               std::uint32_t want, Chunk* out) {
    while (off + 12 <= end) {
        Chunk c;
        if (!ReadChunk(r, off, &c)) return false;
        if (c.payload + c.size > end) return false;
        if (c.id == want) { *out = c; return true; }
        off = c.payload + c.size;
    }
    return false;
}

struct Frame { float rot[9]; float pos[3]; std::int32_t parent; };

// RW USERDATA (0x011f) payload, per the RenderWare user-data plugin stream:
//   i32 numUserDatas
//   per entry: i32 nameLen, char name[nameLen], i32 dataType, i32 numElements,
//              then numElements * (i32 | f32 | length-prefixed string)
// dataType 1 = int32, 2 = float32, 3 = string.
//
// Non-fatal by construction: any inconsistency gives up on the chunk and
// reports "no user data" rather than failing the whole parse, so a track whose
// markers are authored differently degrades to "no markers" instead of
// crashing the track load.
bool ReadUserDataInt(const Rd& r, const Chunk& ud, std::uint32_t* out) {
    // ARRAY 0, ELEMENT 0 — literally what FUN_004b5190(atomic, 0, 0) reads.
    // Positional on purpose: several tracks carry a second array, and on sands
    // and rouabout that second array has the SAME name as the first. Looking
    // the key up by name returns the wrong one. See the header.
    std::int32_t n = 0;
    if (!r.i32(ud.payload, &n) || n <= 0 || n > 64) return false;
    const std::size_t lim = ud.payload + ud.size;
    std::size_t q = ud.payload + 4;
    std::int32_t ln = 0;
    if (!r.i32(q, &ln) || ln < 0 || ln > 256) return false;
    q += 4;
    if (q + static_cast<std::size_t>(ln) > lim) return false;
    char name[260];
    const int nl = ln < 259 ? ln : 259;
    std::memcpy(name, r.d + q, static_cast<std::size_t>(nl));
    name[nl] = '\0';
    q += static_cast<std::size_t>(ln);
    std::int32_t dtype = 0, cnt = 0;
    if (!r.i32(q, &dtype) || !r.i32(q + 4, &cnt)) return false;
    q += 8;
    if (dtype != 1 || cnt < 1 || q + 4 > lim) return false;
    // Guard: array 0 is the "*part_id" key on every shipping track. If some
    // other key ever occupies slot 0, report "no user data" and let the marker
    // be dropped loudly rather than silently decoded as the wrong type.
    const std::size_t name_len = std::strlen(name);
    if (name_len < 7 || _stricmp(name + (name_len - 7), "part_id") != 0)
        return false;
    std::uint32_t v = 0;
    if (!r.u32(q, &v)) return false;
    *out = v;
    return true;
}

}  // namespace

bool ParsePowerupMarkers(const std::uint8_t* data, std::size_t len,
                         std::vector<PowerupMarker>* out, const char** err) {
    auto fail = [&](const char* m) { if (err) *err = m; return false; };
    if (!data || len < 12 || !out) return fail("empty blob");
    const Rd r{data, len};

    Chunk root;
    if (!ReadChunk(r, 0, &root) || root.id != kClump) return fail("not a CLUMP");
    const std::size_t end = root.payload + root.size;

    Chunk st;
    if (!ReadChunk(r, root.payload, &st) || st.id != kStruct)
        return fail("clump STRUCT missing");
    std::int32_t num_atomics = 0;
    if (!r.i32(st.payload, &num_atomics) || num_atomics < 0 || num_atomics > 4096)
        return fail("atomic count");

    // ---- FRAMELIST (0x0e) ------------------------------------------------
    Chunk fl;
    if (!FindChild(r, st.payload + st.size, end, kFrameList, &fl))
        return fail("no FRAMELIST");
    Chunk fs;
    if (!ReadChunk(r, fl.payload, &fs) || fs.id != kStruct)
        return fail("FRAMELIST STRUCT");
    std::int32_t nframes = 0;
    if (!r.i32(fs.payload, &nframes) || nframes < 0 || nframes > 4096)
        return fail("frame count");
    std::vector<Frame> frames(static_cast<std::size_t>(nframes));
    std::size_t q = fs.payload + 4;
    for (std::int32_t i = 0; i < nframes; ++i) {
        Frame& f = frames[static_cast<std::size_t>(i)];
        for (int k = 0; k < 9; ++k)
            if (!r.f32(q + static_cast<std::size_t>(k) * 4, &f.rot[k]))
                return fail("frame rot");
        for (int k = 0; k < 3; ++k)
            if (!r.f32(q + 36 + static_cast<std::size_t>(k) * 4, &f.pos[k]))
                return fail("frame pos");
        if (!r.i32(q + 48, &f.parent)) return fail("frame parent");
        if (f.parent >= i) return fail("frame parent out of order");
        q += 0x38;                                   // 9 floats + 3 floats + 2 i32
    }

    // ---- GEOMETRYLIST (0x1a) --------------------------------------------
    Chunk gl;
    if (!FindChild(r, fl.payload + fl.size, end, kGeometryList, &gl))
        return fail("no GEOMETRYLIST");
    Chunk gs;
    if (!ReadChunk(r, gl.payload, &gs) || gs.id != kStruct)
        return fail("GEOMETRYLIST STRUCT");
    std::int32_t ngeo = 0;
    if (!r.i32(gs.payload, &ngeo) || ngeo < 0 || ngeo > 4096)
        return fail("geometry count");
    // Per geometry, the one thing this file wants: the user-data int the
    // original reads through FUN_004b5190(atomic, 0, 0).
    std::vector<std::uint32_t> geo_raw(static_cast<std::size_t>(ngeo), 0);
    std::vector<bool>          geo_has(static_cast<std::size_t>(ngeo), false);
    std::size_t gq = gs.payload + gs.size;
    for (std::int32_t gi = 0; gi < ngeo; ++gi) {
        Chunk g;
        if (!ReadChunk(r, gq, &g) || g.id != kGeometry) return fail("GEOMETRY");
        const std::size_t gend = g.payload + g.size;
        Chunk gst;
        if (!ReadChunk(r, g.payload, &gst) || gst.id != kStruct)
            return fail("GEOMETRY STRUCT");
        Chunk ex;
        if (FindChild(r, gst.payload + gst.size, gend, kExtension, &ex)) {
            Chunk ud;
            if (FindChild(r, ex.payload, ex.payload + ex.size, kUserData, &ud)) {
                std::uint32_t v = 0;
                if (ReadUserDataInt(r, ud, &v)) {
                    geo_raw[static_cast<std::size_t>(gi)] = v;
                    geo_has[static_cast<std::size_t>(gi)] = true;
                }
            }
        }
        gq = gend;
    }

    // ---- ATOMICs (0x14) --------------------------------------------------
    std::vector<PowerupMarker> m;
    std::size_t aq = gl.payload + gl.size;
    int seen = 0;
    while (aq + 12 <= end) {
        Chunk a;
        if (!ReadChunk(r, aq, &a)) break;
        if (a.id == kAtomic) {
            Chunk as;
            if (!ReadChunk(r, a.payload, &as) || as.id != kStruct)
                return fail("ATOMIC STRUCT");
            std::int32_t fi = 0, gi = 0;
            if (!r.i32(as.payload, &fi) || !r.i32(as.payload + 4, &gi))
                return fail("ATOMIC indices");
            if (fi < 0 || fi >= nframes || gi < 0 || gi >= ngeo)
                return fail("ATOMIC index range");
            PowerupMarker pm{};
            const Frame& f = frames[static_cast<std::size_t>(fi)];
            pm.pos[0] = f.pos[0];          // frame+0x40, the modelling-matrix pos
            pm.pos[1] = f.pos[1];
            pm.pos[2] = f.pos[2];
            pm.atomic = seen;
            pm.has_raw = geo_has[static_cast<std::size_t>(gi)];
            pm.raw = geo_raw[static_cast<std::size_t>(gi)];
            pm.type = pm.has_raw ? static_cast<int>(pm.raw & 0xFFu) : -1;
            pm.respawn = pm.has_raw ? static_cast<float>(pm.raw >> 8) : 0.f;
            m.push_back(pm);
            ++seen;
        }
        aq = a.payload + a.size;
    }
    if (seen != num_atomics) return fail("atomic count mismatch");

    // Enumeration order: reverse of the file's atomic order (see the header).
    out->assign(m.rbegin(), m.rend());
    return true;
}

}  // namespace Track
}  // namespace mashed_re
