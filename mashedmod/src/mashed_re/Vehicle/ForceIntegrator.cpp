// Mashed RE — WS-A5: FUN_0046ddb0 verbatim port (the per-wheel force integrator).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool3, read_only, 2026-06-16). Map + evidence: re/analysis/WSA5_FORCE_INTEGRATOR_MAP.md
//
// Original: void FUN_0046ddb0(float param_1 /*dt*/, undefined4 param_2 /*xform*/),
// vehicle record in EDI. Ported with EDI -> `int* self`; float fields via vF()
// (the decompiler's `(float)self[i]` is a bit reinterpret, and its `self[i] =
// (int)(floatexpr)` is a float store — both honored via vF()). float10 -> double.
//
// This is the WS-B4 consumer: it reads the per-car contact summaries the B2/B3
// solvers fill (the 4-car {active@+4, count@+8} fields in the vehicle array) and
// the per-wheel contact flags, and turns them into grip / drive-drag / gravity /
// steer-torque / suspension-force / airborne updates.
//
// C-LEVEL: C2 faithful transcription. Gate = installed-hook scenario telemetry vs
// the captured contact baseline (re/analysis/wsb_contact_baseline.json), once
// wired (B4). Residual RW-math / PRNG / runtime-global deps stubbed inert.
#include "ForceIntegrator.h"
#include "../Collision/ContactSolvers.h"   // reuse TriangleFaceNormal (0x0046c5f0)

namespace mashed_re {
namespace Vehicle {

using namespace fi;

// 0x0046ddb0
void VehicleWheelForceIntegrate(int* self, float dt, void* xform)
{
    static const float kUpAxis[3]  = { 0.0f, 1.0f, 0.0f };  // DAT_006146fc
    static const float kFwdAxis[3] = { 0.0f, 0.0f, 1.0f };  // DAT_00614708
    char* gb = reinterpret_cast<char*>(g_vehicleArrayBase);  // DAT_008815a0
    auto carActive = [&](int c) { return *reinterpret_cast<int*>(gb + 4 + c * 0xd04); };
    auto carCount  = [&](int c) { return *reinterpret_cast<int*>(gb + 8 + c * 0xd04); };

    // ---- Phase 0: world forward + grounded count + per-wheel transforms ----
    Rw_TransformPoints(vFP(self, 0x275), kFwdAxis, 1, xform);  // forward = xform*(0,0,1)
    self[0x278] = 0;                                           // grounded count = 0.0
    int* piVar10 = reinterpret_cast<int*>(g_suspScratch);      // DAT_00881560
    for (int w = 0; w < 4; ++w) {
        int* piVar12 = self + 0x5b + w * 0x31;                 // wheel block
        if (piVar12[0xb] != 0) vF(self, 0x278) = vF(self, 0x278) + kOne;  // contact -> grounded++
        piVar10[0] = piVar12[0];
        piVar10[1] = 0;
        piVar10[2] = piVar12[2];
        Rw_TransformPoints(vFP(piVar12, 5), reinterpret_cast<float*>(piVar10), 1, xform);
        if (vF(piVar12, 0xf) == kZero) {                       // steer angle == 0
            piVar12[0x2d] = self[0x275];                       // steered fwd = forward
            piVar12[0x2e] = self[0x276];
            piVar12[0x2f] = self[0x277];
        } else {
            unsigned char m[64];
            Rw_MatrixFromAxisAngle(m, kUpAxis, vF(piVar12, 0xf), 0);
            Rw_TransformPoints(vFP(piVar12, 0x2d), vFP(self, 0x275), 1, m);
        }
        piVar10 += 3;
    }

    // ---- Phase 1: velocity history ring + drive-torque base ----
    unsigned int idx = (unsigned int)(self[0x2b4] - 1) & 1;
    self[0x2b4] = (int)idx;
    self[idx * 3 + 0x2b5] = self[0x26c];
    self[idx * 3 + 0x2b6] = self[0x26d];
    self[idx * 3 + 0x2b7] = self[0x26e];
    float fVar4 = vF(self, 0x279);                             // speed
    if (vF(self, 0x279) < kSpeedMin) fVar4 = kZero;
    fVar4 = vF(self, 0x54) * vF(self, 0x55) * vF(self, 0x56) * fVar4;   // gear product * speed
    if (self[0x278] != 0x40800000) fVar4 = fVar4 * kHalf;              // not all-4-grounded -> half
    fVar4 = fVar4 * vF(self, 0x15) * dt * kDt;

    // ---- Phase 2: drafting / proximity grip reduction (local_70) ----
    float local_70 = 1.0f;
    for (int c = 0; c < 4; ++c) {
        int iVar13 = c * 0xd04;
        if ((c != self[0]) && (*reinterpret_cast<int*>(gb + 4 + iVar13) == 1)) {
            int iVar3 = self[0x26a];                            // self active slot
            int iVar8 = *reinterpret_cast<int*>(gb + 0x9a8 + iVar13) * 0x40;  // wheel-set sel
            int iVar9 = iVar8 + iVar13;
            float delta[3], othervel[3], deltaNorm[3];
            delta[0] = *reinterpret_cast<float*>(gb + 0x958 + iVar13 + iVar8) - vF(self, iVar3 * 0x10 + 0x256);
            delta[1] = *reinterpret_cast<float*>(gb + 0x95c + iVar9)         - vF(self, iVar3 * 0x10 + 599);
            othervel[0] = *reinterpret_cast<float*>(gb + 0x9b0 + iVar13);
            delta[2] = *reinterpret_cast<float*>(gb + 0x960 + iVar9)         - vF(self, iVar3 * 0x10 + 600);
            othervel[1] = *reinterpret_cast<float*>(gb + 0x9b4 + iVar13);
            othervel[2] = *reinterpret_cast<float*>(gb + 0x9b8 + iVar13);
            double mag = Vec3Mag3(othervel);
            if ((double)kSpeedMin <= mag) {
                Vec3Norm3(othervel, othervel);
                Vec3Norm3(deltaNorm, delta);
                if (kDraftDot < deltaNorm[2] * othervel[2] + deltaNorm[0] * othervel[0] + deltaNorm[1] * othervel[1]) {
                    double dd = Vec3Mag3(delta);
                    if (dd < (double)kDraftDist) {
                        double r = (double)kOne - ((double)kDraftDist - dd) * (double)kDraft0p125;
                        if (r < (double)local_70) local_70 = (float)r;
                    }
                }
            }
        }
    }

    // ---- Phase 3: player-count / time-ramp / contact-count / rubber-band grip ----
    if (g_playerCount == 4)      local_70 = 1.0f;
    else if (g_playerCount == 9) local_70 = 1.0f;
    else if ((g_playerCount == 8) && (self[0] != 0)) local_70 = 0.75f;

    float fVar5 = kGripRampHi - (float)g_raceTimer;
    float local_6c;
    if (kZero <= fVar5) {
        local_6c = 1.0f;
    } else {
        if (fVar5 < kGripRampLo) fVar5 = kGripRampLo;
        local_6c = kOne - fVar5 * kGripRampK;
    }

    int iVar11 = 0;
    if ((carActive(0) != 0) && (0 < carCount(0))) iVar11 = carCount(0);
    if ((carActive(1) != 0) && (iVar11 < carCount(1))) iVar11 = carCount(1);
    if ((carActive(2) != 0) && (iVar11 < carCount(2))) iVar11 = carCount(2);
    if ((carActive(3) != 0) && (iVar11 < carCount(3))) iVar11 = carCount(3);
    fVar5 = kGripCntNeg;
    if (((iVar11 != 0) && (fVar5 = kZero, iVar11 != 1)) &&
        (fVar5 = kHalf, iVar11 != 2)) { fVar5 = kZero; if (iVar11 == 3) fVar5 = kOne; }

    if (local_6c == kZero) {
        iVar11 = 3;
        if ((carActive(0) != 0) && (carCount(0) < 3)) iVar11 = carCount(0);
        if ((carActive(1) != 0) && (carCount(1) < iVar11)) iVar11 = carCount(1);
        if ((carActive(2) != 0) && (carCount(2) < iVar11)) iVar11 = carCount(2);
        if ((carActive(3) != 0) && (carCount(3) < iVar11)) iVar11 = carCount(3);
        if (iVar11 == 0) { local_6c = 1.0f; fVar5 = kGripCntNeg; }
    }
    local_70 = (fVar5 * local_6c + kOne) * local_70;
    local_70 = (float)((double)RubberBandGrip(self[0], local_70, local_6c) * (double)local_70);
    if ((kZero < local_6c) && (RubberBandGate(self[0]) != 0)) {
        unsigned char n = (carActive(0) != 0) ? 1 : 0;
        if (carActive(1) != 0) n = n + 1;
        if (carActive(2) != 0) n = n + 1;
        if (carActive(3) != 0) n = n + 1;
        if (2 < n) local_70 = local_70 * kHalf;
    }

    // ---- Phase 4: drive-drag + gravity applied to linear velocity ----
    fVar4 = kOne - local_70 * fVar4;
    if ((fVar4 < kZero) || (kOne < fVar4)) fVar4 = kZero;
    vF(self, 0x26c) = fVar4 * vF(self, 0x26c);
    vF(self, 0x26d) = fVar4 * vF(self, 0x26d);
    vF(self, 0x26e) = fVar4 * vF(self, 0x26e);
    float gscale = dt * vF(self, 0x279) * kAngScale;
    if (k2 < gscale) gscale = k2;
    gscale = gscale * g_gravScale;
    vF(self, 0x26c) = g_gravX * gscale + vF(self, 0x26c);
    vF(self, 0x26d) = g_gravY * gscale + vF(self, 0x26d);
    vF(self, 0x26e) = g_gravZ * gscale + vF(self, 0x26e);

    // ---- Phase 5: steer torque from velocity delta (when any wheel grounded) ----
    if (vF(self, 0x278) != kZero) {
        int hi = self[0x2b4];
        unsigned int lo = (unsigned int)(hi - 1) & 1;
        float dv[3];
        dv[0] = (vF(self, hi * 3 + 0x2b5) - vF(self, lo * 3 + 0x2b5)) * dt;
        dv[1] = (vF(self, hi * 3 + 0x2b6) - vF(self, lo * 3 + 0x2b6)) * dt;
        dv[2] = (vF(self, hi * 3 + 0x2b7) - vF(self, lo * 3 + 0x2b7)) * dt;
        float fb = (vF(self, 0x14) / vF(self, 0x278)) * g_suspDtTerm;
        float clamp = fb * kThird;
        // 4 wheels: active flag, right-axis dot dv -> steer torque slot + scratch
        struct WSlot { int act; int rax; int outSlot; int scr; };
        const WSlot ws[4] = {
            { 0x66, 0x60, 0x83,  1 },   // wheel0: right axis self[0x60..], out self[0x83], scratch g_suspScratch[1]
            { 0x97, 0x91, 0xb4,  4 },
            { 200,  0xc2, 0xe5,  7 },
            { 0xf9, 0xf3, 0x116, 10 },
        };
        for (int wi = 0; wi < 4; ++wi) {
            if (self[ws[wi].act] == 0) {
                self[ws[wi].outSlot] = 0;
            } else {
                float proj = (vF(self, ws[wi].rax + 2) * dv[2] +
                              vF(self, ws[wi].rax) * dv[0] +
                              vF(self, ws[wi].rax + 1) * dv[1]) * kSteerProj;
                if (clamp < proj) proj = clamp;
                if (proj < -clamp) proj = -clamp;
                vF(self, ws[wi].outSlot) = fb + proj;
                float scrv;
                if (proj <= k20) { if (proj < kNeg20) proj = kNeg20; scrv = proj * kSteerOut; }
                else { scrv = k20 * kSteerOut; }   // _DAT_005ccd6c = 20.0
                g_suspScratch[ws[wi].scr] = scrv;
            }
        }
        vF(self, 0x116) = vF(self, 0x116) * kHalf;
        unsigned int ring = (unsigned int)(self[0x2fb] + 1) & 0xf;
        self[0x2fb] = (int)ring;
        // steer-feedback normal of 3 suspension-force vectors -> self ring slot
        Collision::TriangleFaceNormal(&g_suspScratch[0], &g_suspScratch[3], &g_suspScratch[6],
                                      vFP(self, ring * 3 + 0x2cb));
        int slot = ring * 3 + 0x2cd;
        if (vF(self, slot) < kZero) vF(self, slot) = vF(self, slot) * kQuarter;
    }

    // ---- Phase 6: per-wheel suspension force + surface jitter (4 wheels) ----
    int wheel = 0;
    do {
        float* pf = vFP(self, 0x7d + wheel * 0x31);   // wheel block (float view)
        if (pf[-0x17] == 0.0f) {                       // wheel NOT in contact
            pf[10] = 0.0f; pf[9] = 0.0f; pf[8] = 0.0f;
        } else {
            int key = asI(pf[-1]);                     // surface key (integer compare)
            pf[0] = kJit0p25;                          // default jitter coeff
            self[0x2c0] = 0; self[0x2c1] = 0; self[0x2c2] = 0;  // clear rand impulse
            if (key == kSurfRandom) {
                float r = vF(self, 0x279) * kRandScale;
                vF(self, 0x2c0) = Fi_RandRange(-r, r);
                self[0x2c1] = 0;
                vF(self, 0x2c2) = Fi_RandRange(-r, r);
            } else if (key == kSurf0p1) {
                pf[0] = kJit0p1;
            } else if (key == kSurf0p01) {
                pf[0] = kJit0p01;
            } else {
                if ((key == kSurfSlipA) || (key == kSurfSlipB))
                    pf[0] = (self[0x340] == 0) ? kJit0p01 : kJit0p2;
                if (key == kSurf0p2)
                    pf[0] = kJit0p2;
            }
            if (self[0xb] != 0) pf[0] = vF(self, 0xb) * pf[0] * kSpinScale;
            float f = pf[-0x1a] * pf[0];
            pf[0] = f;
            pf[1] = k0p6 * f;
            pf[2] = k0p05 * f;
            if (self[4] == 1) { pf[1] = f; pf[2] = f; }
            pf[7] = pf[6] * pf[4];
            if (pf[6] * pf[4] < kZero) pf[7] = 0.0f;
            double fVar17;
            if (self[0x278] == 0x40800000) {            // all 4 grounded
                float fc = g_suspScale * pf[6] * kHalf;
                pf[8]  = fc * pf[3];
                pf[9]  = (pf[4] - kOne) * fc;
                pf[10] = fc * pf[5];
                fVar17 = Vec3Mag3(&pf[8]);
                if ((float)fVar17 != kZero) {
                    double inv = (double)kOne / fVar17;
                    float a0 = (float)(inv * (double)pf[8]);
                    float a1 = (float)(inv * (double)pf[9]);
                    float a2 = (float)(inv * (double)pf[10]);
                    double proj = ((double)a2 * (double)pf[0xd] + (double)a0 * (double)pf[0xb] +
                                   (double)a1 * (double)pf[0xc]) * fVar17;
                    pf[8]  = (float)(proj * (double)pf[0xb]);
                    pf[9]  = (float)(proj * (double)pf[0xc]);
                    pf[10] = (float)(proj * (double)pf[0xd]);
                }
            } else {
                fVar17 = ((double)pf[6] / (double)dt) * (double)g_suspScale;
                pf[8]  = (float)(fVar17 * (double)pf[3]);
                pf[9]  = (float)(fVar17 * (double)pf[4]);
                pf[10] = (float)(fVar17 * (double)pf[5]);
            }
            if ((self[0x2c8] == 0) && ((double)kAirThr < fVar17)) self[0x2c8] = 1;
        }
        wheel += 1;
        if (wheel == 4) {
            if (vF(self, 0x278) < k3) {
                self[0x2c8] = 1;
                vF(self, 0x2c6) = vF(self, 0x2c6) - g_suspDtTerm * vF(self, 0x14) * k3000;
            }
            return;
        }
    } while (true);
}

// ---- 0x00442ce0: catch-up / rubber-band grip multiplier (resolves U-2687) ----
float RubberBandGrip(int car, float current, float band)
{
    if ((g_playerCount == 4) || (g_playerCount == 9) || (g_playerCount == 8)) return kOne;
    Fi_GameModeTick();   // FUN_0040e340 (observed side-effect retained)
    (void)current;
    if (g_rubberBand[car] == kZero) {
        int i1 = CarContactCount(car);
        int i2 = CarContactCount(g_rubberRefCar);
        if (i1 == 0) {
            if (i2 == 3) return k0p6 * band + kOne;          // _DAT_005cc318
            if (i2 == 2) return kHalf * band + kOne;         // _DAT_005cc32c
            if (i2 == 1) return kQuarter * band + kOne;      // _DAT_005cc564
        } else if (i1 == 1) {
            if (i2 == 3) return kHalf * band + kOne;
            if (i2 == 2) return kQuarter * band + kOne;
        } else if ((i1 == 2) && (i2 == 3)) {
            return kQuarter * band + kOne;
        }
    }
    return 0.0f * band + kOne;
}

// ---- 0x00442c80: rubber-band activation gate (resolves U-3563) ----
int RubberBandGate(int car)
{
    int mode = Fi_GameMode();
    if ((mode == 6) && (g_playerCount != 4) && (g_playerCount != 9) &&
        (g_playerCount != 8) && (kRubberThr < g_rubberBand[car])) {
        return 1;
    }
    return 0;
}

// ---- 0x0046dbe0: per-car contact-count getter (DAT_008815a8) ----
int CarContactCount(int car)
{
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(g_vehicleArrayBase) + 8 + car * 0xd04);
}

}  // namespace Vehicle
}  // namespace mashed_re
