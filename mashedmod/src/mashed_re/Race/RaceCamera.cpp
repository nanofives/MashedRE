// Verbatim port — see RaceCamera.h header block and
// re/analysis/race_camera/race_camera.md for the full evidence trail.
// Every constant cites its .rdata VA in the anchored MASHED.exe.
#include "RaceCamera.h"

#include <cmath>
#include <cfenv>
#include <cstdio>
#include <cstring>

#include "../Piz/PizReader.h"

namespace mashed_re {
namespace Race {

namespace {

// 0x004a2c48: FPU ROUND(st0) to integer, banker's rounding (round-to-
// nearest-even, the x87 default mode the original runs in).
inline int BankersRound(float v) {
    return static_cast<int>(std::nearbyint(v));   // FE_TONEAREST default
}

// 0x004c3ac0 Vec3Magnitude (C4-verified RW primitive; here the plain
// sqrt form — the original's LUT fast-sqrt agrees to float precision on
// this range; the C4 port in Math/Vec3.cpp is the bit-exact reference).
inline float Vec3Mag(const float v[3]) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// 0x004c39b0 RW normalize (in-place capable, dst/src form).
inline void Vec3Norm(float dst[3], const float src[3]) {
    const float m = Vec3Mag(src);
    if (m > 0.f) {
        dst[0] = src[0] / m; dst[1] = src[1] / m; dst[2] = src[2] / m;
    } else {
        dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
    }
}

// 0x004c4d20 RwMatrix_SetRotAxisAngle + 0x004c3df0 transform, fused:
// rotate vector v about (unnormalized) axis by deg degrees.
inline void RotateAboutAxis(float v[3], const float axis_in[3], float deg) {
    float axis[3];
    Vec3Norm(axis, axis_in);
    const float rad = deg * 0.01745329252f;   // pi/180 (0x005cd7a8 in orig)
    const float c = std::cos(rad), s = std::sin(rad), t = 1.f - c;
    const float x = axis[0], y = axis[1], z = axis[2];
    const float m[3][3] = {
        {t * x * x + c,     t * x * y + s * z, t * x * z - s * y},
        {t * x * y - s * z, t * y * y + c,     t * y * z + s * x},
        {t * x * z + s * y, t * y * z - s * x, t * z * z + c},
    };
    const float in[3] = {v[0], v[1], v[2]};
    for (int i = 0; i < 3; ++i)
        v[i] = in[0] * m[0][i] + in[1] * m[1][i] + in[2] * m[2][i];
}

constexpr double kRadToDeg = 57.295799255371094;  // f64 @0x005ccae0

}  // namespace

// ---- LED table (Common/LED.piz LE<id>.LED) --------------------------------
// 12-byte header + 384 × (elev°, azim°, height) f32 triplets, stride 0xC —
// the on-disk image of the override table DAT_0063a5f0 (fed in the original
// through cmd-stream opcodes 0xC..0xF, 0x00409790). Format verified against
// the live table 2026-06-10 (re/analysis/race_camera/race_camera.md).
bool RaceCamera::LoadLed(const char* led_piz_path, int course_id) {
    for (auto& e : led_) e = LedEntry{-1.f, -1.f, -1.f};
    led_loaded_ = false;

    Piz::Archive piz;
    if (!piz.Load(led_piz_path) || !piz.valid()) return false;
    char want[32];
    std::snprintf(want, sizeof(want), "LE%d.LED", course_id);
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (_stricmp(piz.entry(i).name, want) != 0) continue;
        std::uint32_t len = 0;
        const std::uint8_t* b = piz.blob(i, &len);
        if (!b || len < 12) return false;
        const std::uint32_t n_trip = (len - 12) / 12;
        const std::uint32_t n = n_trip < kMaxNodes ? n_trip : kMaxNodes;
        for (std::uint32_t k = 0; k < n; ++k) {
            float f[3];
            std::memcpy(f, b + 12 + k * 12, 12);
            led_[k] = LedEntry{f[0], f[1], f[2]};
        }
        led_loaded_ = true;
        return true;
    }
    return false;
}

void RaceCamera::SetNodes(const RaceCamNode* nodes, int count) {
    nodes_ = nodes;
    node_count_ = count;
    primed_ = false;
}

// ---- 0x00441820: per-node camera direction + height -----------------------
void RaceCamera::NodeDir(int node, float out_dir[3], float* out_h) const {
    out_dir[0] = 0.f; out_dir[1] = -1.f; out_dir[2] = 0.f;  // (0,-1,0) default
    *out_h = 0.f;
    if (node < 0 || node >= node_count_ || !nodes_) return;

    const LedEntry& e = (node < kMaxNodes) ? led_[node]
                                           : LedEntry{-1.f, -1.f, -1.f};
    if (e.height != -1.f) *out_h = e.height;
    if (e.elev != -1.f) {
        // rotate (0,-1,0) by (90 - elev) about X (DAT_006146f0 = (1,0,0)),
        // then by (azim + 180) about Y (DAT_006146fc = (0,1,0)).
        const float xaxis[3] = {1.f, 0.f, 0.f};
        const float yaxis[3] = {0.f, 1.f, 0.f};
        RotateAboutAxis(out_dir, xaxis, 90.f /*0x005ccad0*/ - e.elev);
        RotateAboutAxis(out_dir, yaxis, e.azim + 180.f /*0x005cd09c*/);
        return;
    }
    // fallback: node dir (FUN_00426cc0) tilted -25° (0xc1c80000) about the
    // gate's lateral edge (corner0 - corner3, FUN_00426d00(n,0/3)).
    const RaceCamNode& nd = nodes_[node];
    out_dir[0] = nd.dir[0]; out_dir[1] = nd.dir[1]; out_dir[2] = nd.dir[2];
    const float axis[3] = {nd.c0[0] - nd.c3[0], nd.c0[1] - nd.c3[1],
                           nd.c0[2] - nd.c3[2]};
    RotateAboutAxis(out_dir, axis, -25.f);
}

// path sample at progress: node = round(prog), blend with node+1 (wrap) by
// frac = prog - node (0x00446f91..0x00447081; frac may be negative — the
// original extrapolates and so do we).
void RaceCamera::PathSample(float prog, float out_dir[3], float* out_h) const {
    const int node = BankersRound(prog);                 // 0x004a2c48
    const float frac = prog - static_cast<float>(node);
    int next = node + 1;
    if (node_count_ <= next) next = 0;                   // 0x00446fd5 wrap
    float d0[3], d1[3], h0, h1;
    NodeDir(node, d0, &h0);
    NodeDir(next, d1, &h1);
    const float keep = 1.f /*0x005cc320*/ - frac;
    for (int i = 0; i < 3; ++i) out_dir[i] = d1[i] * frac + d0[i] * keep;
    *out_h = h1 * frac + h0 * keep;
}

// ---- 0x0040e180: most-separated pair (3D distance, active+alive+!dying) ---
void RaceCamera::MostSeparatedPair(const RaceCamCar cars[4], int* a, int* b) {
    int best_outer = -1, best_inner = -1;
    float best = 0.f;
    for (int i = 0; i < 4; ++i) {
        if (!cars[i].active || !cars[i].alive || cars[i].dead_flag) continue;
        for (int j = 0; j < 4; ++j) {
            if (!cars[j].active || !cars[j].alive || cars[j].dead_flag)
                continue;
            const float d[3] = {cars[i].pos[0] - cars[j].pos[0],
                                cars[i].pos[1] - cars[j].pos[1],
                                cars[i].pos[2] - cars[j].pos[2]};
            const float m = Vec3Mag(d);
            if (best <= m) {   // `<=` as in the original (later pair wins ties)
                best = m;
                best_outer = i;
                best_inner = j;
            }
        }
    }
    // tail fixups verbatim (0x0040e2f3..0x0040e330)
    if (best_inner == -1) best_inner = best_outer;
    if (best_outer == -1) best_outer = best_inner;
    if (best_inner == -1) best_inner = 0;
    if (best_outer == -1) best_outer = 0;
    *a = best_outer;
    *b = best_inner;
}

// ---- 0x00446520 race branch ------------------------------------------------
void RaceCamera::Update(const RaceCamCar cars[4], int track_type,
                        float dt_blend, std::uint32_t time_ticks,
                        float jitter_amp, bool force_reset, bool overhead) {
    if (!nodes_ || node_count_ <= 0) return;
    if (!primed_) { force_reset = true; primed_ = true; }

    // pair selection (FUN_0040e180; mode-7/4/8/9 overrides not ported —
    // standard path only, matching live mode 0)
    int A, B;
    MostSeparatedPair(cars, &A, &B);

    // lead position: posA + velA × 0.00015 (0x005ccd18), x/z only
    // (0x00446b30..0x00446b6a)
    float leadA[3] = {cars[A].pos[0], cars[A].pos[1], cars[A].pos[2]};
    const float velLead[3] = {cars[A].vel[0] * 0.00015f,
                              cars[A].vel[1] * 0.00015f,
                              cars[A].vel[2] * 0.00015f};
    leadA[0] += velLead[0];
    leadA[2] += velLead[2];

    // separation = (posA - posB) with y forced 0 (0x00446bf0..0x00446c50);
    // dy kept separately for the midpoint
    const float dy = leadA[1] - cars[B].pos[1];
    float sep[3] = {leadA[0] - velLead[0] - cars[B].pos[0], 0.f,
                    leadA[2] - velLead[2] - cars[B].pos[2]};
    float zoom_pair = Vec3Mag(sep) * 0.8f;            // 0x005cc9bc
    float zoom_pair_raw = zoom_pair;                  // local_30

    // degenerate pair (sole car) while dying → 8.0 (0x00446c70 block)
    if (A == B && zoom_pair < 1.f /*0x005cc320*/ && cars[A].dead_flag) {
        zoom_pair = 8.f;
        zoom_pair_raw = 8.f;
    }

    // midpoint (target base): leadA - sep/2 (0x005cc574 = 2.0)
    float mid[3] = {leadA[0] - sep[0] / 2.f,
                    leadA[1] - dy / 2.f,
                    leadA[2] - sep[2] / 2.f};

    // pair-switch hysteresis (cam[0x265..0x267]; 0x00446cd0..0x00446ec0)
    const bool same_pair = (A == pair_a_ && B == pair_b_) ||
                           (A == pair_b_ && B == pair_a_);
    if (!same_pair && !force_reset) {
        pair_blend_ += dt_blend + dt_blend;           // += 2×DAT_007f100c
        if (pair_blend_ < 1.f /*0x005cc320*/) {
            // old-pair midpoint/zoom (no velocity lead in this block)
            const RaceCamCar& oa = cars[pair_a_ < 0 ? A : pair_a_];
            const RaceCamCar& ob = cars[pair_b_ < 0 ? B : pair_b_];
            float osep[3] = {oa.pos[0] - ob.pos[0], 0.f,
                             oa.pos[2] - ob.pos[2]};
            const float ody = oa.pos[1] - ob.pos[1];
            const float ozoom = Vec3Mag(osep) * 0.8f;
            const float omid[3] = {oa.pos[0] - osep[0] / 2.f,
                                   oa.pos[1] - ody / 2.f,
                                   oa.pos[2] - osep[2] / 2.f};
            float w = pair_blend_ + pair_blend_ - 1.f;   // 2b-1
            if (w < 0.f /*DAT_005d757c*/) w = 0.f;
            const float keep = 1.f - w;
            for (int i = 0; i < 3; ++i) mid[i] = omid[i] * keep + mid[i] * w;
            zoom_pair = ozoom * keep + zoom_pair * w;
        } else {
            pair_a_ = A;
            pair_b_ = B;
        }
    } else {
        pair_blend_ = 0.f;
    }

    // path ribbon: sample at each pair member's progress (0x00446f91..)
    float dirA[3], dirB[3], hA, hB;
    PathSample(cars[A].path_prog, dirA, &hA);
    PathSample(cars[B].path_prog, dirB, &hB);
    float path_zoom = (hA + hB) * 0.5f;               // 0x005cc32c

    float view_dir[3] = {dirA[0] + dirB[0], dirA[1] + dirB[1],
                         dirA[2] + dirB[2]};
    Vec3Norm(view_dir, view_dir);                     // 0x004c39b0

    // last-survivor zoom-out (0x004471f0 block): exactly 1 active+alive and
    // it is dying → path_zoom += dead_ms/1000 (0x005cc9fc), cap 10
    {
        int n_alive = 0, n_dead = 0;
        float dead_ms = 0.f;
        for (int i = 0; i < 4; ++i) {
            if (!cars[i].active || !cars[i].alive) continue;
            ++n_alive;
            if (cars[i].dead_flag) { ++n_dead; dead_ms = cars[i].dead_ms; }
        }
        if (n_alive == 1 && n_dead == 1) {
            float z = dead_ms / 1000.f + path_zoom;
            if (10.f /*0x005cc55c*/ < z) z = 10.f;
            path_zoom = z;
        }
    }

    path_zoom *= 1.1f;                                // 0x005ccabc
    // (modes 8/4 force 4.0 — not ported, standard path)
    const float path_zoom_pre = path_zoom;
    float zoom = path_zoom;                           // local_bc (display)
    if (zoom < zoom_pair) zoom = zoom_pair;
    if (10.f < zoom) zoom = 10.f;
    if (overhead) zoom = 10.f;                        // DAT_007f0f38
    float req = path_zoom_pre;                        // local_b8
    if (req < zoom_pair_raw) req = zoom_pair_raw;
    if (10.f < req) req = 10.f;
    required_zoom_ = req;                             // cam[0x268] = DAT_00898980

    // pitch of view dir (0x004473a1..0x004473ff): vert = dot(dir, Y-up)
    float vert = view_dir[1];
    if (vert < -1.f /*0x005cc33c*/) vert = -1.f;
    if (1.f < vert) vert = 1.f;
    const float ang = static_cast<float>(std::acos(static_cast<double>(vert)) *
                                         kRadToDeg);
    const float dir_elev = ang - 90.f;   // 90-(180-ang) (0x004473f3..f9)

    // camera offset (0x00447402..0x0044741a): scale = -3.95 - zoom*0.25/5.0
    const float scale = static_cast<float>(
        -3.95 /*0x005ce1c0*/ -
        (static_cast<double>(zoom) * 0.25 /*0x005ce1c8*/) / 5.0 /*0x005cd030*/);
    float offset[3] = {view_dir[0] * scale, view_dir[1] * scale,
                       view_dir[2] * scale};
    float offn[3];
    Vec3Norm(offn, offset);                            // 0x0044744a
    // axis = offn × Y-up = (-z, 0, x) (0x0044745c..0x004474a8)
    const float axis[3] = {-offn[2], 0.f, offn[0]};
    float pitch_extra = (50.f /*0x005cd120*/ * zoom) / 10.f /*0x005cc55c*/;
    if (track_type == 0x1a) {                          // City (0x004474e0)
        pitch_extra = (67.5f /*0x005ce1b8*/ * zoom) / 10.f;
        zoom = zoom * 1.25f;                           // 0x005cd074
    }
    pitch_extra -= 5.f;                                // 0x005cc358
    if (89.9f /*0x005ce1b4*/ <= pitch_extra + dir_elev)
        pitch_extra = 89.9f - dir_elev;
    if (overhead) pitch_extra = 90.f;                  // 0x00447536
    RotateAboutAxis(offset, axis, pitch_extra);        // 0x00447557+0x00447570

    // distance law (THE SciLor block, 0x00447578): (zoom/0.8)/5.0 + 1.0
    const float dist = (zoom / 0.8f /*0x005cc9bc*/) / 5.f /*0x005cc358*/ +
                       1.f /*0x005cc320*/;
    float cam_pos[3] = {offset[0] * dist + mid[0],
                        offset[1] * dist + mid[1],
                        offset[2] * dist + mid[2]};
    if (overhead) {                                    // 0x004475d6 block
        cam_pos[0] = mid[0];
        cam_pos[1] = 15.f /*0x005cc9b0*/ + mid[1];
        cam_pos[2] = mid[2];
    }

    // height mix along the horizontal camera-backward dir (0x00447614..)
    float back_pl[3] = {offset[0] * dist, 0.f, offset[2] * dist};
    Vec3Norm(back_pl, back_pl);
    float hmix = (2.5f /*0x005cd088*/ * zoom) / 10.f +
                 ((10.f - zoom) * -2.5f /*0x005ce1b0*/) / 10.f;
    if (track_type == 0x1a)
        hmix -= (2.25f /*0x005ce1ac*/ * zoom) / 10.f;
    if (!overhead) {
        for (int i = 0; i < 3; ++i) {
            mid[i] += back_pl[i] * hmix;
            cam_pos[i] += back_pl[i] * hmix;
        }
    }

    // snap-vs-spring (0x004477e6..0x0044798f)
    {
        const float d[3] = {cam_pos[0] - smooth_pos_[0],
                            cam_pos[1] - smooth_pos_[1],
                            cam_pos[2] - smooth_pos_[2]};
        const bool snap = Vec3Mag(d) > 5.f /*0x005cc358*/;
        if (force_reset || snap) {
            for (int i = 0; i < 3; ++i) {
                smooth_pos_[i] = cam_pos[i];
                spring_pos_[i] = 0.f;
                smooth_tgt_[i] = mid[i];
                spring_tgt_[i] = 0.f;
            }
            pair_a_ = A;
            pair_b_ = B;
            pair_blend_ = 0.f;
        }
    }
    // position spring: vel += (new-old)*0.05 (0x005cc9a0); vel *= 0.8
    // (0x005cc9bc); pos -= ((10-zoom)/10)*(pos - (old+vel))
    for (int i = 0; i < 3; ++i) {
        spring_pos_[i] += (cam_pos[i] - smooth_pos_[i]) * 0.05f;
        spring_pos_[i] *= 0.8f;
        cam_pos[i] -= ((10.f - zoom) / 10.f) *
                      (cam_pos[i] - (smooth_pos_[i] + spring_pos_[i]));
    }
    float tgt[3];
    for (int i = 0; i < 3; ++i) {
        spring_tgt_[i] += (mid[i] - smooth_tgt_[i]) * 0.05f;
        spring_tgt_[i] *= 0.8f;
        const float blended = smooth_tgt_[i] + spring_tgt_[i];
        tgt[i] = mid[i] - ((10.f - zoom) / 10.f) * (mid[i] - blended);
    }

    // sway (0x00447c26..0x00447ee4; skipped when overhead): 9 oscillators
    if (!overhead) {
        const std::uint32_t tq = time_ticks / 60000u;   // u32 quotient
        const float ft = static_cast<float>(tq);
        const float amp = (zoom + 0.2f /*0x005cc9c0*/) / 5.f /*0x005cc358*/;
        tgt[0]     += std::sin(ft / 420.f /*0x005ce040*/) / 10.f * amp;
        tgt[2]     += std::sin(ft / 635.f /*0x005ce1a8*/) / 10.f * amp;
        cam_pos[0] += std::sin(ft / 400.f /*0x005ce044*/) / 5.f * amp;
        cam_pos[1] += std::sin(ft / 155.f /*0x005cd9e8*/) / 5.f * amp;
        cam_pos[2] += std::sin(ft / 175.f /*0x005ce1a4*/) / 5.f * amp;
        cam_pos[0] += std::sin(ft / 83.f  /*0x005ce1a0*/) / 10.f * amp;
        cam_pos[2] += std::sin(ft / 139.5f/*0x005ce19c*/) / 10.f * amp;
        cam_pos[0] += std::sin(ft / 33.9f /*0x005ce198*/) / 200.f /*0x005ce194*/ * amp;
        cam_pos[2] += std::sin(ft / 16.5f /*0x005ce190*/) / 200.f * amp;
    }

    // target jitter (0x00447eea..0x00447f4a): rand(-amp, +amp) per component.
    // ADAPTER: xorshift PRNG stands in for FUN_00534870; amp was 0.0 in the
    // live probe, making this inert in the standard race.
    if (jitter_amp != 0.f) {
        auto rnd = [this](float lo, float hi) {
            prng_ ^= prng_ << 13; prng_ ^= prng_ >> 17; prng_ ^= prng_ << 5;
            return lo + (hi - lo) * (static_cast<float>(prng_ & 0x7fffffff) /
                                     2147483648.f);
        };
        for (int i = 0; i < 3; ++i)
            tgt[i] += rnd(-jitter_amp, jitter_amp);
    }

    // write-out (0x00447f50..): output pos also re-bases the smoothed pos;
    // smoothed target = final target (verbatim re-basing).
    for (int i = 0; i < 3; ++i) {
        pos_out_[i] = cam_pos[i];
        smooth_pos_[i] = cam_pos[i];
        smooth_tgt_[i] = tgt[i];
        tgt_out_[i] = tgt[i];
    }
    view_window_ = 0.6f;   // 0x3f19999a -> cam[0x16]
    // (The original also derives cam[0xd]/[0xe] Euler angles from
    // (target - pos) and rebuilds the RW frame via Camera::Apply 0x00441760
    // — yaw about Y, pitch about right, roll 0, translate. A LookAt from
    // pos/target with Y-up is the same rotation for roll = 0; the host
    // builds the view matrix that way.)
}

// ---- 0x00410d10 standard path: the elimination rule ------------------------
int RaceCamera::EliminationCheck(const RaceCamCar cars[4]) const {
    // 0x00410ee3: fcomp [0x005cc55c] equality — fires only when the camera's
    // required zoom SATURATED at exactly 10.0 (the clamp writes exactly 10.0).
    if (required_zoom_ != 10.f) return -1;

    int a, b;
    MostSeparatedPair(cars, &a, &b);    // 0x00410efb call 0x0040e180
    float pa = cars[a].race_pct;        // 0x00408ad0 progress %
    float pb = cars[b].race_pct;
    // lap-wrap adjust (0x00410f4a..0x00410fa6): 80/20/100 window
    if (pa <= 80.f /*0x005cc730*/ || 20.f /*0x005ccd6c*/ <= pb) {
        if (80.f < pb && pa < 20.f) pb -= 100.f;   // 0x005cc568
    } else {
        pa -= 100.f;
    }
    // (mode-1 "it"-player promotion to 100.0 not ported — standard path)

    // straggler = lesser progress (0x00410fe2..0x00411014 swap dance)
    int victim = a, other = b;
    if (pb < pa) { victim = b; other = a; }
    if (cars[other].dead_flag) {
        if (cars[victim].dead_flag) return -1;   // both dying → no kill
        victim = other;                          // verbatim candidate switch
    }
    return victim;
    // Caller applies: kill victim (FUN_00422fd0 equivalent), hit FX
    // (FUN_004922e0), award points (FUN_0040eee0(victim, 1) — ported with
    // the points system), then round-end at ≤1 alive (0x00411082 tail).
}

}  // namespace Race
}  // namespace mashed_re
