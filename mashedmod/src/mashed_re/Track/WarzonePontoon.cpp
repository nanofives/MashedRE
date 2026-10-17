// Mashed RE — the Warzone (course 0x21) track object at 0x00684d18, the moving battle platform ("pontoon"). Exe only.
//
// Decode, arms and predictions: verify/pontoon_20261016/PREREG_pontoon.md (committed before the first run), result:
// RESULT_pontoon.md. Every function below is a transcription of the cited RVA; float arithmetic follows the disassembly's
// operation order (verify/pontoon_20261016/disasm_*.txt). The TU is x87 (x87_tus.txt) and Pontoon_Tick runs at PC_24, the
// original race loop's precision (memory track-load-runs-at-pc24), so each x87 step rounds as the original's does.
//
// The object lives at the original's address 0x00684d18 (+0x00..+0xcc), so the ported AI reader 0x00414f00
// (Ai/AiTrackObjSeek.cpp) reads 0x00684de0 (= +0xc8) and 0x00684dac (= +0x94) unchanged; the exe image backs the
// whole MASHED arena (exe_main.cpp g_b17_low_arena_pad). The words 0x0088ff50..5c, the Tower / Deck tracked matrices
// 0x00684cd8 / 0x00684c98 and their rest copies 0x00684de8 / 0x00684c58 are the original's addresses too. The RW objects
// the original keeps behind pointers (clump, RpHAnim hierarchy, RtAnimInterpolator, frames) are this TU's own structs,
// laid out where the transcribed code depends on the layout (keyframes 36 bytes with a prevFrame pointer first, the
// interpolated frames at +0x4c, 36 bytes each).
//
// Not here, and why (PREREG section 1): the exit hand-off to the car's RWP rigid body (FUN_00480720) and the per-wheel
// RWP impulses (FUN_00481750) have no exe counterpart (no per-car RWP mode); the Tower / Deck bodies are matrices only
// (no scene-object physics in the exe), so FUN_0047d180's body update and the velocity-row zeroing at 0x00452bd5 have
// nothing to write to.
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

#include "WarzonePontoon.h"
#include "../Math/RwLutTables.h"

#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>

extern "C" void __cdecl RwMatrixInvertEntry(void* out, void* in);                        // Render/RwMatrixInvert.cpp
extern "C" unsigned int* __cdecl FUN_004c52f0(unsigned int* dst, unsigned int* src, int op);   // Collision/RwpSolverMath2.cpp
namespace mashed_re { namespace Math {
void* RwMatrixMultiplyCPUDispatch(void* out, const void* A, const void* B);   // 0x004c4600 (Math/RwMatrixMultiplyCpu.cpp)
} }

namespace mashed_re {
namespace Track {

namespace {

inline float FB(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
inline std::uint32_t BF(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }
inline std::uint32_t& Flags(float* m) { return *reinterpret_cast<std::uint32_t*>(m + 3); }
inline std::uint32_t FlagsC(const float* m) { std::uint32_t u; std::memcpy(&u, m + 3, 4); return u; }

// .rdata constants, from the unpatched image (memory reads 2026-10-16).
const float k1     = FB(0x3f800000u);   // _DAT_005cc320
const float kHalf  = FB(0x3f000000u);   // _DAT_005cc32c
const float k0     = FB(0x00000000u);   // DAT_005d757c
const float kInv3k = FB(0x39aec33eu);   // _DAT_005cc948  1/3000 (the slot-1 dt)
const float k2p32  = FB(0x4f800000u);   // _DAT_005cc94c  2^32 (unsigned units fix-up)
const float k388   = FB(0x39ae9681u);   // _DAT_005ce388  state 1 countdown rate
const float k384   = FB(0x3a2e9681u);   // _DAT_005ce384  state 2 / 3 anim rate
const float k360   = FB(0x43b40000u);   // _DAT_005ccac4  movement scale (FUN_00451ce0)
const float k2     = FB(0x40000000u);   // _DAT_005cc574  state 2 -> 3 time
const float kDot   = FB(0x3f7fbe77u);   // _DAT_005cea88  0.999 slerp threshold
const float kPi    = FB(0x40490fdbu);   // _DAT_005ce2f4
const float kPio2  = FB(0x3fc90fdbu);   // _DAT_005cd108
const float kC0    = FB(0x40490fdau);   // _DAT_005d8dc0
const float kC4    = FB(0x3fc90fdau);   // _DAT_005d8dc4
const float kC8    = FB(0x33a22168u);   // _DAT_005d8dc8
const float kQ1    = FB(0x4019d139u);   // _DAT_005d8dcc
const float kQ2    = FB(0x4001572du);   // _DAT_005d8dd0
const float kQ3    = FB(0x3f303361u);   // _DAT_005d8dd4
const float kQ4    = FB(0x3d9dc62eu);   // _DAT_005d8dd8
const float kP1    = FB(0x3ea6b090u);   // _DAT_005d8ddc
const float kP2    = FB(0x3e4e0aa8u);   // _DAT_005d8de0
const float kP3    = FB(0x3d241146u);   // _DAT_005d8de4
const float kP4    = FB(0x3a4f7f04u);   // _DAT_005d8de8
const float kP5    = FB(0x3811ef08u);   // _DAT_005d8dec
const float kS0    = FB(0x3e2aaaabu);   // _DAT_005cc8f4 (also the acos P0)
const float kS1    = FB(0x3c088889u);   // _DAT_005cc8f8
const float kS2    = FB(0x39500d01u);   // _DAT_005cc8fc
const float kS3    = FB(0x3638ef1bu);   // _DAT_005cc900
const float kS4    = FB(0x32d72f34u);   // _DAT_005cc904
const float kS5    = FB(0x2f2ec9d3u);   // _DAT_005cc908
const std::uint32_t kMarkerColour = 0xff804040u;   // DAT_005f9788 (contact-filter colour, FUN_0047d640)

// ---- RW matrix leaves ------------------------------------------------------------------------------------------

using Math::LutInvSqrt;   // FUN_004c3b90 with the real LUT (Math/RwLutTables.cpp)

// Shared bodies (one per RVA per target, scripts/lint_rva_bodies.py): RwMatrixMultiply 0x004c4600 = the dispatcher in
// Math/RwMatrixMultiplyCpu.cpp; RwMatrixCombine 0x004c52f0 = Collision/RwpSolverMath2.cpp (exe arm added by this lane);
// RwMatrixInvert 0x004c4dc0 = Render/RwMatrixInvert.cpp's naked entry (exe arm added by this lane).
void MatMul(float* out, const float* a, const float* b) { Math::RwMatrixMultiplyCPUDispatch(out, a, b); }
void CombinePost(float* dst, const float* src) {          // RwMatrixCombine(dst, src, rwCOMBINEPOSTCONCAT = 2)
    FUN_004c52f0(reinterpret_cast<unsigned int*>(dst), reinterpret_cast<unsigned int*>(const_cast<float*>(src)), 2);
}
void MatInvert(float* out, const float* in) { RwMatrixInvertEntry(out, const_cast<float*>(in)); }

// 0x004c4680 (thunk_FUN_004c4680): re-orthonormalize, transcribed from disasm_ortho.txt (sums (y*y + x*x) + z*z,
// cross products from the register values, dst flags = (dst flags & ~0x20000) | 3, translation kept).
void OrthoNormalize(float* dst, const float* src) {
    float r[3] = { src[0], src[1], src[2] }, u[3] = { src[4], src[5], src[6] }, a[3] = { src[8], src[9], src[10] };
    const float px = src[12], py = src[13], pz = src[14];
    const float ir = LutInvSqrt((r[1] * r[1] + r[0] * r[0]) + r[2] * r[2]);    // 0x004c469d..0x004c4702
    r[0] = ir * r[0]; r[1] = ir * r[1]; r[2] = ir * r[2];
    const float iu = LutInvSqrt((u[1] * u[1] + u[0] * u[0]) + u[2] * u[2]);    // 0x004c472b..0x004c474a
    u[0] = iu * u[0]; u[1] = iu * u[1]; u[2] = iu * u[2];
    const float ia = LutInvSqrt((a[1] * a[1] + a[0] * a[0]) + a[2] * a[2]);    // 0x004c4773..0x004c4792
    a[0] = a[0] * ia; a[1] = a[1] * ia; a[2] = a[2] * ia;
    float *p4, *p5, *p6;                                                       // ebx, esi, edi
    if (!(ir > k0))      { p4 = u; p5 = a; p6 = r; }                           // 0x004c47c9 -> 0x004c48ed/ef
    else if (!(iu > k0)) { p4 = a; p5 = r; p6 = u; }                           // 0x004c47e0 -> 0x004c48dd
    else if (!(ia > k0)) { p4 = r; p5 = u; p6 = a; }                           // 0x004c47f3 -> 0x004c48b4
    else {
        float dau = (a[1] * u[1] + a[0] * u[0]) + a[2] * u[2];               // 0x004c47f9..0x004c4815
        if (dau < k0) dau = -dau;
        float dar = (a[1] * r[1] + a[0] * r[0]) + a[2] * r[2];               // 0x004c4832..0x004c484e
        if (dar < k0) dar = -dar;
        float dur = (u[1] * r[1] + u[0] * r[0]) + u[2] * r[2];               // 0x004c486b..0x004c4887
        if (dur < k0) dur = -dur;
        if (dau < dar) {                                                       // 0x004c4898 fcomp, jp when >=
            if (dau < dur) { p4 = u; p5 = a; p6 = r; }                         // 0x004c48b2 jnp -> 0x004c48ef
            else           { p4 = r; p5 = u; p6 = a; }                         // 0x004c48b4
        } else {
            if (dar < dur) { p4 = a; p5 = r; p6 = u; }                         // 0x004c48cf
            else           { p4 = r; p5 = u; p6 = a; }                         // 0x004c48cd jp -> 0x004c48b4
        }
    }
    p6[0] = p5[2] * p4[1] - p5[1] * p4[2];                                     // 0x004c48fb..0x004c490a
    p6[1] = p4[2] * p5[0] - p5[2] * p4[0];                                     // 0x004c490c..0x004c4918
    p6[2] = p5[1] * p4[0] - p4[1] * p5[0];                                     // 0x004c491b..0x004c4927
    const float i6 = LutInvSqrt((p6[0] * p6[0] + p6[1] * p6[1]) + p6[2] * p6[2]);   // 0x004c492a..0x004c4948
    p6[0] = i6 * p6[0]; p6[1] = p6[1] * i6; p6[2] = p6[2] * i6;               // 0x004c494d..0x004c4960
    p5[0] = p4[2] * p6[1] - p4[1] * p6[2];                                     // 0x004c4963..0x004c496f
    p5[1] = p6[2] * p4[0] - p4[2] * p6[0];                                     // 0x004c4977..0x004c4983
    p5[2] = p4[1] * p6[0] - p6[1] * p4[0];                                     // 0x004c4986..0x004c4992
    const float i5 = LutInvSqrt((p5[0] * p5[0] + p5[1] * p5[1]) + p5[2] * p5[2]);   // 0x004c4995..0x004c49b3
    p5[0] = i5 * p5[0]; p5[1] = p5[1] * i5; p5[2] = p5[2] * i5;               // 0x004c49b8..0x004c49d5
    dst[0] = r[0]; dst[1] = r[1]; dst[2] = r[2];
    dst[4] = u[0]; dst[5] = u[1]; dst[6] = u[2];
    dst[8] = a[0]; dst[9] = a[1]; dst[10] = a[2];
    dst[12] = px; dst[13] = py; dst[14] = pz;
    Flags(dst) = (Flags(dst) & 0xfffdffffu) | 3u;                              // 0x004c4a35..0x004c4a41
}

// ---- RtAnim / RpHAnim (std keyframe scheme 0x00533785: sizes 0x24/0x24, apply 0x00532a60, interpolate 0x00532b80) ----

struct KF { KF* prev; float time; float q[4]; float t[3]; };      // RpHAnimStdKeyFrame, 36 bytes
struct IF { KF* kf1; KF* kf2; float q[4]; float t[3]; };          // interpolated frame, 36 bytes (interp + 0x4c)
static_assert(sizeof(KF) == 36 && sizeof(IF) == 36, "RW std keyframe layout");

struct Anim { int numFrames = 0; float duration = 0.f; std::vector<KF> frames; };
struct Obj;
struct Interp {
    Anim* anim = nullptr;      // +0x00
    float time = 0.f;          // +0x04
    KF*   next = nullptr;      // +0x08 pNextFrame
    int (*cb)(Interp*, void*) = nullptr; void* cbData = nullptr;      // +0x0c / +0x10
    float cbTime = -1.0f;                                              // +0x14 (captured 0xbf800000)
    int (*loopCb)(Interp*, Obj*) = nullptr; Obj* loopData = nullptr;  // +0x18 / +0x1c
    int numNodes = 0;                                                  // +0x2c
    IF  f[4];                                                          // +0x4c
};

// 0x00532b80 RpHAnimStdKeyFrameInterpolate(out, a, b, time, custom): disasm_slerp.txt. b's quaternion is negated IN PLACE
// when the dot is negative (0x00532bfd..0x00532c1c).
void KfInterpolate(IF* out, KF* a, KF* b, float time) {
    float dot = ((b->q[0] * a->q[0] + a->q[1] * b->q[1]) + b->q[2] * a->q[2]) + a->q[3] * b->q[3];   // 0x00532b8f..ab
    const float u = (time - a->time) / (b->time - a->time);                                          // 0x00532bad..bbc
    out->t[0] = (b->t[0] - a->t[0]) * u + a->t[0];                                                   // 0x00532bc0..cd
    out->t[1] = (b->t[1] - a->t[1]) * u + a->t[1];
    out->t[2] = (b->t[2] - a->t[2]) * u + a->t[2];
    if (dot < k0) {                                                                                  // 0x00532bf0 jp skip
        dot = -dot;
        b->q[0] = -b->q[0]; b->q[1] = -b->q[1]; b->q[2] = -b->q[2]; b->q[3] = -b->q[3];
    }
    float wa = k1 - u;                                                                               // 0x00532c1f..c29
    float wb = u;
    if (dot < kDot) {                                                                                // 0x00532c2d..c3a
        const std::uint32_t bits = BF(dot), ab = bits & 0x7fffffffu;
        float th;
        if (ab >= 0x3f800000u) {
            th = (static_cast<std::int32_t>(bits) > 0) ? k0 : kPi;                                   // 0x00532c56..c6d
        } else if (ab < 0x3f000000u) {
            if (ab <= 0x23000000u) th = kPio2;                                                       // 0x00532c84
            else {                                                                                   // 0x00532c91..d01
                const float z = dot * dot;
                const float p = ((((((kP5 * z + kP4) * z - kP3) * z + kP2) * z - kP1) * z + kS0) * z);
                const float q = (((z * kQ4 - kQ3) * z + kQ2) * z - kQ1) * z + k1;
                const float r = (p / q) * dot;
                const float w = dot - (kC8 - r);
                th = kC4 - w;
            }
        } else if (static_cast<std::int32_t>(bits) < 0) {                                            // 0x00532d10..dab
            const float zz = (dot + k1) * kHalf;
            const float s = Math::LutSqrt(zz);
            const float p = ((((((zz * kP5 + kP4) * zz - kP3) * zz + kP2) * zz - kP1) * zz + kS0) * zz);
            const float q = (((zz * kQ4 - kQ3) * zz + kQ2) * zz - kQ1) * zz + k1;
            const float r = (p / q) * s;
            const float w = s + (r - kC8);
            th = kC0 - (w + w);
        } else {                                                                                     // 0x00532db0..e6f
            const float zz = (k1 - dot) * kHalf;
            const float s = Math::LutSqrt(zz);
            const float df = FB(BF(s) & 0xfffff000u);
            const float p = ((((((zz * kP5 + kP4) * zz - kP3) * zz + kP2) * zz - kP1) * zz + kS0) * zz);
            const float q = (((zz * kQ4 - kQ3) * zz + kQ2) * zz - kQ1) * zz + k1;
            const float r = (p / q) * s;
            const float c = (zz - df * df) / (df + s);
            const float w = (r + c) + df;
            th = w + w;
        }
        auto sinp = [](float x) {                                                                    // 0x00532e71..ea9
            const float x2 = x * x;
            const float poly = ((((kS5 * x2 - kS4) * x2 + kS3) * x2 - kS2) * x2 + kS1) * x2 - kS0;
            return poly * (x2 * x) + x;
        };
        const float inv = k1 / sinp(th);                                                             // 0x00532eab fdivr
        wa = sinp(th * wa) * inv;                                                                    // 0x00532eb1..ef3
        wb = sinp(th * u) * inv;                                                                     // 0x00532ef9..f3b
    }
    out->q[0] = wb * b->q[0] + wa * a->q[0];                                                         // 0x00532f43..53
    out->q[1] = wa * a->q[1] + wb * b->q[1];
    out->q[2] = wb * b->q[2] + wa * a->q[2];
    out->q[3] = wa * a->q[3] + wb * b->q[3];
}

// 0x0052de60 RtAnimInterpolatorSetCurrentAnim (disasm_rtanim.txt): time = 0, interpolate each node at 0 between keys
// i and numNodes + i, then link them, pNextFrame = frames + 2 * numNodes.
void SetCurrentAnim(Interp* ip, Anim* an) {
    ip->anim = an;
    ip->time = 0.f;                                                         // 0x0052de70 (integer store)
    KF* fr = an->frames.data();
    const int n = ip->numNodes;
    for (int i = 0; i < n; ++i) KfInterpolate(&ip->f[i], &fr[i], &fr[n + i], 0.f);   // 0x0052dea0..dec5
    for (int i = 0; i < n; ++i) { ip->f[i].kf1 = &fr[i]; ip->f[i].kf2 = &fr[n + i]; }  // 0x0052deea..def9
    ip->next = fr + n * 2;                                                  // 0x0052df0e
}

// 0x0052e0e0 RtAnimInterpolatorAddAnimTime. SetCurrentAnim is called with the new time in ST0 and the time restored from
// it afterwards (0x0052e1a8..0x0052e1b2, 0x0052e1c3..0x0052e1cd): the call resets the keys, not the time.
void AddAnimTime(Interp* ip, float delta) {
    if (!(delta > k0)) return;                                              // 0x0052e0e4..0x0052e0f5
    Anim* an = ip->anim;
    const float dur = an->duration;
    const float cbt = ip->cbTime;
    const int n = ip->numNodes;
    int fire = 0, looped = 0;
    if (ip->time < cbt && !(delta + ip->time < cbt)) fire = 1;              // 0x0052e107..0x0052e14e
    float nt = delta + ip->time;                                            // 0x0052e152..0x0052e15f
    ip->time = nt;
    if (nt > dur) {                                                         // 0x0052e15b fcom, jne when <=
        looped = 1;
        do {
            nt = nt - dur;                                                  // 0x0052e179
            if (!(nt < cbt)) fire = 1;                                      // 0x0052e17f..0x0052e19b
        } while (nt > dur);                                                 // 0x0052e195..0x0052e1a4
        ip->time = nt;
        SetCurrentAnim(ip, an);
        ip->time = nt;
    }
    if (ip->next == nullptr) {                                              // 0x0052e1bc..0x0052e1cd
        const float keep = ip->time;
        SetCurrentAnim(ip, an);
        ip->time = keep;
    }
    KF* const end = an->frames.data() + an->numFrames;                      // 0x0052e1d3..0x0052e1e0
    int c = 0;
    while (ip->next < end && ip->next->prev->time <= ip->time) {            // 0x0052e1e4..0x0052e1fa
        KF* const prev = ip->next->prev;
        IF* hit;
        do {                                                                // 0x0052e1ff..0x0052e214
            hit = &ip->f[c];
            ++c;
            if (c >= n) c = 0;
        } while (hit->kf2 != prev);
        hit->kf1 = hit->kf2;                                                // 0x0052e216..0x0052e21e
        hit->kf2 = ip->next;
        ip->next = ip->next + 1;                                            // 0x0052e221..0x0052e22b
    }
    for (int i = 0; i < n; ++i) KfInterpolate(&ip->f[i], ip->f[i].kf1, ip->f[i].kf2, ip->time);   // 0x0052e23d..0x0052e25e
    if (fire && ip->cb && ip->cb(ip, ip->cbData) == 0) ip->cb = nullptr;    // 0x0052e260..0x0052e27c
    if (looped && ip->loopCb && ip->loopCb(ip, ip->loopData) == 0) ip->loopCb = nullptr;   // 0x0052e27f..0x0052e2a1
}

void SubAnimTime(Interp* ip, float delta);

// 0x0052df70 RtAnimInterpolatorSubAnimTime.
void SubAnimTime(Interp* ip, float delta) {
    Anim* an = ip->anim;
    const float cbt = ip->cbTime;
    int fire = 0, looped = 0;
    if (ip->time > cbt && !(ip->time - delta > cbt)) fire = 1;               // 0x0052df7e..0x0052dfa5
    const float nt = ip->time - delta;                                      // 0x0052dfa7..0x0052dfb3
    ip->time = nt;
    if (nt < k0) {                                                          // 0x0052dfb6..0x0052dfc1, jp when >= 0
        float t = ip->time;
        do { t = t + an->duration; } while (t < k0);                        // 0x0052dfcd..0x0052dfdc
        ip->time = t;
        SetCurrentAnim(ip, an);                                             // 0x0052dfe7 (time := 0)
        AddAnimTime(ip, t);                                                 // 0x0052dff2
        looped = 1;
        if (ip->time < cbt) fire = 1;                                       // 0x0052dff7..0x0052e01e
    } else {
        for (int i = 0; i < ip->numNodes; ++i) {                            // 0x0052e030..0x0052e078
            IF* f = &ip->f[i];
            while (ip->time < f->kf1->time) { f->kf2 = f->kf1; f->kf1 = f->kf1->prev; }
            KfInterpolate(f, f->kf1, f->kf2, ip->time);
        }
        ip->next = nullptr;                                                 // 0x0052e07b
    }
    if (fire && ip->cb && ip->cb(ip, ip->cbData) == 0) ip->cb = nullptr;    // 0x0052e083..0x0052e09d
    if (looped && ip->loopCb && ip->loopCb(ip, ip->loopData) == 0) ip->loopCb = nullptr;   // 0x0052e0a0..0x0052e0c2
}

// 0x0052e2c0 RtAnimInterpolatorSetCurrentTime: d = t - time (stored float); d < 0 -> Sub(-d), else Add(d).
void SetCurrentTime(Interp* ip, float t) {
    const float d = t - ip->time;
    if (d < k0) SubAnimTime(ip, -d); else AddAnimTime(ip, d);
}

struct Frame { float model[16]; float ltm[16]; Frame* parent = nullptr; int nodeId = -1; };
struct NodeInfo { int id; int index; int flags; Frame* frame; };
struct Hier {
    int flags = 0x3000;          // FUN_004b4760 |= 0x3000 (captured 0x00003000)
    int numNodes = 0;
    alignas(16) float mats[4][16];
    NodeInfo info[4];
    Frame* parentFrame = nullptr;
    Interp* interp = nullptr;
};

// 0x00532a60 (inlined in 0x00533ec0 when the apply callback is 0x00532a60): keyframe quaternion + translation -> matrix.
void KfToMatrix(float* m, const IF* f) {
    const float x = f->q[0], y = f->q[1], z = f->q[2], w = f->q[3];
    const float wx = w * x;
    float s = z * z + y * y;
    m[0] = k1 - (s + s);
    float v = y * x + w * z;  m[1] = v + v;
    v = z * x - w * y;        m[2] = v + v;
    v = y * x - w * z;        m[4] = v + v;
    s = z * z + x * x;        m[5] = k1 - (s + s);
    v = wx + z * y;           m[6] = v + v;
    v = w * y + z * x;        m[8] = v + v;
    v = z * y - wx;           m[9] = v + v;
    s = y * y + x * x;        m[10] = k1 - (s + s);
    m[12] = f->t[0]; m[13] = f->t[1]; m[14] = f->t[2];
    Flags(m) = 3u;
}

// 0x00533ec0 RpHAnimHierarchyUpdateMatrices, the branch flags 0x3000 takes (not NOMATRICES 2, not SUBHIERARCHY 1, not
// LOCALSPACEMATRICES 0x4000): world matrices, frame modelling := keyframe matrix (0x1000), frame LTM := world (0x2000).
void UpdateMatrices(Hier* h) {
    alignas(16) float parent[16];
    const Frame* pf = h->parentFrame ? h->parentFrame->parent : nullptr;
    if (!pf) {                                                              // 0x0053440b..: identity, flags |= 0x20003
        std::memset(parent, 0, sizeof parent);
        parent[0] = parent[5] = parent[10] = k1;
        Flags(parent) = 0x20003u;
    } else {
        std::memcpy(parent, pf->ltm, sizeof parent);                        // FUN_004c0ed0 (frame LTM)
    }
    const float* cur = parent;
    const float* stack[8];
    int sp = 0;
    for (int i = 0; i < h->numNodes; ++i) {
        alignas(16) float kfm[16];
        std::memset(kfm, 0, sizeof kfm);
        KfToMatrix(kfm, &h->interp->f[i]);
        MatMul(h->mats[i], kfm, cur);                                       // FUN_004c4600(mats[i], kf, parent)
        if (Frame* fr = h->info[i].frame) {
            std::memcpy(fr->model, kfm, 64);                                // 0x1000
            std::memcpy(fr->ltm, h->mats[i], 64);                           // 0x2000, not 0x4000
        }
        const int fl = h->info[i].flags & 3;
        if (fl == 0) cur = h->mats[i];
        else if (fl == 1) cur = (sp > 0) ? stack[--sp] : cur;               // pop (the original reads its stack slot)
        else if (fl == 2) { if (sp < 8) stack[sp++] = cur; cur = h->mats[i]; }
    }
}

// ---- the object -----------------------------------------------------------------------------------------------

// The original's 0x00684d18 record (+0x00..+0xcc), at the original's address.
struct Obj {
    void*  clump;        // +0x00
    void*  atom[3];      // +0x04 / +0x08 / +0x0c (RpClumpForAllAtomics order: stream atomics 2, 1, 0)
    void*  frame10;      // +0x10 frame of +0x08
    float  box[6];       // +0x14 sup x,y,z ; +0x20 inf x,y,z
    std::uint32_t pad2c; // +0x2c
    std::uint32_t pad30[2];
    float  pos[3];       // +0x38 node-1 position
    float  d44;          // +0x44
    std::int32_t state;  // +0x48
    void*  anim;         // +0x4c
    Hier*  hier;         // +0x50
    float  m54[16];      // +0x54 previous node-1 matrix
    float* m94;          // +0x94 -> node-1 world matrix
    std::uint32_t pad98[3];
    float  a4;           // +0xa4
    float  mov[3];       // +0xa8 movement
    std::int32_t b4[4];  // +0xb4 inside last call, per car
    void (*cb)(Obj*, int);   // +0xc4
    float  c8;           // +0xc8
};
static_assert(offsetof(Obj, box) == 0x14 && offsetof(Obj, pos) == 0x38 && offsetof(Obj, state) == 0x48 &&
              offsetof(Obj, m54) == 0x54 && offsetof(Obj, m94) == 0x94 && offsetof(Obj, a4) == 0xa4 &&
              offsetof(Obj, b4) == 0xb4 && offsetof(Obj, cb) == 0xc4 && offsetof(Obj, c8) == 0xc8, "0x00684d18 layout");

Obj&      O()      { return *reinterpret_cast<Obj*>(0x00684d18u); }
std::int32_t* WordI() { return reinterpret_cast<std::int32_t*>(0x0088ff50u); }
float*    TowerM() { return reinterpret_cast<float*>(0x00684cd8u); }
float*    DeckM()  { return reinterpret_cast<float*>(0x00684c98u); }
float*    TowerR() { return reinterpret_cast<float*>(0x00684de8u); }
float*    DeckR()  { return reinterpret_cast<float*>(0x00684c58u); }

struct Atom { int stream; Frame* frame; float sphereR; };
struct State {
    bool active = false;
    PontoonHost host;
    Anim anim;
    std::vector<std::uint8_t> anmBlob;
    Interp interp;
    Hier hier;
    Frame frames[8];
    int nframes = 0;
    Atom atoms[3];       // index = DFF stream atomic
};
State& S() { static State s; return s; }

bool g_replay = false;   // RunReplay: STATE and CARRY live on the replay's scratch cars
bool StateLive() { return g_replay || Pontoon_StateOn(); }
bool CarryLive() { return g_replay || Pontoon_CarryOn(); }

float* CarMat(int v) { return S().host.car_matrix ? S().host.car_matrix(v) : nullptr; }
int CarCount() { return S().host.car_count ? S().host.car_count() : 0; }

// FUN_0046be10(&DAT_005f9788, v, 0, on): record +0xbfc[0] = colour, +0xc7c[0] = on (car v < 0x10).
void Marker(int v, int on) {
    if (!S().host.car_record || v < 0 || v > 0xf) return;
    if (unsigned char* r = S().host.car_record(v)) {
        std::memcpy(r + 0xbfc, &kMarkerColour, 4);
        std::memcpy(r + 0xc7c, &on, 4);
    }
}

// 0x00451990 FUN_00451990(obj, in, out): in * inv(+0x54); inside the box -> out = that * [+0x94], 1; else out = in, 0.
int BoxCarry(Obj& o, const float* in, float* out) {
    alignas(16) float local[16], inv[16];
    std::memcpy(local, in, 64);
    MatInvert(inv, o.m54);                                                  // 0x004519aa FUN_004c4dc0(local_40, obj+0x54)
    CombinePost(local, inv);                                                // 0x004519b8 FUN_004c52f0(local_80, local_40, 2)
    const float x = local[12], z = local[14];
    int r;
    if ((o.box[0] <= x) || (x <= o.box[3]) || (o.box[2] <= z) || (z <= o.box[5])) r = 0;   // 0x004519c0..0x00451a0d
    else { CombinePost(local, o.m94); r = 1; }                              // 0x00451a14 FUN_004c52f0(local_80, [+0x94], 2)
    std::memmove(out, r ? local : in, 64);                                  // 0x00451a27 rep movsd
    return r;
}

// 0x00451820 FUN_00451820: the 0 -> 1 trigger.
void Trigger(Obj& o) {
    const int n = CarCount();
    for (int v = 0; v < n; ++v) {
        const float* m = CarMat(v);
        if (!m) continue;
        alignas(16) float inv[16], local[16];
        MatInvert(inv, o.m94);                                              // FUN_004c4dc0(local_40, [obj+0x94])
        std::memcpy(local, m, 64);
        CombinePost(local, inv);                                            // FUN_004c52f0(local_80, local_40, 2)
        Marker(v, 0);                                                       // FUN_0046be10(&DAT_005f9788, v, 0, 0)
        const float x = local[12], z = local[14];
        if (x < o.box[0] && o.box[3] < x && z < o.box[2] && o.box[5] < z) o.state = 1;
    }
}

// 0x00451ad0 FUN_00451ad0: per car the in-place carry and word; a car that has just left gets the movement.
void Coupling(Obj& o) {
    const int n = CarCount();
    for (int v = 0; v < n; ++v) {
        float* m = CarMat(v);
        int in = 0;
        if (m) {
            alignas(16) float before[16];
            std::memcpy(before, m, 64);
            const bool carry = CarryLive();
            float scratch[16];
            in = BoxCarry(o, m, carry ? m : scratch);                       // 0x00451b26 FUN_00451990(obj, car, car)
            if (in && carry && S().host.car_carried) S().host.car_carried(v, m);
        }
        if (in) {
            Marker(v, 0);                                                   // 0x00451b42
            WordI()[v] = 1;                                                 // 0x00451b47
        } else {
            WordI()[v] = 0;                                                 // 0x00451b5b
            if (o.b4[v] != 0) {                                             // 0x00451b66
                if (CarryLive() && S().host.car_record) {             // 0x00451b9d FUN_0046bf50(obj+0xa8, v)
                    unsigned char* r = S().host.car_record(v);
                    std::int32_t rwp = 0;
                    if (r) std::memcpy(&rwp, r + 0x14, 4);
                    if (r && rwp == 0) {                                     // the record branch (+0x14 == 0)
                        float* vel = reinterpret_cast<float*>(r + 0x9b0);
                        vel[0] = vel[0] + o.mov[0];
                        vel[1] = o.mov[1] + vel[1];
                        vel[2] = o.mov[2] + vel[2];
                    }
                    // [UNCERTAIN U-9738] +0x14 != 0 (FUN_00481780), the wheel impulses FUN_00481750 and the hand-off
                    // FUN_00480720 need the RWP car body the exe does not have.
                }
            }
            Marker(v, 1);                                                   // 0x00451c95
        }
        o.b4[v] = in;                                                       // 0x00451c9e
    }
}

// 0x00451ce0 FUN_00451ce0: sample node 1.
void Sample(Obj& o) {
    float* n1 = o.hier->mats[1];                                            // FUN_00550bc0(hier) + 0x40
    o.pos[0] = n1[12]; o.pos[1] = n1[13]; o.pos[2] = n1[14];
    o.m94 = n1;
    o.mov[0] = n1[12] - o.m54[12];
    o.mov[1] = n1[13] - o.m54[13];
    o.mov[2] = n1[14] - o.m54[14];
    o.mov[0] = o.mov[0] * k360; o.mov[1] = o.mov[1] * k360; o.mov[2] = o.mov[2] * k360;
}

// 0x00452170: the reset callback FUN_00451680 installs at +0xc4.
void ResetCb(Obj*, int) {
    std::memcpy(TowerM(), TowerR(), 64);                                    // 0x684de8 -> 0x684cd8
    std::memcpy(DeckM(), DeckR(), 64);                                      // 0x684c58 -> 0x684c98
    // FUN_0047d180(Tower / Deck, rest): RWP body matrices, none in the exe.
}

// 0x00451cc0 FUN_00451cc0.
void Deactivate(Obj& o) {
    o.state = 0;
    if (o.cb) o.cb(&o, 0);
}

// 0x00451690: the anim loop callback (SubAnimTime by +0x44, update, state 5).
int LoopCb(Interp* ip, Obj* o) {
    SubAnimTime(ip, o->d44);
    UpdateMatrices(o->hier);
    o->state = 5;
    return 1;                                                               // returns param_1 (non-zero)
}

inline float UnitsF(int units) {
    float f = static_cast<float>(units);
    if (units < 0) f = f + k2p32;
    return f;
}

void CopyM94(Obj& o) { std::memcpy(o.m54, o.m94, 64); }

// 0x00451d80 FUN_00451d80.
void StateMachine(Obj& o, int units) {
    if (o.anim == nullptr) return;                                          // +0x4c gate
    Interp* ip = o.hier->interp;
    switch (o.state) {
    case 0:
        SetCurrentTime(ip, 0.f);
        o.c8 = 0.f;
        UpdateMatrices(o.hier);
        Sample(o);
        CopyM94(o);
        if (StateLive()) Trigger(o);                                        // FUN_00451820
        o.a4 = k1;
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        break;
    case 1: {
        const float a = o.a4 - UnitsF(units) * k388;
        o.a4 = a;
        if (a < k0) o.state = 2;
        CopyM94(o);
        o.d44 = 0.f;
        SetCurrentTime(ip, 0.f);
        o.c8 = 0.f;
        UpdateMatrices(o.hier);
        Sample(o);
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        break;
    }
    case 2: {
        CopyM94(o);
        const float d = UnitsF(units) * k384;
        o.c8 = d + o.c8;
        o.d44 = d;
        AddAnimTime(ip, d);                                                 // FUN_004b47c0
        UpdateMatrices(o.hier);
        Sample(o);
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        const std::int32_t* w = WordI();
        if (w[0] == 0 && w[1] == 0 && w[2] == 0 && w[3] == 0 && k2 < o.c8) o.state = 3;
        break;
    }
    case 3: {
        CopyM94(o);
        const float d = UnitsF(units) * k384;
        o.d44 = d;
        o.c8 = o.c8 - d;
        SubAnimTime(ip, d);                                                 // FUN_0052df70
        if (o.c8 <= k0) {
            Deactivate(o);
            o.c8 = 0.f;
            SetCurrentTime(ip, 0.f);
        }
        UpdateMatrices(o.hier);
        Sample(o);
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        break;
    }
    case 4: {
        CopyM94(o);
        const float d = UnitsF(units) * k0;
        o.d44 = d;
        AddAnimTime(ip, d);
        UpdateMatrices(o.hier);
        Sample(o);
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        break;
    }
    case 5: {
        // sphere = node-1 position, radius = atomic +0x08's (stream atomic 1) bounding sphere (FUN_004e5fc0 refresh)
        const float sph[4] = { o.pos[0], o.pos[1], o.pos[2], S().atoms[1].sphereR };
        CopyM94(o);
        const int vis = S().host.sphere_visible ? S().host.sphere_visible(sph) : 0;
        if (vis == 0) { Deactivate(o); o.c8 = 0.f; }
        if (StateLive()) Coupling(o);                                       // FUN_00451ad0
        break;
    }
    default:
        break;
    }
    *reinterpret_cast<float*>(0x007f0fb4u) = o.c8;                          // write-only in the original
}

// 0x00451a50 FUN_00451a50(obj, in, out): out = ortho(in * inv(+0x54) * [+0x94]).
void TrackBody(Obj& o, float* m) {
    alignas(16) float t[16], inv[16];
    std::memcpy(t, m, 64);
    MatInvert(inv, o.m54);
    CombinePost(t, inv);
    CombinePost(t, o.m94);
    OrthoNormalize(t, t);
    std::memcpy(m, t, 64);
}

// ---- parsing ---------------------------------------------------------------------------------------------------

std::uint32_t RU(const std::uint8_t* p) { std::uint32_t u; std::memcpy(&u, p, 4); return u; }
float RF(const std::uint8_t* p) { float f; std::memcpy(&f, p, 4); return f; }

bool ParseAnm(const std::uint8_t* d, std::size_t len, Anim* an) {
    if (len < 32 || RU(d) != 0x1Bu || RU(d + 16) != 1u) return false;      // rwID_HANIMANIMATION, std scheme
    const std::uint32_t n = RU(d + 20);
    if (32 + std::size_t(n) * 36 > len) return false;
    an->numFrames = static_cast<int>(n);
    an->duration = RF(d + 28);
    an->frames.assign(n, KF{});
    for (std::uint32_t i = 0; i < n; ++i) {
        const std::uint8_t* k = d + 32 + i * 36;
        KF& f = an->frames[i];
        f.time = RF(k);
        for (int j = 0; j < 4; ++j) f.q[j] = RF(k + 4 + 4 * j);
        for (int j = 0; j < 3; ++j) f.t[j] = RF(k + 20 + 4 * j);
        const std::int32_t off = static_cast<std::int32_t>(RU(k + 32));
        f.prev = (off >= 0 && off % 36 == 0 && off / 36 < static_cast<std::int32_t>(i)) ? &an->frames[off / 36] : nullptr;
    }
    return true;
}

struct Chunk { std::uint32_t type; const std::uint8_t* p; std::uint32_t size; };
bool NextChunk(const std::uint8_t*& c, const std::uint8_t* end, Chunk* out) {
    if (c + 12 > end) return false;
    out->type = RU(c); out->size = RU(c + 4); out->p = c + 12;
    if (out->p + out->size > end) return false;
    c = out->p + out->size;
    return true;
}

// BOAT.DFF: frame list (+ HAnim PLG 0x11E per frame), geometries (vertex bbox of the box geometry, morph sphere radius),
// atomics (frame, geometry). verify/pontoon_20261016/dump_boat.py prints the same structure.
bool ParseBoat(const std::uint8_t* d, std::size_t len, State& s, float box[6]) {
    const std::uint8_t* c = d; Chunk clump;
    if (!NextChunk(c, d + len, &clump) || clump.type != 0x10u) return false;
    std::vector<std::vector<float>> geomVerts; std::vector<float> geomR;
    int hierFrame = -1; std::vector<NodeInfo> nodes; int nodeIds[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    struct A { int frame, geom; }; std::vector<A> atoms;
    const std::uint8_t* q = clump.p; const std::uint8_t* qe = clump.p + clump.size; Chunk ch;
    while (NextChunk(q, qe, &ch)) {
        if (ch.type == 0x0Eu) {                                             // frame list
            const std::uint8_t* f = ch.p; const std::uint8_t* fe = ch.p + ch.size; Chunk st;
            if (!NextChunk(f, fe, &st)) return false;
            const int n = static_cast<int>(RU(st.p));
            if (n <= 0 || n > 8) return false;
            s.nframes = n;
            for (int i = 0; i < n; ++i) {
                const std::uint8_t* r = st.p + 4 + i * 56;
                Frame& fr = s.frames[i];
                std::memset(fr.model, 0, 64);
                for (int k = 0; k < 9; ++k) fr.model[(k / 3) * 4 + (k % 3)] = RF(r + 4 * k);
                fr.model[12] = RF(r + 36); fr.model[13] = RF(r + 40); fr.model[14] = RF(r + 44);
                const std::int32_t par = static_cast<std::int32_t>(RU(r + 48));
                fr.parent = (par >= 0 && par < i) ? &s.frames[par] : nullptr;
            }
            for (int i = 0; i < n; ++i) {
                Chunk ext;
                if (!NextChunk(f, fe, &ext)) break;
                const std::uint8_t* e = ext.p; Chunk ec;
                while (NextChunk(e, ext.p + ext.size, &ec)) {
                    if (ec.type != 0x11Eu || ec.size < 12) continue;
                    nodeIds[i] = static_cast<std::int32_t>(RU(ec.p + 4));
                    const int nn = static_cast<int>(RU(ec.p + 8));
                    if (nn > 0) {
                        if (nn > 4 || ec.size < 20u + 12u * nn) return false;
                        hierFrame = i;
                        for (int k = 0; k < nn; ++k) {
                            const std::uint8_t* ni = ec.p + 20 + 12 * k;
                            nodes.push_back(NodeInfo{ static_cast<int>(RU(ni)), static_cast<int>(RU(ni + 4)),
                                                      static_cast<int>(RU(ni + 8)), nullptr });
                        }
                    }
                }
            }
        } else if (ch.type == 0x1Au) {                                      // geometry list
            const std::uint8_t* g = ch.p; const std::uint8_t* ge = ch.p + ch.size; Chunk gc;
            while (NextChunk(g, ge, &gc)) {
                if (gc.type != 0x0Fu) continue;
                const std::uint8_t* h = gc.p; Chunk gs;
                if (!NextChunk(h, gc.p + gc.size, &gs)) return false;
                const std::uint8_t* o = gs.p;
                const std::uint32_t fmt = RU(o); const int nt = static_cast<int>(RU(o + 4)), nv = static_cast<int>(RU(o + 8));
                o += 16;
                if (fmt & 0x08u) o += 4 * nv;
                o += 8 * nv * ((fmt >> 16) & 0xffu);
                o += 8 * nt;
                geomR.push_back(RF(o + 12));                                // morph target 0 bounding sphere radius
                o += 24;
                std::vector<float> vs(static_cast<std::size_t>(nv) * 3);
                std::memcpy(vs.data(), o, vs.size() * 4);
                geomVerts.push_back(std::move(vs));
            }
        } else if (ch.type == 0x14u) {                                      // atomic
            const std::uint8_t* a = ch.p; Chunk as;
            if (!NextChunk(a, ch.p + ch.size, &as)) return false;
            atoms.push_back(A{ static_cast<int>(RU(as.p)), static_cast<int>(RU(as.p + 4)) });
        }
    }
    if (hierFrame < 0 || nodes.size() != 4 || atoms.size() != 3) return false;
    // frame LTMs at rest (modelling chain); the hierarchy update rewrites the node frames' LTMs
    for (int i = 0; i < s.nframes; ++i) {
        Frame& fr = s.frames[i];
        fr.nodeId = nodeIds[i];
        const bool ident = fr.model[0] == k1 && fr.model[5] == k1 && fr.model[10] == k1 && fr.model[1] == 0.f &&
                           fr.model[2] == 0.f && fr.model[4] == 0.f && fr.model[6] == 0.f && fr.model[8] == 0.f &&
                           fr.model[9] == 0.f && fr.model[12] == 0.f && fr.model[13] == 0.f && fr.model[14] == 0.f;
        // The streamed frames' flag words are not captured; only frame 0's LTM is read (the hierarchy root's parent,
        // 0x00533ec0), and with it identity the node-0 product equals the keyframe matrix for either flag value (the
        // replay's node matrices are bit-identical, verify/pontoon_20261016/RESULT_pontoon.md).
        Flags(fr.model) = ident ? 0x20003u : 3u;
        if (!fr.parent) std::memcpy(fr.ltm, fr.model, 64);
        else MatMul(fr.ltm, fr.model, fr.parent->ltm);
    }
    s.hier.numNodes = 4;
    s.hier.parentFrame = &s.frames[hierFrame];
    for (int k = 0; k < 4; ++k) {                                           // RpHAnimHierarchyAttach: node id -> frame
        s.hier.info[k] = nodes[k];
        for (int i = 0; i < s.nframes; ++i)
            if (s.frames[i].nodeId == nodes[k].id) s.hier.info[k].frame = &s.frames[i];
    }
    for (int k = 0; k < 3; ++k) {
        const A& a = atoms[k];
        if (a.frame < 0 || a.frame >= s.nframes || a.geom < 0 || a.geom >= static_cast<int>(geomVerts.size())) return false;
        s.atoms[k] = Atom{ k, &s.frames[a.frame], geomR[a.geom] };
    }
    // FUN_00451730 -> FUN_004c1740 RwBBoxCalculate over stream atomic 1's geometry (obj +0x08 = list position 1)
    const std::vector<float>& bv = geomVerts[atoms[1].geom];
    if (bv.size() < 3) return false;
    box[0] = box[3] = bv[0]; box[1] = box[4] = bv[1]; box[2] = box[5] = bv[2];
    for (std::size_t i = 3; i + 2 < bv.size(); i += 3)
        for (int k = 0; k < 3; ++k) {
            if (bv[i + k] > box[k]) box[k] = bv[i + k];
            if (bv[i + k] < box[3 + k]) box[3 + k] = bv[i + k];
        }
    return true;
}

struct Pc24 {
    unsigned int saved = 0;
    Pc24() { unsigned int t = 0; _controlfp_s(&saved, 0, 0); _controlfp_s(&t, _PC_24, _MCW_PC); }
    ~Pc24() { unsigned int t = 0; _controlfp_s(&t, saved & _MCW_PC, _MCW_PC); }
};

std::FILE* Trace() {
    static bool tried = false; static std::FILE* f = nullptr;
    if (!tried) { tried = true; if (const char* p = std::getenv("MASHED_PONTOON_TRACE")) f = std::fopen(p, "a"); }
    return f;
}

bool Env(const char* n) { const char* e = std::getenv(n); return e && e[0] && e[0] != '0'; }

// The dff/anm blobs kept for a clean re-init after a replay.
std::vector<std::uint8_t> g_dff, g_anm; float g_tower[16], g_deck[16];

bool InitFrom(const std::uint8_t* dff, std::size_t dlen, const std::uint8_t* anm, std::size_t alen,
              const float tower[16], const float deck[16]);
void RunReplay(const char* path);

}  // namespace

// [dflip 2026-10-17] default ON; an opt-out also turns off every knob that implies it.
namespace {
bool NoAnim()  { return Env("MASHED_NO_PONTOON_ANIM"); }
bool NoState() { return NoAnim() || Env("MASHED_NO_PONTOON_STATE"); }
bool NoCarry() { return NoState() || Env("MASHED_NO_PONTOON_CARRY"); }
}  // namespace
bool Pontoon_CarryOn() { static const bool on = Env("MASHED_PONTOON_CARRY") || !NoCarry(); return on; }
bool Pontoon_StateOn() { static const bool on = Pontoon_CarryOn() || Env("MASHED_PONTOON_STATE") || !NoState(); return on; }
bool Pontoon_AnimOn()  { static const bool on = Pontoon_StateOn() || Env("MASHED_PONTOON_ANIM") || !NoAnim(); return on; }

void Pontoon_SetHost(const PontoonHost& h) { S().host = h; }
bool Pontoon_Active() { return S().active; }

namespace {

bool InitFrom(const std::uint8_t* dff, std::size_t dlen, const std::uint8_t* anm, std::size_t alen,
              const float tower[16], const float deck[16]) {
    State& s = S();
    const PontoonHost keepHost = s.host;
    s.~State(); new (&s) State(); s.host = keepHost;
    float box[6];
    if (!ParseAnm(anm, alen, &s.anim) || !ParseBoat(dff, dlen, s, box)) return false;
    Obj& o = O();
    std::memset(&o, 0, sizeof(Obj));
    // 0x00451760 FUN_00451760(&obj, clump, anim)
    o.clump = &s;                                                           // +0x00 (identity only)
    o.anim = &s.anim;                                                       // +0x4c
    o.d44 = 0.f;
    s.interp.numNodes = 4;
    s.hier.interp = &s.interp;
    o.hier = &s.hier;                                                       // +0x50 FUN_004b4790: attach, SetCurrentAnim, update
    SetCurrentAnim(&s.interp, &s.anim);
    UpdateMatrices(&s.hier);
    o.state = 0;
    s.interp.loopCb = &LoopCb; s.interp.loopData = &o;                      // FUN_0052df20(interp, 0x00451690, obj)
    o.atom[0] = &s.atoms[2]; o.atom[1] = &s.atoms[1]; o.atom[2] = &s.atoms[0];
    o.frame10 = s.atoms[1].frame;
    std::memcpy(o.box, box, sizeof box);                                    // FUN_00451730
    SetCurrentTime(&s.interp, 0.f);                                         // FUN_0052e2c0(interp, 0)
    UpdateMatrices(&s.hier);
    float* n1 = s.hier.mats[1];
    o.pos[0] = n1[12]; o.pos[1] = n1[13]; o.pos[2] = n1[14];
    o.m94 = n1;
    std::memcpy(o.m54, n1, 64);
    o.cb = nullptr;
    o.cb = &ResetCb;                                                        // slot 0: FUN_00451680(&obj, 0x00452170)
    std::memcpy(TowerR(), tower, 64);                                       // slot 0: 0x684de8 := Tower body matrix
    std::memcpy(DeckR(), deck, 64);                                         // slot 0: 0x684c58 := Deck body matrix
    for (int v = 0; v < 4; ++v) WordI()[v] = 0;
    s.active = true;
    return true;
}

}  // namespace

bool Pontoon_Init(const std::uint8_t* dff, std::size_t dlen, const std::uint8_t* anm, std::size_t alen,
                  const float tower[16], const float deck[16]) {
    if (!Pontoon_AnimOn()) return false;
    Pc24 pc;
    g_dff.assign(dff, dff + dlen); g_anm.assign(anm, anm + alen);
    std::memcpy(g_tower, tower, 64); std::memcpy(g_deck, deck, 64);
    const bool ok = InitFrom(g_dff.data(), g_dff.size(), g_anm.data(), g_anm.size(), g_tower, g_deck);
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "pontoon init ok=%d box %08x %08x %08x %08x %08x %08x anim frames %d dur %08x state=%d carry=%d\n",
                     ok ? 1 : 0, BF(O().box[0]), BF(O().box[1]), BF(O().box[2]), BF(O().box[3]), BF(O().box[4]),
                     BF(O().box[5]), S().anim.numFrames, BF(S().anim.duration), Pontoon_StateOn() ? 1 : 0,
                     Pontoon_CarryOn() ? 1 : 0);
        std::fflush(f);
    }
    if (ok) {
        if (const char* rp = std::getenv("MASHED_PONTOON_REPLAY")) {
            RunReplay(rp);
            InitFrom(g_dff.data(), g_dff.size(), g_anm.data(), g_anm.size(), g_tower, g_deck);   // clean state again
        }
        Pontoon_Reset();
    }
    return ok;
}

void Pontoon_Reset() {
    if (!S().active) return;
    Deactivate(O());                                                        // slots 6 / 7: FUN_00451cc0(&obj)
}

void Pontoon_Tick(int units) {
    if (!S().active) return;
    Pc24 pc;
    Obj& o = O();
    const int s0 = o.state;
    StateMachine(o, units);                                                 // 0x00452ba2
    TrackBody(o, TowerM());                                                 // 0x00452bab FUN_00451a50(&obj, 0x684cd8, ..)
    TrackBody(o, DeckM());                                                  // 0x00452c03 FUN_00451a50(&obj, 0x684c98, ..)
    if (std::FILE* f = Trace()) {
        static long n = 0;
        ++n;
        const std::int32_t* w = WordI();
        if (s0 != o.state || o.state != 0 || w[0] || w[1] || w[2] || w[3] || n % 600 == 1) {
            std::fprintf(f, "pontoon n=%ld units=%d state %d->%d a4=%08x c8=%08x ff50=%d%d%d%d b4=%d%d%d%d pos=%08x %08x %08x\n",
                         n, units, s0, o.state, BF(o.a4), BF(o.c8), w[0], w[1], w[2], w[3], o.b4[0], o.b4[1], o.b4[2],
                         o.b4[3], BF(o.pos[0]), BF(o.pos[1]), BF(o.pos[2]));
            std::fflush(f);
        }
    }
}

bool Pontoon_AtomicLtm(int stream_atomic, float out[16]) {
    if (!S().active || stream_atomic < 0 || stream_atomic > 2) return false;
    std::memcpy(out, S().atoms[stream_atomic].frame->ltm, 64);
    return true;
}

void Pontoon_Shutdown() {
    if (!S().active) return;
    S().active = false;
    std::memset(&O(), 0, sizeof(Obj));
    for (int v = 0; v < 4; ++v) WordI()[v] = 0;
}

// ---- replay (measurement only, PREREG arm R) -------------------------------------------------------------------
// MASHED_PONTOON_REPLAY=<OP.msd.pontoon.csv>, MASHED_PONTOON_REPLAY_OUT=<out.csv>: feeds every recorded call's entry
// car matrices and units to the port (fresh object, rows in file order) and writes, per row, the count of output dwords
// that differ from the recording, per group. Carry is forced on for the replay's car matrices (scratch copies).

namespace {

struct ReplayRow { std::vector<std::uint32_t> e, l; int n = 0, units = 0, count = 0; };
float g_rcars[4][16];
int   g_rcount = 0;
unsigned char g_rrec[4][0xd04];

bool ParseRow(const std::string& ln, ReplayRow* r) {
    std::vector<std::string> t; std::size_t a = 0;
    for (;;) { const std::size_t b = ln.find(',', a); t.push_back(ln.substr(a, b == std::string::npos ? std::string::npos : b - a));
               if (b == std::string::npos) break; a = b + 1; }
    if (t.size() < 6 || t[4] != "E") return false;
    r->n = std::atoi(t[0].c_str()); r->units = std::atoi(t[2].c_str()); r->count = std::atoi(t[3].c_str());
    std::size_t i = 5;
    for (; i < t.size() && t[i] != "L"; ++i) r->e.push_back(static_cast<std::uint32_t>(std::strtoll(t[i].c_str(), nullptr, 10)));
    for (++i; i < t.size(); ++i) r->l.push_back(static_cast<std::uint32_t>(std::strtoll(t[i].c_str(), nullptr, 10)));
    return true;
}

// pn_rows.py layout: obj block 60 dwords, cars 4 x 27, then (entry) tower 16, deck 16 [, rest 32] / (leave) hier 121.
void ObjBlock(const Obj& o, std::uint32_t* b) {
    std::size_t i = 0;
    b[i++] = static_cast<std::uint32_t>(o.state);
    b[i++] = 0;                                                             // +0x10 pointer: not compared
    for (int k = 0; k < 6; ++k) b[i++] = BF(o.box[k]);
    for (int k = 0; k < 3; ++k) b[i++] = BF(o.pos[k]);
    b[i++] = BF(o.d44);
    b[i++] = 0;                                                             // +0x4c pointer
    for (int k = 0; k < 16; ++k) b[i++] = BF(o.m54[k]);
    b[i++] = 0;                                                             // +0x94 pointer
    for (int k = 0; k < 16; ++k) b[i++] = BF(o.m94[k]);
    b[i++] = BF(o.a4);
    for (int k = 0; k < 3; ++k) b[i++] = BF(o.mov[k]);
    for (int k = 0; k < 4; ++k) b[i++] = static_cast<std::uint32_t>(o.b4[k]);
    b[i++] = 0;                                                             // +0xc4 pointer
    b[i++] = BF(o.c8);
    for (int k = 0; k < 4; ++k) b[i++] = static_cast<std::uint32_t>(WordI()[k]);
}

// Compared dword mask inside a 16-dword matrix: the 12 rows' xyz + the flags word (pads are not written by every path).
bool MatDw(int k) { return k != 7 && k != 11 && k != 15; }

void RunReplay(const char* path) {
    std::FILE* in = std::fopen(path, "r");
    const char* op = std::getenv("MASHED_PONTOON_REPLAY_OUT");
    std::FILE* out = op ? std::fopen(op, "w") : nullptr;
    if (!in || !out) { if (in) std::fclose(in); if (out) std::fclose(out); return; }
    std::fprintf(out, "n,units,state_in,state_out,obj_diff,car_diff,node_diff,atom_diff,body_diff,first\n");
    PontoonHost saved = S().host, h{};
    h.car_count = [] { return g_rcount; };
    h.car_matrix = [](int v) -> float* { return (v >= 0 && v < 4) ? g_rcars[v] : nullptr; };
    h.car_carried = nullptr;
    h.car_record = [](int v) -> unsigned char* { return (v >= 0 && v < 4) ? g_rrec[v] : nullptr; };
    h.sphere_visible = [](const float*) { return 1; };
    S().host = h;
    g_replay = true;
    std::string ln; static char buf[1 << 16];
    long rows = 0, bad = 0;
    bool first = true;
    int prevN = -2;
    while (std::fgets(buf, sizeof buf, in)) {
        ln.assign(buf);
        while (!ln.empty() && (ln.back() == '\n' || ln.back() == '\r')) ln.pop_back();
        ReplayRow r;
        if (!ParseRow(ln, &r) || r.e.size() < 60 + 108 + 32 || r.l.size() < 60 + 108 + 121) continue;
        Obj& o = O();
        int bodyd = 0;
        if (first) {                                                        // Tower / Deck tracked matrices at the first row
            for (int k = 0; k < 16; ++k) { TowerM()[k] = FB(r.e[168 + k]); DeckM()[k] = FB(r.e[184 + k]); }
            if (r.e.size() >= 232) {                                        // rows n < 8 carry the bodies' rest matrices
                float ortho[16];
                std::memcpy(ortho, TowerR(), 64);
                OrthoNormalize(ortho, ortho);
                int dm = 0, dmo = 0, dd = 0;
                for (int k = 0; k < 16; ++k) {
                    if (!MatDw(k)) continue;
                    if (BF(TowerR()[k]) != r.e[200 + k]) ++dm;
                    if (BF(ortho[k]) != r.e[200 + k]) ++dmo;
                    if (BF(DeckR()[k]) != r.e[216 + k]) ++dd;
                }
                std::fprintf(out, "# rest: tower mts-vs-original %d dwords, ortho(mts)-vs-original %d, deck %d; the replay "
                                  "uses the original's\n", dm, dmo, dd);
                for (int k = 0; k < 16; ++k) { TowerR()[k] = FB(r.e[200 + k]); DeckR()[k] = FB(r.e[216 + k]); }
            }
            first = false;
        } else if (r.n == prevN + 1) {                                      // the previous call's slot-1 result
            for (int k = 0; k < 16; ++k) {
                if (MatDw(k) && BF(TowerM()[k]) != r.e[168 + k]) ++bodyd;
                if (MatDw(k) && BF(DeckM()[k]) != r.e[184 + k]) ++bodyd;
            }
        }
        if (r.n != prevN + 1) {                                             // gap: re-seed the tracked bodies
            for (int k = 0; k < 16; ++k) { TowerM()[k] = FB(r.e[168 + k]); DeckM()[k] = FB(r.e[184 + k]); }
        }
        prevN = r.n;
        g_rcount = r.count;
        for (int v = 0; v < 4; ++v) {
            const std::uint32_t* cb = &r.e[60 + v * 27];
            for (int k = 0; k < 16; ++k) g_rcars[v][k] = FB(cb[1 + k]);
            std::memset(g_rrec[v], 0, sizeof g_rrec[v]);
            std::memcpy(g_rrec[v] + 0x14, &cb[17], 8);                       // +0x14 / +0x18
            for (int k = 0; k < 6; ++k) std::memcpy(g_rrec[v] + 0x9b0 + 4 * k, &cb[19 + k], 4);
            std::memcpy(g_rrec[v] + 0xbfc, &cb[25], 4);
            std::memcpy(g_rrec[v] + 0xc7c, &cb[26], 4);
        }
        // compare entry object state with the recording first (does the port's carried state match before the call?)
        const int s0 = o.state;
        {
            Pc24 pc;
            StateMachine(o, r.units);
            TrackBody(o, TowerM());
            TrackBody(o, DeckM());
        }
        std::uint32_t ob[60];
        ObjBlock(o, ob);
        int objd = 0, card = 0, noded = 0, atomd = 0, firstd = -1;
        auto note = [&](int idx) { if (firstd < 0) firstd = idx; };
        for (int k = 0; k < 60; ++k) {
            if (k == 1 || k == 12 || k == 29 || k == 54) continue;           // pointers
            if (k >= 13 && k < 29 && !MatDw(k - 13)) continue;
            if (k >= 30 && k < 46 && !MatDw(k - 30)) continue;
            if (ob[k] != r.l[k]) { ++objd; note(k); }
        }
        for (int v = 0; v < 4; ++v) {
            const std::uint32_t* cl = &r.l[60 + v * 27];
            for (int k = 0; k < 16; ++k)
                if (MatDw(k) && BF(g_rcars[v][k]) != cl[1 + k]) { ++card; note(1000 + v * 100 + k); }
            for (int k = 0; k < 6; ++k) {
                std::uint32_t u; std::memcpy(&u, g_rrec[v] + 0x9b0 + 4 * k, 4);
                if (u != cl[19 + k] && cl[17] == 0) { ++card; note(1000 + v * 100 + 20 + k); }   // RWP-mode rows not ported
            }
            std::uint32_t mk[2]; std::memcpy(&mk[0], g_rrec[v] + 0xbfc, 4); std::memcpy(&mk[1], g_rrec[v] + 0xc7c, 4);
            if (mk[0] != cl[25] || mk[1] != cl[26]) { ++card; note(1000 + v * 100 + 30); }
        }
        const std::uint32_t* hn = &r.l[168 + 2];
        for (int nd = 0; nd < 4; ++nd)
            for (int k = 0; k < 16; ++k)
                if (MatDw(k) && BF(S().hier.mats[nd][k]) != hn[nd * 16 + k]) { ++noded; note(2000 + nd * 100 + k); }
        {                                                                   // interpolator time and next-key index
            const std::uint32_t* ii = &r.l[168 + 2 + 64];
            if (BF(S().interp.time) != ii[0]) { ++noded; note(2900); }
            const long mine = S().interp.next ? static_cast<long>(S().interp.next - S().anim.frames.data()) : -1;
            const long orig = ii[1] ? static_cast<long>((ii[1] - ii[2]) / 36u) : -1;
            if (mine != orig) { ++noded; note(2901); }
        }
        const std::uint32_t* at = &r.l[168 + 2 + 64 + 4];
        const int streamOf[3] = { 2, 1, 0 };
        for (int a = 0; a < 3; ++a)
            for (int k = 0; k < 16; ++k)
                if (MatDw(k) && BF(S().atoms[streamOf[a]].frame->ltm[k]) != at[a * 17 + 1 + k]) { ++atomd; note(3000 + a * 100 + k); }
        std::fprintf(out, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", r.n, r.units, s0, o.state, objd, card, noded, atomd, bodyd, firstd);
        ++rows; if (objd || card || noded || atomd || bodyd) ++bad;
    }
    std::fprintf(out, "# rows %ld rows_with_diff %ld\n", rows, bad);
    std::fclose(in); std::fclose(out);
    g_replay = false;
    S().host = saved;
}

}  // namespace

}  // namespace Track
}  // namespace mashed_re
