// ParticleSystem impl — see ParticleSystem.h.
#include "ParticleSystem.h"
#include <cmath>

namespace mashed_re {
namespace D3d9Render {

namespace {
constexpr int kPool = 1200;
inline void norm3(float v[3]) {
    float l = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
}
inline void cross3(const float a[3], const float b[3], float o[3]) {
    o[0] = a[1]*b[2] - a[2]*b[1];
    o[1] = a[2]*b[0] - a[0]*b[2];
    o[2] = a[0]*b[1] - a[1]*b[0];
}
}  // namespace

float ParticleSystem::Frand() {
    // xorshift32 -> [0,1)
    rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5;
    return static_cast<float>(rng_ & 0xFFFFFF) / static_cast<float>(0x1000000);
}

bool ParticleSystem::EnsureTexture(IDirect3DDevice9* dev) {
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
                const float r  = std::sqrt(dx*dx + dy*dy) * 2.f;   // 0 center..1 edge
                float a = 1.f - r;                                 // linear falloff
                if (a < 0.f) a = 0.f;
                a = a * a;                                         // soft round dot
                const std::uint8_t A = static_cast<std::uint8_t>(a * 255.f);
                row[x] = (static_cast<std::uint32_t>(A) << 24) | 0x00FFFFFFu;  // white RGB
            }
        }
        tex_->UnlockRect(0);
    }
    return true;
}

void ParticleSystem::SetAmbient(Ambient a, float worldRadius) {
    amb_ = a;
    worldR_ = worldRadius > 1.f ? worldRadius : 100.f;
}

void ParticleSystem::Reset() {
    pool_.clear();
    snowAccum_ = dustAccum_ = 0.f;
    next_ = 0;
}

ParticleSystem::P* ParticleSystem::Spawn() {
    if (pool_.size() < static_cast<size_t>(kPool)) {
        pool_.push_back(P{});
        return &pool_.back();
    }
    for (int i = 0; i < kPool; ++i) {
        next_ = (next_ + 1) % kPool;
        if (pool_[next_].life <= 0.f) return &pool_[next_];
    }
    next_ = (next_ + 1) % kPool;        // all alive -> recycle oldest-ish
    return &pool_[next_];
}

void ParticleSystem::Update(float dt, const float camEye[3], const float camFwd[3],
                            const float carPos[3], float carSpeed) {
    if (dt <= 0.f || dt > 0.25f) dt = 0.016f;   // clamp hitches

    // integrate + age
    for (auto& p : pool_) {
        if (p.life <= 0.f) continue;
        p.pos[0] += p.vel[0] * dt;
        p.pos[1] += p.vel[1] * dt;
        p.pos[2] += p.vel[2] * dt;
        p.life   -= dt;
    }

    const float R = worldR_;

    // ambient weather around the camera (snow/dust motes), biased ahead of view
    if (amb_ != None) {
        const float rate = (amb_ == Snow) ? 110.f : 70.f;   // particles/sec
        snowAccum_ += rate * dt;
        int spawn = static_cast<int>(snowAccum_);
        snowAccum_ -= spawn;
        if (spawn > 60) spawn = 60;
        for (int i = 0; i < spawn; ++i) {
            P* p = Spawn();
            const float spread = R * 0.45f;
            // seed in a box centered a bit ahead of the camera
            const float ax = camFwd[0] * R * 0.20f, az = camFwd[2] * R * 0.20f;
            p->pos[0] = camEye[0] + ax + (Frand() - 0.5f) * 2.f * spread;
            p->pos[1] = camEye[1] + R * 0.30f + Frand() * R * 0.15f;   // above
            p->pos[2] = camEye[2] + az + (Frand() - 0.5f) * 2.f * spread;
            if (amb_ == Snow) {
                p->vel[0] = (Frand() - 0.5f) * R * 0.06f;     // slight wind
                p->vel[1] = -R * (0.12f + Frand() * 0.06f);   // fall
                p->vel[2] = (Frand() - 0.5f) * R * 0.06f;
                p->size = R * (0.0025f + Frand() * 0.0030f);  // fine flakes
                const std::uint8_t a = static_cast<std::uint8_t>(150 + Frand() * 70.f);
                p->col = (static_cast<std::uint32_t>(a) << 24) | 0x00FFFFFFu;  // white
            } else {  // Dust
                p->vel[0] = (Frand() - 0.5f) * R * 0.04f;
                p->vel[1] = -R * (0.04f + Frand() * 0.03f);
                p->vel[2] = (Frand() - 0.5f) * R * 0.04f;
                p->size = R * (0.010f + Frand() * 0.010f);
                const std::uint8_t a = static_cast<std::uint8_t>(80 + Frand() * 60.f);
                p->col = (static_cast<std::uint32_t>(a) << 24) | 0x00B0A080u;  // tan
            }
            p->maxlife = p->life = 2.0f + Frand() * 2.0f;
        }
    }

    // car dust: spawn behind/under the car proportional to speed
    if (carPos && carSpeed > R * 0.03f) {
        const float rate = 60.f * (carSpeed / (R * 0.25f));
        dustAccum_ += rate * dt;
        int spawn = static_cast<int>(dustAccum_);
        dustAccum_ -= spawn;
        if (spawn > 30) spawn = 30;
        for (int i = 0; i < spawn; ++i) {
            P* p = Spawn();
            p->pos[0] = carPos[0] + (Frand() - 0.5f) * R * 0.02f;
            p->pos[1] = carPos[1] + Frand() * R * 0.01f;
            p->pos[2] = carPos[2] + (Frand() - 0.5f) * R * 0.02f;
            p->vel[0] = (Frand() - 0.5f) * R * 0.08f;
            p->vel[1] =  R * (0.03f + Frand() * 0.05f);     // puff up
            p->vel[2] = (Frand() - 0.5f) * R * 0.08f;
            p->size = R * (0.012f + Frand() * 0.012f);
            const std::uint8_t a = static_cast<std::uint8_t>(110 + Frand() * 80.f);
            p->col = (static_cast<std::uint32_t>(a) << 24) | 0x00D8E0E8u;  // snow-spray white
            p->maxlife = p->life = 0.7f + Frand() * 0.6f;
        }
    }
}

int ParticleSystem::alive() const {
    int n = 0;
    for (const auto& p : pool_) if (p.life > 0.f) ++n;
    return n;
}

void ParticleSystem::Render(IDirect3DDevice9* dev, const float camEye[3],
                            const float camAt[3]) {
    if (!dev || !EnsureTexture(dev)) return;

    // camera basis for billboarding
    float fwd[3] = {camAt[0]-camEye[0], camAt[1]-camEye[1], camAt[2]-camEye[2]};
    norm3(fwd);
    const float wup[3] = {0.f, 1.f, 0.f};
    float right[3]; cross3(wup, fwd, right); norm3(right);
    float up[3];    cross3(fwd, right, up);

    verts_.clear();
    verts_.reserve(static_cast<size_t>(alive()) * 6);
    for (const auto& p : pool_) {
        if (p.life <= 0.f) continue;
        // fade alpha over life (in/out)
        const float lf = p.life / (p.maxlife > 1e-3f ? p.maxlife : 1.f);  // 1->0
        float fade = lf < 0.25f ? (lf / 0.25f) : 1.f;                     // fade out tail
        const std::uint32_t baseA = (p.col >> 24) & 0xFF;
        const std::uint32_t a = static_cast<std::uint32_t>(baseA * fade);
        const D3DCOLOR c = (a << 24) | (p.col & 0x00FFFFFF);
        const float s = p.size;
        const float rx = right[0]*s, ry = right[1]*s, rz = right[2]*s;
        const float ux = up[0]*s,    uy = up[1]*s,    uz = up[2]*s;
        const float* P = p.pos;
        PV v0{P[0]-rx-ux, P[1]-ry-uy, P[2]-rz-uz, c, 0.f, 1.f};
        PV v1{P[0]-rx+ux, P[1]-ry+uy, P[2]-rz+uz, c, 0.f, 0.f};
        PV v2{P[0]+rx+ux, P[1]+ry+uy, P[2]+rz+uz, c, 1.f, 0.f};
        PV v3{P[0]+rx-ux, P[1]+ry-uy, P[2]+rz-uz, c, 1.f, 1.f};
        verts_.push_back(v0); verts_.push_back(v1); verts_.push_back(v2);
        verts_.push_back(v0); verts_.push_back(v2); verts_.push_back(v3);
    }
    if (verts_.empty()) return;

    // state: alpha-blend, depth-test ON / write OFF, no fog/cull/lighting
    dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    D3DMATRIX wm = { {{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}} };
    dev->SetTransform(D3DTS_WORLD, &wm);
    dev->SetTexture(0, tex_);
    dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    dev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                         static_cast<UINT>(verts_.size() / 3),
                         verts_.data(), sizeof(PV));

    // leave depth-write restored for whatever draws next; blend left enabled is
    // harmless (the HUD path sets its own states).
    dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

}  // namespace D3d9Render
}  // namespace mashed_re
