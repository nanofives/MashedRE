// Mashed RE — WS-A5: FUN_0046ddb0 verbatim port (the per-wheel force integrator).
//
// [dcphys2 2026-10-22] ONE body, shared by mashed_re.exe and mashed_re_dev.asi (this TU is in both rsp files). The
// .asi's own copy (PhysicsChainHooks.cpp A5_Body) was deleted once this body read against the original in that file's
// in-process A/B (verify/dcphys2_20261022). Target-specific code is only the A5* thunks below: the record base, the
// out-of-record state (live addresses in the .asi), and the leaves with no both-target port (live RVAs in the .asi).
// The standalone's ports of the leaves 0x00442ce0, 0x00442c80 and 0x0046dbe0 moved, verbatim, to the exe-only
// Vehicle/ForceIntegratorLeaves.cpp; the .asi calls the originals.
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
#include <cstdio>                           // [A5GDIAG] default-OFF D2 diagnostic
#include <cstdlib>                          // [A5GDIAG] getenv
#include <cstring>                          // memset (D2 steered-forward diagnostic)
#include "ForceIntegrator.h"
#include "../Math/RwLutTables.h"           // [u9760] LutSqrt = FUN_004c3b30 with the real LUT
#include "../Collision/ContactSolvers.h"   // reuse TriangleFaceNormal (0x0046c5f0)
#include "../Collision/ContactDeps.h"      // Collision::g_suspScratch (DAT_00881560)
static bool A5DraftLut() { static const bool s = [] { const char* e = std::getenv("MASHED_A5_DRAFT_LUT"); return e && e[0] == '1'; }(); return s; }   // [u9771]

namespace mashed_re {
namespace Vehicle {

using namespace fi;

// [u9772] MASHED_A5_FD0_LIVE=1 (default OFF): A5 (0x0046ddb0), FUN_00442ce0 and FUN_00442c80 test the live
// DAT_007f0fd0 word, as the original does, instead of g_playerCount (= the car count, VehiclePhysics_Init).
#ifdef MASHED_STANDALONE
static int Fd0Word() {
    static const bool s = [] { const char* e = std::getenv("MASHED_A5_FD0_LIVE"); return e && e[0] == '1'; }();
    return s ? *reinterpret_cast<const int*>(0x007f0fd0u) : g_playerCount;
}
#else
static int Fd0Word() { return *reinterpret_cast<const int*>(0x007f0fd0u); }   // DAT_007f0fd0
#endif

// ── [dcphys2] target thunks ────────────────────────────────────────────────────
#ifdef MASHED_STANDALONE
static inline char*  A5VehBase()    { return reinterpret_cast<char*>(g_vehicleArrayBase); }
static inline float* A5Scratch()    { return Collision::g_suspScratch; }
static inline int    A5RaceTimer()  { return g_raceTimer; }
static inline float  A5GravScale()  { return g_gravScale; }
static inline float  A5GravX()      { return g_gravX; }
static inline float  A5GravY()      { return g_gravY; }
static inline float  A5GravZ()      { return g_gravZ; }
static inline float  A5SuspDtTerm() { return g_suspDtTerm; }
static inline float  A5SuspScale()  { return g_suspScale; }
static inline void   A5XformVec(float* dst, const float* src, void* m)   { Rw_TransformPoints(dst, src, 1, m); }
static inline void   A5AxisAngle(void* m, const float* axis, float a)    { Rw_MatrixFromAxisAngle(m, axis, a, 0); }
static inline float  A5Rand(float lo, float hi)                          { return Fi_RandRange(lo, hi); }
static inline float  A5RubberGrip(int car, float cur, float band)        { return RubberBandGrip(car, cur, band); }
static inline int    A5RubberGate(int car)                               { return RubberBandGate(car); }
static inline void   A5FaceNormal(float* a, float* p1, float* p2, float* out) { Collision::TriangleFaceNormal(a, p1, p2, out); }
#else
static inline char*  A5VehBase()    { return reinterpret_cast<char*>(0x008815a0u); }             // DAT_008815a0
static inline float* A5Scratch()    { return reinterpret_cast<float*>(0x00881560u); }            // DAT_00881560
static inline int    A5RaceTimer()  { return *reinterpret_cast<const int*>(0x007f0ff8u); }       // DAT_007f0ff8
static inline float  A5GravScale()  { return *reinterpret_cast<const float*>(0x00803340u); }     // _DAT_00803340
static inline float  A5GravX()      { return *reinterpret_cast<const float*>(0x00803334u); }     // _DAT_00803334
static inline float  A5GravY()      { return *reinterpret_cast<const float*>(0x00803338u); }     // _DAT_00803338
static inline float  A5GravZ()      { return *reinterpret_cast<const float*>(0x0080333cu); }     // _DAT_0080333c
static inline float  A5SuspDtTerm() { return *reinterpret_cast<const float*>(0x0088e610u); }     // _DAT_0088e610
static inline float  A5SuspScale()  { return *reinterpret_cast<const float*>(0x0088e5f0u); }     // _DAT_0088e5f0
static inline void   A5XformVec(float* dst, const float* src, void* m) {                         // FUN_004c3df0
    reinterpret_cast<void (__cdecl*)(float*, const float*, int, const void*)>(0x004c3df0u)(dst, src, 1, m);
}
static inline void   A5AxisAngle(void* m, const float* axis, float a) {                          // FUN_004c4d20
    reinterpret_cast<void (__cdecl*)(void*, const float*, float, int)>(0x004c4d20u)(m, axis, a, 0);
}
static inline float  A5Rand(float lo, float hi) {                                                // FUN_00472650
    return reinterpret_cast<float (__cdecl*)(float, float)>(0x00472650u)(lo, hi);
}
static inline float  A5RubberGrip(int car, float cur, float band) {                              // FUN_00442ce0
    return reinterpret_cast<float (__cdecl*)(int, float, float)>(0x00442ce0u)(car, cur, band);
}
static inline int    A5RubberGate(int car) {                                                     // FUN_00442c80
    return reinterpret_cast<int (__cdecl*)(int)>(0x00442c80u)(car);
}
// FUN_0046c5f0: EAX = apex, ECX = p1, EDX = p2, ESI = out, no stack args (0x0046c5f0..; v = apex - ECX and
// u = apex - EDX, n0 = v.z*u.y - v.y*u.z at 0x0046c5f3..0x0046c627, which is Collision::TriangleFaceNormal's p1 / p2).
static __declspec(naked) void A5FaceNormalAsm(float* /*a*/, float* /*p1*/, float* /*p2*/, float* /*out*/) {
    __asm {
        push esi
        push ebx
        mov  eax, dword ptr [esp+12]
        mov  ecx, dword ptr [esp+16]
        mov  edx, dword ptr [esp+20]
        mov  esi, dword ptr [esp+24]
        mov  ebx, 0x0046c5f0
        call ebx
        pop  ebx
        pop  esi
        ret
    }
}
static inline void   A5FaceNormal(float* a, float* p1, float* p2, float* out) { A5FaceNormalAsm(a, p1, p2, out); }
#endif

// [u9760 2026-10-18] MASHED_A5_SUSP_LUT=1 (default OFF, verify/u9760_20261019/PREREG_u9760.md): Phase 6's all-grounded
// suspension force in the original's arithmetic (LUT length FUN_004c3ac0, float steps, its summation order) instead of
// std::sqrt and double intermediates.
static bool A5SuspLut() {
    // [a6flip 2026-10-19] default ON (user decision); MASHED_NO_A5_SUSP_LUT=1 opts out.
    static const bool s = [] { const char* e = std::getenv("MASHED_NO_A5_SUSP_LUT"); return !(e && e[0] == '1'); }();
    return s;
}

// [dcphys2 2026-10-22] MASHED_A5_DOT_ORDER=1 (default OFF in the exe; the .asi seeds it ON): A5's 3-term dots in the
// original's association. Both were written in Ghidra's print order, c*z + a*x + b*y, which C++ groups (c*z + a*x) + b*y;
// the original adds the first two products it loads:
//   drafting dot    (dN.y*o.y + dN.x*o.x) + dN.z*o.z     0x0046dff6..0x0046e013 (dN = [esp+0x30..0x38], o = [esp+0x24..0x2c])
//   steer projection (r.y*dv.y + r.x*dv.x) + r.z*dv.z     wheel 0 0x0046e419..0x0046e43b, wheel 1 0x0046e4d1..0x0046e4f1,
//                                                         wheel 2 0x0046e589..; dv = [esp+0x18..0x20] (0x0046e39d / 3b3 / 3f2)
// [dcphys2 2026-10-22] MASHED_A5_AIR_F32=1 (default OFF in the exe; the .asi seeds it ON): Phase 6's not-all-grounded
// suspension force as the original rounds it at PC_24: f = (load / dt) * _DAT_0088e5f0, F = f * n, every step to float
// (0x0046e927 FLD +0x18, 0x0046e92a FDIV dt, 0x0046e931 FMUL, 0x0046e937..0x0046e94c three FMUL / FSTP), and f is what
// 0x0046e957 compares with 65536.0. OFF keeps the former double chain.
static bool A5AirF32() { static const bool s = [] { const char* e = std::getenv("MASHED_A5_AIR_F32"); return e && e[0] == '1'; }(); return s; }
// [dcphys2 2026-10-22] MASHED_A5_CONTACT_INT=1 (default OFF in the exe; the .asi seeds it ON): Phase 6's per-wheel
// contact test as the original makes it, an integer compare of the word with 0 (0x0046e761 CMP [ESI-0x5c],EBP with
// EBP = 0, 0x0046e764 JE), as Phase 5 already reads the same word (0x0046e4c5). The float compare it replaces takes
// 0x80000000 (-0.0) for "no contact"; the original takes it for contact. Measured on Warzone (verify/dcphys2_20261022).
static bool A5ContactInt() { static const bool s = [] { const char* e = std::getenv("MASHED_A5_CONTACT_INT"); return e && e[0] == '1'; }(); return s; }
// [dcphys2 2026-10-22] MASHED_A5_SPIN_FILD=1 (default OFF in the exe; the .asi seeds it ON): the jitter scale reads
// record +0x2c as an INTEGER (0x0046e824 MOV EAX,[EDI+0x2c], 0x0046e82f FILD, 0x0046e833 FMUL [ESI], 0x0046e835 FMUL
// _DAT_005cc328 = 0.01). The port read the word as float bits, so +0x2c = 50 gave a denormal and a jitter of 0 where the
// original stores 50 * 0.25 * 0.01 = 0.125 (Warzone, verify/dcphys2_20261022).
static bool A5SpinFild() { static const bool s = [] { const char* e = std::getenv("MASHED_A5_SPIN_FILD"); return e && e[0] == '1'; }(); return s; }
static bool A5DotOrder() { static const bool s = [] { const char* e = std::getenv("MASHED_A5_DOT_ORDER"); return e && e[0] == '1'; }(); return s; }

// 0x0046ddb0
void VehicleWheelForceIntegrate(int* self, float dt, void* xform)
{
    static const float kUpAxis[3]  = { 0.0f, 1.0f, 0.0f };  // DAT_006146fc
    static const float kFwdAxis[3] = { 0.0f, 0.0f, 1.0f };  // DAT_00614708
    char* gb = A5VehBase();                                  // DAT_008815a0

    // [A5GDIAG] D2 attempt 12 step 1 — default-OFF. Registered in
    // verify/d2_sink_20261001/PREREG.md (commit 26b859fd). Reads only; changes no
    // value A5 computes. §24.4 could only BACK OUT local_70 and G from the .msd;
    // this logs both directly, plus the exact scalar A5 applies at :166-168.
    static const bool s_a5g = (std::getenv("MASHED_A5GDIAG") != nullptr);
    static int s_a5gN = 0;
    float a5g_sEnt = 0.0f;
    if (s_a5g) {
        const float* ev = reinterpret_cast<const float*>(self) + 0x26c;   // +0x9b0
        a5g_sEnt = (float)Vec3Mag3(ev);
    }
    auto carActive = [&](int c) { return *reinterpret_cast<int*>(gb + 4 + c * 0xd04); };
    auto carCount  = [&](int c) { return *reinterpret_cast<int*>(gb + 8 + c * 0xd04); };

    // ---- Phase 0: world forward + grounded count + per-wheel transforms ----
    A5XformVec(vFP(self, 0x275), kFwdAxis, xform);             // forward = xform*(0,0,1)
    self[0x278] = 0;                                           // grounded count = 0.0
    int* piVar10 = reinterpret_cast<int*>(A5Scratch());        // DAT_00881560
    for (int w = 0; w < 4; ++w) {
        int* piVar12 = self + 0x5b + w * 0x31;                 // wheel block
        if (piVar12[0xb] != 0) vF(self, 0x278) = vF(self, 0x278) + kOne;  // contact -> grounded++
        piVar10[0] = piVar12[0];
        piVar10[1] = 0;
        piVar10[2] = piVar12[2];
        A5XformVec(vFP(piVar12, 5), reinterpret_cast<float*>(piVar10), xform);
        if (vF(piVar12, 0xf) == kZero) {                       // steer angle == 0
            piVar12[0x2d] = self[0x275];                       // steered fwd = forward
            piVar12[0x2e] = self[0x276];
            piVar12[0x2f] = self[0x277];
        } else {
            unsigned char m[64];
            // Hygiene only — `m` was read uninitialised. (The parenthetical that
            // used to stand here, "Rw_TransformPoints is a POINT transform and
            // consumes the translation row", is REFUTED: the original's call at
            // 0x0046de5b targets 0x004c3df0, device slot +0x14, which ignores
            // m[12..14] — re/analysis/D3_CONTACT_2026-09-27.md §2.2, U-9138.)
            // Kept because reading uninitialised memory is UB regardless.
            //
            // D2 2026-08-21: this was ALSO tested as a fix for the steered-forward
            // defect and REFUTED. Symptom: a steered wheel's steered-forward comes
            // out as EXACTLY 2x body forward with direction unchanged
            // (fwd0=(0.0420,1.9996) vs unsteered fwd3=(0.0210,0.9998)) — a scale,
            // not a rotation, so no lateral force and the car cannot turn. Zeroing
            // `m` changed the output by nothing at all, so the garbage translation
            // is NOT the cause. Whatever RwMatrixRotate(kUpAxis, 8.5, mode 0)
            // produces, feeding it to RwV3dTransformPointsCPU yields 2*src. Next
            // step is to read RwMatrixRotate / RwV3dTransformPointsCPU directly —
            // see re/analysis/D2_REALPHYS_REMEASURE_2026-08-21.md.
            std::memset(m, 0, sizeof m);
            A5AxisAngle(m, kUpAxis, vF(piVar12, 0xf));
            A5XformVec(vFP(piVar12, 0x2d), vFP(self, 0x275), m);
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
            if (A5DraftLut()) {
                // [u9771] MASHED_A5_DRAFT_LUT (default OFF): the original's drafting test (0x0046ddb0 loop): FUN_004c3ac0 and
                // FUN_004c39b0 are the RW LUT bodies, and the race loop's PC_24 rounds r = 1 - (6 - |gap|) * 0.125 to float.
                const float mag = mashed_re::Math::LutLength(othervel);
                if (kSpeedMin <= mag) {
                    mashed_re::Math::LutNormalize(othervel, othervel);
                    mashed_re::Math::LutNormalize(deltaNorm, delta);
                    const float dot = A5DotOrder()
                        ? (deltaNorm[1] * othervel[1] + deltaNorm[0] * othervel[0]) + deltaNorm[2] * othervel[2]
                        : deltaNorm[2] * othervel[2] + deltaNorm[0] * othervel[0] + deltaNorm[1] * othervel[1];
                    if (kDraftDot < dot) {
                        const float dd = mashed_re::Math::LutLength(delta);
                        if (dd < kDraftDist) {
                            const float r = kOne - (kDraftDist - dd) * kDraft0p125;
                            if (r < local_70) local_70 = r;
                        }
                    }
                }
                continue;
            }
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
    const int fd0 = Fd0Word();                                // DAT_007f0fd0
    if (fd0 == 4)      local_70 = 1.0f;
    else if (fd0 == 9) local_70 = 1.0f;
    else if ((fd0 == 8) && (self[0] != 0)) local_70 = 0.75f;

    float fVar5 = kGripRampHi - (float)A5RaceTimer();
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
    local_70 = (float)((double)A5RubberGrip(self[0], local_70, local_6c) * (double)local_70);
    if ((kZero < local_6c) && (A5RubberGate(self[0]) != 0)) {
        unsigned char n = (carActive(0) != 0) ? 1 : 0;
        if (carActive(1) != 0) n = n + 1;
        if (carActive(2) != 0) n = n + 1;
        if (carActive(3) != 0) n = n + 1;
        if (2 < n) local_70 = local_70 * kHalf;
    }

    // ---- Phase 4: drive-drag + gravity applied to linear velocity ----
    const float a5g_base = fVar4;            // [A5GDIAG] fVar4 as of :90
    fVar4 = kOne - local_70 * fVar4;
    if ((fVar4 < kZero) || (kOne < fVar4)) fVar4 = kZero;
    if (s_a5g && s_a5gN < 40000) {
        ++s_a5gN;
        if (std::FILE* lf = std::fopen("a5g_diag.log", "a")) {
            std::fprintf(lf,
                "i=%d car=%d sEnt=%.6f sp=%.6f g0=%.9g g1=%.9g g2=%.9g G=%.9g "
                "m54=%.9g dt=%.6f base=%.9g l70=%.9g sigma=%.9g gnd=0x%08X\n",
                s_a5gN, self[0], a5g_sEnt, vF(self, 0x279),
                vF(self, 0x54), vF(self, 0x55), vF(self, 0x56),
                vF(self, 0x54) * vF(self, 0x55) * vF(self, 0x56),
                vF(self, 0x15), dt, a5g_base, local_70, fVar4,
                (unsigned)self[0x278]);
            std::fclose(lf);
        }
    }
    vF(self, 0x26c) = fVar4 * vF(self, 0x26c);
    vF(self, 0x26d) = fVar4 * vF(self, 0x26d);
    vF(self, 0x26e) = fVar4 * vF(self, 0x26e);
    float gscale = dt * vF(self, 0x279) * kAngScale;
    if (k2 < gscale) gscale = k2;
    gscale = gscale * A5GravScale();
    vF(self, 0x26c) = A5GravX() * gscale + vF(self, 0x26c);
    vF(self, 0x26d) = A5GravY() * gscale + vF(self, 0x26d);
    vF(self, 0x26e) = A5GravZ() * gscale + vF(self, 0x26e);

    // ---- Phase 5: steer torque from velocity delta (when any wheel grounded) ----
    if (vF(self, 0x278) != kZero) {
        int hi = self[0x2b4];
        unsigned int lo = (unsigned int)(hi - 1) & 1;
        float dv[3];
        dv[0] = (vF(self, hi * 3 + 0x2b5) - vF(self, lo * 3 + 0x2b5)) * dt;
        dv[1] = (vF(self, hi * 3 + 0x2b6) - vF(self, lo * 3 + 0x2b6)) * dt;
        dv[2] = (vF(self, hi * 3 + 0x2b7) - vF(self, lo * 3 + 0x2b7)) * dt;
        float fb = (vF(self, 0x14) / vF(self, 0x278)) * A5SuspDtTerm();
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
                float proj = (A5DotOrder()
                              ? (vF(self, ws[wi].rax + 1) * dv[1] + vF(self, ws[wi].rax) * dv[0]) + vF(self, ws[wi].rax + 2) * dv[2]
                              : vF(self, ws[wi].rax + 2) * dv[2] + vF(self, ws[wi].rax) * dv[0] + vF(self, ws[wi].rax + 1) * dv[1])
                             * kSteerProj;
                if (clamp < proj) proj = clamp;
                if (proj < -clamp) proj = -clamp;
                vF(self, ws[wi].outSlot) = fb + proj;
                float scrv;
                if (proj <= k20) { if (proj < kNeg20) proj = kNeg20; scrv = proj * kSteerOut; }
                else { scrv = k20 * kSteerOut; }   // _DAT_005ccd6c = 20.0
                A5Scratch()[ws[wi].scr] = scrv;
            }
        }
        vF(self, 0x116) = vF(self, 0x116) * kHalf;
        unsigned int ring = (unsigned int)(self[0x2fb] + 1) & 0xf;
        self[0x2fb] = (int)ring;
        // steer-feedback normal of 3 suspension-force vectors -> self ring slot
        // [d2off] MASHED_A5_RING_ORDER=1 (default OFF): the original's register order, EAX = &scratch[0], ECX (p1) =
        // &scratch[6], EDX (p2) = &scratch[3] (0x0046e6f3..0x0046e728); the port's order gives the opposite sign.
        static const bool s_ringOrder = [] { const char* e = std::getenv("MASHED_A5_RING_ORDER"); return e && e[0] == '1'; }();
        float* const sc = A5Scratch();
        if (s_ringOrder) A5FaceNormal(&sc[0], &sc[6], &sc[3], vFP(self, ring * 3 + 0x2cb));
        else             A5FaceNormal(&sc[0], &sc[3], &sc[6], vFP(self, ring * 3 + 0x2cb));
        int slot = ring * 3 + 0x2cd;
        if (vF(self, slot) < kZero) vF(self, slot) = vF(self, slot) * kQuarter;
    }

    // ---- Phase 6: per-wheel suspension force + surface jitter (4 wheels) ----
    int wheel = 0;
    do {
        float* pf = vFP(self, 0x7d + wheel * 0x31);   // wheel block (float view)
        if (A5ContactInt() ? (asI(pf[-0x17]) == 0) : (pf[-0x17] == 0.0f)) {   // wheel NOT in contact (0x0046e761)
            pf[10] = 0.0f; pf[9] = 0.0f; pf[8] = 0.0f;
        } else {
            int key = asI(pf[-1]);                     // surface key (integer compare)
            pf[0] = kJit0p25;                          // default jitter coeff
            self[0x2c0] = 0; self[0x2c1] = 0; self[0x2c2] = 0;  // clear rand impulse
            if (key == kSurfRandom) {
                float r = vF(self, 0x279) * kRandScale;
                vF(self, 0x2c0) = A5Rand(-r, r);
                self[0x2c1] = 0;
                vF(self, 0x2c2) = A5Rand(-r, r);
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
            if (self[0xb] != 0) pf[0] = (A5SpinFild() ? (float)self[0xb] : vF(self, 0xb)) * pf[0] * kSpinScale;   // 0x0046e82f
            float f = pf[-0x1a] * pf[0];
            pf[0] = f;
            pf[1] = k0p6 * f;
            pf[2] = k0p05 * f;
            if (self[4] == 1) { pf[1] = f; pf[2] = f; }
            pf[7] = pf[6] * pf[4];
            if (pf[6] * pf[4] < kZero) pf[7] = 0.0f;
            double fVar17;
            if (self[0x278] == 0x40800000 && A5SuspLut()) {
                // [u9760] the original's arithmetic, 0x0046e895..0x0046e925 (x87 at PC_24, so every step rounds to float):
                //   fc = (_DAT_0088e5f0 * load) * 0.5; F = (fc*n.x, (n.y - 1)*fc, fc*n.z)          0x0046e895..0x0046e8c0
                //   len = FUN_004c3ac0(F) = LutSqrt(float((x*x + y*y) + z*z))                      0x0046e8c3, 0x004c3ac4..
                //   inv = 1/len (0x0046e8de); a = inv*F (stored floats 0x0046e8e4 / 8ed / 8f4)
                //   t = ((a.y*axis.y + a.x*axis.x) + a.z*axis.z) * len; F = t * axis                 0x0046e8f8..0x0046e925
                float fc = A5SuspScale() * pf[6] * kHalf;
                pf[8]  = fc * pf[3];
                pf[9]  = (pf[4] - kOne) * fc;
                pf[10] = fc * pf[5];
                const float m2 = (pf[8] * pf[8] + pf[9] * pf[9]) + pf[10] * pf[10];
                const float len = mashed_re::Math::LutSqrt(m2);
                fVar17 = len;
                if (len != kZero) {
                    const float inv = kOne / len;
                    const float a0 = inv * pf[8], a1 = inv * pf[9], a2 = inv * pf[10];
                    const float t = ((a1 * pf[0xc] + a0 * pf[0xb]) + a2 * pf[0xd]) * len;
                    pf[8]  = t * pf[0xb];
                    pf[9]  = t * pf[0xc];
                    pf[10] = t * pf[0xd];
                }
            } else if (self[0x278] == 0x40800000) {     // all 4 grounded
                float fc = A5SuspScale() * pf[6] * kHalf;
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
            } else if (A5AirF32()) {
                const float f = (pf[6] / dt) * A5SuspScale();
                pf[8]  = f * pf[3];
                pf[9]  = f * pf[4];
                pf[10] = f * pf[5];
                fVar17 = f;
            } else {
                fVar17 = ((double)pf[6] / (double)dt) * (double)A5SuspScale();
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
                vF(self, 0x2c6) = vF(self, 0x2c6) - A5SuspDtTerm() * vF(self, 0x14) * k3000;
            }
            return;
        }
    } while (true);
}


}  // namespace Vehicle
}  // namespace mashed_re
