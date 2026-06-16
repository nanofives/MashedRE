// Mashed RE — WS-B2: car<->world per-wheel contact query (terrain + object).
//
// VERBATIM port of the original contact solvers. Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Decompiled this session from Ghidra pool3 (read_only). Map + evidence:
//   re/analysis/WSB2_B3_CONTACT_PORT_MAP.md
//
// Functions ported (B2 set per COLLISION_GATE_BRIEF / ratified Option B):
//   0x0046c5f0  TriangleFaceNormal            (leaf; FUN_004c3b30 only)
//   0x00468b40  ContactHistoryLookup          (32-slot key scan @veh+0xbfc)
//   0x0046cc40  WheelTerrainContactClassifier (approaching-contact classify)
//   0x00468d80  VehicleTerrainContactSolver   (deep-penetration impulse)
//   0x004694e0  VehicleObjectContactSolver    (dynamic-object impulse)
//   0x00469aa0  VehicleContactHistoryUpdate   (history shift + dispatch + scan)
//
// FIDELITY NOTES (see the port map for the full ledger):
//  * The vehicle record is the original 0xd04 struct, passed by register (ESI/
//    EDI) in the original; here it is an explicit `int* v` (== DAT_008815a0 +
//    car*0xd04). vF()/vFP() reinterpret a dword as a float, reproducing the
//    decompiler's `(float)param_1[i]` bit view.
//  * Dev-only debug-event ring enqueues (DAT_007e9de0..DAT_007ec9e4, gated by
//    game-mode and param_1[0x27c]) are GAMEPLAY-INERT and omitted; their RVA is
//    cited at each site. The control flow that gates a *contact write* is kept.
//  * The contact geometry source (DAT_0088e60c batch) is produced by the RW
//    broadphase walk FUN_00538c80 + callback LAB_00468b80 (in the wheel solver,
//    runs before this dispatcher). That producer is the B4/A wiring; here the
//    batch base is g_terrainBatch.
//  * C-LEVEL: C2 faithful transcription. NOT diff-original-verified (these are
//    state-readers, not isolatable leaves); the gate is an installed-hook
//    scenario trace once the WS-A consumer (FUN_0046ddb0) drives them.
#include "ContactConstants.h"
#include "ContactDeps.h"

namespace mashed_re {
namespace Collision {

// Contact-batch sentinel field values (entry float [0xd] / [0xb]); they are the
// debug-ring colour dwords reinterpreted as floats and gate the debug class.
//   -1.7147562e+38 == 0xff010101  (first-frame "white" sentinel)
//   -1.7014636e+38 == 0xff0000ff  (persistent "blue" sentinel)
static const float kSentFirstFrame = asF(0xff010101);  // -1.7147562e+38
static const float kSentPersistent = asF(0xff0000ff);  // -1.7014636e+38

// ---------------------------------------------------------------------------
// 0x0046c5f0  TriangleFaceNormal
//   Original: __fastcall, EAX=apex, ESI=out-normal, stack=p1,p2. Unit normal of
//   the triangle (apex,p1,p2) via cross product; guarded against degeneracy.
//   Numerator _DAT_005cc320 = 1.0f @0x005cc320. FastSqrt = FUN_004c3b30.
// ---------------------------------------------------------------------------
void TriangleFaceNormal(const float* apex, const float* p1, const float* p2, float* outN)
{
    float fVar2 = (apex[2] - p1[2]) * (apex[1] - p2[1]) - (apex[1] - p1[1]) * (apex[2] - p2[2]);
    float fVar3 = (apex[0] - p1[0]) * (apex[2] - p2[2]) - (apex[2] - p1[2]) * (apex[0] - p2[0]);
    float fVar1 = (apex[1] - p1[1]) * (apex[0] - p2[0]) - (apex[0] - p1[0]) * (apex[1] - p2[1]);
    float fVar4 = fVar2 * fVar2 + fVar3 * fVar3 + fVar1 * fVar1;
    if (kZero <= fVar4) {
        float len = FastSqrt(fVar4);
        if (len != kZero) {
            float inv = kOne / len;        // _DAT_005cc320 / len
            outN[0] = fVar2 * inv;
            outN[1] = fVar3 * inv;
            outN[2] = fVar1 * inv;
        }
    }
}

// ---------------------------------------------------------------------------
// 0x00468b40  ContactHistoryLookup
//   Scan up to 32 history slots at veh+0xbfc; slot is a parallel array: key at
//   [i], active flag at [i+0x20]. Match key == *(int*)(entry+0x34).
//   Returns 1 if a matching active slot exists, else 0.
// ---------------------------------------------------------------------------
int ContactHistoryLookup(const void* entry, int* veh)
{
    int  key    = *reinterpret_cast<const int*>(reinterpret_cast<const char*>(entry) + 0x34);
    int* piVar1 = reinterpret_cast<int*>(reinterpret_cast<char*>(veh) + 0xbfc);
    int  iVar2  = 0;
    while (piVar1[0x20] == 0 || key != *piVar1) {
        iVar2 += 1;
        piVar1 += 1;
        if (0x1f < iVar2) return 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// 0x0046cc40  WheelTerrainContactClassifier
//   Iterate the DAT_0088e60c terrain batch; for each of 4 wheels not already
//   flagged (g_wheelSkipFlags), run the 3 SAT half-plane tests + depth band
//   [_DAT_005cea5c, _DAT_005cc564) + approach-speed test, and on a NEW contact
//   (ContactHistoryLookup==0) fill the per-wheel contact entry at veh+0x65.
//   param_2 = batch base (float*). Early-out when 4 contacts filled.
// ---------------------------------------------------------------------------
void WheelTerrainContactClassifier(int* param_1, float* param_2)
{
    if (g_terrainEntryCount == 0) return;
    unsigned int local_74 = 0;
    do {
        float fVar2  = param_2[0];   float fVar3  = param_2[1];   float fVar4  = param_2[2];
        float fVar5  = param_2[3];   float fVar6  = param_2[4];   float fVar7  = param_2[5];
        float fVar8  = param_2[6];   float fVar9  = param_2[7];   float fVar10 = param_2[8];
        float fVar11 = param_2[9];   float fVar12 = param_2[10];  float fVar13 = param_2[0xb];

        float* local_90 = vFP(param_1, 0x65);      // per-wheel contact entry base
        int*   local_88 = g_wheelSkipFlags;        // DAT_0088e5e0
        float* local_8c = g_wheelContactPos;       // DAT_0088e624 (4 x vec3)
        for (int w = 0; w < 4; ++w) {
            if (*local_88 == 0) {
                float fVar14 = local_8c[-1];       // wheel pos x (local_8c starts at +1 of slot)
                float fVar15 = local_8c[0];        // wheel pos y
                float fVar16 = local_8c[1];        // wheel pos z
                if ((kZero <= (fVar14 - fVar2) * param_2[0x1b] + (fVar15 - fVar3) * param_2[0x1c] + (fVar16 - fVar4) * param_2[0x1d]) &&
                    (kZero <= (fVar14 - fVar5) * param_2[0x1e] + (fVar15 - fVar6) * param_2[0x1f] + (fVar16 - fVar7) * param_2[0x20])) {
                    if (kZero <= (fVar14 - fVar8) * param_2[0x21] + (fVar15 - fVar9) * param_2[0x22] + (fVar16 - fVar10) * param_2[0x23]) {
                        float depth = (fVar14 - fVar8) * fVar11 + (fVar15 - fVar9) * fVar12 + (fVar16 - fVar10) * fVar13;
                        if ((kCls_DepthLo <= depth && depth < kCls_DepthHi) &&
                            (kCls_ApproachThr < fVar11 * vF(param_1, 0x272) + fVar12 * vF(param_1, 0x273) + fVar13 * vF(param_1, 0x274))) {
                            float sentinel = param_2[0xd];
                            if (sentinel == kSentFirstFrame) {
                                // [0x0046cd.. debug-event ring (type 6, white) — dev-viz only; omitted]
                            } else {
                                int found = ContactHistoryLookup(param_2, param_1);
                                if (found == 0) {
                                    local_90[0x1b] = param_2[9];
                                    local_90[0x1c] = param_2[10];
                                    local_90[0x1d] = param_2[0xb];
                                    local_90[0]    = depth;
                                    local_90[0x16] = param_2[0xc];
                                    local_90[0x17] = param_2[0xd];
                                    // [0x0046d0.. / 0x0046d1.. debug-event ring (type 6, blue) — dev-viz only; omitted]
                                    *local_88 = 1;
                                    g_activeContactCount += 1;
                                    if (g_activeContactCount == 4) return;
                                }
                            }
                        }
                    }
                }
            }
            local_8c += 3;
            local_90 += 0x31;
            local_88 += 1;
        }
        local_74 += 1;
        param_2 += 0x24;
    } while (local_74 < (unsigned int)g_terrainEntryCount);
}

// ---------------------------------------------------------------------------
// 0x00468d80  VehicleTerrainContactSolver
//   For each terrain batch entry under the depth threshold _DAT_005cea48, run 18
//   contact slots (slots 0..3 only if their wheel-active flag is 0) through the
//   SAT half-plane tests and, on a deep enough penetration (< _DAT_005cd0fc),
//   build the corrective velocity into the contact record at veh+0x130 and (for
//   a NEW persistent contact) latch the contact normal/depth.
//   param_2 = batch base (byte address; first entry data at +8).
// ---------------------------------------------------------------------------
void VehicleTerrainContactSolver(int* param_1, void* param_2)
{
    int* local_50 = param_1 + param_1[0x26b] * 0x10 + 0x24a;     // wheel-ring matrix block
    if (g_terrainEntryCount == 0) return;

    float* pfVar7 = reinterpret_cast<float*>(reinterpret_cast<char*>(param_2) + 8);
    unsigned int local_78 = 0;
    do {
        float local_48 = pfVar7[-2], local_70 = pfVar7[-1], local_54 = pfVar7[0];
        float local_4c = pfVar7[2],  local_58 = pfVar7[4],  local_68 = pfVar7[6];
        float local_6c = pfVar7[1],  local_74 = pfVar7[8],  local_5c = pfVar7[3];
        float local_60 = pfVar7[5],  local_44 = pfVar7[7],  local_64 = pfVar7[9];

        if (local_74 < kTer_DepthThresh) {
            float* local_98 = vFP(param_1, 0x27f);
            int*   local_80 = param_1 + 0x66;
            int    local_7c = 0;
            float* pfVar8   = vFP(param_1, 0x130);
            float* local_9c = pfVar7;
            do {
                if ((3 < local_7c) || (*local_80 == 0)) {
                    float local_8c = local_98[-1];
                    float local_88 = local_98[0];
                    float local_84 = local_98[1];
                    float local_94 = (local_8c - local_48) * local_44 +
                                     (local_88 - local_70) * local_74 +
                                     local_64 * (local_84 - local_54);
                    if ((local_94 < kZero) &&
                        ((kZero <= (local_84 - local_54) * pfVar7[0x1b] + (local_8c - local_48) * pfVar7[0x19] + (local_88 - local_70) * pfVar7[0x1a] &&
                          kZero <= (local_8c - local_6c) * pfVar7[0x1c] + (local_88 - local_4c) * pfVar7[0x1d] + (local_84 - local_5c) * pfVar7[0x1e]) &&
                         kZero <= (local_8c - local_58) * pfVar7[0x1f] + (local_88 - local_60) * pfVar7[0x20] + (local_84 - local_68) * pfVar7[0x21]) &&
                        (kTer_DepthMin <= local_94)) {
                        pfVar8[3] = -local_94;
                        // local_a8/a4/a0 are a contiguous stack vec3 in the original.
                        float local_a[3];
                        local_a[0] = (local_98[-0x267] - vF(param_1, 0x57)) * kArm_SandSpWheel;
                        local_a[1] = (local_98[-0x266] - vF(param_1, 0x58)) * kArm_SandSpWheel;
                        local_a[2] = (local_98[-0x265] - vF(param_1, 0x59)) * kArm_SandSpWheel;
                        Rw_TransformPoints(pfVar8 + 5, local_a, 1, local_50);
                        if ((vF(param_1, 0x26f) == kZero) && (vF(param_1, 0x270) == kZero) && (vF(param_1, 0x271) == kZero)) {
                            local_a[2] = 0.0f; local_a[1] = 0.0f; local_a[0] = 0.0f;
                        } else {
                            unsigned char local_40[64];
                            Rw_MatrixFromAxisAngle(local_40, vFP(param_1, 0x26f), kAxisAngle90, 0);
                            Rw_TransformPoints(local_a, pfVar8 + 5, 1, local_40);
                            Vec3Normalize(local_a, local_a);
                            double fVar9 = Vec3Mag(vFP(param_1, 0x26f));
                            fVar9 = fVar9 * (double)kTer_VelScale * (double)pfVar8[-1] * (double)kTer_VelScale2;
                            local_a[0] = (float)((double)local_a[0] * fVar9);
                            local_a[1] = (float)((double)local_a[1] * fVar9);
                            local_a[2] = (float)((double)local_a[2] * fVar9);
                        }
                        pfVar8[0] = vF(param_1, 0x26c) + local_a[0];
                        pfVar8[1] = vF(param_1, 0x26d) + local_a[1];
                        pfVar8[2] = vF(param_1, 0x26e) + local_a[2];
                        float local_90 = Vec3Mag(pfVar8);
                        pfVar7 = local_9c;
                        if (kTer_MagThresh < local_90) {
                            Vec3Normalize(pfVar8, pfVar8);
                            float* pfVar4 = local_9c;
                            float fVar2 = local_9c[9] * pfVar8[2] + pfVar8[0] * local_9c[7] + pfVar8[1] * local_9c[8];
                            pfVar7 = local_9c;
                            if (fVar2 < kZero) {
                                if (fVar2 < kCC_ImpulseLoClamp) fVar2 = kCC_ImpulseLoClamp;
                                local_90 = local_90 * fVar2;
                                if (local_9c[0xb] == kSentFirstFrame) {
                                    // [0x004690.. debug-event ring (type 6, white) — dev-viz only; omitted]
                                } else {
                                    // [0x004691.. optional debug-event (type 6, blue) — dev-viz only; omitted]
                                    int iVar5 = ContactHistoryLookup(pfVar4 - 2, param_1);
                                    pfVar7 = local_9c;
                                    if (iVar5 == 0) {
                                        pfVar8[8]  = local_90;
                                        pfVar8[-4] = pfVar4[7];
                                        pfVar8[-3] = pfVar4[8];
                                        pfVar8[-2] = pfVar4[9];
                                        pfVar8[-6] = local_94;
                                        pfVar8[-5] = pfVar4[10];
                                    }
                                }
                            }
                        }
                    }
                }
                local_7c += 1;
                local_80 += 0x31;
                local_98 += 3;
                pfVar8  += 0x10;
            } while (local_7c < 0x12);
        }
        local_78 += 1;
        pfVar7  += 0x24;
    } while (local_78 < (unsigned int)g_terrainEntryCount);
}

// ---------------------------------------------------------------------------
// 0x004694e0  VehicleObjectContactSolver
//   Same SAT structure against the dynamic-object geometry list (Obj_List*);
//   entry filtered by owning-vehicle id (entry[0xe] == veh id). On contact,
//   writes the corrective velocity into veh+0x130 and exchanges momentum with
//   the object entry (entry[0x13..0x15] += ; veh velocity += scaled normal).
// ---------------------------------------------------------------------------
void VehicleObjectContactSolver(int* param_1)
{
    int* local_10 = param_1 + param_1[0x26b] * 0x10 + 0x24a;
    float* pfVar6 = Obj_ListBase();
    unsigned int local_14 = 0;
    int iVar7 = Obj_ListCount();
    if (iVar7 == 0) return;
    do {
        if (pfVar6[0xe] == vF(param_1, 0)) {
            float local_50 = pfVar6[0],  local_58 = pfVar6[1],  local_48 = pfVar6[2];
            float local_60 = pfVar6[3],  local_38 = pfVar6[4],  local_3c = pfVar6[5];
            float local_64 = pfVar6[6],  local_44 = pfVar6[7],  local_4c = pfVar6[8];
            float fVar3   = pfVar6[9],   fVar4   = pfVar6[10],  fVar5   = pfVar6[0xb];

            float* local_5c = vFP(param_1, 0x66);  // wheel active flags (as float* for ==0 test)
            int    local_18 = 0;
            float* pfVar9  = vFP(param_1, 0x130);
            float* pfVar11 = vFP(param_1, 0x280);
            do {
                if ((3 < local_18) || (*local_5c == 0.0f)) {
                    float local_54 = pfVar11[-2];
                    float local_68 = pfVar11[-1];
                    float local_40 = pfVar11[0];
                    float fVar1 = (local_40 - local_48) * fVar5 + (local_68 - local_58) * fVar4 + fVar3 * (local_54 - local_50);
                    if (fVar1 < kZero) {
                        float local_34 = fVar4 * (local_3c - local_48) - (local_38 - local_58) * fVar5;
                        float local_30 = (local_60 - local_50) * fVar5 - (local_3c - local_48) * fVar3;
                        if (kZero <= (local_68 - local_58) * local_30 + (local_54 - local_50) * local_34 +
                                     (local_40 - local_48) * ((local_38 - local_58) * fVar3 - (local_60 - local_50) * fVar4)) {
                            local_34 = fVar4 * (local_4c - local_3c) - (local_44 - local_38) * fVar5;
                            local_30 = (local_64 - local_60) * fVar5 - (local_4c - local_3c) * fVar3;
                            if (kZero <= (local_54 - local_60) * local_34 + (local_68 - local_38) * local_30 +
                                         (local_40 - local_3c) * ((local_44 - local_38) * fVar3 - (local_64 - local_60) * fVar4)) {
                                local_34 = fVar4 * (local_48 - local_4c) - (local_58 - local_44) * fVar5;
                                local_30 = (local_50 - local_64) * fVar5 - (local_48 - local_4c) * fVar3;
                                if ((kZero <= (local_54 - local_64) * local_34 + (local_68 - local_44) * local_30 +
                                              (local_40 - local_4c) * ((local_58 - local_44) * fVar3 - (local_50 - local_64) * fVar4)) &&
                                    (kObj_DepthMin <= fVar1)) {
                                    pfVar9[3] = -fVar1;
                                    // local_74/70/6c are a contiguous stack vec3 in the original.
                                    float local_arm[3];
                                    local_arm[0] = (pfVar11[-0x268] - vF(param_1, 0x57)) * kArm_SandSpWheel;
                                    local_arm[1] = (pfVar11[-0x267] - vF(param_1, 0x58)) * kArm_SandSpWheel;
                                    local_arm[2] = (pfVar11[-0x266] - vF(param_1, 0x59)) * kArm_SandSpWheel;
                                    Rw_TransformPoints(pfVar9 + 5, local_arm, 1, local_10);
                                    pfVar9[0] = vF(param_1, 0x26c); local_arm[2] = 0.0f;
                                    pfVar9[1] = vF(param_1, 0x26d); local_arm[1] = 0.0f;
                                    pfVar9[2] = vF(param_1, 0x26e); local_arm[0] = 0.0f;
                                    float local_1c = Vec3Mag(pfVar9);
                                    bool fallback = (local_1c <= kTer_MagThresh);  // _DAT_005cd03c
                                    if (!fallback) {
                                        Vec3Normalize(pfVar9, pfVar9);
                                        float fVar2 = pfVar6[0xb] * pfVar9[2] + pfVar9[1] * pfVar6[10] + pfVar6[9] * pfVar9[0];
                                        if (kZero <= fVar2) {
                                            fallback = true;
                                        } else {
                                            if (fVar2 < kCC_ImpulseLoClamp) fVar2 = kCC_ImpulseLoClamp;
                                            pfVar9[8]  = local_1c * fVar2;
                                            pfVar9[-4] = pfVar6[9];
                                            pfVar9[-3] = pfVar6[10];
                                            pfVar9[-2] = pfVar6[0xb];
                                            pfVar9[-6] = fVar1;
                                            pfVar9[-5] = asF(0xffc00000);   // -NAN marker (compares != to all)
                                            pfVar6[0x13] = vF(param_1, 0x14) * vF(param_1, 0x26c);
                                            pfVar6[0x14] = vF(param_1, 0x14) * vF(param_1, 0x26d);
                                            float fM = vF(param_1, 0x14);
                                            float fVz = vF(param_1, 0x26e);
                                            pfVar6[0x19] = asF(0x00000001);  // 1.4013e-45
                                            pfVar6[0x15] = fM * fVz;
                                            pfVar6[0x13] = pfVar6[0x13] - pfVar6[0x10] * pfVar6[0xf];
                                            pfVar6[0x14] = pfVar6[0x14] - pfVar6[0x11] * pfVar6[0xf];
                                            pfVar6[0x15] = pfVar6[0x15] - pfVar6[0x12] * pfVar6[0xf];
                                            pfVar6[0x16] = pfVar11[-2];
                                            pfVar6[0x17] = pfVar11[-1];
                                            pfVar6[0x18] = pfVar11[0];
                                        }
                                    }
                                    if (fallback) {   // LAB_0046992d
                                        pfVar6[0x19] = asF(0x00000001);  // 1.4013e-45
                                        pfVar6[0x16] = pfVar11[-2];
                                        pfVar6[0x17] = pfVar11[-1];
                                        pfVar6[0x18] = pfVar11[0];
                                        pfVar6[0x15] = 0.0f;
                                        pfVar6[0x14] = 0.0f;
                                        pfVar6[0x13] = 0.0f;
                                    }
                                    // orig FUN_00485420(*(u32*)(pfVar6[0x1a]+0xc), &local_74):
                                    // writes object world pos into the same stack vec3 (local_arm).
                                    Obj_ReadWorldPos(*reinterpret_cast<void**>(reinterpret_cast<char*>(&pfVar6[0x1a]) + 0xc), local_arm);
                                    float local_28 = local_arm[0] - vF(local_10, 0xc);
                                    float local_24 = local_arm[1] - vF(local_10, 0xd);
                                    float local_20 = local_arm[2] - vF(local_10, 0xe);
                                    float delta[3] = { local_28, local_24, local_20 };
                                    Vec3Normalize(delta, delta);
                                    local_28 = delta[0]; local_24 = delta[1]; local_20 = delta[2];
                                    vF(param_1, 0x26c) = local_28 * kObj_VelCorrScale + vF(param_1, 0x26c);
                                    vF(param_1, 0x26d) = local_24 * kObj_VelCorrScale + vF(param_1, 0x26d);
                                    vF(param_1, 0x26e) = local_20 * kObj_VelCorrScale + vF(param_1, 0x26e);
                                    pfVar6[0x13] = pfVar6[0x13] + local_28 * kObj_ImpulseScale;
                                    pfVar6[0x14] = pfVar6[0x14] + local_24 * kObj_ImpulseScale;
                                    pfVar6[0x15] = pfVar6[0x15] + local_20 * kObj_ImpulseScale;
                                }
                            }
                        }
                    }
                }
                local_18 += 1;
                local_5c += 0x31;
                pfVar9  += 0x10;
                pfVar11 += 3;
            } while (local_18 < 0x12);
        }
        local_14 += 1;
        pfVar6  += 0x24;
    } while (local_14 < (unsigned int)Obj_ListCount());
}

// ---------------------------------------------------------------------------
// 0x00469aa0  VehicleContactHistoryUpdate
//   Shifts the 3 two-frame contact-history manifolds (current<->previous, set
//   sentinel -1), runs the two RW contact queries, dispatches the terrain +
//   object solvers, then scans the 18 contact slots and writes the active
//   contact count to veh+0x9ec. Returns 1 if any valid contact found.
//   `self` = vehicle (ESI in the original).
// ---------------------------------------------------------------------------
int VehicleContactHistoryUpdate(int* self)
{
    int iVar4 = self[0x26b];
    float local_4c[3] = { 0.0f, kGravProbeY, 0.0f };   // {0, 0.1, 0}

    int* piVar3 = self + 0x133;
    for (int iVar6 = 2; iVar6 != 0; --iVar6) {
        piVar3[1]    = piVar3[0];   piVar3[0]    = 0;
        int t;
        t = piVar3[0x10]; piVar3[0x10] = 0; piVar3[0x11] = t;
        t = piVar3[0x20]; piVar3[0x20] = 0; piVar3[0x21] = t;
        t = piVar3[0x30]; piVar3[0x30] = 0; piVar3[0x31] = t;
        t = piVar3[0x40]; piVar3[-8] = -1; piVar3[0x40] = 0; piVar3[8] = -1; piVar3[0x41] = t;
        t = piVar3[0x50]; piVar3[0x18] = -1; piVar3[0x28] = -1; piVar3[0x50] = 0; piVar3[0x51] = t;
        t = piVar3[0x60]; piVar3[0x38] = -1; piVar3[0x48] = -1; piVar3[0x60] = 0; piVar3[0x58] = -1; piVar3[0x61] = t;
        t = piVar3[0x70]; piVar3[0x68] = -1; piVar3[0x70] = 0; piVar3[0x78] = -1; piVar3[0x71] = t;
        piVar3[0x81] = piVar3[0x80]; piVar3[0x80] = 0;
        piVar3 += 0x90;
    }

    Rw_VtableDispatch(self + 0x27e, self + 0x18, 0x12, self + iVar4 * 0x10 + 0x24a);
    Rw_VtableDispatch(g_contactQueryScratch /*&DAT_0088e600*/, local_4c, 1, self + iVar4 * 0x10 + 0x24a);
    VehicleTerrainContactSolver(self, g_terrainBatch);
    VehicleObjectContactSolver(self);

    int found    = 0;       // iVar4 in orig (first-found gate + return)
    int local_54 = 0;       // active contact count
    int* piVar7  = self + 299;            // 0x4ac byte: 18-slot contact array
    for (int iVar6 = 0; iVar6 < 0x12; ++iVar6) {
        int iVar5 = piVar7[0];
        if ((iVar5 != -1) && (local_54 += 1, found == 0) &&
            (asF(piVar7[9]) <= asF(piVar7[8]))) {     // (float)piVar7[9] <= (float)piVar7[8]
            // (orig assembles a debug payload from piVar7[5..7]*piVar7[0xd] and
            //  enqueues debug-event type 3 at DAT_007e9de0 — dev-viz only; omitted)
            found = 1;
        }
        piVar7 += 0x10;
    }
    self[0x27b] = local_54;
    return found;
}

}  // namespace Collision
}  // namespace mashed_re
