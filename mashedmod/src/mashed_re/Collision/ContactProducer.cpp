// Mashed RE — WS-B4 (producer half): the car↔world terrain-batch producer.
//
// Faithful port of the per-triangle collector LAB_00468b80 (0x00468b80..0x00468d7c,
// the callback FUN_0046f6c0 passes to the RW broadphase FUN_00538c80), and the
// FUN_00538c80 case-3 sphere walk driver ProduceTerrainBatchBsp below. d2close deleted the
// port-only triangle-soup stand-in (ProduceTerrainBatch); the walk is the only terrain query.
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
#include "CollisionWorld.h"   // [d2bsp] fix W
#include <cstdint>

namespace mashed_re {
namespace Collision {

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

// ---- [d2bsp / d2close] the original's walk + collector (verify/d2bsp_20261016/PREREG_d2bsp.md) --------
// The original's batch DAT_00828320 has no bound in the collector. This storage is sized far above any
// count measured on the sphere walk; an entry past it is COUNTED (g_bspBatchOverflow), never written.
static ContactBatchEntry s_bspBatchStorage[2048];
int g_bspBatchOverflow = 0;

// 0x00468b80 — the RW collision callback FUN_0046f6c0 hands to FUN_00538c80 (push 0x468b80 @0x0046f7e8).
// (isect, sector, collTriangle {normal[3], point[3], int index, RwV3d* v[3]}, distance, batch base).
static void* __cdecl TerrainCollector(void* /*isect*/, void* sector, float* collTri, float /*dist*/, void* data)
{
    const int count = g_terrainEntryCount;                                           // 0x00468b83
    ContactBatchEntry* e = reinterpret_cast<ContactBatchEntry*>(data) + count;       // count*0x90 + data
    const std::uint16_t* tris = *reinterpret_cast<std::uint16_t* const*>(
        reinterpret_cast<const char*>(sector) + 4);                                  // 0x00468b93 sector+4
    const int index = *reinterpret_cast<const int*>(&collTri[6]);                   // 0x00468ba2 tri+0x18
    const int mat = tris[index * 4 + 3];                                             // 0x00468ba5 movzx [+6]
    const int key = D2BspKey0() ? 0
                  : static_cast<int>(CollWorld_MaterialColour(mat));                 // 0x00468baa..0x00468bbc
    const float* v0 = *reinterpret_cast<float* const*>(&collTri[7]);                 // tri+0x1c
    const float* v1 = *reinterpret_cast<float* const*>(&collTri[8]);                 // tri+0x20
    const float* v2 = *reinterpret_cast<float* const*>(&collTri[9]);                 // tri+0x24
    if (count < (int)(sizeof(s_bspBatchStorage) / sizeof(s_bspBatchStorage[0])))
        FillBatchEntry(e, v0, v1, v2, collTri /*normal, tri+0*/, mat, key);
    else
        ++g_bspBatchOverflow;
    g_terrainEntryCount = count + 1;                                                 // 0x00468d6c..0x00468d73
    return collTri;                                                                  // EAX = tri (0x00468b99)
}

// The 0x0046f7d3..0x0046f817 sequence: sphere {centre, r} type 3, DAT_0088e60c = 0, then FUN_00538c80.
int ProduceTerrainBatchBsp(const float* center, float radius)
{
    g_terrainBatch = reinterpret_cast<int*>(s_bspBatchStorage);
    float isect[7] = { center[0], center[1], center[2], radius, 0.0f, 0.0f, 0.0f };
    *reinterpret_cast<std::int32_t*>(&isect[6]) = 3;                                // local_74 = 3
    g_terrainEntryCount = 0;                                                         // 0x0046f7fb
    CollWorld_ForAllIntersections(isect, &TerrainCollector, s_bspBatchStorage);      // 0x0046f817
    if (g_terrainEntryCount > (int)(sizeof(s_bspBatchStorage) / sizeof(s_bspBatchStorage[0])))
        g_terrainEntryCount = (int)(sizeof(s_bspBatchStorage) / sizeof(s_bspBatchStorage[0]));
    g_terrainPassCount = g_terrainEntryCount;
    return g_terrainEntryCount;
}

}  // namespace Collision
}  // namespace mashed_re
