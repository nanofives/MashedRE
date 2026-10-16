// Mashed RE — d2bsp: the RenderWare collision world the wheel solver queries, and the
// original's sphere walk over it (FUN_00538c80 case 3 -> FUN_00539ec0).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
// Evidence and layout: verify/d2bsp_20261016/PREREG_d2bsp.md section 0.
//
// The original queries the RpWorld built from COLLI*.BSP (`[[EBX]+8]` at 0x0046f80b). This
// module keeps a replica of the parts of that world FUN_00539ec0 and its callback read, in
// the original's in-memory layout, built from the same stream at track load:
//   world   +0x10 RpMaterial** (colour at material +0x04), +0x1c root sector
//   plane   {type (0/4/8), value, left, right, leftValue, rightValue}
//   sector  {type = -1, +0x04 RpTriangle* {u16 v0,v1,v2,mat}, +0x08 RwV3d*, coll tree at
//            +[0x007dc5b4] (this replica: g_bw_collPluginOffset)}
//   coll    the 0x11D plugin's tree, layout of FUN_0053d5a0 (+0 flags, +4 sup, +0x10 inf,
//            +0x1c u16 numTris, +0x1e u16 numSplits, +0x20 splits, +0x24 u16 map)
// The walk itself is verbatim naked asm generated from the bytes (CollisionWorld_naked.inc,
// spec re/tools/naked_specs/collworld_d2bsp.json) and runs under x87 PC_24.
#pragma once

#include <cstddef>
#include <cstdint>

namespace mashed_re {
namespace Track { class World; }
namespace Collision {

// Build the replica from a COLLI*.BSP stream. `parsed` is the same blob already parsed by
// Track::World (vertices and triangles per sector, in the same left-first sector order).
// Returns false (and leaves the replica not ready) on any layout mismatch.
bool CollWorld_Build(const std::uint8_t* blob, std::size_t len, const Track::World& parsed);
bool CollWorld_Ready();

// The RW collision callback type (cdecl, as the original calls it at 0x0053a643).
typedef void* (__cdecl* CollWorldCb)(void* isect, void* sector, float* collTri, float dist, void* data);

// 0x00538c80 — RpCollisionWorldForAllIntersections over the replica. Only case 3 (sphere) is
// ported; any other type returns without a callback (the line / box / atomic cases keep their
// existing callers). `isect` = {x, y, z, r, -, -, int type} as built at 0x0046f7d3..0x0046f810.
void CollWorld_ForAllIntersections(float* isect, CollWorldCb cb, void* data);

// [[0x0088e654]+8]+0x10 -> materials[matIdx]+4: the RGBA dword FUN_00468b80 stores at
// entry +0x34 (0x00468baa..0x00468bbc). 0 when the replica is not ready or out of range.
std::uint32_t CollWorld_MaterialColour(int matIdx);

// d2bsp knobs (read once). Fix W: MASHED_D2BSP=1 (opt-in in this lane); MASHED_D2BSP_KEY0=1
// forces the key to 0 (diagnostic).
bool D2BspOn();
bool D2BspKey0();

// Diagnostic, default OFF: MASHED_BATCH_DUMP=<path>. The batch the classifier is about to read
// (player record only), in the --batch-probe site-0 layout.
void CollWorld_DumpBatch(const int* self, const float* qc, float r);

// Diagnostic, default OFF: MASHED_BSP_REPLAY=<orig batch-probe csv> + MASHED_BSP_REPLAY_OUT=<path>.
// Runs fix W, the stand-in and the port's classifier on every captured query. Called once the
// stand-in triangle soup is set (VehiclePhysics_SetWorld); does nothing when unset.
void CollWorld_RunReplayIfRequested();

}  // namespace Collision
}  // namespace mashed_re
