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

// Broadphase: append every triangle whose plane is within `radius` of the query
// centre (the standalone's stand-in for FUN_00538c80's BSP walk over COLLI*.BSP).
// Resets + sets DAT_0088e60c, points g_terrainBatch at the batch storage.
// Returns the number of entries produced.
int ProduceTerrainBatch(const float* center, float radius,
                        const CollTriangle* tris, int triCount)
{
    g_terrainBatch = reinterpret_cast<int*>(s_batchStorage);
    if (long long* pc = Vehicle::PerfBatchTestCounter()) *pc += triCount;  // PERF: scan size
    int count = 0;
    const int cap = (int)(sizeof(s_batchStorage) / sizeof(s_batchStorage[0]));
    for (int t = 0; t < triCount && count < cap; ++t) {
        const CollTriangle& tr = tris[t];
        // plane distance of the query centre from the triangle plane
        float d = (center[0] - tr.v2[0]) * tr.normal[0] +
                  (center[1] - tr.v2[1]) * tr.normal[1] +
                  (center[2] - tr.v2[2]) * tr.normal[2];
        if (d < 0.0f) d = -d;
        if (d <= radius) {
            FillBatchEntry(&s_batchStorage[count], tr.v0, tr.v1, tr.v2,
                           tr.normal, tr.material, tr.surfaceKey);
            ++count;
        }
    }
    g_terrainEntryCount = count;   // DAT_0088e60c (collector increments per entry)
    return count;
}

}  // namespace Collision
}  // namespace mashed_re
