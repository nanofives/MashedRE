// Mashed RE — WS-B2/B3 residual-dependency stubs + per-tick scratch globals.
//
// The contact solvers (CarWorldContacts.cpp / CarCarContacts.cpp) are faithful
// ports of the original contact LOGIC. The RW-engine queries and the dynamic-
// object list they call are NOT yet ported; they are wired in WS-B4 / WS-A when
// the contact path replaces the ground-raycast scaffold and binds to the real
// RW math (already ported under Math/) + the COLLI*.BSP broadphase walk.
//
// Until then these stubs make the module COMPILE + LINK inert (nothing calls
// the solvers yet; Obj_ListCount()=0 and g_terrainEntryCount=0 keep them no-op).
// Each stub cites the real RVA so the wiring step knows what to bind.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ContactDeps.h"

namespace mashed_re {
namespace Collision {

// --- per-tick contact scratch (DAT_0088e5e0..) ------------------------------
int   g_wheelSkipFlags[4]       = {0,0,0,0};   // DAT_0088e5e0
float g_contactQueryScratch[16] = {0};         // DAT_0088e600
float g_wheelContactPos[12]     = {0};         // DAT_0088e624
int   g_activeContactCount      = 0;           // DAT_0088e650
int   g_terrainEntryCount       = 0;           // DAT_0088e60c
int*  g_terrainBatch            = nullptr;     // batch base (B4 sets)
int   g_playerCount             = 0;           // DAT_00803320 (B4/A sets)

// --- residual engine deps (stubbed; real RVAs cited) ------------------------
// FUN_004c3df0 — RwV3dTransformPoints. Real impl: Math/RwV3dTransformPoints.cpp.
void Rw_TransformPoints(float* dst, const float* src, int count, void* /*mtx*/) {
    for (int i = 0; i < count * 3; ++i) dst[i] = src[i];   // identity until wired
}
// FUN_004c3d90 — RW vtable contact query (indirect through DAT_007d3ffc table).
void Rw_VtableDispatch(void* /*dst*/, void* /*src*/, int /*count*/, void* /*mtxBlock*/) {}
// FUN_004c4d20 — RwMatrix from axis+angle. Real impl: Math/RwMatrixRotate.cpp.
void Rw_MatrixFromAxisAngle(void* /*outMtx*/, const float* /*axis*/, float /*deg*/, int /*flag*/) {}
// FUN_004c4dc0 — derive working matrix from the vehicle contact-matrix block.
void Rw_MatrixDerive(void* /*outMtx*/, void* /*srcMtx*/) {}
// FUN_004c52f0 — set rotation on the wheel-ring RW matrix block.
void Rw_SetRotation(void* /*mtxBlock*/, void* /*rot*/, int /*mode*/) {}
// FUN_004a3384 — acos approximation.
float Math_Acos(double /*x*/) { return 0.0f; }
// FUN_004a2c48 — monotone tick counter.
int Sys_TickCount() { return 0; }
// FUN_0040e350 — game-mode discriminator (race modes 6/7/10/0xb).
int Game_Mode() { return 0; }
// FUN_00538c80 — RW broadphase walk filling the terrain batch (DAT_0088e60c).
void Rw_BroadphaseWalk(void* /*world*/, void* /*box*/, void* /*cb*/, void* /*user*/) {}
// FUN_00485370 / FUN_00485360 / FUN_00485420 — dynamic-object contact list.
float* Obj_ListBase() { static float dummy[36] = {0}; return dummy; }   // &DAT_006e87b8
int    Obj_ListCount() { return 0; }                                    // DAT_006fa0f8
void   Obj_ReadWorldPos(void* /*obj*/, float* outPos) { outPos[0]=outPos[1]=outPos[2]=0.0f; }

}  // namespace Collision
}  // namespace mashed_re
