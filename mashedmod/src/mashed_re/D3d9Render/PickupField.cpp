// PickupField impl — see PickupField.h.
#include "PickupField.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace mashed_re {
namespace D3d9Render {

// ---- P3 acceptance instrumentation (pickups stage 1) ----------------------
// Inert unless MASHED_DBG_PICKUPDUMP is set in the environment. When armed,
// Render() records — for the frame it just drew — every live orb's world
// centre and the screen-space disc the ENGINE projects it to, taken from the
// very D3DTS_VIEW * D3DTS_PROJECTION and viewport that draw used. The
// acceptance mask in verify/pickups_fix_20261001/PREREG_STAGE1.md rule P3 is
// built from these discs, i.e. from projected geometry, never from a colour
// class or a hand-drawn rectangle.
//
// This block writes no pixels and is read-only with respect to every value the
// renderer computes; the pre-registered instrumentation control measures that
// (HEAD vs HEAD+this must differ in ZERO pixels over the whole frame).
namespace {
std::string g_projdump;
bool ProjDumpArmed() {
    static const bool armed = std::getenv("MASHED_DBG_PICKUPDUMP") != nullptr;
    return armed;
}
}  // namespace

void PickupField_WriteProjDump(const char* bmp_path) {
    if (!ProjDumpArmed() || !bmp_path || g_projdump.empty()) return;
    char p[320];
    std::snprintf(p, sizeof(p), "%s.pudump.txt", bmp_path);
    if (std::FILE* f = std::fopen(p, "w")) {
        std::fwrite(g_projdump.data(), 1, g_projdump.size(), f);
        std::fclose(f);
    }
}

namespace {
inline void norm3(float v[3]) {
    float l = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
}
inline void cross3(const float a[3], const float b[3], float o[3]) {
    o[0]=a[1]*b[2]-a[2]*b[1]; o[1]=a[2]*b[0]-a[0]*b[2]; o[2]=a[0]*b[1]-a[1]*b[0];
}
const std::uint32_t kKindCol[PickupField::kKindCount] = {
    0xffff5040u,  // Missile - red
    0xff70d0ffu,  // Mine    - cyan
    0xffffe040u,  // Shock   - yellow
    0xff60ff70u,  // Boost   - green
    0xffc0a0ffu,  // Shield  - violet
};
}  // namespace

const char* PickupField::KindName(int k) {
    static const char* n[kKindCount] = {"Missile", "Mine", "Shock", "Boost", "Shield"};
    return (k >= 0 && k < kKindCount) ? n[k] : "None";
}

// Faithful MASHED type names (data-verified vs POWERUPS_GOLD.LUA constants and the
// runtime effect table @0x005f9998, which holds entries for codes 7,9,10,11,12,16,
// 17,18,19). Codes 6/8/21 have no own table entry (D1 map U-WSD-3).
const char* PickupField::RealTypeName(int t) {
    switch (t) {
        case 6:  return "Mine";        // MINE   (no own effect entry; aliases P_MINE? U-WSD-3)
        case 7:  return "Mortar";      // MORTAR
        case 8:  return "Detonator";   // DETONATOR (no own effect entry; U-WSD-3)
        case 9:  return "Gatling Gun"; // GUN
        case 10: return "Drum";        // DRUM
        case 11: return "Missile";     // MISSILE
        case 12: return "Proximity Mine"; // P_MINE
        case 16: return "Flamethrower";   // R_FLAME
        case 17: return "Shotgun";     // SHOTGUN
        case 18: return "Flash";       // FLASH
        case 19: return "Oil Slick";   // OIL
        case 21: return "Random";      // BLANK (random box; U-WSD-3)
        default: return "Power-up";
    }
}

bool PickupField::TypeHasEffectEntry(int t) {
    switch (t) {
        case 7: case 9: case 10: case 11: case 12:
        case 16: case 17: case 18: case 19: return true;   // present in 0x005f9998
        default:                            return false;  // 6/8/21 absent (U-WSD-3)
    }
}

float PickupField::Frand() {
    rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5;
    return static_cast<float>(rng_ & 0xFFFFFF) / static_cast<float>(0x1000000);
}

bool PickupField::EnsureTexture(IDirect3DDevice9* dev) {
    if (tex_) return true;
    if (!dev) return false;
    const UINT S = 32;
    if (FAILED(dev->CreateTexture(S, S, 1, 0, D3DFMT_A8R8G8B8,
                                  D3DPOOL_MANAGED, &tex_, nullptr)) || !tex_)
        return false;
    D3DLOCKED_RECT lr{};
    if (SUCCEEDED(tex_->LockRect(0, &lr, nullptr, 0))) {
        for (UINT y = 0; y < S; ++y) {
            std::uint32_t* row = reinterpret_cast<std::uint32_t*>(
                static_cast<std::uint8_t*>(lr.pBits) + y * lr.Pitch);
            for (UINT x = 0; x < S; ++x) {
                const float dx = (x + 0.5f) / S - 0.5f;
                const float dy = (y + 0.5f) / S - 0.5f;
                const float r = std::sqrt(dx*dx + dy*dy) * 2.f;     // 0..1
                // bright core + a ring halo -> reads as a glowing pickup orb
                float a = (r < 0.55f) ? 1.f : (r < 0.9f ? 0.6f * (1.f - (r-0.55f)/0.35f) : 0.f);
                if (a < 0.f) a = 0.f;
                const std::uint8_t A = static_cast<std::uint8_t>(a * 255.f);
                row[x] = (static_cast<std::uint32_t>(A) << 24) | 0x00FFFFFFu;
            }
        }
        tex_->UnlockRect(0);
    }
    return true;
}

// [SCAFFOLD] MASHED powerup type -> one of the 5 scaffolded effect Kinds. This is
// an INVENTED stand-in: the real effects are the per-type FIRE handlers in the
// 0x005f9998 table (Missile FIRE = FUN_00455150, etc.), gated on WS-A1/WS-B/WS-E
// (D1 map). The mapping picks the nearest scaffold behaviour per real type so the
// live stand-in stays plausible until the verbatim per-type port (D2) replaces it.
int PickupField::KindFromType(int t) {
    switch (t) {
        case 11:                      return Missile;  // MISSILE  -> homing projectile
        case 7: case 8: case 10:      return Missile;  // MORTAR/DETONATOR/DRUM -> projectile
        case 6: case 12:              return Mine;     // MINE / P_MINE -> dropped hazard
        case 9: case 17:              return Shock;    // GUN / SHOTGUN -> disrupt nearby
        case 16:                      return Boost;    // R_FLAME (flamethrower) -> forward burst
        case 19:                      return Mine;     // OIL (slick) -> dropped hazard
        case 18:                      return Shield;   // FLASH (blind) -> defensive stand-in
        default:                      return Boost;    // BLANK(21) random box -> stand-in
    }
}
std::uint32_t PickupField::ColForType(int t) {
    int k = KindFromType(t);
    return (k >= 0 && k < kKindCount) ? kKindCol[k] : 0xffffffffu;
}

void PickupField::Init(const std::vector<std::array<float, 3>>& spots, float worldRadius) {
    orbs_.clear();
    collected_ = 0; held_ = -1; held_type_ = -1; phase_ = 0.f;
    worldR_ = worldRadius > 1.f ? worldRadius : 100.f;
    // FALLBACK placement (no POWERUPS_GOLD.LUA): every 8th gate gets an orb.
    const int step = 8;
    for (size_t i = 0; i < spots.size(); i += step) {
        Orb o{};
        o.pos[0] = spots[i][0];
        o.pos[1] = spots[i][1] + worldR_ * 0.02f;   // float above the surface
        o.pos[2] = spots[i][2];
        o.active = true; o.respawn = 6.0f; o.cooldown = 0.f;
        o.gameType = -1;                            // index-based kind
        o.col = kKindCol[(i / step) % kKindCount];
        orbs_.push_back(o);
    }
}

// Spawn the pool, applying the original's accept/reject predicates verbatim.
//
// FUN_00458e00 @0x00458e00, normal-race arm, in its own order:
//
//     iVar2 = FUN_0042fe30();                       // placement rank
//     if (0x18 < DAT_0068b9a8) return -1;           // pool cap: 25 entries
//     ... for each existing entry:                  // dedupe against +0x2c..0x34
//         if (dx*dx + dy*dy + dz*dz < _DAT_005cc558) return -1;
//     if (iVar2 == 2) { ... }                       // BLANK-only arm, not wired
//     else if (param_2 == 0x15) return -1;          // reject BLANK (type 21)
//     ... position written to BOTH +0x2c..0x34 and +0x38..0x40, verbatim
//
// _DAT_005cc558 = 0x3a83126f = 0.00100000005, a SQUARED distance.
//
// Rank. FUN_0042fe30 @0x0042fe30 is `FUN_0042f6a0() != 0xb ? DAT_0067ea74 : 1`.
// Measured live: a normal race reads 1 (Quick Battle on TRAINING, Challenge Cup
// on ARCTIC), and the rank-0 routes place NOTHING at all (FUN_004264d0's early
// return at 0x004265a6, live count 0). Only the rank-1 arm is wired here, which
// is the arm the standalone's race flow corresponds to and the only one with
// live evidence; rank 0 and the rank-2 BLANK randomiser (FUN_00458d00
// @0x00458d00) are [UNCERTAIN] because what drives DAT_0067ea74 has not been
// derived. See verify/pickups_fix_20261001/PREREG_STAGE1.md.
//
// NOTE the original stores the position VERBATIM into both the live position
// and the anchor. The port's old `worldR_ * 0.012f` Y lift was invented and is
// gone; the boxes sit where the DFF frame puts them.
void PickupField::InitReal(const std::vector<Spawn>& spawns, float worldRadius) {
    orbs_.clear();
    collected_ = 0; held_ = -1; held_type_ = -1; phase_ = 0.f;
    worldR_ = worldRadius > 1.f ? worldRadius : 100.f;
    const float kDedupeEps2 = 0.00100000005f;   // _DAT_005cc558 = 0x3a83126f
    const int   kPoolCap    = 25;               // 0x18 < DAT_0068b9a8 -> reject
    std::FILE* lf = std::fopen("mashed_re.log", "a");
    for (const Spawn& s : spawns) {
        const char* reject = nullptr;
        if (static_cast<int>(orbs_.size()) > kPoolCap - 1) {
            reject = "cap";
        } else {
            for (const Orb& e : orbs_) {
                const float dx = e.pos[0] - s.pos[0];
                const float dy = e.pos[1] - s.pos[1];
                const float dz = e.pos[2] - s.pos[2];
                if (dx*dx + dy*dy + dz*dz < kDedupeEps2) { reject = "dedupe"; break; }
            }
            if (!reject && s.type == 0x15) reject = "blank";
        }
        if (reject) {
            if (lf) std::fprintf(lf, "PUDROP atomic=%d type=%d reason=%s\n",
                                 s.atomic, s.type, reject);
            continue;
        }
        Orb o{};
        o.pos[0] = s.pos[0];
        o.pos[1] = s.pos[1];
        o.pos[2] = s.pos[2];
        o.active = true;
        o.respawn = s.respawn > 0.f ? s.respawn : 5.0f;
        o.cooldown = 0.f;
        o.gameType = s.type;
        o.col = ColForType(s.type);
        if (lf) {
            std::uint32_t b[3];
            std::memcpy(b, o.pos, sizeof(b));
            std::fprintf(lf,
                         "PUPLACE i=%d atomic=%d type=%d respawn=%.6f "
                         "pos=%.8f,%.8f,%.8f bits=%08x,%08x,%08x\n",
                         static_cast<int>(orbs_.size()), s.atomic, s.type,
                         o.respawn, o.pos[0], o.pos[1], o.pos[2],
                         b[0], b[1], b[2]);
        }
        orbs_.push_back(o);
    }
    if (lf) {
        std::fprintf(lf, "PUINIT placed=%d of %d spawns worldR=%.6f\n",
                     static_cast<int>(orbs_.size()),
                     static_cast<int>(spawns.size()), worldR_);
        std::fclose(lf);
    }
}

void PickupField::Reset() {
    for (auto& o : orbs_) { o.active = true; o.cooldown = 0.f; }
    collected_ = 0; held_ = -1; held_type_ = -1; phase_ = 0.f;
}

bool PickupField::Update(float dt, const float carPos[3]) {
    if (dt <= 0.f || dt > 0.25f) dt = 0.016f;
    phase_ += dt;
    bool got = false;
    const float pickR = worldR_ * 0.04f;       // collection radius
    const float pickR2 = pickR * pickR;
    for (size_t i = 0; i < orbs_.size(); ++i) {
        Orb& o = orbs_[i];
        if (!o.active) {
            o.cooldown -= dt;
            if (o.cooldown <= 0.f) o.active = true;
            continue;
        }
        if (!carPos) continue;
        const float dx = carPos[0] - o.pos[0];
        const float dz = carPos[2] - o.pos[2];
        const float dy = carPos[1] - o.pos[1];
        if (dx*dx + dz*dz <= pickR2 && std::fabs(dy) <= pickR * 2.f) {
            o.active = false;
            o.cooldown = o.respawn;            // real per-pickup respawn time
            ++collected_;
            held_ = (o.gameType >= 0) ? KindFromType(o.gameType)
                                      : static_cast<int>(i % kKindCount);
            held_type_ = o.gameType;           // faithful real type code (HUD)
            got = true;
        }
    }
    return got;
}

bool PickupField::CollectAt(const float carPos[3], int* type) {
    if (!carPos) return false;
    const float pickR = worldR_ * 0.04f;       // same collection radius as Update
    const float pickR2 = pickR * pickR;
    for (size_t i = 0; i < orbs_.size(); ++i) {
        Orb& o = orbs_[i];
        if (!o.active) continue;
        const float dx = carPos[0] - o.pos[0];
        const float dz = carPos[2] - o.pos[2];
        const float dy = carPos[1] - o.pos[1];
        if (dx*dx + dz*dz <= pickR2 && std::fabs(dy) <= pickR * 2.f) {
            o.active = false;
            o.cooldown = o.respawn;
            if (type) *type = o.gameType;
            return true;
        }
    }
    return false;
}

int PickupField::ActiveOrbCount() const {
    int n = 0;
    for (const Orb& o : orbs_) if (o.active) ++n;
    return n;
}

float PickupField::NearestActiveOrbDist(const float pos[3]) const {
    float best = -1.f;
    for (const Orb& o : orbs_) {
        if (!o.active) continue;
        const float dx = pos[0] - o.pos[0], dz = pos[2] - o.pos[2];
        const float d = std::sqrt(dx * dx + dz * dz);
        if (best < 0.f || d < best) best = d;
    }
    return best;
}

void PickupField::Render(IDirect3DDevice9* dev, const float camEye[3],
                         const float camAt[3]) {
    // Stale-dump guard: every frame starts from empty, so a frame on which the
    // pickup draw bailed out writes no dump at all and the checker fails loudly
    // instead of masking with the previous frame's discs.
    if (ProjDumpArmed()) g_projdump.clear();
    if (!dev || orbs_.empty() || !EnsureTexture(dev)) return;
    float fwd[3] = {camAt[0]-camEye[0], camAt[1]-camEye[1], camAt[2]-camEye[2]};
    norm3(fwd);
    const float wup[3] = {0,1,0};
    float right[3]; cross3(wup, fwd, right); norm3(right);
    float up[3]; cross3(fwd, right, up);

    const float bob = std::sin(phase_ * 2.0f) * worldR_ * 0.008f;
    const float s   = worldR_ * 0.018f;
    const float rx=right[0]*s, ry=right[1]*s, rz=right[2]*s;
    const float ux=up[0]*s, uy=up[1]*s, uz=up[2]*s;

    // Recorded BEFORE the draw, from the same view/projection this frame set,
    // so the dump exists even on a frame where every orb is collected and the
    // draw below bails out on an empty vertex list.
    if (ProjDumpArmed()) RecordProjDump(dev, bob, s);

    verts_.clear();
    for (const auto& o : orbs_) {
        if (!o.active) continue;
        const float P[3] = {o.pos[0], o.pos[1] + bob, o.pos[2]};
        const D3DCOLOR c = o.col;
        PV v0{P[0]-rx-ux,P[1]-ry-uy,P[2]-rz-uz,c,0,1};
        PV v1{P[0]-rx+ux,P[1]-ry+uy,P[2]-rz+uz,c,0,0};
        PV v2{P[0]+rx+ux,P[1]+ry+uy,P[2]+rz+uz,c,1,0};
        PV v3{P[0]+rx-ux,P[1]+ry-uy,P[2]+rz-uz,c,1,1};
        verts_.push_back(v0); verts_.push_back(v1); verts_.push_back(v2);
        verts_.push_back(v0); verts_.push_back(v2); verts_.push_back(v3);
    }
    if (verts_.empty()) return;

    D3DMATRIX wm = { {{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}} };
    dev->SetTransform(D3DTS_WORLD, &wm);
    dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    dev->SetTexture(0, tex_);
    dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    dev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);     // additive glow
    dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                         static_cast<UINT>(verts_.size() / 3),
                         verts_.data(), sizeof(PV));
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

// ---- P3 instrumentation; see the block at the top of this file. ------------
void PickupField::RecordProjDump(IDirect3DDevice9* dev, float bob, float s) {
        D3DMATRIX V{}, Pm{};
        D3DVIEWPORT9 vp{};
        if (FAILED(dev->GetTransform(D3DTS_VIEW, &V)) ||
            FAILED(dev->GetTransform(D3DTS_PROJECTION, &Pm)) ||
            FAILED(dev->GetViewport(&vp)))
            return;
        // Quad corner reach (half-size s along both camera axes) plus the full
        // bob amplitude, so the disc bounds the orb at ANY bob phase and a
        // phase difference between two arms cannot move the mask.
        const float bound = s * 1.41421356f + worldR_ * 0.008f;
        char hdr[256];
        std::snprintf(hdr, sizeof(hdr),
                      "PUDUMP v1 vp=%ux%u worldR=%.6f orbs=%d active=%d "
                      "halfsize=%.6f bound_r=%.6f\n",
                      vp.Width, vp.Height, worldR_,
                      static_cast<int>(orbs_.size()), ActiveOrbCount(),
                      s, bound);
        g_projdump.assign(hdr);
        for (std::size_t i = 0; i < orbs_.size(); ++i) {
            const Orb& o = orbs_[i];
            const float P[3] = {o.pos[0], o.pos[1] + bob, o.pos[2]};
            // world -> clip through the matrices this frame actually drew with
            float e[4];
            for (int c = 0; c < 4; ++c)
                e[c] = P[0]*V.m[0][c] + P[1]*V.m[1][c] + P[2]*V.m[2][c] + V.m[3][c];
            float cl[4];
            for (int c = 0; c < 4; ++c)
                cl[c] = e[0]*Pm.m[0][c] + e[1]*Pm.m[1][c] +
                        e[2]*Pm.m[2][c] + e[3]*Pm.m[3][c];
            const bool vis = (cl[3] > 1e-4f);
            const float sx = vis ? (cl[0]/cl[3]*0.5f + 0.5f) * vp.Width  + vp.X : 0.f;
            const float sy = vis ? (0.5f - cl[1]/cl[3]*0.5f) * vp.Height + vp.Y : 0.f;
            // same depth, same vertical scale the projection applies
            const float rp = vis ? bound * Pm.m[1][1] / cl[3] * 0.5f * vp.Height
                                 : 0.f;
            char ln[320];
            std::snprintf(ln, sizeof(ln),
                          "ORB %d type=%d respawn=%.6f w=%.8f,%.8f,%.8f "
                          "ez=%.6f s=%.4f,%.4f r=%.4f active=%d onscreen=%d\n",
                          static_cast<int>(i), o.gameType, o.respawn,
                          o.pos[0], o.pos[1], o.pos[2],
                          cl[3], sx, sy, rp, o.active ? 1 : 0, vis ? 1 : 0);
            g_projdump += ln;
        }
}

}  // namespace D3d9Render
}  // namespace mashed_re
