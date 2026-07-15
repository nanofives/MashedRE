// Mashed RE — B5b: RenderWare-Physics-3.7 collision-body create + cone-cast hull
// builder (RwpConvexHull.c), load-time only.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-14). Verbatim transcription of FUN_004826d0 +
// FUN_00481e00. Source map: re/analysis/B5b_RWP37_QHULL_VENDOR_2026-07-14.md.
//
// C-LEVEL: source-faithful transcription; NOT yet runtime-verified standalone (its
// producer FUN_0055c000 = RwpGjk support map is a B5c port target, and the cone
// table depends on runtime-written globals — see [UNCERTAIN] notes). The isolated
// qhull path IS verified via QhullBridge.cpp + the B5b Frida capture (feeding the
// captured point cloud past this producer). Do NOT add to build.bat until the RWP
// helpers land in B5c (unresolved externs).
#include "CollisionBuildDeps.h"
#include <cmath>

namespace mashed_re {
namespace Collision {

// ---- MASHED globals referenced by FUN_00481e00 (extern; do not exist standalone) ----
// Cone-direction sample table + its cached count.
extern "C" int   DAT_006ce818;       // 0x006ce818 cached sample count (lazy-init guard)
extern "C" float DAT_006ce278;       // 0x006ce278 sample table base (stride 3 floats)
// Cone constants (raw bytes @ RVA; see memory_read 0x005cf238 = 01 00 7a 04 | 0a d7 e3 3f | 00 00 00 00).
extern "C" float _DAT_005cf23c;      // 0x005cf23c 1.78     (0x3fe3d70a) cone step numerator
// 0x005cf240 is an 8-byte DOUBLE = 120.0 (0x405e000000000000), read as `FLD qword` at
// 0x00481e14. Ghidra mis-typed the LOW dword as float 0.0 (the "_-overlap" warning); B5b
// inherited that and computed sqrtf(0.0). RESOLVED 2026-07-15 (B5c note §5): reference_to
// 0x005cf240 = exactly 1 READ (0x00481e14), 0 WRITE — a static .rdata constant, no runtime
// writer. Inlined below as the literal double; the mis-typed extern is retired.
static const double kConeSqrtDivisor_005cf240 = 120.0;
extern "C" float _DAT_005cf238;      // 0x005cf238 DENORMAL (0x047a0001) real float const, type-1 scale mul (read `FMUL m32fp` 0x00481f40)
extern "C" float _DAT_005ce2f4;      // 0x005ce2f4 pi       (0x40490fdb) latitude max
extern "C" float _DAT_005cd2c0;      // 0x005cd2c0 2*pi     (0x40c90fdb) longitude span
extern "C" float DAT_005d757c;       // 0x005d757c 0.0      latitude start / mesh threshold
extern "C" float _DAT_005cd0ec;      // 0x005cd0ec 0.005    (0x3ba3d70a) type-4 radius floor

// 0x004826d0  RwpConvexHull.c — create one collision body: cone-cast hull + mesh, activate.
//   int FUN_004826d0(int param_1 /*scene*/, int param_2 /*linkFlag*/)
int PhysicsCollisionBodyCreate(int scene, int linkFlag)
{
    int iVar3 = (linkFlag == 0) ? 0 : scene;                    // 0x004826d0
    void* hull = ConeCastHullBuild(scene, 1.0f, (void*)iVar3);  // FUN_00481e00(scene,1.0f,iVar3)
    // mesh flag: DAT_005d757c(0.0) < *(float*)(scene+0x4c)
    int meshFlag = (DAT_005d757c < *(float*)(scene + 0x4c)) ? 1 : 0;
    void* mesh = FUN_00482140(hull, meshFlag);                  // FUN_00482140 (RpAtomic collision mesh)
    FUN_00563810(hull);                                         // FUN_00563810 activate body
    return (int)mesh;                                           // stored into DAT_006c9a78[i]
}

// 0x00481e00  RwpConvexHull.c — build a convex hull by cone-sampling the shape's
// GJK support map in ~120 directions, then handing the point cloud to qhull.
//   undefined4 FUN_00481e00(int param_1 /*shape*/, float param_2 /*scale*/, undefined4 param_3 /*mtx*/)
void* ConeCastHullBuild(int shape, float scale, void* mtx)
{
    // ---- lazy cone-direction sample table build (once; guard DAT_006ce818 == 0) ----
    // RESOLVED (B5c): 0x005cf240 is the double 120.0, so step = 1.78/sqrt(120) = 0.1625 < pi
    // and this cone-build block RUNS, populating ~120 dirs (cap 0x78 at 0x00481eb0). There is
    // NO runtime writer (the earlier "populated elsewhere" note was the sqrtf(0.0) mis-read).
    if (DAT_006ce818 == 0) {                                            // 0x00481e0c
        float step = (float)((double)_DAT_005cf23c / sqrt(kConeSqrtDivisor_005cf240)); // 0x00481e14..e1c FLD qword/FSQRT/FDIVR
        double lat0 = (double)DAT_005d757c;                            // fVar12 (latitude carry)
        double lat  = (double)step;                                    // fVar13
        if (step < _DAT_005ce2f4) {                                    // step < pi
            do {
                float s = (float)sin(lat);                            // fsin(lat)
                float c = (float)cos(lat);                            // fcos(lat)
                float as = (s < DAT_005d757c) ? -s : s;               // abs(sin)
                float lonEnd = _DAT_005cd2c0 - step / as;             // 2*pi - step/abs(sin)
                if (lat0 < (double)lonEnd) {
                    float* d = &DAT_006ce278 + DAT_006ce818 * 3;      // element base
                    double lon = lat0;
                    do {
                        if (0x77 < DAT_006ce818) break;               // cap 119
                        float cl = (float)cos(lon);                   // fcos(lon)
                        DAT_006ce818 = DAT_006ce818 + 1;
                        d[0] = cl * s;                                // x = cos(lon)*sin(lat)
                        float sl = (float)sin(lon);                   // fsin(lon)
                        d[1] = sl * s;                                // y = sin(lon)*sin(lat)
                        d[2] = c;                                     // z = cos(lat)
                        double dlon = (double)(step / as);
                        lon = dlon + dlon + lon;                      // lon += 2*step/abs(sin)
                        d += 3;
                    } while (lon < (double)lonEnd);
                }
                lat0 = (double)step - lat0;                           // fVar12 = step - fVar12
                lat  = (double)step + (double)step + lat;             // lat += 2*step
            } while (lat < (double)_DAT_005ce2f4);                    // lat < pi
        }
    }

    // ---- radius + shape-type adjust ----
    float radius = *(float*)(shape + 0x4c) * scale;                    // local_5b4  0x00481e90
    short type = **(short**)(shape + 0x5c);                            // shape type
    if (type == 1) scale = scale * _DAT_005cf238;                     // 0x00481e9e [UNCERTAIN denormal]
    if (type == 4 && radius < _DAT_005cd0ec) radius = 0.005f;         // 0x00481ea7 clamp

    // ---- cone sweep: support-map each direction, accumulate point cloud ----
    unsigned n = (unsigned)DAT_006ce818;
    float cloud[360];                                                 // local_5a0
    float* out = cloud;
    if (DAT_006ce818 != 0) {
        float* dir = &DAT_006ce278;
        unsigned i = 0;
        do {
            float* pt = (float*)FUN_0055c000(shape, mtx, dir, out);   // support map  0x00481ed4
            n = (unsigned)DAT_006ce818;
            if (pt == 0) return 0;                                    // NULL => miss, return 0
            float x = pt[0], y = pt[1], z = pt[2];
            i = i + 1;
            out = pt + 3;
            pt[0] = scale * x;                                        // (verbatim: written then overwritten)
            pt[1] = scale * y;
            pt[2] = scale * z;
            pt[0] = radius * dir[0] + scale * x;                      // support*scale + radius*dir
            pt[1] = dir[1] * radius + scale * y;
            pt[2] = dir[2] * radius + scale * z;
            dir += 3;
        } while (i < (unsigned)DAT_006ce818);
    }
    return QhullBuildHull((int)n, cloud, 0, 0);                       // FUN_0057ca30(count, cloud, 0, 0)
}

} // namespace Collision
} // namespace mashed_re
