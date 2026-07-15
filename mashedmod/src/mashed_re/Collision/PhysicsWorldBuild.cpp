// Mashed RE — B5b: RenderWare-Physics-3.7 4-body build root (RwpConvexHull.c /
// RwpMgr.c), load-time only. Gated DAT_0086caa0==0x7b, cleared once at 0x0047eba7.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-14). Verbatim transcription of FUN_0047d3c0.
// Source map: re/analysis/B5b_RWP37_QHULL_VENDOR_2026-07-14.md.
//
// Builds the 4 proxy bodies DAT_006c9a78[0..3] into the physics world DAT_006ce274
// (param world). C-LEVEL: source-faithful transcription; NOT runtime-verified
// standalone (every FUN_0055xx/0057cxx callee is a B5c RWP port target). Do NOT add
// to build.bat until those land.
#include "CollisionBuildDeps.h"
#include "ContactDeps.h"  // asF (bit-reinterpret helper)

namespace mashed_re {
namespace Collision {

// ---- MASHED globals (extern; do not exist standalone) ----
extern "C" int   DAT_006c9a78;        // 0x006c9a78 base of the 4 body-handle keys ((&DAT_006c9a78)[i])
// car bounding box (min/max per axis) used for the CoM offsets:
extern "C" float _DAT_00881630, _DAT_0088163c, _DAT_00881634, _DAT_00881638;
extern "C" float _DAT_00881650, _DAT_00881664;
extern "C" float _DAT_005cc32c;       // 0x005cc32c 0.5   bbox scale
extern "C" float _DAT_005cc9dc;       // 0x005cc9dc 0.95  bbox shrink
extern "C" float _DAT_005cc94c;       // 0x005cc94c int->float sign-fix addend (i>=0 so unused)
extern "C" float _DAT_005cd074;       // 0x005cd074 per-body X spacing step  [UNCERTAIN value]
// CoM broadcast targets (the 4 car records, stride ~0xd04):
extern "C" float DAT_008816d8, DAT_008816dc, DAT_008816e0;
extern "C" float DAT_008823dc, _DAT_008823e0, _DAT_008823e4;
extern "C" float _DAT_008830e0, _DAT_008830e4, _DAT_008830e8;
extern "C" float _DAT_00883de4, _DAT_00883de8, _DAT_00883dec;

// 0x0047d3c0  RwpConvexHull.c / RwpMgr.c — VehiclePhysicsWorldCreate.
//   void FUN_0047d3c0(undefined4 param_1 /*world = DAT_006ce274*/)
void VehiclePhysicsWorldCreate(void* world)
{
    unsigned char color[4] = { 0x40, 0x40, 0x60, 0xff };   // local_20..1d body debug colour
    float com[3];                                          // local_18/14/10 centre-of-mass offset

    unsigned* puVar3 = (unsigned*)FUN_0057c500();          // scene/material builder
    puVar3[0xd] = 0x3f800000u;                             // 1.0f
    puVar3[0xe] = 0x40200000u;                             // 2.5f
    puVar3[1]   = 0u;                                      // gravity.x = 0
    puVar3[2]   = 0xbdcccccdu;                             // gravity.y = -0.1f
    puVar3[3]   = 0u;                                      // gravity.z = 0

    // centre-of-mass offset from the car bbox * 0.5 * 0.95  (verbatim operand form):
    com[0] = (_DAT_00881630 - _DAT_0088163c) * _DAT_005cc32c * _DAT_005cc9dc;  // local_18
    com[1] = (_DAT_00881664 - _DAT_00881634) * _DAT_005cc32c * _DAT_005cc9dc;  // local_14
    com[2] = (_DAT_00881638 - _DAT_00881650) * _DAT_005cc32c * _DAT_005cc9dc;  // local_10
    DAT_008816d8 = (-_DAT_0088163c - _DAT_00881630) * _DAT_005cc32c;
    DAT_008816dc = -(_DAT_00881634 + com[1]);
    DAT_008816e0 = (-_DAT_00881650 - _DAT_00881638) * _DAT_005cc32c;
    // broadcast to the 3 sibling car records (stride ~0xd04):
    DAT_008823dc = DAT_008816d8; _DAT_008823e0 = DAT_008816dc; _DAT_008823e4 = DAT_008816e0;
    _DAT_008830e0 = DAT_008816d8; _DAT_008830e4 = DAT_008816dc; _DAT_008830e8 = DAT_008816e0;
    _DAT_00883de4 = DAT_008816d8; _DAT_00883de8 = DAT_008816dc; _DAT_00883dec = DAT_008816e0;

    int scene = (int)puVar3[0];                            // uVar1 = *puVar3 (RWP scene handle)
    FUN_0055c810(scene, com);                              // set CoM offset
    FUN_0055c4f0(scene, asF(0x3ecccccd));                  // friction    0.4f
    FUN_0055c4a0(scene, asF(0x3f000000));                  // restitution 0.5f
    FUN_0055bab0(scene, asF(0x3c23d70a));                  // damping     0.01f
    FUN_0055b940(puVar3, asF(0x3fc00000), 1);             // mass        1.5f

    for (int i = 0; i < 4; ++i) {                          // body count fixed at 4
        int node = FUN_0057c300(puVar3);                  // per-body scene node -> iVar4
        int body = PhysicsCollisionBodyCreate(scene, 1);  // FUN_004826d0(uVar1, 1)
        (&DAT_006c9a78)[i] = body;                         // store body-handle key
        FUN_00482730(body, color);                        // set debug colour
        void* payload = FUN_004c0b30();
        FUN_004e7e30((&DAT_006c9a78)[i], payload);         // attach
        FUN_0057c220((&DAT_006c9a78)[i], node);            // link body ptr into DAT_007dc8d8
        // place body i: FUN_0047d240(i, 0, 0.5, -10.0, i*_DAT_005cd074)
        FUN_0047d240((float)i, 0, asF(0x3f000000), asF(0xc1200000), (float)i * _DAT_005cd074);
        *(unsigned*)(node + 8) &= 0xfffffff3u;             // clear flag bits
        void* reg = FUN_0055dec0(world, node, -1, 2);      // register body into world
        FUN_00559c40(reg);
        int group;                                         // collision group
        if (FUN_00426c00() == 0x26) group = i + 2;         // mode 0x26 -> per-body group
        else                        group = 1;
        FUN_0055ae70(*(int*)(node + 0x24), node, group);
    }
    FUN_0057c550(puVar3);                                  // finalize builder
}

} // namespace Collision
} // namespace mashed_re
