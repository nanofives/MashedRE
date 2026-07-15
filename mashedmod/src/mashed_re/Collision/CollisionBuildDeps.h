// Mashed RE — B5b: extern surface for the RenderWare-Physics-3.7 load-time
// collision-body build chain + the qhull bridge.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-14).
// Map: re/analysis/B5b_RWP37_QHULL_VENDOR_2026-07-14.md + B5a_SYSTEM2_PLATING_2026-07-14.md.
//
// "System 2" is RenderWare Physics 3.7 (//Physics/Rwp37Active/). Only the qhull
// subtree is vendored (mashedmod/deps/qhull-2002.1, REALfloat=1). Every RWP helper
// (FUN_0055xxxx / FUN_0057cxxx / FUN_0056xxxx below) is a clean-room port target for
// B5c/B5d; here they are extern-declared so the build-chain ports transcribe verbatim.
// NO-GUESSING: signatures are exactly as the decomp uses them; unknown ABIs are cdecl
// placeholders (these do NOT link into the main exe yet — see the .cpp headers).
#ifndef MASHED_RE_COLLISION_BUILD_DEPS_H
#define MASHED_RE_COLLISION_BUILD_DEPS_H

namespace mashed_re {
namespace Collision {

// ---- The qhull bridge + its game-side output packaging (ported in QhullBridge.cpp) ----
// 0x0057ca30 RwpQHullWrapper.c — build a convex hull from a point cloud.
void* QhullBuildHull(int numpoints, float* points, const char* opt, void* file);
// 0x00563840 — allocate the packed collision-hull slab (verts | facets | edges).
void* CollHull_Alloc(int numVerts, int numEdges, int numFacets);
// 0x0057c670 — pack the qhull result (qh vertex_list / facet_list / ridges) into the slab.
void* CollHull_Pack(void* slab);

// ---- The 4-body build chain (ported in PhysicsWorldBuild.cpp / CollisionBodyCreate.cpp) ----
// 0x0047d3c0 — VehiclePhysicsWorldCreate: build the 4 proxy bodies into the world.
void  VehiclePhysicsWorldCreate(void* world);
// 0x004826d0 — PhysicsCollisionBodyCreate: hull + collision mesh for one body.
int   PhysicsCollisionBodyCreate(int scene, int linkFlag);
// 0x00481e00 — cone-cast hull builder: sample the shape support-map, feed qhull.
void* ConeCastHullBuild(int shape, float scale, void* mtx);

// ===================== RWP-3.7 helpers — NOT YET PORTED (B5c/B5d) =====================
// Declared exactly as the decomp calls them. RVA in the trailing comment.
extern "C" {
// scene / material builder (RwpVolume / RwpMgr)
void* FUN_0057c500(void);                                   // 0x0057c500 scene/material builder
int   FUN_0057c300(void* builder);                          // 0x0057c300 per-body scene node create
void  FUN_0057c220(int bodyKey, int node);                  // 0x0057c220 link body ptr into DAT_007dc8d8
void  FUN_0057c550(void* builder);                          // 0x0057c550 finalize builder
void  FUN_0055c810(int scene, float* comVec3);              // 0x0055c810 set centre-of-mass offset
void  FUN_0055c4f0(int scene, float friction);              // 0x0055c4f0 set friction   (0.4f)
void  FUN_0055c4a0(int scene, float restitution);           // 0x0055c4a0 set restitution(0.5f)
void  FUN_0055bab0(int scene, float damping);               // 0x0055bab0 set damping    (0.01f)
void  FUN_0055b940(void* builder, float mass, int flag);    // 0x0055b940 set mass       (1.5f)
// body / world registration (RwpBodyObj / RwpMgr)
void  FUN_00482730(int body, void* colorRGBA);              // 0x00482730 set debug colour
void* FUN_004c0b30(void);                                   // 0x004c0b30 (attach payload)
void  FUN_004e7e30(int body, void* payload);                // 0x004e7e30 attach
void  FUN_0047d240(float i, int a, float b, float y, float x); // 0x0047d240 place body i
void* FUN_0055dec0(void* world, int node, int minus1, int two); // 0x0055dec0 register body into world
void  FUN_00559c40(void* reg);                              // 0x00559c40 post-register
int   FUN_00426c00(void);                                   // 0x00426c00 game-mode query (==0x26?)
void  FUN_0055ae70(int owner, int node, int group);         // 0x0055ae70 set collision group
// collision-mesh + body activate (RwpConvexHull / RwpBodyObj)
void* FUN_00482140(void* hull, int meshFlag);               // 0x00482140 RpAtomic collision-mesh strip
void  FUN_00563810(void* hull);                             // 0x00563810 activate body
// GJK support / extreme-point mapping (RwpGjk.c) — vtable-indirect on the shape
void* FUN_0055c000(int shape, void* mtx, float* dir, float* out); // 0x0055c000 support map
// x87 transcendentals used by the cone table (kept x87 for bit-identity)
double fsin_x87(double);                                    // fsin  (float10)
double fcos_x87(double);                                    // fcos  (float10)
}

} // namespace Collision
} // namespace mashed_re
#endif
