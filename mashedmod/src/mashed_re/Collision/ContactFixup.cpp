// Mashed RE — U-9154: FUN_0046ef70, the POST-CONTACT FIXUP.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (every instruction below cited from `py -3.12 re/tools/disasm_fn.py 0x0046ef70 0x0046f6c0`
// over `original/MASHED.exe.unpatched`; 485 instructions, RET at 0x0046f6bf).
// Ghidra decomp cross-read from a read-only pool slot (`re/tools/decomp_pc.py 0x0046ef70`).
//
// WHAT IT IS. This is the third link of the original's per-substep car<->world chain
// (`FUN_004709a0`, 0x00470ab0..0x00470b0b):
//     0x00470ad3  call 0x0046e9e0   A9 integrate
//     0x00470ae0  call 0x0046f6c0   wheel contact solver
//     0x00470ae8  call 0x00469aa0   contact scan  -> writes the 18 slots + [rec+0x9ec]
//     0x00470aef  mov eax,[esi+0x9ec] / test / je     <- gate
//     0x00470afe  call 0x0046ef70   THIS FUNCTION
// It reduces the 18 contact slots at rec+0x4a8 (stride 0x40) into ONE corrective
// linear velocity written over rec+0x9b0/9b4/9b8 and ONE angular impulse added into
// rec+0x144/148/14c. That write is the wall bounce: on the original's Training solo
// capture `orig_solo3.msd` frame 980->981 it flips vel.x -1716.69 -> +176.47.
//
// ABI. The original takes the vehicle record in EDI and ONE stack argument (the
// +0x928 RwMatrix, pushed at 0x00470afd). A balanced-ESP walk of the whole body
// (`sub esp,0x80` @0x0046ef70 + 3 pushes; the four `add esp,N` are call cleanups;
// epilogue `pop esi/ebp/ebx` + `add esp,0x80` @0x0046f6b9, delta 0) finds NO read of
// [esp+0x90] — the caller's arg1 slot — anywhere in the function. **The matrix
// argument is dead inside 0x0046ef70**, exactly like A4's param_4 inside A5
// (`re/analysis/D2_REOPEN_2026-09-29.md` §4.3). The port therefore takes only the
// record, and the caller does not have to supply a matrix.
//
// FLOAT MODEL, stated. The decompiler shows `float10` (x87 80-bit) intermediates
// around FUN_004c3ac0 / FUN_004c39b0; this TU compiles under the project default
// /arch:SSE2 and holds those chains in `double`. Same residual class as
// `ContactDeps.h:32-34` (FastSqrt) — not bit-identical, and this function is not a
// bit-identity gate. Adding the TU to `mashedmod/x87_tus.txt` is a MEASURED decision
// per that file's own rule, not taken here.
//
// C-LEVEL: C2 faithful transcription (no behavioural diff yet).
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"
#include <cstdio>
#include <cstdlib>
#include "../Vehicle/D2SinkProbe.h"   // [D2 attempt 19] diagnostic, default-OFF

namespace mashed_re {
namespace Collision {

// Constants — raw value read from the anchored binary at the cited address, and the
// instruction that loads it.
static const float kFx_NegOne   = -1.0f;    // _DAT_005cc33c  0x0046f02c / 0x0046f050
static const float kFx_NyThresh =  0.9f;    // _DAT_005cc9c8  0x0046f034 (also the +0x9b0 clamp @0x0046f5c0)
static const float kFx_WheelAmp =  4.0f;    // _DAT_005cc35c  0x0046f041
static const float kFx_HullAmp  = -0.5f;    // _DAT_005cd50c  0x0046f058
static const float kFx_Zero     =  0.0f;    // DAT_005d757c   0x0046f067 / 0x0046f44c
static const float kFx_One      =  1.0f;    // _DAT_005cc320  0x0046f076 / 0x0046f3d8
static const float kFx_Floor500 =  500.0f;  // _DAT_005ccd04  0x0046f0a3 / 0x0046f11d
static const float kFx_Scale075 =  0.75f;   // _DAT_005cc950  0x0046f0ba
static const float kFx_Tan90    =  90.0f;   // _DAT_005ccad0  0x0046f13e
static const float kFx_Torque   =  50.24f;  // _DAT_005cea04  0x0046f338
static const float kFx_Eps1e4   =  1.0e-4f; // _DAT_005cd03c  0x0046f354 / 0x0046f610
static const float kFx_MassDt   =  0.0027778f; // _DAT_005cea80 0x0046f484  (= 1/360)
static const float kFx_Eight    =  8.0f;    // _DAT_005cc9f4  0x0046f495
static const float kFx_AngMin   =  0.25f;   // _DAT_005cc564  0x0046f4d4
static const float kFx_AngCap   =  50.0f;   // _DAT_005cd120  0x0046f4eb
static const float kFx_Half     =  0.5f;    // _DAT_005cc32c  0x0046f526
static const float kFx_Three    =  3.0f;    // _DAT_005cc31c  0x0046f5ba
static const float kFx_Slide    =  0.1f;    // _DAT_005cc56c  0x0046f658
static const int   kFx_Grounded4 = 0x40800000;  // 4.0f as an int cmp @0x0046f5f3

// [U-9156] diagnostic sink, resolved once; nullptr (and so inert) when unset.
static const char* FixupLogPath() {
    static const char* s_p = std::getenv("MASHED_FIXUP_LOG");
    return s_p;
}

// Record field views (byte offsets; the record base is the 0xd04 struct).
static inline float& Rf(int* v, int byteOff) {
    return *reinterpret_cast<float*>(reinterpret_cast<char*>(v) + byteOff);
}
static inline int& Ri(int* v, int byteOff) {
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(v) + byteOff);
}

// ---------------------------------------------------------------------------
// 0x0046ef70  VehicleContactFixup
//
// Slot layout (18 slots, byte base S = 0x4a8 + i*0x40 — the array 0x00469aa0 scans
// at rec+0x4ac and VehicleTerrainContactSolver fills):
//     S+0x00 (0x4a8)  penetration depth      (terrain solver pfVar8[-6])
//     S+0x04 (0x4ac)  slot key, int, -1 = empty  (cmp [esi-8],-1 @0x0046f01a)
//     S+0x08..0x10    contact normal x/y/z   (pfVar7[-1], [0], [1])
//     S+0x14 (0x4bc)  per-slot scale         (pfVar7[2])
//     S+0x2c..0x34    contact arm            (pfVar7[8],[9],[10])
//     S+0x38 (0x4e0)  impulse magnitude      (pfVar7[0xb])
// ---------------------------------------------------------------------------
void VehicleContactFixup(int* self)
{
    // 0x0046ef76..0x0046f009: cVar8 = number of wheels whose state != 0.
    int cVar8 = (Ri(self, 0x198) != 0) ? 1 : 0;
    if (Ri(self, 0x25c) != 0) ++cVar8;
    if (Ri(self, 0x320) != 0) ++cVar8;   // 800 decimal in the decomp
    if (Ri(self, 0x3e4) != 0) ++cVar8;

    // Accumulators. Contiguous stack vec3s in the original (E-0x54/-0x60/-0x6c).
    float local_54[3] = { 0.0f, 0.0f, 0.0f };   // "sum A" -> the linear velocity write
    float local_60[3] = { 0.0f, 0.0f, 0.0f };   // "sum B" -> the slide-direction term
    float local_6c[3] = { 0.0f, 0.0f, 0.0f };   // torque accumulator -> +0x144
    float local_78    = 0.0f;                   // scalar arm-impulse accumulator
    float local_74    = 1.0f;                   // 0x0046efd3: 0x3f800000

    // local_48/44/40: written inside the `3 < i` arm AND re-written after the loop.
    // The original leaves them as stack garbage if neither writer runs; the caller
    // gates on [rec+0x9ec] != 0 so at least one slot is live, and the +0x9e0 == 4.0
    // consumer below is the only reader. Zero-initialised here; stated, not hidden.
    float local_48[3] = { 0.0f, 0.0f, 0.0f };

    float* pfVar7 = nullptr;                    // survives the loop (last-slot guard)
    for (int i = 0; i < 0x12; ++i) {            // 0x0046f3c4: cmp ebp,0x12
        char* slotB = reinterpret_cast<char*>(self) + 0x4b4 + i * 0x40;   // 0x0046f00d/0x0046f3c1
        pfVar7 = reinterpret_cast<float*>(slotB);
        if (*reinterpret_cast<int*>(slotB - 8) == -1) continue;           // 0x0046f01a

        float local_34 = pfVar7[0xb];                                     // 0x0046f027
        if (i < 4) {                                                      // 0x0046f024
            local_34 = local_34 * kFx_NegOne;                             // 0x0046f02c
            if (pfVar7[0] < kFx_NyThresh) local_34 = local_34 * kFx_WheelAmp;  // 0x0046f034/41
        } else if (cVar8 == 4) {                                          // 0x0046f049
            local_34 = local_34 * kFx_NegOne;                             // 0x0046f050
        } else {
            local_34 = local_34 * kFx_HullAmp;                            // 0x0046f058
        }

        // 0x0046f05e..0x0046f08e: last-contact damp factor from magnitude / speed.
        float fVar2 = pfVar7[0xb] / Rf(self, 0x9e4);
        if (fVar2 < kFx_Zero) {
            fVar2 = -fVar2;                                               // 0x0046f074 fchs
            if (kFx_One < fVar2) fVar2 = kFx_One;
            local_74 = kFx_One - fVar2;
        }

        if (Ri(self, 0xcfc) != 0 && local_34 < kFx_Floor500) local_34 = kFx_Floor500;  // 0x0046f097/a3
        local_34 = local_34 * kFx_Scale075;                               // 0x0046f0ba

        float local_3c[3];
        local_3c[0] = local_34 * pfVar7[-1];
        local_3c[1] = local_34 * pfVar7[0];
        local_3c[2] = local_34 * pfVar7[1];

        float sc, local_44v, local_40v;
        if (Ri(self, 0xcfc) == 0) {                                       // 0x0046f13e
            local_44v = pfVar7[0] * kFx_Tan90;
            local_40v = pfVar7[1] * kFx_Tan90;
            sc        = kFx_Tan90;
        } else {                                                          // 0x0046f11d
            local_44v = pfVar7[0] * kFx_Floor500;
            local_40v = pfVar7[1] * kFx_Floor500;
            sc        = kFx_Floor500;
        }
        const float local_48v = pfVar7[-1] * sc;

        local_54[0] += local_48v + local_3c[0];
        local_54[1] += local_44v + local_3c[1];
        local_54[2] += local_40v + local_3c[2];
        local_60[0] += local_48v + local_3c[0];
        local_60[1] += local_44v + local_3c[1];
        local_60[2] += local_40v + local_3c[2];

        if (3 < i) {   // hull slots only (0x0046f1ae region)
            float local_24[3];
            Vec3Normalize(local_24, pfVar7 + 8);                          // FUN_004c39b0 @0x0046f1bb
            const double dotv = (double)local_24[0] * pfVar7[-1]
                              + (double)local_24[1] * pfVar7[0]
                              + (double)local_24[2] * pfVar7[1];
            double fVar9 = (double)Vec3Mag(local_3c);                     // FUN_004c3ac0 @0x0046f1e3
            fVar9 = fVar9 * dotv;
            float local_c[3];
            local_c[0] = (float)((double)local_24[0] * fVar9);
            local_c[1] = (float)((double)local_24[1] * fVar9);
            local_c[2] = (float)((double)local_24[2] * fVar9);

            float local_18[3];
            local_18[0] = local_3c[0] - local_c[0];
            local_18[1] = local_3c[1] - local_c[1];
            local_18[2] = local_3c[2] - local_c[2];

            float local_30[3];
            local_30[0] = local_18[2] * local_c[1] - local_18[1] * local_c[2];
            local_30[1] = local_18[0] * local_c[2] - local_18[2] * local_c[0];
            local_30[2] = local_18[1] * local_c[0] - local_18[0] * local_c[1];
            if (kFx_Zero < pfVar7[10] * local_3c[2] + local_3c[0] * pfVar7[8]
                         + pfVar7[9] * local_3c[1]) {
                local_30[0] = -local_30[0];
                local_30[1] = -local_30[1];
                local_30[2] = -local_30[2];
            }

            local_54[0] += local_c[0];
            local_54[1] += local_c[1];
            local_54[2] += local_c[2];
            local_60[0] += local_3c[0];
            local_60[1] += local_3c[1];
            local_60[2] += local_3c[2];

            const float sl = pfVar7[2];
            float arm[3] = { local_18[0] * sl, local_18[1] * sl, local_18[2] * sl };
            const float armMag = (float)((double)Vec3Mag(arm) * (double)kFx_Torque);  // 0x0046f333/338
            local_78 += armMag;
            if (kFx_Eps1e4 < Vec3Mag(local_30)) {                         // 0x0046f34f/354
                Vec3Normalize(local_30, local_30);                        // 0x0046f36e
                local_6c[0] -= local_30[0] * armMag;
                local_6c[1] -= local_30[1] * armMag;
                local_6c[2] -= local_30[2] * armMag;
            }
        }
    }

    // 0x0046f3cd: fild [edi+0x9ec] — INTEGER active-contact count -> float, reciprocal.
    const float inv = kFx_One / (float)Ri(self, 0x9ec);
    for (int k = 0; k < 3; ++k) { local_60[k] *= inv; local_54[k] *= inv; local_6c[k] *= inv; }

    // 0x0046f438..0x0046f479
    if ((inv * local_78) - Vec3Mag(local_6c) != kFx_Zero) {
        local_48[0] = local_60[0] - local_54[0];
        local_48[1] = local_60[1] - local_54[1];
        local_48[2] = local_60[2] - local_54[2];
    }

    float fVar2 = Rf(self, 0x5c) * kFx_MassDt;      // 0x0046f47d/484
    Ri(self, 0xb20) = 1;                            // 0x0046f48b — "contact resolved this tick"
    fVar2 = fVar2 * kFx_Eight;                      // 0x0046f495
    for (int k = 0; k < 3; ++k) local_6c[k] *= fVar2;
    {
        const float m = Vec3Mag(local_6c);          // 0x0046f4b7
        if (m != kFx_Zero && m < kFx_AngMin) {      // 0x0046f4d4
            float s = kFx_AngMin / m;
            if (kFx_AngCap < s) s = kFx_AngCap;     // 0x0046f4eb
            for (int k = 0; k < 3; ++k) local_6c[k] *= s;
        }
    }
    for (int k = 0; k < 3; ++k) local_6c[k] *= kFx_Half;   // 0x0046f526

    // [U-9156 2026-09-30] MASHED_FIXUP_LOG=<relative path> -> the reduced torque
    // local_6c and the accumulator before/after, DIAGNOSTIC ONLY, default-OFF.
    // Why it exists: the accumulator's YAW component also receives the steer torque and
    // the `(w + accum) * damp` update inside BodyOrient_OmegaFromSteer in the same
    // frame, so d(+0x148) across a contact frame CANNOT be read as the contact torque.
    // Its ROLL component can (the steer axis is (0,1,0), so w.z == 0 and the port's
    // pre-contact +0x14c is exactly 0). This log removes the inference on both.
    if (const char* fl = FixupLogPath()) {
        if (std::FILE* f = std::fopen(fl, "a")) {
            std::fprintf(f, "t6c=(%.9g,%.9g,%.9g) acc=(%.9g,%.9g,%.9g) cnt=%d sp=%.9g\n",
                         local_6c[0], local_6c[1], local_6c[2],
                         Rf(self, 0x144), Rf(self, 0x148), Rf(self, 0x14c),
                         Ri(self, 0x9ec), Rf(self, 0x9e4));
            std::fclose(f);
        }
    }
    Rf(self, 0x144) += local_6c[0];                 // 0x0046f552/558
    Rf(self, 0x148) += local_6c[1];
    Rf(self, 0x14c) += local_6c[2];

    // THE BOUNCE: rec+0x9b0/9b4/9b8 (linear velocity) += the reduced contact impulse.
    float* vel = reinterpret_cast<float*>(reinterpret_cast<char*>(self) + 0x9b0);  // 0x0046f52c
    const float v0 = vel[0];
    vel[0] = local_54[0] + v0;
    const float v1 = local_54[1] + vel[1]; vel[1] = v1;
    const float v2 = local_54[2] + vel[2]; vel[2] = v2;
    // [D2 attempt 19] the three anchored velocity writes of 0x0046ef70, each sampled at
    // its own exit, so T_post's substep share splits by write. Diagnostic, default-OFF.
    D2Sink::NoteFixup();
    D2Sink::Mark("fx_bounce", self, -1, 0, 0.f, 0.f);   // 0x0046f52c

    // LAST-CONTACT DAMP. [U-9156 2026-09-30] CORRECTED — the guard used to read
    // slot 17's key against -1 and so never fired; the original reads SLOT 0's key
    // against -2 and so almost always fires. Both errors skipped the damp, which is
    // why the port bounced off Training's wall at 4x the original's outgoing speed
    // and never got away from it (re/analysis/D2_REOPEN_2026-09-29.md §17.3).
    //
    //   0046f00d  lea esi,[edi+0x4b4]     ; once, BEFORE the loop
    //   0046f013  lea eax,[esi-0xc]       ; EAX = rec+0x4a8 = SLOT 0's base
    //   0046f016  mov [esp+0x1c],eax      ; = [S+0x1c], written once, never rewritten
    //   0046f522  mov eax,[esp+0x1c]      ; ESP is back at S here
    //   0046f5ae  mov ecx,[eax+4]         ; = [rec+0x4ac] = SLOT 0's key
    //   0046f5b1  cmp ecx,-2
    //   0046f5b4  je  0x46f5f3            ; skip the damp ONLY on -2
    //
    // The same displacement `[esp+0x1c]` at 0x0046f347/0x0046f34b is a DIFFERENT
    // slot: two pushes are live there (0x0046f2cd, 0x0046f342), so it resolves to
    // [S+0x14] = local_78. Balanced-ESP walk in §17.3.
    // [UNCERTAIN U-9156] what the -2 sentinel means; its writer is not identified.
    // The transcription does not depend on knowing.
    (void)pfVar7;
    if (Ri(self, 0x4ac) != -2) {
        float fVar5 = local_74 * kFx_Three;                    // 0x0046f5ba
        if (kFx_NyThresh < fVar5) fVar5 = kFx_NyThresh;        // 0x0046f5c0
        // [D2 section 22.2] the damp factor itself, on the same MASHED_FIXUP_LOG sink.
        // The line above is printed BEFORE the damp, so it cannot witness it; and the
        // damp is the whole of section 22.1's first diverging term (C4 at d = 0, port
        // 251.48 vs original 219.89). It has a KNEE: local_74 = 1 - min(1, |m|/+0x9e4),
        // so |m|/+0x9e4 <= 0.7 saturates fVar5 at the 0.9 cap and keeps 90%, while above
        // the knee retention falls as 3*(1 - |m|/+0x9e4). The ORIGINAL is on that cap on
        // 15 of its 23 fixups; the port on 0 of 211.
        if (const char* fl = FixupLogPath()) {
            if (std::FILE* f = std::fopen(fl, "a")) {
                std::fprintf(f, "   damp l74=%.9g damp=%.9g cap=%d key0=%d "
                                "pre=(%.9g,%.9g,%.9g) preDamp=(%.9g,%.9g,%.9g)\n",
                             local_74, fVar5, (fVar5 >= kFx_NyThresh) ? 1 : 0,
                             Ri(self, 0x4ac), v0, vel[1] - local_54[1], vel[2] - local_54[2],
                             local_54[0] + v0, v1, v2);
                std::fclose(f);
            }
        }
        vel[0] = (local_54[0] + v0) * fVar5;
        vel[1] = v1 * fVar5;
        vel[2] = v2 * fVar5;
    }
    D2Sink::Mark("fx_damp", self, -1, 0, 0.f, 0.f);     // 0x0046f5ba / 0x0046f5c0

    // 0x0046f5f3: only when all four wheels are grounded (+0x9e0 == 4.0f as an int).
    if (Ri(self, 0x9e0) == kFx_Grounded4) {
        const float sp = Vec3Mag(vel);                          // 0x0046f607 (kept @[esp+0x1c])
        if (kFx_Eps1e4 < sp) {                                  // 0x0046f610
            // 0x0046f624 `lea ecx,[esp+0x44]` = local_48 — the normalize writes OVER
            // local_48/44/40, so the three factors below are the NORMALISED velocity,
            // not the local_60-local_54 difference stored there earlier. That earlier
            // store (0x0046f459..0x0046f479) therefore has NO reader in the original;
            // it is kept above verbatim rather than deleted.
            Vec3Normalize(local_48, vel);                       // 0x0046f62a
            const float k = (local_48[2] * Rf(self, 0x9d0)      // 0x0046f650
                           + local_48[0] * Rf(self, 0x9c8)      // 0x0046f644
                           + local_48[1] * Rf(self, 0x9cc))     // 0x0046f639
                          * kFx_Slide * sp;                     // 0x0046f658 / 0x0046f65e
            vel[0] -= k * Rf(self, 0x9c8);
            vel[1] -= k * Rf(self, 0x9cc);
            vel[2] -= k * Rf(self, 0x9d0);
            (void)Vec3Mag(local_54);    // 0x0046f6ac: result discarded by the original
        }
    }
    D2Sink::Mark("fx_slide", self, -1, 0, 0.f, 0.f);    // 0x0046f5f3
}

}  // namespace Collision
}  // namespace mashed_re
