// Mashed RE — WS-A8: standalone driver for the ported vehicle-physics chain.
// See VehiclePhysicsRun.h for status + the documented approximations.
//
// Dispatcher scale constants harvested from FUN_00470c70 (Ghidra pool13, 2026-06-17):
//   _DAT_005cea80 = 0x3b360bc0 = 0.00277780  (= 1/360; suspDtTerm = dt * this)
//     CORRECTED 2026-08-26: this said 0.0027809, which is a WRONG DECIMAL GLOSS —
//     0.0027809f compiles to 0x3b363fc3, not the image's 0x3b360bc0. The hex was
//     right and the decimal was not, which is the documented 'plate hex gloss'
//     trap in this project. Verified by reading .rdata straight out of the PE.
//   _DAT_005ccd08 = 0x453b8000 = 3000.0      (suspScale  = this / suspDtTerm)
#include "VehiclePhysicsRun.h"
#include "VehicleStruct.h"
#include "ForceIntegrator.h"                 // extern globals + VehicleWheelForceIntegrate
#include "../Collision/ContactSolvers.h"     // WheelContactSolver (B4)
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <vector>
#include <chrono>     // WS-A s3 perf profiler (steady_clock == QPC on MSVC)
// #define MASHED_PHYS_DIAG 1   /* enable the steer-chain diag block (G2 drive debug) */

namespace mashed_re {
namespace Vehicle {

// ===========================================================================
// WS-A s3 (PERF): env-gated frame-cost profiler. Attributes the in-race frame
// time to the physics step vs the per-wheel terrain probe so the hot path is
// MEASURED, not assumed. Writes a per-frame CSV to phys_prof.log (cwd-relative;
// = the MASHED_ROOT main repo for worktree runs). Off unless MASHED_PHYS_PROF
// is set, so normal/shipping runs pay nothing.
//   MASHED_PHYS_NOCONTACT=1 — skip the wheel-contact solver (flat-ground
//   substitute) to isolate the probe cost via an A/B run.
// ===========================================================================
namespace prof {
inline double NowMs() {
    return std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
static const bool g_on        = (std::getenv("MASHED_PHYS_PROF")      != nullptr);
static const bool g_noContact = (std::getenv("MASHED_PHYS_NOCONTACT") != nullptr);
// Per-frame accumulators (summed across StepPlayer(0)+StepCar(1..3); flushed at
// the next slot-0 step, which is the frame boundary — player always steps first).
double    f_physMs   = 0.0;  // total time inside StepCar bodies this frame
double    f_probeMs  = 0.0;  // total time inside SolveWheelContacts this frame
long long f_triTests = 0;    // ProbeGround per-triangle tests this frame
long long f_batchTests = 0;  // ProduceTerrainBatch per-triangle tests this frame
int       f_substeps = 0;    // substep iterations this frame
int       f_cars     = 0;    // cars stepped this frame
float     f_wallMs   = 0.f;  // frame wall dt (the dt passed to slot 0)
int       g_triCount = 0;    // terrain soup triangle count (set at SetWorld)
int       g_frame    = 0;    // logged-frame counter
bool      g_have     = false;// a frame is in progress
void Flush() {
    if (!g_on || !g_have) return;
    static std::FILE* lf = nullptr;
    static bool opened = false;
    if (!opened) { lf = std::fopen("phys_prof.log", "w");
                   if (lf) std::fprintf(lf, "frame,wall_ms,phys_ms,probe_ms,tri_tests,batch_tests,substeps,cars,tris\n");
                   opened = true; }
    if (lf && g_frame < 6000) {
        std::fprintf(lf, "%d,%.3f,%.3f,%.3f,%lld,%lld,%d,%d,%d\n",
            g_frame, f_wallMs, f_physMs, f_probeMs, f_triTests, f_batchTests,
            f_substeps, f_cars, g_triCount);
        std::fflush(lf);
    }
    ++g_frame;
    f_physMs = f_probeMs = 0.0; f_triTests = f_batchTests = 0;
    f_substeps = 0; f_cars = 0;
}
}  // namespace prof
// Exposed so the broadphase (ContactProducer.cpp) can add its per-tri test count.
long long* PerfBatchTestCounter() { return prof::g_on ? &prof::f_batchTests : nullptr; }

// Chain entry points (defined in their .cpp; declared here to avoid a header churn).
int  VehicleInit(int slot, int trackType);                                       // A3 0x0046b540
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xf, int car); // A4 0x00470670
// U-D3-DRIVE-FORCE: the A6a start-boost order/participant bindings (ForceIntegratorStubs.cpp).
extern int g_participantCount;   // DAT_008a94d0 (FUN_0040e340)
void Fi_UpdateBoostOrder();      // FUN_00470c70 0x00470e2e..0x00470f0a

// A8 body-orientation integrator (BodyOrientationIntegrate.cpp = FUN_0046e9e0's
// orientation half + the 0x004c4680 ortho-normalize). Wired 2026-08-25.
void  BodyOrient_Init(float* m, float yaw);
void  BodyOrient_IntegrateStep(float* m, const float omega[3]);
void  BodyOrient_OmegaFromSteer(void* rec, float dtMs, const std::uint8_t* in, float omegaOut[3]);
float BodyOrient_Heading(const float* m);
extern int g_torqueRingPhase;   // DAT_007f101c (defined in ForceIntegratorStubs.cpp)

// Build a vehicle WORLD-transform RwMatrix (the layout RwV3dTransformPointsCPU /
// RwMatrixRotate use: right@m[0..2], flags@m[3], up@m[4..6], at@m[8..10],
// pos@m[12..14]) for a heading `yaw` so that transforming the body forward axis
// (0,0,1) yields the car's heading {cos(yaw),0,sin(yaw)} (the TrackRenderer/adapter
// forward convention). In the original this matrix comes from the live RW scene
// graph; the standalone has no RW device, so we synthesize it from the car's yaw.
//   at    = (cos, 0, sin)         [forward; transform of (0,0,1)]
//   up    = (0, 1, 0)
//   right = up x at = (sin, 0, -cos)   [transform of (1,0,0)]
static void BuildYawMatrix(float yaw, float* m /*[16]*/) {
    const float c = std::cos(yaw), s = std::sin(yaw);
    m[0]  =  s; m[1]  = 0.f; m[2]  = -c; m[3]  = 0.f;   // right
    m[4]  = 0.f; m[5] = 1.f; m[6]  = 0.f; m[7]  = 0.f;  // up
    m[8]  =  c; m[9]  = 0.f; m[10] =  s; m[11] = 0.f;   // at (forward)
    m[12] = 0.f; m[13] = 0.f; m[14] = 0.f; m[15] = 0.f; // pos (origin; only axes used)
}

namespace {
constexpr std::size_t kRec   = 0xd04;          // record stride (== sizeof, vehicle.md)
constexpr float       kSuspDtK = 0.0027778f;   // _DAT_005cea80 = 0x3b360bc0 (= 1/360)
                                               // was 0.0027809f -> 0x3b363fc3, WRONG BITS
constexpr float       kSuspNum = 3000.0f;      // _DAT_005ccd08
// [U-A8-SUBSTEP] CORRECTED 2026-08-25. The dispatcher FUN_00470c70 takes a budget
// of 0x32 (50) units per frame and its OUTER chunk is min(remaining, 0x32) — but
// that outer chunk is then split again: FUN_004709a0 is called twice with 25.0
// each (inner cap 0x19 = 25), and FUN_0046e9e0 runs once per vehicle per call
// (twice only on the collision retry guarded by `if (1 < iVar10)` at LAB_00470c47).
// So the substep granularity is 25 units, not 50. Decode:
// re/analysis/data/A8_substep_budget_20260825.md (Q2).
constexpr int         kMaxSubstep = 25;        // 0x19 (FUN_004709a0 inner chunk)

unsigned char g_records[16 * kRec];            // the 0xd04 record array (mirror of DAT_008815a0)
bool          g_inited = false;

// A8 (2026-08-25): persistent integrated BODY ORIENTATION basis per car — the
// state FUN_0046e9e0's `dst[row] = src[row] + (omega x src[row])` advances. This
// REPLACES the old `g_bodySpeed` scalar + PD relaxation, which existed only to
// carry a heading-plus-scalar motion model that cannot represent slip.
// The original keeps this basis in the record's +0x928/+0x968 double buffer; the
// port holds it here because our +0x928 block is already used as the contact ring
// (SolveWheelContacts writes through it) — the deviation is stated in full in
// BodyOrientationIntegrate.cpp's BodyOrient_Init comment. Only the storage
// location differs; the arithmetic is the original's.
float         g_bodyBasis[16][16] = { { 0.f } };
bool          g_bodyBasisOk[16]   = { false };
// Set by VehiclePhysics_ResetOrientation, reported by MASHED_MOTION_DIAG and
// cleared there. A reseed is a HEADING DISCONTINUITY (spawn, grid placement,
// off-mesh recovery re-aim), not an integration step, so any yaw-RATE statistic
// must exclude the sample pair that straddles one. Without this marker a recovery
// re-aim shows up as a huge instantaneous yaw rate and inflates the bands — the
// same artifact class as the round-boundary respawn in the PLAY-DEMO metric.
bool          g_bodyBasisReseed[16] = { false };
// [U-D3-DRIVE 2026-09-28] one-shot spawn-settle flag per slot; cleared by
// VehiclePhysics_Init. See the SPAWN SETTLE block in VehiclePhysics_StepCar.
bool          g_settled[16] = { false };
// [U-D3-DRIVE-FORCE 2026-09-29] one-shot start-boost arm flag per slot; cleared by
// VehiclePhysics_Init. See the START BOOST ARM block in VehiclePhysics_StepCar.
bool          g_startBoosted[16] = { false };
// Re-entrancy marker for the spawn settle's inner VehiclePhysics_StepCar call. The
// settle deliberately steps the chain once with an all-zero input block and then
// DISCARDS what it produced, so the start boost must not be armed inside it (that
// call runs with the post-memset wheel states, where A6a's drive/boost gate
// `p[-0xf] == 2` is false, so the boost would be armed and then sit unconsumed).
bool          g_inSpawnSettle = false;

// A8 position law constants (re/analysis/data/A8_position_law_20260825.md).
// FUN_0046e9e0 builds the per-substep position increment as
//   inc = dtMs * _DAT_005cc948 * _DAT_005cea80 * linVel
// with the two FMULs at 0x0046e9e8 and 0x0046e9f6. kSuspDtK below is already
// _DAT_005cea80; this is the other factor.
// AUDITED 2026-08-26: 3.33320e-4f compiles to the WRONG BITS. The image's
// 0x39aec33e is 0.00033333333 (= 1/3000). Same "plate hex gloss" trap as
// kSuspDtK. Built bit-exactly from the pattern instead. Error was 0.004%.
const     float kPosDtK = [] { std::uint32_t b = 0x39aec33e; float f;
                               std::memcpy(&f, &b, 4); return f; }();  // _DAT_005cc948

// Terrain contact soup (built from the track collision triangles) the B4 wheel
// solver's broadphase walks via Collision::g_worldTris.
std::vector<Collision::CollTriangle> g_worldTriStore;

inline float& F(unsigned char* r, std::size_t o) { return *reinterpret_cast<float*>(r + o); }
inline int&   I(unsigned char* r, std::size_t o) { return *reinterpret_cast<int*>(r + o); }
inline unsigned char* rec(int slot) { return g_records + static_cast<std::size_t>(slot) * kRec; }
}  // namespace

// [terrain] Build the wheel solver's contact soup from the track collision tris
// (TrackRenderer col_verts_/col_tris_) so WheelContactSolver reports grounded
// wheels -> A5's suspension force is no longer inert. verts = x,y,z flat;
// tris = 3 vertex indices per triangle.
void VehiclePhysics_SetWorld(const float* verts, int vertCount,
                             const unsigned* tris, int triCount,
                             const unsigned* mats) {
    g_worldTriStore.clear();
    if (!verts || !tris || triCount <= 0) {
        Collision::g_worldTris = nullptr; Collision::g_worldTriCount = 0; return;
    }
    g_worldTriStore.reserve(static_cast<std::size_t>(triCount));
    for (int t = 0; t < triCount; ++t) {
        const unsigned i0 = tris[t * 3 + 0], i1 = tris[t * 3 + 1], i2 = tris[t * 3 + 2];
        if ((int)i0 >= vertCount || (int)i1 >= vertCount || (int)i2 >= vertCount) continue;
        Collision::CollTriangle ct{};
        for (int k = 0; k < 3; ++k) {
            ct.v0[k] = verts[i0 * 3 + k];
            ct.v1[k] = verts[i1 * 3 + k];
            ct.v2[k] = verts[i2 * 3 + k];
        }
        const float e1x = ct.v1[0]-ct.v0[0], e1y = ct.v1[1]-ct.v0[1], e1z = ct.v1[2]-ct.v0[2];
        const float e2x = ct.v2[0]-ct.v0[0], e2y = ct.v2[1]-ct.v0[1], e2z = ct.v2[2]-ct.v0[2];
        float nx = e1y*e2z - e1z*e2y, ny = e1z*e2x - e1x*e2z, nz = e1x*e2y - e1y*e2x;
        const float m = std::sqrt(nx*nx + ny*ny + nz*nz);
        if (m > 1e-12f) { nx/=m; ny/=m; nz/=m; }
        ct.normal[0]=nx; ct.normal[1]=ny; ct.normal[2]=nz;
        // material index -> contact entry +0x30 -> record +0x1ec (0x0046d00b).
        ct.material = mats ? static_cast<int>(mats[t]) : 0;
        // surfaceKey (-> entry +0x34 -> record +0x1f0) still 0: the original puts
        // the material-table COLOUR here via [0x88e654]->+8->+0x10->[matIdx*4]->+0x4
        // (0x00468bbc), and the collision material table is not retained yet.
        // Wiring it is a separate step, gated on the rgba-packing and
        // collision-vs-render index-space questions. Do NOT hardcode a magic.
        ct.surfaceKey = 0;
        g_worldTriStore.push_back(ct);
    }
    Collision::g_worldTris     = g_worldTriStore.empty() ? nullptr : g_worldTriStore.data();
    Collision::g_worldTriCount = static_cast<int>(g_worldTriStore.size());
    prof::g_triCount = Collision::g_worldTriCount;
}

bool VehiclePhysics_Enabled() {
    // ROADMAP v3 D2, inverted 2026-09-14: the ported RWP-3.7 chain is the DEFAULT drive
    // model. `MASHED_REAL_PHYSICS=0` reverts to the kinematic scaffold for A/B only (the
    // v3 default-build rule: a flag may only turn the ported behaviour OFF). Evidence:
    // re/analysis/data/A8_velocity_vector_motion_20260825.md follow-ups 25-27 — on the
    // held full-lock recipe the default order matches the original: slip 0.192/0.263 vs
    // 0.191/0.250, av.y 1.12/1.58 vs 1.14/1.46, driving-median speed 1874 vs 1901.
    static const bool e = [] {
        const char* v = std::getenv("MASHED_REAL_PHYSICS");
        return !(v && v[0] == '0');
    }();
    return e;
}

void VehiclePhysics_Init(int carCount, int trackType) {
    std::memset(g_records, 0, sizeof(g_records));
    for (int s = 0; s < 16; ++s) g_bodyBasisOk[s] = false;   // A8: basis reseeds at first step
    for (int s = 0; s < 16; ++s) g_settled[s] = false;       // U-D3-DRIVE spawn settle
    for (int s = 0; s < 16; ++s) g_startBoosted[s] = false;  // U-D3-DRIVE-FORCE start boost
    // DAT_008a94d0 stand-in for FUN_0040e340: the A6a start-boost 8e6 arm tests it == 4.
    g_participantCount = carCount;
    Fi_UpdateBoostOrder();                                   // seed DAT_0088e668/66c
    // The integrator's "other cars" base (DAT_008815a0) -> our standalone array.
    g_vehicleArrayBase = reinterpret_cast<int*>(g_records);
    if (carCount < 1)  carCount = 1;
    if (carCount > 16) carCount = 16;
    g_playerCount = carCount;
    for (int s = 0; s < carCount; ++s) VehicleInit(s, trackType);  // A3: suspension/mass/geom
    g_inited = true;
}

// ===========================================================================
// WS-A CONTACTS (2026-06-29): replace the flat-ground SetGrounded substitute with
// the ported first-party wheel-contact solver Collision::WheelContactSolver
// (FUN_0046f6c0) run over the track collision tris fed via VehiclePhysics_SetWorld.
//
// What the original chain consumes from contacts (all RVA-cited this session,
// Ghidra pool0 read_only, MASHED.exe BDCAE0…):
//   * wheel STATE  byte +0x198 + w*0xc4  — FUN_0046f6c0's 3-state machine sets it;
//     A5 FUN_0046ddb0 Phase 0 sums the states into the grounded count, Phase 6
//     gates the per-wheel suspension on it (pf[-0x17]==0 -> wheel not in contact).
//   * contact NORMAL vec3 +0x200 + w*0xc4 — the classifier FUN_0046cc40 writes the
//     contacted triangle's face normal here (local_90[0x1b..0x1d]); A5 Phase 6
//     reads it as pf[3..5] and builds the per-wheel suspension force along it
//     (pf[8]=fc*pf[3], pf[9]=(pf[4]-1)*fc, pf[10]=fc*pf[5]). THIS is "slope normals".
//   * contact LOAD  +0x20c + w*0xc4 (A5 pf[6]) — NOT supplied by the contact solver:
//     A5 itself WRITES it in Phase 5 (ws[0].outSlot=0x83 -> +0x20c) =
//     (mass / grounded_count) * g_suspDtTerm + lateral_proj. So WEIGHT TRANSFER is
//     intrinsic to A5: fewer grounded wheels -> more load each; the lateral proj
//     adds cornering transfer. The old SetGrounded load=1.0 write was overwritten
//     by A5 Phase 5 (moot) — we no longer write +0x20c at all.
//
// So the behavioural change is the per-wheel NORMAL (slope) + per-wheel GROUNDING
// (which feeds A5's mass/grounded_count load = weight transfer). The wheel-mount
// world positions are a substitute (BuildYawMatrix yaw rotation, no roll/pitch —
// adequate on flat, approximate on slopes; blessed for this step).
// ===========================================================================
namespace {
constexpr std::size_t kWheelStride  = 0xc4;     // per-wheel record stride (0x31 ints)
constexpr std::size_t kWheelState0  = 0x198;    // wheel-0 state (== off::kWheel0State)
constexpr std::size_t kContactNorm0 = 0x200;    // wheel-0 contact normal (A5 pf[3..5])
constexpr std::size_t kMountX0      = 0x16c;    // wheel-0 mount x (FUN_0046b540 +0x16c)
constexpr std::size_t kMountZ0      = 0x174;    // wheel-0 mount z (FUN_0046b540 +0x174)
constexpr float       kProbeScale   = 0.277779f;// _DAT_005cea60 (FUN_0046f6c0 reset: probe = mount*this)
constexpr float       kPenetration  = 0.05f;    // seat probe this far below the surface (classifier
                                                // depth band [_DAT_005cea5c -0.25, _DAT_005cc564 0.25))
inline std::size_t wheelStateOff (int w) { return kWheelState0  + (std::size_t)w * kWheelStride; }
inline std::size_t wheelNormOff  (int w) { return kContactNorm0 + (std::size_t)w * kWheelStride; }
inline std::size_t wheelMountXOff(int w) { return kMountX0      + (std::size_t)w * kWheelStride; }
inline std::size_t wheelMountZOff(int w) { return kMountZ0      + (std::size_t)w * kWheelStride; }

// Per-call contact cache (StepCar processes one car fully before the next, so a
// single buffer is safe): lets the post-A4 re-assert restore the same states+normals
// without re-running the broadphase.
bool  g_cOnMesh[4]    = { false, false, false, false };
float g_cNormal[4][3] = { {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0} };
float g_cAvgN[3]      = { 0.f, 1.f, 0.f };   // averaged on-mesh normal (edge fallback)
bool  g_cGrounded     = false;
}  // namespace

// Highest collision triangle under world XZ (x,z) from the soup VehiclePhysics_SetWorld
// built (g_worldTriStore, with precomputed face normals). Returns ground Y + UPWARD
// face normal. Standalone substitute for the un-portable RW BSP broadphase walk
// FUN_00538c80 (same COLLI*.BSP triangles); mirrors TrackRenderer::GroundProbe.
static bool ProbeGround(float x, float z, float& gy, float* n /*[3]*/) {
    bool found = false; float best = -1e30f;
    n[0] = 0.f; n[1] = 1.f; n[2] = 0.f;
    if (prof::g_on) prof::f_triTests += (long long)g_worldTriStore.size();  // PERF: scan size
    for (const Collision::CollTriangle& tr : g_worldTriStore) {
        const float* a = tr.v0; const float* b = tr.v1; const float* c = tr.v2;
        const float d00x = b[0]-a[0], d00z = b[2]-a[2];
        const float d01x = c[0]-a[0], d01z = c[2]-a[2];
        const float den = d00x*d01z - d01x*d00z;
        if (den > -1e-9f && den < 1e-9f) continue;
        const float px = x-a[0], pz = z-a[2];
        const float u = (px*d01z - d01x*pz) / den;
        const float v = (d00x*pz - px*d00z) / den;
        if (u < 0.f || v < 0.f || u+v > 1.f) continue;
        const float y = a[1] + u*(b[1]-a[1]) + v*(c[1]-a[1]);
        if (y > best) {
            best = y; found = true;
            const float s = (tr.normal[1] < 0.f) ? -1.f : 1.f;   // force upward (drive surface)
            n[0] = tr.normal[0]*s; n[1] = tr.normal[1]*s; n[2] = tr.normal[2]*s;
        }
    }
    gy = best; return found;
}

// Apply the cached contact result to the record: write each wheel's REAL per-wheel
// terrain normal (A5 pf[3..5]), set its grounded state, and recompute the grounded
// count (+0x9e0). DRIVABILITY GATE: the car-on-track fact (caller's GroundHeight,
// io.grounded) grounds ALL 4 wheels — matching the old SetGrounded gate — because
// A5 Phase 0 (FUN_0046ddb0) RE-DERIVES the grounded count from the wheel states, so
// demoting an off-mesh wheel to state 0 halves A5's drive force (Phase 1 `!=4 ->
// *0.5`) and weakens the front-wheel steer, which wedged the car at track edges
// (Arctic regression). Off-mesh wheels at the edge fall back to the averaged on-mesh
// (slope) normal; on-mesh wheels keep their own per-wheel terrain normal — so slope
// normals are preserved while drivability matches the old flat path.
static void ReassertContacts(unsigned char* r) {
    float gc = 0.f;
    for (int w = 0; w < 4; ++w) {
        if (g_cGrounded) {
            if (I(r, wheelStateOff(w)) == 0) I(r, wheelStateOff(w)) = 1;   // ground (on-track gate)
            const float* nrm = g_cOnMesh[w] ? g_cNormal[w] : g_cAvgN;      // real per-wheel, or slope avg
            const std::size_t nb = wheelNormOff(w);
            F(r, nb + 0x0) = nrm[0];              // contact normal.x  (A5 Phase 6 pf[3])
            F(r, nb + 0x4) = nrm[1];              // contact normal.y  (A5 Phase 6 pf[4])
            F(r, nb + 0x8) = nrm[2];              // contact normal.z  (A5 Phase 6 pf[5])
        } else {
            I(r, wheelStateOff(w)) = 0;           // airborne (caller's GroundHeight failed)
        }
        if (I(r, wheelStateOff(w)) != 0) gc += 1.f;
    }
    F(r, off::kGroundedCnt) = gc;   // +0x9e0; 4.0 (0x40800000) == all-grounded sentinel
}

// [U-9154 2026-09-29] Mirror the integrated body basis into the record's +0x928
// contact ring, so the ring holds what the ORIGINAL's ring holds.
//
// The original's +0x928 block IS the body RwMatrix: A4 computes
// `record + [record+0x9a8]*0x40 + 0x928` at 0x0047068e/0x00470696/0x00470699 and hands
// that one pointer to A5 (0x00470918) and A6b (0x0047093b), and A6b feeds it to
// RwMatrixRotate as the matrix argument (0x00468a31 etc.) — decode and live witness in
// re/analysis/D2_REOPEN_2026-09-29.md §4.1/§4.2, and §13.1 witness 4 reads the
// original's WORLD POSITION out of its translation row (+0x958/+0x95c/+0x960).
// The port keeps the basis in g_bodyBasis (U-9152, the storage split) and until now
// wrote ONLY the ring's translation row, leaving rows 0..11 zero.
//
// That zero rotation is why the car-vs-world query could not work even with the
// dispatch bound: `Rw_VtableDispatch(self+0x27e, self+0x18, 0x12, ring)`
// (CarWorldContacts.cpp:407) would map all 18 body points onto the translation.
// This does NOT move the storage — g_bodyBasis stays the authority — it publishes it,
// which is the minimum U-9152 owes the contact chain.
static void SyncContactRingMatrix(unsigned char* r, const float* basis, const float* pos) {
    const int sel = I(r, 0x9ac);                    // self[0x26b]; the ring index
    if (sel < 0 || sel > 1) return;
    float* m = reinterpret_cast<float*>(r + (std::size_t)sel * 0x40 + 0x928);
    for (int i = 0; i < 12; ++i) m[i] = basis[i];   // right / up / at rows
    m[12] = pos[0]; m[13] = pos[1]; m[14] = pos[2]; // translation (+0x30)
    m[15] = 0.f;
}

// Run the ported FUN_0046f6c0 over the track tris (replaces SetGrounded).
static void SolveWheelContacts(unsigned char* r, const PlayerCarIO& io, int substep,
                               const float* bodyUp) {
    int* self = reinterpret_cast<int*>(r);

    // PERF A/B (MASHED_PHYS_NOCONTACT): skip the terrain probe + solver entirely
    // (flat-ground substitute) to isolate the probe cost — measures the doom-loop
    // floor with contacts disabled. Not a shipping path; profiling only.
    if (prof::g_noContact) {
        g_cGrounded = (io.grounded != 0);
        for (int w = 0; w < 4; ++w) {
            g_cOnMesh[w] = false;
            g_cNormal[w][0] = 0.f; g_cNormal[w][1] = 1.f; g_cNormal[w][2] = 0.f;
        }
        g_cAvgN[0] = 0.f; g_cAvgN[1] = 1.f; g_cAvgN[2] = 0.f;
        ReassertContacts(r);
        return;
    }

    // 1. yaw rotation for the wheel-mount world placement (BuildYawMatrix: right@[0],[2];
    //    forward/at@[8],[10]; no roll/pitch — the blessed flat substitute).
    float rot[16]; BuildYawMatrix(io.yaw, rot);

    // 2. per wheel: world XZ of the 0.2778-scaled suspension probe (FUN_0046f6c0's reset
    //    loop scales the mount by _DAT_005cea60), then query the real terrain.
    g_cGrounded = (io.grounded != 0);
    float avgN[3] = { 0.f, 0.f, 0.f }; int nOn = 0; float sumGY = 0.f; int nGY = 0;
    for (int w = 0; w < 4; ++w) {
        const float mx = F(r, wheelMountXOff(w)) * kProbeScale;
        const float mz = F(r, wheelMountZOff(w)) * kProbeScale;
        const float wx = io.pos[0] + rot[0]*mx + rot[8]*mz;    // + right.x*mx + at.x*mz
        const float wz = io.pos[2] + rot[2]*mx + rot[10]*mz;   // + right.z*mx + at.z*mz
        float gyw = 0.f, nw[3];
        const bool ok = ProbeGround(wx, wz, gyw, nw);
        g_cOnMesh[w] = ok;
        g_cNormal[w][0] = ok ? nw[0] : 0.f;
        g_cNormal[w][1] = ok ? nw[1] : 1.f;
        g_cNormal[w][2] = ok ? nw[2] : 0.f;
        float* wcp = &Collision::g_wheelContactPos[w * 3];     // DAT_0088e620 (classifier reads this)
        wcp[0] = wx; wcp[2] = wz;
        if (ok) {
            wcp[1] = gyw - kPenetration;                       // just below surface -> depth in band
            avgN[0] += nw[0]; avgN[1] += nw[1]; avgN[2] += nw[2]; ++nOn;
            sumGY += gyw; ++nGY;
        } else {
            wcp[1] = io.pos[1] + 1.0e4f;                       // far above any tri -> no contact
        }
    }
    if (nOn > 0) { avgN[0] /= nOn; avgN[1] /= nOn; avgN[2] /= nOn; }
    else { avgN[0] = 0.f; avgN[1] = 1.f; avgN[2] = 0.f; }
    g_cAvgN[0] = avgN[0]; g_cAvgN[1] = avgN[1]; g_cAvgN[2] = avgN[2];  // edge fallback (ReassertContacts)

    // 3. solver preconditions. iVar12 = self + self[0x26b]*0x40 + 0x928 (wheel-ring
    //    matrix double-buffer, init self[0x26b]=1); the broadphase query centre is
    //    iVar12+0x30..0x38. Seat its Y at ground level so the ground-tri plane distance
    //    < the init radius (+0x4a4) and the broadphase gathers it.
    const int sel = I(r, 0x9ac);   // self[0x26b]
    float* centre = reinterpret_cast<float*>(r + (std::size_t)sel * 0x40 + 0x928 + 0x30);
    centre[0] = io.pos[0];
    centre[1] = (nGY > 0) ? (sumGY / nGY) : io.pos[1];
    centre[2] = io.pos[2];
    // +0x9c8/+0x9cc/+0x9d0: the classifier's approach direction (FUN_0046cc40 gate
    // _DAT_005cc99c=0.3 < faceNormal . this) AND `BodyOrient_OmegaFromSteer`'s steer
    // ROTATION AXIS (it reads &Fb(rec,0x9c8)) AND the slide term in 0x0046ef70's tail
    // (`fmul [edi+0x9c8]` @0x0046f644/0x0046f666). Read back by FUN_0046d700, which is a
    // pure getter: 0x0046d710 `imul eax,eax,0xd04` / 0x0046d716 `mov edx,[eax+0x881f68]`
    // + the two following dwords, and 0x00881f68 - 0x008815a0 (the record base) = 0x9c8.
    //
    // [U-9156 2026-09-30] IT IS THE BODY'S OWN UP AXIS, NOT THE AVERAGED TERRAIN NORMAL.
    // The old line below seeded it with the averaged per-wheel ground normal, which its
    // own comment marked [UNCERTAIN]. Measured against the anchored original, that is
    // wrong on the D2 reference scenario:
    //   * On all six Training solo captures (verify/d2_reopen_20260929/orig_solo{3,4}.msd,
    //     verify/a8_steer_20260824/orig_steer{R,L}.msd, orig_a8_elim.msd, peek1.msd;
    //     1441..1444 driving frames each) +0x9c8 and +0x9d0 are NUMERICALLY ZERO on
    //     EVERY frame (max|x| = max|z| = 0.000e+00) and +0x9cc lies in
    //     [0.999923, 1.000077].
    //   * The surface there is NOT flat: the original's own pos.y (+0x95c) climbs
    //     0.38063 -> 0.38864 while pos.z goes -3.741 -> -3.452 (orig_solo3 frames
    //     973..1010), a 1.58 deg z-tilt. So the field cannot be the local ground normal.
    //   * The port's probe returns the real tri normal there, (0, 0.99951, -0.031236)
    //     = a 1.79 deg z-tilt, constant over the whole run, and the port's own pos.y
    //     range 0.42028..0.47480 over pos.z -3.748..-2.003 confirms the tri really is
    //     tilted. So the two sides disagree about the FIELD, not about the geometry.
    //   * The off-unit magnitude (0.99992..1.00008) is the signature of an
    //     ortho-normalized matrix row, and the original's +0x928 `up` row carries the
    //     same character (0.99990..1.00006) with x/z also zero.
    //   * It does tilt on other captures (orig_ramp.msd max|x| = 1.000, c2.msd 0.996),
    //     i.e. it is not hardcoded — it tracks the body, which on a gentle incline the
    //     suspension keeps level and on a ramp does not.
    // Consequences of the old value, all measured on verify/u9156b_20260930/p1:
    // the steer torque w = axis * t acquired a world-Z component every step, so the
    // integrated basis pitched (record +0x9d8 = +0.04587 where the original's is BITWISE
    // ZERO on 2333/2333 frames), so ForceIntegrator.cpp:40's forward = xform*(0,0,1)
    // pointed 2.63 deg above horizontal and the drive force grew a vertical component
    // (record +0x9b4 = +74.84 at the first wall contact where the original's is BITWISE
    // ZERO on all 981 pre-contact frames -- this closes §17.4's open [UNCERTAIN]).
    // MASHED_STEER_AXIS_TERRAIN=1 reverts to the averaged terrain normal for A/B only.
    static const bool s_axisTerrain = [] {
        const char* e = std::getenv("MASHED_STEER_AXIS_TERRAIN");
        return e && e[0] == '1';
    }();
    if (s_axisTerrain || !bodyUp) {
        F(r, 0x9c8) = avgN[0]; F(r, 0x9cc) = avgN[1]; F(r, 0x9d0) = avgN[2];
    } else {
        F(r, 0x9c8) = bodyUp[0]; F(r, 0x9cc) = bodyUp[1]; F(r, 0x9d0) = bodyUp[2];
    }

    // 4. the real first-party solver: 3-state machine + broadphase + classifier +
    //    velocity-friction impulse + airborne drift + grounded count. `world` is unused
    //    by the port (ProduceTerrainBatch reads Collision::g_worldTris directly).
    //    The solver's reset loop overwrites the shared DAT_00881560 scratch with the
    //    wheel mounts, which A5 Phase 5 (FUN_0046ddb0) would then feed into its steer-
    //    feedback TriangleFaceNormal — perturbing the standalone's SEPARATELY-calibrated
    //    steer path (kYawScale/kWorldVel) and bleeding speed in turns (Arctic regression).
    //    This task changes CONTACTS, not the steer integrator, so isolate the scratch:
    //    save it, run the solver, restore it (the steer path is unchanged from baseline).
    float savedScratch[12];
    std::memcpy(savedScratch, Collision::g_suspScratch, sizeof(savedScratch));
    unsigned char world[16] = { 0 };
    Collision::WheelContactSolver(self, world, substep);
    std::memcpy(Collision::g_suspScratch, savedScratch, sizeof(savedScratch));

    // 5. apply the cached result (states + real per-wheel normals + grounded count).
    ReassertContacts(r);
}

// A8 (2026-08-25): (re)seed a slot's body basis. Callers that teleport or re-aim
// the car (spawn, grid placement, off-mesh recovery) must call this, otherwise the
// integrated basis keeps the pre-teleport heading — the basis is now the authority
// for io.yaw, so writing car_yaw_ alone no longer turns the car.
void VehiclePhysics_ResetOrientation(int slot, float yaw) {
    if (slot < 0 || slot >= 16) return;
    BodyOrient_Init(g_bodyBasis[slot], yaw);
    g_bodyBasisOk[slot] = true;
    g_bodyBasisReseed[slot] = true;   // heading discontinuity — see the decl comment
}

float VehiclePhysics_RecordF32(int slot, int off) {
    if (!g_inited || slot < 0 || slot >= 16 || off < 0 || off > static_cast<int>(kRec) - 4) return 0.f;
    return F(rec(slot), static_cast<std::size_t>(off));
}

int VehiclePhysics_RecordI32(int slot, int off) {
    if (!g_inited || slot < 0 || slot >= 16 || off < 0 || off > static_cast<int>(kRec) - 4) return 0;
    return I(rec(slot), static_cast<std::size_t>(off));
}

void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io) {
    VehiclePhysics_StepCar(0, dt, io);
}

void VehiclePhysics_StepCar(int slot, float dt, PlayerCarIO& io) {
    if (!g_inited) VehiclePhysics_Init(4, 0);
    if (dt <= 0.f) return;
    if (slot < 0 || slot >= 16) return;
    unsigned char* r = rec(slot);

    // =====================================================================
    // SPAWN SETTLE — U-D3-DRIVE, 2026-09-28. MEASURED, not inferred.
    //
    // VehiclePhysics_Init memsets the record, so the FIRST step of a race runs with
    // the per-wheel suspension load (+0x210 / +0x2d4 / +0x398 / +0x45c) at ZERO.
    // Gravity is therefore unopposed for exactly that one step and the body picks up
    // ~216.67 of DOWNWARD velocity, after which it spends ~10 frames settling.
    //
    // The ORIGINAL never has such a step. Read out of its own capture
    // (verify/d3_drive_20260928/e3.msd, re/tools/statediff/msd_fields.py, frames
    // 857-859, i.e. at rest on the grid immediately before the race starts):
    //     +0x9b4 (vy) = 0.000        +0x9e4 (speed) = 0.000     +0xb0c = 0.0000
    //     loads = 1083.3 / 1083.3 / 1083.3 / 541.7
    //     +0x490 (gear) = 0          +0x494 (shift timer) = 0
    // It has been sitting on loaded suspension for the whole countdown (~800 frames
    // on this recipe), because FUN_00470c70 runs during the countdown too.
    //
    // Why the transient is not cosmetic. At the first step the standalone's velocity
    // is almost entirely vertical, so the slide measure +0xb0c — A4's
    // (1 - |fwd . vel| / speed) * speed at VehicleControl.cpp:113, the at-rest zero
    // arm at :106 / 0x0047072c — reads 216.668 instead of ~0. Integrate2.cpp:126
    // (FUN_00467650) then shortens fVar5_base to 1500 - 216.668, so fVar5 falls from
    // 2000 to 1711, and the gear-0 candidate (speed + 250) * 4 = 1862 now EXCEEDS it.
    // That flips the upshift test at Integrate2.cpp:136-143: the port leaves gear 0 on
    // its second step (gear=1, gtmr=2950 measured) and gear 0 is the only gear with a
    // nonzero drive term at standstill (:134, t = (speed+250)*[+0x478]; every other
    // gear is g * speed = 0). Measured consequence, D3_SPEED_GAP_2026-09-28.md §2.3:
    // the launch gains +182 over the first 11 calls where the original gains
    // +1426 / +2053.
    //
    // The fix reproduces the original's MEASURED rest state rather than inventing a
    // schedule: run ONE step with an all-zero input block, which is what establishes
    // the loads (measured: they reach 1083.34 / 1083.34 / 1083.34 / 541.671 after
    // exactly one step, matching the original's 1083.3 / 541.7), then discard the
    // velocity that step injected and reset the gearbox pair. No position is touched —
    // StepCar returns motion through io.drive_delta, which the settle call discards.
    // A/B revert only, per the v3 default-build rule that a flag may only turn the
    // PORTED behaviour off: MASHED_NO_SPAWN_SETTLE=1 restores the pre-fix first step.
    // It exists because the settle REGRESSES AI criterion (b) on cars 2 and 3 (see
    // re/analysis/D3_DRIVE_2026-09-28.md section 4.3), so the two arms have to stay
    // measurable against each other until that is adjudicated.
    static const bool s_noSettle = (std::getenv("MASHED_NO_SPAWN_SETTLE") != nullptr);
    if (!g_settled[slot] && !s_noSettle) {
        g_settled[slot] = true;          // set BEFORE the call: the inner step must not re-enter
        PlayerCarIO s = io;
        for (int k = 0; k < 8; ++k) s.input[k] = 0;
        s.steer = 0.f; s.raw_steer = 1;  // raw_steer so the zero bytes are used verbatim
        s.vel[0] = s.vel[1] = s.vel[2] = 0.f;
        s.speed = 0.f;
        g_inSpawnSettle = true;          // keep the START BOOST ARM out of this call
        VehiclePhysics_StepCar(slot, dt, s);
        g_inSpawnSettle = false;
        for (std::size_t o = 0; o < 12; o += 4) {
            F(r, off::kVelocity + o) = 0.f;   // +0x9b0..+0x9b8 linear
            F(r, 0x9bc + o)          = 0.f;   // +0x9bc..+0x9c4 angular
        }
        F(r, off::kSpeed) = 0.f;   // +0x9e4
        F(r, 0x9e8)       = 0.f;   // angular speed magnitude
        F(r, 0xb0c)       = 0.f;   // slide measure, the original's at-rest value
        I(r, 0x490)       = 0;     // gear
        I(r, 0x494)       = 0;     // shift timer
    }
    // =====================================================================

    // PERF (MASHED_PHYS_PROF): slot 0 == frame boundary (player steps first). Flush
    // the previous frame's accumulators, then time this car's whole physics body.
    const double t_phys0 = prof::g_on ? prof::NowMs() : 0.0;
    if (prof::g_on) {
        if (slot == 0) { prof::Flush(); prof::g_have = true; prof::f_wallMs = dt * 1000.0f; }
        ++prof::f_cars;
    }

    // --- adapter IN: world velocity, forward (= {cos,0,sin} per TrackRenderer), speed ---
    F(r, off::kVelocity + 0) = io.vel[0];
    F(r, off::kVelocity + 4) = io.vel[1];
    F(r, off::kVelocity + 8) = io.vel[2];
    F(r, off::kForward + 0)  = std::cos(io.yaw);
    F(r, off::kForward + 4)  = 0.f;
    F(r, off::kForward + 8)  = std::sin(io.yaw);
    F(r, off::kSpeed)        = io.speed;
    I(r, off::kActiveFlag)   = 1;

    std::uint8_t input[8];
    std::memcpy(input, io.input, sizeof(input));
    // WS-A8-STEER: map the steer command [-1,+1] onto the descriptor's mutually-
    // exclusive STEER bytes [0]/[1] — the byte channel A4 (FUN_00470670) reads (it
    // writes the scaled result to the FRONT-wheel steer-angle slots +0x1a8/+0x26c,
    // which A5 FUN_0046ddb0 Phase 0 turns into a per-wheel forward-axis rotation via
    // FUN_004c4d20). +steer -> input[0] (sign A, +angle), -steer -> input[1] (sign B,
    // -angle); 0..255 magnitude exactly as AI writer FUN_00416250 / human cook
    // FUN_00496530 produce. Mutually exclusive (the original writes one or the other).
    if (!io.raw_steer) {
        float st = io.steer;
        if (st >  1.0f) st =  1.0f;
        if (st < -1.0f) st = -1.0f;
        const int mag = static_cast<int>((st < 0.f ? -st : st) * 255.f + 0.5f);
        const std::uint8_t m = static_cast<std::uint8_t>(mag > 255 ? 255 : mag);
        input[0] = (st > 0.f) ? m : 0;
        input[1] = (st < 0.f) ? m : 0;
    }

    // =====================================================================
    // START BOOST ARM — U-D3-DRIVE-FORCE, 2026-09-29. MEASURED, not inferred.
    //
    // A6a's +0xbf8 == 1 arm (Integrate2.cpp, ported today) is what produces the
    // original's launch: 5e6 of force on each state-2 driving wheel for as long as the
    // +0xbf4 timer lasts. NOTHING in the standalone arms that state machine, and the
    // upstream writer is NOT LOCATED — see [UNCERTAIN] U-D3-BOOST-ARM below. So the
    // ARMED STATE is reproduced from the original's own captures instead.
    //
    // What the captures show, identically in all 10 regime-0 originals (e3, e4 of
    // verify/d3_drive_20260928/ and p1,p2,p3,p4,p8,p10,p11,p12 of
    // verify/d3_ai_20260927b/), read with re/tools/statediff/msd_fields.py:
    //   - during the last 5 countdown frames: +0xbf8 == 0, +0xbf4 == 50,100,150,200,250
    //     and the AI accel byte is still 0 (+0xb20 == 0 on every one of them);
    //   - on the FIRST frame the accel byte is 255: +0xbf8 == 1 and +0xbf4 == 1100;
    //   - then +0xbf4 == 900, 700, 500, 300, 100, 0 — exactly -200 per frame, which is
    //     A6a's own two decrement sites x two qualifying wheels x dt 50.
    // The render-tick snapshot is taken AFTER A6a has run, so the value A6a SEES on the
    // arming frame is 1100 + 200 = 1300, and seeding 1300 here (before A4/A6a run below)
    // reproduces the original's whole 1100/900/700/500/300/100/0 trajectory and therefore
    // the exact set of frames on which the 5e6 is applied.
    //
    // WHICH CARS, and WHEN — and the one place this is a DEVIATION not a transcription.
    // The arm happens at the green light, once per car. It is armed on all three AI cars in
    // every capture and on the PLAYER in NONE of them:
    //   - verify/a8_steer_20260824/orig_steerR.msd (player, D2 held-lock): +0xbf4 counts UP
    //     50/frame to 3000 and saturates, +0xbf8 == 0 on all 2210 frames. That recipe holds
    //     the throttle from before the lights.
    //   - verify/d3_force_20260929/o_c0.msd (player, D3 recipe, 2725 frames): +0xbf4 and
    //     +0xbf8 are 0 on EVERY frame. That recipe never gives the player throttle at all.
    // So the original's arm is gated on something the countdown measures about the car's own
    // throttle timing, and there is NO original observation of an armed player. The
    // standalone simulates no countdown, so it cannot represent "held the throttle too
    // early". This port therefore arms exactly the slots the captures show armed — the AI
    // cars, slot != 0 — and leaves the player unarmed. Consequences, stated:
    //   + it reproduces the measured state for all three AI cars, which is what criterion
    //     (e) is scored on, and
    //   + it leaves the D2 player guard bit-untouched (no player force changes at all), but
    //   - a human player who does not jump the lights presumably IS boosted in the real
    //     game, and this port will not boost them. That is unported, not decided: it needs
    //     the arming writer, i.e. U-D3-BOOST-ARM below.
    //
    // [UNCERTAIN] U-D3-BOOST-ARM — which function writes +0xbf8 = 1 / +0xbf4 = 1300, and
    // whether 1300 is a constant or a value the countdown computes. Evidence missing: the
    // writer. `py -3.12 re/tools/findoffset.py --writes 0xbf8 0xbf4 0xbf0` finds FOUR
    // +0xbf8 accesses and SEVEN +0xbf4 accesses in the whole of .text and every one of them
    // is inside FUN_00467650 (0x00467d08..0x00467e44); +0xbf0 has no writer at all. So the
    // arming store does not use a `[reg + 0xbf8]` displacement and the sweep is blind to it
    // (the dword-index-off-a-computed-base blind spot). Next command, in order:
    //   1. Frida write-watchpoint on &rec[car]+0xbf8 armed during the countdown. A6a is the
    //      only writer and its two bf8 stores are both inside `bf8 == 1` / `== 2` arms, so
    //      while bf8 == 0 NOTHING in A6a writes it and the first fault is the arming
    //      instruction. Report the faulting EIP, then decomp its containing function.
    //   2. If the watchpoint API is unavailable, a Ghidra script that walks stores whose
    //      base is DAT_008815a0 + k and whose displacement is 0xbf8 - k.
    // Until then the FORCE law is transcribed (Integrate2.cpp cites 0x00467d3a..0x00467e44)
    // and only the SEED is measured. MASHED_NO_START_BOOST=1 reverts the seed.
    static const bool s_noStartBoost = (std::getenv("MASHED_NO_START_BOOST") != nullptr);
    if (slot != 0 && !g_startBoosted[slot] && !s_noStartBoost && !g_inSpawnSettle) {
        g_startBoosted[slot] = true;
        I(r, 0xbf8) = 1;      // boost state 1 (Integrate2.cpp +0xbf8 == 1 arm)
        I(r, 0xbf4) = 1300;   // pre-A6a value; A6a's two sites leave the measured 1100
    }
    // Refresh DAT_0088e668/66c once per frame, as FUN_00470c70 does before its
    // per-vehicle A4 loop (slot 0 steps first, so slot 0 is the frame boundary).
    if (slot == 0 && !g_inSpawnSettle) Fi_UpdateBoostOrder();
    // =====================================================================

    // The chain works in the ORIGINAL's millisecond time base: FUN_00470c70 passes
    // A4 a dt that is the integer ms chunk count (local_24 = min(remaining,0x32)),
    // and computes the per-frame suspension scale from the FRAME dt in ms:
    //   _DAT_0088e610 (suspDtTerm) = frameMs * _DAT_005cea80 (0.0027778 = 1/360)
    //
    // *** BOTH FACTORS OF THIS FORMULA WERE WRONG. Measured 2026-08-26 by reading
    // the globals out of a LIVE original (verify/a8_suspglobals_20260826/):
    //     _DAT_0088e610 = 4.33336782 (bits 0x408aaaf3), constant over the run
    //     _DAT_0088e5f0 = 692.302185, constant; product exactly 3000
    // and 4.33336782 / 0.00277779996 = 1559.99996, with 1560 * that constant
    // reproducing 0x408aaaf3 EXACTLY. So the multiplicand is 1560, not the
    // dispatcher's 50-unit budget (1560/50 = 31.20 = the measured discrepancy).
    // WHERE 1560 COMES FROM IS NOT YET DECODED, so this is left as-is for now.
    // The error is behaviourally INERT: every consumer of the per-wheel field
    // derived from 0088e610 also multiplies by 0088e5f0, and the two are
    // reciprocal through 3000, so it cancels. Detail: seventeenth follow-up of
    // re/analysis/data/A8_velocity_vector_motion_20260825.md.
    //   _DAT_0088e5f0 (suspScale)  = _DAT_005ccd08 (3000) / suspDtTerm
    // (the chain constants — kDt 3.33e-4, etc. — are calibrated for ms, NOT seconds).
    // A8 TIME BASE CORRECTED (2026-08-25). This was `dt * 1000.0f`, i.e. the wall
    // frame time read as MILLISECONDS. That is wrong by exactly 3x, and it was the
    // whole of the measured yaw-rate gain gap (port reached 48-57% of the original
    // in every speed band; implied budget 52.48 units/frame vs our 16.67).
    //
    // The original does NOT feed the chain its wall frame time. FUN_00470c70's
    // budget is a FIXED 50 (0x32) per frame, produced by a tick quantizer:
    //   DAT_007f1000 pinned to 0x32          @0x004933d5 (FUN_00493390)
    //   re-derived as uVar1*0x32             @0x00493514 (FUN_00493480), with the
    //                                         sub-50 remainder carried in DAT_007719d4
    //   PUSH 0x32                            @0x0042c980
    //   forwarded verbatim: MOV ESI,[ESP+8]  @0x00425a78
    //                       PUSH ESI; CALL 0x00470c70 @0x00425a8d
    // and the catch-up loop iterates ceil(DAT_007f1000/50) = 1 in steady state, so
    // the dispatcher runs once per render frame. Decode:
    // re/analysis/data/A8_substep_budget_20260825.md.
    //
    // 50 units == one 1/60 s frame, from the engine's own two constants:
    //   DAT_007f1004  = 0x3c888889 = 1/60      (the float frame dt)
    //   _DAT_005cc948 = 0x39aec33e = 1/3000    (unit -> seconds)
    //   50 * (1/3000) = 1/60                   -- consistent
    // So the unit is 1/3000 s, not 1/1000 s, and the correct budget is dt*3000
    // (exactly 50 at our fixed 60 Hz sim rate).
    //
    // *** APPLIED 2026-08-25 (second attempt). *** The first attempt was reverted
    // because it failed the speed-profile gate, and that failure was WRONGLY
    // attributed to an internal->world unit mismatch. Both parts were then settled:
    //
    //  - The original's world position IS the +0x928 block translation (+0x30).
    //    Its per-frame delta equals this increment with a budget of 50 to a median
    //    ratio of 1.0147 (X, n=1207) / 1.0077 (Z, n=1210), with ZERO discontinuities
    //    above 0.5 across the drive. The earlier "it is contact-rebased and bounded,
    //    so it is not the world position" reading was a REGIME ARTIFACT: that
    //    capture holds full lock for 38 s, so the car drives a ~2-unit circle.
    //
    //  - Measured at MATCHED internal speed, dt*3000 reproduces the original and
    //    dt*1000 does not (band 1500-2000):
    //        metric        ORIGINAL   dt*1000   dt*3000
    //        world speed     4.94      1.58      4.74
    //        yaw rate        2.66      1.29      2.61
    //        turn radius     1.86      1.23      1.81
    //    Turn radius is a LENGTH, so its agreement is the unit check and it passes.
    //
    // What actually failed the gate was an off-mesh recovery defect in
    // TrackRenderer (it re-aimed the car but never relocated it, so its 0.6 speed
    // damping compounded to a respawn — 0.6^10.3, measured). That is fixed
    // separately; see the long note on the off-mesh branch there.
    const float frameMs = dt * 3000.0f;
    // CORRECTED 2026-08-26. This was `frameMs * kSuspDtK` — it passed the
    // dispatcher's per-frame BUDGET (param_1 = 50) where the original passes
    // `*param_2`, a COURSE-LOAD CONSTANT. Two different parameters, conflated.
    //
    // The original (FUN_00470c70, math 0x00470f1c-0x00470f3a):
    //     FLD [EAX] ; FMUL [0x005cea80] ; FSTP [0x0088e610]   @0x00470f28
    //     FLD 3000.0 ; FDIV [0x0088e610] ; FSTP [0x0088e5f0]  @0x00470f3a
    // where [EAX] = *param_2 = DAT_00803324, a hardcoded float literal
    // 0x44C30000 = 1560.0 stored at 0x0040d3d2 in the course-load path
    // FUN_0040d270 (sole writer; the respawn path FUN_004704c0 only reads it).
    //
    // 1560 * (1/360) = 13/3, whose float32 is 0x408aaaf3 = 4.33336782 — an EXACT
    // bit match to the value MEASURED out of a live original
    // (verify/a8_suspglobals_20260826/), and 1560/50 = 31.20 is exactly the
    // 31.17x discrepancy that measurement showed.
    //
    // [UNCERTAIN] whether any other course-load path writes DAT_00803324 with a
    // different value; the decode says this is the sole writer, so 1560 is used
    // as a constant here rather than plumbed through a course parameter.
    //
    // Expected behavioural effect: NONE. Every consumer of the per-wheel field
    // derived from suspDtTerm also multiplies by suspScale, and the two are
    // reciprocal through 3000 — so this corrects the record CONTENTS (a 31x
    // fidelity error) without changing any force. If a gate moves, the
    // cancellation analysis in the seventeenth follow-up is wrong.
    constexpr float kCourseSuspConst = 1560.0f;   // DAT_00803324 (0x44C30000)
    g_suspDtTerm = kCourseSuspConst * kSuspDtK;
    g_suspScale  = (g_suspDtTerm != 0.f) ? (kSuspNum / g_suspDtTerm) : 0.f;

    // A8 (2026-08-25): the body-forward/wheel-axis world transform A5 needs IS the
    // integrated body basis now, not a yaw matrix synthesized from io.yaw. That is
    // the whole point of the change: with a synthesized BuildYawMatrix(io.yaw) the
    // wheel axes are forced to agree with the velocity heading (because io.yaw was
    // itself a lag chasing the velocity heading), so slip was unrepresentable.
    float* basis = g_bodyBasis[slot];
    if (!g_bodyBasisOk[slot]) { BodyOrient_Init(basis, io.yaw); g_bodyBasisOk[slot] = true; }
    // Keep io.yaw the basis heading from here on: it feeds record +0x9d4 (above) and
    // SolveWheelContacts' wheel placement, both of which must see the BODY heading.
    io.yaw = BodyOrient_Heading(basis);
    F(r, off::kForward + 0) = std::cos(io.yaw);
    F(r, off::kForward + 8) = std::sin(io.yaw);

    // A8 position accumulator: the frame's world-space position delta, summed over
    // the substeps exactly as FUN_0046e9e0 accumulates it into the matrix
    // translation (0x0046ea5f / 0x0046ea69 / 0x0046ea73).
    float posDelta[3] = { 0.f, 0.f, 0.f };

    // Subdivide the frame ms budget into <=50ms chunks (the FUN_00470c70 chunk loop,
    // local_24 = min(remaining,0x32)); A4's dt per chunk is that ms count.
    // [A8-ORDER 2026-09-13] env MASHED_A8_A4_FIRST=1: run A4 -> A5 -> A6a BEFORE the
    // substep loop, i.e. on the PRE-rotation basis and on last frame's contacts, which is
    // the original's order (FUN_00470c70: step 3 = FUN_00470670 per vehicle, step 5 =
    // the FUN_004709a0 substep loop that rotates the body). Evidence for the phase:
    // in orig_steerR.msd every wheel axis sits +0.0446 rad from +0x9d4 while the body
    // rotates -0.0446 rad/frame (A8 twenty-sixth follow-up), i.e. the axes A5 wrote are
    // one frame older than the forward vector FUN_0046e9e0 wrote after them. Here the
    // axes and +0x9d4 are written from the same post-rotation basis (0.00 rad apart).
    // DEFAULT ON since 2026-09-13 (A8 twenty-sixth follow-up: same-build A/B on the
    // held-lock recipe gives slip 0.192/0.263 vs the original's 0.191/0.250, av.y and
    // the axis phase matching, no spin-out). `MASHED_A8_A4_FIRST=0` reverts to the
    // pre-2026-09-13 order for A/B only (ROADMAP v3 default-build rule: the flag may
    // only turn the ported behaviour OFF).
    static const bool s_a4First = [] {
        const char* e = std::getenv("MASHED_A8_A4_FIRST");
        return !(e && e[0] == '0');
    }();
    if (s_a4First) {
        g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;
        VehicleControlIntegrate(reinterpret_cast<int*>(r), frameMs, input, basis, slot);
        ReassertContacts(r);
    }

    // [U-9154 2026-09-29] The car<->WORLD contact chain. The original's substep body
    // (FUN_004709a0, 0x00470ab0..0x00470b0b, disassembled from MASHED.exe.unpatched) is
    //     0x00470ab0  cmp ebp,2 / jge          ; at most 2 executions of this body
    //     0x00470ad3  call 0x0046e9e0          ; A9: integrate pos + basis
    //     0x00470ae0  call 0x0046f6c0          ; wheel contact solver
    //     0x00470ae8  call 0x00469aa0          ; contact scan -> [rec+0x9ec], returns a flag
    //     0x00470aef  mov eax,[esi+0x9ec] / test / je 0x470b0d
    //     0x00470afe  call 0x0046ef70          ; the fixup (writes +0x9b0 / +0x144)
    //     0x00470b06  test ebx,ebx / je 0x470b0d
    //     0x00470b0a  inc ebp / jmp 0x470ab0   ; a REPORTED contact re-runs the substep
    // The port's substep already runs A9's two halves (position from the velocity
    // vector, basis from BodyOrient_*) and FUN_0046f6c0; what it never ran is the
    // 0x00469aa0 -> 0x0046ef70 pair, so nothing ever wrote the corrective velocity.
    // `MASHED_WORLD_CONTACT=0` reverts to the pre-U-9154 substep for A/B (ROADMAP v3
    // default-build rule: a flag may only turn the ported behaviour OFF).
    static const bool s_worldContact = [] {
        const char* e = std::getenv("MASHED_WORLD_CONTACT");
        return !(e && e[0] == '0');
    }();
    // Diagnostic, default-OFF: one line per fixup, so a wall bounce can be read back
    // frame by frame without a Frida attach.
    static const char* const s_wcLog = std::getenv("MASHED_WORLD_CONTACT_LOG");
    static int s_wcLines = 0;

    float remMs = frameMs;
    int guard = 0;
    while (remMs > 0.0f && guard++ < 64) {
        float chunkMs = (remMs < (float)kMaxSubstep) ? remMs : (float)kMaxSubstep;

        // 0x00470ab0's retry needs the substep to be REDOABLE. The original gets that
        // for free: A9 writes dst = src + delta into the OTHER half of the +0x928
        // double buffer, so re-running it recomputes the step from the same source with
        // the corrected velocity. The port's position/basis advance is cumulative in
        // place, so snapshot what the body mutates and restore it before a retry.
        // The record is deliberately NOT snapshotted — the fixup's velocity write is
        // exactly what the retry is supposed to see, as in the original.
        float savedPos[3]   = { io.pos[0], io.pos[1], io.pos[2] };
        float savedDelta[3] = { posDelta[0], posDelta[1], posDelta[2] };
        float savedBasis[16];
        std::memcpy(savedBasis, basis, sizeof(savedBasis));
        const float savedYaw = io.yaw;

        for (int pass = 0; pass < 2; ++pass) {   // 0x00470ab0: `cmp ebp,2`
        if (pass != 0) {
            io.pos[0] = savedPos[0]; io.pos[1] = savedPos[1]; io.pos[2] = savedPos[2];
            posDelta[0] = savedDelta[0]; posDelta[1] = savedDelta[1]; posDelta[2] = savedDelta[2];
            std::memcpy(basis, savedBasis, sizeof(savedBasis));
            io.yaw = savedYaw;
        }

        // --- FUN_0046e9e0 (0x0046e9e0) runs FIRST in the original's substep
        // (FUN_004709a0 order: FUN_0046e9e0 -> FUN_0046f6c0 -> FUN_00469aa0), so
        // position and orientation advance on the velocity/omega state the previous
        // substep left in the record. Keep that order.

        // POSITION, from the velocity VECTOR (0x0046e9fe / 0x0046ea0a / 0x0046ea14):
        //   inc = dtMs * _DAT_005cc948 * _DAT_005cea80 * (+0x9b0,+0x9b4,+0x9b8)
        // DEVIATION, STATED: the original zeroes this increment unless
        // FUN_0040e350() is 6, 0xb or 0xa (gate 0x0046ea1e..0x0046ea4c). The
        // standalone's Fi_GameMode() is a STUB returning 0
        // (ForceIntegratorStubs.cpp:49), so consuming that gate here would zero all
        // motion unconditionally — it would be reading a stub, not the original's
        // mode. The gate is therefore not applied; this code path only runs during
        // a race, which is one of the three passing modes. Porting FUN_0040e350 is
        // tracked separately (it also gates RubberBandGate and Integrate2's mode-7
        // branch, so it is not a one-line change).
        {
            const float k = chunkMs * kPosDtK * kSuspDtK;
            posDelta[0] += k * F(r, off::kVelocity + 0);
            posDelta[1] += k * F(r, off::kVelocity + 4);
            posDelta[2] += k * F(r, off::kVelocity + 8);
            // Advance io.pos within the frame so the contact solve below sees the
            // position this substep produced, as the original's FUN_0046f6c0 does
            // (it reads the translation FUN_0046e9e0 just wrote). The caller
            // re-seeds io.pos from its own car_pos_ every frame.
            io.pos[0] += k * F(r, off::kVelocity + 0);
            io.pos[2] += k * F(r, off::kVelocity + 8);
        }

        // ORIENTATION: omega from the wheel-force arm (record +0x10 == 0, which the
        // Ghidra pass established is the arm a normal driving car takes), then the
        // first-order dR = omega x R row integration + ortho-normalize.
        {
            float w[3];
            BodyOrient_OmegaFromSteer(r, chunkMs, input, w);
            BodyOrient_IntegrateStep(basis, w);
            io.yaw = BodyOrient_Heading(basis);
            F(r, off::kForward + 0) = std::cos(io.yaw);
            F(r, off::kForward + 8) = std::sin(io.yaw);
        }

        // WS-A contacts: run the REAL ported wheel-contact solver (FUN_0046f6c0) over the
        // track tris instead of the flat SetGrounded substitute -> per-wheel slope normals
        // (+0x200) + per-wheel grounding (-> A5's mass/grounded_count load = weight transfer).
        const double tp0 = prof::g_on ? prof::NowMs() : 0.0;
        // basis + 4 = the `up` row of the integrated body basis (BodyOrient_Init's
        // m[4..6]); it is what record +0x9c8/+0x9cc/+0x9d0 holds on the original.
        SolveWheelContacts(r, io, guard, basis + 4);
        if (prof::g_on) { prof::f_probeMs += prof::NowMs() - tp0; ++prof::f_substeps; }

        // --- 0x00470ae8 / 0x00470aef / 0x00470afe: the car<->world contact pair ---
        if (!s_worldContact) break;
        // Publish the integrated basis + the true world position into the +0x928 ring
        // the scan transforms the 18 body contact points by. AFTER the wheel solver,
        // because the wheel solver's broadphase reads the ring's +0x30 as its query
        // centre and SolveWheelContacts deliberately seats that at ground level.
        SyncContactRingMatrix(r, basis, io.pos);
        int* const wcSelf = reinterpret_cast<int*>(r);
        const int contacted = Collision::VehicleContactHistoryUpdate(wcSelf); // 0x00470ae8
        if (I(r, 0x9ec) != 0) {                                              // 0x00470aef
            const float pv[3] = { F(r, off::kVelocity + 0), F(r, off::kVelocity + 4),
                                  F(r, off::kVelocity + 8) };
            Collision::VehicleContactFixup(wcSelf);                          // 0x00470afe
            if (s_wcLog && s_wcLines < 4000) {
                if (std::FILE* lf = std::fopen(s_wcLog, "a")) {
                    std::fprintf(lf,
                        "slot=%d pass=%d n=%d ret=%d pos=(%.4f,%.4f,%.4f) "
                        "vel=(%.4f,%.4f,%.4f)->(%.4f,%.4f,%.4f)",
                        slot, pass, I(r, 0x9ec), contacted,
                        io.pos[0], io.pos[1], io.pos[2],
                        pv[0], pv[1], pv[2],
                        F(r, off::kVelocity + 0), F(r, off::kVelocity + 4),
                        F(r, off::kVelocity + 8));
                    // which of the 18 slots reported, and with what normal/depth.
                    // Slot base S = 0x4a8 + i*0x40: +0x00 depth, +0x04 key (-1 empty),
                    // +0x08..+0x10 normal, +0x38 magnitude.
                    for (int si = 0; si < 0x12; ++si) {
                        const std::size_t S = 0x4a8 + (std::size_t)si * 0x40;
                        if (I(r, S + 4) == -1) continue;
                        std::fprintf(lf, " | s%d d=%.4f n=(%.3f,%.3f,%.3f) m=%.1f",
                            si, F(r, S + 0), F(r, S + 8), F(r, S + 0xc),
                            F(r, S + 0x10), F(r, S + 0x38));
                    }
                    std::fprintf(lf, "\n");
                    std::fclose(lf);
                }
                ++s_wcLines;
            }
            if (contacted != 0) continue;    // 0x00470b06/0x00470b0a: re-run the substep
        }
        break;
        }   // end retry
        remMs -= chunkMs;
    }

    // A4 (FUN_00470670) RUNS ONCE PER FRAME, OUTSIDE THE SUBSTEP LOOP — corrected
    // 2026-08-25. It used to be called inside the loop above, which at the corrected
    // budget meant TWICE per frame at dt=25.
    //
    // The original's order (FUN_00470c70): step 3 is a per-vehicle FUN_00470670,
    // which zeroes +0xb14/18/1c at its start (0x004706af/b5/bb) then calls A5
    // (FUN_0046ddb0), A6a (FUN_00467650) EXACTLY ONCE, and A6b (FUN_00468980).
    // Step 5 is the substep loop FUN_004709a0 (2 x 25), which calls FUN_0046e9e0 /
    // FUN_00469aa0 and NEVER calls A4 or A6a. Decode: the eighth follow-up of
    // re/analysis/data/A8_velocity_vector_motion_20260825.md.
    //
    // This matters beyond bookkeeping: grip-clamp #6 lives at the tail of A6a and
    // does `vel -= lateral * k`. Running A4 twice per frame applied that lateral
    // bleed TWICE, suppressing slip. Measured slip at 2000-2600 full lock:
    // 0.0547 (twice/frame) -> 0.0733 (once/frame), against the original's 0.2498.
    // A real improvement in the right direction; the remaining 3.4x is still open.
    //
    // A4 therefore consumes the WHOLE frame budget, and sees the contact state the
    // substep loop above left in the record — which is what the original does too,
    // since its contacts are solved in the loop that follows A4.
    if (!s_a4First) {   // [A8-ORDER] see the knob above the substep loop
        g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;
        VehicleControlIntegrate(reinterpret_cast<int*>(r), frameMs, input, basis, slot);
        // A6a's drive block can clear wheel states in some branches; re-assert from the
        // cached contact result (no re-broadphase) so the next frame stays engaged.
        ReassertContacts(r);
    }

    // WS-A contacts telemetry: per-wheel contact NORMAL (+0x200) + LOAD (+0x20c, A5
    // Phase-5 weight-transfer slot) + STATE (+0x198), to prove they VARY with terrain
    // (slope vs flat). Gated by MASHED_CONTACT_DIAG so normal runs stay quiet.
    {
        static const bool s_diag = (std::getenv("MASHED_CONTACT_DIAG") != nullptr);
        if (s_diag) {
            static int dn = 0;
            if (dn < 4000) {
                if (std::FILE* lf = std::fopen("contact_diag.log", "a")) {
                    std::fprintf(lf, "slot=%d gnd=%d gc=%.0f pos=(%.2f,%.2f,%.2f) sp=%.2f",
                        slot, io.grounded, F(r, off::kGroundedCnt),
                        io.pos[0], io.pos[1], io.pos[2], io.speed);
                    for (int w = 0; w < 4; ++w) {
                        const std::size_t nb = wheelNormOff(w);
                        std::fprintf(lf, " | w%d st=%d n=(%.3f,%.3f,%.3f) ld=%.4f",
                            w, I(r, wheelStateOff(w)),
                            F(r, nb + 0), F(r, nb + 4), F(r, nb + 8),
                            F(r, nb + 0xc));   // +0x20c load (A5 pf[6])
                    }
                    std::fprintf(lf, "\n");
                    std::fclose(lf);
                }
                ++dn;
            }
        }
    }

    // WS-A COUPLING (2026-06-29): anti-overflow safety clamp ONLY. The OLD code
    // hard-clamped record +0x9b0 to 45 here, which destroyed the accel ramp (the
    // chain pinned to 45 instantly). The recovered law's SOFT top-speed asymptote
    // (below) governs the visible top speed now, so this clamp is set high — far
    // above where the tanh saturates — purely to stop the ported chain's unbounded
    // straight-line ramp (+0x9b0 grows ~77->553+, Integrate2 grip-clamp #6 limits
    // only LATERAL speed) from overflowing the round-tripped car_vel_ over a long run.
    {
        // RAISED 2026-08-22 from 1500 to 16384. The old value's premise -- "far
        // above where the tanh saturates", i.e. high enough never to bind -- is
        // measurably false against the stock original. Measured from the archived
        // stock arm (verify/statediff_proto/drive_stock_a.msd, +0x9e4):
        //   the original RAMPS UNBOUNDED within a round, peaking at 4275.1 /
        //   4070.4 / 4344.5 / 1970.3, then RESETS to ~0 at each round boundary
        //   (zero-runs of 897 / 13 / 246 / 235 frames). It never reaches a
        //   terminal velocity, and 28% of its frames exceed 1500.
        // So the unbounded ramp is FAITHFUL and 1500 was truncating a range the
        // original occupies -- pinning our +0x9b0 where the original swings to
        // ~4344. 16384 keeps the anti-overflow guarantee (the stated purpose)
        // while sitting ~3.8x above the highest value the original was observed
        // to reach, so it should never bind in practice.
        // Detail: re/analysis/D2_REALPHYS_REMEASURE_2026-08-21.md.
        constexpr float kSafetyInternal = 16384.0f;
        float vx = F(r, off::kVelocity + 0), vy = F(r, off::kVelocity + 4), vz = F(r, off::kVelocity + 8);
        float sp = std::sqrt(vx*vx + vy*vy + vz*vz);
        if (sp > kSafetyInternal && sp > 1e-4f) {
            float s = kSafetyInternal / sp;
            F(r, off::kVelocity + 0) = vx * s;
            F(r, off::kVelocity + 4) = vy * s;
            F(r, off::kVelocity + 8) = vz * s;
            F(r, off::kSpeed) = kSafetyInternal;
        }
    }

    // ========================================================================
    // A8 VELOCITY-VECTOR MOTION MODEL (2026-08-25) — replaces the WS-A COUPLING
    // block that lived here (a PD-relaxed scalar body speed + a first-order lag
    // that chased the VELOCITY heading, with env knobs MASHED_CHAINSCALE /
    // MASHED_ALIGNRATE / MASHED_TOPSPEED).
    //
    // WHY IT IS GONE. That model was heading-plus-scalar: it projected the chain
    // velocity onto {cos,0,sin(io.yaw)}, relaxed a scalar toward the projection,
    // and the caller re-expanded the scalar along the same axis. It is only
    // correct while io.yaw EQUALS the velocity heading, which the alignment lag
    // enforced. The two halves were mutually load-bearing, so the car could not
    // slip: any independent body orientation shrank the projection by cos(slip)
    // and collapsed the drive (measured twice, median speed 378 -> 7.28 and
    // -> 5.56 when the orientation was wired against this block).
    //
    // The original has no such coupling. FUN_0046e9e0 integrates position and
    // orientation SEPARATELY in one function, with nothing forcing them to agree:
    //   position:    EBX[0xc..0xe] = EDI[0xc..0xe] + k*dt*ESI[0x26c..0x26e]
    //   orientation: EBX[row]      = EDI[row] + (omega x EDI[row])
    // Both halves are ported now: position accumulates into posDelta in the
    // substep loop above, orientation into g_bodyBasis via BodyOrient_*.
    //
    // The velocity-heading alignment law the old block cited as FUN_0047eb30 is
    // real but belongs to the DAT_006ce274-gated RWP servo bridge, which never
    // touches +0x9bc/+0x9c0/+0x9c4 — the citation was misattributed. Evidence:
    // re/analysis/data/A8_body_heading_law_20260825.md.
    //
    // MASHED_ALIGNRATE's 7.0 default and MASHED_CHAINSCALE's 0.0083 were fitted
    // numbers with no address behind them. Both are deleted rather than
    // re-defaulted: the internal->world scale is now the original's own
    // _DAT_005cc948 * _DAT_005cea80 = 9.2598e-7 per ms, cited in the substep loop.
    // ========================================================================
    io.drive_delta[0] = posDelta[0];
    io.drive_delta[1] = posDelta[1];
    io.drive_delta[2] = posDelta[2];

    // A8 motion diag (env MASHED_MOTION_DIAG): the three quantities that decide
    // whether the model can represent slip — the velocity heading, the BODY
    // heading, and their difference (the slip angle) — logged next to the STEER
    // INPUT that is supposed to drive them. Logging the input on the same line is
    // deliberate: a previous pass drew a conclusion from a sample window in which
    // steer was 0 on 58 of 60 frames, i.e. a regime filter masquerading as a
    // measurement. Slot 0 only, uncapped so the cap cannot become a regime filter.
    {
        static const bool s_md = (std::getenv("MASHED_MOTION_DIAG") != nullptr);
        if (s_md && slot == 0) {
            if (std::FILE* lf = std::fopen("motion_diag.log", "a")) {
                const float cvx = F(r, off::kVelocity + 0), cvz = F(r, off::kVelocity + 8);
                const float hs  = std::sqrt(cvx*cvx + cvz*cvz);
                const float velH = (hs > 1e-3f) ? std::atan2(cvz, cvx) : 0.f;
                float slip = velH - io.yaw;
                while (slip >  3.14159265f) slip -= 6.28318531f;
                while (slip < -3.14159265f) slip += 6.28318531f;
                std::fprintf(lf,
                    "reseed=%d gear=%d gtmr=%d ftot=[%g,%g,%g] susp=%g p15=%g p16=%g w1b=[%g,%g,%g,%g] fl=[%d,%d,%d,%d] "
                    "b14=[%g,%g,%g] gt=[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f] in=(%u,%u,%u,%u) steer=%+.3f gnd=%.1f sp=%.2f horiz=%.2f "
                    "velH=%.4f bodyH=%.4f slip=%+.4f "
                    "d=(%.5f,%.5f) av=(%g,%g,%g) "
                    // [A8-ORIENT 2026-09-13] per-wheel force X/Z (p[0x1c]/p[0x1e]), wheel
                    // axis X/Z (p[0x1f]/p[0x21]), and Integrate2's le4/ld4, wheels 0..3.
                    "wf=[%g,%g,%g,%g,%g,%g,%g,%g] wax=[%g,%g,%g,%g,%g,%g,%g,%g] "
                    // [U-D3-DRIVE 2026-09-28] the three record fields the gearbox
                    // upshift test at Integrate2.cpp:131-143 reads and the log did not
                    // show: +0xb0c (the term that shortens fVar5_base, Integrate2.cpp:126)
                    // and the pair +0x498/+0x49c (Integrate2.cpp:129-130). The ORIGINAL
                    // holds b0c ~ 0 and 40000 / 4000 through its launch
                    // (verify/d3_drive_20260928/e3.msd via re/tools/statediff/msd_fields.py),
                    // and upshifts out of gear 0 only once +0x9e4 passes 250 -- which is
                    // exactly (speed+250)*4 crossing fVar5 = 2000.
                    "wle4=[%g,%g,%g,%g] wld4=[%g,%g,%g,%g] b0c=%g gb498=%g gb49c=%g\n",
                    g_bodyBasisReseed[slot] ? 1 : 0,
                    I(r, 0x490), I(r, 0x494),   // gearbox state (Integrate2.cpp:137-143)
                    // [A8-FTOTDIR] the SUMMED per-wheel force VECTOR, p[0x1c..0x1e]
                    // over the 4 wheels (base 0x1a4 + w*0xc4, +0x70/+0x74/+0x78).
                    // Every comparison so far used only |fTot|; a matching magnitude
                    // with a different DIRECTION would explain a slip gap that the
                    // magnitudes do not. These are record fields on both sides, so
                    // the original's can be read straight out of the .msd capture.
                    F(r,0x214)+F(r,0x2d8)+F(r,0x39c)+F(r,0x460),
                    F(r,0x218)+F(r,0x2dc)+F(r,0x3a0)+F(r,0x464),
                    F(r,0x21c)+F(r,0x2e0)+F(r,0x3a4)+F(r,0x468),
                    // [A8-FTOT] block-#4 per-wheel force inputs. Wheel w base is
                    // 0x1a4 + w*0xc4; p[0x15]=+0x54, p[0x16]=+0x58, p[0x1b]=+0x6c,
                    // p[-1]=-0x04. ORIGINAL (speed>1000): p15=0.15, p16=0.0125,
                    // p[-1]=0 on all four, p[0x1b]=1091/1081/1086/536.
                    g_suspScale, F(r,0x1f8), F(r,0x1fc),
                    F(r,0x210), F(r,0x2d4), F(r,0x398), F(r,0x45c),
                    I(r,0x1a0), I(r,0x264), I(r,0x328), I(r,0x3ec),
                    // [A8-B14CADENCE] the drive-force accumulator AS SEEN AFTER the
                    // whole substep loop — i.e. at the same point in the frame that
                    // the original-side capture's render-tick snapshot sees it.
                    // Compare against the consumption-time value logged inside
                    // Integrate2 (MASHED_COUPLING_DIAG `ctrl=`). If they differ, a
                    // render-tick snapshot of +0xb14 is a residue, not the force the
                    // integrator used, and every figure derived from the captured
                    // +0xb14 is invalid.
                    F(r,0xb14), F(r,0xb18), F(r,0xb1c),
                    // per-gear drive table +0x478..+0x48c. READ AT RUNTIME, not
                    // grepped: a literal-offset grep cannot see a dword-index write
                    // off a computed base, and that exact mistake has produced a
                    // committed false "no writer anywhere" claim in this project before.
                    F(r,0x478), F(r,0x47c), F(r,0x480), F(r,0x484), F(r,0x488), F(r,0x48c),
                    (unsigned)input[0], (unsigned)input[1],
                    (unsigned)input[4], (unsigned)input[5],
                    io.steer, F(r, 0x9e0), F(r, off::kSpeed), hs,
                    velH, io.yaw, slip,
                    posDelta[0], posDelta[2],
                    F(r, 0x9bc), F(r, 0x9c0), F(r, 0x9c4),
                    F(r,0x214), F(r,0x21c), F(r,0x2d8), F(r,0x2e0), F(r,0x39c), F(r,0x3a4), F(r,0x460), F(r,0x468),
                    F(r,0x1a4+0x7c), F(r,0x1a4+0x84), F(r,0x268+0x7c), F(r,0x268+0x84),
                    F(r,0x32c+0x7c), F(r,0x32c+0x84), F(r,0x3f0+0x7c), F(r,0x3f0+0x84),
                    g_a8WheelLe4[0], g_a8WheelLe4[1], g_a8WheelLe4[2], g_a8WheelLe4[3],
                    g_a8WheelLd4[0], g_a8WheelLd4[1], g_a8WheelLd4[2], g_a8WheelLd4[3],
                    F(r, 0xb0c), F(r, 0x498), F(r, 0x49c));
                std::fclose(lf);
                g_bodyBasisReseed[slot] = false;
            }
        }
    }

    // [U-9147 2026-09-29] MASHED_A6ADUMP=<path> -> one line per FRAME for slot 0 with
    // BOTH phases of A6a block #4, at %.17g. DIAG ONLY, default-OFF, writes nothing
    // when unset.
    //
    //   act.*   A6a's OWN values, captured at the instruction that computes them
    //           (Integrate2.cpp block #4). This is the ground truth the replay tool
    //           re/tools/statediff/a6a_replay.py validates itself against.
    //   snap.*  the SAME record fields read here, i.e. at the render-tick phase the
    //           original's .msd capture sees them.
    //
    // Both are needed because the original side only ever gives the snapshot phase.
    // Replaying the law from `snap` and comparing the prediction to `act` measures how
    // much the phase difference costs; only if that cost is small is a snapshot-driven
    // cross-side comparison (this tool's predecessor a8_wheelfit.py) sound at all.
    {
        static const char* s_a6ad = std::getenv("MASHED_A6ADUMP");
        static long s_a6f = 0;
        if (s_a6ad && slot == 0) {
            ++s_a6f;
            if (std::FILE* lf = std::fopen(s_a6ad, "a")) {
                std::fprintf(lf, "f=%ld", s_a6f);
                std::fprintf(lf, " snap.vel=%.17g,%.17g,%.17g snap.av=%.17g,%.17g,%.17g"
                                 " snap.bf=%.17g,%.17g,%.17g snap.sp=%.17g snap.angsp=%.17g"
                                 " snap.gc=%.17g snap.susp=%.17g snap.steer=%.17g",
                             F(r,0x9b0), F(r,0x9b4), F(r,0x9b8),
                             F(r,0x9bc), F(r,0x9c0), F(r,0x9c4),
                             F(r,0x9d4), F(r,0x9d8), F(r,0x9dc),
                             F(r,0x9e4), F(r,0x9e8), F(r,0x9e0), g_suspScale, F(r,0x1a8));
                std::fprintf(lf, " act.vel=%.17g,%.17g,%.17g act.av=%.17g,%.17g,%.17g"
                                 " act.bf=%.17g,%.17g,%.17g act.sp=%.17g act.angsp=%.17g"
                                 " act.gc=%.17g act.susp=%.17g",
                             g_a6aFrame.vel[0], g_a6aFrame.vel[1], g_a6aFrame.vel[2],
                             g_a6aFrame.av[0],  g_a6aFrame.av[1],  g_a6aFrame.av[2],
                             g_a6aFrame.bf[0],  g_a6aFrame.bf[1],  g_a6aFrame.bf[2],
                             g_a6aFrame.sp, g_a6aFrame.angSp, g_a6aFrame.gc,
                             g_a6aFrame.suspScale);
                // clamp #6: the record inputs at the snapshot phase, then A6a's own
                // l_60 / grip / k. m18c/m2c/m34/tid/d00 are record fields the .msd
                // also carries, so the same chain is computable on the original.
                std::fprintf(lf, " snap.m18c=%.17g snap.m2c=%d snap.m34=%d snap.tid=%d"
                                 " snap.d00=%d act.l60=%.17g act.m18c=%.17g act.speed=%.17g"
                                 " act.grip=%.17g act.kvel=%.17g act.kav=%.17g act.arm=%d"
                                 " act.clamp=%d",
                             F(r,0x18c), I(r,0x2c), I(r,0x34), I(r,0x1f0), I(r,0xd00),
                             g_a6aFrame.l60, g_a6aFrame.m18c, g_a6aFrame.speed,
                             g_a6aFrame.grip, g_a6aFrame.kVel, g_a6aFrame.kAv,
                             g_a6aFrame.arm, g_a6aFrame.clampRan);
                for (int w = 0; w < 4; ++w) {
                    const int b = 0x1a4 + w * 0xc4;
                    const Vehicle::A6aWheelDump& D = g_a6aDump[w];
                    std::fprintf(lf,
                        " |w%d fired=%d spin=%d"
                        " s.off=%.17g,%.17g,%.17g s.ax=%.17g,%.17g,%.17g"
                        " s.p15=%.17g s.p16=%.17g s.p1b=%.17g s.pm1=%d s.pm3=%d s.pmb=%.17g"
                        " s.F=%.17g,%.17g,%.17g"
                        " a.off=%.17g,%.17g,%.17g a.ax=%.17g,%.17g,%.17g"
                        " a.p15=%.17g a.p16=%.17g a.p1b=%.17g a.pm1=%d"
                        " a.le=%.17g,%.17g,%.17g a.le4raw=%.17g a.le4=%.17g"
                        " a.lat=%.17g,%.17g,%.17g a.ld4=%.17g a.lbc=%.17g a.f5=%.17g"
                        " a.dF=%.17g,%.17g,%.17g",
                        w, D.fired, D.spin,
                        F(r,b-0x24), F(r,b-0x20), F(r,b-0x1c),
                        F(r,b+0x7c), F(r,b+0x80), F(r,b+0x84),
                        F(r,b+0x54), F(r,b+0x58), F(r,b+0x6c), I(r,b-0x04),
                        I(r,b-0x0c), F(r,b-0x2c),
                        F(r,b+0x70), F(r,b+0x74), F(r,b+0x78),
                        D.off[0], D.off[1], D.off[2], D.ax[0], D.ax[1], D.ax[2],
                        D.p15, D.p16, D.p1b, D.pm1,
                        D.le[0], D.le[1], D.le[2], D.le4raw, D.le4,
                        D.lat[0], D.lat[1], D.lat[2], D.ld4, D.lbc, D.f5,
                        D.dF[0], D.dF[1], D.dF[2]);
                }
                std::fprintf(lf, "\n");
                std::fclose(lf);
            }
        }
    }

    // [U-D3-DRIVE 2026-09-28] MASHED_MOTION_DIAG_AI=1 -> the same gearbox/launch fields
    // for the OPPONENT slots, in their own file so the a8 reducers (which key on
    // `reseed=` ... `wax=[...]` and assume slot 0) are untouched. Criterion (e) is scored
    // on cars 1..3 and the slot-0-only diag above could not see them
    // (D3_SPEED_GAP_2026-09-28.md section 6.3 asked for exactly this widening).
    {
        static const bool s_mdai = (std::getenv("MASHED_MOTION_DIAG_AI") != nullptr);
        if (s_mdai && slot != 0) {
            if (std::FILE* lf = std::fopen("motion_diag_ai.log", "a")) {
                const float cvx = F(r, off::kVelocity + 0), cvz = F(r, off::kVelocity + 8);
                std::fprintf(lf,
                    "slot=%d gear=%d gtmr=%d b0c=%g gb498=%g gb49c=%g sp=%g horiz=%g "
                    "in=(%u,%u,%u,%u) gnd=%d\n",
                    slot, I(r, 0x490), I(r, 0x494), F(r, 0xb0c), F(r, 0x498), F(r, 0x49c),
                    F(r, off::kSpeed), std::sqrt(cvx * cvx + cvz * cvz),
                    (unsigned)input[0], (unsigned)input[1],
                    (unsigned)input[4], (unsigned)input[5], io.grounded);
                std::fclose(lf);
            }
        }
    }

#if defined(MASHED_PHYS_DIAG)  /* TEMP diag — compiled only with -DMASHED_PHYS_DIAG */
    {
        static int dn = 0;
        if (dn < 200) {
            if (std::FILE* lf = std::fopen("phys_diag.log", "a")) {   // cwd-relative
                std::fprintf(lf, "DIAG in0=%d in1=%d in4=%d in5=%d gnd=%d sp=%g "
                    "vel=(%.2f,%.2f,%.2f) steerAng=(%g,%g) angvel=(%g,%g,%g) "
                    "wheel0st=%d wheelfwd=(%g,%g,%g)\n",
                    input[0], input[1], io.input[4], io.input[5], io.grounded,
                    F(r,0x9e4),
                    F(r,off::kVelocity), F(r,off::kVelocity+4), F(r,off::kVelocity+8),
                    F(r,0x1a8), F(r,0x26c),                 /* A4 front-wheel steer-angle out */
                    F(r,0x9bc), F(r,0x9c0), F(r,0x9c4),     /* A6a body angular velocity */
                    I(r,off::kWheel0State),                  /* grounded gate */
                    F(r,0x180), F(r,0x184), F(r,0x188));    /* wheel0 right/steer axis sample */
                std::fclose(lf);
            }
            ++dn;
        }
    }
#endif  /* TEMP diag */
    // --- adapter OUT: read back the chain's INTERNAL velocity (round-trips through
    // record +0x9b0 to seed next frame, and feeds car_vel_ consumers — the AI host).
    // The visible world motion is io.drive_delta (velocity vector) + io.yaw (the
    // integrated body basis) — both set above, independently of each other.
    io.vel[0] = F(r, off::kVelocity + 0);
    io.vel[1] = F(r, off::kVelocity + 4);
    io.vel[2] = F(r, off::kVelocity + 8);
    io.speed  = F(r, off::kSpeed);

    if (prof::g_on) prof::f_physMs += prof::NowMs() - t_phys0;   // PERF: whole-step cost
}

}  // namespace mashed_re::Vehicle
}  // namespace mashed_re
