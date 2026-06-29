// Mashed RE — WS-A8: standalone driver for the ported vehicle-physics chain.
// See VehiclePhysicsRun.h for status + the documented approximations.
//
// Dispatcher scale constants harvested from FUN_00470c70 (Ghidra pool13, 2026-06-17):
//   _DAT_005cea80 = 0x3b360bc0 = 0.0027809   (suspDtTerm = dt * this)
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
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xf); // A4 0x00470670
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
constexpr float       kSuspDtK = 0.0027809f;   // _DAT_005cea80
constexpr float       kSuspNum = 3000.0f;      // _DAT_005ccd08
// [U-A8-SUBSTEP] the dispatcher FUN_00470c70 runs the chain in <=0x32 (50) chunks
// (local_24 = min(remaining, 0x32)) over the frame's substep budget. We subdivide
// the frame into <=kMaxSubstep fixed ~1ms steps (the dispatcher's chunks are in ms).
constexpr int         kMaxSubstep = 50;        // 0x32 (ms chunk cap)

unsigned char g_records[16 * kRec];            // the 0xd04 record array (mirror of DAT_008815a0)
bool          g_inited = false;

// WS-A COUPLING (2026-06-29): persistent WORLD forward body speed per car — the
// rigid body the recovered coupling law (FUN_0047eb30) drives. The chain's INTERNAL
// velocity persists in the record (+0x9b0, g_records); this is the inertial world
// speed that follows it through the recovered PD relaxation (gain 20). See
// re/analysis/vehicle_coupling.md.
float         g_bodySpeed[16] = { 0.f };

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
                             const unsigned* tris, int triCount) {
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
        ct.normal[0]=nx; ct.normal[1]=ny; ct.normal[2]=nz; ct.material=0; ct.surfaceKey=0;
        g_worldTriStore.push_back(ct);
    }
    Collision::g_worldTris     = g_worldTriStore.empty() ? nullptr : g_worldTriStore.data();
    Collision::g_worldTriCount = static_cast<int>(g_worldTriStore.size());
    prof::g_triCount = Collision::g_worldTriCount;
}

bool VehiclePhysics_Enabled() {
    static const bool e = (std::getenv("MASHED_REAL_PHYSICS") != nullptr);
    return e;
}

void VehiclePhysics_Init(int carCount, int trackType) {
    std::memset(g_records, 0, sizeof(g_records));
    for (int s = 0; s < 16; ++s) g_bodySpeed[s] = 0.f;   // WS-A coupling: reset rigid bodies
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

// Run the ported FUN_0046f6c0 over the track tris (replaces SetGrounded).
static void SolveWheelContacts(unsigned char* r, const PlayerCarIO& io, int substep) {
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
    // _DAT_005cc99c=0.3 < faceNormal . this). No first-party writer for it exists in the
    // ported A5/A6 chain ([UNCERTAIN] which original fn fills it); seed it with the
    // averaged ground normal so the ground triangle passes the approach test.
    F(r, 0x9c8) = avgN[0]; F(r, 0x9cc) = avgN[1]; F(r, 0x9d0) = avgN[2];

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

void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io) {
    VehiclePhysics_StepCar(0, dt, io);
}

void VehiclePhysics_StepCar(int slot, float dt, PlayerCarIO& io) {
    if (!g_inited) VehiclePhysics_Init(4, 0);
    if (dt <= 0.f) return;
    if (slot < 0 || slot >= 16) return;
    unsigned char* r = rec(slot);

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
    {
        float st = io.steer;
        if (st >  1.0f) st =  1.0f;
        if (st < -1.0f) st = -1.0f;
        const int mag = static_cast<int>((st < 0.f ? -st : st) * 255.f + 0.5f);
        const std::uint8_t m = static_cast<std::uint8_t>(mag > 255 ? 255 : mag);
        input[0] = (st > 0.f) ? m : 0;
        input[1] = (st < 0.f) ? m : 0;
    }

    // The chain works in the ORIGINAL's millisecond time base: FUN_00470c70 passes
    // A4 a dt that is the integer ms chunk count (local_24 = min(remaining,0x32)),
    // and computes the per-frame suspension scale from the FRAME dt in ms:
    //   _DAT_0088e610 (suspDtTerm) = frameMs * _DAT_005cea80 (0.0027809)
    //   _DAT_0088e5f0 (suspScale)  = _DAT_005ccd08 (3000) / suspDtTerm
    // (the chain constants — kDt 3.33e-4, etc. — are calibrated for ms, NOT seconds).
    const float frameMs = dt * 1000.0f;
    g_suspDtTerm = frameMs * kSuspDtK;
    g_suspScale  = (g_suspDtTerm != 0.f) ? (kSuspNum / g_suspDtTerm) : 0.f;

    // The body-forward/wheel-axis world transform A5 needs (zeroed +0x928 wheel
    // matrix block produced (0,0,0) -> no drive direction -> no motion). Synthesize
    // it from the car's yaw each frame (origin position; only the rotation axes are
    // read by A5's forward/right-axis transforms).
    float xform[16];
    BuildYawMatrix(io.yaw, xform);

    // Subdivide the frame ms budget into <=50ms chunks (the FUN_00470c70 chunk loop,
    // local_24 = min(remaining,0x32)); A4's dt per chunk is that ms count.
    float remMs = frameMs;
    int guard = 0;
    while (remMs > 0.0f && guard++ < 64) {
        float chunkMs = (remMs < (float)kMaxSubstep) ? remMs : (float)kMaxSubstep;
        // WS-A contacts: run the REAL ported wheel-contact solver (FUN_0046f6c0) over the
        // track tris instead of the flat SetGrounded substitute -> per-wheel slope normals
        // (+0x200) + per-wheel grounding (-> A5's mass/grounded_count load = weight transfer).
        const double tp0 = prof::g_on ? prof::NowMs() : 0.0;
        SolveWheelContacts(r, io, guard);
        if (prof::g_on) { prof::f_probeMs += prof::NowMs() - tp0; ++prof::f_substeps; }
        g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;
        VehicleControlIntegrate(reinterpret_cast<int*>(r), chunkMs, input, xform);
        // A6a's drive block can clear wheel states in some branches; re-assert from the
        // cached contact result (no re-broadphase) so the next chunk stays engaged.
        ReassertContacts(r);
        remMs -= chunkMs;
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
        constexpr float kSafetyInternal = 1500.0f;   // >> tanh-saturation point (~3*top/scale)
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
    // WS-A COUPLING (2026-06-29): the recovered first-party chain->body coupling
    // law (FUN_0047eb30 — re/analysis/vehicle_coupling.md), replacing the degenerate
    // hard kSpeedCap clamp + TrackRenderer's kWorldVel/kYawScale gains.
    //
    // The original springs an RW-Physics proxy body to the chain's render position
    // with PD gain _DAT_005ccd6c=20.0 and a velocity lookahead
    // _DAT_005cd8fc*_DAT_005cf014 = 300*0.0002 = 0.06 s, then reads the integrated
    // body transform back (renderPos=bodyPos) — a 2-body CLOSED loop. Transcribed
    // open-loop onto a single body it DIVERGES (simulated: the +vel*0.06 lookahead
    // self-amplifies ~1.2x/tick with no readback to cancel the 20*(anchor-pos) term).
    // So this is the faithful single-body REDUCTION: the car body follows the chain's
    // force-integrated velocity (+0x9b0 = the real accel ramp / throttle response)
    // through the recovered PD relaxation (gain 20 -> inertia + smooth ramp), with a
    // SOFT top-speed asymptote replacing the hard 45 clamp (the hard clamp is what
    // produced "instant pin to 45"). Direction = the chain-grip heading (+0x9c0)
    // with full lateral grip; speed emitted in WORLD units (io.drive_speed).
    // ========================================================================
    {
        const float dtSec = dt;            // StepCar's dt is ALREADY seconds (in.dt;
                                           // the chain converts to ms via frameMs).

        // forward axis from the integrated heading (TrackRenderer convention forward={cos,0,sin}).
        const float fwdx = std::cos(io.yaw), fwdz = std::sin(io.yaw);
        // chain force-integrated velocity (+0x9b0), HORIZONTAL part (internal units).
        // The chain ramps |+0x9b0| from 0 over ~2 s (the real throttle force ramp);
        // its horizontal magnitude is the body's forward speed, its direction is the
        // heading the grip is driving the car toward.
        const float cvx = F(r, off::kVelocity + 0);
        const float cvz = F(r, off::kVelocity + 8);
        const float horizSpeed = std::sqrt(cvx*cvx + cvz*cvz);   // internal, >=0
        const float fwdDot     = cvx*fwdx + cvz*fwdz;            // signed (reverse < 0)
        const float vsign      = (fwdDot >= 0.f) ? 1.0f : -1.0f;

        // internal -> world unit scale: the ONE linear calibration. The original
        // needs none (RW proxy + render share world units); the standalone chain
        // self-limits in internal units, so convert. The chain's horizontal-speed
        // RAMP (0 -> ~1440 over ~2 s) maps LINEARLY to the world speed ramp, so the
        // accel ramp is the chain's real force-integration ramp (not an instant pin).
        // Env MASHED_CHAINSCALE (replaces the old TrackRenderer kWorldVel).
        static const float kChainScale = [] {
            const char* e = std::getenv("MASHED_CHAINSCALE");
            float v = e ? (float)std::atof(e) : 0.0083f;  // ~12 world top at the chain's ~1440 internal
            return (v > 0.f) ? v : 0.0083f;
        }();
        // world top-speed clamp (the natural top, chain_internal*scale, sits just under
        // it so the ramp is smooth, not a hard pin). Old effective top was 45*0.22 ~= 10
        // world u/s; keep that scale. Env MASHED_TOPSPEED.
        static const float kTopSpeedBase = [] {
            const char* e = std::getenv("MASHED_TOPSPEED");
            float v = e ? (float)std::atof(e) : 12.0f;
            return (v > 0.f) ? v : 12.0f;
        }();
        // [G4] per-car top-speed spread (unchanged intent: AI tops differ so the
        // field SEPARATES on track — the elimination rule needs cars to spread).
        // Slot 0 (player) = base; opponents 1..3 vary. Env MASHED_AI_SPREAD=0 disables.
        static const bool kSpread = [] {
            const char* e = std::getenv("MASHED_AI_SPREAD");
            return !(e && (e[0] == '0'));
        }();
        static const float kSlotCapMul[4] = { 1.00f, 0.86f, 1.00f, 0.92f };
        const float kTopSpeed = kTopSpeedBase *
            ((kSpread && slot >= 1 && slot <= 3) ? kSlotCapMul[slot] : 1.0f);
        float desiredSp = vsign * horizSpeed * kChainScale;       // world, signed
        if (desiredSp >  kTopSpeed)        desiredSp =  kTopSpeed;
        if (desiredSp < -kTopSpeed * 0.4f) desiredSp = -kTopSpeed * 0.4f;   // reverse slower

        // recovered PD relaxation (gain _DAT_005ccd6c @0x005ccd6c = 20.0): the body
        // speed approaches the desired speed at rate 20/s -> inertia (coasts on
        // release). At low fps (a -> 1) it tracks desiredSp, whose own ramp is the
        // chain's; at high fps it adds the recovered 0.05 s smoothing.
        constexpr float kPdGain = 20.0f;     // _DAT_005ccd6c
        float a = kPdGain * dtSec;
        if (a > 1.0f) a = 1.0f;
        float& bs = g_bodySpeed[slot];
        bs += (desiredSp - bs) * a;

        // recovered ANGULAR law (FUN_0047eb30): the body faces its VELOCITY heading
        // (the original sets body angVel.y = wrapped(bodyHeading - velHeading); the
        // body rotates to align forward with where the chain velocity points). This
        // is BOUNDED (|heading error| <= pi) so it is framerate-stable, unlike the old
        // yawRate*frameMs integration which spun at low fps. The chain's grip (A6a)
        // curves the velocity direction with the steer input -> facing it turns the
        // car. Env MASHED_ALIGNRATE = how fast the nose snaps to velocity (1/s).
        static const float kAlignRate = [] {
            const char* e = std::getenv("MASHED_ALIGNRATE");
            float v = e ? (float)std::atof(e) : 7.0f;
            return (v > 0.f) ? v : 7.0f;
        }();
        constexpr float kTwoPi = 6.2831853f, kPi = 3.14159265f;
        if (fwdDot > 0.f && horizSpeed > 1e-3f) {     // moving forward with real velocity
            float velHeading = std::atan2(cvz, cvx);  // forward={cos,0,sin} -> heading=atan2(z,x)
            float err = velHeading - io.yaw;
            while (err >  kPi) err -= kTwoPi;
            while (err < -kPi) err += kTwoPi;
            const float frac = 1.0f - std::exp(-kAlignRate * dtSec);   // fps-independent
            io.yaw += err * frac;
        }
        while (io.yaw >  kTwoPi) io.yaw -= kTwoPi;
        while (io.yaw < -kTwoPi) io.yaw += kTwoPi;

        // OUT: coupled WORLD forward speed (signed). TrackRenderer integrates
        // pos += {cos,0,sin(io.yaw)} * io.drive_speed * dt (no kWorldVel).
        io.drive_speed = bs;

        // TEMP coupling diag (env MASHED_COUPLING_DIAG): the chain velocity vector,
        // its forward projection, the yaw rate, and the emitted body speed — to
        // calibrate the coupling. Slot 0 only.
        {
            static const bool s_cd = (std::getenv("MASHED_COUPLING_DIAG") != nullptr);
            if (s_cd && slot == 0) {
                static int cn = 0;
                if (cn < 2000) {
                    const float cvy = F(r, off::kVelocity + 4);
                    if (std::FILE* lf = std::fopen("coupling_diag.log", "a")) {
                        std::fprintf(lf, "cv=(%.2f,%.2f,%.2f) horiz=%.2f fwdDot=%.2f "
                            "desired=%.3f bs=%.3f frameMs=%.1f yaw=%.4f velH=%.4f\n",
                            cvx, cvy, cvz, horizSpeed, fwdDot, desiredSp, bs,
                            frameMs, io.yaw,
                            (horizSpeed > 1e-3f ? std::atan2(cvz, cvx) : 0.f));
                        std::fclose(lf);
                    }
                    ++cn;
                }
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
    // The visible world motion is io.drive_speed + io.yaw, set by the coupling above.
    io.vel[0] = F(r, off::kVelocity + 0);
    io.vel[1] = F(r, off::kVelocity + 4);
    io.vel[2] = F(r, off::kVelocity + 8);
    io.speed  = F(r, off::kSpeed);

    if (prof::g_on) prof::f_physMs += prof::NowMs() - t_phys0;   // PERF: whole-step cost
}

}  // namespace mashed_re::Vehicle
}  // namespace mashed_re
