// Mashed RE — WS-B (orchestrator): FUN_0046f6c0 wheel contact solver.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool3, read_only, 2026-06-16). Map: re/analysis/WSB2_B3_CONTACT_PORT_MAP.md
//
// The per-tick car<->world contact ORCHESTRATOR: resets the contact scratch, runs
// the RW contact queries + the broadphase (producer) + the classifier, then the
// per-wheel 3-state machine (state at self+0x198/0x25c/0x320/0x3e4) + position /
// orientation correction + rotation alignment + velocity-friction impulse, and
// writes the grounded-wheel count to self+0x9e0. The wheel STATES it sets are
// exactly what FUN_0046ddb0 (the force integrator) reads as the grounded count —
// this is the missing link between the producer/classifier and the integrator.
//
// Original: void FUN_0046f6c0(int param_1 /*substep*/); EDI=vehicle, EAX=world.
// Ported: WheelContactSolver(int* self, void* world, int substep). EDI->self via
// vF()/vFP() (float reinterpret); float10->double; contiguous stack vec3s -> arrays.
//
// C-LEVEL: C2 faithful transcription. Residuals: the RW contact queries
// (FUN_004c3d90), the register-arg FUN_0046c5f0 (wheel normal), and the RW
// rotation set (FUN_004c52f0) are stubbed/marked; they do not affect the wheel-
// state -> grounded-count output the integrator consumes.
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"

namespace mashed_re {
namespace Collision {

const CollTriangle* g_worldTris    = nullptr;
int                 g_worldTriCount = 0;

// constants (raw value @ address)
static const float kWTorque   = 0.277779f;  // _DAT_005cea60
static const float kReset10   = 10.0f;      // literal 0x41200000
static const float kState2Lo  = 0.02f;      // _DAT_005ce18c
static const float kStateCmp  = -0.005f;    // _DAT_005ceaa0
static const float kSpring2   = -2.0f;      // _DAT_005cc34c
static const double kFwdLo    = -0.1;       // _DAT_005cea98 (double)
static const double kFwdHi    =  0.1;       // _DAT_005cea90 (double)
static const float kHiProj    = 0.998901f;  // _DAT_005cea88
static const float kRadToDeg  = 57.29578f;  // _DAT_005cc98c
static const float kRotClamp  = 2.5f;       // _DAT_005cd088
static const float kFricVel   = 1000.0f;    // _DAT_005cc9fc
static const float kLowSpeed  = 0.7f;       // _DAT_005cc340
static const float kFricImp   = 10.0f;      // _DAT_005cc55c
static const float kAngScale  = 9.98199e-6f;// _DAT_005cc990
static const float kGroundThr = 2.0f;       // _DAT_005cc574
static const float kDrift     = -20.0f;     // _DAT_005cd61c
static const float kDriftAng  = -0.0625f;   // _DAT_005cea84
static const float kSentSurf  = asF(0xff020202);  // -1.7281006e+38
static const float kSentActive= asF(0x00000001);  // 1.4013e-45 (contact-active marker)
// up axis (0,1,0) = kUpX/kUpY/kUpZ from ContactConstants.h
// (_DAT_006146fc/00614700/00614704)

// 0x0046f6c0
void WheelContactSolver(int* self, void* world, int substep)
{
    char cVar13 = 0;
    int* iVar12 = self + self[0x26b] * 0x10 + 0x24a;   // wheel-ring matrix block
    g_activeContactCount = 0;                          // DAT_0088e650
    float local_98[3] = { 0.0f, 0.0f, 0.0f };          // contact-query scratch
    int local_ac = Sys_TickCount();

    // --- per-wheel init: clear skip flags, reset contact result, susp positions
    int*   puVar11 = g_wheelSkipFlags;
    int*   puVar6  = self + 0x65;
    float* puVar7  = &g_suspScratch[1];   // &DAT_00881564
    for (int w = 0; w < 4; ++w) {
        *puVar11 = 0;
        float fv = vF(puVar6, -10) * kWTorque;
        puVar6[0x16] = (int)0xffffffff;   // contact result = -1
        vF(puVar6, 0) = kReset10;         // reset spring distance
        puVar7[0]  = 0.0f;
        puVar7[-1] = fv;
        float* pf15 = reinterpret_cast<float*>(puVar6 - 8);
        puVar7[1]  = pf15[0] * kWTorque;
        puVar11 += 1;
        puVar6  += 0x31;
        puVar7  += 3;
    }

    // --- RW contact queries (residual: stubbed vtable dispatch) ---
    Rw_VtableDispatch(g_contactQueryScratch, local_98, 1, iVar12);          // &DAT_0088e600
    Rw_VtableDispatch(g_wheelContactPos, g_suspScratch, 4, iVar12);         // &DAT_0088e620 <- transform susp pos
    // FUN_0046c5f0() — register-arg TriangleFaceNormal (wheel normal); args not
    // resolvable from the decomp, residual for full fidelity (no effect on states).

    // --- broadphase (producer) + classifier ---
    float qc[3] = { vF(iVar12, 0xc), vF(iVar12, 0xd), vF(iVar12, 0xe) };    // iVar12+0x30/0x34/0x38
    float qradius = vF(self, 0x129);                                       // self+0x4a4
    g_terrainEntryCount = 0;
    ProduceTerrainBatch(qc, qradius, g_worldTris, g_worldTriCount);        // FUN_00538c80 stand-in
    WheelTerrainContactClassifier(self, reinterpret_cast<float*>(g_terrainBatch));
    (void)world; (void)substep;

    // --- "high-speed but no slow-contact" flag (bVar4) over 4 wheels ---
    int iVar8 = 0;
    if (self[0x66] == 1) {
        float fv = vF(self, 0x65);
        cVar13 = (kState2Lo <= fv) ? 1 : 0;
        iVar8 = 1;
        if ((fv < kStateCmp) == (fv == kStateCmp)) iVar8 = 0;  // orig: if idiom holds, goto iVar8=0
    }
    auto accWheel = [&](int stateIdx, int loadIdx) {
        if (self[stateIdx] == 1) {
            float fv = vF(self, loadIdx);
            if (kState2Lo <= fv) cVar13 = (char)(cVar13 + 1);
            if ((fv < kStateCmp) != (fv == kStateCmp)) iVar8 = iVar8 + 1;
        }
    };
    accWheel(0x97, 0x96);   // self+0x25c state, self+0x258 load (600 byte = 0x258 -> int 0x96)
    accWheel(0xc8, 0xc7);   // self+0x320 (800), self+0x31c
    accWheel(0xf9, 0xf8);   // self+0x3e4, self+0x3e0
    bool bVar4 = (cVar13 != 0) && (iVar8 == 0);

    // --- per-wheel 3-state machine + per-wheel accumulators ---
    float local_bc[4] = {0}, local_100v[4] = {0}, local_a8[4] = {0}, local_70[12] = {0};
    int iVar14 = 0; int byteoff = 0;
    int* piVar9 = self + 0x66;     // wheel-0 state
    for (int wi = 0; wi < 4; ++wi) {
        float fv = vF(piVar9, -1);                 // spring load (piVar9[-1])
        bool latched = false;
        if (piVar9[0] == 0) {
            if ((kSpring2 < fv) && ((fv < kZero) != (fv == kZero))) {
                piVar9[0] = 2;
                float proj = (vF(self, 0x277) * vF(self, 0x26e) +   // fwd.z*vel.z
                              vF(self, 0x275) * vF(self, 0x26c) +   // fwd.x*vel.x
                              vF(self, 0x276) * vF(self, 0x26d)) /  // fwd.y*vel.y
                             vF(self, 0x279);                       // / speed
                if ((proj != kZero) && (((double)kFwdLo < proj) && ((double)proj < kFwdHi)))
                    piVar9[0] = 2;
                latched = true;
            }
        } else if (((kState2Lo < fv) && bVar4) || (piVar9[0x15] == -1)) {
            piVar9[0] = 0;
        } else {
            piVar9[0] = 1;
            latched = true;
        }
        if (latched) {   // LAB_0046f9c0: accumulate contact force
            float f0 = vF(piVar9, 0x1a) * vF(piVar9, -1);
            local_bc[iVar14 / 4] = f0;
            *reinterpret_cast<float*>(reinterpret_cast<char*>(local_70) + byteoff) =
                g_wheelContactPos[byteoff / 4] - f0;
            float f1 = vF(piVar9, 0x1b) * vF(piVar9, -1);
            local_100v[iVar14 / 4] = f1;
            *reinterpret_cast<float*>(reinterpret_cast<char*>(local_70) + byteoff + 4) =
                g_wheelContactPos[byteoff / 4 + 1] - f1;
            float f2 = vF(piVar9, 0x1c) * vF(piVar9, -1);
            local_a8[iVar14 / 4] = f2;
            *reinterpret_cast<float*>(reinterpret_cast<char*>(local_70) + byteoff + 8) =
                g_wheelContactPos[byteoff / 4 + 2] - f2;
        }
        iVar14 += 4; byteoff += 0xc; piVar9 += 0x31;
    }

    // --- combine grounded wheels (bVar16) + drop the most-loaded if all 4 ---
    unsigned char bVar16 = (self[0x66] == 1) ? 1 : 0;
    float local_110 = 0.0f, local_f0 = 0.0f, fVar2 = kZero;
    if (bVar16) { local_f0 = local_100v[0]; local_110 = local_a8[0]; fVar2 = local_bc[0]; }
    if (self[0x97] == 1) { fVar2 += local_bc[1]; bVar16++; local_f0 += local_100v[1]; local_110 += local_a8[1]; }
    if (self[0xc8] == 1) { fVar2 += local_bc[2]; bVar16++; local_f0 += local_100v[2]; local_110 += local_a8[2]; }
    if (self[0xf9] == 1) { fVar2 += local_bc[3]; bVar16++; local_f0 += local_100v[3]; local_110 += local_a8[3]; }

    bool goLAB_0047001f = false;
    float local_e0[3] = {0,0,0};   // {local_e0, local_dc, local_d8} contiguous
    if (bVar16 == 4) {
        float a0 = vF(self, 0x65); if (a0 < kZero) a0 = -a0;
        float a1 = vF(self, 0x96); if (a1 < kZero) a1 = -a1;
        float mx = (a0 < a1) ? a1 : a0; unsigned int sel = (a0 < a1) ? 1u : 0u;
        float a2 = vF(self, 0xc7); if (a2 < kZero) a2 = -a2;
        if (mx < a2) { sel = 2; mx = a2; }
        float a3 = vF(self, 0xf8); if (a3 < kZero) a3 = -a3;
        if (mx < a3) sel = 3;
        fVar2   -= local_bc[sel];
        bVar16   = 3;
        local_f0 -= local_100v[sel];
        local_110-= local_a8[sel];
        self[sel * 0x31 + 0x66] = 0;   // (uVar10*0xc4 + 0x198 + EDI)
    } else if (bVar16 == 0) {
        goLAB_0047001f = true;
    }

    if (!goLAB_0047001f) {
        float* pos = vFP(iVar12, 0xc);   // iVar12+0x30 (contact-point pos x)
        float inv = kOne / (float)bVar16;
        pos[0] -= inv * fVar2;
        vF(iVar12, 0xd) -= inv * local_f0;
        vF(iVar12, 0xe) -= inv * local_110;
        if (bVar16 == 3) {
            // orientation correction: promote one inactive wheel slot (verbatim copies)
            if (self[0x66] == 1) {
                if (self[0x97] == 1) {
                    if (self[0xc8] == 1) {
                        self[0x113]=self[0xe2]; self[0x114]=self[0xe3]; self[0x115]=self[0xe4];
                        self[0xf9]=1; self[0xf8]=self[0xc7]; self[0x10e]=self[0xdd];
                    } else {
                        self[0xe2]=self[0x113]; self[0xe3]=self[0x114]; self[0xe4]=self[0x115];
                        self[0xdd]=self[0x10e]; self[0xc8]=1; self[0xc7]=self[0xf8];
                    }
                } else {
                    self[0x97]=1; self[0xb1]=self[0x80]; self[0xb2]=self[0x81];
                    self[0xb3]=self[0x82]; self[0x96]=self[0x65]; self[0xac]=self[0x7b];
                }
            } else {
                self[0x80]=self[0xb1]; self[0x81]=self[0xb2]; self[0x82]=self[0xb3];
                self[0x66]=1; self[0x65]=self[0x96]; self[0x7b]=self[0xac];
            }
            // FUN_0046c5f0() — register-arg wheel normal (residual; no state effect).
        } else if (bVar16 < 3) {
            unsigned char n = (self[0x66] != 0) ? 1 : 0;
            if (n) { local_e0[0]=vF(self,0x80); local_e0[1]=vF(self,0x81); local_e0[2]=vF(self,0x82); }
            if (self[0x97] != 0) { n++; local_e0[0]+=vF(self,0xb1); local_e0[1]+=vF(self,0xb2); local_e0[2]+=vF(self,0xb3); }
            if (self[0xc8] != 0) { n++; local_e0[0]+=vF(self,0xe2); local_e0[1]+=vF(self,0xe3); local_e0[2]+=vF(self,0xe4); }
            if (self[0xf9] != 0) { n++; local_e0[0]+=vF(self,0x113); local_e0[1]+=vF(self,0x114); local_e0[2]+=vF(self,0x115); }
            if (n != 0) {
                float f = kOne / (float)n;
                local_e0[0]*=f; local_e0[1]*=f; local_e0[2]*=f;
                Vec3Normalize(local_e0, local_e0);
            }
            goLAB_0047001f = true;
        }
        if (bVar16 == 3) {
            // rotation alignment from the averaged contact normal (local_e0).
            // [UNCERTAIN] for bVar16==3 the orientation branch above did NOT set
            // local_e0; the original reads stale stack here. Zero-init makes the
            // cross product zero -> alignment skipped (deterministic, safe). Does
            // not affect the wheel-state -> grounded-count output.
            float cx = local_e0[2] * vF(self, 0x273) - local_e0[1] * vF(self, 0x274);
            float cy = local_e0[0] * vF(self, 0x274) - local_e0[2] * vF(self, 0x272);
            float cz = local_e0[1] * vF(self, 0x272) - local_e0[0] * vF(self, 0x273);
            float dotp = local_e0[2] * vF(self, 0x274) + local_e0[1] * vF(self, 0x273) + local_e0[0] * vF(self, 0x272);
            if (((cx != kZero) || (cy != kZero) || (cz != kZero)) &&
                ((dotp < kHiProj) || (local_ac < 3))) {
                float c = kZero - 1.0f;   // _DAT_005cc33c = -1.0
                if ((-1.0f <= dotp) && (c = dotp, kOne < dotp)) c = kOne;
                double ang = (double)Math_Acos((double)c) * (double)kRadToDeg;
                float angf = (float)ang;
                if ((substep != 0) && (kRotClamp < angf)) angf = kRotClamp;
                float axis[3] = { cx, cy, cz };
                float saved[3] = { vF(iVar12, 0xc), vF(iVar12, 0xd), vF(iVar12, 0xe) };
                unsigned char m[64];
                Rw_MatrixFromAxisAngle(m, axis, angf, 0);
                Rw_SetRotation(iVar12, m, 2);
                vF(iVar12, 0xc) = saved[0]; vF(iVar12, 0xd) = saved[1]; vF(iVar12, 0xe) = saved[2];
            }
        }
    }

    // --- LAB_0047001f: velocity-friction impulse (4 wheels) ---
    {
        float* pf = vFP(self, 0x82);   // unaff_EDI+0x208
        for (int k = 4; k != 0; --k) {
            if (pf[-0x1c] == kSentActive) {
                if (pf[-6] == kSentSurf) {
                    pf[-0x1c] = 0.0f;
                    vF(self, 0x26c) = pf[-2] * kFricVel + vF(self, 0x26c);
                    vF(self, 0x26d) = pf[-1] * kFricVel + vF(self, 0x26d);
                    vF(self, 0x26e) = pf[0]  * kFricVel + vF(self, 0x26e);
                }
                if (pf[-1] < kLowSpeed) {
                    pf[-0x1c] = 0.0f;
                    float e0 = pf[-2] * kFricImp, e1 = pf[-1] * kFricImp, e2 = pf[0] * kFricImp;
                    vF(self, 0x26c) += e0; vF(self, 0x26d) += e1; vF(self, 0x26e) += e2;
                    // orig: normalize VELOCITY (+0x9b0) into local_ec/e8/e4 (overwriting
                    // the impulse), then cross(velNorm, contactDir) * speed*angScale.
                    float vn[3];
                    Vec3Normalize(vn, vFP(self, 0x26c));
                    float s = vF(self, 0x279) * kAngScale;
                    float ix = (vn[2] * pf[-1] - vn[1] * pf[0])  * s;
                    float iy = (vn[0] * pf[0]  - vn[2] * pf[-2]) * s;
                    float iz = (vn[1] * pf[-2] - vn[0] * pf[-1]) * s;
                    vF(self, 0x51) += ix; vF(self, 0x52) += iy; vF(self, 0x53) += iz;
                }
            }
            pf += 0x31;
        }
    }

    // --- grounded count + lateral airborne drift ---
    float gc = (float)bVar16;
    vF(self, 0x278) = (float)bVar16;
    if ((self[0x27c] == 0) && (gc != kZero) && ((gc < kGroundThr) != (gc == kGroundThr))) {
        unsigned char nb = (self[0x66] != 1) ? 1 : 0;
        float v0 = kZero, v1 = 0.0f, v2 = 0.0f;
        if (nb) { v0 = vF(self, 0x27e); v1 = vF(self, 0x27f); v2 = vF(self, 0x280); }
        if (self[0x97] != 1) { nb++; v0 += vF(self, 0x281); v1 += vF(self, 0x282); v2 += vF(self, 0x283); }
        if (self[0xc8] != 1) { nb++; v0 += vF(self, 0x284); v1 += vF(self, 0x285); v2 += vF(self, 0x286); }
        if (self[0xf9] != 1) { nb++; v0 += vF(self, 0x287); v1 += vF(self, 0x288); v2 += vF(self, 0x289); }
        float f = kOne / (float)nb;
        float d[3];
        d[0] = vF(self, 0x256) - f * v0;
        d[1] = vF(self, 0x257) - f * v1;
        d[2] = vF(self, 0x258) - f * v2;
        Vec3Normalize(d, d);
        d[0] *= kDrift; d[1] = 0.0f; d[2] *= kDrift;
        vF(self, 0x26c) = d[0] + vF(self, 0x26c);
        vF(self, 0x26e) = d[2] + vF(self, 0x26e);
        Vec3Normalize(d, d);
        float bc[3];
        bc[0] = d[1] * kUpZ - d[2] * kUpY;
        bc[1] = d[2] * kUpX - d[0] * kUpZ;
        bc[2] = d[0] * kUpY - d[1] * kUpX;
        Vec3Normalize(bc, bc);
        vF(self, 0x51) += bc[0] * kDriftAng;
        vF(self, 0x52) += bc[1] * kDriftAng;
        vF(self, 0x53) += bc[2] * kDriftAng;
    }

    // --- final grounded count from wheel states (what FUN_0046ddb0 reads) ---
    self[0x278] = 0;
    if (self[0x66] != 0) vF(self, 0x278) = kOne;
    if (self[0x97] != 0) vF(self, 0x278) = vF(self, 0x278) + kOne;
    if (self[0xc8] != 0) vF(self, 0x278) = vF(self, 0x278) + kOne;
    if (self[0xf9] != 0) vF(self, 0x278) = vF(self, 0x278) + kOne;
}

}  // namespace Collision
}  // namespace mashed_re
