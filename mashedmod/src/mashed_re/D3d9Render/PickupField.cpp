// PickupField impl — see PickupField.h.
#include "PickupField.h"
#include <cmath>

namespace mashed_re {
namespace D3d9Render {

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

void PickupField::Init(const std::vector<std::array<float, 3>>& spots, float worldRadius) {
    orbs_.clear();
    collected_ = 0; held_ = -1; phase_ = 0.f;
    worldR_ = worldRadius > 1.f ? worldRadius : 100.f;
    // every 8th spot gets an orb (keeps the field readable, not cluttered)
    const int step = 8;
    for (size_t i = 0; i < spots.size(); i += step) {
        Orb o{};
        o.pos[0] = spots[i][0];
        o.pos[1] = spots[i][1] + worldR_ * 0.02f;   // float above the surface
        o.pos[2] = spots[i][2];
        o.active = true;
        o.respawn = 0.f;
        o.col = kKindCol[(i / step) % kKindCount];
        orbs_.push_back(o);
    }
}

void PickupField::Reset() {
    for (auto& o : orbs_) { o.active = true; o.respawn = 0.f; }
    collected_ = 0; held_ = -1; phase_ = 0.f;
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
            o.respawn -= dt;
            if (o.respawn <= 0.f) o.active = true;
            continue;
        }
        if (!carPos) continue;
        const float dx = carPos[0] - o.pos[0];
        const float dz = carPos[2] - o.pos[2];
        const float dy = carPos[1] - o.pos[1];
        if (dx*dx + dz*dz <= pickR2 && std::fabs(dy) <= pickR * 2.f) {
            o.active = false;
            o.respawn = 6.0f;                  // cooldown
            ++collected_;
            held_ = static_cast<int>(i % kKindCount);   // pick up its kind
            got = true;
        }
    }
    return got;
}

void PickupField::Render(IDirect3DDevice9* dev, const float camEye[3],
                         const float camAt[3]) {
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

}  // namespace D3d9Render
}  // namespace mashed_re
