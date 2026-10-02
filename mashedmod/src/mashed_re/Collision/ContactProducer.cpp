// Mashed RE — WS-B4 (producer half): the car↔world terrain-batch producer.
//
// Faithful port of the per-triangle collector LAB_00468b80 (0x00468b80..0x00468d7c,
// the callback FUN_0046f6c0 passes to the RW broadphase FUN_00538c80), plus a
// broadphase loop that fills the batch from the standalone's collision triangles.
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool3, read_only, 2026-06-16). Map: re/analysis/WSB2_B3_CONTACT_PORT_MAP.md
//
// LAB_00468b80 writes one 0x90-byte batch entry per intersecting triangle into the
// batch base DAT_00828320 (the userData arg), then increments DAT_0088e60c:
//   f[0..2]=v0  f[3..5]=v1  f[6..8]=v2          (collTriangle vertex ptrs +0x1c/+0x20/+0x24)
//   f[9..0xb]=face normal                        (collTriangle[0..2])
//   f[0xc]=material idx  f[0xd]=surface key       (byte 0x34 — the history-match key)
//   f[0xf]=100000.0 (0x47c35000 @0x468c69)        f[0x10..0x12]=0
//   f[0x1b..0x1d]=N×(v1-v0)  f[0x1e..0x20]=N×(v2-v1)  f[0x21..0x23]=N×(v0-v2)
// (the 3 SAT half-plane edge normals; sat0.x = N.y*e0.z - N.z*e0.y confirmed @0x468c8f-9b).
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"
#include <cstdlib>   // getenv (MASHED_D2_BATCHMODE A/B, see ProduceTerrainBatch)
#include <cstring>   // strcmp

namespace mashed_re {
namespace Vehicle { long long* PerfBatchTestCounter(); }  // WS-A s3 perf (VehiclePhysicsRun.cpp)
namespace Collision {

// Batch storage (the DAT_00828320 equivalent — the original's batch base global).
static ContactBatchEntry s_batchStorage[256];

static inline int& asIref(float& f) { return *reinterpret_cast<int*>(&f); }

// N × edge   (matches LAB_00468b80: out.x = N.y*e.z - N.z*e.y, etc.)
static inline void crossNE(const float* N, const float* e, float* out) {
    out[0] = N[1] * e[2] - N[2] * e[1];
    out[1] = N[2] * e[0] - N[0] * e[2];
    out[2] = N[0] * e[1] - N[1] * e[0];
}

// LAB_00468b80 — fill one batch entry from a triangle (verts, RW-precomputed
// face normal, material index, surface key).
void FillBatchEntry(ContactBatchEntry* e,
                    const float* v0, const float* v1, const float* v2,
                    const float* faceNormal, int material, int surfaceKey)
{
    e->f[0] = v0[0]; e->f[1] = v0[1]; e->f[2] = v0[2];
    e->f[3] = v1[0]; e->f[4] = v1[1]; e->f[5] = v1[2];
    e->f[6] = v2[0]; e->f[7] = v2[1]; e->f[8] = v2[2];
    e->f[9] = faceNormal[0]; e->f[10] = faceNormal[1]; e->f[11] = faceNormal[2];
    asIref(e->f[0xc]) = material;       // entry[0xc] (byte 0x30) = material idx
    asIref(e->f[0xd]) = surfaceKey;     // entry[0xd] (byte 0x34) = history key / sentinel
    e->f[0xf] = kObj_ImpulseScale;      // 100000.0 (0x47c35000 @0x468c69)
    e->f[0x10] = 0.0f; e->f[0x11] = 0.0f; e->f[0x12] = 0.0f;
    float e0[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
    float e1[3] = { v2[0]-v1[0], v2[1]-v1[1], v2[2]-v1[2] };
    float e2[3] = { v0[0]-v2[0], v0[1]-v2[1], v0[2]-v2[2] };
    crossNE(faceNormal, e0, &e->f[0x1b]);
    crossNE(faceNormal, e1, &e->f[0x1e]);
    crossNE(faceNormal, e2, &e->f[0x21]);
}

// CollTriangle (declared in ContactSolvers.h) is the standalone's COLLI*.BSP
// triangle: the face normal must be consistent with the winding ((v1-v0)×(v2-v0)
// direction) so the SAT half-plane signs match the solvers' `>=0`-is-inside test.

// Broadphase: append every triangle whose AABB, grown by `radius`, contains the
// query centre (the standalone's stand-in for FUN_00538c80's BSP walk over
// COLLI*.BSP). [D2 attempt 20 / U-9179] This was a PLANE-distance test until
// 2026-10-02; see the block comment inside the function for why that was wrong
// and what it cost.
// Resets + sets DAT_0088e60c, points g_terrainBatch at the batch storage.
// Returns the number of entries produced.
int ProduceTerrainBatch(const float* center, float radius,
                        const CollTriangle* tris, int triCount)
{
    g_terrainBatch = reinterpret_cast<int*>(s_batchStorage);
    if (long long* pc = Vehicle::PerfBatchTestCounter()) *pc += triCount;  // PERF: scan size
    int count = 0;
    const int cap = (int)(sizeof(s_batchStorage) / sizeof(s_batchStorage[0]));
    // [D2 attempt 20 / U-9179] THE ADMISSION TEST IS SPATIAL. It used to be a
    // PLANE-distance test, and because a plane is unbounded that is not a locality test at
    // all: on a largely coplanar track it admitted ground triangles from anywhere on the
    // surface, the 256-entry store saturated (measured: on 3565 of 3565 solver calls), the
    // loop stopped scanning, and the triangles actually under wheels 0 and 1 were never
    // offered to the classifier 0x0046cc40. The classifier then left those wheels' records
    // unfilled, so `key` (+0x1ec) stayed at the init loop's -1 and 0x0046f6c0's state
    // machine demoted them to state 0 at 0x0046f91a/0x0046f91f -- which opened the
    // `0 < count <= 2.0` gate at 0x004701e8 and fired the airborne lateral drift 47 times
    // in 29 frames. That was U-9179, and 84.33 % of D2's T_post sink.
    //
    // THE ORIGINAL HAS NO CAP: collector LAB_00468b80 increments DAT_0088e60c
    // unconditionally at 0x00468d6c..0x00468d73, with no bound test anywhere in
    // 0x00468b80..0x00468d7c. Its locality comes entirely from the BSP walk FUN_00538c80
    // that this function stands in for, and the standard broadphase admission test for
    // that is sphere-vs-triangle. The AABB form below is a conservative SUPERSET of it, so
    // it cannot drop a triangle the narrow phase would have used, and it introduces no new
    // constant -- same `center`, same `radius` (the record's own +0x4a4) as before.
    // Measured at d = 222..250: 17 entries instead of 256, all 348 wheel-rows FILLED,
    // K = 4 and S1 = 4 (the ORIGINAL's own numbers at matched d), wcs_drift 0 firings.
    // Evidence: verify/d2_wheelstate_20261002/RESULT_STEP2.md sections 4 and 4.2.
    //
    // MASHED_D2_BATCHMODE is kept as the DIAGNOSTIC pre-fix arm, so the paired collateral
    // and any future A/B has a legacy side. It is not a shipping path:
    //   unset / "local"  the spatial test above (DEFAULT)
    //   "plane"          the old plane test, scan stops at cap  (pre-fix baseline)
    //   "planefull"      the old plane test, full scan           (isolates truncation)
    static const char* const s_mode = std::getenv("MASHED_D2_BATCHMODE");
    static const bool s_plane = s_mode && (std::strcmp(s_mode, "plane") == 0 ||
                                           std::strcmp(s_mode, "planefull") == 0);
    static const bool s_full  = !s_plane || (s_mode && std::strcmp(s_mode, "planefull") == 0);
    const bool full = s_full, local = !s_plane;
    int pass = 0;
    for (int t = 0; t < triCount && (full || count < cap); ++t) {
        const CollTriangle& tr = tris[t];
        bool admit;
        if (local) {
            // spatial: the triangle's AABB grown by `radius` (the record's own +0x4a4
            // query radius) must contain the query centre. A conservative superset of
            // the sphere-vs-triangle test, which is the right semantics for a
            // broadphase: it cannot drop a triangle the narrow phase would have used.
            admit = true;
            for (int k = 0; k < 3 && admit; ++k) {
                float lo = tr.v0[k], hi = tr.v0[k];
                if (tr.v1[k] < lo) lo = tr.v1[k];
                if (tr.v1[k] > hi) hi = tr.v1[k];
                if (tr.v2[k] < lo) lo = tr.v2[k];
                if (tr.v2[k] > hi) hi = tr.v2[k];
                admit = (center[k] >= lo - radius) && (center[k] <= hi + radius);
            }
        } else {
            // plane distance of the query centre from the triangle plane
            float d = (center[0] - tr.v2[0]) * tr.normal[0] +
                      (center[1] - tr.v2[1]) * tr.normal[1] +
                      (center[2] - tr.v2[2]) * tr.normal[2];
            if (d < 0.0f) d = -d;
            admit = (d <= radius);
        }
        if (admit) {
            ++pass;
            if (count < cap) {
                FillBatchEntry(&s_batchStorage[count], tr.v0, tr.v1, tr.v2,
                               tr.normal, tr.material, tr.surfaceKey);
                ++count;
            }
        }
    }
    g_terrainPassCount = pass;     // uncapped admissions (diagnostic; == count when unsaturated)
    g_terrainEntryCount = count;   // DAT_0088e60c (collector increments per entry)
    return count;
}

}  // namespace Collision
}  // namespace mashed_re
