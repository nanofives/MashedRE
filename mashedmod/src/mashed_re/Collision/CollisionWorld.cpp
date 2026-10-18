// Mashed RE — d2bsp: RW collision-world replica + the original's sphere walk. See CollisionWorld.h.
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "CollisionWorld.h"
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"
#include "../Math/RwLutTables.h"
#include "../Track/TrackWorld.h"
#include "../Vehicle/VehicleRecordBase.h"   // Vehicle::g_vehicleArrayBase (record 0 = the player)

#include <float.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>
#include <vector>

namespace mashed_re {
namespace Collision {
namespace {

// ---- the replica, in the original's in-memory layout (PREREG_d2bsp.md section 0) ---------------
struct RwPlaneSectorRep {          // RpPlaneSector
    std::int32_t type;             // +0x00 byte offset of the axis, 0 / 4 / 8
    float        value;            // +0x04 (not read by the walk)
    void*        left;             // +0x08
    void*        right;            // +0x0c
    float        leftValue;        // +0x10
    float        rightValue;       // +0x14
};
struct RwCollTreeRep {             // FUN_0053d5a0's allocation
    std::uint32_t  flags;          // +0x00 (bit 0: map present)
    float          sup[3];         // +0x04 (stream floats 3..5)
    float          inf[3];         // +0x10 (stream floats 0..2)
    std::uint16_t  numTris;        // +0x1c
    std::uint16_t  numSplits;      // +0x1e
    std::uint8_t*  splits;         // +0x20 16-byte split nodes, verbatim from the stream
    std::uint16_t* map;            // +0x24 leaf slot -> sector triangle index
};
struct RwSectorRep {               // RpWorldSector (the fields the walk and the callback read)
    std::int32_t   type;           // +0x00 -1 = atomic sector
    std::uint16_t* tris;           // +0x04 RpTriangle {v0, v1, v2, matIndex}
    float*         verts;          // +0x08 RwV3d
    void*          normals;        // +0x0c (unused here)
    RwCollTreeRep* coll;           // +0x10 = the 0x11D plugin slot, offset in g_bw_collPluginOffset
};
struct RwMaterialRep { void* texture; std::uint32_t color; };   // RpMaterial: colour at +0x04
struct RwWorldRep {
    std::uint8_t    pad0[0x10];
    RwMaterialRep** materials;     // +0x10 matList.materials
    std::int32_t    numMaterials;  // +0x14
    std::int32_t    space;         // +0x18
    void*           root;          // +0x1c
};
static_assert(sizeof(RwPlaneSectorRep) == 0x18, "plane sector layout");
static_assert(offsetof(RwCollTreeRep, sup) == 0x04 && offsetof(RwCollTreeRep, inf) == 0x10 &&
              offsetof(RwCollTreeRep, numTris) == 0x1c && offsetof(RwCollTreeRep, numSplits) == 0x1e &&
              offsetof(RwCollTreeRep, splits) == 0x20 && offsetof(RwCollTreeRep, map) == 0x24, "coll tree layout");
static_assert(offsetof(RwSectorRep, tris) == 0x04 && offsetof(RwSectorRep, verts) == 0x08, "sector layout");
static_assert(offsetof(RwWorldRep, materials) == 0x10 && offsetof(RwWorldRep, root) == 0x1c, "world layout");

struct Replica {
    bool ready = false;
    RwWorldRep world{};
    std::vector<RwMaterialRep>  mats;
    std::vector<RwMaterialRep*> matPtrs;
    std::deque<RwPlaneSectorRep> planes;      // deque: stable addresses on push_back
    std::deque<RwSectorRep>      sectors;
    std::deque<RwCollTreeRep>    colls;
    std::deque<std::vector<std::uint16_t>> triStore, mapStore;
    std::deque<std::vector<float>>         vertStore;
    std::deque<std::vector<std::uint8_t>>  splitStore;
};
Replica g_rep;

// ---- RW binary stream helpers -------------------------------------------------------------------
struct Chunk { std::uint32_t id, size, ver; std::size_t payload; };
bool ReadChunk(const std::uint8_t* d, std::size_t len, std::size_t off, Chunk* c) {
    if (off + 12 > len) return false;
    std::memcpy(&c->id, d + off, 4); std::memcpy(&c->size, d + off + 4, 4); std::memcpy(&c->ver, d + off + 8, 4);
    c->payload = off + 12;
    return c->payload + c->size <= len;
}
std::uint32_t U32(const std::uint8_t* d, std::size_t o) { std::uint32_t v; std::memcpy(&v, d + o, 4); return v; }
float F32(const std::uint8_t* d, std::size_t o) { float v; std::memcpy(&v, d + o, 4); return v; }

// 0x0053d2b0 -> 0x0053d820 -> 0x0053d5a0: the 0x11D sector-plugin payload into a coll tree.
RwCollTreeRep* ReadCollTree(const std::uint8_t* d, std::size_t len, std::size_t p, std::size_t end) {
    if (p + 4 > end) return nullptr;
    const std::uint32_t version = U32(d, p);                  // FUN_004cc790(stream, &v, 4)
    if (version < 0x36001u) return nullptr;                   // old layout: not present in the 13 tracks
    std::size_t q = p + 4;
    Chunk ct{};
    for (;;) {                                                // FUN_004cc5e0(stream, 0x2c): find COLLTREE
        if (!ReadChunk(d, len, q, &ct) || ct.payload + ct.size > end) return nullptr;
        if (ct.id == 0x2c) break;
        q = ct.payload + ct.size;
    }
    Chunk st{};
    q = ct.payload;
    for (;;) {                                                // FUN_004cc5e0(stream, 1): find STRUCT
        if (!ReadChunk(d, len, q, &st) || st.payload + st.size > ct.payload + ct.size) return nullptr;
        if (st.id == 0x01) break;
        q = st.payload + st.size;
    }
    if (st.size < 0x24) return nullptr;
    const std::size_t h = st.payload;                         // FUN_004cbd30(stream, &hdr, 0x24)
    const std::uint32_t flags = U32(d, h);
    const std::uint16_t numTris = static_cast<std::uint16_t>(U32(d, h + 0x1c));
    const std::uint16_t numSplits = static_cast<std::uint16_t>(U32(d, h + 0x20));
    std::size_t need = 0x24 + static_cast<std::size_t>(numSplits) * 0x10 +
                       ((flags & 1u) ? static_cast<std::size_t>(numTris) * 2 : 0);
    if (st.size < need) return nullptr;
    g_rep.colls.emplace_back();
    RwCollTreeRep* c = &g_rep.colls.back();
    c->flags = flags;
    for (int k = 0; k < 3; ++k) {
        c->sup[k] = F32(d, h + 4 + 0xc + k * 4);              // local_3c.. = stream floats 3..5
        c->inf[k] = F32(d, h + 4 + k * 4);                    // local_30.. = stream floats 0..2
    }
    c->numTris = numTris;
    c->numSplits = numSplits;
    c->splits = nullptr;
    if (numSplits) {                                          // FUN_004cbd30(stream, splits, n*0x10)
        g_rep.splitStore.emplace_back(d + h + 0x24, d + h + 0x24 + numSplits * 0x10);
        c->splits = g_rep.splitStore.back().data();
    }
    c->map = nullptr;
    if (flags & 1u) {                                         // FUN_004cc790(stream, map, numTris*2)
        const std::size_t mo = h + 0x24 + static_cast<std::size_t>(numSplits) * 0x10;
        g_rep.mapStore.emplace_back(numTris);
        std::memcpy(g_rep.mapStore.back().data(), d + mo, static_cast<std::size_t>(numTris) * 2);
        c->map = g_rep.mapStore.back().data();
    }
    return c;
}

struct Builder {
    const std::uint8_t* d; std::size_t len; const Track::World& parsed;
    std::size_t nextSector = 0;
    const char* err = nullptr;

    void* Node(std::size_t off, int depth) {
        Chunk c{};
        if (depth > 64 || !ReadChunk(d, len, off, &c)) { err = "sector chunk"; return nullptr; }
        if (c.id == 0x0A) {                                   // PLANESECTOR
            Chunk ps{};
            if (!ReadChunk(d, len, c.payload, &ps) || ps.id != 0x01 || ps.size != 0x18) { err = "plane struct"; return nullptr; }
            g_rep.planes.emplace_back();
            RwPlaneSectorRep* p = &g_rep.planes.back();
            p->type = static_cast<std::int32_t>(U32(d, ps.payload + 0x00));
            p->value = F32(d, ps.payload + 0x04);
            p->leftValue = F32(d, ps.payload + 0x10);
            p->rightValue = F32(d, ps.payload + 0x14);
            if (p->type != 0 && p->type != 4 && p->type != 8) { err = "plane type"; return nullptr; }
            const std::size_t left = ps.payload + ps.size;
            Chunk lc{};
            if (!ReadChunk(d, len, left, &lc)) { err = "left child"; return nullptr; }
            p->left = Node(left, depth + 1);
            if (!p->left) return nullptr;
            p->right = Node(lc.payload + lc.size, depth + 1);
            if (!p->right) return nullptr;
            return p;
        }
        if (c.id != 0x09) { err = "sector id"; return nullptr; }   // ATOMICSECTOR
        Chunk as{};
        if (!ReadChunk(d, len, c.payload, &as) || as.id != 0x01 || as.size < 0x2c) { err = "atomic struct"; return nullptr; }
        const std::uint32_t base = U32(d, as.payload + 0x00);
        const std::uint32_t nt = U32(d, as.payload + 0x04);
        const std::uint32_t nv = U32(d, as.payload + 0x08);
        if (nextSector >= parsed.sectors.size()) { err = "sector count"; return nullptr; }
        const Track::Sector& ps = parsed.sectors[nextSector++];
        if (ps.tris.size() != static_cast<std::size_t>(nt) * 4 || ps.verts.size() != static_cast<std::size_t>(nv) * 3) {
            err = "sector arrays"; return nullptr;
        }
        g_rep.sectors.emplace_back();
        RwSectorRep* s = &g_rep.sectors.back();
        s->type = -1;
        g_rep.vertStore.emplace_back(ps.verts);
        s->verts = g_rep.vertStore.back().data();
        g_rep.triStore.emplace_back(static_cast<std::size_t>(nt) * 4);
        std::uint16_t* t = g_rep.triStore.back().data();
        for (std::uint32_t i = 0; i < nt; ++i) {              // Track::World stores (mat+base, v0, v1, v2)
            t[i * 4 + 0] = ps.tris[i * 4 + 1];
            t[i * 4 + 1] = ps.tris[i * 4 + 2];
            t[i * 4 + 2] = ps.tris[i * 4 + 3];
            t[i * 4 + 3] = static_cast<std::uint16_t>(ps.tris[i * 4 + 0] - base);   // RpTriangle.matIndex
        }
        s->tris = t;
        s->normals = nullptr;
        s->coll = nullptr;
        Chunk ex{};
        const std::size_t exOff = as.payload + as.size;
        if (ReadChunk(d, len, exOff, &ex) && ex.id == 0x03) {
            std::size_t q = ex.payload;
            Chunk k{};
            while (q + 12 <= ex.payload + ex.size && ReadChunk(d, len, q, &k)) {
                if (k.id == 0x11d) {
                    s->coll = ReadCollTree(d, len, k.payload, k.payload + k.size);
                    if (!s->coll) { err = "coll tree"; return nullptr; }
                }
                q = k.payload + k.size;
            }
        }
        if (nt != 0 && !s->coll) { err = "non-empty sector without a 0x11D tree"; return nullptr; }
        return s;
    }
};

bool Env(const char* n) { const char* e = std::getenv(n); return e && e[0] && e[0] != '0'; }

}  // namespace
}  // namespace Collision
}  // namespace mashed_re

// ---- symbols the generated asm reads (exe-safe copies; the standalone's .rdata reads 0) -----------
extern "C" {
const std::uint32_t g_bw_c_5cc320 = 0x3f800000u;   // 1.0   (_DAT_005cc320)
const std::uint32_t g_bw_c_5e4578 = 0x3f11eb85u;   // 0.57  (_DAT_005e4578)
const std::uint32_t g_bw_c_5d757c = 0x00000000u;   // 0.0   (DAT_005d757c)
std::uint32_t g_bw_collPluginOffset = 0x10u;       // [0x007dc5b4] = offsetof(RwSectorRep, coll)
// [u9755] the line walk BW_538d60's three further .rdata reads (re/tools/naked_specs/collworld_line_u9755.json)
const std::uint32_t g_bw_c_5ce54c = 0x358637bdu;   // 1e-6  (_DAT_005ce54c)
const std::uint32_t g_bw_c_5cea1c = 0xb727c5acu;   // -1e-5 (_DAT_005cea1c)
const std::uint32_t g_bw_c_5e4574 = 0x322bcc77u;   // 1e-8  (_DAT_005e4574)
// [*(0x007d3ff8) + *(0x007d3ffc)] = sqrt table root, [.. + 4] = inv-sqrt table root (FUN_004c3b90)
struct { const std::uint32_t* sqrt_; const std::uint32_t* inv_; } g_bw_lutptrs = { nullptr, nullptr };
}
static_assert(offsetof(mashed_re::Collision::RwSectorRep, coll) == 0x10, "plugin offset");

#include "CollisionWorld_naked.inc"
#include "CollisionWorldLine_naked.inc"   // [u9755] 0x00538d60, FUN_00538c80 case 1

namespace mashed_re {
namespace Collision {

bool D2BspKey0() { static const bool s = Env("MASHED_D2BSP_KEY0"); return s; }

bool CollWorld_Ready() { return g_rep.ready; }

bool CollWorld_Build(const std::uint8_t* blob, std::size_t len, const Track::World& parsed) {
    g_rep = Replica();
    Chunk w{}, st{}, ml{};
    if (!blob || !ReadChunk(blob, len, 0, &w) || w.id != 0x0B) return false;
    if (!ReadChunk(blob, len, w.payload, &st) || st.id != 0x01) return false;
    if (!ReadChunk(blob, len, st.payload + st.size, &ml) || ml.id != 0x08) return false;
    g_rep.mats.resize(parsed.materials.size());
    g_rep.matPtrs.resize(parsed.materials.size());
    for (std::size_t i = 0; i < parsed.materials.size(); ++i) {
        const std::uint8_t* c = parsed.materials[i].rgba;   // material STRUCT +4, the RwRGBA bytes
        g_rep.mats[i].texture = nullptr;
        g_rep.mats[i].color = static_cast<std::uint32_t>(c[0]) | (static_cast<std::uint32_t>(c[1]) << 8) |
                              (static_cast<std::uint32_t>(c[2]) << 16) | (static_cast<std::uint32_t>(c[3]) << 24);
        g_rep.matPtrs[i] = &g_rep.mats[i];
    }
    Builder b{blob, len, parsed};
    void* root = b.Node(ml.payload + ml.size, 0);
    if (!root || b.nextSector != parsed.sectors.size()) {
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "[d2bsp] collision replica FAILED: %s\n", b.err ? b.err : "sector count");
            std::fclose(lf);
        }
        return false;
    }
    g_rep.world.materials = g_rep.matPtrs.empty() ? nullptr : g_rep.matPtrs.data();
    g_rep.world.numMaterials = static_cast<std::int32_t>(g_rep.matPtrs.size());
    g_rep.world.root = root;
    g_bw_lutptrs.sqrt_ = Math::RwLutSqrtTable();
    g_bw_lutptrs.inv_ = Math::RwLutInvSqrtTable();
    g_rep.ready = true;
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[d2bsp] collision replica: %u planes, %u sectors, %u trees, %u materials\n",
                     (unsigned)g_rep.planes.size(), (unsigned)g_rep.sectors.size(),
                     (unsigned)g_rep.colls.size(), (unsigned)g_rep.mats.size());
        std::fclose(lf);
    }
    return true;
}

std::uint32_t CollWorld_MaterialColour(int matIdx) {
    if (!g_rep.ready || matIdx < 0 || matIdx >= g_rep.world.numMaterials) return 0u;
    return g_rep.world.materials[matIdx]->color;              // [[world+0x10]+idx*4]+4
}

// 0x00538c80 — the dispatcher (jump table 0x00538d4c). Case 3 (0x00538cbb..0x00538cda):
// FUN_00539ec0(world, isect, isect, cb, data). Run under x87 PC_24 (the original's race-loop
// precision, memory track-load-runs-at-pc24); the CW is restored afterwards.
void CollWorld_ForAllIntersections(float* isect, CollWorldCb cb, void* data) {
    if (!g_rep.ready) return;
    const std::int32_t type = *reinterpret_cast<const std::int32_t*>(isect + 6);   // +0x18
    if (type != 3 && type != 1) return;   // cases 4, 5 keep their existing callers; out of scope
    typedef void (__cdecl* walk_fn)(void*, float*, float*, CollWorldCb, void*);
    typedef void (__cdecl* line_fn)(void*, float*, CollWorldCb, void*);
    unsigned int cw = 0, tmp = 0;
    _controlfp_s(&cw, 0, 0);
    _controlfp_s(&tmp, _PC_24, _MCW_PC);
    if (type == 3)
        reinterpret_cast<walk_fn>(&BW_539ec0)(&g_rep.world, isect, isect, cb, data);
    else   // [u9755] FUN_00538c80 case 1: FUN_00538d60(world, isect, cb, data)
        reinterpret_cast<line_fn>(&BW_538d60)(&g_rep.world, isect, cb, data);
    _controlfp_s(&tmp, cw & _MCW_PC, _MCW_PC);
}

// ---- [u9755] FUN_0046d400 + its callback FUN_0046d3a0 (verify/u9755_20261018/PREREG_u9755.md) -----------------
namespace {
// 0x0046d3a0: keep dist * 6.0 (_DAT_005cd0a0) when smaller, unless the triangle's material colour is 0xff0000ff or
// 0xff010101 (the material lookup FUN_00468b80 uses: sector+4 tris[index*4+3] -> materials[m]+4).
void* __cdecl SpawnDropCb(void* /*isect*/, void* sector, float* collTri, float dist, void* data)
{
    float* best = static_cast<float*>(data);
    const float d6 = dist * 6.0f;                                                   // param_4 * _DAT_005cd0a0
    if (d6 < *best) {
        const std::uint16_t* tris = *reinterpret_cast<std::uint16_t* const*>(reinterpret_cast<const char*>(sector) + 4);
        const int index = *reinterpret_cast<const int*>(&collTri[6]);               // tri+0x18
        const std::int32_t col = static_cast<std::int32_t>(CollWorld_MaterialColour(tris[index * 4 + 3]));
        if (col != -0xffff01 && col != -0xfefeff) *best = d6;
    }
    return collTri;
}
}  // namespace

// FUN_0046d400(x, y, z, world) as FUN_004704c0 uses it (decomp: local_8 = param_3 - FUN_0046d400): returns the spawn y
// (float)(y - FUN_0046d400(...)). Line (x, y + 3, z) -> (x, y - 3, z) (_DAT_005cc31c = 3.0), best starts at 6.0;
// no hit -> 0 - 3. False when the replica is not built.
bool CollWorld_SpawnY(float x, float y, float z, float* out)
{
    if (!g_rep.ready) return false;
    float isect[7] = { x, y + 3.0f, z, x, y - 3.0f, z, 0.0f };
    *reinterpret_cast<std::int32_t*>(&isect[6]) = 1;
    float best = 6.0f;
    CollWorld_ForAllIntersections(isect, &SpawnDropCb, &best);
    const float drop = (best != 6.0f) ? best - 3.0f : 0.0f - 3.0f;              // FUN_0046d400 return (DAT_005d757c = 0)
    *out = y - drop;
    return true;
}

// ---- MASHED_BSP_REPLAY (diagnostic) -------------------------------------------------------------
namespace {
std::uint32_t Hex(const char* s) { return static_cast<std::uint32_t>(std::strtoul(s, nullptr, 16)); }
float HexF(const char* s) { const std::uint32_t u = Hex(s); float f; std::memcpy(&f, &u, 4); return f; }
std::vector<std::string> Split(const std::string& s, char sep) {
    std::vector<std::string> out; std::size_t a = 0;
    for (;;) { const std::size_t b = s.find(sep, a); out.push_back(s.substr(a, b == std::string::npos ? std::string::npos : b - a));
               if (b == std::string::npos) break; a = b + 1; }
    return out;
}
// the 23 dwords per entry the probe captures: f[0..0xd], f[0xf], f[0x1b..0x23]
void PutEntries(std::string& o, const float* batch, int n) {
    for (int i = 0; i < n; ++i) {
        const std::uint32_t* e = reinterpret_cast<const std::uint32_t*>(batch + i * 0x24);
        if (i) o += '|';
        char b[16];
        for (int k = 0; k <= 0x23; ++k) {
            if (!(k <= 0xd || k == 0xf || (k >= 0x1b && k <= 0x23))) continue;
            std::snprintf(b, sizeof b, (k == 0 ? "%08x" : ";%08x"), e[k]);
            o += b;
        }
    }
}
// the port's classifier 0x0046cc40 on `batch` with the captured inputs, into a scratch record
void Classify(const float* batch, int n, const std::vector<std::string>& wpos, const std::vector<std::string>& vel,
              const std::vector<std::string>& wstart, const std::vector<std::string>& hist, std::string& o) {
    static int rec[0xd04 / 4 + 64];
    std::memset(rec, 0, sizeof rec);
    for (int k = 0; k < 3 && k < (int)vel.size(); ++k) rec[0x272 + k] = (int)Hex(vel[k].c_str());     // +0x9c8..
    for (int w = 0; w < 4; ++w) {
        const int b = (0x194 + w * 0xc4) / 4;
        const int src[6] = { 0, 0x16, 0x17, 0x1b, 0x1c, 0x1d };   // +0x194 +0x1ec +0x1f0 +0x200 +0x204 +0x208
        for (int k = 0; k < 6; ++k)
            if (w * 6 + k < (int)wstart.size()) rec[b + src[k]] = (int)Hex(wstart[w * 6 + k].c_str());
    }
    for (const std::string& h : hist) {                       // "slot:key:flag"
        const std::vector<std::string> f = Split(h, ':');
        if (f.size() != 3) continue;
        const int s = std::atoi(f[0].c_str());
        if (s < 0 || s >= 32) continue;
        rec[0xbfc / 4 + s] = (int)Hex(f[1].c_str());
        rec[0xbfc / 4 + 0x20 + s] = (int)Hex(f[2].c_str());
    }
    for (int k = 0; k < 12 && k < (int)wpos.size(); ++k) g_wheelContactPos[k] = HexF(wpos[k].c_str());
    for (int w = 0; w < 4; ++w) g_wheelSkipFlags[w] = 0;
    g_activeContactCount = 0;
    g_terrainEntryCount = n;
    WheelTerrainContactClassifier(rec, const_cast<float*>(batch));
    char b[16];
    for (int w = 0; w < 4; ++w) {
        const int base = (0x194 + w * 0xc4) / 4;
        const int src[6] = { 0, 0x16, 0x17, 0x1b, 0x1c, 0x1d };
        for (int k = 0; k < 6; ++k) {
            std::snprintf(b, sizeof b, (w == 0 && k == 0) ? "%08x" : ";%08x", (unsigned)rec[base + src[k]]);
            o += b;
        }
    }
}
}  // namespace

// MASHED_BATCH_DUMP=<path> (diagnostic): the batch the classifier is about to read, player slot only,
// same field layout as the original-side --batch-probe site-0 row (minus the record inputs).
void CollWorld_DumpBatch(const int* self, const float* qc, float r) {
    static const char* const s_path = std::getenv("MASHED_BATCH_DUMP");
    static std::FILE* s_f = nullptr;
    static int s_seq = 0;
    if (!s_path || self != Vehicle::g_vehicleArrayBase) return;
    if (!s_f) {
        s_f = std::fopen(s_path, "wb");
        if (!s_f) return;
        std::fprintf(s_f, "seq,frame,kind,cx,cy,cz,r,count,entries,wpos\n");
    }
    std::string o;
    char b[64];
    std::uint32_t u[4];
    std::memcpy(&u[0], &qc[0], 4); std::memcpy(&u[1], &qc[1], 4); std::memcpy(&u[2], &qc[2], 4); std::memcpy(&u[3], &r, 4);
    std::snprintf(b, sizeof b, "%d,0,Q,%08x,%08x,%08x,%08x,%d,", s_seq++, u[0], u[1], u[2], u[3], g_terrainEntryCount);
    o += b;
    PutEntries(o, reinterpret_cast<const float*>(g_terrainBatch), g_terrainEntryCount);
    o += ',';
    for (int k = 0; k < 12; ++k) {
        std::uint32_t w; std::memcpy(&w, &g_wheelContactPos[k], 4);
        std::snprintf(b, sizeof b, k ? ";%08x" : "%08x", w);
        o += b;
    }
    o += '\n';
    std::fputs(o.c_str(), s_f);
    std::fflush(s_f);
}

void CollWorld_RunReplayIfRequested() {
    static bool s_done = false;
    const char* in = std::getenv("MASHED_BSP_REPLAY");
    const char* outp = std::getenv("MASHED_BSP_REPLAY_OUT");
    if (s_done || !in || !outp || !g_rep.ready) return;
    s_done = true;
    std::FILE* fi = std::fopen(in, "rb");
    std::FILE* fo = std::fopen(outp, "wb");
    if (!fi || !fo) { if (fi) std::fclose(fi); if (fo) std::fclose(fo); return; }
    std::fprintf(fo, "seq,nW,entW,clsW\n");
    std::vector<char> line(1 << 20);
    int rows = 0;
    while (std::fgets(line.data(), (int)line.size(), fi)) {
        std::string s(line.data());
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
        // seq,frame,Q,cx,cy,cz,r,count,entries,wpos,vel,wstart,hist
        const std::vector<std::string> f = Split(s, ',');
        if (f.size() < 13 || f[2] != "Q") continue;
        const float c[3] = { HexF(f[3].c_str()), HexF(f[4].c_str()), HexF(f[5].c_str()) };
        const float r = HexF(f[6].c_str());
        const std::vector<std::string> wpos = Split(f[9], ';'), vel = Split(f[10], ';'),
                                       wstart = Split(f[11], ';');
        const std::vector<std::string> hist = f[12].empty() ? std::vector<std::string>() : Split(f[12], ';');
        std::string o = f[0];
        const int nW = ProduceTerrainBatchBsp(c, r);
        static std::vector<float> wcopy; wcopy.assign(reinterpret_cast<float*>(g_terrainBatch),
                                                      reinterpret_cast<float*>(g_terrainBatch) + nW * 0x24);
        o += ',' + std::to_string(nW) + ',';
        PutEntries(o, wcopy.data(), nW);
        o += ',';
        Classify(wcopy.data(), nW, wpos, vel, wstart, hist, o);
        o += '\n';
        std::fputs(o.c_str(), fo);
        ++rows;
    }
    std::fclose(fi);
    std::fclose(fo);
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[d2bsp] replay: %d queries -> %s\n", rows, outp);
        std::fclose(lf);
    }
}

}  // namespace Collision
}  // namespace mashed_re
