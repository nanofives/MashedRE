// Mashed RE — WS-B2/B3 contact-solver public entry points.
//
// These are the verbatim ports of the original contact subset (see the .cpp
// files for RVA citations + fidelity notes). Wired into the per-frame vehicle
// update by WS-B4 (replacing the ground-raycast scaffold) once WS-A's consumer
// FUN_0046ddb0 drives them. Until then they are inert (the residual engine deps
// in ContactDeps.h / ContactStubs.cpp are stubbed).
#pragma once

namespace mashed_re {
namespace Collision {

// --- B2: car<->world --------------------------------------------------------
void TriangleFaceNormal(const float* apex, const float* p1, const float* p2, float* outN); // 0x0046c5f0
int  ContactHistoryLookup(const void* entry, int* veh);                                     // 0x00468b40
void WheelTerrainContactClassifier(int* veh, float* batchEntry);                            // 0x0046cc40
void VehicleTerrainContactSolver(int* veh, void* batchBase);                                // 0x00468d80
void VehicleObjectContactSolver(int* veh);                                                  // 0x004694e0
int  VehicleContactHistoryUpdate(int* veh);                                                 // 0x00469aa0

// --- B3: car<->car ----------------------------------------------------------
bool VehicleCarCarContact(int* vehA, int* vehB, int pass);                                  // 0x00469df0

// --- B4 producer half: the terrain-batch producer (LAB_00468b80 + broadphase) ---
struct ContactBatchEntry;  // defined in ContactConstants.h
struct CollTriangle { float v0[3], v1[3], v2[3]; float normal[3]; int material; int surfaceKey; };
void FillBatchEntry(ContactBatchEntry* e, const float* v0, const float* v1, const float* v2,
                    const float* faceNormal, int material, int surfaceKey);                 // LAB_00468b80
int  ProduceTerrainBatch(const float* center, float radius,
                         const CollTriangle* tris, int triCount);                            // FUN_00538c80 stand-in

// The standalone's COLLI*.BSP triangle list the wheel solver's broadphase walks
// (the FUN_00538c80 query over the collision world). B4 wiring (or a self-test)
// points these at the loaded track collision triangles.
extern const CollTriangle* g_worldTris;
extern int                 g_worldTriCount;

// The 4 transformed wheel-contact world positions (DAT_0088e620; the classifier
// FUN_0046cc40 reads them). In the original these are filled by the RW device
// transform FUN_004c3d90 (stubbed inert standalone), so the WS-A contact wiring
// fills them itself from the wheel mounts before calling WheelContactSolver.
extern float g_wheelContactPos[12];

// DAT_00881560 — per-wheel suspension/steer scratch (12 floats) the wheel solver's
// reset loop populates and the force integrator (A5) reuses for its steer-feedback
// normal. Exposed so the WS-A contact wiring can save/restore it across the solver
// call (the standalone steer path is a separate calibrated substitute).
extern float g_suspScratch[12];

// 0x0046f6c0 — the wheel solver / contact orchestrator: runs the broadphase +
// classifier and the per-wheel 3-state machine that sets the wheel states the
// force integrator (FUN_0046ddb0) reads as the grounded count.
void WheelContactSolver(int* self, void* world, int substep);

}  // namespace Collision
}  // namespace mashed_re
