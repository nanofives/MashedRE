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
// #define MASHED_PHYS_DIAG 1   /* enable the steer-chain diag block (G2 drive debug) */

namespace mashed_re {
namespace Vehicle {

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
}

bool VehiclePhysics_Enabled() {
    static const bool e = (std::getenv("MASHED_REAL_PHYSICS") != nullptr);
    return e;
}

void VehiclePhysics_Init(int carCount, int trackType) {
    std::memset(g_records, 0, sizeof(g_records));
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
        SolveWheelContacts(r, io, guard);
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

    // [U-A8-SPEEDCAP] The verbatim chain self-limits LATERAL velocity (grip-clamp #6,
    // Integrate2.cpp 324-348) but not STRAIGHT-line speed — the drive force ramps the
    // internal velocity unbounded (observed 77->553 going straight), far past the small
    // standalone collision strip. Cap the chain VELOCITY MAGNITUDE here (env MASHED_SPEEDCAP,
    // default 60) — applied AFTER the chunk loop so the chain's own grip/yaw dynamics run
    // at full fidelity; only the carried-over velocity is bounded. Kept well above the
    // grip-clamp's low-speed full-stop (16) so it does NOT re-trigger the over-damp that
    // killed +0x9c0 before the contact-load fix. 0 disables.
    {
        static const float kSpeedCapBase = [] {
            const char* e = std::getenv("MASHED_SPEEDCAP");
            return e ? (float)std::atof(e) : 45.0f;   // calibrated 2026-06-18 (smooth lap)
        }();
        // [G4] Per-car cap spread: AI opponents get slightly different top speeds so the
        // field SEPARATES on track (the elimination rule fires only when the race camera's
        // required zoom saturates at 10.0 — i.e. cars spread far apart; identical cars cluster
        // and never resolve a round). A documented standalone substitute for the un-ported
        // per-AI skill/rubber-band variation (FUN_004177b0/FUN_00417180). Slot 0 (player) =
        // full base so a human races at the base pace; opponents 1..3 vary. Env MASHED_AI_SPREAD=0
        // disables (all cars = base).
        static const bool kSpread = [] {
            const char* e = std::getenv("MASHED_AI_SPREAD");
            return !(e && (e[0] == '0'));   // on unless explicitly "0"
        }();
        static const float kSlotCapMul[4] = { 1.00f, 0.86f, 1.00f, 0.92f };
        const float kSpeedCap = kSpeedCapBase *
            ((kSpread && slot >= 1 && slot <= 3) ? kSlotCapMul[slot] : 1.0f);
        if (kSpeedCap > 0.f) {
            float vx = F(r, off::kVelocity + 0), vy = F(r, off::kVelocity + 4), vz = F(r, off::kVelocity + 8);
            float sp = std::sqrt(vx*vx + vy*vy + vz*vz);
            if (sp > kSpeedCap && sp > 1e-4f) {
                float s = kSpeedCap / sp;
                F(r, off::kVelocity + 0) = vx * s;
                F(r, off::kVelocity + 4) = vy * s;
                F(r, off::kVelocity + 8) = vz * s;
                F(r, off::kSpeed) = kSpeedCap;
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
    // --- adapter OUT: read back the integrated body state ---
    io.vel[0] = F(r, off::kVelocity + 0);
    io.vel[1] = F(r, off::kVelocity + 4);
    io.vel[2] = F(r, off::kVelocity + 8);
    io.speed  = F(r, off::kSpeed);

    // WS-A8-STEER: PHYSICS-real heading. The chain integrates the body angular
    // velocity into +0x9bc (vec3, world frame) — A6a FUN_00467650 (Integrate2.cpp
    // 0x9bc/0x9c0/0x9c4) accrues the per-wheel lateral-grip cross-product torque
    // there. The steer input drives it: input[0]/[1] -> A4 front-wheel steer angle
    // (+0x1a8/+0x26c) -> A5 rotates the wheel forward axes -> A6a's grip yaws the
    // body. Our body forward (+0x9d4 = {cos,0,sin}) is about world-up (Y), so the Y
    // component of the angular velocity (+0x9c0) is the yaw rate. Advance io.yaw by
    // it over the frame — this REPLACES the kinematic steer*kSteer*dt stopgap.
    //
    // [U-A8-YAWSCALE] In the original the orientation is integrated from +0x9bc by
    // the RW-Physics rigid-body integrator (the FUN_0055xxxx RW-Physics/qhull island,
    // applied to the RW frame matrix — NOT a first-party function; the standalone has
    // no RW node so we fold +0x9c0 into io.yaw ourselves). The exact rate->orientation
    // scale that engine uses is therefore not recoverable from a first-party decomp;
    // kYawScale is the one tunable here. 1.0 integrates +0x9c0 directly as rad over
    // the frame ms; the follow-up runtime smoke (WS-PHYS-SMOKE) calibrates it to match
    // the original's turn radius. The STEER PATH is physics; only this gain is tuned.
    constexpr std::size_t kAngVelY  = 0x9c0;   // angular-velocity Y = yaw rate
    // [U-A8-YAWSCALE] integrate the chain yaw rate (+0x9c0) over the frame ms. Matched
    // to kWorldVel (TrackRenderer) by the SAME factor so slowing the world pace keeps
    // the turn radius (= world_speed / yaw_rate) constant. Env-tunable (MASHED_YAWSCALE)
    // for runtime calibration without a rebuild; default tracks kWorldVel's 0.12.
    static const float kYawScale = [] {
        const char* e = std::getenv("MASHED_YAWSCALE");
        float v = e ? (float)std::atof(e) : 0.34f;   // calibrated 2026-06-18 (smooth lap)
        return (v > 0.f) ? v : 0.34f;
    }();
    const float yawRate = F(r, kAngVelY);
    io.yaw += yawRate * frameMs * kYawScale;
    // keep yaw bounded so the synthesized forward stays well-conditioned
    constexpr float kTwoPi = 6.2831853f;
    while (io.yaw >  kTwoPi) io.yaw -= kTwoPi;
    while (io.yaw < -kTwoPi) io.yaw += kTwoPi;
}

}  // namespace mashed_re::Vehicle
}  // namespace mashed_re
