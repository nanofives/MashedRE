// Mashed RE — R4 opener track renderer implementation. See TrackRenderer.h.

#include "TrackRenderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>   // WS-A s3 PERF: static-geometry vertex-buffer cache

#include "../Piz/PizReader.h"
#include "../Track/TrackWorld.h"
#include "../Track/DffModel.h"
#include "../Track/PowerupMarkers.h"  // POWERUPS_GOLD.DFF placement (FUN_00426460)
#include "../Render/ArcticTrackNodeSlot0.h"  // the real 0x00448940 body + its sea grid
#include "../Track/LapLogic.h"      // F4 lap-line crossing sequence (shared w/ test)
#include "../Txd/TxdDecoder.h"
#include "../Audio/AudioEngine.h"   // real SFX (permdict.rws) for countdown/powerups
#include "../LibRw/RwRaceSubmit.h"  // E2'b step 3: in-loop librw submit (the shipping race renderer)
#include "../LibRw/RwCarFx.h"       // rtex: car dirt / env-map set-up tables (U-9693)
#include "../LibRw/RwRaceFx.h"      // rfx: car shadow / headlight beam / HUD on librw
#include "PngLoader.h"              // rtex: dirt.piz PNGs (WIC)
// rtex: 0x004c4d20 RwMatrixRotate (Math/RwMatrixRotate.cpp), for RwFrameRotate's REPLACE build
extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);
#include "DrawStreamDump.h"         // parity harness: MASHED_DBG_DRAWSTREAM3D race-3D summary
#include "DrawRec.h"                // d1r1: per-draw recorder names (MASHED_DBG_D3DREC)
#include "../Ai/AiStandalone.h"     // WS-C: racing-line target Ai_ComputeTarget is the DEFAULT
#include "../Race/SubStateCallees.h"  // D-11073 leg 1a: FUN_0041d930 tail partial (MASHED_SUBSTATE)
#include "../Race/PathNodes.h"    // D-11072 P1: course block (MASHED_PATHPROG)
#include "../Race/PathProgStep.h" // D-11072 P2: per-frame FUN_00408610 (MASHED_PATHPROG)
#include "../Ai/AiTargetClosest.h" // D-11072 P3: FUN_00414a70 (MASHED_TGT3)
#include "../Ai/AiLeafTargets.h"   // D3 bulk G-AILEAF: FUN_00414570 / FUN_00415880 (MASHED_AILEAF)
#include "../Race/RoundGrid.h"      // round-restart grid Stage A (MASHED_ROUND_GRID)
#include "../Vehicle/PosModelKnobs.h" // [d2posmodel] fix B knobs (default OFF)
#include "../Race/WorldRegistry.h"  // mode 7 M1: world-object registry (MASHED_REGISTRY)
#include "../Gameplay/PickupTick.h"  // mode 7 M2a: pickup tick FUN_00459000 (MASHED_PICKUPS_ORIG)
#include "../Race/CarRegistrant.h"  // mode 7 M2b: car registrant FUN_0041f710 (MASHED_REG_CARS)
#include "../Race/SubStateMachine.h"  // D-11073 leg 1b: FUN_0040dbd0 / FUN_004103a0 (MASHED_SUBSTATE)
#include "../Race/CameraEntries.h"    // D-11073 leg 2: phase-3 camera entries (MASHED_SUBSTATE)
#include "../Race/CameraApply.h"      // d1r2: the director tail's frame build (render camera)
extern "C" void __cdecl VehicleSeedWritePair(std::uint32_t, std::uint32_t);   // 0x00448700
extern "C" void __cdecl UtilZeroTable8964c0();                                // 0x004425d0
// [librwrest] FUN_00420870's part poses: RwFrameTranslate's matrix half (0x004c51a0, ported in
// Collision/RwpSolverMath2.cpp), and FUN_0040da50's per-car gate (CarRegistrant.cpp's pair).
extern "C" void __cdecl FUN_004c51a0(float* mtx, float* vec, int combineOp);   // RwMatrixTranslate
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                 // 0x0040e370
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);   // 0x0046c7b0
extern "C" void __cdecl SplineCubicBlend3(float t, float* out, float* p0, float* p1, float* p2,
                                          float* p3);                           // 0x00482ae0
#include "../Vehicle/VehicleRecordBase.h"
                                    // (MASHED_GATE_RIBBON_AI reverts it to the ribbon scaffold).
                                    // [D3 2026-09-26] the ported TICK Ai_Standalone_Tick runs every
                                    // race frame and its ctrl bytes drive the opponents.
#include "../Vehicle/VehiclePhysicsRun.h"  // WS-A8: ported physics chain (behind MASHED_REAL_PHYSICS)
#include "../Vehicle/ForceIntegrator.h"    // U-9174: LaunchRev_PreRaceTick / _Release
#include "../Ai/AiState.h"          // WS-AI-BRIDGE: ctrl-block / slot-table / spline addrs
#include "../Ai/AiData.h"           // WS-AI-BRIDGE: .AI loader (AiData_LoadInto)

// [GATEFIRE idx364 2026-10-08] FUN_00414060's step-6 tail, default-OFF behind
// MASHED_A364_RESET. Body: Race/AiDifficultyReset.cpp. Declared extern "C" with
// a flat name because the call site below sits inside
// namespace mashed_re::D3d9Render and cannot re-open mashed_re::Race.
extern "C" void __cdecl RaceAiDifficultyResetTail();

namespace mashed_re {
namespace D3d9Render {

namespace {

// ===========================================================================
// WS-AI-BRIDGE (2026-06-17): make the standalone AI tick (Ai_Standalone_Tick,
// Ai/AiStandalone.cpp) actually drive the opponents.
// [D3 2026-09-26] ACHIEVED: all three parts are the default. The tick runs every race
// frame (UpdateCar, faithful_nav block) and each opponent's ctrl block is fed to the
// ported physics chain (VehiclePhysics_StepCar, raw_steer). The pre-D3 Option B motion
// model survives only as TrackRenderer::AiOptionBStep, reached on MASHED_REAL_PHYSICS=0.
// re/analysis/D3_AI_PORT_2026-09-26.md.
// Three parts, as originally planned:
//   (1) Ai_BridgeLoad  — load AI<course>.AI (Common/AI.piz) into the controller
//       image @0x007f1a9c so the race-line banks fill (tick non-inert) + init the
//       per-vehicle slot table and behaviour records (race line, type0/idx0).
//   (2) host snapshot + Ai::Host fns — feed the tick each car's world pos/vel
//       (Ai::Host uses context-free fn ptrs, so they read file-static g_aib).
//   (3) the adapter (inline in UpdateCar) reads the tick's ctrl-block outputs and
//       steers/throttles ai_cars_, REPLACING the gate-ribbon scaffold when on.
// Absolute writes (0x007f1a9c, kSlotTableBase, kCtrlBlockBase) target the
// image-pad in mashed_re.exe (valid writable memory — standalone-exe-phase-h);
// everything runs only on the standalone path (there is NO MASHED_REAL_AI accessor —
// dead name retired 2026-09-09, FLAG_INVENTORY_2026-08-15 class C; the live gates are
// MASHED_GATE_RIBBON_AI at :1813 and MASHED_AI_DRIVES_PLAYER at :2660) so the dev .asi
// (injected into MASHED.exe) never clobbers the original's live AI image. PENDING diff-original C4 — the
// ctrl->yaw/throttle mapping (turn rate, accel bands) is APPROXIMATE: the exact
// bands live in the un-ported FUN_00416250 ([U-C-BANDS]).
// ===========================================================================
struct AiBridgeState {
    float pos[4][2];   // [v] = {x,z};  v0 = player, v1..3 = ai_cars_[v-1]
    float vel[4][2];
    float fwd[4][2];   // [D3 2026-09-26] body forward {cos yaw, sin yaw} (FUN_0046d510)
    int   alive[4];
    int   course;
    bool  loaded;
};
AiBridgeState g_aib = {};
// Active TrackRenderer for the AI host's LOS query (set each frame in the real_ai
// block). The aib_* host fns are context-free, so the GroundHeight raycast they need
// for the spline lookahead's wall-march reaches the instance through this pointer.
const TrackRenderer* g_aiTrack = nullptr;
TrackRenderer*       g_pkcbTrack = nullptr;   // [mode 7 M3] the grant host mutates pw_ (set with g_aiTrack, UpdateCar)

void aib_own_xz(int v, float* x, float* z) {
    if (v < 0 || v > 3) { *x = *z = 0.f; return; }
    *x = g_aib.pos[v][0]; *z = g_aib.pos[v][1];
}
void aib_own_vel_xz(int v, float* vx, float* vz) {
    if (v < 0 || v > 3) { *vx = *vz = 0.f; return; }
    *vx = g_aib.vel[v][0]; *vz = g_aib.vel[v][1];
}
// [D3 2026-10-03] MEASUREMENT CONTROL, default OFF. See the comment at the
// elimination block in TrackRenderer::UpdateRaceRules for what it suppresses and
// why. Read once; unset, every caller is `if (... && true)` on the old condition.
static bool NoElim() {
    static const char* s_p = std::getenv("MASHED_NO_ELIM");
    return s_p && s_p[0] && s_p[0] != '0';
}
int aib_alive(int v)       { return (v >= 0 && v <= 3) ? g_aib.alive[v] : 0; }
int aib_veh_type(int v)    { return v == 0 ? 0 : 2; }   // 0 = player(human); else AI
// [D-11073 leg 1b] Under default-OFF MASHED_SUBSTATE the port keeps a real DAT_0063ba8c
// (written by the sub-state block in Update and by SubStateEnter4 / SubStateHeldCountdown5,
// Race/SubStateMachine.cpp); FUN_0040e350 returns the race sub-state. Knob OFF: 6, as before.
int aib_game_sub_mode() {                                // race (FUN_0040e350)
    if (SubState_Enabled()) {
        const int s = *reinterpret_cast<const int*>(0x0063ba8cu);
        if (s != 0) return s;
    }
    return 6;
}
int pkcb_sub_mode()        { return aib_game_sub_mode(); }   // [mode 7 M3] the same FUN_0040e350 host
int aib_round_type()       { return 3; }                // AI-enabled round (FUN_0042f6a0)
int aib_game_mode_fd0()    { return 0; }
int aib_track_index()      { return g_aib.course; }
// [D3 2026-09-26] FUN_00443080 = MOV EAX,[0x00897ffc]. MEASURED 0 on every AI step of
// the original race (verify/d3_ai_20260926/o_inputs.msd.aistep.csv, tgt_7ffc column,
// 6259/6259). This returned 1, which (a) skipped FUN_00416250's sub-state-6 block, the
// one that calls the fire decision, (b) stepped the PLAYER through the AI tick
// (FUN_00418860's `|| FUN_00443080() == 1`) and (c) forced DAT_0089a368 = 2 every frame.
int aib_ai_target_enable() { return 0; }
void aib_own_fwd_xz(int v, float* fx, float* fz) {
    if (v < 0 || v > 3) { *fx = *fz = 0.f; return; }
    *fx = g_aib.fwd[v][0]; *fz = g_aib.fwd[v][1];
}
// ---------------------------------------------------------------------------
// [D3 2026-10-05] U-9188 legs A and B. DEFAULT OFF: with MASHED_AI_YAWW unset
// every entry point here returns on its first line and nothing is written.
// Counters and file output only — no game value is derived from any of it.
// Pre-registration: verify/d3_yaww_20261005/PREREG_YAWW.md.
//
// Leg A counts all NINE `a.yaw` writers per vehicle slot, plus whether each
// site's VehiclePhysics_ResetOrientation was reached, plus faithful_nav / phys,
// so "unreachable by control flow" is WITNESSED and not inferred. U-9188
// enumerates five writers; `:2690` and `:3304` are missing from it (PREREG
// Correction 5). `W_3334` is the mandatory coverage control — a zero there means
// this block never executed, which VOIDs the leg rather than proving a negative
// (memories arm-coverage-counters-before-first-run,
// absent-log-proves-nothing-run-a-control).
//
// Leg B measures, IN-PROCESS at two known program points on the same frame, the
// angle between the heading AiBridgeSnapshot publishes and the record's forward
// row (+0x9d4/+0x9dc). sa_headwatch.py measures that same quantity by
// ReadProcessMemory poll; this has no poll and so no H-JITTER term, which is
// what separates H1 (a real writer) from H2 (a poll artifact) in the PREREG.
// ---------------------------------------------------------------------------
namespace yaww {

enum Site {
    W_2690 = 0,   // a.yaw = baseYaw          grid placement        (no basis reseed)
    W_3283,       // a.yaw += 12.0f * dt      spin scaffold, phys path
    R_3285,       //    .. its VehiclePhysics_ResetOrientation
    W_3304,       // a.yaw = atan2(...)       respawn relocation
    R_3307,       //    .. its VehiclePhysics_ResetOrientation
    W_3334,       // a.yaw = io.yaw           THE SYNC — coverage control
    W_3351,       // a.yaw = ry               off-mesh re-aim
    R_3355,       //    .. its VehiclePhysics_ResetOrientation
    W_3393,       // a.yaw += 12.0f * dt      legacy v2 spin        (unreachable, (b) arm)
    W_3423,       // a.yaw += yerr * ..       legacy v2 ribbon limiter   ("      ")
    W_3683,       // a.yaw += yerr * ..       AiOptionBStep limiter      ("      ")
    W_3717,       // a.yaw = ry               AiOptionBStep off-mesh     ("      ")
    SITE_N
};

const char* const kSite[SITE_N] = {
    "w2690_grid",    "w3283_spin",     "r3285_reset",   "w3304_respawn",
    "r3307_reset",   "w3334_sync",     "w3351_offmesh", "r3355_reset",
    "w3393_v2spin",  "w3423_v2ribbon", "w3683_oblimit", "w3717_oboffmesh",
};

static const char* Path() {
    static const char* s_p = std::getenv("MASHED_AI_YAWW");
    return (s_p && s_p[0]) ? s_p : nullptr;
}
static bool On() { static const bool s_on = (Path() != nullptr); return s_on; }

struct State {
    long   frame;
    int    nav, phys;
    long   count[SITE_N][4];
    // per-frame scratch, reset by Frame()
    double pubYaw[4];   bool pubOk[4];
    double pubRec[4];   bool pubRecOk[4];
    double postYaw[4];  bool postOk[4];
    double postRec[4];  bool postRecOk[4];
    // accumulated samples, for the medians the gates are written against
    std::vector<float> sPubRec[4], sPostRec[4], sPubPost[4];
    std::FILE* fp;
};
static State& S() { static State s = {}; return s; }

// |wrap(b - a)| in degrees. Both arguments are headings, so this is exactly the
// angle between the unit vectors cos/sin of them.
static double AngDeg(double a, double b) {
    const double kPi = 3.14159265358979323846;
    double d = b - a;
    while (d >  kPi) d -= 2.0 * kPi;
    while (d < -kPi) d += 2.0 * kPi;
    return (d < 0.0 ? -d : d) * (180.0 / kPi);
}

// atan2 of the record's forward row. off::kForward = 0x9d4 is fwd.x (VehicleStruct.h:105),
// +8 = 0x9dc is fwd.z. Written ONLY as cos/sin(io.yaw) at VehiclePhysicsRun.cpp:621-623,
// :820-821 and :1004-1005 (PREREG Correction 3). Returns ok=false on an all-zero row,
// which is the uninitialised record and must not be fitted as a heading of 0.
static double RecYaw(int v, bool* ok) {
    const float fx = Vehicle::VehiclePhysics_RecordF32(v, 0x9d4);
    const float fz = Vehicle::VehiclePhysics_RecordF32(v, 0x9dc);
    const bool nz = (fx != 0.f) || (fz != 0.f);
    if (ok) *ok = nz;
    return nz ? std::atan2(static_cast<double>(fz), static_cast<double>(fx)) : 0.0;
}

void Hit(int site, int v) {
    if (!On() || v < 0 || v > 3 || site < 0 || site >= SITE_N) return;
    ++S().count[site][v];
}

void Flags(bool nav, bool phys) {
    if (!On()) return;
    S().nav = nav ? 1 : 0;
    S().phys = phys ? 1 : 0;
}

// called from AiBridgeSnapshot, with the yaw that slot is about to publish
void Pub(int v, double yaw) {
    if (!On() || v < 0 || v > 3) return;
    State& s = S();
    s.pubYaw[v] = yaw; s.pubOk[v] = true;
    bool rok = false; const double ry = RecYaw(v, &rok);
    s.pubRecOk[v] = rok;
    if (rok) s.pubRec[v] = AngDeg(yaw, ry);
}

// called immediately after `a.yaw = io.yaw`, with the freshly synced yaw
void Post(int v, double yaw) {
    if (!On() || v < 0 || v > 3) return;
    State& s = S();
    s.postYaw[v] = yaw; s.postOk[v] = true;
    bool rok = false; const double ry = RecYaw(v, &rok);
    s.postRecOk[v] = rok;
    if (rok) s.postRec[v] = AngDeg(yaw, ry);
}

static double Median(std::vector<float>& x) {
    if (x.empty()) return -1.0;
    std::vector<float> t(x);
    std::sort(t.begin(), t.end());
    const std::size_t n = t.size();
    return (n & 1) ? static_cast<double>(t[n / 2])
                   : 0.5 * (static_cast<double>(t[n / 2 - 1]) + static_cast<double>(t[n / 2]));
}

static void WriteSummary() {
    State& s = S();
    std::string sp = std::string(Path()) + ".summary.txt";
    std::FILE* f = std::fopen(sp.c_str(), "w");
    if (!f) return;
    std::fprintf(f, "U-9188 leg A/B summary   frames=%ld  faithful_nav=%d  phys=%d\n",
                 s.frame, s.nav, s.phys);
    std::fprintf(f, "PREREG verify/d3_yaww_20261005/PREREG_YAWW.md\n\n");
    std::fprintf(f, "LEG A — a.yaw writer counts per slot (slot0=player, 1..3=AI)\n");
    std::fprintf(f, "%-18s %10s %10s %10s %10s\n", "site", "slot0", "slot1", "slot2", "slot3");
    for (int i = 0; i < SITE_N; ++i)
        std::fprintf(f, "%-18s %10ld %10ld %10ld %10ld\n", kSite[i],
                     s.count[i][0], s.count[i][1], s.count[i][2], s.count[i][3]);
    std::fprintf(f, "\nA-COV: w3334_sync must be > 0 on every live AI slot, else leg A is VOID.\n");
    std::fprintf(f, "A-REACH: w3393/w3423/w3683/w3717 predicted 0 with faithful_nav=1 phys=1.\n\n");
    std::fprintf(f, "LEG B — in-process angle, degrees. pub_rec is sa_headwatch's quantity\n");
    std::fprintf(f, "with no poll. Slot 0 is CONSTRUCTION-ZERO, not a floor (PREREG 3b).\n");
    std::fprintf(f, "%-6s %8s %12s %12s %12s %12s %12s\n",
                 "slot", "n", "pub_rec_med", "pub_rec_max", "post_rec_med",
                 "post_rec_max", "pub_post_med");
    for (int v = 0; v < 4; ++v) {
        double a = Median(s.sPubRec[v]), b = Median(s.sPostRec[v]), c = Median(s.sPubPost[v]);
        float ma = 0.f, mb = 0.f;
        for (std::size_t j = 0; j < s.sPubRec[v].size(); ++j)  if (s.sPubRec[v][j]  > ma) ma = s.sPubRec[v][j];
        for (std::size_t j = 0; j < s.sPostRec[v].size(); ++j) if (s.sPostRec[v][j] > mb) mb = s.sPostRec[v][j];
        std::fprintf(f, "%-6d %8d %12.6f %12.6f %12.6f %12.6f %12.6f\n",
                     v, static_cast<int>(s.sPubRec[v].size()), a, (double)ma, b, (double)mb, c);
    }
    std::fprintf(f, "\nB-INPROC: U-9188 medians to reproduce are 0.5214 / 1.1430 / 0.4300 deg\n");
    std::fprintf(f, "on slots 1/2/3. In band [0.25x,4x] on all three = H1. All three below\n");
    std::fprintf(f, "0.05 = H2 (poll artifact). Anything else = INCONCLUSIVE, by PREREG.\n");
    std::fclose(f);
}

// one call per frame, after both Pub and Post have run
void Frame() {
    if (!On()) return;
    State& s = S();
    if (!s.fp) {
        s.fp = std::fopen(Path(), "w");
        if (s.fp) std::fprintf(s.fp, "frame,nav,phys,slot,pub_yaw,post_yaw,rec_ok,"
                                     "ang_pub_rec,ang_post_rec,ang_pub_post\n");
    }
    for (int v = 0; v < 4; ++v) {
        if (s.pubRecOk[v]) s.sPubRec[v].push_back(static_cast<float>(s.pubRec[v]));
        if (s.postRecOk[v]) s.sPostRec[v].push_back(static_cast<float>(s.postRec[v]));
        double pp = -1.0;
        if (s.pubOk[v] && s.postOk[v]) {
            pp = AngDeg(s.pubYaw[v], s.postYaw[v]);
            s.sPubPost[v].push_back(static_cast<float>(pp));
        }
        if (s.fp && (s.pubOk[v] || s.postOk[v])) {
            std::fprintf(s.fp, "%ld,%d,%d,%d,%.9g,%.9g,%d,%.9g,%.9g,%.9g\n",
                         s.frame, s.nav, s.phys, v,
                         s.pubOk[v]  ? s.pubYaw[v]  : -999.0,
                         s.postOk[v] ? s.postYaw[v] : -999.0,
                         (s.pubRecOk[v] ? 1 : 0) + (s.postRecOk[v] ? 2 : 0),
                         s.pubRecOk[v]  ? s.pubRec[v]  : -1.0,
                         s.postRecOk[v] ? s.postRec[v] : -1.0,
                         pp);
        }
        s.pubOk[v] = s.postOk[v] = s.pubRecOk[v] = s.postRecOk[v] = false;
    }
    ++s.frame;
    // flushed on an interval, not at exit: the capture harness kills by PID, so an
    // atexit dump would never run (memory race-capture-wait-for-exit).
    if ((s.frame % 120) == 0) { if (s.fp) std::fflush(s.fp); WriteSummary(); }
}

}  // namespace yaww
// rec+0x9e4 / +0xb0c of slot v, read from the ported physics record (the standalone
// mirror of 0x008815a0 + v*0xd04). Opponents are stepped through it since D3 2026-09-26.
float aib_veh_f32(int v, int off) { return Vehicle::VehiclePhysics_RecordF32(v, off); }
// *[0x0088fc88 + v*0xb4]: the held type code, 0 when none (the original tests the
// entry pointer for NULL at 0x004152b4).
int aib_held_powerup(int v) {
    if (!g_aiTrack || v < 0 || v >= Powerup::PowerupSystem::kSlots) return 0;
    if (!g_aiTrack->pw_.Armed(v)) return 0;
    const int c = g_aiTrack->pw_.ActiveCode(v);
    return c < 0 ? 0 : c;
}
// [mode 7 M3] hosts for FUN_00458920's bound callees (Gameplay/PickupTick.cpp; PREREG_M3.md section 1).
// FUN_0045bff0: [0x0088fc88 + v*0xb4] == 0 -> the slot holds no active entry; Activate / Deactivate set and
// clear activeCode (+0xa8) and armed (+0xac) together (PowerupSystem.cpp:108-134).
int  pkcb_slot_empty(int v) {
    if (!g_aiTrack || v < 0 || v >= Powerup::PowerupSystem::kSlots) return 1;
    return g_aiTrack->pw_.Armed(v) ? 0 : 1;
}
void pkcb_grant(int v, int type) { if (g_pkcbTrack) g_pkcbTrack->PickupGrant(v, type); }   // FUN_0045c010
int  pkcb_sub_mode();                                                                  // FUN_0040e350
// LOS clearance for the lookahead wall-march (FUN_00443dc0 Phase 8): march the
// straight segment (ax,az)->(bx,bz) and report blocked (0) if any intermediate
// sample leaves the drivable surface. Backs Ai::Host::los_clear with the track
// collision (GroundHeight) instead of the original's AI tile grid.
int aib_los_clear(float ax, float az, float bx, float bz) {
    if (!g_aiTrack) return 1;
    const float dx = bx - ax, dz = bz - az;
    const float dist = std::sqrt(dx*dx + dz*dz);
    if (dist < 1e-4f) return 1;
    const int n = 10;
    for (int i = 1; i < n; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(n);
        bool ok = false;
        g_aiTrack->GroundHeight(ax + dx*t, az + dz*t, &ok);
        if (!ok) return 0;
    }
    return 1;
}

// (1) load AI<course>.AI from Common/AI.piz into the controller image.
bool Ai_BridgeLoad(int course, const char* trackPizPath) {
    // derive <...>/Common/AI.piz from <...>/TRACKS/<track>.piz (mirrors LED.piz).
    std::string ai(trackPizPath ? trackPizPath : "");
    std::size_t cut = ai.find_last_of("/\\");
    if (cut == std::string::npos) return false;
    cut = ai.find_last_of("/\\", cut - 1);
    if (cut == std::string::npos) return false;
    ai.resize(cut + 1);
    ai += "Common\\AI.piz";

    Piz::Archive arc;
    if (!arc.Load(ai.c_str())) return false;
    char want[32];
    std::snprintf(want, sizeof(want), "AI%d.AI", course);
    const std::uint8_t* raw = nullptr; std::uint32_t len = 0;
    for (std::uint32_t i = 0; i < arc.count(); ++i) {
        const char* n = arc.entry(i).name;
        const char* base = n;
        for (const char* p = n; *p; ++p) if (*p == '\\' || *p == '/') base = p + 1;
        if (_stricmp(base, want) == 0) { raw = arc.blob(i, &len); break; }
    }
    if (!raw) return false;
    if (!Ai::AiData_LoadInto(raw, len, reinterpret_cast<void*>(0x007f1a9cu))) return false;

    // slot table: vehicle v -> ctrl slot v (image-pad is zeroed; make it explicit).
    // NOTE kSlotTableBase is 0x007f1a14, the AI ctrl-SLOT-INDEX table. It is NOT the
    // slot-STATE table seeded just below; the two are unrelated despite the names.
    for (int v = 0; v < 4; ++v)
        Ai::I32(Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * Ai::kSlotTableStride) = v;
    // [D-11072 leg C 2026-10-06] slot-STATE table self-pointer. 0x005f2770 is a
    // load-time .data constant whose value is 0x005f2728 and which NOTHING writes at
    // runtime (decomp_pc.py 0x005f2770 --datarefs: "type pointer len 4 value 005f2728",
    // "WRITES: (none)"; second witness, the same dword read out of MASHED.exe.unpatched
    // at file offset 0x1f2770, Ai/AiStandalone.cpp:1388-1390). The standalone does not
    // load the original's .data, so its image-pad owns the RVA zero-filled and
    // CarSlotStateSet (AiStandalone.cpp:1404-1409) early-returns on base == 0. Writing
    // the constant back reproduces a static initializer the binary itself carries -- it
    // is not a bridge and no C-level follows. Predicted, and then measured, behaviourally
    // INERT because the standalone's only consumer of the concept, Ai::Host::veh_type, is
    // the synthesized constant at :93 rather than a read of this table.
    // PREREG: verify/d3_racepos_20261006/PREREG_LEGC_LEGA.md section 3a.
    //
    // [H2 2026-10-08] NOW DEFAULT-ON. verify/d3_gatefire_20261008/{PREREG_H2,RESULT_H2}.md.
    // H2-FAITHFUL: 0x005f2728 is the dword the image itself carries at file offset
    // 0x1f2770 (.data offset 0x008770 < RawSize 0x04d000, so genuinely file-backed) --
    // identical in MASHED.exe and MASHED.exe.unpatched. H2-SCOPE: over 19,418 keys the
    // seed moves exactly 3 of 80 stepdump columns (ss_base/ss_v/ss_raw, its own
    // witnesses) and nothing else. This is a data-initialisation correction, NOT a
    // behavioural improvement -- it is inert on (b), (e) and 77/80 columns. It is worth
    // taking because the value is a precondition later ports will READ, and leaving the
    // port on 0 where the original reads 0x005f2728 is a latent wrong answer.
    // MASHED_NO_SLOTSTATE_SEED restores the old zero-filled behaviour exactly (H2-OPTOUT),
    // following the house convention for default-ON features (MASHED_NO_FOG etc.).
    // MASHED_SLOTSTATE_SEED is retained as a no-op so existing drivers keep working.
    {
        static const bool s_noSlotStateSeed =
            (std::getenv("MASHED_NO_SLOTSTATE_SEED") != nullptr);
        if (!s_noSlotStateSeed) Ai::I32(0x005f2770u) = 0x005f2728;
    }
    // [GATEFIRE 2026-10-08] The slot-STATE cells themselves. Seeding the pointer
    // above only makes the table addressable; nothing fills it for the PLAYER.
    //
    // THIS IS A BRIDGE, EXPLICITLY, AND NOT A PORT OF CONTROL FLOW. The original's
    // writer is FUN_0042b960 (single-player) / FUN_0042b9e0 (multi-player), and BOTH
    // live inside FUN_0043dfd0 — a MENU MESSAGE HANDLER, sole caller FUN_00492d30,
    // dispatching on codes like 0xff1d0000. The standalone never runs that menu, so
    // there is no faithful site for these stores; they are placed here because this
    // block is already the port's stand-in for that frontend initialisation (it
    // writes the slot-INDEX table at :371 and seeds the pointer just above).
    // Evidence + the placement argument: verify/d3_gatefire_20261008/
    // RESULT_SLOTPLACE.md; the writer itself: RESULT_SLOTWRITER.md.
    //
    // WHICH VARIANT, settled by measurement not choice: o_e470.csv read the ORIGINAL
    // at {v0: 1, v1/v2/v3: 2} on 581/672 samples, which is FUN_0042b960's
    // (0,1)/(1,0)/(2,0)/(3,0) followed by FUN_00418860's alive-AI -> 2
    // (AiStandalone.cpp:1759-1761, already ported). The multi-player path would have
    // put 1 on two or more slots. So the single-player body is the right model.
    //
    // WHY IT MATTERS: FUN_004148b0:104 is `if (E470(i) == 1) last = i`, and without
    // the player's 1 NO car ever matches, so `last` stays -1 and branch 2 dies at
    // exit site 105 on every row (RESULT_GF1.md s3, RESULT_E470.md).
    //
    // NOT reproduced here, and deliberately: FUN_0042b960 also sets DAT_007f1a0c = 1
    // and writes the index table as [playerBlockIdx, -1, -1, -1] where :371 above
    // writes identity for all four. Both are pre-existing divergences with their own
    // rows; widening this knob to cover them would mix two changes in one gate.
    //
    // [H4 2026-10-08] NOW DEFAULT-ON. verify/d3_gatefire_20261008/{PREREG_H4,RESULT_H4}.md.
    // H4-C0-INERT: player_trace.log (car 0) and the stepdump (cars 1..3) are
    // byte-identical with and without these stores over 14,400 deterministic frames;
    // only the gates dump's slot_state for v0 moves (0 -> 1). The values reproduce the
    // original's measured {v0:1, v1-3:2} (RESULT_E470.md, 581/672). Still a bridge at an
    // unfaithful site, as stated above; taken because it is inert today and is the cell
    // FUN_004148b0:104 reads. MASHED_NO_SLOT_PLAYER restores the old default exactly;
    // MASHED_SLOT_PLAYER is retained as a no-op so existing drivers keep working.
    // MASHED_WIRE_B2 stays OFF: its other prerequisites (A364_RESET, REFDIST,
    // RACEPCT_BRIDGE, and the harness override NO_ELIM) are all default-OFF.
    {
        static const bool s_slotPlayer = (std::getenv("MASHED_NO_SLOT_PLAYER") == nullptr);
        if (s_slotPlayer) {
            Ai::Ai_SetCarSlotState(0, 1);   // FUN_0040e480(0,1) — the human slot
            Ai::Ai_SetCarSlotState(1, 0);   // FUN_0040e480(1,0)
            Ai::Ai_SetCarSlotState(2, 0);   // FUN_0040e480(2,0)
            Ai::Ai_SetCarSlotState(3, 0);   // FUN_0040e480(3,0)
        }
    }
    // per-vehicle behaviour record -> race line (type 0, index 0).
    for (int v = 0; v < 4; ++v) {
        Ai::I32(Ai::kAiLineType    + static_cast<std::uintptr_t>(v) * Ai::kAiStateDwords * 4u) = 0;
        Ai::I32(Ai::kAiSplineIndex + static_cast<std::uintptr_t>(v) * Ai::kAiStateDwords * 4u) = 0;
    }
    Ai::Host h = {
        aib_game_sub_mode, aib_round_type, aib_game_mode_fd0, aib_track_index,
        aib_alive, aib_veh_type, aib_ai_target_enable, aib_own_xz, aib_own_vel_xz,
        aib_los_clear, aib_own_fwd_xz, aib_veh_f32, aib_held_powerup,
    };
    // [D3 2026-09-26] FUN_00413fe0 race reset, and DAT_0089a360 = 2.5: the value
    // measured on every AI step of the original race (o_inputs, diff_a360 column);
    // FUN_004177b0 truncates it to the difficulty-table row. Its frontend writer is not
    // traced ([UNCERTAIN] U-D3-DIFF360: next, reference_to 0x0089a360 WRITE sites).
    Ai::Ai_ResetRace();
    Ai::F32(0x0089a360u) = 2.5f;
    // [D-11073 leg 2] MASHED_SUBSTATE only: race setup's FUN_004425d0 (called from 0x0040ff31,
    // zeroes the camera-entry table and the FUN_00442600 guard) and the state-1 spawn's
    // DAT_005f29b8 = 100000 (0x004100d3), which sends round 1's state-2 tail into phase 3.
    if (SubState_Enabled()) {
        UtilZeroTable8964c0();
        *reinterpret_cast<int*>(0x005f29b8u) = 100000;
    }
    // [GATEFIRE idx364 2026-10-08] FUN_00414060 step 6, default-OFF behind
    // MASHED_A364_RESET (Race/AiDifficultyReset.cpp). This is the SAME function
    // whose step 5 produces the 2.5 seeded on the line above — the port now owns
    // its input-free tail (the 0x0089a364 = -1 sentinel) while steps 1-5 stay
    // blocked on unproduced globals (RESULT_A360.md). Placed here because the
    // original's caller FUN_004111c0 runs it at race reset, which is this path.
    RaceAiDifficultyResetTail();
    // [D-11072 P2] FUN_00407a60, which the original's FUN_004111c0 runs at race reset (MASHED_PATHPROG).
    Race::PathProg_RaceInit();
    // [mode 7 M1] FUN_00484c90, which FUN_004111c0 runs at race reset (MASHED_REGISTRY).
    Race::Registry_RaceInit();
    Ai::Ai_SetHost(&h);
    return true;
}

// G3: the internal-velocity -> world-position scale (shared with the player path's
// [U-A8-WORLDVEL] kWorldVel) so AI opponents driven through the real physics chain
// cover ground at the same calibrated pace. Env MASHED_WORLDVEL, default 0.22.
float AiPhysWorldVel() {
    static const float v = [] {
        const char* e = std::getenv("MASHED_WORLDVEL");
        float f = e ? (float)std::atof(e) : 0.22f;
        return (f > 0.f) ? f : 0.22f;
    }();
    return v;
}

// WS-E s2 — RenderWare atomic lighting (props + cars), baked into vertex diffuse.
// The standalone's track path is unlit fixed-function (D3DRS_LIGHTING FALSE; the
// per-vertex colour modulates the texture), so RW's per-vertex CPU lighting is
// reproduced here at build time instead of via D3D T&L. Model (RW _rpWorldLight /
// atomic instance, matching the asset flags confirmed from the DFFs):
//   * geometry with rpGEOMETRYLIGHT(0x20) + per-vertex NORMALS  ("lit"):
//        colour = prelight(or 0) + ambient + sunColour * max(0, N·L)
//     (surface ambient/diffuse coefficients are 1 for the shipped materials, so
//      they drop out; one directional light is present per track LIGHTS.DFF).
//   * MODULATEMATERIALCOLOR(0x40): the lit colour is then multiplied by the
//        material's own RGBA (car-livery panels, geo flags 0x73).
//   * prelit, non-lit geometry (car glass; no normals): colour = prelight (+the
//        track ambient, matching the world/prop prelit path) — no directional.
// ambient/sun are 0x00RRGGBB (0 = none); sunL is the unit direction TO the light
// (= -lightDir). N·L is one-sided (CULLMODE is NONE so back faces self-shadow,
// matching RW's single-sided lighting of a closed hull).
struct AtomicLight {
    float amb[3]  = {0, 0, 0};   // ambient colour, 0..1
    float sun[3]  = {0, 0, 0};   // directional colour, 0..1
    float L[3]    = {0, 0, 0};   // unit direction TO the light
    bool  has_sun = false;       // a directional light exists
};

inline AtomicLight MakeAtomicLight(D3DCOLOR ambient, D3DCOLOR sun_color,
                                   const float sun_dir[3]) {
    AtomicLight lt;
    lt.amb[0] = ((ambient >> 16) & 0xFF) / 255.f;
    lt.amb[1] = ((ambient >>  8) & 0xFF) / 255.f;
    lt.amb[2] = ( ambient        & 0xFF) / 255.f;
    if (sun_color) {
        lt.sun[0] = ((sun_color >> 16) & 0xFF) / 255.f;
        lt.sun[1] = ((sun_color >>  8) & 0xFF) / 255.f;
        lt.sun[2] = ( sun_color        & 0xFF) / 255.f;
        // L = -lightDir (direction the light travels), then normalise.
        float lx = -sun_dir[0], ly = -sun_dir[1], lz = -sun_dir[2];
        const float ll = std::sqrt(lx*lx + ly*ly + lz*lz);
        if (ll > 1e-6f) { lx/=ll; ly/=ll; lz/=ll; lt.has_sun = true; }
        lt.L[0] = lx; lt.L[1] = ly; lt.L[2] = lz;
    }
    return lt;
}

// Compute one atomic vertex's D3D diffuse. prelit = RGBA dword (RW order) or
// nullptr; n = world-space normal (x,y,z) or nullptr; mat = material RGBA bytes.
inline D3DCOLOR LightAtomicVertex(const AtomicLight& lt, bool lit, bool modmat,
                                  const std::uint8_t mat[4],
                                  const std::uint32_t* prelit, const float* n) {
    std::uint8_t a = 255;
    float r = 0.f, g = 0.f, b = 0.f;
    if (prelit) {
        const std::uint32_t p = *prelit;            // RW byte order: R,G,B,A
        r = ( p        & 0xFF) / 255.f;
        g = ((p >>  8) & 0xFF) / 255.f;
        b = ((p >> 16) & 0xFF) / 255.f;
        a = static_cast<std::uint8_t>(p >> 24);
    }
    if (lit && n) {
        r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];
        if (lt.has_sun) {
            float nx = n[0], ny = n[1], nz = n[2];
            const float n2 = nx*nx + ny*ny + nz*nz;
            if (n2 > 1e-12f) { const float inv = 1.f/std::sqrt(n2);
                               nx*=inv; ny*=inv; nz*=inv; }
            float ndl = nx*lt.L[0] + ny*lt.L[1] + nz*lt.L[2];
            if (ndl < 0.f) ndl = 0.f;
            r += lt.sun[0]*ndl; g += lt.sun[1]*ndl; b += lt.sun[2]*ndl;
        }
    } else if (!prelit) {
        r = g = b = 1.f;                            // untextured/unlit fallback
    } else {
        // prelit, non-lit (e.g. glass / sea): keep the track ambient as a fill so
        // the prop sits at the same exposure as the lit geometry. WS-E s4 NOTE:
        // strictly per RW a non-lit prelit atomic gets NO runtime ambient (sea.dff
        // GEOM flags 0x0001000f have no rpGEOMETRYLIGHT 0x20), but zeroing it here
        // darkens the big Arctic sea clump to near-black (whole-frame lum ~24 in
        // the sea-heavy chase view) — further from the original's exposure than
        // keeping the fill. The dominant teal cast lived in the WORLD path (fixed
        // above); fog was measured negligible here, so the fill stays.
        // [TERRAIN-DIAG mode 4] MASHED_TERRAIN_NOLIGHT=4 drops the fill HERE ONLY,
        // i.e. scoped to non-lit prelit atomics, leaving the lit branch above
        // untouched. Modes 1/2 zero amb_f_ at load time and therefore hit BOTH
        // branches. Diagnostic only; default behaviour unchanged.
        //
        // RESULT, MEASURED 2026-08-30 -- THIS IS NOT THE SITE. Mode 4 on TRAINING
        // at the parity pose gives mean 18.47 / 42.35%, BIT-IDENTICAL to base.
        // The race terrain never reaches this function on the default path:
        // librw is the default renderer (RwRaceSubmit.cpp:142) and lights
        // IN-SHADER -- see the note at :1428, "the lit batches librw cannot dump
        // (they carry no uploaded vertex colour -- lit in-shader)".
        // [CORRECTED 2026-08-30, branch race/geomlight] The terrain over-brightness
        // is NOT the librw AMBIENT light (RwRaceSubmit.cpp:555): that light only
        // reaches rw::Geometry::LIGHT-flagged geometry, and TRAINING's ground
        // ROAD.DFF carries flags 0x2008b -- no rpGEOMETRYLIGHT -- so BuildClump
        // never sets LIGHT for it (RwSceneBuild.cpp:468) and the light cannot touch
        // it. The real site is the MANUAL prelit fold in BuildClump
        // (RwSceneBuild.cpp:479), which adds amb_world_ into non-lit prelit vertex
        // colours -- the librw twin of the :252 fill just below. PROVEN: skipping
        // the fold while leaving the :555 light active drops TRAINING to the
        // ambient-starve ceiling (18.47->15.45), so the light contributes nothing
        // here. See re/analysis/race_terrain_ambient_20260830.md CORRECTION.
        // Consequence: the "zeroing it darkens the Arctic sea / cars go near-black"
        // objections were measured against a GLOBAL amb_f_ zero; the scoped fold
        // fix leaves cars (lit) alone but the Arctic sea (also non-lit prelit,
        // 0x1000f) does darken and is not yet confirmed against an original.
        static const bool s_scoped_nofill = [] {
            const char* e = std::getenv("MASHED_TERRAIN_NOLIGHT");
            return e && e[0] == '4';
        }();
        if (!s_scoped_nofill) {
            r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];
        }
    }
    if (modmat) {
        r *= mat[0]/255.f; g *= mat[1]/255.f; b *= mat[2]/255.f;
    }
    auto cl = [](float f) -> std::uint8_t {
        int v = static_cast<int>(f*255.f + 0.5f);
        return static_cast<std::uint8_t>(v < 0 ? 0 : v > 255 ? 255 : v);
    };
    return D3DCOLOR_ARGB(a, cl(r), cl(g), cl(b));
}

// WS-E vehicle lighting (RpLight subset). Toggle: MASHED_RPLIGHT (default ON;
// =0 reverts to the legacy load-time model-space bake + quantized colours).
bool RpLightEnabled() {
    static const bool v = [] {
        const char* e = std::getenv("MASHED_RPLIGHT");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    return v;
}

// Float-precision AtomicLight (RpLight colours are RwRGBAReal floats; the
// legacy MakeAtomicLight quantizes them through D3DCOLOR first). `L` is the
// precomputed unit world direction TO the light (= -lightDir).
inline AtomicLight MakeAtomicLightF(const float amb[3], const float sun[3],
                                    bool has_sun, const float L[3]) {
    AtomicLight lt;
    lt.amb[0] = amb[0]; lt.amb[1] = amb[1]; lt.amb[2] = amb[2];
    lt.sun[0] = sun[0]; lt.sun[1] = sun[1]; lt.sun[2] = sun[2];
    lt.L[0] = L[0]; lt.L[1] = L[1]; lt.L[2] = L[2];
    lt.has_sun = has_sun &&
        (L[0]*L[0] + L[1]*L[1] + L[2]*L[2]) > 0.5f;   // unit or zero
    return lt;
}

// Expand a Txd::Texture base mip (PAL4 / PAL8 / ARGB8888) into a fresh
// A8R8G8B8 D3D9 texture. TXD palettes are RGBA byte order (per TxdDecoder's
// PAL8 path in QuadRenderer); D3D wants BGRA dwords.
IDirect3DTexture9* MakeTexture(IDirect3DDevice9* dev,
                               const mashed_re::Txd::Texture& tex) {
    if (tex.mip_count == 0) return nullptr;
    const std::uint32_t w0 = tex.mips[0].width, h0 = tex.mips[0].height;
    if (w0 == 0 || h0 == 0) return nullptr;

    // Decode level 0 to a linear ARGB8888 buffer.
    std::vector<std::uint32_t> lvl0(static_cast<std::size_t>(w0) * h0);
    {
        const auto& mip = tex.mips[0];
        for (std::uint32_t y = 0; y < h0; ++y) {
            std::uint32_t* dst = &lvl0[static_cast<std::size_t>(y) * w0];
            const std::uint8_t* row = mip.pixels + static_cast<std::size_t>(y) * mip.stride;
            for (std::uint32_t x = 0; x < w0; ++x) {
                std::uint8_t r, g, b, a;
                if (mip.depth == 32) {
                    r = row[x * 4 + 0]; g = row[x * 4 + 1];
                    b = row[x * 4 + 2]; a = row[x * 4 + 3];
                } else {
                    // PAL4 and PAL8 are BOTH stored one byte per pixel; `depth`
                    // selects the PALETTE SIZE (16 vs 256 entries), not storage
                    // width. Census 2026-07-31 (5194 mips): stride == max(4,
                    // width*bpp), bpp==1 for depth 4 AND 8, no depth-4 byte > 0x0F.
                    const std::uint32_t idx =
                        (mip.depth == 4) ? (row[x] & 0x0Fu) : row[x];
                    const std::uint8_t* pe = mip.palette + idx * 4;
                    r = pe[0]; g = pe[1]; b = pe[2]; a = pe[3];
                }
                dst[x] = (static_cast<std::uint32_t>(a) << 24) |
                         (static_cast<std::uint32_t>(r) << 16) |
                         (static_cast<std::uint32_t>(g) << 8) | b;
            }
        }
    }

    // MIP-FIX: the shipped TXDs store a SINGLE level per track texture
    // (mip_count==1). The original RW D3D9 pipeline auto-generates the mip chain
    // at load, so minified track/terrain surfaces trilinear-filter (smooth,
    // slightly darker) instead of point-sampling mip 0 (sharp / bright / grainy).
    // Software-generate a box-filtered chain from level 0 down to 1x1 and upload
    // all levels; the sampler adds D3DSAMP_MIPFILTER=LINEAR.
    std::uint32_t nlevels = 1;
    for (std::uint32_t d = (w0 > h0 ? w0 : h0); d > 1; d >>= 1) ++nlevels;

    IDirect3DTexture9* out = nullptr;
    if (FAILED(dev->CreateTexture(w0, h0, nlevels, 0, D3DFMT_A8R8G8B8,
                                  D3DPOOL_MANAGED, &out, nullptr)))
        return nullptr;
    mashed_re::DrawRec::NameTexture(out, tex.name);   // d1r1 recorder; no-op by default

    std::vector<std::uint32_t> cur = std::move(lvl0);
    std::uint32_t cw = w0, ch = h0;
    for (std::uint32_t lvl = 0; lvl < nlevels; ++lvl) {
        D3DLOCKED_RECT lr;
        if (FAILED(out->LockRect(lvl, &lr, nullptr, 0))) { out->Release(); return nullptr; }
        for (std::uint32_t y = 0; y < ch; ++y) {
            std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(
                static_cast<std::uint8_t*>(lr.pBits) + static_cast<std::size_t>(y) * lr.Pitch);
            std::memcpy(dst, &cur[static_cast<std::size_t>(y) * cw],
                        static_cast<std::size_t>(cw) * 4);
        }
        out->UnlockRect(lvl);
        if (lvl + 1 == nlevels) break;
        // box-downsample cur -> next (2x2 average; clamp odd dims to >=1).
        const std::uint32_t nw = cw > 1 ? cw >> 1 : 1;
        const std::uint32_t nh = ch > 1 ? ch >> 1 : 1;
        std::vector<std::uint32_t> nxt(static_cast<std::size_t>(nw) * nh);
        for (std::uint32_t y = 0; y < nh; ++y) {
            const std::uint32_t y0 = y * 2, y1 = (y * 2 + 1 < ch) ? y * 2 + 1 : y0;
            for (std::uint32_t x = 0; x < nw; ++x) {
                const std::uint32_t x0 = x * 2, x1 = (x * 2 + 1 < cw) ? x * 2 + 1 : x0;
                const std::uint32_t p00 = cur[static_cast<std::size_t>(y0) * cw + x0];
                const std::uint32_t p01 = cur[static_cast<std::size_t>(y0) * cw + x1];
                const std::uint32_t p10 = cur[static_cast<std::size_t>(y1) * cw + x0];
                const std::uint32_t p11 = cur[static_cast<std::size_t>(y1) * cw + x1];
                std::uint32_t o = 0;
                for (int s = 0; s < 32; s += 8) {
                    const std::uint32_t c =
                        (((p00 >> s) & 0xFF) + ((p01 >> s) & 0xFF) +
                         ((p10 >> s) & 0xFF) + ((p11 >> s) & 0xFF) + 2) >> 2;
                    o |= (c & 0xFF) << s;
                }
                nxt[static_cast<std::size_t>(y) * nw + x] = o;
            }
        }
        cur = std::move(nxt); cw = nw; ch = nh;
    }
    return out;
}

// Build per-material vertex batches (+ textures) from a parsed DffModel,
// resolving texture names across the given TXD dictionaries. Used by the sky dome
// the one model still drawn through D3D9 (the copters moved to librw in librwrest).
//
// WS-E s2 lighting: `lt` (or nullptr) is the track's atomic light set (ambient +
// directional sun). When supplied, each vertex's diffuse is computed by the RW
// atomic-lighting model (see LightAtomicVertex): rpGEOMETRYLIGHT batches that
// carry normals get ambient + sun·N·L (×material colour where MODULATEMATERIAL-
// COLOR), prelit batches get prelight (+ambient), others fall back to white.
// nullptr reproduces the legacy unlit behaviour (prelight as-is, else white) —
// used for the sky dome (a prelit camera-locked backdrop, not a lit atomic).
void BuildDffBatches(IDirect3DDevice9* dev,
                     const mashed_re::Track::DffModel& model,
                     const std::vector<mashed_re::Txd::Dictionary>& dicts,
                     std::vector<std::vector<TrackRenderer::V>>* batches,
                     std::vector<IDirect3DTexture9*>* textures,
                     const AtomicLight* lt = nullptr) {
    using V = TrackRenderer::V;
    textures->assign(model.materials.size(), nullptr);
    // P3b diagnostic (MASHED_DBG_TEXMATCH): classify each material's texture
    // resolution as matched / empty-name (colour-only, correctly untextured) /
    // unmatched (a name the dicts don't contain — a real binding bug). The 2026-06-19
    // run of this on the car model answered the question it was built for: 71 materials,
    // 44 matched, 27 colour-only, 0 unmatched — i.e. the untextured batches are
    // by-design, NOT a binding bug. Kept because the same question recurs per-model,
    // and because the earlier "65% vs 97%" framing was a batch-vs-draw denominator
    // artefact that this counter is what disambiguates.
    static const bool s_dbg_tex = std::getenv("MASHED_DBG_TEXMATCH") != nullptr;
    int n_match = 0, n_empty = 0, n_unmatched = 0;
    std::string unmatched_names;
    for (std::size_t mi = 0; mi < model.materials.size(); ++mi) {
        const char* want = model.materials[mi].tex_name;
        if (!want[0]) { ++n_empty; continue; }
        for (std::size_t di = 0; di < dicts.size() && !(*textures)[mi]; ++di)
            for (std::uint32_t ti = 0; ti < dicts[di].count(); ++ti)
                if (std::strcmp(dicts[di].texture(ti).name, want) == 0) {
                    (*textures)[mi] = MakeTexture(dev, dicts[di].texture(ti));
                    break;
                }
        if ((*textures)[mi]) { ++n_match; }
        else { ++n_unmatched; if (unmatched_names.size() < 400) { unmatched_names += want; unmatched_names += ' '; } }
    }
    if (s_dbg_tex) {
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "TEXMATCH: %zu mats match=%d empty=%d unmatched=%d"
                         " dicts=%zu unmatched_names=[%s]\n",
                         model.materials.size(), n_match, n_empty, n_unmatched,
                         dicts.size(), unmatched_names.c_str());
            // also list the available dict texture names once, to spot case/format diffs
            for (std::size_t di = 0; di < dicts.size(); ++di) {
                std::fprintf(lf, "  dict[%zu] (%u texs):", di, dicts[di].count());
                for (std::uint32_t ti = 0; ti < dicts[di].count() && ti < 24; ++ti)
                    std::fprintf(lf, " %s", dicts[di].texture(ti).name);
                std::fputc('\n', lf);
            }
            std::fclose(lf);
        }
    }
    batches->assign(model.materials.size(), {});
    for (const auto& b : model.batches) {
        auto& out = (*batches)[b.material];
        const std::uint8_t* matRGBA = model.materials[b.material].rgba;
        const bool has_n  = !b.normals.empty();
        const std::size_t n = b.tris.size();
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint16_t vi = b.tris[i];
            V v;
            v.x = b.verts[vi * 3 + 0];
            v.y = b.verts[vi * 3 + 1];
            v.z = b.verts[vi * 3 + 2];
            const std::uint32_t* pl = b.prelit.empty() ? nullptr : &b.prelit[vi];
            if (lt) {
                const float* nrm = has_n ? &b.normals[vi * 3] : nullptr;
                v.c = LightAtomicVertex(*lt, b.lit, b.modulate_mat, matRGBA,
                                        pl, nrm);
            } else if (pl) {
                const std::uint32_t p = *pl;     // legacy: prelight as-is
                v.c = D3DCOLOR_ARGB(static_cast<std::uint8_t>(p >> 24),
                                    static_cast<std::uint8_t>(p),
                                    static_cast<std::uint8_t>(p >> 8),
                                    static_cast<std::uint8_t>(p >> 16));
            } else {
                v.c = 0xFFFFFFFFu;
            }
            v.u = b.uvs[vi * 2 + 0];
            v.v = b.uvs[vi * 2 + 1];
            out.push_back(v);
        }
    }
}

// WS-E lighting: parse a track's LIGHTS.DFF (COURSE.LUA Lights_Filename) for
// its ambient term. LIGHTS.DFF is a standard RW clump (rwID_CLUMP 0x10) whose
// only payload is RpLight chunks (rwID_LIGHT 0x12); each light's STRUCT body is
// { f32 radius; f32 r,g,b; f32 minusCosAngle; u32 type_flags } (type =
// type_flags>>16: 1=DIRECTIONAL 2=AMBIENT; flags = type_flags&0xffff, bit 0x2 =
// rpLIGHTLIGHTWORLD). Arctic LIGHTS.DFF (verified): AMBIENT (0.2,0.3,0.3) +
// DIRECTIONAL (0.6,0.7,0.7) both flags=0x3. The WORLD BSP has NO vertex normals
// (format 0x4001004d — TRISTRIP|TEXTURED|PRELIT|MODULATEMATERIALCOLOR, no 0x10
// NORMALS, no 0x20 LIGHT), so RW's directional term cannot apply to it per-
// vertex; only the directionless AMBIENT brightens the world. Returns the summed
// AMBIENT colour (rpLIGHTLIGHTWORLD lights only) as 0x00RRGGBB; 0 if none.
D3DCOLOR ParseLightsDffAmbient(const std::uint8_t* d, std::uint32_t len) {
    if (!d || len < 24) return 0;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return 0;                 // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    float ar = 0.f, ag = 0.f, ab = 0.f;
    // CLUMP's direct children: LIGHT chunks are siblings of FRAMELIST/GEOMLIST.
    for (std::size_t o = 12; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {  // LIGHT->STRUCT
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t tf = u32(sb + 20);
                const std::uint32_t type = tf >> 16, flags = tf & 0xffff;
                if (type == 2 && (flags & 0x2)) { ar += r; ag += g; ab += b; }
            }
        }
        o = body + sz;
    }
    auto cl = [](float f) -> int {
        int v = static_cast<int>(f * 255.f + 0.5f);
        return v < 0 ? 0 : v > 255 ? 255 : v;
    };
    return D3DCOLOR_XRGB(cl(ar), cl(ag), cl(ab));
}

// WS-E s2: parse the track's DIRECTIONAL RpLight (type-1) from LIGHTS.DFF — its
// colour AND world-space direction. Unlike ambient, a directional light's
// direction is NOT in the light STRUCT; it is the light frame's at-vector. Layout
// (asset-verified, arctic LIGHTS.DFF): CLUMP -> STRUCT{numAtomics,numLights,
// numCameras} -> FRAMELIST{ STRUCT{int n; then n*(9f rot, 3f pos, i32 parent,
// u32 flags)=0x38 each} } -> GEOMETRYLIST -> (per light) STRUCT{i32 frameIdx} +
// LIGHT{ STRUCT{f32 radius; 3f rgb; f32 minusCosAngle; u32 type_flags} }. We pair
// each LIGHT with the preceding size-4 STRUCT (its frame index), and for a
// DIRECTIONAL light that lights atomics (flags bit rpLIGHTLIGHTATOMICS 0x1) we
// return its colour (0x00RRGGBB) and the frame's parent-chain-composed at-vector.
// Returns false if there is no such light. Arctic -> colour (153,178,178),
// dir (0.577,-0.577,-0.577).
// E2'b step 3: out_color retyped D3DCOLOR* -> std::uint32_t* to match sun_color_
// after its move into RaceSceneState. D3DCOLOR is `unsigned long` and uint32_t is
// `unsigned int` -- distinct types, so the VALUE sites converted silently but this
// POINTER site did not, which is what caught it at compile time.
bool ParseLightsDffDirectional(const std::uint8_t* d, std::uint32_t len,
                               std::uint32_t* out_color, float out_dir[3]) {
    if (!d || len < 24) return false;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto i32 = [&](std::size_t o) -> std::int32_t {
        std::int32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return false;                  // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    // CLUMP STRUCT
    if (12 + 12 > end || u32(12) != 0x01) return false;
    std::size_t o = 12 + 12 + u32(16);                 // past the clump struct
    // FRAMELIST
    struct Fr { float rot[9]; std::int32_t parent; };
    std::vector<Fr> frames;
    if (o + 12 <= end && u32(o) == 0x0E) {
        const std::size_t flb = o + 12, flsz = u32(o + 4);
        if (flb + 12 <= end && u32(flb) == 0x01) {
            const std::size_t fstb = flb + 12;
            const std::int32_t nf = i32(fstb);
            std::size_t q = fstb + 4;
            for (std::int32_t i = 0; i < nf && q + 0x38 <= end; ++i) {
                Fr f;
                for (int k = 0; k < 9; ++k) f.rot[k] = f32(q + k * 4u);
                f.parent = i32(q + 48);
                frames.push_back(f);
                q += 0x38;
            }
        }
        o = flb + flsz;
    }
    // remaining top-level children: pair STRUCT(frameIdx) with the next LIGHT.
    std::int32_t pending_frame = -1;
    for (; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x01 && sz == 4) {
            pending_frame = i32(body);
        } else if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t tf = u32(sb + 20);
                const std::uint32_t type = tf >> 16, flags = tf & 0xffff;
                if (type == 1 && (flags & 0x1) && pending_frame >= 0 &&
                    pending_frame < static_cast<std::int32_t>(frames.size())) {
                    // world at-vector = parent-chain rotation of local at
                    // (cols 6,7,8 of the frame's 3x3).
                    float v[3] = {frames[pending_frame].rot[6],
                                  frames[pending_frame].rot[7],
                                  frames[pending_frame].rot[8]};
                    // rot[6..8] is the frame's own at-axis ALREADY expressed in
                    // its parent's space (RW row-vector: at_world =
                    // (0,0,1)*M_self*M_parent... = row2(M_self)*M_parent...), so
                    // the parent-chain walk must START AT THE PARENT. Starting at
                    // pending_frame applied M_self twice and mis-aimed the sun on
                    // 13/13 tracks -- TRAINING L.y +0.352 -> +0.030.
                    // re/analysis/CAR_BRIGHTNESS_2026-09-30.md
                    for (int fi = frames[pending_frame].parent; fi >= 0; ) {
                        const float* m = frames[fi].rot;
                        const float x = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];
                        const float y = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];
                        const float z = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];
                        const std::int32_t pa = frames[fi].parent;
                        if (pa == fi) break;            // guard
                        if (pa < 0) { v[0]=x; v[1]=y; v[2]=z; break; }
                        v[0]=x; v[1]=y; v[2]=z; fi = pa;
                    }
                    const float l = std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
                    if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
                    auto cl = [](float f) -> int {
                        int x = static_cast<int>(f * 255.f + 0.5f);
                        return x < 0 ? 0 : x > 255 ? 255 : x;
                    };
                    if (out_color) *out_color = D3DCOLOR_XRGB(cl(r), cl(g), cl(b));
                    if (out_dir) { out_dir[0]=v[0]; out_dir[1]=v[1]; out_dir[2]=v[2]; }
                    return true;
                }
            }
            pending_frame = -1;
        }
        o = body + sz;
    }
    return false;
}

// WS-E vehicle lighting: faithful FUN_00479330 (0x00479330) DFF-branch light
// extraction. The original collects EVERY clump light (RpClumpForAllLights
// via FUN_004b4010; callback LAB_004b3f20 is 13 instructions with NO filter —
// code units read 2026-07-02, Mashed_pool14), keys on the light's subtype
// byte alone (light+0x01: 1=DIRECTIONAL, 2=AMBIENT — three in-function
// witnesses, see re/prior_art/notes/rw_lighting_research_2026-07.md §8.1),
// applies NO stream-flag test, and stores last-wins per subtype (plain scalar
// stores into course+0x105e0/+0x105e4). In the stream the subtype is
// typeAndFlags>>16. Directional direction = the light frame's parent-chain
// at-vector (same layout walk as ParseLightsDffDirectional above). Colours
// returned as floats (RwRGBAReal precision — the original never quantizes).
void ParseLightsDffFaithful(const std::uint8_t* d, std::uint32_t len,
                            float out_amb[3], bool* has_amb,
                            float out_sun[3], float out_dir[3],
                            bool* has_sun) {
    *has_amb = false;
    *has_sun = false;
    if (!d || len < 24) return;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto i32 = [&](std::size_t o) -> std::int32_t {
        std::int32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return;                        // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    if (12 + 12 > end || u32(12) != 0x01) return;      // CLUMP STRUCT
    std::size_t o = 12 + 12 + u32(16);
    struct Fr { float rot[9]; std::int32_t parent; };
    std::vector<Fr> frames;
    if (o + 12 <= end && u32(o) == 0x0E) {             // FRAMELIST
        const std::size_t flb = o + 12, flsz = u32(o + 4);
        if (flb + 12 <= end && u32(flb) == 0x01) {
            const std::size_t fstb = flb + 12;
            const std::int32_t nf = i32(fstb);
            std::size_t q = fstb + 4;
            for (std::int32_t i = 0; i < nf && q + 0x38 <= end; ++i) {
                Fr f;
                for (int k = 0; k < 9; ++k) f.rot[k] = f32(q + k * 4u);
                f.parent = i32(q + 48);
                frames.push_back(f);
                q += 0x38;
            }
        }
        o = flb + flsz;
    }
    std::int32_t pending_frame = -1;
    for (; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x01 && sz == 4) {
            pending_frame = i32(body);
        } else if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t type = u32(sb + 20) >> 16;
                if (type == 2) {                       // AMBIENT — last wins
                    out_amb[0] = r; out_amb[1] = g; out_amb[2] = b;
                    *has_amb = true;
                } else if (type == 1) {                // DIRECTIONAL — last wins
                    out_sun[0] = r; out_sun[1] = g; out_sun[2] = b;
                    // frame's world at-vector (parent-chain composed); a
                    // frameless stream light keeps the previous/zero dir —
                    // shipped LIGHTS.DFF directionals all carry a frame.
                    if (pending_frame >= 0 &&
                        pending_frame < static_cast<std::int32_t>(frames.size())) {
                        float v[3] = {frames[pending_frame].rot[6],
                                      frames[pending_frame].rot[7],
                                      frames[pending_frame].rot[8]};
                        // Parent-start, for the same reason as the twin in
                        // ParseLightsDffDirectional above: rot[6..8] is already
                        // in the parent's space, so seeding the walk at
                        // pending_frame composed M_self a second time.
                        // re/analysis/CAR_BRIGHTNESS_2026-09-30.md
                        for (int fi = frames[pending_frame].parent; fi >= 0; ) {
                            const float* m = frames[fi].rot;
                            const float x = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];
                            const float y = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];
                            const float z = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];
                            const std::int32_t pa = frames[fi].parent;
                            if (pa == fi) break;        // guard
                            if (pa < 0) { v[0]=x; v[1]=y; v[2]=z; break; }
                            v[0]=x; v[1]=y; v[2]=z; fi = pa;
                        }
                        const float l = std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
                        if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
                        out_dir[0]=v[0]; out_dir[1]=v[1]; out_dir[2]=v[2];
                    }
                    *has_sun = true;
                }
            }
            pending_frame = -1;
        }
        o = body + sz;
    }
}

void MatIdentity(D3DMATRIX* m) {
    std::memset(m, 0, sizeof(*m));
    m->_11 = m->_22 = m->_33 = m->_44 = 1.f;
}

// out = a * b (row-vector convention: v' = v * a * b)
void MatMul(D3DMATRIX* out, const D3DMATRIX& a, const D3DMATRIX& b) {
    D3DMATRIX r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] +
                        a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
    *out = r;
}

void MatLookAtLH(D3DMATRIX* m, const float eye[3], const float at[3]) {
    float z[3] = {at[0] - eye[0], at[1] - eye[1], at[2] - eye[2]};
    const float zl = std::sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
    z[0] /= zl; z[1] /= zl; z[2] /= zl;
    const float up[3] = {0.f, 1.f, 0.f};
    float x[3] = {up[1]*z[2] - up[2]*z[1], up[2]*z[0] - up[0]*z[2],
                  up[0]*z[1] - up[1]*z[0]};
    const float xl = std::sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
    x[0] /= xl; x[1] /= xl; x[2] /= xl;
    const float y[3] = {z[1]*x[2] - z[2]*x[1], z[2]*x[0] - z[0]*x[2],
                        z[0]*x[1] - z[1]*x[0]};
    // MIRROR FIX (2026-08-16). RenderWare's camera space and D3D's disagree on
    // which way the right axis points on screen, and the standalone had been
    // rendering the world REFLECTED relative to the original because of it --
    // self-consistently, which is why it survived this long unnoticed.
    //
    // Measured: transplanting the original's own RwCamera basis verbatim gives a
    // mirror image (89.68% vs the original capture); negating the right axis
    // gives 33.79%, and beats a post-hoc pixel flip of the same render (44.38%)
    // because it fixes occlusion rather than just moving pixels. It also
    // retro-explains the ~90px car-projection offset previously blamed on
    // capture timing. Evidence: verify/d1_basis/RESULT.md.
    //
    // Applied to the axis, not to the projection's _11, deliberately: librw
    // builds its own projection from the published fov/aspect/near/far, so
    // negating there would flip only the D3D9 path and silently break the
    // renderer A/B. Negating here keeps the change in the view basis.
    // y is computed from the ORIGINAL x above, so the negation happens after.
    x[0] = -x[0]; x[1] = -x[1]; x[2] = -x[2];
    MatIdentity(m);
    m->_11 = x[0]; m->_12 = y[0]; m->_13 = z[0];
    m->_21 = x[1]; m->_22 = y[1]; m->_23 = z[1];
    m->_31 = x[2]; m->_32 = y[2]; m->_33 = z[2];
    m->_41 = -(x[0]*eye[0] + x[1]*eye[1] + x[2]*eye[2]);
    m->_42 = -(y[0]*eye[0] + y[1]*eye[1] + y[2]*eye[2]);
    m->_43 = -(z[0]*eye[0] + z[1]*eye[1] + z[2]*eye[2]);
}

// Same view matrix, but from an EXPLICIT orthonormal basis instead of a
// look-at pair. MatLookAtLH above hardcodes up = world +Y, which silently
// forces roll to zero; the original's race camera carries a real roll (measured
// ~26 deg, RwCamera frame right.y = 0.440, verify/d1_carproj/RESULT.md), so a
// look-at pair CANNOT reproduce its view and any transplant through one is
// wrong about the horizon by construction. Rows are the basis, translation is
// -dot(axis, pos) -- identical convention to MatLookAtLH, just without
// re-deriving the axes from a target point.
void MatViewFromBasis(D3DMATRIX* m, const float pos[3], const float right[3],
                      const float up[3], const float at[3]) {
    // Same right-axis negation as MatLookAtLH -- see the block there. The basis
    // passed in is RenderWare's, straight off the RwCamera frame, so it gets the
    // identical treatment and a transplanted pose needs NO pre-negation.
    const float r[3] = { -right[0], -right[1], -right[2] };
    MatIdentity(m);
    m->_11 = r[0]; m->_12 = up[0]; m->_13 = at[0];
    m->_21 = r[1]; m->_22 = up[1]; m->_23 = at[1];
    m->_31 = r[2]; m->_32 = up[2]; m->_33 = at[2];
    m->_41 = -(r[0]*pos[0]  + r[1]*pos[1]  + r[2]*pos[2]);
    m->_42 = -(up[0]*pos[0] + up[1]*pos[1] + up[2]*pos[2]);
    m->_43 = -(at[0]*pos[0] + at[1]*pos[1] + at[2]*pos[2]);
}

void MatPerspectiveFovLH(D3DMATRIX* m, float fovy, float aspect,
                         float zn, float zf) {
    const float ys = 1.f / std::tan(fovy * 0.5f);
    const float xs = ys / aspect;
    std::memset(m, 0, sizeof(*m));
    m->_11 = xs; m->_22 = ys;
    m->_33 = zf / (zf - zn); m->_34 = 1.f;
    m->_43 = -zn * zf / (zf - zn);
}

// [F2] World matrix from an HAnim sample: unit quaternion (x,y,z,w) -> rotation
// + translation, in the row-vector convention the MTS/prop path already feeds
// to D3DTS_WORLD (matches D3DXMatrixRotationQuaternion).
void QuatPosMatrix(const float q[4], const float p[3], D3DMATRIX* m) {
    const float x = q[0], y = q[1], z = q[2], w = q[3];
    const float xx = x*x, yy = y*y, zz = z*z;
    const float xy = x*y, xz = x*z, yz = y*z, wx = w*x, wy = w*y, wz = w*z;
    m->_11 = 1.f - 2.f*(yy+zz); m->_12 = 2.f*(xy+wz);     m->_13 = 2.f*(xz-wy);     m->_14 = 0.f;
    m->_21 = 2.f*(xy-wz);       m->_22 = 1.f - 2.f*(xx+zz); m->_23 = 2.f*(yz+wx);   m->_24 = 0.f;
    m->_31 = 2.f*(xz+wy);       m->_32 = 2.f*(yz-wx);       m->_33 = 1.f - 2.f*(xx+yy); m->_34 = 0.f;
    m->_41 = p[0];             m->_42 = p[1];             m->_43 = p[2];           m->_44 = 1.f;
}

}  // namespace

// ---- [rprops U-9695] per-track prop placement ---------------------------------------------------
// The ported RW leaves the original's frame ops go through (Math/RwMatrixRotate.cpp,
// Collision/RwpSolverMath2.cpp), the same externs Race/CameraEntries.cpp binds.
extern "C" void* __cdecl RwMatrixRotate(void* m, const float* axis, float deg, int mode);   // 0x004c4d20
extern "C" void  __cdecl FUN_004c51a0(float* m, float* v, int mode);                        // 0x004c51a0 RwMatrixTranslate

namespace {

// Camera-man placement, one record per track whose COURSE.LUA loads CamMan.dff and whose per-frame
// helper places it. `n` = the helper's loop bound (`mov ebp, N`), entries 1..N; `helper` = the helper's
// first byte. The clump each creation helper clones is `course + 0x10110 + idx*4` = the CamMan.dff
// index of that track's COURSE.LUA (read from the bytes by verify/rprops_20261016/find_camman_arrays.py,
// 12 of 12). Each helper has exactly one caller, inside its own record's slot 1 (the per-frame node):
// arctic 0x00448f02 (slot 1 0x00448ef0), rouabout 0x0044d3fc (0x0044d3f0), egypt 0x00449825 (0x00449820),
// sands 0x00450901 (0x004507b0), superg 0x00450f85 (0x00450f80), training 0x004513ee (0x004513b0), warzone 0x00452b2c
// (0x00452b20), forest 0x0044ded6 (0x0044ded0), storm 0x0044da49 (0x0044da40), dump 0x00449515
// (0x00449510), highway 0x0044eebc (0x0044eeb0), neustein 0x0044aeff (0x0044aec0)
// (verify/rprops_20261016/find_calls.py).
struct CamManTrack { int course_id; int n; std::uint32_t helper; };
const CamManTrack kCamMan[] = {
    {  0,  5, 0x00448dc0u },   // arctic    array 0x00896460  clump 7  (+0x1012c)
    {  2,  4, 0x0044cb00u },   // rouabout  array 0x00896230  clump 14 (+0x10148)
    {  3,  6, 0x00449640u },   // egypt     array 0x00896260  clump 2  (+0x10118)
    { 11,  6, 0x004502a0u },   // sands     array 0x0088ffc0  clump 17 (+0x10154)
    { 25,  6, 0x00450ea0u },   // superg    array 0x0088ffa0  clump 2  (+0x10118)
    { 30,  3, 0x00451220u },   // training  array 0x0088ff80  clump 1  (+0x10114)
    { 33,  9, 0x00452790u },   // warzone   array 0x0088ff20  clump 6  (+0x10128)
    { 34,  6, 0x0044de40u },   // forest    array 0x00895960  clump 7  (+0x1012c)
    { 36,  6, 0x0044d8d0u },   // storm     array 0x00895fe0  clump 3  (+0x1011c)
    { 37, 13, 0x004494a0u },   // dump      array 0x00896280  clump 13 (+0x10144)
    { 38,  9, 0x0044e3b0u },   // highway   array 0x00894d60  clump 51 (+0x101dc)
    { 39,  5, 0x0044ac10u },   // neustein  array 0x00896240  clump 7  (+0x1012c)
};

// Arctic ship tables (decomp 0x00448770 / 0x00448880 / 0x00448cb0 / 0x00448e20; floats read from the
// image by verify/rprops_20261016/dump_ship_tables.py).
const float kFreighterPos[2][3] = { { -15.0f, -4.0f, -66.0f }, { -20.5f, -4.0f, 81.0f } };  // 0x005f95d0
const float kFreighterYaw[2]    = { 110.0f, 80.0f };                                       // 0x005f95e8
const float kBoatPos[4][3] = { { 29.195f, -4.0f, 70.0f }, { -4.357f, -4.0f, 54.0f },       // 0x005f95f0
                               { 45.9f, -4.0f, 8.8f },    { 50.0f, -4.0f, 23.05f } };
const float kBoatYaw[4]    = { 120.0f, 60.0f, 0.0f, 30.0f };                               // 0x005f9620
const double kShipRotDeg   = 5.0;    // qword 0x005cd030
const double kFreighterAmp = 0.5;    // qword 0x005ce1d8
const double kFreighterOff = 4.5;    // qword 0x005ce1f0
const double kBoatAmp      = 0.25;   // qword 0x005ce1c8
const double kBoatOff      = 4.0;    // qword 0x005ce1f8
const float  kStepBob      = 0.02f;  // dword 0x005ce18c  -> 0x00683258
const float  kStepRoll     = 0.04f;  // dword 0x005cd18c  -> 0x00683260
const float  kStepPitch    = 0.01f;  // dword 0x005cc328  -> 0x0068325c
const float  kAxisY[3]     = { 0.f, 1.f, 0.f };   // 0x006146fc

void RwMatIdentity16(float m[16]) {            // RwFrameLoadIdentity 0x004c15c0
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = 1.f;
}
void RwMatToD3D(const float m[16], D3DMATRIX* o) {   // RwMatrix rows right/up/at/pos
    o->_11 = m[0];  o->_12 = m[1];  o->_13 = m[2];  o->_14 = 0.f;
    o->_21 = m[4];  o->_22 = m[5];  o->_23 = m[6];  o->_24 = 0.f;
    o->_31 = m[8];  o->_32 = m[9];  o->_33 = m[10]; o->_34 = 0.f;
    o->_41 = m[12]; o->_42 = m[13]; o->_43 = m[14]; o->_44 = 1.f;
}

// A yaw-and-translate frame: load identity, RwFrameRotate(frame, (0,1,0), yaw, REPLACE),
// RwFrameTranslate(frame, pos, POSTCONCAT). It is the camera-man helper loop body (e.g.
// 0x00448dc0..0x00448e1f) and the ships' load-time placement (0x00448770 / 0x00448880).
void PlaceMatrix(const float pos[3], float yaw, D3DMATRIX* out) {
    float m[16]; RwMatIdentity16(m);
    RwMatrixRotate(m, kAxisY, yaw, 0);
    float p[3] = { pos[0], pos[1], pos[2] };
    FUN_004c51a0(m, p, 2);
    RwMatToD3D(m, out);
}

// One ship's frame, the loop body of 0x00448cb0 (freighters) / 0x00448e20 (boats): identity, rotate
// about +Y by the table yaw (REPLACE), roll about the matrix `at` row by cos(acc260)*5 deg, pitch about
// the matrix `right` row by sin(acc25c)*5 deg (both POSTCONCAT, axis = the matrix's own row, so it moves
// with the rotation already applied), then translate by (x, sin(acc258)*amp - off, z) POSTCONCAT.
void ShipMatrix(const float pos[3], float yaw, const float acc[3], double amp, double off, D3DMATRIX* out) {
    float m[16]; RwMatIdentity16(m);
    RwMatrixRotate(m, kAxisY, yaw, 0);                                      // 0x00448cf3 / 0x00448e63
    RwMatrixRotate(m, m + 8, static_cast<float>(std::cos(static_cast<double>(acc[2])) * kShipRotDeg), 2);   // 0x00448d14
    RwMatrixRotate(m, m + 0, static_cast<float>(std::sin(static_cast<double>(acc[1])) * kShipRotDeg), 2);   // 0x00448d35
    float p[3] = { pos[0], static_cast<float>(std::sin(static_cast<double>(acc[0])) * amp - off), pos[2] };  // 0x00448d4b..57
    FUN_004c51a0(m, p, 2);                                                  // 0x00448d5e
    RwMatToD3D(m, out);
}

// Prop kinds for prop_anims_.
enum { kAnimFreighter = 1, kAnimBoat = 2 };

}  // namespace

namespace { void InvalidateBatchCache(); }  // WS-A s3 PERF (defined before Render)

bool TrackRenderer::Load(IDirect3DDevice9* dev, const char* piz_path,
                         const char* log_path) {
    InvalidateBatchCache();   // drop any prior track's cached VBs (reload safety)
    // E2'b step 3: same reload safety on the librw side -- see the header note on
    // why this cannot live at the tail alongside RaceSubmit_OnTrackLoaded.
    LibRw::RaceSubmit_BeginTrackLoad();
    rain_enabled_ = false;    // [rstates] re-read from this track's COURSE.LUA below
    std::FILE* log = log_path ? std::fopen(log_path, "a") : nullptr;
    auto fail = [&](const char* why) {
        if (log) { std::fprintf(log, "R4 track load FAILED: %s\n", why); std::fclose(log); }
        return false;
    };
    // [rtex] the course struct is zeroed per load (0x0047a051) with +0x204 = 0 and
    // +0x208 = 1.0 (0x0047a060 / 0x0047a066) before COURSE.LUA runs
    shine_lo_ = 0.f;
    shine_hi_ = 1.f;

    Piz::Archive piz;
    if (!piz.Load(piz_path)) return fail(piz.last_error());

    // GRAPH*.BSP (name varies: GRAPH.BSP / GRAPHICS.BSP). Textures: decode
    // EVERY *.TXD in the piz (DUMP.TXD + CITY.TXD coexist; props reference
    // secondary dictionaries) and resolve material names across all of them.
    const std::uint8_t* world_blob = nullptr; std::uint32_t world_len = 0;
    std::vector<std::pair<const std::uint8_t*, std::uint32_t>> txds;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        char u[129];
        for (std::size_t k = 0; k <= ln && k < 128; ++k)
            u[k] = static_cast<char>(::toupper(static_cast<unsigned char>(n[k])));
        u[128] = '\0';
        if (std::strncmp(u, "GRAPH", 5) == 0 && ln > 4 &&
            std::strcmp(u + ln - 4, ".BSP") == 0) {
            world_blob = piz.blob(i, &world_len);
        } else if (std::strncmp(u, "COLL", 4) == 0 && ln > 4 &&
                   std::strcmp(u + ln - 4, ".BSP") == 0) {
            // COLLISIONS.BSP / COLLIDE.BSP / COLL.BSP — same world stream;
            // flatten into the raycast triangle soup.
            std::uint32_t cl = 0;
            const std::uint8_t* cb = piz.blob(i, &cl);
            Track::World cw;
            if (cb && cw.Parse(cb, cl)) {
                for (const auto& s : cw.sectors) {
                    const std::uint32_t vbase =
                        static_cast<std::uint32_t>(col_verts_.size() / 3);
                    col_verts_.insert(col_verts_.end(), s.verts.begin(),
                                      s.verts.end());
                    // [rtex] keep the sector prelit for the env-gloss shade sample
                    // (FUN_00479100 reads the hit sector's colours (sector+0x30); with none it
                    // returns 1.0, which CollisionPrelitSample keys on col_tri_pl_)
                    const std::size_t snv = s.verts.size() / 3;
                    if (s.prelit.size() == snv)
                        col_prelit_.insert(col_prelit_.end(), s.prelit.begin(), s.prelit.end());
                    else
                        col_prelit_.insert(col_prelit_.end(), snv, 0u);
                    const std::size_t nt = s.tris.size() / 4;
                    for (std::size_t ti = 0; ti < nt; ++ti) {
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 1]);
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 2]);
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 3]);
                        // element 0 is the 16-bit material index (same index
                        // space the render path uses at :1132/:1581) — keep it
                        // so the wheel contact can reach record +0x1ec.
                        col_mat_.push_back(s.tris[ti * 4 + 0]);
                        col_tri_pl_.push_back(s.prelit.size() == snv ? 1 : 0);   // [rtex]
                    }
                }
            }
        } else if (ln > 4 && std::strcmp(u + ln - 4, ".TXD") == 0) {
            std::uint32_t bl = 0;
            const std::uint8_t* b = piz.blob(i, &bl);
            if (b) txds.emplace_back(b, bl);
        }
    }
    if (!world_blob) return fail("GRAPH*.BSP not found");

    Track::World world;
    if (!world.Parse(world_blob, world_len)) return fail(world.last_error());

    std::vector<Txd::Dictionary> dicts(txds.size());
    for (std::size_t di = 0; di < txds.size(); ++di)
        dicts[di].Decode(txds[di].first, txds[di].second);  // failures = count 0

    // ---- F3 .UVA UV-anim: bind each track .UVA (rwID_UVANIMDICT) entry's
    // scroll rate (units/sec) to the materials that reference it through their
    // RW UVAnim material extension (rwID_UVANIMPLUGIN 0x135) — the original's
    // real binding (TrackWorld/DffModel parse the extension into
    // Material::uv_anim). Applies to DFF props/sky below: on Arctic the world BSP
    // carries no UVAnim extension and the sea (SEA.DFF) / sky (SKY.DFF) clumps do
    // (names "bmp_Sea_M"/"bmp_Sky_M"), so the prop path is where the scroll
    // actually shows. See the format doc F3. (The D3D9 world scroll state,
    // mat_scroll_/uv_anim_, was only read by the deleted D3D9 world loop.)
    std::vector<std::pair<std::string, MatScroll>> uv_rates;   // entry name -> rate
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (ln < 4 || _stricmp(n + ln - 4, ".UVA") != 0) continue;
        std::uint32_t ul = 0;
        const std::uint8_t* ub = piz.blob(i, &ul);
        Track::UVDict uv;
        if (!ub || !uv.Parse(ub, ul)) break;
        for (const auto& e : uv.anims)
            if (e.du_dt != 0.f || e.dv_dt != 0.f)
                uv_rates.push_back({e.name, MatScroll{e.du_dt, e.dv_dt}});
        break;   // one .UVA dictionary per track
    }
    // UVAnim-extension name -> scroll rate (exact match; empty -> {0,0})
    auto uv_rate = [&](const char* nm) -> MatScroll {
        if (nm && nm[0])
            for (const auto& kv : uv_rates)
                if (kv.first == nm) return kv.second;
        return {};
    };

    // ---- F4 LAPDATA.LUA: real lap lines (parsed once; used by UpdateRace to
    // count laps at the data-declared finish gate instead of a hardcoded 0).
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (_strnicmp(piz.entry(i).name, "LAPDATA", 7) != 0) continue;
        std::uint32_t ll = 0;
        const std::uint8_t* lb = piz.blob(i, &ll);
        if (lb && ll)
            lap_data_.Parse(reinterpret_cast<const char*>(lb), ll);
        break;
    }

    // WS-E lighting: parse the track's RpLight set (COURSE.LUA
    // Lights_Filename -> LIGHTS.DFF) BEFORE building the world batches, so it
    // can be baked into the prelit vertex colours below. The world BSP carries
    // no vertex normals, so RW applies only the (directionless) ambient to the
    // static world — see ParseLightsDffAmbient.
    //
    // WS-E vehicle lighting (MASHED_RPLIGHT, default ON): faithful
    // FUN_00479330 (0x00479330) acquisition — float colours, no stream-flag
    // filter, last-wins per subtype, default lights when no Lights_Filename,
    // Ambient_RGB override in the DFF branch only. =0 reverts to the legacy
    // quantized first/summed extraction below.
    rp_light_on_ = RpLightEnabled();
    amb_world_ = 0;
    sun_color_ = 0;
    sun_dir_[0] = sun_dir_[1] = sun_dir_[2] = 0.f;
    has_sun_dir_ = false;
    amb_f_[0] = amb_f_[1] = amb_f_[2] = 0.f;
    sun_f_[0] = sun_f_[1] = sun_f_[2] = 0.f;
    sun_L_[0] = sun_L_[1] = sun_L_[2] = 0.f;
    char  lights_fn[64] = {};
    float amb_cfg[3] = {0.f, 0.f, 0.f};   // COURSE.LUA Ambient_RGB(r,g,b)
    bool  has_amb_cfg = false;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (ln < 4 || _strnicmp(n, "COURSE", 6) != 0 ||
            _stricmp(n + ln - 4, ".LUA") != 0)
            continue;
        std::uint32_t cl = 0;
        const std::uint8_t* cb = piz.blob(i, &cl);
        if (!cb) break;
        const char* cs = reinterpret_cast<const char*>(cb);
        for (std::uint32_t pos = 0; pos < cl; ) {
            std::uint32_t eol = pos;
            while (eol < cl && cs[eol] != '\n') ++eol;
            std::string line(cs + pos, eol - pos);
            pos = eol + 1;
            const std::size_t cmt = line.find("--");
            if (cmt != std::string::npos) line.resize(cmt);
            if (!lights_fn[0]) {
                std::sscanf(line.c_str(), " Lights_Filename( \"%63[^\"]\" )",
                            lights_fn) == 1 ||
                std::sscanf(line.c_str(), " Lights_Filename(\"%63[^\"]\")",
                            lights_fn);
            }
            // Ambient_RGB -> handler 0x0047aad0 -> course-description floats
            // +0x76..0x78 (config-table entry 0x00440d10). No shipped course
            // sets it; call syntax follows the other config calls.
            if (!has_amb_cfg &&
                std::sscanf(line.c_str(), " Ambient_RGB( %f , %f , %f",
                            &amb_cfg[0], &amb_cfg[1], &amb_cfg[2]) == 3)
                has_amb_cfg = true;
        }
        break;
    }
    const std::uint8_t* lights_db = nullptr;
    std::uint32_t       lights_dl = 0;
    if (lights_fn[0])
        for (std::uint32_t j = 0; j < piz.count(); ++j)
            if (_stricmp(piz.entry(j).name, lights_fn) == 0) {
                lights_db = piz.blob(j, &lights_dl);
                break;
            }
    if (!rp_light_on_) {
        // legacy path (pre-RpLight-subset behaviour, byte-identical)
        if (lights_db) {
            amb_world_ = ParseLightsDffAmbient(lights_db, lights_dl);
            ParseLightsDffDirectional(lights_db, lights_dl, &sun_color_,
                                      sun_dir_);
        }
    } else if (!lights_db) {
        // FUN_00479330 default branch (course_block[0x2640] empty,
        // LAB_00479951): frameless ambient from DAT_006132dc =
        // (0.25,0.25,0.25,1.0) + directional from DAT_006132ec =
        // (0.75,0.75,0.75,1.0) on a fresh frame rotated 60.0f (0x42700000)
        // about (1,0,0), combine 0 (replace). at-row of the axis-angle matrix
        // (FUN_004c4d20 deg->rad 0x3c8efa35, FUN_004c4a50 element formulas):
        // (0, -sin60, cos60).
        amb_f_[0] = amb_f_[1] = amb_f_[2] = 0.25f;
        sun_f_[0] = sun_f_[1] = sun_f_[2] = 0.75f;
        sun_dir_[0] = 0.f;
        sun_dir_[1] = -0.8660254f;   // -sin(60 deg)
        sun_dir_[2] = 0.5f;          //  cos(60 deg)
        has_sun_dir_ = true;
    } else {
        // FUN_00479330 DFF branch: every clump light, subtype byte only,
        // last-wins; then the Ambient_RGB override (this branch only) when
        // any component exceeds DAT_005d757c = 0.0f, replacing/creating the
        // ambient light's colour (FUN_004e4900 @ 0x00479944).
        bool has_amb = false;
        ParseLightsDffFaithful(lights_db, lights_dl, amb_f_, &has_amb,
                               sun_f_, sun_dir_, &has_sun_dir_);
        if (has_amb_cfg &&
            (amb_cfg[0] > 0.f || amb_cfg[1] > 0.f || amb_cfg[2] > 0.f)) {
            amb_f_[0] = amb_cfg[0]; amb_f_[1] = amb_cfg[1];
            amb_f_[2] = amb_cfg[2];
        }
    }
    if (rp_light_on_) {
        // packed mirrors for the legacy consumers (world prelit fill, logs)
        auto cl255 = [](float f) -> int {
            int v = static_cast<int>(f * 255.f + 0.5f);
            return v < 0 ? 0 : v > 255 ? 255 : v;
        };
        amb_world_ = D3DCOLOR_XRGB(cl255(amb_f_[0]), cl255(amb_f_[1]),
                                   cl255(amb_f_[2]));
        if (has_sun_dir_) {
            sun_color_ = D3DCOLOR_XRGB(cl255(sun_f_[0]), cl255(sun_f_[1]),
                                       cl255(sun_f_[2]));
            float lx = -sun_dir_[0], ly = -sun_dir_[1], lz = -sun_dir_[2];
            const float ll = std::sqrt(lx*lx + ly*ly + lz*lz);
            if (ll > 1e-6f) { sun_L_[0] = lx/ll; sun_L_[1] = ly/ll;
                              sun_L_[2] = lz/ll; }
            else            { has_sun_dir_ = false; }
        }
    }
    // [TERRAIN-DIAG] MASHED_TERRAIN_NOLIGHT=1 zeros the runtime ambient+sun so
    // every prop/atomic renders at its pure baked prelit (no +amb / +sun*ndl in
    // LightAtomicVertex). Diagnostic ONLY: isolates whether the race first-frame
    // terrain over-brightness comes from runtime lighting or the baked prelit.
    // =1 zero both, =2 zero ambient only, =3 zero sun only.
    const char* e_nolight = std::getenv("MASHED_TERRAIN_NOLIGHT");
    if (e_nolight && (e_nolight[0] == '1' || e_nolight[0] == '2')) {
        amb_f_[0] = amb_f_[1] = amb_f_[2] = 0.f;
        amb_world_ = 0;
    }
    if (e_nolight && (e_nolight[0] == '1' || e_nolight[0] == '3')) {
        sun_f_[0] = sun_f_[1] = sun_f_[2] = 0.f;
        sun_color_ = 0;
        has_sun_dir_ = false;
    }
    if (log)
        std::fprintf(log,
                     "  WS-E lights: ambient=0x%06lX (RGB %lu,%lu,%lu)"
                     " sun=0x%06lX dir=(%.3f,%.3f,%.3f)\n",
                     static_cast<unsigned long>(amb_world_),
                     static_cast<unsigned long>((amb_world_ >> 16) & 0xFF),
                     static_cast<unsigned long>((amb_world_ >> 8) & 0xFF),
                     static_cast<unsigned long>(amb_world_ & 0xFF),
                     static_cast<unsigned long>(sun_color_),
                     sun_dir_[0], sun_dir_[1], sun_dir_[2]);

    // ---- track props ------------------------------------------------------
    // COURSE.LUA wires them: RWP_Object(i,"name","X.dff","Y.mts") = physics
    // props placed by an MTS matrix set (ExportMatrices: u32 count + per-
    // instance chunk 0x0d {STRUCT: 3x3 rot + translation + u32}); and
    // Clump_Filename(i,"x.dff") = world clumps at identity (their frames
    // carry placement), minus Clump_Exclude_From_World(i) overlays.
    {
        auto find_entry = [&](const char* name, std::uint32_t* len)
            -> const std::uint8_t* {
            for (std::uint32_t i = 0; i < piz.count(); ++i)
                if (_stricmp(piz.entry(i).name, name) == 0)
                    return piz.blob(i, len);
            return nullptr;
        };
        std::uint32_t lua_len = 0;
        const std::uint8_t* lua = nullptr;
        for (std::uint32_t i = 0; i < piz.count(); ++i) {
            const char* n = piz.entry(i).name;
            const std::size_t ln = std::strlen(n);
            if (ln > 4 && _stricmp(n + ln - 4, ".LUA") == 0 &&
                _strnicmp(n, "COURSE", 6) == 0) {
                lua = piz.blob(i, &lua_len);
                break;
            }
        }
        // POWER-UP PLACEMENT. The original places from the track piz's
        // POWERUPS_GOLD.DFF, not from POWERUPS_GOLD.LUA: FUN_004264d0
        // @0x004264d0 pushes "powerups_gold.dff" (0x005cd4e4) at 0x004265bd,
        // loads the clump (FUN_0042a5d0, 0x004265c2) and calls the live
        // placement path FUN_00426460 @0x00426460 at 0x004265d2. The Lua arm
        // below is the original's FALLBACK (JE 0x004265df, taken only when the
        // clump is null) and is unreachable on shipping data — all 13 track
        // pizzes carry the DFF. Keeping it mirrors the original's structure;
        // it does not run.
        //
        // The two files genuinely disagree, which is why this mattered: on
        // Forest they share ZERO positions, on Arctic and City 4 of 18. Live
        // reads of the original's pool on TRAINING (5 boxes) and ARCTIC (7)
        // match the DFF bit-for-bit — see
        // re/analysis/PICKUPS_LOOK_PLACEMENT_2026-09-29.md and
        // verify/pickups_fix_20261001/.
        powerup_spawns_.clear();
        {
            std::uint32_t pdl = 0;
            const std::uint8_t* pdb = find_entry("POWERUPS_GOLD.DFF", &pdl);
            std::vector<mashed_re::Track::PowerupMarker> marks;
            const char* perr = nullptr;
            if (pdb && pdl &&
                mashed_re::Track::ParsePowerupMarkers(pdb, pdl, &marks, &perr)) {
                for (const auto& m : marks) {
                    if (!m.has_raw) continue;     // FUN_004b5190 found no user data
                    PickupField::Spawn s{};
                    s.pos[0] = m.pos[0];
                    s.pos[1] = m.pos[1];
                    s.pos[2] = m.pos[2];
                    s.type = m.type;
                    s.respawn = m.respawn;
                    s.atomic = m.atomic;
                    powerup_spawns_.push_back(s);
                }
            }
            if (log)
                std::fprintf(log, "PU-SRC dff=%d bytes=%u markers=%zu err=%s\n",
                             pdb ? 1 : 0, pdl, marks.size(),
                             perr ? perr : "-");
        }
        for (std::uint32_t i = 0; i < piz.count(); ++i) {
            if (!powerup_spawns_.empty()) break;   // DFF won; Lua arm not taken
            if (_strnicmp(piz.entry(i).name, "POWERUPS", 8) != 0) continue;
            std::uint32_t pl = 0;
            const std::uint8_t* pb = piz.blob(i, &pl);
            if (!pb || pl == 0) break;
            std::string txt(reinterpret_cast<const char*>(pb), pl);
            // alias map from the "local NAME = id" lines at the top of the file
            std::vector<std::pair<std::string,int>> alias;
            {
                std::size_t p = 0;
                while ((p = txt.find("local ", p)) != std::string::npos) {
                    char nm[32]; int id = 0;
                    if (std::sscanf(txt.c_str() + p, "local %31[A-Za-z0-9_] = %d", nm, &id) == 2)
                        alias.emplace_back(nm, id);
                    p += 6;
                }
            }
            auto resolve = [&](const char* a) -> int {
                for (auto& kv : alias) if (kv.first == a) return kv.second;
                return std::atoi(a);   // numeric type
            };
            // walk Set_Current_Position(...) / Set_Current_Type(...) /
            // Set_Current_Respawn_Time(...) / Place_Powerup()
            PickupField::Spawn cur{}; cur.respawn = 5.f; bool havePos = false;
            std::size_t p = 0;
            auto findNext = [&](const char* key) { return txt.find(key, p); };
            while (p < txt.size()) {
                std::size_t pos = txt.find("Set_Current_Position", p);
                std::size_t typ = txt.find("Set_Current_Type", p);
                std::size_t rsp = txt.find("Set_Current_Respawn_Time", p);
                std::size_t plc = txt.find("Place_Powerup", p);
                std::size_t nxt = pos;
                if (typ < nxt) nxt = typ;
                if (rsp < nxt) nxt = rsp;
                if (plc < nxt) nxt = plc;
                if (nxt == std::string::npos) break;
                if (nxt == pos) {
                    std::sscanf(txt.c_str() + pos,
                                "Set_Current_Position( %f , %f , %f )",
                                &cur.pos[0], &cur.pos[1], &cur.pos[2]);
                    havePos = true; p = pos + 20;
                } else if (nxt == typ) {
                    char a[32] = {};
                    std::sscanf(txt.c_str() + typ, "Set_Current_Type( %31[A-Za-z0-9_] )", a);
                    cur.type = resolve(a); p = typ + 16;
                } else if (nxt == rsp) {
                    std::sscanf(txt.c_str() + rsp, "Set_Current_Respawn_Time( %f )", &cur.respawn);
                    p = rsp + 24;
                } else {
                    if (havePos) powerup_spawns_.push_back(cur);
                    havePos = false; cur.respawn = 5.f; p = plc + 13;
                }
            }
            (void)findNext;
            break;
        }
        // Every prop model is handed to librw (RaceSubmit_RegisterModel). Only the
        // sky dome ALSO keeps a D3D9 vertex bake (`d3d9_batches`): librw does not draw
        // the sky, TrackRenderer::Render does, as a camera-locked unlit prelit
        // backdrop. Track props have no D3D9 copy; the D3D9 prop draw was deleted in
        // D1-residue R3 (2026-10-16).
        auto load_prop = [&](const char* dff_name, Prop* p,
                             bool d3d9_batches = false,
                             bool bake_frames = true) -> bool {
            std::uint32_t dl = 0;
            const std::uint8_t* db = find_entry(dff_name, &dl);
            if (!db) return false;
            Track::DffModel m;
            if (!m.Parse(db, dl, bake_frames)) return false;
            // [geomlight water-scope 2026-08-31] Parse() gets only a blob, so the
            // asset name has to be attached here -- this is the one place that
            // knows it. RwSceneBuild::BuildClump uses it to scope the ambient fold
            // to actual water surfaces (SEA/WATER/LAKE/RIVER); the RW geometry
            // flags alone also match awnings, lampposts and skydomes.
            std::snprintf(m.source_name, sizeof(m.source_name), "%s", dff_name);
            if (d3d9_batches)
                BuildDffBatches(dev, m, dicts, &p->batches, &p->textures);
            // [rstates] Vertex-alpha need of the mesh RpClumpRender (0x004e6680) draws
            // LAST for this model. RpClumpAddAtomic FUN_004e6f80 inserts at the list head,
            // so the walk is newest-first and ends on DFF atomic 0; within an atomic the
            // meshes run in BinMesh order (rstates F7), which DffModel's batches follow, so the
            // last one is atomic 0's last batch (its highest material when F7 is off). The
            // need is the per-mesh rule of the default callback (0x004ec462..0x004ec491)
            // and FUN_00543500: material alpha != 0xff or vertex alpha in the mesh. The
            // prelit test covers this batch's vertices only (RW tests the mesh's vertex
            // range; the two agree for a single-mesh atomic such as every track sky).
            {
                std::ptrdiff_t last = -1;
                for (std::size_t bi = 0; bi < m.batches.size(); ++bi)
                    if (m.batches[bi].atomic == 0) last = static_cast<std::ptrdiff_t>(bi);
                p->last_mesh_va = false;
                if (last >= 0) {
                    const Track::DffBatch& lb = m.batches[static_cast<std::size_t>(last)];
                    p->last_mesh_va = m.materials[lb.material].rgba[3] != 255;
                    for (std::uint32_t c : lb.prelit)
                        if ((c >> 24) != 0xFFu) { p->last_mesh_va = true; break; }
                }
            }
            // F3: bind each material's UVAnim-extension name to its .UVA rate.
            p->mat_scroll.assign(m.materials.size(), {});
            for (std::size_t mi = 0; mi < m.materials.size(); ++mi)
                p->mat_scroll[mi] = uv_rate(m.materials[mi].uv_anim);
            // E2'b step 3: hand this model to librw HERE -- `m` is a local that dies
            // at the closing brace, and nothing else retains a parsed DffModel.
            // The UV rates must be resolved BEFORE this call, not after: librw has no
            // texture matrix and applies the scroll by moving the coordinates, so it
            // needs the rates at registration time. They used to be filled in below
            // this call, which left the librw side with none.
            std::vector<float> rates(m.materials.size() * 2, 0.f);
            for (std::size_t mi = 0; mi < p->mat_scroll.size(); ++mi) {
                rates[mi * 2 + 0] = p->mat_scroll[mi].du;
                rates[mi * 2 + 1] = p->mat_scroll[mi].dv;
            }
            p->rw_model = LibRw::RaceSubmit_RegisterModel(
                m, dicts.data(), dicts.size(), amb_world_,
                rates.empty() ? nullptr : rates.data(), m.materials.size());
            return true;
        };
        // An .MTS is u32 count + one RwMatrix chunk (0x0d) per instance, {STRUCT: 3x3 rotation +
        // translation + u32} = 76 bytes each (FUN_00478110 -> RwMatrixStreamRead FUN_004cf2b0). One
        // reader for RWP_Object and Clump_Mts_Pair, so the two cannot drift.
        auto read_mts = [](const std::uint8_t* mb, std::uint32_t ml,
                           std::vector<D3DMATRIX>* out) {
            const std::uint32_t cnt = *reinterpret_cast<const std::uint32_t*>(mb);
            std::size_t off = 4;
            for (std::uint32_t k = 0; k < cnt && off + 24 + 48 <= ml; ++k) {
                const std::uint32_t csz =
                    *reinterpret_cast<const std::uint32_t*>(mb + off + 4);
                const float* f = reinterpret_cast<const float*>(mb + off + 24);
                D3DMATRIX m2;
                MatIdentity(&m2);
                m2._11 = f[0]; m2._12 = f[1]; m2._13 = f[2];
                m2._21 = f[3]; m2._22 = f[4]; m2._23 = f[5];
                m2._31 = f[6]; m2._32 = f[7]; m2._33 = f[8];
                m2._41 = f[9]; m2._42 = f[10]; m2._43 = f[11];
                out->push_back(m2);
                off += 12 + csz;
            }
        };
        // [rprops U-9695] MASHED_NO_PROPPLACE=1 keeps the pre-rprops placement (every Clump_Filename
        // clump at identity) for A/B: Clump_Mts_Pair, the camera-men and the Arctic ships.
        static const bool s_no_propplace = std::getenv("MASHED_NO_PROPPLACE") != nullptr;
        prop_anims_.clear();
        prop_acc_[0] = prop_acc_[1] = prop_acc_[2] = 0.f;
        prop_tick_frames_ = 0;
        if (lua) {
            // line-scan the Lua text for the wiring calls (comments start
            // with "--"; skip them)
            const char* s = reinterpret_cast<const char*>(lua);
            std::size_t pos = 0;
            bool excluded[64] = {};
            struct ClumpRef { int idx; char dff[64]; };
            std::vector<ClumpRef> clumps;
            // [rprops U-9695] Mts_Filename(i,"x.mts") (FUN_0047a280, slot i of the 16 Mts) and
            // Clump_Mts_Pair(clump, mts) (FUN_0047a950): Course::CreateFromDescription 0x00479330
            // replicates the clump's first atomic once per record of the paired Mts.
            char mts_names[16][64] = {};
            struct MtsPair { int clump; int mts; };
            std::vector<MtsPair> mts_pairs;
            while (pos < lua_len) {
                std::size_t eol = pos;
                while (eol < lua_len && s[eol] != '\n') ++eol;
                std::string line(s + pos, eol - pos);
                pos = eol + 1;
                const std::size_t cmt = line.find("--");
                if (cmt != std::string::npos) line.resize(cmt);
                char a[64] = {}, b[64] = {};
                int idx = -1, pm = -1;
                if (std::sscanf(line.c_str(),
                                " RWP_Object( %*d , \"%*[^\"]\" , \"%63[^\"]\" "
                                ", \"%63[^\"]\" )", a, b) == 2 ||
                    std::sscanf(line.c_str(),
                                " RWP_Object( %*d ,\"%*[^\"]\", \"%63[^\"]\", "
                                "\"%63[^\"]\" )", a, b) == 2) {
                    Prop p;
                    std::uint32_t ml = 0;
                    const std::uint8_t* mb = find_entry(b, &ml);
                    if (mb && ml >= 4 && load_prop(a, &p)) {
                        read_mts(mb, ml, &p.instances);
                        // [rstates] RWP_Object props are drawn by the original's world
                        // pass between sectors (Arctic Tyres after its first sector).
                        p.rw_kind = LibRw::kInstInWorld;
                        if (!p.instances.empty())
                            props_.push_back(std::move(p));
                    }
                } else if (line.find("RainEnable(") != std::string::npos) {
                    // [rstates] Lua `RainEnable` (handler 0x004917a0, name 0x005cdb28)
                    // sets DAT_00771534 (0x004917a5), which gates the line pass
                    // FUN_004914b0 -> FUN_00499d90 and its direct cull write (U-9690).
                    // Comments are already stripped from `line`.
                    rain_enabled_ = true;
                } else if (std::sscanf(line.c_str(),
                                       " Clump_Filename( %d , \"%63[^\"]\" )",
                                       &idx, a) == 2 ||
                           std::sscanf(line.c_str(),
                                       " Clump_Filename(%d,\"%63[^\"]\")",
                                       &idx, a) == 2) {
                    if (idx >= 0 && idx < 64) {
                        ClumpRef c; c.idx = idx;
                        std::snprintf(c.dff, sizeof(c.dff), "%s", a);
                        clumps.push_back(c);
                    }
                } else if (std::sscanf(line.c_str(),
                                       " Clump_Exclude_From_World( %d )",
                                       &idx) == 1 ||
                           std::sscanf(line.c_str(),
                                       " Clump_Exclude_From_World(%d)",
                                       &idx) == 1) {
                    if (idx >= 0 && idx < 64) excluded[idx] = true;
                } else if (std::sscanf(line.c_str(),
                                       " Mts_Filename( %d , \"%63[^\"]\" )",
                                       &idx, a) == 2) {
                    if (idx >= 0 && idx < 16)
                        std::snprintf(mts_names[idx], sizeof(mts_names[idx]), "%s", a);
                } else if (std::sscanf(line.c_str(), " Clump_Mts_Pair( %d , %d )",
                                       &idx, &pm) == 2) {
                    if (idx >= 0 && idx < 64 && pm >= 0 && pm < 16)
                        mts_pairs.push_back({ idx, pm });
                } else if (std::sscanf(line.c_str(),
                                       " Course_Id( %d )", &idx) == 1 ||
                           std::sscanf(line.c_str(),
                                       " Course_Id(%d)", &idx) == 1) {
                    course_id_ = idx;   // -> Common/LED.piz LE<id>.LED
                } else if (std::strstr(line.c_str(), "Vehicle_Shininess_Range")) {
                    // [rtex] Lua binding 0x0047ac80: arg 1 -> course +0x204 (0x0047acb0),
                    // arg 2 -> +0x208 (0x0047acb7); FUN_00426de0 / FUN_00426df0 read them
                    // back as the env coefficient's lerp ends (FUN_00420290).
                    float lo = 0.f, hi = 1.f;
                    const char* p = std::strchr(line.c_str(), '(');
                    if (p && std::sscanf(p + 1, " %f , %f", &lo, &hi) == 2) {
                        shine_lo_ = lo;
                        shine_hi_ = hi;
                    }
                } else {
                    // Setup_Fog(near, far, r, g, b) — track fog wiring. args 1/2
                    // are ABSOLUTE camera-space fog distances (near < far), NOT a
                    // fraction. (Asset survey: Arctic 0.1/70, Egypt 20/100,
                    // Highway 50/300, training 20/360, Storm 20/100, sands
                    // 250/255 — every track has near < far.)
                    float fa = 0.f, fb = 0.f; int fr = 0, fg2 = 0, fb2 = 0;
                    char sk[64] = {};
                    if (std::sscanf(line.c_str(),
                                    " Setup_Fog( %f , %f , %d , %d , %d )",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5 ||
                        std::sscanf(line.c_str(),
                                    " Setup_Fog(%f,%f,%d,%d,%d)",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5) {
                        fog_on_ = true;
                        // WS-E TRACKFIX (2026-06-29): fog_start_ = fa (the near
                        // distance), NOT fa*fb. The old `fa*fb` only stayed valid
                        // for Arctic (0.1*70 = 7 < 70); for every track with
                        // fa >= 1 it produced fog_start_ > fog_end_, INVERTING the
                        // D3D linear-fog ramp (factor = (end-d)/(end-start) goes
                        // negative for near d) so ALL near geometry clamped to
                        // factor 0 = full fog colour. That is the blown-out frame
                        // (Egypt/Highway/training/sands == the exact fog colour)
                        // and the black frame (Storm, fog colour 0,0,0).
                        fog_start_ = fa;
                        fog_end_   = fb;
                        fog_color_ = D3DCOLOR_XRGB(fr & 0xFF, fg2 & 0xFF,
                                                   fb2 & 0xFF);
                    } else if (std::sscanf(line.c_str(),
                                    " Sky_Filename( %*d , \"%63[^\"]\" )",
                                    sk) == 1 ||
                               std::sscanf(line.c_str(),
                                    " Sky_Filename(%*d,\"%63[^\"]\")",
                                    sk) == 1) {
                        // sky clump: drawn first, z-write off, unfogged. The only
                        // prop that keeps a D3D9 bake (librw does not draw it).
                        // [rprops U-9696] the game
                        // rebuilds the sky clump frame every frame (0x004491e0 / 0x00426d20), so
                        // its root-frame offset (Arctic SKY.DFF y -0.155, City SKYDOME.DFF y -0.070)
                        // must not be baked into the vertices.
                        static const bool s_no_skyfix_ld = std::getenv("MASHED_NO_SKYFIX") != nullptr;
                        load_prop(sk, &sky_, /*d3d9_batches=*/true, s_no_skyfix_ld);
                    } else if (std::sscanf(line.c_str(),
                                    " AI_Bsp_Filename( \"%63[^\"]\" )", sk) == 1 ||
                               std::sscanf(line.c_str(),
                                    " AI_Bsp_Filename(\"%63[^\"]\")", sk) == 1) {
                        // the gate BSP filename varies per track (ai.bsp /
                        // lap.bsp / Ai1.bsp); capture it so the gate parser finds
                        // the right entry instead of guessing "AI*.BSP".
                        std::snprintf(gate_bsp_, sizeof(gate_bsp_), "%s", sk);
                    }
                }
            }
            // [U-9062 2026-08-31] Clump_Exclude_From_World does NOT mean "do not load".
            // DECODED from MASHED.exe (Ghidra, analyzeHeadless on Mashed_pool14):
            //
            //   Lua registration table 0x00440bc0..0x00440d40 binds the command name at
            //   0x005cde94 to handler 0x0047aa20 (registrar 0x0047b980).
            //   Handler 0x0047aa20 does exactly one thing: desc[0xd4 + idx*4] = 1.
            //   It never unloads, frees, or clears the filename slot.
            //
            //   The course loader Course::CreateFromDescription (0x00479330) then:
            //     - loads EVERY clump whose filename slot is non-empty, gating only on
            //       the name, never on the flag (clump loop, "LOADING Clump [%s] with
            //       index of [%d]");
            //     - walks the 0x40 clump slots against the flag array (`add edi,0xd4`
            //       at 0x00479d93) and gates exactly ONE call on it:
            //         0x00479da6  cmp dword ptr [edi],0     <- the exclude flag
            //         0x00479da9  jnz  (skip)
            //         0x00479db3  call 0x004e4450(world, clump)
            //       0x004e4450 links the clump's frame and runs RpClumpForAllAtomics /
            //       RpClumpForAllLights to register it INTO the world; the world arg is
            //       [course+0x105d4], the same field RpWorldAddLight is called on.
            //     - and unconditionally calls 0x00474fd0 (get-first-atomic) at
            //       0x00479dbe for excluded and non-excluded clumps alike, storing the
            //       handle at +0x120.
            //
            // So an excluded clump IS loaded and DOES keep a live atomic handle; it is
            // only kept out of the world's frame/atomic/light registration. Reading the
            // flag as "skip loading" is what deleted City's road (index 11), its four
            // buildings, Standard, Trunk and water, and Dump's road (index 9) -- the
            // U-9062/U-9063 surfaces are MISSING, not mis-lit.
            //
            // We render props explicitly rather than through a world pass, so loading
            // them as ordinary props is the faithful analogue of "loaded, has an atomic,
            // not in the world". Measured against pose-matched originals: City 25.43 ->
            // 11.23, Dump 84.78 -> 9.62, Arctic and TRAINING unchanged.
            // MASHED_TRACK_SKIP_EXCLUDED=1 restores the old drop-them behaviour for A/B.
            //
            // NOT established: what the world registration itself changes in the
            // original beyond this (PVS/culling participation, draw order). A pixel
            // measurement cannot see it, and none is claimed here.
            static const bool s_skip_excluded =
                std::getenv("MASHED_TRACK_SKIP_EXCLUDED") != nullptr;
            for (const auto& c : clumps) {
                if (excluded[c.idx] && s_skip_excluded) continue;
                Prop p;
                // [rprops U-9695] a Clump_Mts_Pair clump's atomic frame is replaced by the Mts
                // record and its clump-frame translation zeroed (0x00479330), so its vertices must
                // stay in geometry space: City Standard.dff's root frame carries y -0.957, which
                // the original discards and a baked parse would keep.
                bool paired = false;
                if (!s_no_propplace)
                    for (const auto& mp : mts_pairs)
                        if (mp.clump == c.idx && mts_names[mp.mts][0]) paired = true;                if (load_prop(c.dff, &p, false, !paired)) {
                    // [SEA-TILE 2026-09-29] Arctic's sea is NOT placed by its DFF
                    // (SEA.DFF frame[0].pos == (0,0,0), verts Y 0.000..0.904) and NOT
                    // by COURSE.LUA (no placement command exists in the 68-entry Lua
                    // binding table). The original places it from the per-track node
                    // table 0x005f33f8: record "arctic" at 0x005f3488, slot 0 at
                    // 0x005f349c = 0x00448940. Confirmed live: the original's clump[2]
                    // frame carries modelling == LTM == translate(-150,-4.1,-150) --
                    // tile 0 of that grid (verify/sea_level_20260929/orig_sea_matrix.json).
                    // Placed ONCE, at load: a whole-.text sweep for the tile array finds
                    // only slot 0 (place+clone), slot 2 0x00449030 (render, 0x004e6680
                    // per tile) and slot 5 0x004490d0 (RpClumpDestroy on the 24 clones,
                    // from 0x008963e4 -- it skips the base). Neither transforms a frame,
                    // and the per-frame slot 1 0x00448ef0 bobs DAT_008962e0, a DIFFERENT
                    // array. So a static instance list is the complete placement model.
                    // At identity the sheet landed on the road (GRAPH.BSP median Y under
                    // the start grid = 0.250) and covered 70.39% of the s8 frame.
                    //
                    // THE PER-INSTRUCTION TRANSCRIPTION IS NOT REPEATED HERE. It lives
                    // once, with the body, in Render/ArcticTrackNodeSlot0.cpp -- two
                    // copies of the same citation drift (memory
                    // `duplicate-rva-implementations-drift`).
                    // Diagnosis: re/analysis/SEA_LEVEL_2026-09-29.md
                    static const bool s_no_sea_tile =
                        std::getenv("MASHED_NO_SEA_TILE") != nullptr;
                    // RETROFIT 2026-10-01: the 5x5 loop that used to sit here was
                    // a second body for the grid. It now comes from the REAL port
                    // of the node, Render/ArcticTrackNodeSlot0.cpp
                    // (`ArcticTrackNodeSlot0`, 0x00448940) -- one TU, in both
                    // mashedmod/exe_sources.rsp and asi_sources.rsp. The node's
                    // own translate loop and this renderer call the SAME
                    // ArcticSeaTileGrid; no second copy of the grid remains.
                    // [rstates] World registration (0x00479da6/0x00479db3): only the
                    // clumps Clump_Exclude_From_World does not flag join the RpWorld.
                    p.rw_kind = excluded[c.idx] ? LibRw::kInstAfterWorld
                                                : LibRw::kInstInWorld;
                    p.clump_idx = c.idx;
                    if (!s_no_sea_tile && course_id_ == 0 && c.idx == 2) {
                        // The 25 tiles are drawn by Arctic slot 2 (0x00449030,
                        // RpClumpRender per tile) after the world, not by the world pass.
                        p.rw_kind = LibRw::kInstAfterWorld;
                        float grid[mashed_re::Render::kArcticSeaTileCount][3];
                        mashed_re::Render::ArcticSeaTileGrid(grid);
                        for (int k = 0; k < mashed_re::Render::kArcticSeaTileCount; ++k) {
                            D3DMATRIX m;
                            MatIdentity(&m);
                            m._41 = grid[k][0];
                            m._42 = grid[k][1];
                            m._43 = grid[k][2];
                            p.instances.push_back(m);
                        }
                        // Rule 1 of verify/sea_fix_20260930/PREREG.md is a
                        // per-instance assertion, so print every instance --
                        // a count alone cannot show the grid is the original's.
                        if (log) {
                            std::fprintf(log,
                                "  SEA-TILE course_id=%d clump=%d dff=%s "
                                "instances=%zu grid=5x5 origin=-150 step=60 y=-4.1\n",
                                course_id_, c.idx, c.dff, p.instances.size());
                            for (std::size_t si = 0; si < p.instances.size(); ++si)
                                std::fprintf(log,
                                    "  SEA-TILE[%02zu] pos=(%.6f, %.6f, %.6f)\n",
                                    si, p.instances[si]._41,
                                    p.instances[si]._42, p.instances[si]._43);
                        }
                    } else {
                        D3DMATRIX id;
                        MatIdentity(&id);
                        p.instances.push_back(id);
                        // Rule 1(4): the branch must NOT fire off Arctic. Print
                        // the negative for clump 2 on every track so "0 sea-tile
                        // instances" is an observed line, not a missing one
                        // (memory `absent-log-proves-nothing-run-a-control`).
                        if (log && c.idx == 2)
                            std::fprintf(log,
                                "  SEA-TILE-OFF course_id=%d clump=%d dff=%s "
                                "instances=1 (identity) sea_tile_instances=0\n",
                                course_id_, c.idx, c.dff);
                    }
                    // [rprops U-9695] per-track placement, each case from the original's code.
                    int anim_kind = 0;
                    if (!s_no_propplace) {
                        int pair_mts = -1;
                        for (const auto& mp : mts_pairs)
                            if (mp.clump == c.idx) { pair_mts = mp.mts; break; }
                        if (pair_mts >= 0 && mts_names[pair_mts][0]) {
                            // Clump_Mts_Pair: the single atomic of the clump (all ten paired
                            // clumps in the game have exactly one) is replaced by one copy per
                            // Mts record, each frame set to the record matrix (0x00479330, the
                            // pair loop after the sky load: FUN_004e69a0 clone, FUN_004c1480
                            // RwFrameTransform(frame, record, REPLACE)). The record array is
                            // course + mts*0x1000, count at course + 0x10000 + mts*4.
                            std::uint32_t ml = 0;
                            const std::uint8_t* mb = find_entry(mts_names[pair_mts], &ml);
                            std::vector<D3DMATRIX> recs;
                            if (mb && ml >= 4) read_mts(mb, ml, &recs);
                            if (!recs.empty()) {
                                p.instances = std::move(recs);
                                p.mts_pair = true;   // [rstates F8]
                                if (log)
                                    std::fprintf(log, "  PROP-PAIR course_id=%d clump=%d dff=%s mts=%d "
                                                 "(%s) instances=%zu\n", course_id_, c.idx, c.dff,
                                                 pair_mts, mts_names[pair_mts], p.instances.size());
                            }
                        } else if (_stricmp(c.dff, "CAMMAN.DFF") == 0) {
                            // Camera-men: entries 1..N of the track's camera table, each placed by
                            // the track's per-frame helper (kCamMan) at its row xyz with its yaw.
                            for (const auto& ct : kCamMan) {
                                if (ct.course_id != course_id_) continue;
                                std::vector<D3DMATRIX> cm;
                                for (int k = 1; k <= ct.n; ++k) {
                                    float xyz[3], yaw = 0.f;
                                    if (!CamEntry::TableRow(course_id_, k, xyz, &yaw)) { cm.clear(); break; }
                                    D3DMATRIX m;
                                    PlaceMatrix(xyz, yaw, &m);
                                    cm.push_back(m);
                                }
                                if (!cm.empty()) {
                                    p.instances = std::move(cm);
                                    if (log)
                                        std::fprintf(log, "  PROP-CAMMAN course_id=%d clump=%d helper=0x%08x "
                                                     "instances=%zu\n", course_id_, c.idx, ct.helper,
                                                     p.instances.size());
                                }
                                break;
                            }
                        } else if (course_id_ == 0 && _stricmp(c.dff, "FREIGHTER.DFF") == 0) {
                            // Arctic freighters: the clump + one clone (0x00448770), yaw/position
                            // from the 0x005f95d0 tables; UpdatePropAnims re-poses them per frame.
                            p.instances.clear();
                            for (int k = 0; k < 2; ++k) {
                                D3DMATRIX m;
                                PlaceMatrix(kFreighterPos[k], kFreighterYaw[k], &m);
                                p.instances.push_back(m);
                            }
                            anim_kind = kAnimFreighter;
                        } else if (course_id_ == 0 && _stricmp(c.dff, "FISHINGBOAT.DFF") == 0) {
                            // Arctic fishing boats: the clump + three clones (0x00448880).
                            p.instances.clear();
                            for (int k = 0; k < 4; ++k) {
                                D3DMATRIX m;
                                PlaceMatrix(kBoatPos[k], kBoatYaw[k], &m);
                                p.instances.push_back(m);
                            }
                            anim_kind = kAnimBoat;
                        }
                    }
                    props_.push_back(std::move(p));
                    if (anim_kind) prop_anims_.push_back({ anim_kind, props_.size() - 1 });
                }
            }
        }
    }

    // F3 diag: materials that actually scroll, by where they live.
    if (log) {
        int pc = 0; for (const auto& pr : props_) for (const auto& s : pr.mat_scroll)
                        if (s.du != 0.f || s.dv != 0.f) ++pc;
        int sc = 0; for (const auto& s : sky_.mat_scroll) if (s.du != 0.f || s.dv != 0.f) ++sc;
        std::fprintf(log, "  F3 UV-anim: %zu .UVA rate(s); scrolling materials "
                          "props=%d sky=%d\n", uv_rates.size(), pc, sc);
    }

    // ---- F2 animated copters: COURSE.LUA SetCopter / KTCSCRIPT.LUA
    // KTC_NewCopter bind a copter model DFF (Common/Perm.piz) to a
    // General_Anim_Filename .ANM flight path (track piz); flown under the
    // per-frame HAnim transform (see LoadCopters + the Render copter pass).
    LoadCopters(dev, piz, piz_path, log);

    // [camribbon] The camera-path ribbon (FUN_00426d00's node corners) comes from the original's
    // node-record build by default (Race::PathNodes_CameraNodes = FUN_00478660 BuildNode, the
    // sign classification at 0x0047897c). MASHED_NO_CAM_RIBBON_FIX=1 keeps the previous
    // stream-order corners (the gate vertices sorted by index, c0 = first, c3 = fourth), for A/B only.
    const bool ribbon_stream_order = (std::getenv("MASHED_NO_CAM_RIBBON_FIX") != nullptr);
    cam_nodes_.clear();

    // ---- AI path gates (AI*.BSP — same world stream; 4-vert vertical quad
    // per gate; the material RED byte = gate index; LAPDATA's Lap_Line
    // numbers index these). Build ordered gate centers; gate 0 = start line.
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        // Prefer the COURSE.LUA-declared gate BSP (ai.bsp / lap.bsp / Ai1.bsp...);
        // fall back to the "AI*.BSP" heuristic when none was declared.
        const bool is_gate_bsp =
            gate_bsp_[0] ? (_stricmp(n, gate_bsp_) == 0)
                         : (ln >= 6 && _strnicmp(n, "AI", 2) == 0 &&
                            _stricmp(n + ln - 4, ".BSP") == 0);
        if (is_gate_bsp) {
            std::uint32_t al = 0;
            const std::uint8_t* ab = piz.blob(i, &al);
            Track::World aw;
            if (ab && aw.Parse(ab, al)) {
                // [D-11072 P1] the original's course block (node records, route, tables)
                // from this same AI BSP + LAPDATA.LUA; default-OFF MASHED_PATHPROG.
                // PREREG verify/d3_modes37_20261009/PREREG_PATHPROG.md.
                if (Race::PathProg_Enabled() || Race::RacePctOrig_Enabled()) {   // [U-9685] arm R needs the course block too
                    const std::uint8_t* lb = nullptr; std::uint32_t ll = 0;
                    for (std::uint32_t j = 0; j < piz.count(); ++j)
                        if (_strnicmp(piz.entry(j).name, "LAPDATA", 7) == 0) { lb = piz.blob(j, &ll); break; }
                    Race::PathNodes_Build(aw, reinterpret_cast<const char*>(lb), ll);
                    Race::PathProg_MaybeReplay();     // [D-11072 P2] G-P2-REPLAY (env-gated)
                    Ai::TargetClosest_MaybeReplay();  // [D-11072 P3] G-P3-REPLAY (env-gated)
                    Ai::ModeSeven_MaybeReplay();      // [mode 7 M4] G-M4-REPLAY (env-gated)
                    Ai::AiLeaf_MaybeReplay();         // [D3 bulk G-AILEAF] G-AILEAF-REPLAY (env-gated)
                    Race::RoundGrid_MaybeReplay();    // [round grid Stage A] G-RG-SNAP (env-gated)
                    Race::Registry_MaybeReplay();     // [mode 7 M1] G-M1-REG / G-M1-PASS (env-gated)
                    Gameplay::Pickups_MaybeReplay();  // [mode 7 M2a] G-M2a-REPLAY (env-gated)
                    Gameplay::PickupCb_MaybeReplay(); // [mode 7 M3] G-M3-REPLAY (env-gated)
                }
                gates_.assign(aw.materials.size(), Gate{});
                std::vector<int> counts(aw.materials.size(), 0);
                // per-gate unique vertex indices in stream order — the
                // original keeps the quad's 4 verts as node corners j=0..3
                // (FUN_00426d00); we need j=0 and j=3 for the camera port.
                std::vector<std::vector<std::uint32_t>> gate_verts(
                    aw.materials.size());
                std::vector<const Track::Sector*> gate_sector(
                    aw.materials.size(), nullptr);
                for (const auto& s : aw.sectors) {
                    const std::size_t nt = s.tris.size() / 4;
                    for (std::size_t ti = 0; ti < nt; ++ti) {
                        const std::uint16_t mat = s.tris[ti * 4 + 0];
                        // gate index = material RED byte (== mat order here)
                        const std::uint8_t gi = aw.materials[mat].rgba[0];
                        if (gi >= gates_.size()) continue;
                        gate_sector[gi] = &s;
                        for (int k = 1; k <= 3; ++k) {
                            const std::uint16_t vi = s.tris[ti * 4 + k];
                            gates_[gi].center[0] += s.verts[vi * 3 + 0];
                            gates_[gi].center[1] += s.verts[vi * 3 + 1];
                            gates_[gi].center[2] += s.verts[vi * 3 + 2];
                            ++counts[gi];
                            auto& gv = gate_verts[gi];
                            bool seen = false;
                            for (auto u : gv) if (u == vi) { seen = true; break; }
                            if (!seen) gv.push_back(vi);
                        }
                    }
                }
                for (std::size_t gi = 0; gi < gates_.size(); ++gi)
                    if (counts[gi] > 0)
                        for (int k = 0; k < 3; ++k)
                            gates_[gi].center[k] /= static_cast<float>(counts[gi]);
                // corners (vertex order ascending = stream order) + race dir
                for (std::size_t gi = 0; gi < gates_.size(); ++gi) {
                    Gate& g = gates_[gi];
                    auto& gv = gate_verts[gi];
                    if (gv.size() < 4 || !gate_sector[gi]) continue;
                    std::sort(gv.begin(), gv.end());
                    const float* vs = gate_sector[gi]->verts.data();
                    for (int k = 0; k < 3; ++k) {
                        g.c0[k] = vs[gv[0] * 3 + k];
                        g.c3[k] = vs[gv[3] * 3 + k];
                    }
                    // dir = quad normal (cross of two edges from c0),
                    // oriented toward the next gate's center
                    const float e1[3] = {vs[gv[1] * 3 + 0] - g.c0[0],
                                         vs[gv[1] * 3 + 1] - g.c0[1],
                                         vs[gv[1] * 3 + 2] - g.c0[2]};
                    const float e2[3] = {vs[gv[2] * 3 + 0] - g.c0[0],
                                         vs[gv[2] * 3 + 1] - g.c0[1],
                                         vs[gv[2] * 3 + 2] - g.c0[2]};
                    float n3[3] = {e1[1] * e2[2] - e1[2] * e2[1],
                                   e1[2] * e2[0] - e1[0] * e2[2],
                                   e1[0] * e2[1] - e1[1] * e2[0]};
                    const float nl = std::sqrt(n3[0] * n3[0] + n3[1] * n3[1] +
                                               n3[2] * n3[2]);
                    if (nl > 0.f)
                        for (float& v : n3) v /= nl;
                    const Gate& gn = gates_[(gi + 1) % gates_.size()];
                    const float to_next[3] = {gn.center[0] - g.center[0],
                                              gn.center[1] - g.center[1],
                                              gn.center[2] - g.center[2]};
                    const float dot = n3[0] * to_next[0] + n3[1] * to_next[1] +
                                      n3[2] * to_next[2];
                    for (int k = 0; k < 3; ++k)
                        g.dir[k] = (dot < 0.f) ? -n3[k] : n3[k];
                }
                if (!ribbon_stream_order)
                    Race::PathNodes_CameraNodes(aw, cam_nodes_);   // [camribbon] dir / c0 / c3 per node
            }
            break;
        }
    }

    // RaceCamera wiring: gate ribbon -> camera-path nodes, plus the per-node
    // angle table from Common/LED.piz LE<Course_Id>.LED (see Race/RaceCamera).
    if (ribbon_stream_order) {
        for (const auto& g : gates_) {
            Race::RaceCamNode n{};
            for (int k = 0; k < 3; ++k) {
                n.dir[k] = g.dir[k];
                n.c0[k]  = g.c0[k];
                n.c3[k]  = g.c3[k];
            }
            cam_nodes_.push_back(n);
        }
        if (log)
            std::fprintf(log, "  [camribbon] MASHED_NO_CAM_RIBBON_FIX: camera ribbon corners from the "
                              "sorted gate vertices (previous behaviour), %zu nodes\n", cam_nodes_.size());
    }
    race_cam_.SetNodes(cam_nodes_.data(), static_cast<int>(cam_nodes_.size()));
    {
        // <...>/TRACKS/<track>.piz -> <...>/Common/LED.piz
        std::string led(piz_path);
        std::size_t cut = led.find_last_of("/\\");
        if (cut != std::string::npos) {
            cut = led.find_last_of("/\\", cut - 1);
            if (cut != std::string::npos) {
                led.resize(cut + 1);
                led += "Common\\LED.piz";
                race_cam_.LoadLed(led.c_str(), course_id_);
            }
        }
    }
    // [d1r2] MASHED_CAM_DUMP=<csv> also writes <csv>.nodes.txt: the camera-path ribbon
    // in cam_driver's nodes.txt format (dir, c0, c3), plus course id and LED state, so
    // a replay can be checked for running on the original's ribbon.
    if (const char* e = std::getenv("MASHED_CAM_DUMP")) {
        if (e[0]) {
            const std::string np = std::string(e) + ".nodes.txt";
            if (std::FILE* nf = std::fopen(np.c_str(), "w")) {
                for (const auto& n : cam_nodes_)
                    std::fprintf(nf, "%.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",
                                 n.dir[0], n.dir[1], n.dir[2], n.c0[0], n.c0[1], n.c0[2],
                                 n.c3[0], n.c3[1], n.c3[2]);
                std::fclose(nf);
            }
            // [camribbon] <csv>.ribbon.txt: which resolution built the ribbon, and per node whether
            // the LED table has an elevation (1) or NodeDir takes the c0/c3 fallback (0).
            int no_elev = 0;
            const std::string rp = std::string(e) + ".ribbon.txt";
            if (std::FILE* rf = std::fopen(rp.c_str(), "w")) {
                std::fprintf(rf, "# resolution=%s course_id=%d led_loaded=%d nodes=%zu\n",
                             ribbon_stream_order ? "stream-order(opt-out)" : "classified(FUN_00478660)",
                             course_id_, (int)race_cam_.has_led(), cam_nodes_.size());
                for (std::size_t i = 0; i < cam_nodes_.size(); ++i) {
                    const bool has = race_cam_.has_led_elev(static_cast<int>(i));
                    if (!has) ++no_elev;
                    std::fprintf(rf, "%zu %d\n", i, has ? 1 : 0);
                }
                std::fclose(rf);
            }
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "[d1r2] camera nodes=%zu course_id=%d led=%d -> %s\n",
                             cam_nodes_.size(), course_id_, (int)race_cam_.has_led(), np.c_str());
                std::fprintf(lf, "[camribbon] ribbon=%s nodes=%zu no_led_elev=%d -> %s\n",
                             ribbon_stream_order ? "stream-order(opt-out)" : "classified",
                             cam_nodes_.size(), no_elev, rp.c_str());
                std::fclose(lf);
            }
        }
    }

    // WS-AI-BRIDGE / Option B: load AI<course>.AI -> controller image so the faithful
    // racing-line lookahead (Ai_ComputeTarget) is available. Loaded by DEFAULT now (the
    // opponents drive the real line); MASHED_GATE_RIBBON_AI=1 forces the legacy gate-ribbon
    // scaffold (A/B hatch). A load failure also falls back to the gate ribbon.
    {
        static const bool s_gate_ribbon = (std::getenv("MASHED_GATE_RIBBON_AI") != nullptr);
        g_aib.course  = course_id_;
        g_aib.loaded  = s_gate_ribbon ? false : Ai_BridgeLoad(course_id_, piz_path);
    }

    // render-world soup for spawn validation (visible-surface heights)
    for (const auto& s : world.sectors) {
        const std::uint32_t vbase =
            static_cast<std::uint32_t>(rend_verts_.size() / 3);
        rend_verts_.insert(rend_verts_.end(), s.verts.begin(), s.verts.end());
        const std::size_t nt = s.tris.size() / 4;
        for (std::size_t ti = 0; ti < nt; ++ti) {
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 1]);
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 2]);
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 3]);
        }
    }

    center_[0] = (world.bbox[0] + world.bbox[3]) * 0.5f;
    center_[1] = (world.bbox[1] + world.bbox[4]) * 0.5f;
    center_[2] = (world.bbox[2] + world.bbox[5]) * 0.5f;
    const float dx = world.bbox[0] - world.bbox[3];
    const float dz = world.bbox[2] - world.bbox[5];
    radius_ = 0.5f * std::sqrt(dx * dx + dz * dz);

    // Orbit-camera focus from the AI gate ribbon (the raceable track). The raw
    // world bbox above is skewed by the skybox/backdrop, so its midpoint sits in
    // empty air well above the track and orbiting it frames mostly sky. The gate
    // centers trace the racing line, so their XZ centroid + max extent give a
    // vantage that keeps the track in frame and at the track surface Y.
    for (int k = 0; k < 3; ++k) track_center_[k] = center_[k];
    track_radius_ = radius_;
    if (gates_.size() >= 3) {
        float c[3] = {0.f, 0.f, 0.f};
        for (const auto& g : gates_)
            for (int k = 0; k < 3; ++k) c[k] += g.center[k];
        const float inv = 1.f / static_cast<float>(gates_.size());
        for (int k = 0; k < 3; ++k) c[k] *= inv;
        float rmax = 0.f;
        for (const auto& g : gates_) {
            const float gdx = g.center[0] - c[0];
            const float gdz = g.center[2] - c[2];
            const float r = std::sqrt(gdx * gdx + gdz * gdz);
            if (r > rmax) rmax = r;
        }
        for (int k = 0; k < 3; ++k) track_center_[k] = c[k];
        track_radius_ = (rmax > 1.f) ? rmax : radius_;
    }

    if (log) {
        std::size_t prop_inst = 0;
        for (const auto& p : props_) prop_inst += p.instances.size();
        std::fprintf(log, "R4 track load OK: %s — tris=%u verts=%u sectors=%zu "
                          "mats=%zu radius=%.2f props=%zu "
                          "(instances=%zu) gates=%zu\n",
                     piz_path, world.total_tris, world.total_verts,
                     world.sectors.size(), world.materials.size(),
                     radius_, props_.size(), prop_inst, gates_.size());
        std::fprintf(log, "  CAM-DIAG bbox=[%.1f,%.1f,%.1f .. %.1f,%.1f,%.1f] "
                          "bbox_center=(%.1f,%.1f,%.1f) bbox_R=%.1f | "
                          "gate_center=(%.1f,%.1f,%.1f) gate_R=%.1f\n",
                     world.bbox[0], world.bbox[1], world.bbox[2],
                     world.bbox[3], world.bbox[4], world.bbox[5],
                     center_[0], center_[1], center_[2], radius_,
                     track_center_[0], track_center_[1], track_center_[2],
                     track_radius_);
        std::fclose(log);
    }
    // E2'b step 3: hand the librw submitter the SAME parsed world + texture
    // dictionaries this function just used. Called here because `world` and
    // `dicts` are locals that die at the closing brace -- building the librw
    // scene from anywhere else would mean re-parsing GRAPH.BSP/TEXTURES.TXD into
    // a second copy that could disagree with this one. There is no D3D9 world to
    // fall back to (deleted in D1-residue R3, 2026-10-16), so a failed scene build
    // fails the load: a race with no world must not look like a loaded track.
    if (!LibRw::RaceSubmit_OnTrackLoaded(world, dicts.data(), dicts.size())) {
        if (std::FILE* ef = log_path ? std::fopen(log_path, "a") : nullptr) {
            std::fprintf(ef, "R4 track load FAILED: librw scene build failed "
                             "(see log/librw_race.txt)\n");
            std::fclose(ef);
        }
        return false;
    }
    {
        // [rstates] the course facts RenderWare's world pass depends on
        // (PREREG_rstates.md F3/F4). Arctic (Course_Id 0) is the one record whose slot 0
        // installs the world-sector callback 0x0049a0c0 (0x00448951/0x00448963, the only
        // callers), and that callback never sets the vertex-alpha flag. The draw before
        // its first sector is the slot-3 sky (RpClumpRender of the Sky_Filename clumps,
        // 0x00426d9d), so the flag the world starts with is that sky's last-mesh need.
        LibRw::RaceCourseFacts facts;
        facts.sector_keeps_vertex_alpha = (course_id_ == 0);
        facts.sky_last_vertex_alpha     = sky_.last_mesh_va;
        facts.rain                      = rain_enabled_;
        LibRw::RaceSubmit_SetCourseFacts(facts);
    }
    BuildPropOrder(log);

    ready_ = true;
    return true;
}

// [F2] Animated copters. COURSE.LUA wires them in two parts:
//   General_Anim_Filename( slot , "X.anm" )   slot -> flight-path .ANM (track piz)
//   SetCopter( "Copter"|"JetRanger" , slot )   scenery copter on that path
// and KTCSCRIPT.LUA adds the gameplay copter:
//   KTC_NewCopter( "KTC_Apache", slot )
// The named models (COPTER/JETRANGER/KTC_APACHE.DFF) + COPTERS.TXD live in the
// shared Common/Perm.piz, not the track piz. Each copter flies its bound HAnim
// path looped on the race clock (the .ANM durations are in seconds).
void TrackRenderer::LoadCopters(IDirect3DDevice9* dev, Piz::Archive& piz,
                                const char* piz_path, std::FILE* log) {
    auto basename = [](const char* n) -> const char* {
        const char* b = n;
        for (const char* p = n; *p; ++p)
            if (*p == '/' || *p == '\\') b = p + 1;
        return b;
    };
    auto find_blob = [&](Piz::Archive& a, const char* want_base,
                         std::uint32_t* len) -> const std::uint8_t* {
        for (std::uint32_t i = 0; i < a.count(); ++i)
            if (_stricmp(basename(a.entry(i).name), want_base) == 0)
                return a.blob(i, len);
        return nullptr;
    };

    // slot -> .anm filename; (model, slot) bindings
    char anm[16][64] = {};
    // gameplay=true: KTC_NewCopter (KTCSCRIPT.LUA, rule-5 feed); false:
    // SetCopter (COURSE.LUA, scenery-only, D-11056/U-8997).
    struct Bind { char model[32]; int slot; bool gameplay; };
    std::vector<Bind> binds;
    auto scan_lua = [&](const char* txt, std::size_t len) {
        std::size_t pos = 0;
        while (pos < len) {
            std::size_t eol = pos;
            while (eol < len && txt[eol] != '\n') ++eol;
            std::string line(txt + pos, eol - pos);
            pos = eol + 1;
            const std::size_t cmt = line.find("--");
            if (cmt != std::string::npos) line.resize(cmt);
            int slot = -1;
            char nm[64] = {};
            const char* g = std::strstr(line.c_str(), "General_Anim_Filename");
            const char* s = std::strstr(line.c_str(), "SetCopter");
            const char* k = std::strstr(line.c_str(), "KTC_NewCopter");
            if (g) {
                char f[64] = {};
                if (std::sscanf(g, "General_Anim_Filename( %d %*[ ,]\"%63[^\"]\"",
                                &slot, f) == 2 && slot >= 0 && slot < 16)
                    std::snprintf(anm[slot], sizeof(anm[slot]), "%s", f);
            } else if (s) {
                if (std::sscanf(s, "SetCopter( \"%31[^\"]\" %*[ ,]%d",
                                nm, &slot) == 2 && slot >= 0) {
                    Bind b{}; std::snprintf(b.model, sizeof(b.model), "%s", nm);
                    b.slot = slot; binds.push_back(b);
                }
            } else if (k) {
                if (std::sscanf(k, "KTC_NewCopter( \"%31[^\"]\" %*[ ,]%d",
                                nm, &slot) == 2 && slot >= 0) {
                    Bind b{}; std::snprintf(b.model, sizeof(b.model), "%s", nm);
                    b.slot = slot; b.gameplay = true; binds.push_back(b);
                }
            }
        }
    };
    std::uint32_t cl = 0;
    const std::uint8_t* cb = find_blob(piz, "COURSE.LUA", &cl);
    if (cb && cl) scan_lua(reinterpret_cast<const char*>(cb), cl);
    std::uint32_t kl = 0;
    const std::uint8_t* kb = find_blob(piz, "KTCSCRIPT.LUA", &kl);
    if (kb && kl) scan_lua(reinterpret_cast<const char*>(kb), kl);
    if (binds.empty()) return;

    // Common/Perm.piz (sibling of TRACKS/) — derived like LED.piz in Load().
    std::string perm(piz_path);
    std::size_t cut = perm.find_last_of("/\\");
    if (cut == std::string::npos) return;
    cut = perm.find_last_of("/\\", cut - 1);
    if (cut == std::string::npos) return;
    perm.resize(cut + 1);
    perm += "Common\\Perm.piz";
    Piz::Archive pp;
    if (!pp.Load(perm.c_str())) {
        if (log) std::fprintf(log, "  F2 copters: Perm.piz load FAILED (%s): %s\n",
                              perm.c_str(), pp.last_error());
        return;
    }
    std::vector<Txd::Dictionary> dicts(1);
    {
        std::uint32_t tl = 0;
        const std::uint8_t* tb = find_blob(pp, "COPTERS.TXD", &tl);
        if (tb) dicts[0].Decode(tb, tl);
    }

    // build a model DFF on demand, cached by "<NAME>.DFF"
    std::vector<std::string> model_names;   // parallel to copter_models_
    auto model_index = [&](const char* name) -> int {
        char up[40];
        std::size_t j = 0;
        for (; name[j] && j < sizeof(up) - 1; ++j)
            up[j] = static_cast<char>(::toupper(static_cast<unsigned char>(name[j])));
        up[j] = '\0';
        char want[48];
        std::snprintf(want, sizeof(want), "%s.DFF", up);
        for (std::size_t mi = 0; mi < model_names.size(); ++mi)
            if (model_names[mi] == want) return static_cast<int>(mi);
        std::uint32_t dl = 0;
        const std::uint8_t* db = find_blob(pp, want, &dl);
        if (!db) return -1;
        Track::DffModel m;
        if (!m.Parse(db, dl)) return -1;
        std::snprintf(m.source_name, sizeof(m.source_name), "%s", want);
        CopterModel cm;
        // [librwrest] librw with the frame hierarchy (lit atomics: rpGEOMETRYLIGHT + normals
        // reach librw's ambient / directional lights, as the original's draws run with
        // D3DRS_LIGHTING 1, verify/d1r1_20261016 copter draw records).
        cm.rw_model = LibRw::RaceSubmit_RegisterModel(m, dicts.data(), dicts.size(), amb_world_,
                                                      nullptr, 0, /*hierarchy=*/true);
        int natom = 0;
        for (const auto& b : m.batches) if (b.atomic + 1 > natom) natom = b.atomic + 1;
        auto atom_frame = [&](int stream_idx) {
            for (const auto& b : m.batches) if (b.atomic == stream_idx) return b.frame;
            return -1;
        };
        // RW clump order index i = stream atomic natom-1-i (FUN_004516d0 slots i = 0, 1, 2)
        cm.body_frame = atom_frame(natom - 1);
        cm.rotA_frame = atom_frame(natom - 2);
        cm.rotB_frame = atom_frame(natom - 3);
        auto bind12 = [&](int fi, float* o) {
            if (fi < 0 || fi >= static_cast<int>(m.frames.size())) return;
            std::memcpy(o, m.frames[static_cast<std::size_t>(fi)].rot, 9 * sizeof(float));
            std::memcpy(o + 9, m.frames[static_cast<std::size_t>(fi)].pos, 3 * sizeof(float));
        };
        bind12(cm.rotA_frame, cm.rotA_bind);
        bind12(cm.rotB_frame, cm.rotB_bind);
        // FUN_00425e40 @ 0x00425e40: frustum test (FUN_004c1b40) on the body (slot+4) atomic's world
        // sphere, then render slot+4, slot+0xc, slot+8 (d1r1 backtraces: returns 0x00425e78 / 7f / 86)
        const std::int32_t order[3] = { cm.body_frame, cm.rotB_frame, cm.rotA_frame };
        LibRw::RaceSubmit_SetModelDrawOrder(cm.rw_model, order, 3, cm.body_frame);
        // the original's copter draws run with the RW driver's states (d1r1 records: body
        // AT 0 / AB 0, blades AT 1 / AB 1 by texel alpha, AREF 0, AF 5, CULL 2)
        LibRw::RaceSubmit_SetModelRwStates(cm.rw_model);
        if (log) std::fprintf(log, "  [librwrest] copter %s: handle=%d atomics=%d frames body=%d "
                                   "rotA=%d rotB=%d\n", want, cm.rw_model, natom, cm.body_frame,
                              cm.rotA_frame, cm.rotB_frame);
        copter_models_.push_back(std::move(cm));
        model_names.emplace_back(want);
        return static_cast<int>(copter_models_.size()) - 1;
    };

    int loaded = 0;
    for (const auto& b : binds) {
        if (b.slot < 0 || b.slot >= 16 || !anm[b.slot][0]) continue;
        std::uint32_t al = 0;
        const std::uint8_t* ab = find_blob(piz, anm[b.slot], &al);
        if (!ab) continue;
        Track::HAnim ha;
        if (!ha.Parse(ab, al)) continue;
        const int mi = model_index(b.model);
        if (mi < 0) continue;
        AnimCopter c;
        c.model = mi;
        c.anim = std::move(ha);
        c.gameplay = b.gameplay;
        copters_.push_back(std::move(c));
        ++loaded;
    }
    if (log) {
        int gameplay_n = 0;
        for (const auto& c : copters_) if (c.gameplay) ++gameplay_n;
        std::fprintf(log,
            "  F2 copters: %d flying, %zu models from %s (%d gameplay/rule-5)\n",
            loaded, copter_models_.size(), perm.c_str(), gameplay_n);
    }
}

namespace {
// Vertical height query on a triangle soup: barycentric XZ test, plane height
// at (x,z); returns the HIGHEST surface (bridges: highest = drivable deck).
float HeightOnSoup(const std::vector<float>& verts,
                   const std::vector<std::uint32_t>& tris,
                   float x, float z, bool* ok) {
    float best = -1e9f;
    bool found = false;
    const std::size_t nt = tris.size() / 3;
    for (std::size_t i = 0; i < nt; ++i) {
        const float* a = &verts[tris[i * 3 + 0] * 3];
        const float* b = &verts[tris[i * 3 + 1] * 3];
        const float* c = &verts[tris[i * 3 + 2] * 3];
        const float d00x = b[0] - a[0], d00z = b[2] - a[2];
        const float d01x = c[0] - a[0], d01z = c[2] - a[2];
        const float den = d00x * d01z - d01x * d00z;
        if (den > -1e-9f && den < 1e-9f) continue;   // degenerate in XZ
        const float px = x - a[0], pz = z - a[2];
        const float u = (px * d01z - d01x * pz) / den;
        const float v = (d00x * pz - px * d00z) / den;
        if (u < 0.f || v < 0.f || u + v > 1.f) continue;
        const float y = a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]);
        if (y > best) { best = y; found = true; }
    }
    if (ok) *ok = found;
    return best;
}
}  // namespace

float TrackRenderer::GroundHeight(float x, float z, bool* ok) const {
    return HeightOnSoup(col_verts_, col_tris_, x, z, ok);
}

// 16-ray ring probe for an on-mesh escape heading, preferring the one nearest curYaw.
bool TrackRenderer::FindOnMeshHeading(float px, float pz, float curYaw,
                                     float radius, float& outYaw) const {
    const int   kRays = 16;
    const float kPi   = 3.14159265358979f;
    bool found = false; float bestDev = 1e9f, bestYaw = curYaw;
    for (int i = 0; i < kRays; ++i) {
        const float a = (2.f * kPi) * static_cast<float>(i) / static_cast<float>(kRays);
        bool ok = false;
        GroundHeight(px + std::cos(a) * radius, pz + std::sin(a) * radius, &ok);
        if (!ok) continue;
        float d = a - curYaw;
        while (d >  kPi) d -= 2.f * kPi;
        while (d < -kPi) d += 2.f * kPi;
        d = (d < 0.f) ? -d : d;
        if (d < bestDev) { bestDev = d; bestYaw = a; found = true; }
    }
    outYaw = bestYaw;
    return found;
}

// Gate-independent off-mesh recovery (called from the off-mesh branches of UpdateCar).
// car_pos_ is still the last ON-MESH point (the edge) — only the attempted next step
// went off. Find an on-mesh heading near the current one, nudge the position a little
// inward (committing only an on-mesh landing), and redirect heading + a modest forward
// speed. Guarantees the car never permanently freezes against an edge; the player's
// steering (or the chain) then carries it back to the interior.
void TrackRenderer::RecoverOffMesh() {
    float ry; float rad = 0.f;
    if      (FindOnMeshHeading(car_pos_[0], car_pos_[2], car_yaw_, 1.2f, ry)) rad = 1.2f;
    else if (FindOnMeshHeading(car_pos_[0], car_pos_[2], car_yaw_, 0.5f, ry)) rad = 0.5f;
    if (rad <= 0.f) {                 // fully stranded (isolated point) — rare
        car_vel_[0] = car_vel_[2] = 0.f; car_speed_ = 0.f; return;
    }
    float mx = car_pos_[0] + std::cos(ry) * 0.3f;
    float mz = car_pos_[2] + std::sin(ry) * 0.3f;
    bool mok = false; float my = GroundHeight(mx, mz, &mok);
    if (!mok) {                       // small step landed off; jump to the verified ring point
        mx = car_pos_[0] + std::cos(ry) * rad;
        mz = car_pos_[2] + std::sin(ry) * rad;
        my = GroundHeight(mx, mz, &mok);
    }
    if (mok) { car_pos_[0] = mx; car_pos_[2] = mz; car_pos_[1] = my + car_ground_off_; }
    const float sp = (car_speed_ > 4.f ? car_speed_ * 0.5f : 4.f);
    car_yaw_ = ry;
    // A8: re-seed the integrated body basis to the recovered heading.
    Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
    car_vel_[0] = std::cos(ry) * sp; car_vel_[2] = std::sin(ry) * sp;
    car_speed_  = sp;
}

float TrackRenderer::GroundProbe(float x, float z, bool* ok,
                                 float normal[3]) const {
    // Like GroundHeight but also returns the hit triangle's (upward) normal
    // for slope gravity.
    float best = -1e9f;
    bool found = false;
    normal[0] = 0.f; normal[1] = 1.f; normal[2] = 0.f;
    const std::size_t nt = col_tris_.size() / 3;
    for (std::size_t i = 0; i < nt; ++i) {
        const float* a = &col_verts_[col_tris_[i * 3 + 0] * 3];
        const float* b = &col_verts_[col_tris_[i * 3 + 1] * 3];
        const float* c = &col_verts_[col_tris_[i * 3 + 2] * 3];
        const float d00x = b[0] - a[0], d00z = b[2] - a[2];
        const float d01x = c[0] - a[0], d01z = c[2] - a[2];
        const float den = d00x * d01z - d01x * d00z;
        if (den > -1e-9f && den < 1e-9f) continue;
        const float px = x - a[0], pz = z - a[2];
        const float u = (px * d01z - d01x * pz) / den;
        const float v = (d00x * pz - px * d00z) / den;
        if (u < 0.f || v < 0.f || u + v > 1.f) continue;
        const float y = a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]);
        if (y > best) {
            best = y; found = true;
            const float e1[3] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
            const float e2[3] = {c[0]-a[0], c[1]-a[1], c[2]-a[2]};
            float n[3] = {e1[1]*e2[2] - e1[2]*e2[1],
                          e1[2]*e2[0] - e1[0]*e2[2],
                          e1[0]*e2[1] - e1[1]*e2[0]};
            const float l = std::sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
            if (l > 1e-6f) {
                const float s = (n[1] < 0.f ? -1.f : 1.f) / l;  // upward
                normal[0] = n[0]*s; normal[1] = n[1]*s; normal[2] = n[2]*s;
            }
        }
    }
    if (ok) *ok = found;
    return best;
}

// Drop a vehicle DFF's NON-RENDER atomics, and log the census at load so the
// two acceptance arms are comparable.
//
// A vehicle DFF ships non-render atomics alongside the painted body: four
// coarse untextured car-sized hulls plus 16-24 one- and two-triangle locator
// quads. Every one of them has no rpGEOMETRYTEXTURED (0x04) and no
// rpGEOMETRYTEXTURED2 (0x80) and material colour (102,102,102); the body,
// livery and glass atomics all declare one or two UV sets, so the predicate
// `(geo_flags & 0x84) == 0` selects exactly the non-render set. Measured on
// all 13 vehicle DFFs and all 3 liveries each
// (re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md M2/M4, re-derived by
// verify/car_gray_fix_20260930/dff_atomic_census.py): ADVANTAGE0..3 = 71
// atomics, 44 textured, 27 untextured (4 hulls of 40-60 tris + 23 one-tri
// locators). Rendering them covered the painted body with a faceted grey
// shell and ringed it with dark spikes.
//
// The ORIGINAL never renders the vehicle clump wholesale: FUN_00420420 @
// 0x00420420 enumerates the atomics (FUN_004b3fc0 @ 0x004b3fc0 ->
// RpClumpForAllAtomics @ 0x004e66d0) and scatters each into the per-player
// table at DAT_0063d9e0 + player*0x2AC indexed by the PART CODE from
// FUN_004b5190 @ 0x004b5190; parts the game never looks up are never drawn,
// and the four hulls (part codes 0x3b..0x3e) go to the BVH build at
// FUN_0053d090 @ 0x0053d090 / FUN_0053d400 @ 0x0053d400, i.e. they are
// collision geometry.
//
// [UNCERTAIN] This is a MEASURED data-driven equivalent of that selection,
// NOT a verbatim port: the part code is produced at runtime by a mechanism
// that is not derivable from the DFF (every frame/atomic extension chunk in
// ADVANTAGE0.DFF is zero length), which is tracker row U-9079. No hooks.csv
// row and no C-level follows from this function.
//
// model.bbox is deliberately NOT recomputed: the locator atomics extend it,
// and it feeds car_ground_off_ / car_len_ / car_height_ below, i.e. the chase
// camera rig and the ground offset. Leaving it alone keeps this purely visual.
//
// `kept`/`dropped` report what this loader hands downstream, NOT what the
// predicate merely selects, so an arm with the filter removed logs
// kept == atoms and dropped == 0.
static void CarDropNonRenderAtomics(Track::DffModel& model, const char* tag,
                                    std::FILE* log) {
    const std::size_t total = model.batches.size();
    model.batches.erase(
        std::remove_if(model.batches.begin(), model.batches.end(),
                       [](const Track::DffBatch& b) {
                           return (b.geo_flags & 0x84u) == 0;
                       }),
        model.batches.end());
    if (!log) return;
    std::size_t untex = 0;
    for (const auto& b : model.batches)
        if ((b.geo_flags & 0x84u) == 0) ++untex;
    std::fprintf(log,
                 "R5 car atomics: %s atoms=%zu kept=%zu dropped=%zu "
                 "untex_kept=%zu\n",
                 tag, total, model.batches.size(),
                 total - model.batches.size(), untex);
}

// [rtex 2026-10-16] FUN_00479100 @ 0x00479100 (via FUN_00426dc0): the nearest collision-world
// hit on segment a->b (FUN_004b4cd0 @ 0x004b4cd0; the same stand-in rule as
// Powerup/PowerupContact.cpp SegmentQuery: two-sided Moller-Trumbore over the collision soup,
// nearest t wins), then the hit triangle's prelit RED bytes (`& 0xff` of each RGBA dword,
// 0x00479100 decomp) times 1/255 ([0x005ceb90]) weighted by the hit point's barycentric
// coordinates, clamped to at most 1.0 ([0x005cc320]). A hit sector without colours gives
// 1.0. Returns false on no hit (the caller keeps its previous sample).
// [UNCERTAIN U-9697] FUN_00559040's weight matrix is not transcribed: it is replaced by the
// Moller-Trumbore barycentrics, equal in exact arithmetic; and the candidate set is the flat
// soup, not FUN_00538c80's BSP walk.
// [rfx 2026-10-16] `out_t`, when non-null, receives the hit's segment fraction: FUN_0041f590's
// second output ([esp+8], 0x0041f619), from which it stores rec+0x288 = 1 - t (0x0041f648).
static bool CollisionPrelitSample(const std::vector<float>& vtx, const std::vector<std::uint32_t>& tri,
                                  const std::vector<std::uint32_t>& prelit,
                                  const std::vector<std::uint8_t>& has_pl,
                                  const float a[3], const float b[3], float* out,
                                  float* out_t = nullptr) {
    const float d[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
    float best = 2.f, bu = 0.f, bw = 0.f;
    std::size_t bi = static_cast<std::size_t>(-1);
    const std::size_t nt = tri.size() / 3;
    for (std::size_t i = 0; i < nt; ++i) {
        const float* v0 = &vtx[tri[i * 3 + 0] * 3];
        const float* v1 = &vtx[tri[i * 3 + 1] * 3];
        const float* v2 = &vtx[tri[i * 3 + 2] * 3];
        const float e1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        const float e2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };
        const float p[3] = { d[1] * e2[2] - d[2] * e2[1], d[2] * e2[0] - d[0] * e2[2],
                             d[0] * e2[1] - d[1] * e2[0] };
        const float det = e1[0] * p[0] + e1[1] * p[1] + e1[2] * p[2];
        if (det > -1e-12f && det < 1e-12f) continue;
        const float inv = 1.0f / det;
        const float s[3] = { a[0] - v0[0], a[1] - v0[1], a[2] - v0[2] };
        const float u = (s[0] * p[0] + s[1] * p[1] + s[2] * p[2]) * inv;
        if (u < 0.0f || u > 1.0f) continue;
        const float q[3] = { s[1] * e1[2] - s[2] * e1[1], s[2] * e1[0] - s[0] * e1[2],
                             s[0] * e1[1] - s[1] * e1[0] };
        const float w = (d[0] * q[0] + d[1] * q[1] + d[2] * q[2]) * inv;
        if (w < 0.0f || u + w > 1.0f) continue;
        const float t = (e2[0] * q[0] + e2[1] * q[1] + e2[2] * q[2]) * inv;
        if (t < 0.0f || t > 1.0f || t >= best) continue;
        best = t; bu = u; bw = w; bi = i;
    }
    if (bi == static_cast<std::size_t>(-1)) return false;
    if (out_t) *out_t = best;
    if (bi >= has_pl.size() || !has_pl[bi]) { *out = 1.0f; return true; }
    const float r0 = static_cast<float>(prelit[tri[bi * 3 + 0]] & 0xff) * (1.0f / 255.0f);
    const float r1 = static_cast<float>(prelit[tri[bi * 3 + 1]] & 0xff) * (1.0f / 255.0f);
    const float r2 = static_cast<float>(prelit[tri[bi * 3 + 2]] & 0xff) * (1.0f / 255.0f);
    const float s = r0 * (1.0f - bu - bw) + r1 * bu + r2 * bw;
    *out = (s < 1.0f) ? s : 1.0f;
    return true;
}

namespace {
// [librwrest] RW row-vector affine 4x3 (rows right, up, at, pos): v' = v * R + p.
void AffMul(const float* a, const float* b, float* o) {   // a then b
    float t[12];
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            t[r * 3 + c] = a[r * 3 + 0] * b[0 + c] + a[r * 3 + 1] * b[3 + c] + a[r * 3 + 2] * b[6 + c];
    for (int c = 0; c < 3; ++c)
        t[9 + c] = a[9] * b[0 + c] + a[10] * b[3 + c] + a[11] * b[6 + c] + b[9 + c];
    std::memcpy(o, t, sizeof(t));
}
void AffInv(const float* a, float* o) {
    const double c00 = (double)a[4] * a[8] - (double)a[5] * a[7], c01 = (double)a[5] * a[6] - (double)a[3] * a[8],
                 c02 = (double)a[3] * a[7] - (double)a[4] * a[6];
    const double det = a[0] * c00 + a[1] * c01 + a[2] * c02;
    const double id = det != 0.0 ? 1.0 / det : 0.0;
    float r[9];
    r[0] = (float)(c00 * id); r[1] = (float)(((double)a[2] * a[7] - (double)a[1] * a[8]) * id);
    r[2] = (float)(((double)a[1] * a[5] - (double)a[2] * a[4]) * id);
    r[3] = (float)(c01 * id); r[4] = (float)(((double)a[0] * a[8] - (double)a[2] * a[6]) * id);
    r[5] = (float)(((double)a[2] * a[3] - (double)a[0] * a[5]) * id);
    r[6] = (float)(c02 * id); r[7] = (float)(((double)a[1] * a[6] - (double)a[0] * a[7]) * id);
    r[8] = (float)(((double)a[0] * a[4] - (double)a[1] * a[3]) * id);
    for (int c = 0; c < 3; ++c)
        o[9 + c] = -(a[9] * r[0 + c] + a[10] * r[3 + c] + a[11] * r[6 + c]);
    std::memcpy(o, r, sizeof(r));
}
void AffFromFrame(const Track::DffFrame& f, float* o) {
    std::memcpy(o, f.rot, 9 * sizeof(float));
    std::memcpy(o + 9, f.pos, 3 * sizeof(float));
}
}  // namespace

// [librwrest] load `dff_entry` from car_piz_ (plus every TXD in it), register it with librw
// with its frame hierarchy, bind the car dirt / env passes (rtex) to it, and record the frames
// FUN_0041f3d0 / FUN_0041f4f0 pose. Returns the index into car_models_, or -1. `out_bbox`
// (6 floats) receives the parsed model's bbox when non-null.
int TrackRenderer::RegisterCarModel(IDirect3DDevice9* dev, const char* dff_entry, std::FILE* log) {
    (void)dev;
    for (std::size_t i = 0; i < car_models_.size(); ++i)
        if (_stricmp(car_models_[i].dff, dff_entry) == 0) return static_cast<int>(i);
    Piz::Archive piz;
    if (!piz.Load(car_piz_)) {
        if (log) std::fprintf(log, "[librwrest] car model %s: piz %s FAILED: %s\n", dff_entry,
                              car_piz_, piz.last_error());
        return -1;
    }
    const std::uint8_t* dff = nullptr; std::uint32_t dff_len = 0;
    std::vector<std::pair<const std::uint8_t*, std::uint32_t>> txds;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (_stricmp(n, dff_entry) == 0) {
            dff = piz.blob(i, &dff_len);
        } else if (ln > 4 && _stricmp(n + ln - 4, ".TXD") == 0) {
            std::uint32_t bl = 0;
            const std::uint8_t* b = piz.blob(i, &bl);
            if (b) txds.emplace_back(b, bl);
        }
    }
    if (!dff) {
        if (log) std::fprintf(log, "[librwrest] car model %s: DFF entry not found\n", dff_entry);
        return -1;
    }
    Track::DffModel model;
    if (!model.Parse(dff, dff_len)) {
        if (log) std::fprintf(log, "[librwrest] car model %s: %s\n", dff_entry, model.last_error());
        return -1;
    }
    // [rfx 2026-10-16] the headlight beam's two frames, parts 25 / 28 (FUN_0041faf0 reads
    // rec+0x64 / rec+0x70; FUN_00420420 stores each atomic at rec + part*4). Taken BEFORE
    // CarDropNonRenderAtomics: both are one-triangle frame markers it removes.
    {
        bool h25 = false, h28 = false;
        for (const auto& b : model.batches) {
            if (b.part_id == 25 && !h25) { std::memcpy(car_part25_, b.frame_pos, sizeof(car_part25_)); h25 = true; }
            if (b.part_id == 28 && !h28) { std::memcpy(car_part28_, b.frame_pos, sizeof(car_part28_)); h28 = true; }
        }
        car_beam_parts_ok_ = h25 && h28;
    }
    CarDropNonRenderAtomics(model, dff_entry, log);
    std::vector<Txd::Dictionary> dicts(txds.size());
    for (std::size_t di = 0; di < txds.size(); ++di)
        dicts[di].Decode(txds[di].first, txds[di].second);

    CarModel cm;
    std::snprintf(cm.dff, sizeof(cm.dff), "%s", dff_entry);
    cm.handle = LibRw::RaceSubmit_RegisterModel(model, dicts.data(), dicts.size(), amb_world_,
                                                nullptr, 0, /*hierarchy=*/true);
    // FUN_0041f3d0 @ 0x0041f3d0: wheel w's atomic is rec[w] (parts 0..3, rec+0x298 == 0) or
    // rec[0x21 + w] (rec+0x298 == 1); it rotates the atomic's frame (atomic+4) and that
    // frame's parent (frame+4), whose position it restores.
    // FUN_0041f4f0 @ 0x0041f4f0: part 0x13's atomic frame (rec+0x4c -> +4), position restored.
    for (int k = 0; k < 8; ++k) {
        cm.wheel_frame[k] = cm.hub_frame[k] = -1;
        cm.hub_pos[k][0] = cm.hub_pos[k][1] = cm.hub_pos[k][2] = 0.f;
    }
    const int nfr = static_cast<int>(model.frames.size());
    for (const auto& b : model.batches) {
        if (b.frame < 0 || b.frame >= nfr) continue;
        int k = -1;
        if (b.part_id >= 0 && b.part_id <= 3) k = b.part_id;
        else if (b.part_id >= 0x21 && b.part_id <= 0x24) k = 4 + (b.part_id - 0x21);
        if (k >= 0 && cm.wheel_frame[k] < 0) {
            cm.wheel_frame[k] = b.frame;
            const int par = model.frames[static_cast<std::size_t>(b.frame)].parent;
            if (par >= 0) {
                cm.hub_frame[k] = par;
                std::memcpy(cm.hub_pos[k], model.frames[static_cast<std::size_t>(par)].pos,
                            sizeof(cm.hub_pos[k]));
            }
        }
        if (b.part_id == 0x13 && cm.body_frame < 0) {
            cm.body_frame = b.frame;
            std::memcpy(cm.body_pos, model.frames[static_cast<std::size_t>(b.frame)].pos,
                        sizeof(cm.body_pos));
        }
    }
    // diagnostic replay chains (TrackRenderer.h): part 0xe is the renderable whose frame LTM
    // camera_probe.py --carframes records as F{i}_* (rec+0x38 = 0x0063da18 - 0x0063d9e0)
    if (cm.body_frame >= 0) {
        int fe = -1;
        for (const auto& b : model.batches)
            if (b.part_id == 0x0e && b.frame >= 0 && b.frame < nfr) { fe = b.frame; break; }
        float X[12] = { 1,0,0, 0,1,0, 0,0,1, 0,0,0 }, Y[12] = { 1,0,0, 0,1,0, 0,0,1, 0,0,0 };
        bool under = false;
        for (int i = fe; i >= 0; i = model.frames[static_cast<std::size_t>(i)].parent) {
            if (i == cm.body_frame) { under = true; break; }
            float M[12];
            AffFromFrame(model.frames[static_cast<std::size_t>(i)], M);
            AffMul(X, M, X);
        }
        for (int i = model.frames[static_cast<std::size_t>(cm.body_frame)].parent;
             i >= 0 && model.frames[static_cast<std::size_t>(i)].parent >= 0;
             i = model.frames[static_cast<std::size_t>(i)].parent) {
            float M[12];
            AffFromFrame(model.frames[static_cast<std::size_t>(i)], M);
            AffMul(Y, M, Y);
        }
        cm.body_chain_ok = under;
        std::memcpy(cm.body_X, X, sizeof(X));
        std::memcpy(cm.body_Y, Y, sizeof(Y));
    }
    if (log) {
        std::fprintf(log, "[librwrest] car model %s: handle=%d frames=%d body_frame=%d wheels=",
                     dff_entry, cm.handle, nfr, cm.body_frame);
        for (int k = 0; k < 8; ++k) std::fprintf(log, "%d/%d ", cm.wheel_frame[k], cm.hub_frame[k]);
        std::fprintf(log, "\n");
    }
    if (cm.handle < 0) return -1;
    // [rfx 2026-10-16] the car's descriptor family for the shadow tables, and the beam parts
    // captured before CarDropNonRenderAtomics above (LibRw/RwRaceFx.h).
    car_family_ = LibRw::RaceFx_FamilyOfPiz(car_piz_);
    LibRw::RaceFx_SetCarModel(car_family_, car_part25_, car_part28_, car_beam_parts_ok_);
    for (float& f : car_fade_) f = 0.f;   // FUN_00420420 zero-clears the record (FUN_004b6520, 0x00420457)
    // [rtex 2026-10-16] the car material passes (U-9693, LibRw/RwCarFx.h), per model.
    {
        char base[64] = {};
        {
            const char* s = std::strrchr(car_piz_, '/');
            const char* s2 = std::strrchr(car_piz_, '\\');
            if (s2 && (!s || s2 > s)) s = s2;
            s = s ? s + 1 : car_piz_;
            std::size_t n = 0;
            while (s[n] && s[n] != '.' && n + 1 < sizeof(base)) { base[n] = s[n]; ++n; }
        }
        const int veh = LibRw::CarVehicleGroupId(base);
        // FUN_0040cea0 @ 0x0040cea0: dirt.piz, FUN_00404e00(course) / FUN_00404e20(course, veh)
        const char* bn = LibRw::CarDirtBodyName(course_id_);
        const char* wn = LibRw::CarDirtWheelName(course_id_, veh);
        if (LibRw::CarDirtEnabled() && bn && wn) {
            Piz::Archive dp;
            std::uint8_t* bimg = nullptr;
            std::uint8_t* wimg = nullptr;
            std::uint32_t bw = 0, bh = 0, ww = 0, wh = 0;
            if (dp.Load("original/TOASTART/VEHICLES/dirt.piz")) {
                char want[48];
                for (std::uint32_t i = 0; i < dp.count(); ++i) {
                    const char* en = dp.entry(i).name;
                    std::uint32_t bl = 0;
                    std::snprintf(want, sizeof(want), "%s.PNG", bn);
                    if (!bimg && _stricmp(en, want) == 0)
                        if (const std::uint8_t* b = dp.blob(i, &bl))
                            bimg = DecodeImageToBGRA(b, bl, &bw, &bh);
                    std::snprintf(want, sizeof(want), "%s.PNG", wn);
                    if (!wimg && _stricmp(en, want) == 0)
                        if (const std::uint8_t* b = dp.blob(i, &bl))
                            wimg = DecodeImageToBGRA(b, bl, &ww, &wh);
                }
            }
            // filterAddressing 0x1101 (NEAREST, WRAP, WRAP) read live off both original
            // dirt textures (+0x50), PREREG_rtex amendment 1
            const int nb = (bimg && wimg)
                ? LibRw::RaceSubmit_BindCarDirt(cm.handle, bimg, bw, bh, wimg, ww, wh, 0x1101u)
                : 0;
            if (log)
                std::fprintf(log, "[rtex] car dirt: course=%d veh=%d body=%s wheel=%s bound=%d\n",
                             course_id_, veh, bn, wn, nb);
            std::free(bimg);
            std::free(wimg);
        }
        // FUN_00420420 @ 0x00420420: env texture = the vehicle table's +0x30 name, `cargloss`
        // for all 78 entries (0x005f37a8 + i*0x84 + 0x30)
        if (LibRw::CarEnvEnabled()) {
            const int ne = LibRw::RaceSubmit_SetupCarEnv(cm.handle, dicts.data(),
                                                          dicts.size(), "cargloss");
            cm.env_ok = ne > 0;
            if (log) std::fprintf(log, "[rtex] car env: %d materials\n", ne);
        }
    }
    car_models_.push_back(cm);
    return static_cast<int>(car_models_.size()) - 1;
}

// [librwrest] FUN_0040d110 @ 0x0040d110 per car slot: livery = DAT_007f1a1c[i*4] (the
// per-car colour this renderer mirrors as colour_), or i when FUN_0042f6a0 (DAT_0067e9fc)
// == 0xb (0x0040d199..0x0040d1a6). The vehicle table entry's DFF is name + livery.
void TrackRenderer::ResolveCarLiveries() {
    livery_dirty_ = false;
    if (car_models_.empty() || !car_dff_base_[0]) return;
    // MASHED_CAR_LIVERY=0: every slot keeps slot 0's model (the pre-librwrest behaviour)
    static const bool s_livery_off = [] {
        const char* e = std::getenv("MASHED_CAR_LIVERY");
        return e && e[0] == '0' && e[1] == '\0';
    }();
    if (s_livery_off) return;
    std::FILE* log = std::fopen("mashed_re.log", "a");
    const int mode = *reinterpret_cast<const int*>(0x0067e9fcu);   // FUN_0042f6a0
    for (int s = 0; s < 4; ++s) {
        int liv = (mode == 0xb) ? s : colour_[s];
        if (liv < 0 || liv > 5) liv = 0;
        char dff[48];
        std::snprintf(dff, sizeof(dff), "%s%d.DFF", car_dff_base_, liv);
        const int mi = RegisterCarModel(nullptr, dff, log);
        car_slot_model_[s] = (mi >= 0) ? mi : 0;
        if (log) std::fprintf(log, "[librwrest] livery: slot %d mode=%d colour=%d -> %s (model %d)\n",
                              s, mode, colour_[s], dff, car_slot_model_[s]);
    }
    if (log) std::fclose(log);
}

// [librwrest] FUN_00420870 @ 0x00420870's part inputs, at FUN_0040fc00's call of FUN_0040da50
// (0x0040fc75: before this frame's physics, 0x0040fc9b), for every car FUN_0040da50 updates.
// MASHED_CAR_PARTS=0 skips it (the cars then draw in the streamed pose, as before).
static bool CarPartsEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_CAR_PARTS");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    return on;
}

// FUN_0041f3d0 @ 0x0041f3d0's spin step for one wheel (0x0041f42e..0x0041f474): fild units;
// fmul [0x005cc948] (1/3000); fimul spin (0x0041f434); fadd [angle]; fst; then the two folds. The
// race loop runs at x87 PC_24, so each step rounds to single precision: float steps here.
// Shared by CarParts_FrameTick and the W1 law replay (units >= 0, so no 2^32 fix-up).
static float CarPartsSpinStep(float ang, int units, std::int32_t spin) {
    std::uint32_t kb = 0x39aec33eu;                     // _DAT_005cc948 (1/3000), .rdata
    float k3000;
    std::memcpy(&k3000, &kb, sizeof k3000);
    float f = static_cast<float>(units) * k3000;
    f = f * static_cast<float>(spin);
    f = f + ang;
    float a = f;
    if (f > 360.0f) a = f - 360.0f;                     // 0x0041f440..0x0041f45b
    if (a < -360.0f) a = a + 360.0f;                    // 0x0041f45d..0x0041f474
    return a;
}

void TrackRenderer::CarParts_FrameTick(int units) {
    if (!CarPartsEnabled()) return;
    for (int v = 0; v < 4; ++v) {
        // FUN_0040da50 @ 0x0040da50: *(PTR_005f2770 + 0x34 + v*4) != 0, then FUN_0046c7b0(v) == 1
        // (the FUN_0046c770(v) == 1 arm is not bound, as CarRegistrant.cpp, U-9210)
        if (IsCarSlotActive(v) == 0) continue;
        if (VehicleSlotGetter(static_cast<std::uint32_t>(v)) != 1u) continue;
        const char* r = Vehicle::VehRecord(static_cast<std::uint32_t>(v));
        if (!r) continue;
        CarPartState& cp = car_parts_[v];
        for (int w = 0; w < 4; ++w) {
            const char* wb = r + 0x1a4 + w * 0xc4;    // FUN_0046bd60: 0x00881744 + (v*0x11 + w)*0xc4
            std::int32_t spin;
            std::memcpy(&spin, wb, 4);
            cp.ang[w] = CarPartsSpinStep(cp.ang[w], units, spin);
            // FUN_0046bda0: 16 floats from +0x1ac, summed in order from 0.0, times [0x005cd058]
            float acc = 0.0f;
            for (int k = 0; k < 16; ++k) {
                float s;
                std::memcpy(&s, wb + 8 + k * 4, 4);
                acc += s;
            }
            cp.steer[w] = acc * 0.0625f;
        }
        std::memcpy(cp.va, r + 0xaf4, 12);   // FUN_0046bce0: 0x00882094 + v*0xd04
        std::memcpy(cp.vb, r + 0xb00, 12);   // FUN_0046bd20: 0x008820a0 + v*0xd04
        cp.ok = true;
    }
}

namespace {
// RwMatrix (16 floats: right, flags, up, pad, at, pad, pos, pad) -> FramePose rows
void PoseFromRwMatrix(const float* m, int frame, LibRw::FramePose* out) {
    out->frame = frame;
    out->m[0] = m[0];  out->m[1] = m[1];  out->m[2] = m[2];
    out->m[3] = m[4];  out->m[4] = m[5];  out->m[5] = m[6];
    out->m[6] = m[8];  out->m[7] = m[9];  out->m[8] = m[10];
    out->m[9] = m[12]; out->m[10] = m[13]; out->m[11] = m[14];
}

// The frame matrices FUN_00420870 writes for one car, from its part inputs. Both LOD sets
// (parts 0..3 and 0x21..0x24) are posed: the original poses only the set rec+0x298 selects
// and hides the other through FUN_004b5240 (the car LOD visibility, not ported here), so
// posing both draws the same as posing the visible one. Returns the pose count.
int BuildCarPoses(const TrackRenderer::CarModel& cm, const TrackRenderer::CarPartState& cp,
                  LibRw::FramePose* out) {
    static const float kX[3] = { 1.f, 0.f, 0.f };   // DAT_006146f0
    static const float kY[3] = { 0.f, 1.f, 0.f };   // DAT_006146fc
    static const float kZ[3] = { 0.f, 0.f, 1.f };   // DAT_00614708
    int n = 0;
    for (int k = 0; k < 8; ++k) {
        const int w = k & 3;
        float m[16];
        if (cm.wheel_frame[k] >= 0) {
            // FUN_0041f3d0: RwFrameRotate(wheel frame, X, rec+0x220+w*4, rwCOMBINEREPLACE)
            RwMatrixRotate(m, kX, cp.ang[w], 0);
            PoseFromRwMatrix(m, cm.wheel_frame[k], &out[n++]);
        }
        if (cm.hub_frame[k] >= 0) {
            // FUN_0041f3d0: RwFrameRotate(parent, Y, FUN_0046bda0(v, w), REPLACE), then
            // RwFrameTranslate(parent, its saved position, rwCOMBINEPOSTCONCAT)
            RwMatrixRotate(m, kY, cp.steer[w], 0);
            float p[3] = { cm.hub_pos[k][0], cm.hub_pos[k][1], cm.hub_pos[k][2] };
            FUN_004c51a0(m, p, 2);
            PoseFromRwMatrix(m, cm.hub_frame[k], &out[n++]);
        }
    }
    if (cm.body_frame >= 0) {
        // FUN_0041f4f0: RwFrameRotate(part 0x13 frame, Z, (b.x + a.x) * [0x005cd274] (40), REPLACE),
        // RwFrameRotate(.., X, (b.z + a.z) * [0x005cd038] (-100), POSTCONCAT),
        // RwFrameTranslate(.., saved position, POSTCONCAT)
        float m[16];
        RwMatrixRotate(m, kZ, (cp.vb[0] + cp.va[0]) * 40.0f, 0);
        RwMatrixRotate(m, kX, (cp.vb[2] + cp.va[2]) * -100.0f, 2);
        float p[3] = { cm.body_pos[0], cm.body_pos[1], cm.body_pos[2] };
        FUN_004c51a0(m, p, 2);
        PoseFromRwMatrix(m, cm.body_frame, &out[n++]);
    }
    return n;
}
}  // namespace

// [librwrest_checks] W1 law replay. Input, one record per line (whitespace separated):
//   row car units a0 a1 a2 a3 spin0..3 steer0..3 va0 va1 va2 vb0 vb1 vb2
// a0..a3 = the original's recorded angle at the previous row (rec+0x220 + w*4), spin = the ints
// FUN_0046bd60 reads (veh+0x1a4 + w*0xc4), steer = FUN_0046bda0's mean, va / vb = veh+0xaf4 /
// veh+0xb00. Output: "A row car ang0..3" (the angles CarPartsSpinStep returns), then one line per
// pose BuildCarPoses writes for that car's livery model: "W|H row car k m0..m11" (wheel / hub
// frame of part slot k, 0..3 = parts 0..3, 4..7 = parts 0x21..0x24) and "B row car m0..m11" (part
// 0x13's frame). Matrices are the RwFrame modelling rows right, up, at, pos.
void TrackRenderer::CarPartsLawReplay() {
    static bool s_done = false;
    if (s_done || car_models_.empty()) return;
    char in_path[MAX_PATH] = {}, out_path[MAX_PATH] = {};
    if (!GetEnvironmentVariableA("MASHED_CARPARTS_LAW", in_path, MAX_PATH) ||
        !GetEnvironmentVariableA("MASHED_CARPARTS_LAW_OUT", out_path, MAX_PATH)) {
        s_done = true;
        return;
    }
    s_done = true;
    std::FILE* fi = std::fopen(in_path, "r");
    std::FILE* fo = std::fopen(out_path, "w");
    if (!fi || !fo) {
        if (fi) std::fclose(fi);
        if (fo) std::fclose(fo);
        return;
    }
    int nrec = 0;
    char line[1024];
    while (std::fgets(line, sizeof(line), fi)) {
        int row = 0, car = 0, units = 0;
        float a[4], t[4], va[3], vb[3];
        int spin[4];
        const int got = std::sscanf(line,
            "%d %d %d %f %f %f %f %d %d %d %d %f %f %f %f %f %f %f %f %f %f",
            &row, &car, &units, &a[0], &a[1], &a[2], &a[3], &spin[0], &spin[1], &spin[2], &spin[3],
            &t[0], &t[1], &t[2], &t[3], &va[0], &va[1], &va[2], &vb[0], &vb[1], &vb[2]);
        if (got != 21 || car < 0 || car > 3) continue;
        const CarModel& cm = car_models_[static_cast<std::size_t>(car_slot_model_[car])];
        CarPartState cp;
        for (int w = 0; w < 4; ++w) {
            cp.ang[w] = CarPartsSpinStep(a[w], units, spin[w]);
            cp.steer[w] = t[w];
        }
        std::memcpy(cp.va, va, sizeof va);
        std::memcpy(cp.vb, vb, sizeof vb);
        cp.ok = true;
        LibRw::FramePose poses[20];
        const int np = BuildCarPoses(cm, cp, poses);
        std::fprintf(fo, "A %d %d %.9g %.9g %.9g %.9g\n", row, car,
                     cp.ang[0], cp.ang[1], cp.ang[2], cp.ang[3]);
        // BuildCarPoses' own order: per k a wheel pose (wheel_frame[k] >= 0) then a hub pose
        // (hub_frame[k] >= 0), then part 0x13's pose (body_frame >= 0)
        int p = 0;
        auto emit = [&](char kind, int k) {
            if (p >= np) return;
            if (kind == 'B') std::fprintf(fo, "B %d %d", row, car);
            else std::fprintf(fo, "%c %d %d %d", kind, row, car, k);
            for (int e = 0; e < 12; ++e) std::fprintf(fo, " %.9g", poses[p].m[e]);
            std::fprintf(fo, "\n");
            ++p;
        };
        for (int k = 0; k < 8; ++k) {
            if (cm.wheel_frame[k] >= 0) emit('W', k);
            if (cm.hub_frame[k] >= 0) emit('H', k);
        }
        if (cm.body_frame >= 0) emit('B', 0);
        if (p != np) std::fprintf(fo, "ERR %d %d poses %d emitted %d\n", row, car, np, p);
        ++nrec;
    }
    std::fclose(fi);
    std::fclose(fo);
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[librwrest_checks] MASHED_CARPARTS_LAW: %d records %s -> %s\n", nrec,
                     in_path, out_path);
        std::fclose(lf);
    }
}

namespace {
// [librwrest] sin by the polynomial FUN_004752f0 / FUN_00546e70 use: a + a^3 * (((((c908 a^2 -
// c904) a^2 + c900) a^2 - c8fc) a^2 + c8f8) a^2 - c8f4), constants [0x005cc8f4..0x005cc908].
float AnimSinPoly(float a) {
    const float a2 = a * a;
    return a2 * a * (((((1.58969102e-10f * a2 - 2.50507597e-08f) * a2 + 2.75573143e-06f) * a2 -
                       0.000198412701f) * a2 + 0.00833333377f) * a2 - 0.166666672f) + a;
}
// FUN_00546e70 @ 0x00546e70's acos of the (non-negative) quaternion dot: the rational form with
// the constants at 0x005d8dc4..0x005d8dec (pio2_hi / pio2_lo, pS0..pS5, qS1..qS4).
float AnimAcos(float x) {
    auto P = [](float z) {
        return (((((3.47933092e-05f * z + 0.000791535014f) * z - 0.0400555357f) * z +
                  0.201212525f) * z - 0.325565815f) * z + 0.166666672f) * z;
    };
    auto Q = [](float z) {
        return (((z * 0.077038154f - 0.68828398f) * z + 2.02094579f) * z - 2.40339494f) * z + 1.0f;
    };
    std::uint32_t xb;
    std::memcpy(&xb, &x, 4);
    xb &= 0x7fffffffu;
    if (xb >= 0x3f800000u) return 0.0f;                         // |x| >= 1 (x >= 0 here): 0.0
    if (xb < 0x3f000000u) {                                     // |x| < 0.5
        if (xb < 0x23000001u) return 1.5707964f;
        const float z = x * x;
        return 1.57079625f - (x - (7.54978942e-08f - P(z) / Q(z) * x));
    }
    const float z = (1.0f - x) * 0.5f;                          // x > 0.5
    const float s = std::sqrt(z);                                // FUN_004c3b30
    float df = s;
    std::uint32_t db;
    std::memcpy(&db, &df, 4);
    db &= 0xfffff000u;
    std::memcpy(&df, &db, 4);
    const float w = (z - df * df) / (df + s) + P(z) / Q(z) * s + df;
    return w + w;
}

// FUN_00532a60 @ 0x00532a60: keyframe quaternion (x, y, z, w) and translation -> RwMatrix.
void AnimKeyMatrix(const Track::HKeyFrame& k, float* m) {
    const float x = k.q[0], y = k.q[1], z = k.q[2], w = k.q[3];
    float t;
    t = z * z + y * y; m[0] = 1.0f - (t + t);
    t = y * x + w * z; m[1] = t + t;
    t = z * x - w * y; m[2] = t + t;
    t = y * x - w * z; m[4] = t + t;
    t = z * z + x * x; m[5] = 1.0f - (t + t);
    t = w * x + z * y; m[6] = t + t;
    t = w * y + z * x; m[8] = t + t;
    t = z * y - w * x; m[9] = t + t;
    t = y * y + x * x; m[10] = 1.0f - (t + t);
    std::uint32_t fl = 3u;                                      // matrix flags = 3
    std::memcpy(&m[3], &fl, 4);
    m[7] = m[11] = m[15] = 0.f;
    m[12] = k.t[0]; m[13] = k.t[1]; m[14] = k.t[2];
}

// FUN_004752f0 @ 0x004752f0: the HAnim matrix at time t (keyframe search, slerp of the
// bracketing quaternions with FUN_00546e70's set-up, the cubic position blend of FUN_00482ae0
// over the neighbouring keys, wrapping to the last / first key at the ends).
// The race loop runs at x87 PC_24: single-precision steps throughout.
void CopterAnimMatrix(const Track::HAnim& an, float t, float* m) {
    const std::vector<Track::HKeyFrame>& F = an.frames;
    const int n = static_cast<int>(F.size());
    if (n == 0) { for (int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.f : 0.f; return; }
    if (t <= 0.0f) { AnimKeyMatrix(F[0], m); return; }                // first key
    if (!(t < an.duration) || n < 2) { AnimKeyMatrix(F[n - 1], m); return; }   // last key
    int k = 0;
    while (k + 1 < n - 1 && F[k + 1].time < t) ++k;                   // first k with key k+1 >= t
    const Track::HKeyFrame& p1 = F[k];
    const Track::HKeyFrame& p2 = F[k + 1];
    const Track::HKeyFrame& p0 = (k + 1 < 2) ? F[n - 1] : F[k - 1];
    const Track::HKeyFrame& p3 = (k + 1 < n - 2) ? F[k + 2] : F[0];
    const float u = (t - p1.time) / (p2.time - p1.time);
    // FUN_00546e70: slerp set-up (hemisphere, theta, 1/sin(theta) pre-scale)
    float a[4] = { p1.q[0], p1.q[1], p1.q[2], p1.q[3] }, b[4];
    float dot = p1.q[3] * p2.q[3] + p1.q[2] * p2.q[2] + p1.q[0] * p2.q[0] + p1.q[1] * p2.q[1];
    if (dot >= 0.0f) {
        if (dot > 1.0f) dot = 1.0f;
        for (int j = 0; j < 4; ++j) b[j] = p2.q[j];
    } else {
        dot = (dot >= -1.0f) ? -dot : 1.0f;
        for (int j = 0; j < 4; ++j) b[j] = -p2.q[j];
    }
    const float theta = AnimAcos(dot);
    const bool lerp = 0.999989986f <= dot;                            // [0x005e4888]
    if (!lerp) {
        const float s = 1.0f / AnimSinPoly(theta);
        for (int j = 0; j < 4; ++j) { a[j] *= s; b[j] *= s; }
    }
    float q[4];
    if (u > 0.0f) {
        if (u < 1.0f) {
            float fa = 1.0f - u, fb = u;
            if (!lerp) { fa = AnimSinPoly(fa * theta); fb = AnimSinPoly(theta * u); }
            q[0] = b[0] * fb + a[0] * fa;
            q[1] = b[1] * fb + a[1] * fa;
            q[2] = b[2] * fb + a[2] * fa;
            q[3] = a[3] * fa + b[3] * fb;
        } else {
            for (int j = 0; j < 4; ++j) q[j] = p2.q[j];
        }
    } else {
        for (int j = 0; j < 4; ++j) q[j] = p1.q[j];
    }
    // FUN_00482ae0 (Vehicle/SplineCubicBlend.cpp, verbatim) on the four keys' translations
    float pos[3];
    SplineCubicBlend3(u, pos, const_cast<float*>(p0.t), const_cast<float*>(p1.t),
                      const_cast<float*>(p2.t), const_cast<float*>(p3.t));
    // matrix from q scaled by [0x005cc574] (2) / |q|^2, then RwMatrixTranslate (0x004c51a0)(pos,
    // rwCOMBINEPOSTCONCAT) onto a zero translation
    const float s = 2.0f / (q[3] * q[3] + q[2] * q[2] + q[0] * q[0] + q[1] * q[1]);
    const float sx = q[0] * s, sy = s * q[1], sz = s * q[2];
    m[0] = 1.0f - (q[2] * sz + sy * q[1]);
    m[1] = q[3] * sz + sy * q[0];
    m[2] = sx * q[2] - sy * q[3];
    std::uint32_t fl = 3u;
    std::memcpy(&m[3], &fl, 4);
    m[4] = sy * q[0] - q[3] * sz;
    m[5] = 1.0f - (q[2] * sz + sx * q[0]);
    m[6] = sz * q[1] + sx * q[3];
    m[8] = sx * q[2] + sy * q[3];
    m[9] = sz * q[1] - sx * q[3];
    m[10] = 1.0f - (sy * q[1] + sx * q[0]);
    m[7] = m[11] = m[15] = 0.f;
    m[12] = m[13] = m[14] = 0.f;
    FUN_004c51a0(m, pos, 2);
}
}  // namespace

// [librwrest] FUN_00425d50 @ 0x00425d50, from FUN_0040fc00 (0x0040fcb9) with the frame's
// units: per copter, slot+0x40 += units * [0x005cc948], less the duration when past it; the
// two rotors turn by units * [0x005cc9a0] (0.05) and units * [0x005cd054] (-0.05) degrees
// (FUN_00425ca0 @ 0x00425ca0: about each rotor frame's own up vector, at its own position).
void TrackRenderer::CopterTick(int units) {
    std::uint32_t kb = 0x39aec33eu;                     // _DAT_005cc948 (1/3000)
    float k3000;
    std::memcpy(&k3000, &kb, sizeof k3000);
    const float fu = static_cast<float>(units);
    const float dt = fu * k3000;
    for (auto& c : copters_) {
        if (c.anim.frames.empty()) continue;
        const float e = dt + c.elapsed;
        c.elapsed = e;
        if (c.anim.duration < e) c.elapsed = e - c.anim.duration;
        // accumulated angle; the original post-multiplies the frame matrix every call, which is the
        // same rotation about the unchanged up axis (kept within +-360 here for float range)
        c.rotA += fu * 0.0500000007f;
        c.rotB += fu * -0.0500000007f;
        if (c.rotA > 360.f) c.rotA -= 360.f;
        if (c.rotB < -360.f) c.rotB += 360.f;
    }
}

bool TrackRenderer::LoadCar(IDirect3DDevice9* dev, const char* piz_path,
                            const char* dff_entry, const char* log_path) {
    std::FILE* log = log_path ? std::fopen(log_path, "a") : nullptr;
    auto fail = [&](const char* why) {
        if (log) { std::fprintf(log, "R5 car load FAILED: %s\n", why); std::fclose(log); }
        return false;
    };
    Piz::Archive piz;
    if (!piz.Load(piz_path)) return fail(piz.last_error());

    const std::uint8_t* dff = nullptr; std::uint32_t dff_len = 0;
    for (std::uint32_t i = 0; i < piz.count(); ++i)
        if (_stricmp(piz.entry(i).name, dff_entry) == 0) dff = piz.blob(i, &dff_len);
    if (!dff) return fail("DFF entry not found");

    // The car's metrics (this parse); the librw models are built by RegisterCarModel.
    Track::DffModel model;
    if (!model.Parse(dff, dff_len)) return fail(model.last_error());
    CarDropNonRenderAtomics(model, dff_entry, nullptr);   // as the registered models (part 0x13 below)

    // The car's long (fwd/back) model axis, from its bbox. Feeds the placement matrix
    // Render builds for the librw instance (car_long_is_x_) and the chase-camera scale.
    const float cdx = model.bbox[3] - model.bbox[0];
    const float cdz = model.bbox[5] - model.bbox[2];
    const bool long_is_x = cdx >= cdz;     // car's long (fwd/back) model axis

    // [librwrest] the car models: slot 0's livery as asked (`dff_entry`), registered with its
    // frame hierarchy by RegisterCarModel, which also binds rtex's dirt / env passes per model.
    // The other slots' liveries resolve at the first render (ResolveCarLiveries), because the
    // per-car colours are set after this load (RaceSession::Begin, MASHED_ROUND_COLOURS).
    // There is no D3D9 copy of the car (deleted in D1-residue R3, 2026-10-16).
    std::snprintf(car_piz_, sizeof(car_piz_), "%s", piz_path);
    {
        // "<base><livery>.DFF": strip the extension and the trailing livery digit
        std::size_t n = 0;
        while (dff_entry[n] && dff_entry[n] != '.' && n + 1 < sizeof(car_dff_base_)) {
            car_dff_base_[n] = dff_entry[n]; ++n;
        }
        car_dff_base_[n] = 0;
        if (n > 0 && car_dff_base_[n - 1] >= '0' && car_dff_base_[n - 1] <= '9') car_dff_base_[n - 1] = 0;
    }
    car_models_.clear();
    const int cm0 = RegisterCarModel(dev, dff_entry, log);
    rw_car_model_ = (cm0 >= 0) ? car_models_[static_cast<std::size_t>(cm0)].handle : -1;
    for (int s = 0; s < 4; ++s) car_slot_model_[s] = (cm0 >= 0) ? cm0 : 0;
    for (auto& cp : car_parts_) cp = CarPartState{};
    livery_dirty_ = true;
    // [rtex 2026-10-16] Part 0x13's frame origin: the env coefficient fades with its
    // distance to the camera.
    car_part19_ok_ = false;
    for (const auto& b : model.batches)
        if (b.part_id == 0x13) {
            std::memcpy(car_part19_, b.frame_pos, sizeof(car_part19_));
            car_part19_ok_ = true;
            break;
        }
    car_env_ok_ = cm0 >= 0 && car_models_[static_cast<std::size_t>(cm0)].env_ok && car_part19_ok_;
    if (log) std::fprintf(log, "[rtex] car env: part19=%d env_ok=%d\n", (int)car_part19_ok_,
                          (int)car_env_ok_);
    car_ground_off_ = -model.bbox[1];   // lift so model min-Y sits on ground
    // WS-E s3: car model size -> chase-camera scale. The Arctic geometry loads at
    // a small unit scale (car ~0.7 units long), so the chase rig is derived from
    // the car's own bbox rather than absolute units.
    car_len_    = (cdx > cdz ? cdx : cdz);
    if (car_len_ < 0.05f) car_len_ = 0.05f;
    car_height_ = model.bbox[4] - model.bbox[1];
    if (car_height_ < 0.02f) car_height_ = 0.02f;
    car_long_is_x_ = long_is_x;   // nose axis for the body-render orientation

    // REAL start line when the AI gates parsed: gate 0 center (LAPDATA's
    // Lap_Line(0)), facing gate 1 — the race direction.
    if (gates_.size() >= 2) {
        const float* g0 = gates_[0].center;
        const float* g1 = gates_[1].center;
        bool gok = false;
        const float gy = GroundHeight(g0[0], g0[2], &gok);
        if (gok) {
            car_pos_[0] = g0[0];
            car_pos_[2] = g0[2];
            car_pos_[1] = gy + car_ground_off_;
            car_yaw_ = std::atan2(g1[2] - g0[2], g1[0] - g0[0]);
            car_speed_ = 0.f;
            // A8: re-seed the integrated body basis to the grid heading.
            Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
            car_ready_ = true;
            // 3 AI opponents as a starting GRID just ahead of the start line, on the
            // unambiguous forward leg of the racing-line spline, all facing the race
            // direction. (The old "scatter at gates 2/4/6" put a car at a spot where the
            // spline passes near itself, so the faithful lookahead's geometric-nearest seed
            // landed on the wrong leg and the car drove backward into a jitter trap.)
            ai_cars_.clear();
            // [U-9145 2026-09-29] MASHED_MEASURE_SOLO=1 — MEASUREMENT HARNESS ONLY, on
            // the MASHED_STEER_HOLD / MASHED_MEASURE_NOOPP precedent: spawn NO AI
            // opponents at all, so `ai_cars_` stays empty and every downstream consumer
            // (UpdateRace, the RaceCamera framing, ParticipantCount, the rule engine)
            // sees a one-car race. Default-OFF; with it unset this line is
            // `if (gates_.size() > 8)`, byte-for-byte the previous behaviour.
            //
            // WHY IT EXISTS: the D2 gate's ORIGINAL-side reference
            // verify/a8_steer_20260824/orig_steerR.msd was captured with
            // `re/frida/scenario_launch.py` and NO `--cars` argument, i.e. its default
            // `cars=1` — a SOLO race. Re-run live 2026-09-29 on the same recipe
            // (verify/player_reg_20260929/orig_elim/, log/rules_oracle_rule0.json): the
            // oracle reports `SegmentCheck 0x00410d10 calls=1448 segment-end=0` and
            // `m1Max = -1`, i.e. no second car exists and no elimination ever runs. The
            // port side of that same gate spawns THREE opponents here, so the two sides
            // were never the same scenario. MASHED_MEASURE_NOOPP=1 only PARKS them,
            // which still leaves them in ai_cars_, in the camera and in UpdateRace; this
            // knob is the one that actually reproduces the reference's scenario.
            static const bool s_measureSolo = (std::getenv("MASHED_MEASURE_SOLO") != nullptr);
            if (gates_.size() > 8 && !s_measureSolo) {
                const float* gg0 = gates_[0].center;
                const float* gg1 = gates_[1].center;
                float fx = gg1[0] - gg0[0], fz = gg1[2] - gg0[2];
                const float fl = std::sqrt(fx*fx + fz*fz);
                if (fl > 1e-3f) { fx /= fl; fz /= fl; }
                // Track lateral axis. CONVENTION: perp(f) = (-fz, +fx), the SAME one used by
                // StartRound (:3133 lat = {-dir.z, dir.x}) and the AI lane offset (:2770
                // aim = g + (-tdz, +tdx)*lane). This site used to compute the OPPOSITE
                // perpendicular, (+fz, -fx), which was harmless where consumers are symmetric
                // (+/-0.9 grid sides, lanes -3/0/+3) but NOT here: lat[] below is asymmetric
                // and puts all three cars on ONE side, so the side depended on the odd-one-out
                // convention. Unified 2026-08-16 after the mirror-fix audit flagged it as a
                // trap for any future code that assumes one convention and calls the other.
                //
                // Behaviour is UNCHANGED: the axis is negated and lat[] is negated with it, so
                // every spawn lands on the same world position as before. The side itself is
                // load-bearing and stays put -- it is chosen so the faithful lookahead's
                // geometric-nearest seed lands on the forward leg of the spline (see the
                // comment above), which is a world-space/spline argument, not a visual one.
                const float rx = -fz, rz = fx;               // track "right" = perp(forward)
                const float baseYaw = std::atan2(fz, fx);    // race direction
                const float unit = (car_len_ > 0.05f ? car_len_ : 0.5f);
                const float lane = unit * 1.6f, row = unit * 2.2f;
                const float lat[3] = { -0.7f, -1.7f, -0.7f }; // 2-wide grid on the forward-seeding side
                const float lon[3] = {  1.f,   1.f,  2.4f };  // (front row + one behind), staggered ahead
                for (int i = 0; i < 3; ++i) {
                    AiCar a{};
                    float px = gg0[0] + rx*lane*lat[i] + fx*row*lon[i];
                    float pz = gg0[2] + rz*lane*lat[i] + fz*row*lon[i];
                    bool aok = false; float ay = GroundHeight(px, pz, &aok);
                    if (!aok) { px = gg0[0]; pz = gg0[2]; ay = GroundHeight(px, pz, &aok); }
                    a.pos[0] = px; a.pos[2] = pz;
                    a.pos[1] = (aok ? ay : gg0[1]) + car_ground_off_;
                    a.target = 1;
                    a.speed  = radius_ * (0.10f + 0.02f * static_cast<float>(i));
                    a.yaw    = baseYaw;
                    yaww::Hit(yaww::W_2690, static_cast<int>(i) + 1);
                    ai_cars_.push_back(a);
                }
            }
            if (log) {
                std::fprintf(log, "R6 car spawn at REAL start line: gate0="
                                  "(%.2f, %.2f, %.2f) yaw=%.2f (gates=%zu)\n",
                             car_pos_[0], car_pos_[1], car_pos_[2], car_yaw_,
                             gates_.size());
                std::fclose(log);
            }
            return true;
        }
    }

    // Spawn: scan the collision world for the most OPEN ground point (the
    // racing surface, not a pit/garage): score each candidate by how many of
    // 8 directions still have ground 4 units out at <1.0 height delta, and
    // face the car along the longest open run.
    bool ok = false;
    float sx = 0.f, sz = 0.f, sy = 0.f, syaw = 0.f;
    int best_score = -1;
    for (float rr = 0.f; rr < radius_; rr += radius_ * 0.07f) {
        for (int k = 0; k < 16; ++k) {
            const float ang = static_cast<float>(k) * 0.3926991f;
            const float x = center_[0] + rr * std::cos(ang);
            const float z = center_[2] + rr * std::sin(ang);
            bool hit = false;
            const float y = GroundHeight(x, z, &hit);
            if (!hit) continue;
            // the ground must be VISIBLE (render world within 1.0) — rules
            // out the frozen bay whose ice is collision-only
            bool rh = false;
            const float ry = HeightOnSoup(rend_verts_, rend_tris_, x, z, &rh);
            if (!rh || std::fabs(ry - y) > 1.0f) continue;
            int score = 0; float best_run = 0.f; float best_dir = 0.f;
            for (int d = 0; d < 8; ++d) {
                const float da = static_cast<float>(d) * 0.7853982f;
                float run = 0.f;
                for (float step = 4.f; step <= 16.f; step += 4.f) {
                    const float px = x + std::cos(da) * step;
                    const float pz = z + std::sin(da) * step;
                    bool h2 = false, h3 = false;
                    const float y2 = GroundHeight(px, pz, &h2);
                    const float y3 = HeightOnSoup(rend_verts_, rend_tris_,
                                                  px, pz, &h3);
                    if (!h2 || !h3 || std::fabs(y2 - y) > 1.5f ||
                        std::fabs(y3 - y2) > 1.0f) break;
                    run = step;
                }
                if (run >= 4.f) ++score;
                if (run > best_run) { best_run = run; best_dir = da; }
            }
            if (score > best_score) {
                best_score = score; ok = true;
                sx = x; sz = z; sy = y; syaw = best_dir;
                if (score == 8 && best_run >= 16.f) { rr = radius_; break; }
            }
        }
    }
    if (!ok) return fail("no collision ground found for spawn");
    car_pos_[0] = sx; car_pos_[1] = sy + car_ground_off_; car_pos_[2] = sz;
    car_yaw_ = syaw; car_speed_ = 0.f;
    // A8: re-seed the integrated body basis to the spawn heading.
    Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);

    if (log) {
        std::fprintf(log, "R5 car load OK: %s::%s — mats=%zu "
                          "tris=%u groundoff=%.3f spawn=(%.2f, %.2f, %.2f) "
                          "coltris=%zu\n",
                     piz_path, dff_entry,
                     model.materials.size(), model.total_tris,
                     car_ground_off_, sx, sy, sz, col_tris_.size() / 3);
        std::fclose(log);
    }
    car_ready_ = true;
    return true;
}

// [U-9683] Race/SpectatorDistances.cpp (declared again at the RaceComputeDistancesTick use in UpdateRace).
extern "C" void __cdecl RaceComputeDistancesTick(int site);
extern "C" int  __cdecl RaceRefDistAfterAi();
extern "C" void __cdecl RaceRefDistTraceFrame();
extern "C" void __cdecl RaceRefDistTraceAiEntry();

void TrackRenderer::UpdateCar(const DriveInput& in) {
    if (!car_ready_ || in.dt <= 0.f) return;
    // R6 handling v2 — velocity-vector model whose STRUCT SHAPE follows
    // VehicleControlUpdate (0x00470670): velocity vec (+0x9b0), forward vec
    // (+0x9d4), throttle as a force along forward (+0x1a8 pipeline, negated
    // for reverse), slide = the velocity component orthogonal to forward
    // (the +0xb0c measure). Tuning constants are SCAFFOLD (see
    // re/analysis/standalone_menu_sm/HANDLING_V2_2026-06-10.md).
    // Constants fitted to LIVE original telemetry (Quick Battle, 4 cars
    // sampled at VehicleControlUpdate 0x00470670; harvest_handling.py,
    // log/handling_telemetry.jsonl). Transferable measurements (rates, unit-
    // independent): coast decay k ~= 0.21/s (weighty ~4.7s time-constant);
    // lateral slide rms ~2% of forward speed (strong grip). max-speed const
    // +0x190 = 34.0 (internal units; world scale differs, so kTop stays
    // track-radius-relative). See HANDLING_V2_2026-06-10.md.
    float       kTop      = radius_ * 0.25f;
    if (boost_timer_ > 0.f) kTop *= 1.4f;   // Boost power-up: +40% top speed
    const float kDrag     = 0.21f;          // HARVESTED coast decay (throttle-OFF), weighty
    // Throttle factor: was 0.42 (reached kTop in ~3.3s). CALIBRATED 2026-06-30 against the
    // original via the in-race injector (re/frida/capture_player_dynamics.py, Arctic): the
    // original accelerates to ~top in ~2.2s from GO (63% in ~1s), so the old value was ~1.4x
    // too sluggish. 0.58 reaches kTop in ~2.2s. Raising it ONLY speeds the approach to the
    // clamp — top speed (kTop clamp) and the weighty coast decay (kDrag, throttle-off) are
    // unchanged. Env MASHED_THROTTLE_K overrides (=0.42 reverts). re/analysis/feel_calibration.md.
    static const float s_throttleK = []{ char b[16];
        DWORD n = GetEnvironmentVariableA("MASHED_THROTTLE_K", b, sizeof b);
        float f = n ? (float)atof(b) : 0.58f; return (f > 0.f) ? f : 0.58f; }();
    const float kThrottle = kTop * s_throttleK;
    const float kGrip     = 6.0f;    // strong (slide ~2% of fwd, harvested)
    const float kSteer    = 2.2f;
    const float kGravity  = radius_ * 0.12f;

    // [U-9174, D2 attempt 15] PHYSICS INIT HOISTED ABOVE THE COUNTDOWN. It used to sit
    // inside the VehiclePhysics_Enabled() block below, which the countdown branch
    // `return`s before reaching — so for the whole pre-race the 0xd04 record array did
    // not exist yet (`g_vehicleArrayBase` nullptr) and the launch rev-charge AV'd on its
    // first tick. The original has the records long before its pre-race state: the race
    // state machine's case 1 ("start of race", `FUN_004111c0`) runs the per-car init
    // (`FUN_0046b1c0` / `FUN_0046b540`) and only then advances toward the countdown, which
    // is state 5. Hoisting matches that ordering. The init is idempotent and one-shot, and
    // the first physics STEP still happens after the countdown — only the memset + world
    // handoff move earlier by the countdown's length.
    if (Vehicle::VehiclePhysics_Enabled()) {
        static bool s_pinit_pre = false;
        if (!s_pinit_pre) {
            Vehicle::VehiclePhysics_Init(1 + static_cast<int>(ai_cars_.size()), course_id_);
            Vehicle::VehiclePhysics_SetWorld(
                col_verts_.data(), static_cast<int>(col_verts_.size() / 3),
                col_tris_.data(),  static_cast<int>(col_tris_.size()  / 3),
                col_mat_.size() == col_tris_.size() / 3 ? col_mat_.data() : nullptr);
            s_pinit_pre = true;
        }
    }

    // [D-11073 leg 1b step 2] MASHED_L1B_INPUTS (default OFF): measurement-only per-frame
    // dump of the sub-state handlers' inputs. Writes nothing the game reads.
    {
        const int al[4] = { aib_alive(0), aib_alive(1), aib_alive(2), aib_alive(3) };
        float ps[4][3] = { { car_pos_[0], car_pos_[1], car_pos_[2] } };
        for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
            for (int c = 0; c < 3; ++c) ps[i + 1][c] = ai_cars_[static_cast<std::size_t>(i)].pos[c];
        SubState_InputsDump(countdown_, al, ps);
    }

    // [D-11073 leg 1b, PREREG_LEG1B.md section 3] RACE SUB-STATES 4 AND 5, default-OFF
    // MASHED_SUBSTATE. Replaces the countdown stand-in below (countdown_ -= dt,
    // LaunchRev_PreRaceTick / _Release) with FUN_004111c0's own phases, in its order:
    //   frame E      state-2 tail: DAT_005f29b8 = 12000, DAT_0063ba8c = 4 (no FUN_0040fc00)
    //   frame E+1    case 4: DAT_007f0fc8 = 0; FUN_0040dbd0; FUN_0040fc00 partials
    //   E+2..E+113   case 5: FUN_004103a0 (AI tick, per-car FUN_0046baa0 + FUN_0046d7f0,
    //                exit on the 112th call); FUN_0040fc00 partials
    // FUN_0040fc00 partials = Ai_AdvanceClock (incl. the 0x0040fc57 step store) + the
    // FUN_0041d930 tail. Cars are held exactly as the stand-in holds them, and the frame
    // returns before any physics. countdown_ is zeroed on the exit frame so every
    // countdown_ consumer sees GO at E+114. Knob OFF: this block is skipped entirely.
    if (SubState_Enabled() && countdown_ > 0.f) {
        car_speed_ = 0.f; car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
        for (auto& a : ai_cars_) a.cur_speed = 0.f;
        static TrackRenderer* s_subTr = nullptr;
        static float          s_subAccel = 0.f;
        s_subTr = this;
        s_subAccel = in.accel;
        static bool s_hostSet = false;
        if (!s_hostSet) {
            SubStateHost h{};
            h.car_alive = [](int v) { return aib_alive(v); };
            h.ai_tick = [] {
                TrackRenderer* t = s_subTr;
                if (!t || !(g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3))) return;
                g_aiTrack = t;
                t->AiBridgeSnapshot();
                Ai::Ai_Standalone_Tick();
            };
            h.player_accel_byte = [] {
                const float a = s_subAccel > 0.f ? s_subAccel : 0.f;   // LaunchRev_PreRaceTick's cook
                return static_cast<unsigned char>(a * 255.f);
            };
            SubState_SetHost(h);
            s_hostSet = true;
        }
        const int units = static_cast<int>(in.dt * 3000.0f + 0.5f);
        int& ba8c = *reinterpret_cast<int*>(0x0063ba8cu);
        int& s29b8 = *reinterpret_cast<int*>(0x005f29b8u);
        if (ba8c == 0) {                                   // FUN_004111c0 state-2 tail
            SubStateRollCountdown414180();                 // 0x00410259 FUN_00414180 (cd roll)
            SubState_SeedCd();                             // MASHED_SUBSTATE_CD override (measurement, unset by default)
            // [D-11073 leg 2] 0x0041026d: phase 3 only while the state-1 seed 100000 is intact
            // (round 1; seeded in Ai_BridgeLoad). Rounds 2+ see 0 (0x004104c0) -> state 4.
            if (s29b8 == 100000) {
                ba8c = 3;                                  // 0x00410279
                sub_p3_frames_ = 0;
            } else {
                s29b8 = 12000;                             // the else arm after 0x00414180's call
                ba8c = 4;
            }
            return;
        }
        if (ba8c == 3) {                                   // case 3: FUN_004102f0 (EMULATED, units in EBX)
            if (s29b8 == 100000) {                         // 0x004102f8 entry block, first frame only
                VehicleSeedWritePair(0, 0);                // 0x00448700: 100 x FUN_004464c0, +0x1c/+0x20 = 0
                *reinterpret_cast<int*>(0x00897fe0u) = 1;  // FUN_004430a0(1) @0x00410307 (the hold flag)
                *reinterpret_cast<int*>(0x007f0ff8u) = 0;  // FUN_0040e590 PARTIAL: 0x0040e5a2 clock store only
                SubStateCameraChain(true);                 // FUN_0040d470(1) -> FUN_00446520(cam, 1)
            }
            {                                              // 0x00410320..0x00410343 hold alive cars
                const std::uintptr_t tbl = static_cast<std::uintptr_t>(*reinterpret_cast<int*>(0x005f2770u));
                for (int v = 0, off = 0x34; off < 0x44; off += 4, ++v)
                    if (tbl && *reinterpret_cast<int*>(tbl + static_cast<std::uintptr_t>(off)) != 0 && aib_alive(v) == 1)
                        VehiclePhysicsStateReset(v);       // FUN_0046baa0
            }
            s29b8 = s29b8 - static_cast<int>(static_cast<unsigned>(units * 0x44c) / 0x3cu);   // 0x00410345..
            ++sub_p3_frames_;
            static const int s_measure = [] { const char* e = std::getenv("MASHED_P3_MEASURE");
                                              return (e && e[0]) ? std::atoi(e) : 0; }();
            const bool held_by_measure = (s_measure > 0 && sub_p3_frames_ < s_measure);
            // 0x0041035b: FUN_00405460() == 0 (no clip module in the exe: DAT_00639d70/78 are 0) &&
            // FUN_004430b0() == 0 (the flag, cleared by FUN_00445aa0 site (a) or (b)).
            if (!held_by_measure && *reinterpret_cast<int*>(0x00897fe0u) == 0) {
                ba8c = 4;                                  // 0x00410367
                s29b8 = 12000;                             // 0x00410371
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "[leg2] phase 3 released after %d frames -> DAT_0063ba8c=4\n", sub_p3_frames_);
                    std::fclose(lf);
                }
            }
        } else if (ba8c == 4) {
            *reinterpret_cast<int*>(0x007f0fc8u) = 0;      // 0x0041129b
            SubStateEnter4();                              // 0x004112a5
        } else if (ba8c == 5) {
            SubStateHeldCountdown5_C(units);               // 0x004112bb (units in EAX)
        }
        if (ba8c >= 3 && ba8c <= 5) SubStateCameraChain(false);   // FUN_0040fc00 -> FUN_0040d470(0) (L81-85)
        if (g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3))
            Ai::Ai_AdvanceClock(units);                    // FUN_0040fc00 0x0040fc57 / 0x0040fc63
        CarParts_FrameTick(units);                         // [librwrest] FUN_0040fc00 -> 0x0040fc75
        CopterTick(units);                                 // [librwrest] FUN_0040fc00 -> 0x0040fcb9
        HudSlideBillboardTimerTail();                      // FUN_0040fc00 -> 0x0040fd11
        if (ba8c == 6) {
            countdown_ = 0.f;
            Audio::SfxPlay("go", 0.9f);                    // real "GO!" SFX, as the stand-in
        }
        return;
    }

    // [rfx 2026-10-16] STAND-IN ADAPTER for the HUD's start-light clock (MASHED_SUBSTATE off;
    // under it the ported FUN_0040dbd0 / FUN_0041d930 tail run instead). The original zeroes
    // DAT_0063d588 at the state-4 entry (FUN_0040dbd0 -> FUN_0041d910 @ 0x0041d910), one state-4
    // frame plus 112 state-5 frames before GO (FUN_004103a0 exits on the 112th call), and adds
    // DAT_007f100c every frame from then on (FUN_0040fc00 -> FUN_0041d930 tail, 0x0041da76). This
    // countdown has no state 4, so the zeroing is placed where 113 frames remain, and the frame dt
    // is added on every later race frame, after GO too. Nothing else in the exe reads the two
    // globals with MASHED_SUBSTATE off. Removed with the stand-in when MASHED_SUBSTATE becomes the
    // default.
    if (!SubState_Enabled() && LibRw::RaceFx_HudEnabled()) {
        const float lead = 113.0f / 60.0f;
        if (countdown_ > lead && countdown_ - in.dt <= lead) {
            *reinterpret_cast<std::uint32_t*>(0x0063d584u) = 0u;   // 0x0041d910
            *reinterpret_cast<std::uint32_t*>(0x0063d588u) = 0u;   // 0x0041d91a
        } else if (countdown_ <= lead) {
            *reinterpret_cast<std::uint32_t*>(0x0063d584u) += 1u;
            *reinterpret_cast<float*>(0x0063d588u) += in.dt;
        }
    }
    // Countdown freeze (3-2-1-GO): cars are held still until it elapses.
    if (countdown_ > 0.f) {
        countdown_ -= in.dt;
        car_speed_ = 0.f; car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
        for (auto& a : ai_cars_) a.cur_speed = 0.f;
        // [U-9174, D2 attempt 15] START-LINE LAUNCH REV-CHARGE. The original runs
        // FUN_0046d7f0(car, 50) per active car on every pre-race state-tick
        // (0x00410441) and FUN_0046d780(car) once when the 1.86 s timer elapses
        // (0x0041049b) — the writer of veh+0xbf8 is 0x0046d7a2 inside the latter.
        // The cars are still frozen above; this only accumulates the integer charge
        // at veh+0xbf4, which A6a reads after GO. Confirmed on the running original,
        // two boots: verify/d2_writer_20261001/RESULT_STEP1.md.
        Vehicle::LaunchRev_PreRaceTick(in.dt, in.accel);
        if (countdown_ <= 0.f) {
            Audio::SfxPlay("go", 0.9f);                      // real "GO!" SFX
            Vehicle::LaunchRev_Release();
        }
        // [d1r2] The original runs the director on every pre-race frame (sub-states
        // 3/4/5: FUN_0040fc00 -> FUN_0040d470 -> FUN_00446520, main block when cam[7]
        // == 0), not from GO; MASHED_SUBSTATE does the same in SubStateCameraChain.
        // So the stand-in countdown advances race_cam_ too, and the first frame of a
        // round snaps (Reset() at round start). Render-side: race_cam_'s only sim
        // reader is EliminationCheck on required_zoom_, which Update recomputes from
        // the current cars (RaceCamera.cpp:359-362) and which runs only after GO.
        // The frame that reaches GO skips this: UpdateRace below advances it once.
        static const bool s_cd_cam_optout =
            GetEnvironmentVariableA("MASHED_NO_RACE_CAM_BASIS", nullptr, 0) != 0;
        if (countdown_ > 0.f && !s_cd_cam_optout) {
            Race::RaceCamCar cc[kRaceCars] = {};
            FillRaceCamCars(cc);
            cam_ticks_ += static_cast<double>(in.dt) * 3000000.0;   // DAT_007f1030 rate
            RaceCamUpdate(cc, false);
        }
        // [librwrest] the original runs FUN_0040fc00 (so FUN_00425d50's copter clock) in the
        // pre-race sub-states too; the GO frame ticks once, below
        if (countdown_ > 0.f) { CopterTick(static_cast<int>(in.dt * 3000.0f + 0.5f)); return; }
    }

    RaceRefDistTraceFrame();   // [U-9683] frame ordinal for MASHED_REFDIST_TRACE (measurement only)
    float accel = in.accel, steer = in.steer;
    if (round_mode_ && !human_drive_) {
        // exhibition round: the player car auto-follows the gate loop too
        // (skipped when human_drive_ — then accel/steer come from the player).
        if (!gates_.empty() && race_[0].alive) {
            const float* g = gates_[static_cast<std::size_t>(race_[0].gate) %
                                    gates_.size()].center;
            const float want = std::atan2(g[2] - car_pos_[2],
                                          g[0] - car_pos_[0]);
            float err = want - car_yaw_;
            while (err >  3.14159f) err -= 6.28318f;
            while (err < -3.14159f) err += 6.28318f;
            steer = (err > 0.25f) ? 1.f : (err < -0.25f ? -1.f : err * 4.f);
            accel = 0.82f;   // slowest car on the grid
        } else {
            accel = 0.f; steer = 0.f;
        }
    }
    // [G4] AI-driven player (autonomous playthrough, MASHED_AI_DRIVES_PLAYER): source the
    // player's steer/accel from slot 0's AI ctrl block — the standalone AI controller already
    // steps v0 -- [D3 AUDIT 2026-09-14] FALSE: Ai_Standalone_Tick is never called; this path
    // reads no AI ctrl block, it runs its own pure-pursuit below. A complete race/cup can
    // still run + be verified headless on
    // the REAL systems (no human, no gate-ribbon scaffold). Last frame's ctrl (the tick runs
    // later in this fn); one-frame lag is negligible. Requires the .AI bridge (real AI) loaded.
    static const bool s_ai_player = (std::getenv("MASHED_AI_DRIVES_PLAYER") != nullptr);
    if (s_ai_player && g_aib.loaded && Ai::I32(Ai::kSplineRaceCnt) > 3) {
        // Drive the player via the SAME faithful racing-line target + robust steer the
        // opponents use (lets a full field run headless for verification). No verbatim
        // bands -> no accel+brake deadlock; the player's own motion path integrates it.
        g_aiTrack = this;   // bind the LOS host before Ai_ComputeTarget
        float tx, tz;
        if (Ai::Ai_ComputeTarget(0, car_pos_[0], car_pos_[2], &tx, &tz)) {
            float dx = tx - car_pos_[0], dz = tz - car_pos_[2];
            float ye = std::atan2(dz, dx) - car_yaw_;
            while (ye >  3.14159265f) ye -= 6.28318531f;
            while (ye < -3.14159265f) ye += 6.28318531f;
            steer = ye / 0.5f;
            if (steer >  1.f) steer =  1.f;
            if (steer < -1.f) steer = -1.f;
            accel = 1.0f;
        }
    }

    // [librwrest] FUN_0040fc00 calls FUN_0040da50 (0x0040fc75) -> FUN_00420870 per car BEFORE
    // this frame's physics (0x0040fc9b), so the part inputs are read here, ahead of StepPlayer
    // and the opponents' steps.
    CarParts_FrameTick(static_cast<int>(in.dt * 3000.0f + 0.5f));
    CopterTick(static_cast<int>(in.dt * 3000.0f + 0.5f));   // FUN_0040fc00 -> FUN_00425d50 (0x0040fcb9)

    // WS-A8 (2026-06-17): when MASHED_REAL_PHYSICS is on, drive the player car
    // through the ported chain (A3 init -> A4 control -> A5 -> A6a -> A6b) instead
    // of the kinematic scaffold. PENDING runtime-smoke + C4 (WS-A-VERIFY-3).
    if (Vehicle::VehiclePhysics_Enabled()) {
        // Init + SetWorld (WS-A8-GAPS: the track collision soup, so the wheel solver
        // finds ground) MOVED above the countdown branch, see the U-9174 note there.
        Vehicle::PlayerCarIO io;
        io.pos[0] = car_pos_[0]; io.pos[1] = car_pos_[1]; io.pos[2] = car_pos_[2];
        io.vel[0] = car_vel_[0]; io.vel[1] = car_vel_[1]; io.vel[2] = car_vel_[2];
        io.yaw = car_yaw_; io.speed = car_speed_;
        for (int k = 0; k < 8; ++k) io.input[k] = 0;
        // Input byte map RESOLVED (ai_ctrl_byte_map_RESOLVED + WS-A8-STEER):
        // [0]/[1]=STEER (sign A/B; A4 FUN_00470670 reads them and writes the front-
        // wheel steer angle to +0x1a8/+0x26c), [4]=accelerator, [5]=brake/reverse
        // (A6a FUN_00467650 reads them). StepPlayer sets input[0]/[1] from io.steer.
        io.input[4] = static_cast<unsigned char>((accel > 0.f ? accel : 0.f) * 255.f);
        io.input[5] = static_cast<unsigned char>((accel < 0.f ? -accel : 0.f) * 255.f);
        io.steer    = steer;   // [-1,+1] -> descriptor steer bytes [0]/[1]
        io.ground_off = car_ground_off_;   // [d2posmodel] fix B part 3: physics y = pos[1] - this
        // G2 DRIVE-STABILIZE (2026-06-18): ground from the car's CURRENT pos (is it on
        // the ground NOW?), not a probe-ahead with last frame's velocity — the
        // probe-ahead + a runaway velocity made `grounded` oscillate 1/0 each frame,
        // so A6a's grounded-gated grip fired only intermittently -> no yaw. (diag:
        // re/analysis/STEER_CALIB_RUNTIME_2026-06-18.md)
        bool gok = false;
        GroundHeight(car_pos_[0], car_pos_[2], &gok);
        io.grounded = gok ? 1 : 0;
        Vehicle::VehiclePhysics_StepPlayer(in.dt, io);
        car_vel_[0] = io.vel[0]; car_vel_[1] = io.vel[1]; car_vel_[2] = io.vel[2];
        car_speed_  = io.speed;
        // CORRECTED 2026-08-24: this said "recovered chain-grip heading (+0x9c0)",
        // which named the wrong source. io.yaw is NOT derived from the yaw rate —
        // +0x9c0 is never read in VehiclePhysicsRun.cpp. StepPlayer relaxes io.yaw
        // toward the chain VELOCITY heading, in this same atan2(z,x) convention
        // (VehiclePhysicsRun.cpp:596-603): velHeading = atan2(cvz,cvx);
        // io.yaw += (velHeading - io.yaw) * frac. So this is a velocity-heading
        // follow, not a yaw-rate integral. The distinction matters: a sign bug was
        // predicted on the yaw-rate-integral reading and does not exist — measured
        // 2026-08-24, car_yaw decreases under +steer exactly as the original does
        // (D2_REALPHYS_REMEASURE_2026-08-21.md "A8 standalone arm").
        // A8 (2026-08-25): io.yaw is now the BODY heading read back from the
        // integrated orientation basis (FUN_0046e9e0's omega x R row integration),
        // not a lag chasing the velocity heading. The two can now differ — that
        // difference IS the slip angle, which the old model could not represent.
        car_yaw_ = io.yaw;
        // A8: the POSITION step advances by the VELOCITY VECTOR, as FUN_0046e9e0
        // does (`EBX[0xc..0xe] = EDI[0xc..0xe] + k*dt*ESI[0x26c..0x26e]`, stores at
        // 0x0046ea5f/0x0046ea69/0x0046ea73). io.drive_delta is that increment
        // accumulated over the frame's substeps, already scaled by dt and by the
        // original's own internal->world factor _DAT_005cc948 * _DAT_005cea80.
        // REPLACES `pos += {cos,0,sin(car_yaw_)} * io.drive_speed * dt`, which
        // forced the path to follow the nose and so could not express a slide.
        // [d2posmodel 2026-10-16] fix B part 3 (MASHED_POS_TRANS): the physics position is the
        // car's position. StepCar returns it in io.pos (velocity integral per substep plus the
        // 0x0046f6c0 contact-normal corrections, y included), in this file's convention
        // (physics y + car_ground_off_), so y is no longer snapped to GroundHeight + offset.
        // The off-mesh test and the recovery below are unchanged; a recovery re-seeds the
        // physics y from the ground (StepCar's adapter IN), as the spawn does.
        const bool posTrans = Vehicle::PosModelTrans();
        const float nx = posTrans ? io.pos[0] : car_pos_[0] + io.drive_delta[0];
        const float nz = posTrans ? io.pos[2] : car_pos_[2] + io.drive_delta[2];
        bool nok = false;
        const float ngy = GroundHeight(nx, nz, &nok);
        if (nok) { car_pos_[0] = nx; car_pos_[2] = nz; car_pos_[1] = posTrans ? io.pos[1] : ngy + car_ground_off_; }
        else {
            // [G4][U-A8-OFFTRACK] off-mesh recovery.
            //
            // FIXED 2026-08-25. The old version re-aimed velocity + heading toward a
            // gate and multiplied car_speed_ by 0.6, but it NEVER MOVED THE CAR back
            // onto the drivable surface. So if the re-aim did not by itself clear the
            // boundary, this branch ran again the next frame and the 0.6 COMPOUNDED.
            // Measured in verify/a8_velvec_20260825/tb_A.txt: 4198.28 -> 21.88, which
            // is 0.6^10.3 to three figures — ten consecutive frames of decay with no
            // recovery, after which the race logic respawned the car. That
            // decay-to-respawn loop is what floors the gate-(a) median.
            //
            // It was latent before and became load-bearing at the corrected time
            // base, where the car covers ground 3x faster and reaches the boundary
            // far more often. The bug is the missing relocation, not the time base.
            //
            // Now: RELOCATE FIRST (RecoverOffMesh finds a VERIFIED on-mesh point via
            // FindOnMeshHeading and moves the car there, so the next frame starts on
            // the mesh and this branch cannot re-fire indefinitely), then re-aim
            // toward the gate ahead ONLY IF a short probe in that direction is itself
            // on-mesh — otherwise keep the heading RecoverOffMesh verified.
            // [U-9147 2026-09-29] MASHED_OFFMESH_LOG=<path> -> one line per recovery,
            // DIAGNOSTIC ONLY, default-OFF, writes nothing when unset.
            //
            // Why it exists: RecoverOffMesh writes car_vel_ = (cos ry, sin ry) * sp, i.e.
            // it puts the velocity EXACTLY along the body heading, so it sets the slip
            // angle the D2 metric measures to ZERO by construction. Measured on the
            // shipping build: it fires 35 times in the 1080-frame scored window (one per
            // ~31 frames), and slip rebuilds from ~0.08 at d=8 to ~0.26 only at d>=41 --
            // longer than the interval between fires, so the car essentially never
            // reaches its own steady-state slip. Dropping a 30-frame shadow after each
            // fire raises slip 1500-2000 from 0.1445 to 0.2006 at a CONSTANT median speed
            // (1776 -> 1807). See re/analysis/D2_REOPEN_2026-09-29.md section 12.
            //
            // What this log is for: telling "the car is genuinely driving off the track"
            // from "the collision mesh has holes and the car is off-MESH while still on
            // the track". Positions that cluster say mesh; positions spread around the
            // donut say physics.
            {
                static const char* s_oml = std::getenv("MASHED_OFFMESH_LOG");
                static long s_omn = 0;
                if (s_oml) {
                    ++s_omn;
                    const float px = car_pos_[0], py = car_pos_[1], pz = car_pos_[2];
                    const float sp0 = car_speed_, yaw0 = car_yaw_;
                    RecoverOffMesh();
                    if (std::FILE* lf = std::fopen(s_oml, "a")) {
                        std::fprintf(lf,
                            "OFFMESH n=%ld want=(%.17g,%.17g) from=(%.17g,%.17g,%.17g) "
                            "to=(%.17g,%.17g,%.17g) sp=%.17g->%.17g yaw=%.17g->%.17g "
                            "gate=%d\n",
                            s_omn, nx, nz, px, py, pz,
                            car_pos_[0], car_pos_[1], car_pos_[2],
                            sp0, car_speed_, yaw0, car_yaw_, race_[0].gate);
                        std::fclose(lf);
                    }
                } else {
                    RecoverOffMesh();
                }
            }
            if (!gates_.empty()) {
                const int n = static_cast<int>(gates_.size());
                const float* g = gates_[static_cast<std::size_t>((race_[0].gate + 3) % n)].center;  // a few AHEAD
                float dx = g[0] - car_pos_[0], dz = g[2] - car_pos_[2];
                float dl = std::sqrt(dx*dx + dz*dz);
                if (dl > 1e-3f) {
                    // Probe one short step along the gate chord. Same 0.3 step
                    // RecoverOffMesh uses for its own on-mesh verification.
                    bool pok = false;
                    GroundHeight(car_pos_[0] + dx/dl * 0.3f,
                                 car_pos_[2] + dz/dl * 0.3f, &pok);
                    if (pok) {
                        const float sp = car_speed_;      // keep RecoverOffMesh's speed;
                                                          // do NOT damp again here (that
                                                          // double-damp was the compounding)
                        car_vel_[0] = dx/dl * sp; car_vel_[2] = dz/dl * sp;
                        car_yaw_ = std::atan2(dz, dx);
                    }
                }
            }
            // A8: the integrated basis is the authority for the body heading, so any
            // re-aim above must re-seed it. RecoverOffMesh re-seeds for its own case;
            // this covers the gate re-aim that may have overwritten car_yaw_ after it.
            Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
        }
    } else {
    const float fwd[3] = {std::cos(car_yaw_), 0.f, std::sin(car_yaw_)};
    // throttle force along forward
    car_vel_[0] += fwd[0] * accel * kThrottle * in.dt;
    car_vel_[2] += fwd[2] * accel * kThrottle * in.dt;
    // slope gravity: project (0,-g,0) onto the ground plane (XZ components)
    bool gok = false;
    float n[3];
    GroundProbe(car_pos_[0], car_pos_[2], &gok, n);
    if (gok) {
        car_vel_[0] += kGravity * n[1] * n[0] * in.dt;
        car_vel_[2] += kGravity * n[1] * n[2] * in.dt;
    }
    // grip: bleed the lateral (slide) component; drag the forward one
    float vf = car_vel_[0] * fwd[0] + car_vel_[2] * fwd[2];
    float lx = car_vel_[0] - fwd[0] * vf;
    float lz = car_vel_[2] - fwd[2] * vf;
    const float bleed = 1.f - ((kGrip * in.dt > 1.f) ? 1.f : kGrip * in.dt);
    lx *= bleed; lz *= bleed;
    vf -= vf * kDrag * in.dt;
    if (vf >  kTop) vf = kTop;
    if (vf < -kTop * 0.4f) vf = -kTop * 0.4f;
    car_vel_[0] = fwd[0] * vf + lx;
    car_vel_[2] = fwd[2] * vf + lz;
    car_speed_ = vf;
    car_yaw_ += steer * kSteer * (vf / kTop) * in.dt;
    const float nx = car_pos_[0] + car_vel_[0] * in.dt;
    const float nz = car_pos_[2] + car_vel_[2] * in.dt;
    bool ok = false;
    const float gy = GroundHeight(nx, nz, &ok);
    if (ok) {
        car_pos_[0] = nx; car_pos_[2] = nz;
        car_pos_[1] = gy + car_ground_off_;
    } else {
        RecoverOffMesh();   // island edge / off-collision: steer back instead of freezing
    }
    }  // end MASHED_REAL_PHYSICS else (scaffold body)
    // [U-9145 2026-09-29] MASHED_PLAYERTRACE=1 -> player_trace.log, DIAGNOSTIC ONLY,
    // default-OFF, one pre-UpdateRace and one post-UpdateRace line per SIM STEP at
    // %.17g. It prints the player state MASHED_MOTION_DIAG does not: world position,
    // in.dt, race_[0].gate/laps/progress/alive, and three record floats.
    //
    // It is what found both U-9145 channels, and it is kept rather than deleted because
    // the remaining open question needs it: with MASHED_MEASURE_SOLO=1 the a8 arm is
    // still non-deterministic (two attractors, driving-median 1818.47 and 1505.12), so a
    // second real-time-keyed input survives the sim-clock fix in exe_main.cpp. The A/B
    // recipe is one run per configuration plus re/tools/…/firstdiff on the two traces:
    // the first differing FIELD names the channel. Writes nothing when unset.
    //
    // [d1r2 2026-10-16] MASHED_PLAYERTRACE_OUT=<path> moves the file off the CWD. Every
    // worktree run chdirs to the main repo (MASHED_ROOT), so concurrent sessions appended
    // to ONE player_trace.log and the G-CTRL ptrace hash read 91d73bac on a default build
    // (lines from three PIDs interleaved). Unset = the old path, byte for byte.
    {
        static const bool s_ptrace = (std::getenv("MASHED_PLAYERTRACE") != nullptr);
        static const char* s_ptpath = [] { const char* e = std::getenv("MASHED_PLAYERTRACE_OUT");
                                           return (e && e[0]) ? e : "player_trace.log"; }();
        static long s_pf = 0;
        if (s_ptrace) {
            ++s_pf;
            if (std::FILE* pf = std::fopen(s_ptpath, "a")) {
                // [U-9156 2026-09-30] a144/a148/a14c = the persistent angular
                // accumulator ESI[0x51..0x53] that ContactFixup writes at
                // 0x0046f552/558/55e and BodyOrientationIntegrate integrates
                // (BodyOrientationIntegrate.cpp:304-315); av = +0x9bc/9c0/9c4 angular
                // velocity; c9ec = the integer active-contact count [rec+0x9ec] the
                // fixup divides by (0x0046f3cd). These are the four channels the
                // ORIGINAL's .msd already carries, so adding them here makes the
                // cross-side window of §19.3 computable. DIAGNOSTIC ONLY.
                std::fprintf(pf, "f=%ld dt=%.17g pos=(%.17g,%.17g,%.17g) yaw=%.17g sp=%.17g "
                                 "vel=(%.17g,%.17g,%.17g) gate=%d lap=%d prog=%.17g "
                                 "b14=%.17g b1c=%.17g v9e4=%.17g "
                                 "a144=(%.17g,%.17g,%.17g) av=(%.17g,%.17g,%.17g) "
                                 "c9ec=%d gnd=%.17g bodyfwd=(%.17g,%.17g,%.17g) "
                                 "n9c8=(%.17g,%.17g,%.17g) r0up=(%.17g,%.17g,%.17g) "
                                 "r0at=(%.17g,%.17g,%.17g)\n",
                             s_pf, in.dt, car_pos_[0], car_pos_[1], car_pos_[2],
                             car_yaw_, car_speed_, car_vel_[0], car_vel_[1], car_vel_[2],
                             race_[0].gate, race_[0].laps, race_[0].progress,
                             Vehicle::VehiclePhysics_RecordF32(0, 0xb14),
                             Vehicle::VehiclePhysics_RecordF32(0, 0xb1c),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9e4),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x144),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x148),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x14c),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9bc),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9c0),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9c4),
                             Vehicle::VehiclePhysics_RecordI32(0, 0x9ec),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9e0),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9d4),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9d8),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9dc),
                             // +0x9c8/9cc/9d0: BodyOrient_OmegaFromSteer's rotation
                             // axis. The ORIGINAL's is exactly (0, ~1, 0) on 2333/2333
                             // frames of orig_solo3.msd (max|x| = max|z| = 0).
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9c8),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9cc),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x9d0),
                             // ring buffer 0's up and at rows (the mirrored body basis,
                             // SyncContactRingMatrix): matrix at r+0x928, up row +0x938,
                             // at row +0x948.
                             Vehicle::VehiclePhysics_RecordF32(0, 0x938),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x93c),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x940),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x948),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x94c),
                             Vehicle::VehiclePhysics_RecordF32(0, 0x950));
                std::fclose(pf);
            }
        }
    }
    UpdateRace(in.dt);
    {
        static const bool s_ptrace2 = (std::getenv("MASHED_PLAYERTRACE") != nullptr);
        static const char* s_ptpath2 = [] { const char* e = std::getenv("MASHED_PLAYERTRACE_OUT");
                                            return (e && e[0]) ? e : "player_trace.log"; }();
        if (s_ptrace2) {
            if (std::FILE* pf = std::fopen(s_ptpath2, "a")) {
                const int gi = race_[0].gate;
                const float* gc = gates_.empty() ? nullptr
                    : gates_[static_cast<std::size_t>(gi) % gates_.size()].center;
                std::fprintf(pf, "   post gate=%d lap=%d alive=%d prog=%.17g ng=%d "
                                 "gc=(%.17g,%.17g) rt=%.17g\n",
                             gi, race_[0].laps, race_[0].alive ? 1 : 0,
                             race_[0].progress, (int)gates_.size(),
                             gc ? gc[0] : 0.f, gc ? gc[2] : 0.f, race_time_);
                std::fclose(pf);
            }
        }
    }
    // Opponents: once the .AI banks load, the ported AI tick drives them (below). The
    // gate-ribbon scaffold is the fallback (no .AI, or MASHED_GATE_RIBBON_AI=1, the
    // documented A/B revert at the Ai_BridgeLoad call).
    const int ng = static_cast<int>(gates_.size());
    const bool faithful_nav = g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3);
    if (faithful_nav) {
        g_aiTrack = this;   // bind the AI host (held_powerup reads pw_ through it)
        {   // [mode 7 M3] FUN_00458920's bound callees (only used under MASHED_REG_CALLBACKS)
            g_pkcbTrack = this;
            Gameplay::PickupCbHost h{};
            h.slot_empty = pkcb_slot_empty; h.grant = pkcb_grant; h.sub_mode = pkcb_sub_mode;
            h.held_code = aib_held_powerup;
            Gameplay::Pickups_SetCbHost(h);
            Ai::ModeSeven_SetHost(aib_game_sub_mode, aib_held_powerup);   // [mode 7 M4] FUN_0040e350 + held word
        }
        // [D3 2026-09-26] The PORTED tick spine FUN_00418860 (Ai_Standalone_Tick) runs
        // every race frame and its ctrl bytes DRIVE the opponents through the ported
        // physics chain, exactly as the player's bytes drive the player. The
        // MASHED_AI_TICK gate (default OFF, 2026-09-14) is DELETED, not inverted, and
        // the Option B motion model (turn-rate + velocity-shaped speed toward
        // Ai_ComputeTarget) that stood here is gone from the default build.
        //   clock: the original advances DAT_007f1008/0ff4/0ff8 by its 50-unit frame
        //   budget before the AI reads them (0x0040fc63 / 0x0040fe5e); units = dt*3000.
        //   feed:  StepCar with raw_steer=1, input[] = the car's ctrl block, i.e. the
        //   bytes A4 FUN_00470670 reads (0x00470732 / 0x00470754) and A6a reads ([4]/[5]).
        // MASHED_REAL_PHYSICS=0 (the D2 A/B revert) has no chain to feed, so on that
        // revert the opponents fall back to the pre-D3 Option B motion model below; it is
        // reachable ONLY through that revert flag.
        Ai::Ai_AdvanceClock(static_cast<int>(in.dt * 3000.0f + 0.5f));
        AiBridgeSnapshot();
        RaceRefDistTraceAiEntry();   // [U-9683] the array as the consumers will read it (measurement only)
        Ai::Ai_Standalone_Tick();
        AiStepDump();
        U9186GateDump();   // [U-9186 G1] default-OFF measurement probe
        // [D-11073 leg 1a] FUN_0041d930's tail (0x0041da5f..0x0041da76): DAT_0063d584 += 1,
        // DAT_0063d588 += _DAT_007f100c. The original runs it once per frame from FUN_0040fc00
        // (call at 0x0040fd11), after the step store (0x0040fc57, Ai_AdvanceClock above) and
        // after the AI tick, and FUN_004111c0 runs FUN_0040fc00 after the sub-state handler.
        // The port has no handler yet (leg 1b). Default OFF; nothing in the exe reads either
        // global in leg 1a (PREREG_LEG1A.md section 2).
        if (SubState_Enabled()) HudSlideBillboardTimerTail();
        const bool phys = Vehicle::VehiclePhysics_Enabled();
        yaww::Flags(faithful_nav, phys);   // A-REACH witnesses the branch, not prose
        // [U-9141 2026-09-29] MASHED_MEASURE_NOOPP=1 — MEASUREMENT HARNESS ONLY. Skips the
        // whole per-opponent update below (nothing else), so the three AI cars stay parked
        // on the grid and never enter the shared physics/collision world.
        //
        // It COMMANDS THE SCENARIO and changes no computed value, exactly as
        // MASHED_STEER_HOLD does for the player's input (exe_main.cpp:2933-2935: "MEASUREMENT
        // HARNESS ONLY; it commands input, it changes no computed value"). It is default-OFF
        // and is NOT a default-path change: with it unset this line is
        // `ci < (int)ai_cars_.size()`, byte-for-byte the previous behaviour. Every law above
        // and below it — the AI clock advance, AiBridgeSnapshot, Ai_Standalone_Tick,
        // AiStepDump, the player's own solver — is untouched.
        //
        // WHY IT EXISTS: U-9141. D3_DRIVE_2026-09-28.md §3.4 CONFIRMED that the `a8`
        // held-full-lock D2 recipe stopped being a controlled instrument at `09a73dc6`,
        // because three RNG-driven opponents began sharing the player's world during a 50 s
        // donut taken at the start line. With them not updated, HEAD was stable to four
        // decimals on 3/3 runs (slip 1500-2000 = 0.1427) and `09a73dc6` gave the identical
        // 0.1427 on 3/3; with them updated HEAD spans 0.128..0.177. That session created the
        // knob as a temporary diag and DELETED it afterwards; the D2 gate needs it
        // permanently, so it is re-added here as a named harness knob rather than re-invented
        // each time. Pre-registered pass rule and results:
        // re/analysis/D2_CONTROLLED_ARM_2026-09-29.md.
        //
        // NOT a way to make a number look better: it may only be used on the D2 controlled
        // arm, where BOTH ends of the comparison run with it (at `56ad3806` the opponents
        // never entered the chain at all, so that end needs no knob and gets the same state).
        static const bool s_measureNoOpp = (std::getenv("MASHED_MEASURE_NOOPP") != nullptr);
        const int aiCarN = s_measureNoOpp ? 0 : static_cast<int>(ai_cars_.size());
        for (int ci = 0; ci < aiCarN; ++ci) {
            AiCar& a = ai_cars_[static_cast<std::size_t>(ci)];
            const int v = ci + 1;
            if (round_mode_ && !race_[ci + 1].alive) {
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f; continue;
            }
            if (a.spin > 0.f) {   // spun out by a missile/mine (scaffold leaf effect)
                a.spin -= in.dt; a.yaw += 12.0f * in.dt;
                yaww::Hit(yaww::W_3283, v);
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f;
                if (phys) { Vehicle::VehiclePhysics_ResetOrientation(v, a.yaw);
                            yaww::Hit(yaww::R_3285, v); }
                continue;
            }
            if (a.slow > 0.f) a.slow -= in.dt;
            // Standalone stand-in for the original's respawn (FUN_00470c70's
            // elimination/respawn branches are not ported, VehiclePhysicsRun.h): a car
            // whose gate progress stalls 2.5 s is relocated two gates ahead. Every
            // relocation resets the car's lookahead index (FUN_00414030) and its body
            // basis, so the ported AI re-acquires the line from the new position.
            if (ng >= 2) {
                if (race_[v].gate != a.prog_gate) { a.prog_gate = race_[v].gate; a.stuck_t = 0.f; }
                else { a.stuck_t += in.dt; }
                if (a.stuck_t > 2.5f) {
                    const int gA = (race_[v].gate + 2) % ng;
                    const int gB = (race_[v].gate + 3) % ng;
                    const float* gp = gates_[static_cast<std::size_t>(gA)].center;
                    bool rok = false; const float gy2 = GroundHeight(gp[0], gp[2], &rok);
                    if (rok) {
                        a.pos[0] = gp[0]; a.pos[2] = gp[2]; a.pos[1] = gy2 + car_ground_off_;
                        a.yaw = std::atan2(gates_[static_cast<std::size_t>(gB)].center[2] - gp[2],
                                           gates_[static_cast<std::size_t>(gB)].center[0] - gp[0]);
                        yaww::Hit(yaww::W_3304, v);
                        a.vel[0] = a.vel[1] = a.vel[2] = 0.f; a.cur_speed = 0.f;
                        if (phys) { Vehicle::VehiclePhysics_ResetOrientation(v, a.yaw);
                                    yaww::Hit(yaww::R_3307, v); }
                        Ai::Ai_ResetVehicleIndex(v);
                    }
                    a.stuck_t = 0.f;
                }
            }
            if (!phys) { AiOptionBStep(a, v, in.dt, ng); continue; }

            const std::uint8_t* blk = reinterpret_cast<const std::uint8_t*>(
                Ai::kCtrlBlockBase + static_cast<std::uintptr_t>(
                    Ai::I32(Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * Ai::kSlotTableStride))
                * Ai::kCtrlBlockStride);
            Vehicle::PlayerCarIO io;
            io.pos[0] = a.pos[0]; io.pos[1] = a.pos[1]; io.pos[2] = a.pos[2];
            io.vel[0] = a.vel[0]; io.vel[1] = a.vel[1]; io.vel[2] = a.vel[2];
            io.yaw = a.yaw; io.speed = a.cur_speed;
            for (int k = 0; k < 8; ++k) io.input[k] = blk[k];
            // shock power-up (scaffold leaf, PowerupBackendImpl): throttled while a.slow > 0
            if (a.slow > 0.f) io.input[4] = static_cast<std::uint8_t>(io.input[4] * 0.35f);
            io.steer = 0.f;
            io.raw_steer = 1;
            io.ground_off = car_ground_off_;   // [d2posmodel] fix B part 3, as the player
            bool gok = false;
            GroundHeight(a.pos[0], a.pos[2], &gok);
            io.grounded = gok ? 1 : 0;
            Vehicle::VehiclePhysics_StepCar(v, in.dt, io);
            a.vel[0] = io.vel[0]; a.vel[1] = io.vel[1]; a.vel[2] = io.vel[2];
            a.cur_speed = io.speed;
            a.yaw = io.yaw;
            yaww::Hit(yaww::W_3334, v);        // A-COV: the mandatory coverage control
            yaww::Post(v, a.yaw);              // leg B, second program point
            const bool posTrans = Vehicle::PosModelTrans();   // [d2posmodel] fix B part 3, as the player
            const float nx = posTrans ? io.pos[0] : a.pos[0] + io.drive_delta[0];
            const float nz = posTrans ? io.pos[2] : a.pos[2] + io.drive_delta[2];
            bool nok = false;
            const float ngy = GroundHeight(nx, nz, &nok);
            if (nok) { a.pos[0] = nx; a.pos[2] = nz; a.pos[1] = posTrans ? io.pos[1] : ngy + car_ground_off_; }
            else {
                // off-mesh: the same ring-probe relocation the player uses (RecoverOffMesh).
                float ry; float rad = 0.f;
                if      (FindOnMeshHeading(a.pos[0], a.pos[2], a.yaw, 1.2f, ry)) rad = 1.2f;
                else if (FindOnMeshHeading(a.pos[0], a.pos[2], a.yaw, 0.5f, ry)) rad = 0.5f;
                if (rad > 0.f) {
                    float mx = a.pos[0] + std::cos(ry) * 0.3f, mz = a.pos[2] + std::sin(ry) * 0.3f;
                    bool mok = false; float my = GroundHeight(mx, mz, &mok);
                    if (!mok) { mx = a.pos[0] + std::cos(ry) * rad; mz = a.pos[2] + std::sin(ry) * rad;
                                my = GroundHeight(mx, mz, &mok); }
                    if (mok) { a.pos[0] = mx; a.pos[2] = mz; a.pos[1] = my + car_ground_off_; }
                    a.yaw = ry;
                    yaww::Hit(yaww::W_3351, v);
                    const float sp = a.cur_speed * 0.5f;
                    a.vel[0] = std::cos(ry) * sp; a.vel[1] = 0.f; a.vel[2] = std::sin(ry) * sp;
                    a.cur_speed = sp;
                    Vehicle::VehiclePhysics_ResetOrientation(v, a.yaw);
                    yaww::Hit(yaww::R_3355, v);
                } else {
                    a.vel[0] = a.vel[1] = a.vel[2] = 0.f; a.cur_speed = 0.f;
                }
            }
        }
        // diagnostic (env MASHED_AI_DIAG -> mashed_re.log, ~every 30 frames): pos/yaw/speed
        // so a stalled car's trajectory is visible. Read-only (does NOT call
        // Ai_ComputeTarget, which would double-update the continuity index this frame).
        {
            static const bool s_aidiag = (std::getenv("MASHED_AI_DIAG") != nullptr);
            static int s_aidn = 0;
            if (s_aidiag && (s_aidn % 30) == 0) {
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "AI-DIAG f=%d splineCnt=%d", s_aidn, Ai::I32(Ai::kSplineRaceCnt));
                    for (int ci = 0; ci < static_cast<int>(ai_cars_.size()) && ci < 3; ++ci) {
                        const AiCar& a = ai_cars_[(std::size_t)ci];
                        std::fprintf(lf, " | v%d spd=%.1f pos=(%.1f,%.1f) yaw=%.2f",
                                     ci + 1, a.cur_speed, a.pos[0], a.pos[2], a.yaw);
                    }
                    std::fprintf(lf, "\n");
                    std::fclose(lf);
                }
            }
            ++s_aidn;
        }
    } else {
    // AI v2: follow the gate ribbon with a lateral lane offset, braking for
    // sharp upcoming corners, velocity-shaped speed (no teleport).
    for (int ci = 0; ci < static_cast<int>(ai_cars_.size()); ++ci) {
        AiCar& a = ai_cars_[static_cast<std::size_t>(ci)];
        if (ng < 2) break;
        const RaceCar& rc = race_[ci + 1];
        if (round_mode_ && !rc.alive) { a.cur_speed = 0.f; continue; }
        // power-up effect: spun out by a missile/mine -> can't drive, spins in
        // place until the timer elapses.
        if (a.spin > 0.f) {
            a.spin -= in.dt;
            a.yaw += 12.0f * in.dt;
            yaww::Hit(yaww::W_3393, ci + 1);
            a.cur_speed = 0.f;
            continue;
        }
        if (a.slow > 0.f) a.slow -= in.dt;     // shocked: speed capped below
        const float* g  = gates_[static_cast<std::size_t>(a.target) % ng].center;
        const float* g2 = gates_[static_cast<std::size_t>(a.target + 1) % ng].center;
        // lateral aim = gate center + lane offset along the gate's local right
        float tdx = g2[0] - g[0], tdz = g2[2] - g[2];
        const float tl = std::sqrt(tdx * tdx + tdz * tdz);
        if (tl > 1e-3f) { tdx /= tl; tdz /= tl; }
        const float aimx = g[0] + (-tdz) * a.lane;
        const float aimz = g[2] + ( tdx) * a.lane;
        const float dx = aimx - a.pos[0], dz = aimz - a.pos[2];
        const float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < 3.0f) {
            a.target = (a.target + 1) % ng;
        }
        // corner sharpness ahead: angle between this and next ribbon segment
        const float* g3 = gates_[static_cast<std::size_t>(a.target + 2) % ng].center;
        float adx = g2[0] - g[0], adz = g2[2] - g[2];
        float bdx = g3[0] - g2[0], bdz = g3[2] - g2[2];
        const float al = std::sqrt(adx*adx + adz*adz), bl = std::sqrt(bdx*bdx + bdz*bdz);
        float corner = 1.f;   // 1 = straight, ->0 = hairpin
        if (al > 1e-3f && bl > 1e-3f)
            corner = (adx*bdx + adz*bdz) / (al * bl);
        const float want_yaw = std::atan2(dz, dx);
        float yerr = want_yaw - a.yaw;
        while (yerr >  3.14159f) yerr -= 6.28318f;
        while (yerr < -3.14159f) yerr += 6.28318f;
        a.yaw += yerr * (6.0f * in.dt > 1.f ? 1.f : 6.0f * in.dt);   // turn rate
        yaww::Hit(yaww::W_3423, ci + 1);
        // brake into corners: target speed scaled by corner straightness
        const float brake = 0.45f + 0.55f * (corner < 0.f ? 0.f : corner);
        float tgt = a.speed * brake;
        if (a.slow > 0.f) tgt *= 0.35f;        // shock power-up: throttled
        a.cur_speed += (tgt - a.cur_speed) * (2.5f * in.dt > 1.f ? 1.f : 2.5f * in.dt);
        const float nx2 = a.pos[0] + std::cos(a.yaw) * a.cur_speed * in.dt;
        const float nz2 = a.pos[2] + std::sin(a.yaw) * a.cur_speed * in.dt;
        bool aok = false;
        const float ay = GroundHeight(nx2, nz2, &aok);
        if (aok) {
            a.pos[0] = nx2; a.pos[2] = nz2;
            a.pos[1] = ay + car_ground_off_;
        } else {
            a.target = (a.target + 1) % ng;   // edge: skip to next gate
        }
    }
    }
    // [U-9683] default-ON (opt-out MASHED_NO_REFDIST_AFTER_AI): the producer's place in the original's frame order, after
    // every car's physics (player above, opponents in the block just closed) and after
    // PathProg_FrameTick (UpdateRace). Same coverage as the UpdateRace call it replaces:
    // UpdateRace returns before that call when gates_ is empty.
    // [U-9685] arm R: FUN_0040da50's FUN_00408610 loop sits between the AI path (0x0040fc6c) and the producer
    // path (0x0040fc7c) in FUN_0040fc00, so it runs here, before the producer call below.
    if (Race::RacePctOrig_Enabled()) Race::PathProg_RacePctTick();
    if (RaceRefDistAfterAi() && !gates_.empty()) RaceComputeDistancesTick(1);
    yaww::Frame();   // U-9188 leg A/B per-frame emit; no-op unless MASHED_AI_YAWW

    // power-up pickups: collect orbs the player drives through (respawn loop).
    if (pickups_.Update(in.dt, car_pos_))
        Gameplay::PickupCb_NoteFieldCollect(0, pickups_.held_type());   // [mode 7 M3] trace only
    // power-up DISPATCH, every frame (FUN_0045bba0; G-D1).
    TickPowerupDispatch(in.dt);
    // power-up EFFECTS: advance boost/shield timers, missiles, mines.
    UpdatePowerups(in.dt);
}

// slot 0 = player, 1..3 = ai_cars_[slot-1]. Spins the car out (brief loss of
// control) unless it's the player and a shield is up (which absorbs the hit).
void TrackRenderer::SpinOut(int slot) {
    if (slot == 0) {
        if (shield_timer_ > 0.f) { shield_timer_ = 0.f; return; }   // absorbed
        car_speed_ = 0.f;
        car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
    } else {
        const int idx = slot - 1;
        if (idx >= 0 && idx < static_cast<int>(ai_cars_.size()))
            ai_cars_[static_cast<std::size_t>(idx)].spin = 1.6f;   // seconds
    }
    const float* p = (slot == 0) ? car_pos_
                                  : ai_cars_[static_cast<std::size_t>(slot-1)].pos;
    parts_.SpawnBurst(p, 36, 0xffffd060u, track_radius_ * 0.10f,
                      track_radius_ * 0.018f, 0.8f);
    Audio::SfxPlay("explosion1", 0.85f);   // real spin-out explosion SFX
}

// ---- WS-D3: drive the ported power-up dispatch (Powerup/PowerupSystem) -------
// The invented boost/shield/missile/mine/shock ApplyPowerup switch is replaced by
// PowerupBackendImpl, which realises each ported per-type effect's LEAF on the
// existing in-race visuals (missiles_/mines_/ai_cars_/parts_). This is the
// WS-B/WS-E subsystem stand-in (the original effects attach RW models + run the
// contact system); the DISPATCH + per-type decision logic is the verbatim port.
class PowerupBackendImpl : public Powerup::IPowerupBackend {
    TrackRenderer* t_;
    float R() const { return t_->track_radius_ > 1.f ? t_->track_radius_ : t_->radius_; }
public:
    explicit PowerupBackendImpl(TrackRenderer* t) : t_(t) {}

    const Powerup::HostCar& Player() const override { return t_->pu_player_; }
    int  AiCount() const override { return static_cast<int>(t_->pu_ai_.size()); }
    Powerup::HostCar& Ai(int i) override { return t_->pu_ai_[static_cast<std::size_t>(i)]; }

    // MISSILE FIRE leaf (orig FUN_00455150: projectile pool + RwFrameAddChild).
    void SpawnMissile(const float pos[3], const float vel[3], int target) override {
        const float r = R();
        TrackRenderer::Missile mi;
        mi.pos[0]=pos[0]; mi.pos[1]=pos[1]+r*0.01f; mi.pos[2]=pos[2];
        mi.vel[0]=vel[0]*r; mi.vel[1]=vel[1]*r; mi.vel[2]=vel[2]*r;
        mi.target = (target >= 0) ? target : t_->MissileTargetAhead();
        mi.life = 4.0f;
        t_->missiles_.push_back(mi);
    }
    // MORTAR FIRE leaf (orig FUN_004533b0: lobbed pool DAT_00684ea8).
    void SpawnMortar(const float pos[3], const float vel[3], int /*target*/) override {
        const float r = R();
        TrackRenderer::Missile mi;
        mi.pos[0]=pos[0]; mi.pos[1]=pos[1]+r*0.02f; mi.pos[2]=pos[2];
        mi.vel[0]=vel[0]*r; mi.vel[1]=vel[1]*r*0.3f; mi.vel[2]=vel[2]*r;
        mi.target = -1;                       // arcs forward, no homing
        mi.life = 3.0f;
        t_->missiles_.push_back(mi);
    }
    // DRUM/P_MINE FIRE leaf (orig FUN_00454740 / FUN_00457ef0: dropped hazard).
    void DropHazard(const float pos[3], bool /*proximity*/) override {
        const float r = R();
        TrackRenderer::Mine m;
        m.pos[0]=pos[0]-std::cos(t_->car_yaw_)*r*0.04f;
        m.pos[1]=pos[1];
        m.pos[2]=pos[2]-std::sin(t_->car_yaw_)*r*0.04f;
        m.life = 20.0f;
        t_->mines_.push_back(m);
    }
    // GUN FIRE leaf (orig FUN_004561c0: forward hitscan tracer).
    void HitscanForward(const float pos[3], const float fwd[3]) override {
        const float r = R();
        const int tgt = t_->MissileTargetAhead();
        if (tgt >= 0) {
            const TrackRenderer::AiCar& a = t_->ai_cars_[static_cast<std::size_t>(tgt)];
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.8f)*(r*0.8f)) t_->SpinOut(tgt+1);
        }
        float sp[3] = { pos[0]+fwd[0]*r*0.1f, pos[1]+r*0.01f, pos[2]+fwd[2]*r*0.1f };
        t_->parts_.SpawnBurst(sp, 12, 0xffffe080u, r*0.05f, r*0.008f, 0.3f);
    }
    // SHOTGUN FIRE leaf (orig FUN_0045b6e0 -> FUN_0045b390: short spread cone).
    void SpreadCone(const float pos[3], const float fwd[3]) override {
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.4f)*(r*0.4f) && (dx*fwd[0]+dz*fwd[2]) > 0.f) a.slow = 2.5f;
        }
        float sp[3] = {pos[0], pos[1]+r*0.02f, pos[2]};
        t_->parts_.SpawnBurst(sp, 40, 0xffffe040u, r*0.25f, r*0.012f, 0.5f);
    }
    // R_FLAME jet leaf (orig FUN_0045a850: continuous forward burner).
    void FlameJet(const float pos[3], const float fwd[3], bool on) override {
        if (!on) return;
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.25f)*(r*0.25f) && (dx*fwd[0]+dz*fwd[2]) > 0.f) a.slow = 1.5f;
        }
        float sp[3] = {pos[0]+fwd[0]*r*0.08f, pos[1]+r*0.015f, pos[2]+fwd[2]*r*0.08f};
        t_->parts_.SpawnBurst(sp, 18, 0xffff8020u, r*0.10f, r*0.010f, 0.4f);
    }
    // FLASH FIRE leaf (orig FUN_00454db0: blind/screen flash).
    void BlindFlash(const float pos[3]) override {
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.6f)*(r*0.6f)) a.slow = 2.0f;
        }
        float sp[3] = {pos[0], pos[1]+r*0.03f, pos[2]};
        t_->parts_.SpawnBurst(sp, 60, 0xffffffffu, r*0.35f, r*0.010f, 0.5f);
    }
    // OIL FIRE leaf (orig FUN_00457800: ground slick).
    void DropOilSlick(const float pos[3]) override {
        TrackRenderer::Mine m;
        m.pos[0]=pos[0]; m.pos[1]=pos[1]; m.pos[2]=pos[2];
        m.life = 25.0f;
        t_->mines_.push_back(m);
    }
    // DEACT/teardown leaf (orig per-type DEACTs entry+0x10: GUN 0x4566f0 lock-
    // indicator RwFrameRemoveChild, MISSILE 0x455390, OIL 0x456e00, MORTAR 0x453630
    // detach; R_FLAME 0x45a8b0 stop-FX FUN_004661a0(0x16); DRUM/P_MINE/SHOTGUN/FLASH
    // flip a pool state byte). The spike has no on-car weapon attachment model, so
    // the visible teardown is a brief dissipation puff at the firing car, coloured
    // per weapon to match its FIRE FX; the dropped hazards/projectiles persist on
    // their own life timers (matching the original — DEACT only detaches the on-car
    // model, never the in-flight instances).
    void EffectEnd(int code, const float pos[3]) override {
        const float r = R();
        float sp[3] = { pos[0], pos[1] + r * 0.02f, pos[2] };
        std::uint32_t col; int n; float spd, sz;
        switch (code) {
            case Powerup::kGun:     col = 0xc0ffe080u; n = 8;  spd = r*0.04f; sz = r*0.008f; break;
            case Powerup::kMissile: col = 0xc0ff8020u; n = 10; spd = r*0.05f; sz = r*0.010f; break;
            case Powerup::kMortar:  col = 0xc0ff6020u; n = 10; spd = r*0.05f; sz = r*0.012f; break;
            case Powerup::kRFlame:  col = 0xa0ff8020u; n = 14; spd = r*0.06f; sz = r*0.012f; break;
            case Powerup::kShotgun: col = 0xc0ffe040u; n = 8;  spd = r*0.05f; sz = r*0.010f; break;
            case Powerup::kFlash:   col = 0xa0ffffffu; n = 12; spd = r*0.07f; sz = r*0.010f; break;
            case Powerup::kOil:     // detaches drip + drops the per-owner trail
                t_->pu_oil_has_ = false;
                col = 0xa0403020u; n = 6; spd = r*0.03f; sz = r*0.012f; break;
            case Powerup::kDrum:
            case Powerup::kPMine:
            default:                col = 0xa0c0c0c0u; n = 6;  spd = r*0.04f; sz = r*0.010f; break;
        }
        t_->parts_.SpawnBurst(sp, n, col, spd, sz, 0.35f);
    }
    void SfxByName(const char* name, float vol) override { Audio::SfxPlay(name, vol); }
    // OIL distance gate (orig: |pos-lastDrop|^2 >= _DAT_005cc56c, trail per owner).
    bool OilDropDue(int /*owner*/, const float pos[3]) override {
        const float r = R();
        if (!t_->pu_oil_has_) {
            t_->pu_oil_has_ = true;
            std::memcpy(t_->pu_oil_last_, pos, sizeof(float)*3);
            return true;
        }
        const float dx=pos[0]-t_->pu_oil_last_[0], dz=pos[2]-t_->pu_oil_last_[2];
        if (dx*dx+dz*dz >= (r*0.06f)*(r*0.06f)) {
            std::memcpy(t_->pu_oil_last_, pos, sizeof(float)*3);
            return true;
        }
        return false;
    }
};

// nearest AI car ahead of the player (replicates the old missile targeting).
int TrackRenderer::MissileTargetAhead() const {
    int best = -1; float bestd = 1e30f;
    const float fx = std::cos(car_yaw_), fz = std::sin(car_yaw_);
    for (int i = 0; i < static_cast<int>(ai_cars_.size()); ++i) {
        const AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
        if (round_mode_ && !race_[i+1].alive) continue;
        const float dx = a.pos[0]-car_pos_[0], dz = a.pos[2]-car_pos_[2];
        if (dx*fx + dz*fz <= 0.f) continue;       // must be ahead
        const float d = dx*dx + dz*dz;
        if (d < bestd) { bestd = d; best = i; }
    }
    return best;
}

// [D3 2026-09-14] (2) HOST SNAPSHOT — the missing half of WS-AI-BRIDGE part (2).
// The aib_* host fns above read g_aib.pos / .vel / .alive, and NOTHING wrote them:
// aib_own_xz returned (0,0) for every car and aib_alive returned 0 for every car,
// so Ai_Standalone_Tick's `if (s_host.car_alive(v) == 1) VehicleStep(v)` gate could
// never fire. The tick was therefore inert as well as uncalled. Fill the snapshot
// from live scene state each frame, before the tick.
//   v0            = player (car_pos_ / car_vel_)
//   v1..v3        = ai_cars_[v-1]
//   alive         = race_[v].alive, except round_mode_ eliminations
// [D3 2026-09-26] The pre-D3 "Option B" opponent motion model (turn-rate + velocity-shaped
// speed toward the ported racing-line target), moved out of UpdateCar. Reachable ONLY on
// the D2 physics revert (MASHED_REAL_PHYSICS=0), where there is no chain for the AI ctrl
// bytes to drive. The default build drives opponents from the ctrl bytes instead.
void TrackRenderer::AiOptionBStep(AiCar& a, int v, float dt, int ng) {
    // "where to go": the faithful racing-line lookahead target (falls back to the
    // gate ribbon point if the bank is momentarily empty).
    float tx, tz;
    if (!Ai::Ai_ComputeTarget(v, a.pos[0], a.pos[2], &tx, &tz)) {
        if (ng < 1) return;
        const float* g = gates_[static_cast<std::size_t>(a.target) % ng].center;
        tx = g[0]; tz = g[2];
    }
    // forward-progress safety net: where the racing-line spline passes near itself
    // (e.g. by the start/finish), the geometric-nearest seed can land on the wrong
    // leg and the lookahead points BACKWARD, sending the car the wrong way into a
    // seam jitter-trap. The gate ribbon (race_[v].gate) is correctly ordered, so if
    // the target opposes the local race direction, retarget the next gate ahead.
    if (ng >= 2) {
        const int gi = ((race_[v].gate % ng) + ng) % ng;
        const int gj = (gi + 1) % ng;
        const float gfx = gates_[(std::size_t)gj].center[0] - gates_[(std::size_t)gi].center[0];
        const float gfz = gates_[(std::size_t)gj].center[2] - gates_[(std::size_t)gi].center[2];
        const float tdx0 = tx - a.pos[0], tdz0 = tz - a.pos[2];
        const float gfl = std::sqrt(gfx*gfx + gfz*gfz);
        const float tdl = std::sqrt(tdx0*tdx0 + tdz0*tdz0);
        if (gfl > 1e-3f && tdl > 1e-3f &&
            (gfx*tdx0 + gfz*tdz0) / (gfl * tdl) < -0.25f) {   // target >~105deg off race dir
            tx = gates_[(std::size_t)gj].center[0];
            tz = gates_[(std::size_t)gj].center[2];
        }
    }
    // "how to move": steer toward the target (turn-rate) + velocity-shaped speed,
    // scrubbing for a sharp heading error but FLOORED at 45% cruise so the car
    // never fully stops (the verbatim bands' brake-latch deadlock cannot occur).
    float dx = tx - a.pos[0], dz = tz - a.pos[2];
    float yerr = std::atan2(dz, dx) - a.yaw;
    while (yerr >  3.14159265f) yerr -= 6.28318531f;
    while (yerr < -3.14159265f) yerr += 6.28318531f;
    a.yaw += yerr * (6.0f * dt > 1.f ? 1.f : 6.0f * dt);
    yaww::Hit(yaww::W_3683, v);
    float ae = (yerr < 0.f ? -yerr : yerr);
    float align = 1.f - ae / 1.5707963f;      // 1 aligned -> 0 at >=90deg off
    if (align < 0.f) align = 0.f;
    float tgt = a.speed * (0.45f + 0.55f * align);
    if (a.slow > 0.f) tgt *= 0.35f;            // shock power-up: throttled
    a.cur_speed += (tgt - a.cur_speed) * (2.5f * dt > 1.f ? 1.f : 2.5f * dt);
    const float nx2 = a.pos[0] + std::cos(a.yaw) * a.cur_speed * dt;
    const float nz2 = a.pos[2] + std::sin(a.yaw) * a.cur_speed * dt;
    bool aok = false;
    const float ay = GroundHeight(nx2, nz2, &aok);
    if (aok) { a.pos[0] = nx2; a.pos[2] = nz2; a.pos[1] = ay + car_ground_off_; }
    else {
        // off-mesh step: the racing-line target leads off the drivable surface here.
        // Just redirecting the heading (without moving) left the car frozen facing a
        // wall. Ring-probe for an on-mesh heading near the target bearing and COMMIT a
        // nudge — exactly as the player's RecoverOffMesh does — so the car follows the
        // edge back onto the line instead of pinning. (re/analysis/ai_spline_lookahead.md)
        const float want = std::atan2(dz, dx);
        float ry; float rad = 0.f;
        if      (FindOnMeshHeading(a.pos[0], a.pos[2], want, 1.2f, ry)) rad = 1.2f;
        else if (FindOnMeshHeading(a.pos[0], a.pos[2], want, 0.5f, ry)) rad = 0.5f;
        const float fl = a.speed * 0.4f;
        a.cur_speed = (a.cur_speed * 0.5f > fl) ? a.cur_speed * 0.5f : fl;
        if (rad > 0.f) {
            float mx = a.pos[0] + std::cos(ry) * 0.3f;
            float mz = a.pos[2] + std::sin(ry) * 0.3f;
            bool mok = false; float my = GroundHeight(mx, mz, &mok);
            if (!mok) {   // the small nudge landed in a gap; jump to the verified ring
                mx = a.pos[0] + std::cos(ry) * rad;     // point (on-mesh by construction)
                mz = a.pos[2] + std::sin(ry) * rad;     // so the car leaves the island.
                my = GroundHeight(mx, mz, &mok);
            }
            if (mok) { a.pos[0] = mx; a.pos[2] = mz; a.pos[1] = my + car_ground_off_; }
            a.yaw = ry;
            yaww::Hit(yaww::W_3717, v);
        }
    }
    a.vel[0] = std::cos(a.yaw) * a.cur_speed;
    a.vel[1] = 0.f;
    a.vel[2] = std::sin(a.yaw) * a.cur_speed;
}

void TrackRenderer::AiBridgeSnapshot() {
    g_aib.pos[0][0] = car_pos_[0];  g_aib.pos[0][1] = car_pos_[2];
    g_aib.vel[0][0] = car_vel_[0];  g_aib.vel[0][1] = car_vel_[2];
    g_aib.fwd[0][0] = std::cos(car_yaw_);  g_aib.fwd[0][1] = std::sin(car_yaw_);
    yaww::Pub(0, car_yaw_);   // CONSTRUCTION-ZERO, not a floor — PREREG §3b
    g_aib.alive[0]  = (round_mode_ && !race_[0].alive) ? 0 : 1;
    for (int i = 0; i < 3; ++i) {
        const int v = i + 1;
        if (i < static_cast<int>(ai_cars_.size())) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
            g_aib.pos[v][0] = a.pos[0];  g_aib.pos[v][1] = a.pos[2];
            g_aib.vel[v][0] = a.vel[0];  g_aib.vel[v][1] = a.vel[2];
            g_aib.fwd[v][0] = std::cos(a.yaw);  g_aib.fwd[v][1] = std::sin(a.yaw);
            yaww::Pub(v, a.yaw);               // leg B, first program point
            g_aib.alive[v]  = (round_mode_ && !race_[v].alive) ? 0 : 1;
        } else {
            g_aib.pos[v][0] = g_aib.pos[v][1] = 0.f;
            g_aib.vel[v][0] = g_aib.vel[v][1] = 0.f;
            g_aib.alive[v]  = 0;
        }
    }
}

// [D3 2026-09-14] (3a) STEP DUMP — the standalone side of the D3 AI measurement.
// Mirrors the original-side capture taken by re/frida/scenario_launch.py
// --statediff-aistep (which hooks FUN_00416250 and reads the ctrl block the caller
// passes). Same columns, so the two CSVs diff directly. Env MASHED_AI_STEPDUMP=<path>.
//
// Reads the ctrl block exactly as FUN_00418560 does (decomp pool0 2026-09-14):
//   slot  = *(int*)(0x007f1a14 + v*0x10)      @0x0041856c
//   block = 0x007f1038 + slot*0x4c            @0x00418575
//   bytes [0],[1] steer pair, [3] fire, [4] accel, [5] brake
// The standalone already commits slot = v in Ai_BridgeLoad, which is what the
// original's own allocator writes at 0x0043f895.
// [U-9186 leg G1 2026-10-08] MEASUREMENT ONLY. Dumps every input FUN_00442a60
// (0x00442a60) reads, so its gate chain can be scored offline without porting it.
// Default-OFF via MASHED_U9186_GATES=<path>. PREREG: PREREG_G1.md section 1.
//
// The gate chain, from the headless decompilation:
//   FUN_0040e370(v)  : *(int*)(PTR_PTR_005f2770 + v*4 + 0x34) != 0        GATE 1
//   FUN_0046c7b0(v)  : (&DAT_008815a4)[v*0x341] == 1                      GATE 2
//   FUN_0046cbb0(v)  : (&DAT_00881f90)[v*0x341] == 0                      GATE 3
//   FUN_0046d4a0(v)  : rec = 0x00881ec8 + v*0xd04 + t*0x40,
//                      t = *(u32*)(0x00881f48 + v*0xd04); reads +0x30/+0x38
//   FUN_00408ad0(v)  : *(float*)(0x008a96ec + v*0x30c)
//   output           : *(float*)(0x008989b0 + v*4)
//
// STRIDE NOTE: the decompiler renders the per-vehicle tables dword-indexed
// (`[v*0x341]`, `t*0x10`). The byte strides are v*0xd04 and t*0x40 — the decode
// PromoLoop_round58.cpp:21 already established. Using the decompiler's literals
// here would read the wrong addresses (memory offset-grep-misses-dword-index).
// [H1a AMEND A1] the rebound readers, declared at global scope so the probe can
// CALL them — the only code path H1a alters. Bodies: Vehicle/VehicleState.cpp.
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);
extern "C" int __cdecl VehicleCarStateRead(std::uint32_t carIdx,
                                           std::uint32_t* outState,
                                           std::uint32_t* outSecondary);
// [H1b] the standalone body of FUN_0046d4a0 (Vehicle/VehicleRecordPtr.cpp).
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx);
// [H3] FUN_00442a60's default-OFF tick (Race/SpectatorDistances.cpp). Declared
// extern "C" like its neighbours above: this TU is inside
// namespace mashed_re::D3d9Render, so a `namespace mashed_re { namespace Race ...`
// block here would declare mashed_re::D3d9Render::mashed_re::Race instead.
extern "C" void __cdecl RaceComputeDistancesTick(int site);   // site 0 = UpdateRace, 1 = after the AI block [U-9683]
extern "C" int  __cdecl RaceRefDistAfterAi();                 // [U-9683] MASHED_REFDIST_AFTER_AI
extern "C" void __cdecl RaceRefDistTraceFrame();              // [U-9683] measurement-only, MASHED_REFDIST_TRACE
extern "C" void __cdecl RaceRefDistTraceAiEntry();
// [GATEFIRE GF1] FUN_004148b0's standalone arm (Ai/AiLeaderTimer.cpp, now a shared TU
// rather than a second body at that RVA -- rva-lint's allowlist is a burn-down list).
extern "C" int __cdecl AiLeaderTimerProbe(int* outXZ, int vehIdx);
// [GATEFIRE GF1b] which early return the call above actually took.
extern "C" int __cdecl AiLeaderTimerExitSite(void);
// [PREREG_WIRE W-TOOK diagnosis] per-car counters on the WIRED call site in
// ControlStep (Ai/AiStandalone.cpp), distinct from the gates probe above.
extern "C" int g_wireReached[4]; extern "C" int g_wireRet[4];
extern "C" int g_wireFire[4];
// [GF1-CALLWISE] per-call branch-2 outcome, -1 = not evaluated.
extern "C" int g_b2Ret[4]; extern "C" int g_b2Los[4];
extern "C" int g_tg3Ret[4]; extern "C" int g_tg3Los[4];   // [D-11072 P3] Ai/AiTargetClosest.cpp
// [W-SHAPE port side] ctrl[4]/ctrl[5] at ControlStep entry, -1 = step did not run.
extern "C" int g_c4In[4]; extern "C" int g_c5In[4];

void TrackRenderer::U9186GateDump() {
    static const char* s_path = std::getenv("MASHED_U9186_GATES");
    if (!s_path || !s_path[0]) return;
    static std::FILE* gf = nullptr;
    static int gframe = 0;
    if (!gf) {
        gf = std::fopen(s_path, "w");
        if (!gf) { s_path = nullptr; return; }
        // [G1 CORRECTION 2026-10-08] The *_abs columns read the ORIGINAL's absolute
        // addresses (0x008815a4 / 0x00881f90 / 0x00881ec8 + v*0xd04). Under
        // MASHED_STANDALONE those are the blank image-pad: the port's vehicle record
        // array is rebound to Vehicle::g_vehicleArrayBase, NOT 0x008815a0
        // (LaunchRevCharge.cpp:42,62,81; stride witness imul 0xd04 at 0x0046d78e).
        // G1 run 1 read only the absolutes and so measured the pad, not the port.
        // The *_rec columns read the SAME fields through the port's own accessor at
        // the record-relative offsets the absolutes correspond to:
        //   0x008815a4 = record + 0x004     0x00881f90 = record + 0x9f0
        //   0x00881f48 = record + 0x9a8     0x00881ec8 = record + 0x928
        // (record base 0x008815a0 + v*0xd04; +0x928 + 0x30/0x38 = +0x958/+0x960,
        //  the body-matrix translation row the stepdump already logs as rec_958/960).
        std::fprintf(gf, "frame,v,p5f2770,slot_state,"
                         "veh_type_abs,state0_abs,rec_t_abs,rec_x_abs,rec_z_abs,"
                         "veh_type_rec,state0_rec,rec_t_rec,rec_x_rec,rec_z_rec,"
                         // [H1a AMEND A1] the *_fn columns CALL the rebound readers,
                         // which is the only path H1a actually alters. The *_abs
                         // columns read the absolutes as raw memory and so could
                         // never show the rebind — they are retained as the control
                         // (they must stay dead, proving no memory moved).
                         "veh_type_fn,state0_fn,"
                         // [H1b] the exe body of FUN_0046d4a0, CALLED. ptr_ok is its
                         // return; ptr_x/ptr_z are +0x30/+0x38 off the pointer it
                         // yields. They must equal rec_x_rec/rec_z_rec, which is the
                         // only evidence this exe copy has (the .asi copy's Frida
                         // diff exercised the absolute form, not this one).
                         "ptr_ok,ptr_x,ptr_z,"
                         // [H3] witnesses. ref_idx/other_idx are the two slots
                         // FUN_0040e180 selected (the original stores them at
                         // 0x008989a8/0x008989c8 before the guard chain) — gate
                         // H3-PAIR. exp_dist recomputes the expected distance
                         // from VehiclePhysics_RecordF32 WITHOUT calling the
                         // ported function, so H3-WROTE does not compare the
                         // port against itself.
                         "ref_idx,other_idx,exp_dist,"
                         "racepct_ec,refdist,"
                         // [GATEFIRE LEG 0 2026-10-08] RUNTIME substrate census for
                         // U-9186's two branches. PREREG: verify/d3_gatefire_20261008/
                         // PREREG_GATEFIRE.md section 3. APPEND-ONLY: every column above
                         // keeps its position so the H3-era scorers still key on it.
                         // MEASUREMENT ONLY — nothing here is read by game logic, and
                         // nothing is seeded (verify/d3_modes37_20261002: "seeding the
                         // globals would not be a port").
                         //
                         // Per-car. timer_4c8 is GF0-TIMER: the witness measured the port
                         // DEAD at 0 vs the original's 0..1250 stepping by 50
                         // (RESULT_WITNESS.md:70). prog_96e8 is GF0-PROG1, branch 1's
                         // input — U-9186 (UNCERTAINTIES.md:61) says it has no writer.
                         // Note it is 0x008a96e8, FOUR BYTES BELOW the racepct_ec column
                         // above (0x008a96ec), in the same 0x30c per-car stride.
                         "timer_4c8,rank_4c4,prog_96e8,"
                         // Scalars, repeated on each of the 4 rows. idx364 (GF0-IDX364)
                         // and bias374 (GF0-BIAS374) are the two upstream DISAGREEMENTS
                         // the witness left open (RESULT_WITNESS.md:109-113): the
                         // original reads -1 and 0 on all 512 calls, the port read 0 and
                         // {0,1,2,3}. idx364 decides whether E470 is called at all.
                         "idx364,bias374,mode368,flt360,framedt,"
                         // Image-initialised values the standalone never loads, so these
                         // are expected to read 0 and are the census's negative control:
                         // tbl10 is the ONLY limit-table index the original ever uses
                         // (value 1, RESULT_WITNESS.md:106) and thr_a8 is the first of
                         // the four thresholds (6.5 in the original image, :107).
                         "tbl10,thr_a8,"
                         // [GATEFIRE GF1 2026-10-08] branch 2 itself, default-OFF via
                         // MASHED_GF1=1. lt_ret is FUN_004148b0's return; lt_los is
                         // FUN_00416060 over (own XZ -> the leader XZ lt_ret wrote);
                         // lt_fire is the conjunction the original takes 64 times in
                         // the 220-call window (RESULT_STEP2.md:108). GF1-REACH is
                         // "lt_ret was evaluated at all" and is deliberately SEPARATE
                         // from GF1-FIRE, because FUN_004148b0 is stateful and a zero
                         // count must not be read as "the port is wrong" when the
                         // scenario may simply not reach the path.
                         "lt_ran,lt_ret,lt_los,lt_fire,lt_x,lt_z,lt_exit,"
                         // [PREREG_WIRE] the WIRED site, cumulative per car
                         "w_reach,w_ret,w_fire,"
                         // [GATEFIRE idx364 2026-10-08] PREDICTION of what a ported
                         // FUN_00414060 would compute, measured WITHOUT porting it —
                         // the H3-WROTE lesson applied before the code is written
                         // rather than after. Every input is a plain global read, so
                         // nothing here calls the function under test:
                         //   FUN_0042f6a0 = *(u32*)0x0067e9fc  (GameStateGetters.cpp:12)
                         //   FUN_00430790 = *(u32*)0x0067f17c  (SmallLeaves_o6.cpp:22)
                         //   FUN_00413fa0 = FUN_00430790()*3 + (mode==4?1:mode==5?2:0)
                         //   FUN_00431d80 = *(u32*)0x0067ea7c  (SplashGameMode_t5.cpp:118)
                         // and FUN_00414060's own arithmetic, with the four image
                         // constants read from MASHED.exe.unpatched, NOT guessed:
                         //   0x005cc32c = 0x3f000000 = 0.5   (scale on 0x0089a37c)
                         //   0x005cd088 = 0x40200000 = 2.5   (mode-6/10 multiplier)
                         //   0x005d757c = 0x00000000 = 0.0   (init + lower clamp)
                         //   0x005cc55c = 0x41200000 = 10.0  (upper clamp)
                         // pred_360 is the value 0x0089a360 WOULD take. The port
                         // currently hardcodes 2.5 there (TrackRenderer.cpp:397) with
                         // U-D3-DIFF360 open; if pred_360 != 2.5 then porting
                         // FUN_00414060 whole REGRESSES that global, and the real
                         // blocker is 0x0067ea7c's write-site (U-1305), not this body.
                         "m42f6a0,g30790,idx413fa0,tie67ea7c,a37c,a384i,pred_360,"
                         // [GATEFIRE idx364] the SECOND of step 6's two stores.
                         // The sentinel lands in idx364 above; this is the
                         // -1.0f quartet at 0x0089a870..87c. Gate A364-WROTE
                         // must witness BOTH stores or it verifies half a port.
                         "a870,a874,a878,a87c\n");
    }
    const std::uint32_t p = Ai::U32(0x005f2770u);
    // [GATEFIRE GF1] branch-2 counter, default-OFF and independent of the other knobs.
    static const bool s_gf1 = (std::getenv("MASHED_GF1") != nullptr);
    // [GATEFIRE idx364] Predict FUN_00414060's output without porting it. Scalar,
    // so computed once per frame outside the per-car loop. Transcribed from
    // re/analysis/bucket_util_0040e4b0_0042f790/0x00414060.md steps 1-5; step 6
    // (the 0x0089a364 = -1 sentinel and the four -1.0f writes) has no inputs and
    // so needs no prediction.
    const std::int32_t m42f6a0  = Ai::I32(0x0067e9fcu);   // FUN_0042f6a0
    const std::int32_t g30790   = Ai::I32(0x0067f17cu);   // FUN_00430790
    const std::int32_t idx413fa0 =                         // FUN_00413fa0
        g30790 * 3 + (m42f6a0 == 4 ? 1 : (m42f6a0 == 5 ? 2 : 0));
    const std::int32_t tie67ea7c = Ai::I32(0x0067ea7cu);  // FUN_00431d80
    const float        a37c      = Ai::F32(0x0089a37cu);
    // The table index is FUN_00413fa0's result; guard it, because a blank-mapped
    // g30790 could make it large and this is a measurement probe, not game code.
    const bool idx_ok = (idx413fa0 >= 0 && idx413fa0 < 64);
    const std::int32_t a384i = idx_ok
        ? Ai::I32(0x0089a384u + static_cast<std::uintptr_t>(idx413fa0) * 4u) : 0;
    // step 2: fVar2 = _DAT_0089a37c * 0.5 + (float)(int)(&DAT_0089a384)[iVar1]
    float fVar2 = a37c * 0.5f + static_cast<float>(a384i);
    // steps 3-4: the mode overrides, each * 2.5 (0x005cd088)
    if (m42f6a0 == 6)  fVar2 = static_cast<float>(tie67ea7c) * 2.5f;
    // FUN_0042fe80 = GetRaceEndFlag = *(u32*)0x0067ea90 (MenuInit.cpp:28)
    if (m42f6a0 == 10) fVar2 = static_cast<float>(Ai::I32(0x0067ea90u)) * 2.5f;
    // step 5: init 0.0, take fVar2 if >= 0.0, then clamp above at 10.0
    float pred360 = 0.0f;
    if (0.0f <= fVar2) { pred360 = fVar2; if (10.0f < fVar2) pred360 = 10.0f; }
    for (int v = 0; v < 4; ++v) {
        const std::uintptr_t vb = static_cast<std::uintptr_t>(v) * 0xd04u;
        // GATE 1 is only reachable when the pointer is non-null; -1 records
        // "could not be evaluated", which is NOT the same as "evaluated to 0"
        // (memory all-zero-reads-prove-nothing-alone).
        const std::int32_t slot_state =
            p ? Ai::I32(static_cast<std::uintptr_t>(p) + 0x34u +
                        static_cast<std::uintptr_t>(v) * 4u)
              : -1;
        const std::uint32_t t = Ai::U32(0x00881f48u + vb);
        const std::uintptr_t rec = 0x00881ec8u + vb + static_cast<std::uintptr_t>(t) * 0x40u;
        // Record-relative reads through the port's own accessor, which resolves to
        // g_vehicleArrayBase standalone and to 0x008815a0 in the .asi.
        const std::int32_t t_rec = Vehicle::VehiclePhysics_RecordI32(v, 0x9a8);
        const std::size_t mrow = 0x928u + static_cast<std::size_t>(t_rec) * 0x40u;
        std::uint32_t fn_state = 0u, fn_sec = 0u;
        VehicleCarStateRead(static_cast<std::uint32_t>(v), &fn_state, &fn_sec);
        const std::uint32_t fn_type = VehicleSlotGetter(static_cast<std::uint32_t>(v));
        // [H1b] call the exe body of FUN_0046d4a0 and read x/z off what it returns.
        std::uint32_t ptr = 0u;
        const std::uint32_t ptr_ok = PtrCompute881ec8(&ptr, static_cast<std::uint32_t>(v));
        const float ptr_x = (ptr_ok && ptr) ? *reinterpret_cast<const float*>(
                                static_cast<std::uintptr_t>(ptr) + 0x30u) : 0.f;
        const float ptr_z = (ptr_ok && ptr) ? *reinterpret_cast<const float*>(
                                static_cast<std::uintptr_t>(ptr) + 0x38u) : 0.f;
        // [GATEFIRE GF1] branch 2, default-OFF. Evaluated per car per frame here
        // rather than inside ControlStep: this is a COUNTER, and wiring the branch to
        // ctrl is a separate registered leg. lt_ran separates "did not run" from
        // "ran and returned 0" (memory absent-log-proves-nothing-run-a-control).
        int lt_ran = 0, lt_ret = 0, lt_los = 0, lt_fire = 0, lt_exit = -1;
        float lt_x = 0.f, lt_z = 0.f;
        if (s_gf1) {
            int outXZ[2] = {0, 0};
            lt_ret = AiLeaderTimerProbe(outXZ, v);
            lt_exit = AiLeaderTimerExitSite();
            lt_ran = 1;
            // FUN_004148b0 writes param_3[0]/[1] as the raw 4-byte patterns of the
            // leader's world x/z (rec+0x30 / rec+0x38), so reinterpret, do not cast.
            std::memcpy(&lt_x, &outXZ[0], 4);
            std::memcpy(&lt_z, &outXZ[1], 4);
            if (lt_ret != 0) {
                // the second conjunct: LOS from this car to the XZ the timer yielded
                const float ox = Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x30u);
                const float oz = Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x38u);
                lt_los = aib_los_clear(ox, oz, lt_x, lt_z);
                lt_fire = (lt_los != 0) ? 1 : 0;
            }
        }
        // [H3] independent recomputation: the distance from the reference car's
        // position to this car's, both read through VehiclePhysics_RecordF32 and
        // never through Race::ComputeDistances.
        const std::int32_t h3_ref   = Ai::I32(0x008989a8u);
        const std::int32_t h3_other = Ai::I32(0x008989c8u);
        double exp_dist = 0.0;
        if (h3_ref >= 0 && h3_ref < 4) {
            const std::int32_t rt = Vehicle::VehiclePhysics_RecordI32(h3_ref, 0x9a8);
            const std::size_t rrow = 0x928u + static_cast<std::size_t>(rt) * 0x40u;
            const float rx = Vehicle::VehiclePhysics_RecordF32(h3_ref, rrow + 0x30u);
            const float rz = Vehicle::VehiclePhysics_RecordF32(h3_ref, rrow + 0x38u);
            const float cx = Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x30u);
            const float cz = Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x38u);
            const float dx = rx - cx, dz = rz - cz;
            exp_dist = static_cast<double>(std::sqrt(dx * dx + dz * dz) * 0.8f);
        }
        std::fprintf(gf, "%d,%d,0x%08lx,%d,"
                         "%d,%d,%lu,%.9g,%.9g,"
                         "%d,%d,%d,%.9g,%.9g,"
                         "%lu,%lu,"
                         "%lu,%.9g,%.9g,"
                         "%d,%d,%.9g,"
                         "%.9g,%.9g,"
                         // [GATEFIRE LEG 0] census columns, append-only
                         "%d,%d,%.9g,"
                         "%d,%d,%d,%.9g,%d,"
                         "%d,%.9g,"
                         // [GATEFIRE GF1] branch 2 + exit site
                         "%d,%d,%d,%d,%.9g,%.9g,%d,"
                         "%d,%d,%d,"
                         // [GATEFIRE idx364] FUN_00414060 prediction + step-6 quartet
                         "%d,%d,%d,%d,%.9g,%d,%.9g,"
                         "%.9g,%.9g,%.9g,%.9g\n",
                     gframe, v,
                     static_cast<unsigned long>(p),
                     slot_state,
                     // --- the original's absolute addresses (blank pad standalone) ---
                     Ai::I32(0x008815a4u + vb),
                     Ai::I32(0x00881f90u + vb),
                     static_cast<unsigned long>(t),
                     static_cast<double>(Ai::F32(rec + 0x30u)),
                     static_cast<double>(Ai::F32(rec + 0x38u)),
                     // --- the same fields on the port's own record ---
                     Vehicle::VehiclePhysics_RecordI32(v, 0x004),
                     Vehicle::VehiclePhysics_RecordI32(v, 0x9f0),
                     t_rec,
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x30u)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, mrow + 0x38u)),
                     // --- the rebound readers, CALLED (H1a's only altered path) ---
                     static_cast<unsigned long>(fn_type),
                     static_cast<unsigned long>(fn_state),
                     // --- H1b: FUN_0046d4a0's standalone body, called ---
                     static_cast<unsigned long>(ptr_ok),
                     static_cast<double>(ptr_x),
                     static_cast<double>(ptr_z),
                     // --- H3: pair selection + independent expected distance ---
                     h3_ref, h3_other, exp_dist,
                     static_cast<double>(Ai::F32(0x008a96ecu +
                         static_cast<std::uintptr_t>(v) * 0x30cu)),
                     static_cast<double>(Ai::F32(0x008989b0u +
                         static_cast<std::uintptr_t>(v) * 4u)),
                     // --- [GATEFIRE LEG 0] runtime substrate census ---
                     // Per-car, stride 0x74 (AiLeaderTimer.cpp:53,:54).
                     Ai::I32(0x0089a4c8u + static_cast<std::uintptr_t>(v) * 0x74u),
                     Ai::I32(0x0089a4c4u + static_cast<std::uintptr_t>(v) * 0x74u),
                     // Branch 1's input, stride 0x30c (UNCERTAINTIES.md:61).
                     static_cast<double>(Ai::F32(0x008a96e8u +
                         static_cast<std::uintptr_t>(v) * 0x30cu)),
                     // Scalars (AiLeaderTimer.cpp:48-55).
                     Ai::I32(0x0089a364u),
                     Ai::I32(0x0089a374u),
                     Ai::I32(0x0089a368u),
                     static_cast<double>(Ai::F32(0x0089a360u)),
                     Ai::I32(0x007f1008u),
                     // Image-initialised negative control: index 10 is the only
                     // limit-table entry the original uses (AiLeaderTimer.cpp:98's
                     // bias374 + iVar1*5 resolves to 10 there), and 0x005cd0a8 is
                     // the first threshold. Both expected 0 standalone.
                     Ai::I32(0x005f2dd8u + 10u * 4u),
                     static_cast<double>(Ai::F32(0x005cd0a8u)),
                     // --- [GATEFIRE GF1] branch 2: FUN_004148b0 && FUN_00416060 ---
                     lt_ran, lt_ret, lt_los, lt_fire,
                     static_cast<double>(lt_x), static_cast<double>(lt_z), lt_exit,
                     g_wireReached[v], g_wireRet[v], g_wireFire[v],
                     // --- [GATEFIRE idx364] FUN_00414060 prediction, inputs first ---
                     m42f6a0, g30790, idx413fa0, tie67ea7c,
                     static_cast<double>(a37c), a384i,
                     static_cast<double>(pred360),
                     // step 6's -1.0f quartet, stride 4 (plate: 0x0089a870..87c)
                     static_cast<double>(Ai::F32(0x0089a870u)),
                     static_cast<double>(Ai::F32(0x0089a874u)),
                     static_cast<double>(Ai::F32(0x0089a878u)),
                     static_cast<double>(Ai::F32(0x0089a87cu)));
    }
    std::fflush(gf);
    ++gframe;
}

void TrackRenderer::AiStepDump() {
    static const char* s_path = std::getenv("MASHED_AI_STEPDUMP");
    if (!s_path || !s_path[0]) return;
    static std::FILE* lf = nullptr;
    static int frame = 0;
    static long seq = 0;
    if (!lf) {
        lf = std::fopen(s_path, "w");
        if (!lf) { s_path = nullptr; return; }
        std::fprintf(lf, "frame,seq,v,block,spline,c0,c1,c3,c4,c5,"
                         "ai_type,ai_spline_idx,ai_override,ai_mode,"
                         "clk_0ff4,step_1008,diff_a360,flag_a368,tgt_7ffc,"
                         "substate,rec_9e4,rec_b0c,c7,"
                         // [D3 2026-09-27] same columns the original-side capture adds:
                         // the FUN_00416250 target/curvature (read there off the entry
                         // hooks on FUN_00415e20 / FUN_00443440), the record's own x/z
                         // (+0x30/+0x38 per 0x0041628d/0x00416297) and the two steer-history
                         // globals 0x008032d8/0x008032dc + v*0x14, which carry the steering
                         // error and which of the two bands took it.
                         // march_n / march_idx0 mirror the original's FUN_00416230 call
                         // count and last argument (0x00444a2c / 0x0041623b).
                         "look_x,look_z,curv,own_x,own_z,hist_d8,hist_dc,march_n,march_idx0,"
                         // standalone-only: the phase-8 walk index before and after the
                         // wall-march, and whether any pass blocked.
                         "look_best,look_idx,look_blk,"
                         // [D3 2026-10-05] U-9191 leg 3. APPENDED, so every column above
                         // keeps its position and ai_posmatch.py / ai_headattrib.py are
                         // unaffected. Seven raw record reads, no derived value:
                         //   +0x958/+0x960  the record's own world X / Z. Pairing against the
                         //                  original's o_t1.msd uses THESE and not own_x/own_z,
                         //                  so it cannot be contaminated by any difference
                         //                  between what the AI believes its position is and
                         //                  where the physics record puts it.
                         //   +0x9d4/+0x9dc  body forward X / Z, so the heading can be measured
                         //                  DIRECTLY instead of inverted out of signed_err --
                         //                  the inversion U-9192 records as an identity.
                         //   +0x9bc/+0x9c0/+0x9c4  angular velocity x/y/z. +0x9c0 IS the yaw
                         //                  rate, so it is read directly and never differenced.
                         //                  That identification comes from U-9175's notes about
                         //                  the PLAYER and is checked for an AI car by leg 3's
                         //                  KA-1 before any cross-side number is read.
                         // PREREG: verify/d3_leg3_20261005/PREREG_LEG3.md
                         "rec_958,rec_960,rec_9d4,rec_9dc,rec_9bc,rec_9c0,rec_9c4"
                         // [D3 2026-10-05] U-9191/U-9193. APPENDED again, so the 42
                         // columns above keep their positions and ai_posmatch.py /
                         // ai_headattrib.py / ai_yawrate.py stay unaffected. Four more
                         // raw record reads, no derived value:
                         //   +0x144/+0x148/+0x14c  the ANGULAR VELOCITY triple
                         //     (ESI[0x51..0x53], re/analysis/vehicle_promote_c2/
                         //     0046e9e0.md:28). This is the yaw rate; the +0x9bc triple
                         //     appended above it is the TORQUE, not a rate --
                         //     verify/d3_omega_20261005/RESULT_RATEFIELD.md. The
                         //     player_trace at :3375-3377 already reads these three for
                         //     car 0; this is the same three for an AI slot.
                         //   +0x10  the omega-ARM gate FUN_0046e9e0 branches on
                         //     (ESI[4]): non-zero keeps the torque seed, zero rebuilds
                         //     omega from the steer chain. Dumped because the ORIGINAL
                         //     takes the non-zero arm on 2558 of 3623 o_t1.msd frames
                         //     (70.60%) while VehiclePhysicsRun.cpp:1001 calls
                         //     BodyOrient_OmegaFromSteer unconditionally. No plate
                         //     assigns +0x10 a semantic name, so it is logged raw.
                         // PREREG: verify/d3_arm_20261005/PREREG_ARM.md sections 3/3b
                         ",rec_144,rec_148,rec_14c,rec_10"
                         // [D3 2026-10-05] PREREG_CARCAR leg 1 (G-PORTHULL / G-RADIUS /
                         // G-SEL / G-SLOT0-ZERO). APPENDED again, so the 46 columns above
                         // keep their positions and ai_posmatch.py / ai_headattrib.py /
                         // ai_yawrate.py / ai_armregime.py stay unaffected. Nineteen RAW
                         // record reads, no derived value:
                         //   +0xa28/+0xa34/+0xa40/+0xa4c (4 x vec3)  the four world-space
                         //     points VehicleCarCarContact (0x00469df0) runs its
                         //     4-halfplane SAT over, read there at 0x00469e58 /
                         //     0x00469e73 / 0x00469e86 / 0x00469e91 and again from
                         //     0x0046a0ab. Both plates name them "vehicle bounding-hull
                         //     vertices (4 x vec3)" and neither names a producer; it is
                         //     world points 4..7 of the 18-point array FUN_00469aa0
                         //     transforms into +0x9f8 (CarWorldContacts.cpp:448), whose
                         //     body source is FUN_0046b1c0's AABB top face.
                         //     re/analysis/CARCAR_CALLSITE_2026-10-05.md section 4.
                         //   +0x95c, +0x998/+0x99c/+0x9a0  the OTHER two components of
                         //     ring slot [+0x9a8]'s translation row and the whole of ring
                         //     slot [+0x9ac]'s. rec_958/rec_960 above already carry slot
                         //     0's x and z; these complete the pair so G-SLOT0-ZERO can
                         //     compare one slot against the other rather than against an
                         //     expectation.
                         //   +0x4a4  the per-car radius the proximity gate at 0x00470b44
                         //     sums. The original's value is 0.6780367493629456 and
                         //     constant on 4 of 4 captures and 2 cars.
                         //   +0x9a8, +0x9ac  the two ring selectors, logged raw. No plate
                         //     assigns +0x9ac a semantic name.
                         // PREREG: verify/d3_carcar_20261005/PREREG_CARCAR.md section 2
                         ",h4x,h4y,h4z,h5x,h5y,h5z,h6x,h6y,h6z,h7x,h7y,h7z"
                         ",rec_95c,rec_998,rec_99c,rec_9a0,rec_4a4,sel_9a8,sel_9ac"
                         // [D-11072 legs C+A 2026-10-06] APPENDED again, so the 65
                         // columns above keep their positions and ai_speed_env.py /
                         // ai_ctrl_window.py / ai_posmatch.py / ai_yawrate.py /
                         // ai_armregime.py / hull_invariants.py stay unaffected.
                         // LEG C -- three witnesses for the slot-STATE table:
                         //   ss_base  *(u32*)0x005f2770, read RAW with no deref. The
                         //            seed at Ai_BridgeLoad makes this 0x005f2728.
                         //   ss_v     the exact FUN_0040e470(v) expression, which is
                         //            `*(u32*)(PTR_PTR_005f2770 + v*4 + 0x34)` read at
                         //            0x0040e474 (15-byte function, no branches).
                         //            GUARDED: reports -1 when ss_base == 0, because
                         //            the literal expression at base 0 dereferences
                         //            0x34 and AVs -- the measured 0xC0000005 that
                         //            AiStandalone.cpp:1385-1393 records.
                         //   ss_raw   the same slot read ABSOLUTELY at
                         //            0x005f2728 + 0x34 + v*4, no pointer involved, so
                         //            it cannot AV and cannot be an artefact of the
                         //            guard. This is the control: the null hypothesis
                         //            "the seed changes nothing" predicts 0 in BOTH arms.
                         // LEG A -- the port's race-position metric, for the scale
                         // measurement against the original's race_pct 0x008a96ec:
                         //   prog/lap/gate  race_[v].progress / .laps / .gate
                         //                  (Race/RaceSceneState.h:231-242), written at
                         //                  :4860 as laps*n + gate + frac.
                         //   racepct        TrackRenderer::RacePct(v) =
                         //                  fmod(progress,n)/n*100, the port's existing
                         //                  0..100 per-lap analogue (:4616-4619).
                         // PREREG: verify/d3_racepos_20261006/PREREG_LEGC_LEGA.md section 3b
                         ",ss_base,ss_v,ss_raw,prog,lap,gate,racepct"
                         // [D-11072 leg A follow-up 2026-10-06] the MONOTONE
                         // arc-length analogue and its 0..100 per-lap view, for the
                         // monotonicity re-measure. APPENDED, positions preserved.
                         //   arcprog  race_[v].arcprog (laps*n + gate-1 + segment t)
                         //   arcpct   fmod(arcprog, n)/n*100, the bridge candidate
                         // [D-11072 bridge write 2026-10-06] witnesses for the
                         // 0x008a96ec write. APPENDED, positions preserved.
                         //   ra_ec    F32(0x008a96ec + v*0x30c) read back: proves the
                         //            write took (== arcpct with the knob on, 0 off).
                         //   val_880  F32(0x0089a880 + v*4): AiPreTickRubberBand:1471
                         //            stores ra*0.01 + lap here, so it tracking arcpct
                         //            proves the LIVE reader at :1468 consumed the write.
                         //   lap_9648 I32(0x008a9648 + v*0x30c): the a20 lap term in
                         //            that same expression, so val_880 can be checked.
                         // PREREG: verify/d3_racepos_20261006/PREREG_BRIDGE.md section 3b
                         ",arcprog,arcpct,ra_ec,val_880,lap_9648"
                         // [D-11072 leg E2 2026-10-07] AMEND_E2.md A2/A4.
                         // APPENDED, positions preserved.
                         //   rmetric  rule_metric_[v]: the rc.metric[v] actually
                         //            handed to RE::UpdateFinishOrder at :5134 this
                         //            same frame (UpdateRace :3423 precedes this
                         //            dump at :3463). Scores E2-WROTE / E2-DIFF.
                         //   arclaps  race_[v].arclaps, the forward-only lap counter
                         //            arcprog is built from. Paired with lap (=
                         //            race_[v].laps) it scores E2-LAPAGREE, which
                         //            measures the two-lap-counter mismatch baked
                         //            into the pre-registered ON-arm formula.
                         //   rtick    rule_metric_tick_ (AMEND_E2.md A7): bumped
                         //            once per rule-engine write. A row is in
                         //            E2-WROTE's denominator only when rtick rose
                         //            since that car's previous dumped row, so a
                         //            frozen slot is excluded by measurement
                         //            rather than by a frame threshold.
                         ",rmetric,arclaps,rtick"
                         // [GF1-CALLWISE 2026-10-08] PER-CALL branch-2 outcome, the
                         // port's analogue of the original aistep's ret148b0 / ret16060.
                         // APPENDED, so every column above keeps its position. -1 means
                         // "not evaluated on this call", matching how o_t3 encodes a
                         // branch the control flow never reached -- 0 would collide with
                         // a real "the predicate returned 0".
                         ",b2_ret,b2_los"
                         // [W-SHAPE port side 2026-10-08] ctrl[4]/ctrl[5] at ControlStep
                         // ENTRY, matching the original aistep schema's c4_in/c5_in.
                         // Everything else in this row is read at END of frame, i.e. the
                         // OUTPUT pose; these two are the only entry-side columns.
                         // APPENDED, so every column above keeps its position.
                         // -1 = ControlStep did not run for this car before the dump.
                         ",c4_in,c5_in%s\n",
                     Ai::TargetClosest_Enabled() ? ",ret14a70,tg3_los" : "");   // [D-11072 P3]
    }
    for (int v = 1; v <= 3; ++v) {
        if (!g_aib.alive[v]) continue;
        const std::uintptr_t slot = static_cast<std::uintptr_t>(
            Ai::I32(Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * Ai::kSlotTableStride));
        const std::uintptr_t blk = Ai::kCtrlBlockBase + slot * Ai::kCtrlBlockStride;
        const std::uintptr_t ai  = Ai::kAiStateBase +
            static_cast<std::uintptr_t>(v) * Ai::kAiStateDwords * 4u;
        // [D3 2026-09-26] same trailing input columns as scenario_launch.py --statediff-aistep
        const Ai::StepLocals& sl = Ai::Ai_LastStepLocals(v);
        std::fprintf(lf, "%d,%ld,%d,%lu,0,%u,%u,%u,%u,%u,%d,%d,%d,%d,%d,%d,%g,%d,%d,%d,%.9g,%.9g,%u,"
                         "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%d,%d,%d,%d,%d,"
                         "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,"
                         "%.9g,%.9g,%.9g,%d,"
                         // PREREG_CARCAR leg 1, appended. Raw reads only.
                         "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,"
                         "%.9g,%.9g,%.9g,%.9g,%.9g,%d,%d,"
                         // D-11072 legs C+A, appended. ss_base/ss_v/ss_raw as
                         // signed decimal; prog/racepct raw.
                         "%lu,%d,%d,%.9g,%d,%d,%.9g,"
                         // D-11072 leg A follow-up: arcprog, arcpct;
                         // bridge witnesses: ra_ec, val_880, lap_9648.
                         "%.9g,%.9g,%.9g,%.9g,%d,"
                         // leg E2: rmetric, arclaps, rtick.
                         "%.9g,%d,%lu,"
                         // [GF1-CALLWISE] b2_ret, b2_los; [W-SHAPE] c4_in, c5_in
                         "%d,%d,%d,%d",
                     frame, seq++, v, static_cast<unsigned long>(blk),
                     Ai::U8(blk + 0), Ai::U8(blk + 1), Ai::U8(blk + 3),
                     Ai::U8(blk + 4), Ai::U8(blk + 5),
                     Ai::I32(ai + 0x00), Ai::I32(ai + 0x04),
                     Ai::I32(ai + 0x30), Ai::I32(ai + 0x60),
                     Ai::I32(0x007f0ff4u), Ai::I32(0x007f1008u),
                     static_cast<double>(Ai::F32(0x0089a360u)), Ai::I32(0x0089a368u),
                     aib_ai_target_enable(), aib_game_sub_mode(),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9e4)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xb0c)),
                     Ai::U8(blk + 7),
                     static_cast<double>(sl.look_x), static_cast<double>(sl.look_z),
                     static_cast<double>(sl.curv),
                     static_cast<double>(sl.own_x), static_cast<double>(sl.own_z),
                     static_cast<double>(Ai::F32(0x008032d8u + static_cast<std::uintptr_t>(v) * 0x14u)),
                     static_cast<double>(Ai::F32(0x008032dcu + static_cast<std::uintptr_t>(v) * 0x14u)),
                     sl.march_n, sl.march_idx0, sl.look_best, sl.look_idx, sl.look_blk,
                     // U-9191 leg 3, appended. Raw reads only.
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x958)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x960)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9d4)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9dc)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9bc)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9c0)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9c4)),
                     // U-9191 PREREG_ARM sections 3/3b, appended. Raw reads only.
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x144)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x148)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x14c)),
                     Vehicle::VehiclePhysics_RecordI32(v, 0x10),
                     // PREREG_CARCAR leg 1, appended. Raw reads only.
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa28)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa2c)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa30)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa34)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa38)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa3c)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa40)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa44)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa48)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa4c)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa50)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0xa54)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x95c)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x998)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x99c)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x9a0)),
                     static_cast<double>(Vehicle::VehiclePhysics_RecordF32(v, 0x4a4)),
                     Vehicle::VehiclePhysics_RecordI32(v, 0x9a8),
                     Vehicle::VehiclePhysics_RecordI32(v, 0x9ac),
                     // D-11072 leg C: three slot-STATE witnesses. ss_base is the raw
                     // pointer dword (no deref); ss_v is the literal FUN_0040e470(v)
                     // expression, GUARDED to -1 when the pointer is 0 (that deref AVs
                     // at base 0 -- AiStandalone.cpp:1385-1393); ss_raw reads the slot
                     // absolutely at 0x005f2728+0x34+v*4, so it never AVs and is the
                     // control the null hypothesis cannot pass (0 in both arms).
                     static_cast<unsigned long>(Ai::U32(0x005f2770u)),
                     (Ai::U32(0x005f2770u) == 0u) ? -1
                         : static_cast<std::int32_t>(*reinterpret_cast<std::uint32_t*>(
                               static_cast<std::uintptr_t>(Ai::U32(0x005f2770u))
                               + static_cast<std::uintptr_t>(v) * 4u + 0x34u)),
                     Ai::I32(0x005f2728u + 0x34u + static_cast<std::uintptr_t>(v) * 4u),
                     // D-11072 leg A: the port's race-position metric.
                     static_cast<double>(race_[v].progress),
                     race_[v].laps, race_[v].gate,
                     static_cast<double>(RacePct(v)),
                     // D-11072 leg A follow-up: the monotone arc-length analogue.
                     static_cast<double>(race_[v].arcprog),
                     static_cast<double>(
                         (total_len_ > 1e-3f)
                         ? std::fmod(race_[v].arcprog, total_len_) / total_len_ * 100.f
                         : 0.f),
                     // D-11072 bridge witnesses (raw reads of the original slots).
                     static_cast<double>(*reinterpret_cast<const float*>(
                         0x008a96ecu + static_cast<std::uintptr_t>(v) * 0x30cu)),
                     static_cast<double>(*reinterpret_cast<const float*>(
                         0x0089a880u + static_cast<std::uintptr_t>(v) * 4u)),
                     *reinterpret_cast<const int*>(
                         0x008a9648u + static_cast<std::uintptr_t>(v) * 0x30cu),
                     // leg E2 (AMEND_E2.md A2/A4): what the rule engine consumed,
                     // and the arc lap counter it disagrees with.
                     static_cast<double>(rule_metric_[v]),
                     race_[v].arclaps,
                     static_cast<unsigned long>(rule_metric_tick_),
                     // [GF1-CALLWISE] branch 2's per-call outcome, written by the wired
                     // site in ControlStep earlier in THIS frame (Ai_Standalone_Tick at
                     // :3511 runs before AiStepDump at :3512). -1 when the wired block
                     // did not evaluate -- knob off, or the mode-6 gate not taken.
                     g_b2Ret[v], g_b2Los[v],
                     // [W-SHAPE] the entry pose, snapshotted at the top of ControlStep
                     // in that same tick, before any ctrl store on any path.
                     g_c4In[v], g_c5In[v]);
        // [D-11072 P3] per-call FUN_00414a70 outcome, appended ONLY with MASHED_TGT3 so the
        // knob-OFF dump stays byte-identical (G-P3-OFF). -1 = not evaluated.
        if (Ai::TargetClosest_Enabled()) std::fprintf(lf, ",%d,%d", g_tg3Ret[v], g_tg3Los[v]);
        std::fputc('\n', lf);
    }
    std::fflush(lf);
    ++frame;
}


// D3 criterion (c), 2026-09-28: the collision triangle source the ported
// FUN_004b4cd0 segment query walks (Powerup/PowerupContact.cpp). This is the
// stand-in for the original's BSP walk FUN_00538c80 over COLLI*.BSP; it hands the
// same soup TrackRenderer::GroundProbe and the wheel contact solver already use.
// matKey is reported 0 = "allow": col_mat_ carries a material INDEX, and the two
// drop gates (FUN_0045c110 0x0045c116) key on the material's RwRGBA at +4, which
// the standalone's collision soup does not carry. MEASURED: on the original,
// every OIL and P_MINE gate call in verify/d3_contact_20260928/g2 and g3 returned
// 0, so "allow" is the measured behaviour on these two sites, not an assumption.
static int TrackTriSource(void* ctx, int i, float v9[9], std::uint32_t* matKey) {
    const TrackRenderer* t = static_cast<const TrackRenderer*>(ctx);
    const std::size_t nt = t->col_tris_.size() / 3;
    if (i < 0 || static_cast<std::size_t>(i) >= nt) return 0;
    for (int k = 0; k < 3; ++k) {
        const std::uint32_t vi = t->col_tris_[static_cast<std::size_t>(i) * 3 + static_cast<std::size_t>(k)];
        const float* p = &t->col_verts_[static_cast<std::size_t>(vi) * 3];
        v9[k * 3 + 0] = p[0]; v9[k * 3 + 1] = p[1]; v9[k * 3 + 2] = p[2];
    }
    *matKey = 0;
    return 1;
}

void TrackRenderer::EnsurePowerupBackend() {
    if (!pu_be_) { pu_be_ = new PowerupBackendImpl(this); pw_.Init(pu_be_); }
    Powerup::Contact::SetTriSource(TrackTriSource, this,
                                   static_cast<int>(col_tris_.size() / 3));
}

void TrackRenderer::SyncHostCar() {
    pu_player_.pos[0]=car_pos_[0]; pu_player_.pos[1]=car_pos_[1]; pu_player_.pos[2]=car_pos_[2];
    pu_player_.fwd[0]=std::cos(car_yaw_); pu_player_.fwd[1]=0.f; pu_player_.fwd[2]=std::sin(car_yaw_);
    pu_player_.vel[0]=car_vel_[0]; pu_player_.vel[1]=car_vel_[1]; pu_player_.vel[2]=car_vel_[2];
    pu_player_.yaw=car_yaw_; pu_player_.owner=0; pu_player_.alive=true;
    pu_ai_.resize(ai_cars_.size());
    for (std::size_t i=0;i<ai_cars_.size();++i) {
        const AiCar& a = ai_cars_[i];
        pu_ai_[i].pos[0]=a.pos[0]; pu_ai_[i].pos[1]=a.pos[1]; pu_ai_[i].pos[2]=a.pos[2];
        pu_ai_[i].fwd[0]=std::cos(a.yaw); pu_ai_[i].fwd[1]=0.f; pu_ai_[i].fwd[2]=std::sin(a.yaw);
        pu_ai_[i].yaw=a.yaw; pu_ai_[i].owner=static_cast<int>(i)+1;
        pu_ai_[i].alive = !(round_mode_ && !race_[i+1].alive);
    }
}

// [D3 WS-D, 2026-09-26] G-D1 + G-D2: the ported dispatcher FUN_0045bba0 now
// ticks EVERY race frame over all 4 slots (it used to run as a 1-2 frame burst
// per key press inside PowerupFireOnce, which could not exercise the cooldown,
// burst or flight state; re/analysis/D3_POWERUPS_2026-09-26.md).
//   slot 0 = player: armed from the pickup field's held orb; fire input = the
//     fire key's held state, press/held/release derived by the dispatcher.
//   slots 1..3 = ai_cars_[s-1]: armed when that AI car drives through an orb
//     (the original's box collect, 0x00458a1b, serves every car -- measured: AI
//     slots 1 and 2 held OIL and P_MINE in verify/d3_pu_20260926/o1). Fire
//     input = the AI ctrl block byte [7] (0x007f1038 + s*0x4c + 7), which is
//     exactly what the dispatcher reads (0x0045bd72) and what the AI's fire
//     decision FUN_00415220 writes (MOV [EDI+7],1 at 0x0041536c, 8 sites). The
//     ported AI tick runs FUN_00415220 (AiFireDecision, D3 2026-09-26), so
//     opponents OWN and FIRE (verify/d3_ai_20260926/sa2_pu.csv). The slot->ctrl mapping
//     is identity, as the original's race-launch allocator commits (0x0043f895).
void TrackRenderer::TickPowerupDispatch(float dt) {
    EnsurePowerupBackend();
    SyncHostCar();
    Powerup::HostCar cars[Powerup::PowerupSystem::kSlots];
    cars[0] = pu_player_;
    for (int s = 1; s < Powerup::PowerupSystem::kSlots; ++s) {
        if (static_cast<std::size_t>(s - 1) < pu_ai_.size()) cars[s] = pu_ai_[static_cast<std::size_t>(s - 1)];
        else { cars[s] = Powerup::HostCar(); cars[s].owner = s; cars[s].alive = false; }
    }
    // Box state DAT_0068d1f0[slot], the dispatcher's gate at 0x0045bc6b. Only the
    // race-start assignment of FUN_004111c0 @0x004111c0 is mirrored here (1 when
    // the slot has a car, 4 when it does not); the wreck value 2 has no producer
    // in this build -- FUN_00422fd0 / FUN_0040be50 are not ported -- so it is left
    // unwritten rather than invented. Slots without a car were already skipped by
    // the armed test, so 4 changes nothing observable today.
    for (int s = 0; s < Powerup::PowerupSystem::kSlots; ++s)
        pw_.SetBoxState(s, cars[s].alive ? 1 : 4);

    // slot 0 ownership: the HUD held orb stays until the armed type deactivates.
    if (!pw_.Armed(0)) {
        if (pu_slot0_was_armed_) { pickups_.ConsumeHeld(); pu_slot0_was_armed_ = false; }
        if (pickups_.held() >= 0) {
            int code = pickups_.held_type();
            if (code < 0) code = Powerup::kMissile;   // index-only orb -> a default weapon
            pw_.Activate(0, code);
            pu_slot0_was_armed_ = pw_.Armed(0);
        }
    }
    // slots 1..3 ownership (G-D2).
    for (int s = 1; s < Powerup::PowerupSystem::kSlots; ++s) {
        if (!cars[s].alive || pw_.Armed(s)) continue;
        int code = -1;
        if (pickups_.CollectAt(cars[s].pos, &code)) {
            Gameplay::PickupCb_NoteFieldCollect(s, code);   // [mode 7 M3] trace only
            if (code < 0) code = Powerup::kMissile;
            pw_.Activate(s, code);
        }
    }

    // [D3 2026-09-26] orb diagnostic (D3_POWERUPS section 6 item 2): once per 60
    // dispatcher ticks, under MASHED_PU_STEPDUMP=<p>, append to <p>.orbs.csv the orb
    // count, active orbs, the pick radius and per slot the XZ distance to the nearest
    // active orb and the armed code. Answers "why did no car collect an orb".
    {
        static const char* s_pd = std::getenv("MASHED_PU_STEPDUMP");
        static std::FILE* s_of = nullptr;
        static int s_n = 0;
        if (s_pd && s_pd[0]) {
            if (!s_of) {
                std::string op = std::string(s_pd) + ".orbs.csv";
                s_of = std::fopen(op.c_str(), "w");
                if (s_of) std::fprintf(s_of, "tick,orbs,active,pick_r,d0,d1,d2,d3,code0,code1,code2,code3\n");
            }
            if (s_of && (s_n % 60) == 0) {
                std::fprintf(s_of, "%d,%d,%d,%.3f", s_n, pickups_.OrbCount(),
                             pickups_.ActiveOrbCount(), pickups_.PickRadius());
                for (int s = 0; s < Powerup::PowerupSystem::kSlots; ++s)
                    std::fprintf(s_of, ",%.3f", cars[s].alive ? pickups_.NearestActiveOrbDist(cars[s].pos) : -2.f);
                for (int s = 0; s < Powerup::PowerupSystem::kSlots; ++s)
                    std::fprintf(s_of, ",%d", pw_.Armed(s) ? pw_.ActiveCode(s) : -1);
                std::fprintf(s_of, "\n");
                std::fflush(s_of);
            }
            ++s_n;
        }
    }

    // inputs (cook-shadow latch inside SetInput gives prev).
    const bool fire0 = pu_fire_held_ || pu_demo_fire_ > 0;
    if (pu_demo_fire_ > 0) --pu_demo_fire_;
    pw_.SetInput(0, fire0, false);
    for (int s = 1; s < Powerup::PowerupSystem::kSlots; ++s) {
        const std::uint8_t* blk = reinterpret_cast<const std::uint8_t*>(
            Ai::kCtrlBlockBase + static_cast<std::uintptr_t>(s) * Ai::kCtrlBlockStride);
        pw_.SetInput(s, blk[7] != 0, blk[8] != 0);
    }
    // FUN_0040e350() == DAT_0063ba8c; the per-slot pass needs 6 (0x0045bc2b).
    // [UNCERTAIN] the standalone has no DAT_0063ba8c state machine; "running
    // race" (past the countdown -- UpdateCar returns early while it runs) is
    // mapped to 6. Values 1..11 are written at 0x0040d3e7..0x004111a6.
    pw_.Tick(dt, cars, 6);
}

// [mode 7 M3] FUN_0045c010(v, type) -> FUN_0045bfa0 = PowerupSystem::Activate, with the backend and host
// cars ready as on every other Activate path (FirePowerupKind). Called from inside the registry pass.
void TrackRenderer::PickupGrant(int v, int type) {
    if (v < 0 || v >= Powerup::PowerupSystem::kSlots) return;
    EnsurePowerupBackend();
    SyncHostCar();
    pw_.Activate(v, type);
}

bool TrackRenderer::FireHeldPowerup() {
    if (!car_ready_ || !pw_.Armed(0)) return false;
    pu_demo_fire_ = 30;
    return true;
}

void TrackRenderer::FirePowerupKind(int code) {
    if (!car_ready_) return;
    // demo/testing: small indices (0..6) map to representative real type codes;
    // the 9 real codes (7,9,10,11,12,16,17,18,19) pass straight through.
    int real = code;
    if (code >= 0 && code < 7) {
        static const int demo[7] = { Powerup::kMissile, Powerup::kPMine,
                                     Powerup::kShotgun,  Powerup::kGun,
                                     Powerup::kFlash,    Powerup::kDrum, Powerup::kOil };
        real = demo[code];
    }
    EnsurePowerupBackend();
    SyncHostCar();
    pw_.Deactivate(0);
    pw_.Activate(0, real);
    pu_demo_fire_ = 30;
}

void TrackRenderer::UpdatePowerups(float dt) {
    if (dt <= 0.f || dt > 0.25f) dt = 0.016f;
    const float R = track_radius_ > 1.f ? track_radius_ : radius_;
    if (boost_timer_  > 0.f) boost_timer_  -= dt;
    if (shield_timer_ > 0.f) shield_timer_ -= dt;

    // missiles: home toward their target AI car, trail particles, hit on contact
    const float hitR = R * 0.05f;
    for (size_t i = 0; i < missiles_.size();) {
        Missile& m = missiles_[i];
        m.life -= dt;
        bool dead = (m.life <= 0.f);
        if (m.target >= 0 && m.target < static_cast<int>(ai_cars_.size())) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(m.target)];
            float to[3] = {a.pos[0]-m.pos[0], a.pos[1]-m.pos[1], a.pos[2]-m.pos[2]};
            const float d = std::sqrt(to[0]*to[0]+to[1]*to[1]+to[2]*to[2]);
            if (d < hitR) { SpinOut(m.target + 1); dead = true; }
            else if (d > 1e-3f) {                        // steer velocity toward target
                const float spd = R * 0.7f;
                for (int k = 0; k < 3; ++k) m.vel[k] += (to[k]/d*spd - m.vel[k]) * 4.f * dt;
            }
        }
        m.pos[0] += m.vel[0]*dt; m.pos[1] += m.vel[1]*dt; m.pos[2] += m.vel[2]*dt;
        parts_.SpawnTrail(m.pos, 0xffff8020u, R*0.012f, 0.4f);   // orange comet
        if (dead) { missiles_.erase(missiles_.begin()+i); }
        else ++i;
    }

    // mines: armed hazard; spin out any car (player or AI) that contacts it.
    for (size_t i = 0; i < mines_.size();) {
        Mine& m = mines_[i];
        m.life -= dt;
        parts_.SpawnTrail(m.pos, 0xffff4030u, R*0.010f, 0.5f);   // red pulse
        bool det = false;
        {
            const float dx = car_pos_[0]-m.pos[0], dz = car_pos_[2]-m.pos[2];
            if (dx*dx+dz*dz < hitR*hitR) { SpinOut(0); det = true; }
        }
        for (int c = 0; c < static_cast<int>(ai_cars_.size()) && !det; ++c) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(c)];
            const float dx = a.pos[0]-m.pos[0], dz = a.pos[2]-m.pos[2];
            if (dx*dx+dz*dz < hitR*hitR) { SpinOut(c+1); det = true; }
        }
        if (det || m.life <= 0.f) {
            if (det) parts_.SpawnBurst(m.pos, 30, 0xffff6020u, R*0.12f, R*0.016f, 0.7f);
            mines_.erase(mines_.begin()+i);
        } else ++i;
    }
}

void TrackRenderer::StartRound() {
    if (gates_.size() < 4 || !car_ready_) return;
    round_mode_ = true;
    round_alive_ = kRaceCars;
    round_winner_ = -1;
    for (int i = 0; i < kRaceCars; ++i) race_[i] = RaceCar{};
    // U-9005 RESOLVED (rules oracle 2026-07-02, log/rules_oracle_rule8.json
    // round-resets=1): the original DOES re-init the finish order on the
    // per-round restart path (order [0,-1,-1,-1] observed reset to all -1.0f
    // across a round cycle with no track reload). Mirror FUN_00414060's
    // slot reset per round.
    for (int s = 0; s < Race::RuleEngine::kCars; ++s)
        rulep_.finishOrder[s] = Race::RuleEngine::kEmptySlot;
    // The rule-10 countdown IS re-seeded per round. REFUTES the earlier "NOT
    // re-seeded" note here: D3 2026-09-26 oracle run (verify/d3_modes_20260926/
    // rules_oracle_r10seed2.json, seed10[]) caught FUN_004046a0 (0x004046a0)
    // exiting with DAT_007f0fe4 = 30.0 right after segment end #1 (SegmentCheck
    // call 1050) and #2 (call 2206), besides its two setup calls. It writes the
    // timer unconditionally (30.0 default branch) and rebuilds the 6-entry
    // checkpoint table with every hit flag (0x00636b90 + i*0x14) = 0, so the
    // hit mask and lap latch clear with it.
    rulep_.timer = Race::RuleEngine::Rule10Seed(course_id_, 0.f, &rule10_bonus_);
    rule10_hit_mask_ = 0;
    rule10_lap_seen_ = -1;
    // [D-11056] re-arm the rule-5 collectible feed each round (collectTotal
    // is fixed per-track by SetRaceRule; only the per-round done state resets).
    rulep_.collectDone = 0;
    for (auto& c : copters_) c.done = false;
    // F4: reset the race clock + the player's per-lap split records.
    race_time_ = 0.f;
    for (int k = 0; k < kMaxSplits; ++k) { split_time_[k] = 0.f; split_done_[k] = false; }
    // grid: the ORIGINAL's staggered zig-zag formation, ported verbatim from
    // FUN_00408b00 (0x00408b00, "race grid start-position calculator"; decode in
    // verify/grid_orig_ts/FUN_00408b00_decomp.txt). The prior 2x2 box was
    // invented (see :80 "instead of the original's AI tile grid") and did not
    // match the original — measured 2026-08-30, the original spawns a single-file
    // 4-rank stagger, frozen on the grid through the countdown (byte-identical
    // t=0.121..6.910s, so this IS the grid, not roll-away; verify/grid_orig_ts/).
    //
    // Original per-slot (param_3==4): pos_s = A + F*fwdOff[s] - L*latOff[s], with
    //   A = FUN_00426cb0() anchor, F = normalize(node0-node3), L = FUN_00426cc0()
    //   fwdOff = 0.8*(1-/+0.4) = {+0.48,-1.12,+1.12,-0.48}   (_cc9bc, _ccac0)
    //   latOff = {0, 1.1, 2.2, 3.3}                          (_ccabc/ab8/ab4)
    // The standalone start-line frame maps 1:1: gate0 == A (both ~(0.03,·,-2.0)),
    // lat == F, dir == L (dir~=(0,0,-1), lat~=(1,0,0), yaw=-1.576). So:
    //   pos_s = g0 + lat*latMul[s] - dir*dirMul[s]
    // reproduces the original grid to <=0.02 in x/z. See verify/grid_orig_ts/
    // ANALYSIS.md for the algebra and the measured-vs-formula check.
    const float* g0 = gates_[0].center;
    const float* g1 = gates_[1].center;
    float dir[2] = {g1[0] - g0[0], g1[2] - g0[2]};
    const float dl = std::sqrt(dir[0]*dir[0] + dir[1]*dir[1]);
    dir[0] /= dl; dir[1] /= dl;
    float lat[2] = {-dir[1], dir[0]};
    // ---- [round grid Stage A] G-RG-GRID dump (MASHED_ROUND_GRID_CHECK=<out>, once): the formation below at
    // the original's measured restart nodes (o_rounds7b: 0, 7, 11, 14, 21, 24, 28, 1) from the course block,
    // next to this gate-based formation. Measurement only.
    {
        static bool s_checked = false;
        const char* cp = std::getenv("MASHED_ROUND_GRID_CHECK");
        if (cp && !s_checked) {
            s_checked = true;
            if (std::FILE* cf = std::fopen(cp, "w")) {
                static const float kLm[4] = { 0.48f, -1.12f, 1.12f, -0.48f };
                static const float kDm[4] = { 0.00f,  1.10f, 2.20f,  3.30f };
                std::fprintf(cf, "gate g0=(%.6g,%.6g,%.6g) lat=(%.6g,%.6g) dir=(%.6g,%.6g)", g0[0], g0[1], g0[2],
                             lat[0], lat[1], dir[0], dir[1]);
                for (int s2 = 0; s2 < 4; ++s2)
                    std::fprintf(cf, " s%d=(%.4f,%.4f)", s2, g0[0] + lat[0] * kLm[s2] - dir[0] * kDm[s2],
                                 g0[2] + lat[1] * kLm[s2] - dir[1] * kDm[s2]);
                std::fprintf(cf, "\n");
                const int nNodes = *reinterpret_cast<const std::int32_t*>(0x0066d6d8u);   // node count (P1)
                for (int nd = 0; nd < nNodes && nd < 0x200; ++nd) {
                    float A[3], L2[2], D2[2];
                    if (!Race::RoundGrid_AnchorAt(nd, A, L2, D2)) { std::fprintf(cf, "node %d unavailable\n", nd); continue; }
                    std::fprintf(cf, "node %d A=(%.6g,%.6g,%.6g) lat=(%.6g,%.6g) dir=(%.6g,%.6g)", nd, A[0], A[1], A[2],
                                 L2[0], L2[1], D2[0], D2[1]);
                    for (int s2 = 0; s2 < 4; ++s2)
                        std::fprintf(cf, " s%d=(%.4f,%.4f)", s2, A[0] + L2[0] * kLm[s2] - D2[0] * kDm[s2],
                                     A[2] + L2[1] * kLm[s2] - D2[1] * kDm[s2]);
                    std::fprintf(cf, "\n");
                }
                std::fclose(cf);
            }
        }
    }
    // ---- [round grid Stage A] anchor at the restart node (default-OFF MASHED_ROUND_GRID + MASHED_PATHPROG):
    // FUN_0040e590 -> FUN_00409290(&DAT_0063ba80) -> FUN_00408b00 anchor = node centre / dir / corner axis
    // (FIX_BRIEF.md section 4 A2/A3). Same formation table; only A, lat, dir change.
    float gA[3] = { g0[0], g0[1], g0[2] };
    {
        float A[3], L2[2], D2[2]; int nd = -1;
        if (Race::RoundGrid_Anchor(A, L2, D2, &nd)) {
            gA[0] = A[0]; gA[1] = A[1]; gA[2] = A[2];
            lat[0] = L2[0]; lat[1] = L2[1]; dir[0] = D2[0]; dir[1] = D2[1];
        }
    }
    // slot == spawn order: player=0, ai0=1, ai1=2, ai2=3
    static const float kLatMul[4] = { 0.48f, -1.12f, 1.12f, -0.48f };
    static const float kDirMul[4] = { 0.00f,  1.10f, 2.20f,  3.30f };
    auto place = [&](float* pos, float* yaw, int slot) {
        const float x = gA[0] + lat[0] * kLatMul[slot] - dir[0] * kDirMul[slot];
        const float z = gA[2] + lat[1] * kLatMul[slot] - dir[1] * kDirMul[slot];
        bool ok = false;
        const float y = GroundHeight(x, z, &ok);
        pos[0] = x; pos[2] = z;
        pos[1] = (ok ? y : gA[1]) + car_ground_off_;
        *yaw = std::atan2(dir[1], dir[0]);
    };
    // [round grid Stage B] per-car formation slot (default-OFF MASHED_ROUND_GRID_ORDER): FUN_0040e590's slot value
    // & 3; otherwise slot == spawn order as before.
    int kSlot[4] = { 0, 1, 2, 3 };
    {
        int sv[4];
        if (Race::RoundGrid_SlotOrder(sv)) for (int i = 0; i < 4; ++i) kSlot[i] = sv[i] & 3;
    }
    place(car_pos_, &car_yaw_, kSlot[0]);
    car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
    car_speed_ = 0.f;
    // [U-9145 2026-09-29] MASHED_MEASURE_SOLO: StartRound RE-CREATES the opponents, so
    // gating only the load-time spawn was not enough — MATCH-SEED still logged
    // `participants=4` and the knob was inert. BOTH sites are gated; with the knob unset
    // this is `ai_cars_.assign(3, AiCar{})` and `i < 3`, byte-for-byte as before.
    static const bool s_measureSoloR = (std::getenv("MASHED_MEASURE_SOLO") != nullptr);
    const int nAi = s_measureSoloR ? 0 : 3;
    ai_cars_.assign(static_cast<std::size_t>(nAi), AiCar{});
    for (int i = 0; i < nAi; ++i) {
        AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
        place(a.pos, &a.yaw, kSlot[(i + 1) & 3]);   // ai i -> slot i+1 (Stage B: kSlot)
        a.target = 1;
        // staggered top speeds (all faster than the auto-driven player) +
        // distinct racing lanes so they spread out and overtake
        a.speed = radius_ * (0.20f + 0.03f * static_cast<float>(i));
        a.cur_speed = 0.f;
        a.lane = (static_cast<float>(i) - 1.0f) * 3.0f;   // -3, 0, +3
    }
    countdown_ = 3.0f;   // 3-2-1 freeze before GO
    // [D-11073 leg 1b, PREREG_LEG1B.md addendum B] Under MASHED_SUBSTATE every round re-enters
    // the sub-state machine. The original's next round goes case 8 (FUN_00414220) -> 2 -> the
    // state-2 tail, and because FUN_004103a0 zeroed DAT_005f29b8 at 0x004104c0 it takes the
    // else arm: DAT_0063ba8c = 4, DAT_005f29b8 = 12000 (no phase 3). The countdown block's
    // ba8c == 0 entry performs exactly that tail. Knob OFF: nothing here runs.
    if (SubState_Enabled()) *reinterpret_cast<int*>(0x0063ba8cu) = 0;
    Audio::SfxPlay("threetwoone", 0.9f);   // real countdown SFX (permdict)
    race_cam_.Reset();   // force_reset path on the first camera tick
    // clear any in-flight power-up effects from the previous round
    missiles_.clear();
    mines_.clear();
    boost_timer_ = shield_timer_ = 0.f;
}

namespace {
// MASHED_RULE_ENGINE=0 is the only way to reach the legacy two-objective
// collapse. G-G2 (D3 2026-09-26): read here AND in SetRaceRule, so a race that
// is started without SetRaceRule (the MASHED_ROUND dev route with no
// MASHED_ROUND_RULE) runs the rule engine too. The original has no engine-off
// state: FUN_00411170 calls FUN_00410d10 / FUN_00410510 unconditionally.
bool RuleEngineEnabled() {
    const char* e = std::getenv("MASHED_RULE_ENGINE");
    return !(e && e[0] == '0');
}
}  // namespace

// Match start. G-G1 (D3 2026-09-26): there is no round-count parameter. The
// original ends a match inside FUN_00410510 on a score target (0x00410510
// first loop: score == 8 when DAT_008a94d0 is 2/3, FUN_0042f500() != 0 or
// DAT_007f0fd0 == 2; score > 0xb when DAT_008a94d0 == 4), counted from the
// FUN_0040b180 seed. FUN_00411170 has no round counter of its own: segment
// over -> FUN_00410510 != 0 ends the match, else DAT_0063ba8c = 7 (next round).
void TrackRenderer::StartMatch() {
    match_winner_ = -1;
    round_no_ = 1;
    // [rtex] the car records are zeroed at per-car init (FUN_00420420's memset of 0x2ac at
    // 0x00420420) and the race frame counter at the race clock reset (0x0040ff17)
    for (CarGloss& g : car_gloss_) g = CarGloss{};
    gloss_frame_ = 0;
    Race::RoundGrid_RaceInit();   // [round grid Stage A] 0x0040fef0 DAT_0063ba80 = 0 (MASHED_ROUND_GRID)
    rule_engine_on_ = RuleEngineEnabled();
    for (int i = 0; i < kRaceCars; ++i) {
        // score_delta_ (DAT_008a9520) stays 0 here. FUN_0040b180 writes
        // 0xfffffc18 (-1000) to it, but the standings circle picker
        // (exe_main.cpp, `d <= -2` -> MinusTwo) is not the original's
        // FUN_0040b420 classifier, so -1000 would draw a circle the original's
        // classifier may not. [UNCERTAIN U-9137] what the original draws for -1000;
        // open in re/analysis/D3_MODES_2026-09-26.md.
        score_delta_[i] = 0;
        delta_timer_[i] = 0.f;              // DAT_008a9510..1c = 0 (0x0040b180)
        elim_order_[i] = -1;
    }
    SeedMatchScores();
    elim_count_ = 0;
    StartRound();
}

// DAT_008a94d0 equivalent (FUN_004111c0 0x0040ff40..0x0040ffb6: zeroed, then +1
// per slot whose *(PTR_005f2770 + 0x34 + i*4) probe is set). The standalone's
// occupied slots are the ones UpdateRace gives a position (P2[i] != nullptr):
// the player plus up to three AI cars.
int TrackRenderer::ParticipantCount() const {
    const int ai = static_cast<int>(ai_cars_.size());
    return 1 + (ai < kRaceCars - 1 ? ai : kRaceCars - 1);
}

// 0x0040b180 (sole caller FUN_004111c0, CALL @0x0040ffb8, directly after the
// DAT_008a94d0 count). seed = 6; FUN_0040e340() (= DAT_008a94d0) == 2 -> 4;
// == 3 -> 4; FUN_0042f500() (= DAT_0067ea64, team play) != 0 -> 4;
// DAT_007f0fd0 == 1 || == 2 -> 4. Writes the seed to the score array
// DAT_008a94e0..0x008a94ec and to the previous-score array 0x008a9570..7c.
void TrackRenderer::SeedMatchScores() {
    const int participants = ParticipantCount();
    int seed = 6;
    if (participants == 2) seed = 4;
    if (participants == 3) seed = 4;
    if (team_play_) seed = 4;
    if (rule_ == 1 || rule_ == 2) seed = 4;
    for (int i = 0; i < kRaceCars; ++i)
        scores_[i] = score_prev_[i] = seed;
    // setup-time only (StartMatch / SetRaceRule / SetTeamPlay), not per frame
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "MATCH-SEED rule=%d participants=%d teams=%d seed=%d "
                         "engine=%d\n", rule_, participants, team_play_ ? 1 : 0,
                     seed, rule_engine_on_ ? 1 : 0);
        std::fclose(lf);
    }
}

void TrackRenderer::InitPickups() {
    const float R = track_radius_ > 1.f ? track_radius_ : radius_;
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "R4 pickups: %zu real powerup spawns (POWERUPS_GOLD.DFF)\n",
                     powerup_spawns_.size());
        std::fclose(lf);
    }
    // No gate-ribbon fallback. The original has no such behaviour: it places
    // from POWERUPS_GOLD.DFF, falls back to the Lua only when the clump is null
    // (FUN_004264d0 JE 0x004265df), and otherwise places nothing. The old
    // "every 8th AI gate gets an orb" path was invented, and it is what put
    // orbs on Neustein / sands / Storm / SuperG, where the authored boxes sit
    // somewhere else entirely (Storm measured: 7 gate-centre orbs standalone vs
    // 7 authored boxes in the original). PickupField::Init() is kept for dev
    // use but is no longer on the race path.
    pickups_.InitReal(powerup_spawns_, R);
}

// [D-11052] arm the per-rule win-condition engine for the race that is about
// to start. rule = the real DAT_007f0fd0 race rule (0..10) derived by
// Race/RaceModes. MASHED_RULE_ENGINE=0 reverts to the legacy two-objective
// collapse. Rule 10 seeds its countdown + checkpoint bonus from the course id
// (FUN_004046a0; difficulty _DAT_0089a360 fed as 0.0 — the fresh-race default
// before the FUN_00414060 rubber-band seed, which is not modeled standalone).
// Finish order / timers reset here (FUN_00414060 runs on the race-SETUP path:
// FUN_0042c960 -> FUN_004111c0 -> FUN_00414060). (U-9005 RESOLVED 2026-07-02:
// the original ALSO re-inits the finish order per round restart — witnessed
// live, rules oracle round-resets=1 — so StartRound() now mirrors that.)
void TrackRenderer::SetRaceRule(int rule) {
    rule_ = rule;
    rule_engine_on_ = RuleEngineEnabled();
    rulep_ = Race::RuleEngine::Persist{};             // FUN_00414060: slots=-1.0
    rulep_.timer = Race::RuleEngine::Rule10Seed(course_id_, 0.f, &rule10_bonus_);
    rule10_hit_mask_ = 0;
    rule10_lap_seen_ = -1;
    match_draw_ = false;
    // [D-11056] rule-5 collectible feed (U-8997 RESOLVED 2026-07-04): total =
    // the KTC_NewCopter gameplay copters loaded for this track (registrar
    // FUN_00405780's per-object DAT_0063a5d0 increments, one per KTC_NewCopter
    // command; confirmed disjoint from KTC_AddPickUp's separate FUN_00405730
    // registrar). Each copter reports done via OnCollect() on completing one
    // flight-path traversal (UpdateRace), mirroring FUN_004064c0's per-object
    // DAT_0063a5d4 completion tick. See
    // re/analysis/d11056_fable_ghidra_findings_2026-07-03.md.
    {
        int collect_total = 0;
        for (auto& c : copters_) {
            c.done = false;
            if (c.gameplay) ++collect_total;
        }
        SetCollectibles(collect_total);
    }
    // Fallback: a track assigned rule 5 with zero KTC_NewCopter copters could
    // never satisfy CollectAllDone (0x00405890: total==0 -> false), stalling
    // the round forever. Disable the rule engine for THIS race only (per-race
    // flag; the rule_engine_on_ master latch is untouched) so the legacy
    // laps/elimination collapse in UpdateRace terminates rounds instead.
    // Re-armed true here on every new race.
    rule_engine_race_on_ = true;
    if (rule_ == 5 && rulep_.collectTotal == 0) {
        rule_engine_race_on_ = false;
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "rule-5: track has no KTC_NewCopter collectibles -- "
                             "legacy flow fallback\n");
            std::fclose(lf);
        }
    }
    // FUN_0040b180 reads DAT_007f0fd0: re-seed now that the rule is known
    // (RaceSession::Begin calls StartMatch before SetRaceRule; no score has
    // been awarded yet at either call).
    SeedMatchScores();
}

// 0x0040b290 (standard path, mode 0 / no teams): prev snapshot, signed delta
// display + 6000 ms timer, score += delta, floor at 0. (Modes 1/2 gating and
// the replay event ring are not ported — standalone round is mode 0.)
void TrackRenderer::ScoreAward(int car, int delta) {
    if (car < 0 || car >= kRaceCars) return;
    score_prev_[car] = scores_[car];        // DAT_008a9570
    score_delta_[car] = delta;              // DAT_008a9520 (signed display)
    scores_[car] += delta;                  // DAT_008a94e0
    delta_timer_[car] = 6000.f;             // DAT_008a9510
    if (scores_[car] < 0) scores_[car] = 0;
}

// 0x0040eee0, DAT_008a94d0 == 4 path (4 participants, mode 0, no teams),
// delta = 1; called AFTER the victim's alive flag drops (matching the
// original's FUN_00422fd0-then-callback order). AI auto-resolve branches
// (FUN_0040e470 type-2 fast-forward) are not ported — the player is always
// in our round.
void TrackRenderer::SetTeamPlay(bool on, const int team_of[4]) {
    team_play_ = on;
    for (int i = 0; i < kRaceCars; ++i) team_of_[i] = team_of ? team_of[i] : -1;
    SeedMatchScores();       // FUN_0040b180 reads FUN_0042f500() (team play)
}

// 0x00408ad0 equivalent: race progress on a 0..100 scale. Same expression the
// camera feed uses at the RaceCamCar fill (`prog_in_lap / n * 100`), factored
// out so the tie-break below reads the identical value the elimination rule
// does rather than a second approximation of it.
float TrackRenderer::RacePct(int car) const {
    if (car < 0 || car >= kRaceCars || gates_.empty()) return 0.f;
    const float n = static_cast<float>(gates_.size());
    return std::fmod(race_[car].progress, n) / n * 100.f;
}

// The wrap-adjusted "is A behind B?" compare, 0x00410f4a..0x00410fa6 and the
// repeated blocks inside FUN_0040eee0. Bands 80.0 (_DAT_005cc730) / 20.0
// (_DAT_005ccd6c), period 100.0 (_DAT_005cc568). Only `a` is (float)-cast in
// the original; both are already float here.
bool TrackRenderer::ProgBehind(int A, int B) const {
    float a = RacePct(A), b = RacePct(B);
    if ((a <= 80.f) || (20.f <= b)) {
        if ((80.f < b) && (a < 20.f)) b = b - 100.f;
    } else {
        a = a - 100.f;
    }
    return a <= b;
}

// FUN_00422fd0 equivalent at the standalone's own liveness: drop the car and
// stop its AI, mirroring what the UpdateRace call site does for the camera's
// own victim. Used only by the team arms, which eliminate cars themselves.
void TrackRenderer::KillCar(int car) {
    if (car < 0 || car >= kRaceCars || !race_[car].alive) return;
    race_[car].alive = false;
    --round_alive_;
    if (car > 0 && car - 1 < static_cast<int>(ai_cars_.size()))
        ai_cars_[static_cast<std::size_t>(car - 1)].speed = 0.f;
}

// Award +delta to every ASSIGNED slot on `winTeam` and -delta to every other
// assigned slot. This is the shape all three team arms end in:
//     if (piVar8[-1] != -1) FUN_0040b290(i, team(i) == winTeam ? +d : -d)
// with the guard being "the slot has a player", which team_of_ == -1 carries.
void TrackRenderer::AwardByTeam(int winTeam, int delta) {
    for (int i = 0; i < kRaceCars; ++i) {
        if (team_of_[i] == -1) continue;
        ScoreAward(i, (team_of_[i] == winTeam) ? delta : -delta);
    }
    // Instrumented, not inferred: a team award that never fires and a team
    // award that fires with the wrong winner look identical in the standings
    // (both leave a plausible-looking score column).
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf,
            "TEAM_AWARD win=%d delta=%d alive=%d teams=%d,%d,%d,%d "
            "scores=%d,%d,%d,%d\n",
            winTeam, delta, round_alive_,
            team_of_[0], team_of_[1], team_of_[2], team_of_[3],
            scores_[0], scores_[1], scores_[2], scores_[3]);
        std::fclose(lf);
    }
}

// 0x0040eee0, DAT_008a94d0 == 4 with FUN_0042f500() != 0 (TEAM PLAY).
// Three arms, by cars still alive AFTER the victim dropped:
//
//   3 alive -- fires ONLY when all three are on the SAME team (the original
//       counts matches against the first alive car's team and acts at 3). It
//       then eliminates two of them by the progress compare and awards
//       +/-delta by team. In a 2v2 this arm cannot fire; it exists for the
//       1v3 split MenuTeamBalance also accepts.
//   2 alive -- if the two are on the same team, eliminate the one behind and
//       award +/-1 by that team. If they are on DIFFERENT teams the original
//       does NOTHING: no award, no elimination. Transcribed as written.
//   1 alive -- award +/-1 by the survivor's team, then equalize every car's
//       path progress to the survivor's (LAB_0040fbbb, FUN_00408a50/70).
//
// NOTE the delta asymmetry, which is in the original and is not smoothed: the
// 3-alive arm awards +/-param_2 while the 2- and 1-alive arms award a literal
// +/-1 (uVar12 = 1 / 0xffffffff). The standalone's caller passes delta = 1, so
// the two agree here, but the code says what the original says.
// The victim itself gets NO direct penalty in team play, unlike the FFA arms.
void TrackRenderer::ScoreOnEliminationTeams(int victim) {
    (void)victim;                       // team arms never score the victim
    const int remaining = round_alive_;
    const int delta = 1;                // param_2 from 0x00410d10
    // Arm-entry counters, armed BEFORE the first run: an arm that never
    // executes and an arm that executes and awards nothing are the same
    // silence in the score column, and only the 1-alive arm fires in a typical
    // round. TEAM_ELIM is one line per elimination; the arms are distinguished
    // by `alive`, and TEAM_COLLAPSE marks the same-team cases that eliminate a
    // car of their own.
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "TEAM_ELIM victim=%d alive=%d teams=%d,%d,%d,%d\n",
                     victim, remaining,
                     team_of_[0], team_of_[1], team_of_[2], team_of_[3]);
        std::fclose(lf);
    }

    if (remaining == 3) {
        int count = 0, teamFirst = -1, first = -1, second = -1;
        for (int car = 0; car < kRaceCars; ++car) {
            if (!race_[car].alive) continue;
            if (teamFirst == -1) {
                teamFirst = team_of_[car]; count = 1; first = car;
            } else if (teamFirst == team_of_[car]) {
                if (count == 1) second = car;
                ++count;
                if (count == 3) {
                    // The original's three-way pick, goto structure preserved.
                    int keep, drop;
                    if (ProgBehind(car, first)) {
                        KillCar(car);
                        keep = second; drop = first;
                        if (!ProgBehind(first, second)) drop = keep;
                    } else {
                        KillCar(first);
                        keep = car; drop = second;
                        if (ProgBehind(car, second)) drop = keep;
                    }
                    KillCar(drop);
                    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                        std::fprintf(lf, "TEAM_COLLAPSE arm=3 team=%d kept=%d dropped=%d,%d\n",
                                     team_of_[car], keep, first, drop);
                        std::fclose(lf);
                    }
                    AwardByTeam(team_of_[car], delta);
                }
            }
        }
        return;
    }

    if (remaining == 2) {
        int teamFirst = -1, firstCar = -1;
        for (int car = 0; car < kRaceCars; ++car) {
            if (!race_[car].alive) continue;
            if (teamFirst == -1) {
                teamFirst = team_of_[car]; firstCar = car;
            } else if (teamFirst == team_of_[car]) {
                const int winTeam = team_of_[car];   // captured before the swap
                int loser = car;
                if (!ProgBehind(car, firstCar)) loser = firstCar;
                KillCar(loser);
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "TEAM_COLLAPSE arm=2 team=%d loser=%d\n",
                                 winTeam, loser);
                    std::fclose(lf);
                }
                AwardByTeam(winTeam, 1);
            }
        }
        return;
    }

    if (remaining == 1) {
        int survivor = -1;
        for (int i = 0; i < kRaceCars; ++i) if (race_[i].alive) survivor = i;
        if (survivor < 0) return;
        AwardByTeam(team_of_[survivor], 1);
        // LAB_0040fbbb: every car's path progress is set to the survivor's.
        const float p = race_[survivor].progress;
        for (int i = 0; i < kRaceCars; ++i) race_[i].progress = p;
    }
}

void TrackRenderer::ScoreOnElimination(int victim) {
    if (elim_count_ < kRaceCars) elim_order_[elim_count_++] = victim;  // DAT_008a94c0
    if (team_play_) { ScoreOnEliminationTeams(victim); return; }
    const int remaining = round_alive_;
    if (remaining == 3) {
        ScoreAward(victim, -2);             // param_2 * -2
    } else if (remaining == 2) {
        ScoreAward(victim, -1);             // -param_2
    } else if (remaining == 1) {
        int survivor = -1;
        for (int i = 0; i < kRaceCars; ++i)
            if (race_[i].alive) survivor = i;
        // runner-up +1, zeroed when its score already exceeds 10
        ScoreAward(victim, (scores_[victim] > 10) ? 0 : 1);   // FUN_0040b6d0 cap
        ScoreAward(survivor, 2);
    }
}

void TrackRenderer::NextRoundOrEnd() {
    if (round_winner_ < 0) return;
    // Race::EvaluateResult (0x00410510): with 4 participants the match ends
    // when an alive player's score exceeds 11.
    for (int i = 0; i < kRaceCars; ++i)
        if (scores_[i] > 11) { match_winner_ = i; return; }
    ++round_no_;
    StartRound();   // re-grid for the next round
}

// [D-11073 leg 2] FUN_0040d470 -> FUN_00448220 -> FUN_00446520(&DAT_00897fe0, reset) on a
// sub-mode != 6 frame (states 3, 4, 5 under MASHED_SUBSTATE): FUN_00442600 (0x0044655c, guarded),
// FUN_004464c0 (0x004468f9; the entries read cam +4..+0xc from the PREVIOUS update), the camera
// main block when cam[7] == 0 (RaceCamera::Update, the same block the sub-mode-6 path runs), then
// the copy cam+0x40..+0x48 -> cam+4..+0xc (0x004481c9; RaceCamera keeps +0x40 as pos_out_).
// Named gaps: cam[7] != 0 runs FUN_004427c0 in the original (not ported; cam[7] is 0 here, set by
// FUN_00448700(0,0)); FUN_00442a20 (main block, flag != 0) is not in RaceCamera; cam +0x10..+0x18
// (FUN_00445aa0 orientation only) is not written.
void TrackRenderer::SubStateCameraChain(bool reset) {
    static bool s_hostSet = false;
    if (!s_hostSet) {
        static TrackRenderer* s_tr = nullptr;
        s_tr = this;
        CamEntryHost h{};
        h.track_id = [] { return aib_track_index(); };
        h.most_separated = [](int* a, int* b) {
            Race::RaceCamCar cc[kRaceCars] = {};
            if (s_tr) s_tr->FillRaceCamCars(cc);
            Race::RaceCamera::MostSeparatedPair(cc, a, b);
            if (*a < 0) *a = 0;
            if (*b < 0) *b = 0;
        };
        CamEntry_SetHost(h);
        s_hostSet = true;
    }
    CamEntriesInit442600();                                        // 0x0044655c
    CameraEntryDispatchAll(0x00897fe0u);                           // 0x004468f9
    if (*reinterpret_cast<int*>(0x00897ffcu) == 0) {               // cam[7] gate
        Race::RaceCamCar cc[kRaceCars] = {};
        FillRaceCamCars(cc);
        // [U-9201] diagnostic replay (MASHED_CAM_TICKS_REPLAY=<file>, lines "f ticks"): phase-3 calls read
        // the original run's DAT_007f1030 (QPC/3 MHz, FUN_00493390 0x00493459) at game frame
        // f = max(p3f - 1, 0). Off by default; the gate arms and the default build never load it.
        static std::vector<double> s_replay = [] {
            std::vector<double> v; const char* e = std::getenv("MASHED_CAM_TICKS_REPLAY");
            if (e && e[0]) if (std::FILE* rf = std::fopen(e, "r")) {
                int f; double t;
                while (std::fscanf(rf, "%d %lf", &f, &t) == 2) { if (f >= 0) { if ((int)v.size() <= f) v.resize(f + 1, -1.0); v[f] = t; } }
                std::fclose(rf);
            }
            return v; }();
        const int rf = sub_p3_frames_ > 0 ? sub_p3_frames_ - 1 : 0;
        if (!s_replay.empty() && *reinterpret_cast<int*>(0x0063ba8cu) == 3 && rf < (int)s_replay.size() && s_replay[rf] >= 0.0)
            cam_ticks_ = s_replay[rf];
        else
            cam_ticks_ += (1.0 / 60.0) * 3000000.0;                // DAT_007f1030 rate, as UpdateRace
        race_cam_.Update(cc, course_id_, 1.f / 60.f, static_cast<std::uint32_t>(cam_ticks_), 0.f, reset);
        const float* p = race_cam_.pos();                          // +0x40..+0x48
        std::memcpy(reinterpret_cast<void*>(0x00897fe4u), p, 12);  // 0x004481c9 copy -> +4..+0xc
    }
    static std::FILE* s_tf = [] { const char* e = std::getenv("MASHED_LEG2_TRACE");
                                  std::FILE* f = (e && e[0]) ? std::fopen(e, "w") : nullptr;
                                  if (f) std::fprintf(f, "p3f,ba8c,reset,u1008,flag,tx,ty,tz,e0x,e0y,e0z,e0vx,e0vz,e0a8,dist_raw,cam_ticks,s964,s968,s96c,s970,s974,s978,s97c,s980,s984,s988,s98c,s990,c99c,c9a0,c994,c998,ctx,cty,ctz\n");
                                  return f; }();
    if (s_tf) {
        const float* c = reinterpret_cast<const float*>(0x00897fe0u);
        const float* e0 = reinterpret_cast<const float*>(0x008964c0u);
        const float dx = e0[0x3c / 4] - c[1], dz = e0[0x44 / 4] - c[3];
        std::fprintf(s_tf, "%d,%d,%d,%d,%d,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%d,%.9g,", sub_p3_frames_,
                     *reinterpret_cast<int*>(0x0063ba8cu), reset ? 1 : 0, *reinterpret_cast<int*>(0x007f1008u),
                     *reinterpret_cast<int*>(0x00897fe0u), c[1], c[2], c[3], e0[0x3c / 4], e0[0x40 / 4],
                     e0[0x44 / 4], e0[0x8c / 4], e0[0x94 / 4], *reinterpret_cast<const int*>(&e0[0xa8 / 4]),
                     std::sqrt(dx * dx + dz * dz));
        float st[14], tg[3]; int pa = -9, pb = -9;
        race_cam_.DebugState(st, &pa, &pb, tg);
        std::fprintf(s_tf, "%.0f", cam_ticks_);
        for (int i = 0; i < 14; ++i) std::fprintf(s_tf, ",%.9g", st[i]);
        std::fprintf(s_tf, ",%d,%d,%.9g,%.9g,%.9g\n", pa, pb, tg[0], tg[1], tg[2]);
        std::fflush(s_tf);
    }
}

// [D-11073 leg 2] The RaceCamCar adapter UpdateRace has always used, factored out so the
// MASHED_SUBSTATE pre-race camera chain feeds RaceCamera the same quantities. No behaviour change.
void TrackRenderer::FillRaceCamCars(Race::RaceCamCar* cc) {
    if (gates_.empty()) return;
    const int n = static_cast<int>(gates_.size());
    const float* P2[kRaceCars] = {car_pos_, nullptr, nullptr, nullptr};
    for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
        P2[i + 1] = ai_cars_[static_cast<std::size_t>(i)].pos;
    for (int i = 0; i < kRaceCars; ++i) {
        Race::RaceCamCar& c = cc[i];
        c.active = (P2[i] != nullptr);
        c.alive = race_[i].alive && c.active;
        if (!c.active) continue;
        for (int k = 0; k < 3; ++k) c.pos[k] = P2[i][k];
        if (i == 0) {
            for (int k = 0; k < 3; ++k) c.vel[k] = car_vel_[k];
        } else {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(i - 1)];
            c.vel[0] = std::cos(a.yaw) * a.cur_speed;
            c.vel[2] = std::sin(a.yaw) * a.cur_speed;
        }
        // dead_flag/dead_ms: the scaffold has no dying state (cars vanish on
        // elimination); the original's explosion window is not modeled yet.
        const float prog_in_lap =
            std::fmod(race_[i].progress, static_cast<float>(n));
        c.path_prog = prog_in_lap;                       // 0x00408a50 equiv
        c.race_pct = prog_in_lap / static_cast<float>(n) * 100.f;  // 0x00408ad0
    }
}

// [d1r2 2026-10-16] MASHED_CAM_REPLAY=<csv>, DIAGNOSTIC ONLY, default OFF: each
// race_cam_ update consumes the next row of an ORIGINAL-side trace
// (re/frida/camera_probe.py, columns read by header like cam_driver.cpp) in place of
// the host inputs: the four cars' pos/vel/alive/dead/deadt/prog/pct and the
// ticks/dtb/jit/reset/overhead arguments. So the standalone's camera, and the frame
// the renderer draws from it, can be compared with the original frame by frame.
// The standalone's own race keeps running underneath; only the camera is fed.
// Adapters as cam_driver.cpp: active = (alive != -1), dead_ms = the int tick count.
namespace {
struct CamReplay {
    bool on = false;
    std::vector<std::vector<double>> rows;
    std::size_t next = 0;
    int c_x[4], c_y[4], c_z[4], c_vx[4], c_vy[4], c_vz[4], c_al[4], c_de[4], c_dt[4], c_pr[4], c_pc[4];
    int c_tk = -1, c_dtb = -1, c_jit = -1, c_rst = -1, c_oh = -1, c_tt = -1;
    // [d1r1] optional pose-transplant columns (camera_probe.py --frame / --carframes):
    // M_* = the RwCamera frame's modelling matrix at director onLeave (what renders, R2),
    // rvwy = its view window y; F{i}_* = car i's renderable frame LTM (*(0x0063da18 +
    // i*0x2ac) -> +4 -> +0x50), measured equal to that car's body draws' D3DTS_WORLD in the
    // same frame (verify/d1r1_20261016). -1 when the trace lacks them.
    int m_c[12], c_vwy = -1, f_c[4][12];
    bool has_m = false, has_f = false;
    int q_c[4][12];             // [librwrest] Q{i}_*: the car clump root's LTM
    bool has_q = false;
    int c_ba8c = -1;            // [rfx] the HUD replay reads the trace's ba8c / dtb columns
};
// [d1r1] the consumed row's transplant values, read by Render under MASHED_CAM_REPLAY_BASIS /
// MASHED_CAR_REPLAY (diagnostic knobs, default OFF)
float s_rep_basis[12];          // px,py,pz, rx,ry,rz, ux,uy,uz, ax,ay,az (the MASHED_CAM_POSE order)
float s_rep_vwy = 0.f;
bool  s_rep_basis_ok = false;
float s_rep_car[4][12];         // rx,ry,rz, ux,uy,uz, ax,ay,az, px,py,pz
bool  s_rep_car_ok = false;
float s_rep_root[4][12];        // [librwrest] Q{i}_* (clump root LTM), same order; [rfx] the car
                                // shadow's root (equal to R{i}_* for a root frame)
bool  s_rep_root_ok = false;
CamReplay& CamReplayState() {
    static CamReplay R = [] {
        CamReplay r;
        const char* e = std::getenv("MASHED_CAM_REPLAY");
        if (!e || !e[0]) return r;
        std::FILE* f = std::fopen(e, "r");
        if (!f) return r;
        static char line[16384];
        std::vector<std::string> hdr;
        auto split = [](char* s, std::vector<char*>& out) {
            out.clear(); out.push_back(s);
            for (; *s; ++s) {
                if (*s == '\r' || *s == '\n') { *s = 0; break; }
                if (*s == ',') { *s = 0; out.push_back(s + 1); }
            }
        };
        std::vector<char*> f2;
        if (std::fgets(line, sizeof(line), f)) {
            split(line, f2);
            for (char* h : f2) hdr.emplace_back(h);
        }
        auto col = [&](const std::string& n) {
            for (std::size_t i = 0; i < hdr.size(); ++i) if (hdr[i] == n) return static_cast<int>(i);
            return -1;
        };
        bool ok = true;
        auto need = [&](const std::string& n) { const int c = col(n); if (c < 0) ok = false; return c; };
        for (int i = 0; i < 4; ++i) {
            const std::string p = "c" + std::to_string(i);
            r.c_x[i] = need(p + "x");   r.c_y[i] = need(p + "y");   r.c_z[i] = need(p + "z");
            r.c_vx[i] = need(p + "vx"); r.c_vy[i] = need(p + "vy"); r.c_vz[i] = need(p + "vz");
            r.c_al[i] = need(p + "alive"); r.c_de[i] = need(p + "dead"); r.c_dt[i] = need(p + "deadt");
            r.c_pr[i] = need(p + "prog");  r.c_pc[i] = need(p + "pct");
        }
        r.c_tk = need("ticks"); r.c_dtb = need("dtb"); r.c_jit = need("jit");
        r.c_rst = need("reset"); r.c_oh = need("overhead"); r.c_tt = need("track_type");
        {
            static const char* kM12[12] = {"rx","ry","rz","ux","uy","uz","ax","ay","az","px","py","pz"};
            r.has_m = true; r.has_f = true; r.has_q = true;
            for (int k = 0; k < 12; ++k) {
                r.m_c[k] = col(std::string("M_") + kM12[k]);
                if (r.m_c[k] < 0) r.has_m = false;
                for (int i = 0; i < 4; ++i) {
                    r.f_c[i][k] = col("F" + std::to_string(i) + "_" + kM12[k]);
                    if (r.f_c[i][k] < 0) r.has_f = false;
                    r.q_c[i][k] = col("Q" + std::to_string(i) + "_" + kM12[k]);
                    if (r.q_c[i][k] < 0) r.has_q = false;
                }
            }
            r.c_vwy = col("rvwy");
            if (r.c_vwy < 0) r.has_m = false;
            r.c_ba8c = col("ba8c");
        }
        while (ok && std::fgets(line, sizeof(line), f)) {
            split(line, f2);
            if (f2.size() < hdr.size()) continue;
            std::vector<double> v(f2.size());
            for (std::size_t i = 0; i < f2.size(); ++i) v[i] = std::atof(f2[i]);
            r.rows.push_back(std::move(v));
        }
        std::fclose(f);
        r.on = ok && !r.rows.empty();
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "[d1r2] MASHED_CAM_REPLAY %s: %s, %zu rows, track_type %d\n", e,
                         ok ? "columns OK" : "MISSING COLUMNS (replay off)", r.rows.size(),
                         r.on ? static_cast<int>(r.rows[0][static_cast<std::size_t>(r.c_tt)]) : -1);
            std::fclose(lf);
        }
        return r;
    }();
    return R;
}
}  // namespace

void TrackRenderer::RaceCamUpdate(const Race::RaceCamCar* cc_in, bool reset) {
    Race::RaceCamCar cc[kRaceCars];
    for (int i = 0; i < kRaceCars; ++i) cc[i] = cc_in[i];
    float dtb = 1.f / 60.f;                                   // DAT_007f100c live
    float jit = 0.f;                                          // DAT_007f0fc8 live
    bool overhead = false;                                    // DAT_007f0f38
    std::uint32_t ticks = static_cast<std::uint32_t>(cam_ticks_);
    CamReplay& R = CamReplayState();
    if (R.on) {
        if (R.next < R.rows.size()) {
            const std::vector<double>& v = R.rows[R.next];
            auto at = [&](int c) { return v[static_cast<std::size_t>(c)]; };
            for (int i = 0; i < kRaceCars; ++i) {
                cc[i].pos[0] = static_cast<float>(at(R.c_x[i]));
                cc[i].pos[1] = static_cast<float>(at(R.c_y[i]));
                cc[i].pos[2] = static_cast<float>(at(R.c_z[i]));
                cc[i].vel[0] = static_cast<float>(at(R.c_vx[i]));
                cc[i].vel[1] = static_cast<float>(at(R.c_vy[i]));
                cc[i].vel[2] = static_cast<float>(at(R.c_vz[i]));
                const int alive = static_cast<int>(at(R.c_al[i]));
                cc[i].active    = (alive != -1);
                cc[i].alive     = (alive == 1);
                cc[i].dead_flag = (static_cast<int>(at(R.c_de[i])) != 0);
                cc[i].dead_ms   = static_cast<float>(at(R.c_dt[i]));
                cc[i].path_prog = static_cast<float>(at(R.c_pr[i]));
                cc[i].race_pct  = static_cast<float>(at(R.c_pc[i]));
            }
            ticks = static_cast<std::uint32_t>(at(R.c_tk));
            dtb = static_cast<float>(at(R.c_dtb));
            jit = static_cast<float>(at(R.c_jit));
            reset = static_cast<int>(at(R.c_rst)) != 0;
            overhead = static_cast<int>(at(R.c_oh)) != 0;
            // [d1r1] keep this row's transplant values for Render (knob-gated there)
            s_rep_basis_ok = R.has_m;
            if (R.has_m) {
                // M_ order is right,up,at,pos; MASHED_CAM_POSE order is pos,right,up,at
                for (int k = 0; k < 3; ++k) {
                    s_rep_basis[k]     = static_cast<float>(at(R.m_c[9 + k]));
                    s_rep_basis[3 + k] = static_cast<float>(at(R.m_c[k]));
                    s_rep_basis[6 + k] = static_cast<float>(at(R.m_c[3 + k]));
                    s_rep_basis[9 + k] = static_cast<float>(at(R.m_c[6 + k]));
                }
                s_rep_vwy = static_cast<float>(at(R.c_vwy));
            }
            s_rep_car_ok = R.has_f;
            if (R.has_f)
                for (int i = 0; i < 4; ++i)
                    for (int k = 0; k < 12; ++k)
                        s_rep_car[i][k] = static_cast<float>(at(R.f_c[i][k]));
            s_rep_root_ok = R.has_q;
            if (R.has_q)
                for (int i = 0; i < 4; ++i)
                    for (int k = 0; k < 12; ++k)
                        s_rep_root[i][k] = static_cast<float>(at(R.q_c[i][k]));
            // [rfx] the HUD replay input: this row's DAT_0063ba8c and DAT_007f100c
            if (R.c_ba8c >= 0)
                LibRw::RaceFx_ReplayHudRow(static_cast<int>(at(R.c_ba8c)), dtb);
            cam_replay_row_ = static_cast<int>(R.next++);
        } else {
            cam_replay_row_ = -2;                             // trace exhausted: host inputs again
            s_rep_basis_ok = s_rep_car_ok = s_rep_root_ok = false;
        }
    }
    race_cam_.Update(cc, course_id_, dtb, ticks, jit, reset, overhead);
}

void TrackRenderer::UpdateRace(float dt) {
    if (gates_.empty()) return;
    const int n = static_cast<int>(gates_.size());
    // [D-11072 leg A] (re)build the physical arc-length table when it does not
    // match the current gate ring (first race, or a track reload).
    if (static_cast<int>(gate_seglen_.size()) != n) {
        gate_seglen_.assign(static_cast<std::size_t>(n), 0.f);
        gate_cumlen_.assign(static_cast<std::size_t>(n), 0.f);
        float acc = 0.f;
        for (int a = 0; a < n; ++a) {
            gate_cumlen_[static_cast<std::size_t>(a)] = acc;
            const float* ga = gates_[static_cast<std::size_t>(a)].center;
            const float* gb = gates_[static_cast<std::size_t>((a + 1) % n)].center;
            const float dxs = gb[0] - ga[0], dzs = gb[2] - ga[2];
            const float sl = std::sqrt(dxs * dxs + dzs * dzs);
            gate_seglen_[static_cast<std::size_t>(a)] = sl;
            acc += sl;
        }
        total_len_ = acc;
    }
    if (countdown_ <= 0.f) race_time_ += dt;   // F4 race clock (seconds since GO)
    auto step = [&](int i, const float* pos) {
        RaceCar& r = race_[i];
        if (!r.alive) return;
        // [G4] Robust forward-projection progress (replaces the strict 3.0-unit proximity
        // to gate[r.gate], which the AI cars — following the race-line SPLINE, offset from
        // the gate ribbon — often never satisfied, so r.gate stalled and no lap/race ever
        // completed). Project the car onto the NEAREST gate within a forward window and
        // advance r.gate through the intervening gates (forward only — no shortcut/backward
        // jumps), running the same lap-line + split logic on each gate left behind. A lap
        // completes on the LAPDATA lap-line set (FUN_00426cf0 anti-shortcut) when present,
        // else on a start/finish wrap (gate n-1 -> 0).
        const int W = 15;                        // forward search window (gates)
        int best = r.gate; float bestD = 1e18f;
        for (int k = 0; k <= W; ++k) {
            int gi = (r.gate + k) % n;
            const float* gg = gates_[static_cast<std::size_t>(gi)].center;
            const float ex = gg[0] - pos[0], ez = gg[2] - pos[2];
            const float ed = ex * ex + ez * ez;
            if (ed < bestD) { bestD = ed; best = gi; }
        }
        int guard = 0;
        while (r.gate != best && guard++ < W + 2) {
            const int G = r.gate;                // forward advance onto G+1
            bool lapped = false;
            if (!lap_data_.lap_lines.empty()) {
                if (Track::LapLineStep(G, lap_data_.lap_lines, r.lap_mask)) { ++r.laps; lapped = true; }
            } else if (((G + 1) % n) == 0) {      // wrap fallback (no LAPDATA lap lines)
                ++r.laps; lapped = true;
            }
            if (i == 0) {
                for (const auto& s : lap_data_.split_sectors)
                    if (G == s.second && s.first >= 0 && s.first < kMaxSplits &&
                        !split_done_[s.first]) {
                        split_time_[s.first] = race_time_;
                        split_done_[s.first] = true;
                    }
                if (lapped) {
                    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                        std::fprintf(lf, "F4 player lap %d @ t=%.2fs splits:",
                                     r.laps, race_time_);
                        for (int k = 0; k < kMaxSplits; ++k)
                            if (split_done_[k])
                                std::fprintf(lf, " s%d=%.2f", k, split_time_[k]);
                        std::fprintf(lf, "\n");
                        std::fclose(lf);
                    }
                    for (int k = 0; k < kMaxSplits; ++k) split_done_[k] = false;
                }
            }
            r.gate = (G + 1) % n;
        }
        const float* gc = gates_[static_cast<std::size_t>(r.gate)].center;
        const float dx = gc[0] - pos[0], dz = gc[2] - pos[2];
        const float d = std::sqrt(dx * dx + dz * dz);
        const float frac = (d > 8.f) ? 0.f : (1.f - d / 8.f);
        r.progress = static_cast<float>(r.laps * n + r.gate) + frac;

        // [D-11072 leg A] MONOTONE arc-length analogue. The car traverses segment
        // [gate arcseg -> gate arcseg+1]; the along-track projection parameter t of
        // its XZ onto that segment is the sub-gate fraction. t grows with forward
        // motion and ignores lateral weave, so it does not run backward the way the
        // radial `frac` above does. arcseg/arclaps are advanced forward-only the
        // instant the car crosses a gate perpendicular (t >= 1), independent of
        // r.gate -- which lags because it tracks nearest-gate-CENTER, making the
        // first formulation pin arcprog at 1 and then jump ~0.5 at each gate
        // (RESULT measured flat_frac 0.47). Uses only gate centers, no spline load.
        // INERT: writes arcprog/arcseg/arclaps, read only by the stepdump / the
        // future bridge; does NOT feed finish order (that stays on `progress`).
        {
            auto projT = [&](int a) -> float {   // clamped [0,1] projection on seg [a,a+1]
                const float* ga = gates_[static_cast<std::size_t>(a)].center;
                const float* gb = gates_[static_cast<std::size_t>((a + 1) % n)].center;
                const float sx = gb[0] - ga[0], sz = gb[2] - ga[2];
                const float l2 = sx * sx + sz * sz;
                if (l2 <= 1e-6f) return 1.f;     // degenerate seg: treat as crossed
                float t = ((pos[0] - ga[0]) * sx + (pos[2] - ga[2]) * sz) / l2;
                return (t < 0.f) ? 0.f : (t > 1.f ? 1.f : t);
            };
            float t = projT(r.arcseg);
            int adv = 0;
            while (t >= 1.f && adv++ < W + 2) {  // car past gate arcseg+1: advance
                const int nb = (r.arcseg + 1) % n;
                if (nb == 0) ++r.arclaps;        // crossed start/finish
                r.arcseg = nb;
                t = projT(r.arcseg);
            }
            // Weight by PHYSICAL segment length so a gate crossing does not jump:
            // position-in-lap (world units) = cumlen[arcseg] + t * seglen[arcseg];
            // arcprog adds arclaps*total_len_ so it is cumulative and monotone.
            const float inLap = gate_cumlen_[static_cast<std::size_t>(r.arcseg)]
                              + t * gate_seglen_[static_cast<std::size_t>(r.arcseg)];
            r.arcprog = static_cast<float>(r.arclaps) * total_len_ + inLap;
        }
    };
    step(0, car_pos_);
    for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
        step(i + 1, ai_cars_[static_cast<std::size_t>(i)].pos);

    // [U-9186 leg H3 2026-10-08] FUN_00442a60: write the reference-distance array
    // 0x008989b0[v], which the AI fire gates read via FUN_00442cc0 and which has
    // held 0 standalone until now. Default-OFF behind MASHED_REFDIST. Placed here
    // in UpdateRace (:3423), which runs before Ai_Standalone_Tick (:3462), so a
    // consumer sees this frame's value — the same ordering the race_pct bridge
    // below relies on. PREREG: verify/d3_u9186_20261008/PREREG_H3.md
    // [U-9683] The original runs the producer AFTER the AI tick, physics and progress of the
    // frame (FUN_0040fc00: FUN_00425a40 0x0040fc6c, FUN_0040da50 0x0040fc75, FUN_0040d470
    // 0x0040fc7c), so its consumer reads the previous frame's value. DEFAULT-ON 2026-10-16: the
    // call is at the end of UpdateCar (after the opponent block); opt-out
    // MASHED_NO_REFDIST_AFTER_AI restores this site. verify/d3_u9683_20261016.
    if (!RaceRefDistAfterAi()) RaceComputeDistancesTick(0);

    // [D-11072 P2] FUN_0040da50's FUN_00408610 loop (per-car path progress 0x008a96e8 and
    // race_pct 0x008a96ec), before the AI tick like the bridge below. Default-OFF MASHED_PATHPROG.
    Race::PathProg_FrameTick();

    // [mode 7 M1] FUN_0040fc00's FUN_00485070 (0x0040fd07): pair pass + publish. No registrant
    // exists yet, so with MASHED_REGISTRY it publishes cc = 0 every frame. Position relative to the
    // AI tick is an M4 question (PREREG_MODE7.md section 3).
    // [mode 7 M2a] FUN_0045bba0's FUN_00459000 (0x0040fcd0) registers the live pickups first.
    // [mode 7 M2b] the cars register first, from FUN_0040da50 (0x0040dad0), as in FUN_0040fc00's order.
    Race::CarsReg_FrameTick();
    Gameplay::Pickups_FrameTick();
    Race::Registry_FrameTick();

    // [D-11072 bridge write] Mirror the monotone arcpct into the original per-car
    // race_pct slot 0x008a96ec + v*0x30c, behind a default-OFF knob. UpdateRace runs
    // before Ai_Standalone_Tick (:3462), so AiPreTickRubberBand's live read at
    // AiStandalone.cpp:1468 sees this value the same frame. Writes all 4 cars (the
    // original writes race_pct for every participant; AiPreTickRubberBand loops v=0..3).
    // NOT a C-level change: a bridged write is not a behavioural diff.
    // PREREG: verify/d3_racepos_20261006/PREREG_BRIDGE.md section 3a.
    {
        static const bool s_racepctBridge =
            (std::getenv("MASHED_NO_RACEPCT_BRIDGE") == nullptr);   // DEFAULT-ON 2026-10-09 (verify/d3_b2flip_20261009); MASHED_RACEPCT_BRIDGE is now a no-op
        // With MASHED_PATHPROG the faithful producer FUN_00408610 owns 0x008a96ec; the bridge stands down.
        // [U-9685] arm R (MASHED_RACEPCT_FUN408610) publishes the verbatim step's race_pct at the end of UpdateCar.
        if (s_racepctBridge && !Race::PathProg_Enabled() && !Race::RacePctOrig_Active() && total_len_ > 1e-3f) {
            for (int v = 0; v < kRaceCars; ++v) {
                const float ap =
                    std::fmod(race_[v].arcprog, total_len_) / total_len_ * 100.f;
                *reinterpret_cast<float*>(
                    0x008a96ecu + static_cast<std::uintptr_t>(v) * 0x30cu) = ap;
            }
        }
    }

    // [G4] lap-progress diagnostic (env MASHED_LAP_DIAG -> mashed_re.log, ~1/s).
    {
        static const bool s_lapdiag = (std::getenv("MASHED_LAP_DIAG") != nullptr);
        static int s_lf = 0;
        if (s_lapdiag && (++s_lf % 60) == 0) {
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "LAP-DIAG round=%d mode=%d t=%.1f gates=%d laptgt=%d | "
                    "p0 gate=%d lap=%d prog=%.1f | a1 gate=%d lap=%d | a2 gate=%d lap=%d | a3 gate=%d lap=%d\n",
                    round_mode_ ? 1 : 0, race_mode_, race_time_, n, lap_target_,
                    race_[0].gate, race_[0].laps, race_[0].progress,
                    race_[1].gate, race_[1].laps, race_[2].gate, race_[2].laps,
                    race_[3].gate, race_[3].laps);
                std::fclose(lf);
            }
        }
    }

    if (!round_mode_) return;   // race rules only run during a match (both modes
                                // update the shared camera below).

    // [D3 2026-10-03] MEASUREMENT CONTROL, default OFF, no effect on the default
    // build. MASHED_NO_ELIM=1 makes the two elimination blocks below (:4661 and
    // :4716) a no-op and changes nothing else, so that an A/B can attribute the
    // scored (b)/(e) window to a cause other than "the port eliminated a car and
    // the reference arm did not". Registered in
    // verify/d3_elim_20261003/PREREG_STEP1.md STEP 1C before it was run.
    // NOT a fix and NOT a scoring knob: it is never set in a scored default arm.

    // [D-11056] rule-5 collectible feed: a KTC_NewCopter gameplay copter
    // "collects" by completing one traversal of its bound flight path —
    // mirrors FUN_004064c0's per-object completion tick (unconditional on
    // rule, like the original; only rule 5's predicate reads collectDone).
    for (auto& c : copters_) {
        if (!c.gameplay || c.done) continue;
        if (c.anim.duration > 0.f && race_time_ >= c.anim.duration) {
            c.done = true;
            OnCollect();
        }
    }

    // ---- VERBATIM-PORTED camera + elimination (replaces the ">7 gates"
    // scaffold). Adapters: standalone race state -> RaceCamCar quantities
    // the original reads through getters (see Race/RaceCamera.h).
    Race::RaceCamCar cc[kRaceCars] = {};
    FillRaceCamCars(cc);
    cam_ticks_ += static_cast<double>(dt) * 3000000.0;   // DAT_007f1030 rate
    RaceCamUpdate(cc, false);   // race_cam_.Update; DAT_007f100c 1/60, DAT_007f0fc8 0 live

    for (int i = 0; i < kRaceCars; ++i)        // DAT_008a9510 decay
        if (delta_timer_[i] > 0.f) delta_timer_[i] -= dt * 1000.f;

    // ---- [D-11052] per-rule win-condition engine (Race/RuleEngine): the
    // FUN_004177b0 metric/finish-order update, the FUN_00410d10 segment check
    // (per-rule pre-blocks + elimination gating + last-man logic) and — on
    // segment end — the FUN_00410510 result evaluation, following the caller
    // contract of FUN_00411170 (result != 0 -> match over; == 0 -> next
    // round). MASHED_RULE_ENGINE=0 reverts to the legacy collapse below.
    if (rule_engine_on_ && rule_engine_race_on_) {   // [D-11056] per-race gate
        if (match_winner_ >= 0) return;        // result declared (FUN_00443080)
        namespace RE = Race::RuleEngine;
        RE::Cars rc;
        rc.participants = ParticipantCount();  // DAT_008a94d0 (0x0040ff40 count)
        // FUN_0042f500() (= DAT_0067ea64) is read live by FUN_00410510's score
        // loop; the standalone's copy of that flag is team_play_.
        rulep_.teams = team_play_;
        for (int i = 0; i < kRaceCars; ++i) {
            rc.active[i] = cc[i].active;            // == (P2[i] != nullptr), set by FillRaceCamCars
            rc.alive[i]  = race_[i].alive && rc.active[i];
            rc.score[i]  = scores_[i];
            // DAT_0089a880[i] = lapCounter + lapFrac% * 0.01 (FUN_004177b0).
            // [D-11072 leg E2] MASHED_RACEMETRIC_ARC=1 feeds the engine the
            // MONOTONE arc-length fraction instead of the radial one. The radial
            // `progress` runs backward mid-lap (484/681/1287 reversals measured vs
            // the original's 0); `arcprog` is forward-only by construction
            // (:5000-5011). DEFAULT-OFF: with the env unset the two statements
            // below are byte-for-byte the previous behaviour.
            // The formula is PREREG_CONSUMER.md section 4 verbatim, including its
            // use of race_[].laps rather than race_[].arclaps — AMEND_E2.md A4
            // registers E2-LAPAGREE to measure that mismatch rather than silently
            // "fixing" a pre-registered expression.
            static const bool s_arcMetric =
                (std::getenv("MASHED_RACEMETRIC_ARC") != nullptr);
            if (s_arcMetric && total_len_ > 1e-3f) {
                const float arcpct =
                    std::fmod(race_[i].arcprog, total_len_) / total_len_ * 100.f;
                rc.metric[i] = static_cast<float>(race_[i].laps) + arcpct * 0.01f;
            } else {
                const float pct = std::fmod(race_[i].progress, static_cast<float>(n))
                                  / static_cast<float>(n);
                rc.metric[i] = static_cast<float>(race_[i].laps) + pct;
            }
            // AMEND_E2.md A2: hand rc.metric to the stepdump. UpdateCar calls
            // UpdateRace at :3423 and AiStepDump at :3463, so the dump reads THIS
            // frame's value, not a stale one. Written for every i each tick.
            rule_metric_[i] = rc.metric[i];
        }
        ++rule_metric_tick_;   // [AMEND_E2.md A7] in-scope witness for E2-WROTE
        RE::UpdateFinishOrder(rule_, rc, rulep_);

        // Rule-10 countdown (FUN_004039f0): -= dt; checkpoint award once per
        // lap-line gate per lap (flag reset when the lap counter advances).
        if (rule_ == 10 && countdown_ <= 0.f) {
            RE::Rule10Tick(rulep_, dt);
            if (race_[0].laps != rule10_lap_seen_) {
                rule10_lap_seen_ = race_[0].laps;
                rule10_hit_mask_ = 0;
            }
            if (rule10_bonus_ > 0.f) {
                int li = 0;
                for (int g : lap_data_.lap_lines) {
                    const std::uint32_t bit = 1u << li++;
                    // width-1.0 window (0x004039f0.md step 8: *piVar3 <=
                    // fVar5 < *piVar3 + 1.0). Fast multi-gate advance
                    // (>1 gate/tick) can skip it — exactly as the original's
                    // sampled window can; do NOT add crossing-tracking.
                    if ((rule10_hit_mask_ & bit) == 0 && race_[0].gate > g &&
                        race_[0].gate <= g + 1) {
                        rule10_hit_mask_ |= bit;
                        rulep_.timer += rule10_bonus_;
                    }
                    if (li >= 6) break;             // 6-entry table (0x004046a0)
                }
            }
        }

        if (round_winner_ >= 0) return;        // round-end hold (results delay)
        bool runElim = false;
        const int seg = RE::SegmentCheck(rule_, rc, rulep_,
                                         /*resultDeclared=*/false, &runElim);
        if (runElim && countdown_ <= 0.f && !NoElim()) {
            const int victim = race_cam_.EliminationCheck(cc);
            if (victim >= 0 && race_[victim].alive) {
                race_[victim].alive = false;
                --round_alive_;
                if (victim > 0 && victim - 1 < static_cast<int>(ai_cars_.size()))
                    ai_cars_[static_cast<std::size_t>(victim - 1)].speed = 0.f;
                ScoreOnElimination(victim);    // FUN_0040eee0(victim, 1)
            }
        }
        // re-snapshot alive flags AND scores after any elimination this tick:
        // the original's elimination scoring (FUN_0040eee0) runs INSIDE the
        // segment check and FUN_00410510 then reads the live DAT_008a94e0
        // scores in the same tick (0x00410d10.md l.56; FUN_00410510.md
        // Phase 1) — EvaluateResult must not see the pre-elimination snapshot.
        for (int i = 0; i < kRaceCars; ++i) {
            rc.alive[i] = race_[i].alive && rc.active[i];
            rc.score[i] = scores_[i];          // ScoreOnElimination -> ScoreAward
        }
        if (seg == 1 ||
            RE::SegmentCheck(rule_, rc, rulep_, false, &runElim) == 1) {
            bool p0 = false;
            const int r = RE::EvaluateResult(rule_, rc, rulep_, &p0);
            // once per segment end (round), not per frame
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "RULE-EVAL rule=%d round=%d participants=%d "
                                 "r=%d timer=%.2f scores=%d,%d,%d,%d\n", rule_, round_no_,
                             rc.participants, r, rulep_.timer, scores_[0], scores_[1],
                             scores_[2], scores_[3]);
                std::fclose(lf);
            }
            if (r > 0) {                       // match over, winner r-1
                match_winner_ = r - 1;
            } else if (r == -1) {              // over with no winner (draw /
                match_winner_ = 0;             // player failed); results text
                match_draw_ = true;            // shows RACE OVER via match_draw
            } else {                           // r == 0: next round
                for (int i = 0; i < kRaceCars; ++i)
                    if (race_[i].alive) { round_winner_ = i; break; }
                if (round_winner_ < 0) round_winner_ = 0;  // all dead: equalized
                // [round grid Stage A] FUN_00410860 0x004108c5: DAT_0063ba80 = __ftol(progress[last alive])
                // (default-OFF MASHED_ROUND_GRID + MASHED_PATHPROG; FIX_BRIEF.md section 4 A1)
                {
                    bool al[4] = { false, false, false, false };
                    for (int i = 0; i < kRaceCars && i < 4; ++i) al[i] = race_[i].alive;
                    int cd[4] = { 0, 0, 0, 0 };
                    for (int i = 0; i < kRaceCars && i < 4; ++i) cd[i] = score_delta_[i];   // DAT_008a9520
                    Race::RoundGrid_RoundEnd(al, round_winner_, cd);
                }
            }
        }
        return;
    }

    // ---- legacy two-objective collapse (MASHED_RULE_ENGINE=0) -------------
    // Laps mode: no elimination — first car to complete lap_target_ laps wins.
    if (race_mode_ == 1) {
        if (match_winner_ < 0 && countdown_ <= 0.f)
            for (int i = 0; i < kRaceCars; ++i)
                if (race_[i].laps >= lap_target_) { match_winner_ = i; break; }
        return;
    }
    if (round_winner_ >= 0) return;            // elimination round-end hold

    if (countdown_ <= 0.f && !NoElim()) {
        const int victim = race_cam_.EliminationCheck(cc);
        if (victim >= 0 && race_[victim].alive) {
            race_[victim].alive = false;
            --round_alive_;
            if (victim > 0 && victim - 1 < static_cast<int>(ai_cars_.size()))
                ai_cars_[static_cast<std::size_t>(victim - 1)].speed = 0.f;
            ScoreOnElimination(victim);        // FUN_0040eee0(victim, 1)
        }
    }
    if (round_alive_ <= 1) {
        for (int i = 0; i < kRaceCars; ++i)
            if (race_[i].alive) { round_winner_ = i; break; }
    }
}

void TrackRenderer::camera(float eye[3], float at[3]) const {
    for (int i = 0; i < 3; ++i) { eye[i] = last_eye_[i]; at[i] = last_at_[i]; }
}

namespace {
// ===========================================================================
// WS-A s3 PERF: persistent vertex-buffer cache for STATIC geometry batches.
//
// DrawPrimitiveUP is the deprecated user-pointer path: every call re-uploads the
// vertices into a driver scratch buffer and can stall the pipeline. The batches
// drawn here (sky) are STATIC (built at Load, never mutated during
// Render; only their WORLD matrix changes), so each is uploaded once into a
// D3DPOOL_MANAGED vertex buffer and drawn with DrawPrimitive: same vertices, same
// order, same textures, identical pixels, no per-frame re-upload.
//
// Keyed by the batch vector's data() pointer (stable per track session).
// Invalidated on track (re)load so a recycled address can't draw stale geometry.
// Only the STATIC passes use this; the dynamic billboard passes (particles/pickups,
// rebuilt every frame) keep DrawPrimitiveUP.
// ===========================================================================
constexpr DWORD kBatchFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;  // == TrackRenderer::kFVF
struct CachedVB { IDirect3DVertexBuffer9* vb = nullptr; UINT verts = 0; };
std::unordered_map<const void*, CachedVB> g_vbCache;

void InvalidateBatchCache() {
    for (auto& kv : g_vbCache) if (kv.second.vb) kv.second.vb->Release();
    g_vbCache.clear();
}

// Draw one static batch via its cached VB (created on first use). Falls back to
// DrawPrimitiveUP if VB creation/lock fails. The caller sets FVF / texture /
// world transform exactly as before — only the vertex SOURCE changes.
void DrawBatch(IDirect3DDevice9* dev, const std::vector<TrackRenderer::V>& b) {
    if (b.empty()) return;
    const UINT nv = (UINT)b.size();
    auto it = g_vbCache.find(b.data());
    if (it == g_vbCache.end() || it->second.verts != nv) {
        if (it != g_vbCache.end()) {
            if (it->second.vb) it->second.vb->Release();
            g_vbCache.erase(it);
        }
        const UINT bytes = nv * (UINT)sizeof(TrackRenderer::V);
        IDirect3DVertexBuffer9* vb = nullptr;
        if (SUCCEEDED(dev->CreateVertexBuffer(bytes, D3DUSAGE_WRITEONLY, kBatchFVF,
                                              D3DPOOL_MANAGED, &vb, nullptr)) && vb) {
            void* dst = nullptr;
            if (SUCCEEDED(vb->Lock(0, bytes, &dst, 0)) && dst) {
                std::memcpy(dst, b.data(), bytes);
                vb->Unlock();
                it = g_vbCache.emplace(b.data(), CachedVB{ vb, nv }).first;
            } else {
                if (vb) vb->Release();
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nv / 3, b.data(),
                                     sizeof(TrackRenderer::V));
                return;
            }
        } else {
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nv / 3, b.data(),
                                 sizeof(TrackRenderer::V));
            return;
        }
    }
    dev->SetStreamSource(0, it->second.vb, 0, (UINT)sizeof(TrackRenderer::V));
    dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nv / 3);
}
}  // namespace

// Collision/slide FX. [SCAFFOLD] presentation, real data path: reuses the existing
// ParticleSystem billboard pool and scales every threshold to track radius.
//
// CALIBRATION CAVEAT (load-bearing, do not lose this):
// the skid trigger is driven off YAW-RATE x speed, not off lateral slip, because the
// kinematic/AI cars keep velocity aligned with heading and encode motion as
// (speed, yaw) with vel[] zero. The first pass used slip and fired 0 skids per race;
// after the change it fired 251. That calibration is therefore tuned to the CURRENT
// DEFAULT drive model. ROADMAP v3 D2 inverts MASHED_REAL_PHYSICS and makes the ported
// chain the default, at which point vel[] becomes real and the slip term starts
// carrying signal — these thresholds must be re-measured then, not assumed to carry
// over. The `slip` branch below is retained precisely so it takes over when that lands.
void TrackRenderer::EmitCarFx(int slot, const float pos[3], const float vel_in[3],
                              float speed_in, float yaw, float dt) {
    if (slot < 0 || slot > 3 || dt <= 0.f) return;
    // UNITS (2026-10-16, ROADMAP follow-up #2 re-measure, verify/fe_residuals_20261016/fx/): under the default
    // ported physics, `vel_in`/`speed_in` are the chain's INTERNAL units (player 0..3243, AI cur_speed ~3200 on
    // Training, R 27.5) while R and `pos` are world units; position advances by io.drive_delta, which carries
    // the internal->world factor (_DAT_005cc948 * _DAT_005cea80; measured |dpos|/dt / sp = 0.00257 over 3449
    // moving frames). Every R-scaled threshold below therefore fired on nearly every moving frame and the
    // 1200-slot pool peaked at 1200. The FX now reads the velocity from the position step itself, which is in
    // world units for every car and needs no constant. MASHED_FX_RAWSPEED=1 keeps the old inputs for A/B.
    static const bool s_raw = (std::getenv("MASHED_FX_RAWSPEED") != nullptr);
    float wvel[3] = { vel_in[0], vel_in[1], vel_in[2] };
    float speed = speed_in;
    if (!s_raw) {
        const float Rg = track_radius_ > 1.f ? track_radius_ : radius_;
        bool ok = fx_have_prev_[slot];
        float d[3] = { pos[0] - fx_prev_pos_[slot][0], 0.f, pos[2] - fx_prev_pos_[slot][2] };
        // a teleport (respawn, reset) is not motion: skip the step and re-seed
        if (ok && (d[0] * d[0] + d[2] * d[2]) > (Rg * 0.5f) * (Rg * 0.5f)) ok = false;
        fx_prev_pos_[slot][0] = pos[0]; fx_prev_pos_[slot][1] = pos[1]; fx_prev_pos_[slot][2] = pos[2];
        fx_have_prev_[slot] = true;
        wvel[0] = ok ? d[0] / dt : 0.f; wvel[1] = 0.f; wvel[2] = ok ? d[2] / dt : 0.f;
        speed = wvel[0] * std::cos(yaw) + wvel[2] * std::sin(yaw);   // signed forward component, as car_speed_ was
    }
    const float* vel = wvel;
    const float R  = track_radius_ > 1.f ? track_radius_ : radius_;
    const float fx = std::cos(yaw), fz = std::sin(yaw);
    const float vf = vel[0] * fx + vel[2] * fz;             // forward component
    const float lx = vel[0] - fx * vf, lz = vel[2] - fz * vf;
    const float slip = std::sqrt(lx * lx + lz * lz);        // lateral slide speed

    // skid smoke: lateral slip OR hard cornering at speed.
    float dyaw = yaw - fx_prev_yaw_[slot];
    while (dyaw >  3.14159265f) dyaw -= 6.28318531f;
    while (dyaw < -3.14159265f) dyaw += 6.28318531f;
    const float spd = (speed < 0.f) ? -speed : speed;
    const float yawRate = std::fabs(dyaw) / dt;                  // rad/s
    const float corner = yawRate * spd;                          // cornering intensity
    fx_prev_yaw_[slot] = yaw;
    const float kSlip = R * 0.02f, kCorner = R * 0.12f;
    float skidI = slip / kSlip;
    if (corner / kCorner > skidI) skidI = corner / kCorner;      // whichever is stronger
    if (skidI > 1.f && spd > R * 0.04f) {
        fx_skid_accum_[slot] += (skidI > 3.f ? 3.f : skidI) * 36.f * dt;
        int n = static_cast<int>(fx_skid_accum_[slot]);
        if (n > 4) n = 4;
        fx_skid_accum_[slot] -= static_cast<float>(n);
        fx_skids_[slot] += n;
        for (int i = 0; i < n; ++i) {
            const float w  = (i & 1) ? 1.f : -1.f;          // two wheel tracks
            const float wx = pos[0] - fx * (R * 0.012f) + (-fz) * w * (R * 0.006f);
            const float wz = pos[2] - fz * (R * 0.012f) + ( fx) * w * (R * 0.006f);
            const float p[3] = { wx, pos[1] + R * 0.002f, wz };
            float aF = 0.28f + 0.30f * (skidI > 2.f ? 1.f : skidI * 0.5f);
            std::uint8_t a = static_cast<std::uint8_t>(aF * 255.f);
            std::uint32_t col = (static_cast<std::uint32_t>(a) << 24) | 0x00303030u; // dark smoke
            parts_.SpawnTrail(p, col, R * 0.010f, 0.55f);
        }
    } else {
        fx_skid_accum_[slot] = 0.f;
    }

    // impact sparks: a sudden one-frame deceleration is a collision
    const float decel = fx_prev_speed_[slot] - speed;
    const float kImpact = R * 0.06f;
    if (decel > kImpact && fx_prev_speed_[slot] > R * 0.05f) {
        const float k = decel / kImpact;                    // >= 1
        const int n = static_cast<int>(8.f + 8.f * (k > 2.f ? 2.f : k));
        const float p[3] = { pos[0], pos[1] + R * 0.012f, pos[2] };
        parts_.SpawnBurst(p, n, 0xE0FFC850u /*warm spark*/, R * 0.07f, R * 0.006f, 0.35f);
        ++fx_sparks_[slot];
    }
    // [diag] MASHED_FX_DEBUG: one line PER SLOT per sample window, each carrying that
    // slot's OWN inputs and its OWN counters. The previous form logged only slot 1 but
    // printed a counter summed over all four cars, so a rising number could not be
    // attributed and a stationary car appeared to emit thousands of skids.
    static int s_fxdbg = -1;
    if (s_fxdbg < 0) { char b[8]; s_fxdbg = GetEnvironmentVariableA("MASHED_FX_DEBUG", b, sizeof b) ? 1 : 0; }
    if (s_fxdbg) {
        static int dn[4] = {};
        if ((dn[slot]++ % 12) == 0) {
            if (std::FILE* f = std::fopen("log/fx_debug.txt", "a")) {
                // alive/pool: the question "is the FX rate starving the shared pool?"
                // is answerable only from occupancy, not from cumulative emit counts.
                std::fprintf(f, "[s%d] R=%.1f spd=%.2f yawRate=%.2f corner=%.2f kCorner=%.2f decel=%.2f skidI=%.2f skids=%d sparks=%d alive=%d\n",
                             slot, R, spd, yawRate, corner, kCorner, decel, skidI,
                             fx_skids_[slot], fx_sparks_[slot], parts_.alive());
                std::fclose(f);
            }
        }
    }
    fx_prev_speed_[slot] = speed;
}

// [rprops U-9695] The Arctic per-frame tick 0x00448ef0 for the props this port draws: 0x00448cb0 poses
// the freighters from the accumulators, then advances all three (0x00448d78..0x00448db1: bob += 0.02,
// roll += 0.04, pitch += 0.01, each a float add stored to the float global), then 0x00448e20 poses the
// boats from the ADVANCED values. 0x00448dc0 (camera-men) rewrites constant table rows here, so the
// camera-men are placed once at load. Called once per Render, which exe_main calls once per frame.
void TrackRenderer::UpdatePropAnims() {
    // The first two Render calls do not tick: measured against the recorded freighter y at rows 1044,
    // 1051 and 1091 (y = sin(acc258)*0.5 - 4.5 gives acc258/0.02 = 1043.0, 1050.0, 1090.0 = K-1), the
    // original's tick count at the frame that consumed row K is K, while this loop with no delay
    // measured K+2. PREREG_rprops.md amendment 1; the gate that holds slot 1 off for the first two
    // course updates in the original is not decoded (0x00426ac3..0x00426ae3 gates on 0x0041e9e0).
    if (++prop_tick_frames_ <= 2) return;
    for (const auto& a : prop_anims_) {
        if (a.kind != kAnimFreighter || a.prop >= props_.size()) continue;
        auto& inst = props_[a.prop].instances;
        inst.resize(2);
        for (int k = 0; k < 2; ++k)
            ShipMatrix(kFreighterPos[k], kFreighterYaw[k], prop_acc_, kFreighterAmp, kFreighterOff, &inst[k]);
    }
    prop_acc_[0] = prop_acc_[0] + kStepBob;      // 0x00683258
    prop_acc_[2] = prop_acc_[2] + kStepRoll;     // 0x00683260
    prop_acc_[1] = prop_acc_[1] + kStepPitch;    // 0x0068325c
    for (const auto& a : prop_anims_) {
        if (a.kind != kAnimBoat || a.prop >= props_.size()) continue;
        auto& inst = props_[a.prop].instances;
        inst.resize(4);
        for (int k = 0; k < 4; ++k)
            ShipMatrix(kBoatPos[k], kBoatYaw[k], prop_acc_, kBoatAmp, kBoatOff, &inst[k]);
    }
}

// [rstates F6] The excluded clumps are not in the RpWorld (0x00479da6), so the original draws them
// from the per-track hook record's +0x1c slot (table 0x005f33f8, stride 0x48, Course_Id at +0x10),
// one RpClumpRender (0x004e6680) per course clump slot [course + 0x10110 + 4*i], in code order.
// Read from the bytes by verify/rstates_20261016/slot2_order.py; every list is a subset of the
// track's Clump_Exclude_From_World set (excluded_vs_slot2.py), the whole set on TRAINING and City.
// Dump (indexed loop at 0x00449583), Arctic and Egypt (no course clump slot) have no entry.
struct AfterWorldOrder { int course_id; std::uint32_t fn; int n; int idx[8]; };
const AfterWorldOrder kAfterWorldOrder[] = {
    { 30, 0x00451460u, 7, { 9, 3, 4, 5, 6, 7, 8 } },          // training
    { 25, 0x00450fc0u, 1, { 0 } },                            // superg
    {  2, 0x0044d6f0u, 8, { 3, 0, 5, 11, 9, 8, 19, 17 } },    // rouabout
    { 11, 0x00450910u, 2, { 5, 1 } },                         // sands
    { 33, 0x00452d10u, 3, { 19, 20, 0 } },                    // warzone
    { 34, 0x0044df00u, 1, { 0 } },                            // forest
    { 36, 0x0044db40u, 2, { 0, 1 } },                         // storm
    { 38, 0x0044e470u, 1, { 43 } },                           // highway
    { 39, 0x0044af10u, 2, { 5, 6 } },                         // neustein
    { 26, 0x0044b8f0u, 8, { 6, 7, 8, 9, 10, 12, 11, 13 } },   // city (0x0044b920..0x0044b97b)
};

// Queue order for props_: the after-world props a track's slot 2 draws go as one group, in slot-2
// order, where the first of them sits in clump order; every other prop keeps its place (in-world
// props are drawn by the sector walk, so their queue order only seeds tie creation, unchanged).
// [UNCERTAIN U-9708] where the excluded clumps slot 2 does not draw are rendered relative to the
// group; they keep clump order. MASHED_LIBRW_AFTERWORLDORDER=0 keeps plain clump order.
void TrackRenderer::BuildPropOrder(std::FILE* log) {
    prop_order_.clear();
    const char* e = std::getenv("MASHED_LIBRW_AFTERWORLDORDER");
    const bool on = !(e && e[0] == '0' && e[1] == '\0');
    const AfterWorldOrder* aw = nullptr;
    if (on)
        for (const auto& a : kAfterWorldOrder)
            if (a.course_id == course_id_) aw = &a;
    auto rank = [&](std::size_t i) {
        const Prop& p = props_[i];
        if (!aw || p.rw_kind != LibRw::kInstAfterWorld || p.clump_idx < 0) return -1;
        for (int k = 0; k < aw->n; ++k)
            if (aw->idx[k] == p.clump_idx) return k;
        return -1;
    };
    bool grouped = false;
    for (std::size_t i = 0; i < props_.size(); ++i) {
        if (rank(i) < 0) { prop_order_.push_back(i); continue; }
        if (grouped) continue;
        grouped = true;
        for (int k = 0; k < aw->n; ++k)
            for (std::size_t j = 0; j < props_.size(); ++j)
                if (rank(j) == k) prop_order_.push_back(j);
    }
    if (log) {
        std::fprintf(log, "  [rstates F6] after-world order course_id=%d slot2=0x%08x:", course_id_,
                     aw ? aw->fn : 0u);
        for (const std::size_t i : prop_order_)
            if (props_[i].rw_kind == LibRw::kInstAfterWorld)
                std::fprintf(log, " %d", props_[i].clump_idx);
        std::fprintf(log, "\n");
    }
}

void TrackRenderer::Render(IDirect3DDevice9* dev, float t, const CamInput* in) {
    if (!ready_) return;
    // Parity 3D channel (MASHED_DBG_DRAWSTREAM3D) frame delimiter: Render() is
    // the standalone's single race-scene entry — exe_main's race branch calls
    // it exactly once per rendered frame, and it never runs for the menu.
    DrawStreamDump_Race3DBegin();
    // Per-category geometry tallies for the channel: summed beside every
    // DrawBatch submit below and flushed once per category via Race3DCat
    // (sky, the only category still drawn through D3D9 besides particles / pickups; world, props and
    // cars go to librw and have no tally). A few integer adds per frame;
    // Race3DBegin/Cat early-out on g_state3d when the channel is off.
    unsigned ds_b = 0, ds_v = 0, ds_t = 0;
    auto ds_tally = [&](const std::vector<V>& b, IDirect3DTexture9* tex) {
        ++ds_b; ds_v += static_cast<unsigned>(b.size()); if (tex) ++ds_t; };
    auto ds_flush = [&](const char* cat) {
        DrawStreamDump_Race3DCat(cat, ds_b, ds_v, ds_t);
        ds_b = ds_v = ds_t = 0; };
    // WS-A s3 render-section profiler (env MASHED_RENDER_PROF): attribute the
    // 3D frame's CPU cost to props/copters/cars/particles so the real
    // hot path is measured. QPC; logs render_prof.log. Off unless the env is set.
    static const bool s_rprof = (std::getenv("MASHED_RENDER_PROF") != nullptr);
    static LARGE_INTEGER s_rqf = []{ LARGE_INTEGER f;
        QueryPerformanceFrequency(&f); return f; }();
    auto RT  = []{ LARGE_INTEGER c; QueryPerformanceCounter(&c); return c.QuadPart; };
    auto RMs = [](long long a, long long b){
        return (double)(b - a) * 1000.0 / (double)s_rqf.QuadPart; };
    double d_props = 0, d_car = 0, d_part = 0, d_other = 0;
    long long _rt0 = s_rprof ? RT() : 0;
    long long _mk  = _rt0;
    auto MARK = [&](double& acc){ if (s_rprof) { long long n = RT();
        acc += RMs(_mk, n); _mk = n; } };
    // F3 A/B verification toggle: suppress all UV-scroll texture transforms.
    static const bool s_uvscroll_off = std::getenv("MASHED_NO_UVSCROLL") != nullptr;
    // P3 same-view parity: MASHED_CAM_POSE=ex,ey,ez,ax,ay,az pins the camera to a
    // fixed world-space eye/at (overrides orbit/chase/free), so the standalone can
    // render a track from the ORIGINAL's exact captured pose and an in-race diff is
    // finally same-view. The producer is re/frida/race_draw_burst.py, which reads the
    // original's race-camera struct DAT_00897fe0 (+0x40 eye, +0x4c at) and prints the
    // value in this exact format. NOTE the pose is read from that RW struct via Frida,
    // NOT from any D3D transform: 3755c57e established that RenderWare pre-transforms
    // world geometry on the CPU, so D3DTS_VIEW and the first world matrix are BOTH
    // identity and the shim cannot recover the camera from fixed-function state.
    //
    // TWO FORMS, and the 12-float one is the faithful one:
    //   6  floats: ex,ey,ez,ax,ay,az            -- eye + at-point (legacy)
    //   12 floats: px,py,pz,rx,ry,rz,ux,uy,uz,ax,ay,az
    //              -- the camera's full orthonormal BASIS: position, right, up,
    //                 at. This exists because the 6-float form assumes up is
    //                 world +Y and therefore cannot express roll, and the
    //                 original's race camera measurably HAS roll (RwCamera frame
    //                 right.y = 0.440, ~26 deg, verify/d1_carproj/RESULT.md).
    //
    // Source, and this is the part that was wrong for months: feed it from the
    // RwCamera's FRAME (*(RwCamera+4), LTM +0x50 -> right +0x00 / up +0x10 /
    // at +0x20 / pos +0x30), NOT from the controller struct DAT_00897fe0
    // +0x40/+0x4c. Those controller fields are not the world camera: projecting
    // the original's own car world positions through them puts the cars at
    // z 4.8-7.7 spread across mid-frame, while the capture shows them clustered
    // at z ~18.5-21.8 -- which is what the frame basis predicts.
    static float s_campose[12];
    static int  s_campose_n = 0;
    static const bool s_have_campose = [] {
        char b[256];
        if (GetEnvironmentVariableA("MASHED_CAM_POSE", b, sizeof(b)) == 0) return false;
        s_campose_n = std::sscanf(b,
            "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
            &s_campose[0], &s_campose[1], &s_campose[2], &s_campose[3],
            &s_campose[4], &s_campose[5], &s_campose[6], &s_campose[7],
            &s_campose[8], &s_campose[9], &s_campose[10], &s_campose[11]);
        return s_campose_n == 6 || s_campose_n == 12;
    }();
    // RACE-CAM RENDER [d1r2 2026-10-16, D1-residue R2]. The in-race view is the
    // original's frame, built the way the director's tail builds it
    // (Race/CameraApply.cpp): Camera::Apply 0x00441760 from the angles the port
    // writes at 0x00447fc2..0x004481b6 (yaw +0x38, pitch +0x34, roll +0x3c = 0,
    // eye +0x40), or entry 0's matrix when the phase-3 hold flag is set
    // (0x00442a20, MASHED_SUBSTATE only). The 12 floats are PUBLISHED, so librw
    // and the legacy path draw from one basis. race_cam_ runs before Render every
    // in-race frame, the default countdown included (UpdateCar). Evidence:
    // verify/d1r2_20261016/. Supersedes child-C's look-at pair (2026-08-30),
    // whose "roll gap" was measured in phase 3, where entry 0 renders: racing
    // frames carry roll 0 (0x004481a3).
    //
    // MASHED_NO_RACE_CAM_BASIS=1: the pre-d1r2 resolution, byte for byte (Y-up
    // look-at on pos/target, the WS-E chase-rig geometry on frames the director
    // has not run, no countdown update). A/B only; goes with D1-residue R3.
    static const bool s_basis_optout =
        GetEnvironmentVariableA("MASHED_NO_RACE_CAM_BASIS", nullptr, 0) != 0;
    {
        static bool s_said = false;
        if (s_basis_optout && !s_said) {
            s_said = true;
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "[d1r2] MASHED_NO_RACE_CAM_BASIS: pre-d1r2 look-at camera\n");
                std::fclose(lf);
            }
        }
    }
    bool race_basis = false;
    Race::RaceRenderCamera rrc{};
    // Camera: auto-orbit by default; any movement input switches to free
    // mode (WASD/QE move relative to look direction, mouse/arrow look).
    float eye[3];
    float at[3];
    if (in) {
        const bool wants_free =
            in->move_fwd != 0.f || in->move_strafe != 0.f ||
            in->move_up != 0.f || in->yaw_delta != 0.f || in->pitch_delta != 0.f;
        if (in->reset_orbit) free_ = false;
        if (wants_free && !free_) {
            // seed the free camera from the current orbit pose
            const float yaw0 = t * 0.3f;
            eye_[0] = center_[0] + radius_ * 1.15f * std::cos(yaw0);
            eye_[1] = center_[1] + radius_ * 0.55f;
            eye_[2] = center_[2] + radius_ * 1.15f * std::sin(yaw0);
            yaw_   = std::atan2(center_[2] - eye_[2], center_[0] - eye_[0]);
            pitch_ = -0.4f;
            free_  = true;
        }
        if (free_) {
            yaw_   += in->yaw_delta;
            pitch_ += in->pitch_delta;
            if (pitch_ >  1.5f) pitch_ =  1.5f;
            if (pitch_ < -1.5f) pitch_ = -1.5f;
            const float cp = std::cos(pitch_), sp = std::sin(pitch_);
            const float fwd[3] = {std::cos(yaw_) * cp, sp, std::sin(yaw_) * cp};
            const float right[3] = {-std::sin(yaw_), 0.f, std::cos(yaw_)};
            const float speed = radius_ * 0.5f * in->dt;   // half-world/s
            for (int i = 0; i < 3; ++i)
                eye_[i] += fwd[i] * in->move_fwd * speed +
                           right[i] * in->move_strafe * speed;
            eye_[1] += in->move_up * speed;
        }
    }
    if (free_) {
        const float cp = std::cos(pitch_), sp = std::sin(pitch_);
        eye[0] = eye_[0]; eye[1] = eye_[1]; eye[2] = eye_[2];
        at[0] = eye_[0] + std::cos(yaw_) * cp;
        at[1] = eye_[1] + sp;
        at[2] = eye_[2] + std::sin(yaw_) * cp;
    } else if (car_ready_ && !s_basis_optout && race_cam_.primed()) {
        // The director tail 0x004481c1..0x00448208 (Race/CameraApply.cpp).
        Race::ResolveRaceRenderCamera(race_cam_, &rrc);
        for (int i = 0; i < 3; ++i) { eye[i] = rrc.pos[i]; at[i] = rrc.pos[i] + rrc.at[i]; }
        race_basis = true;
    } else if (car_ready_ && !s_basis_optout && race_cam_.node_count() > 0) {
        // The first in-race Render comes before the first UpdateCar (measured: the
        // race-demo dump's seq 1 has countdown 3.0 and no director output). Evaluate
        // a COPY of the director on its snap path, which is what that first update
        // runs, so this frame is the game camera too. race_cam_ itself is untouched.
        Race::RaceCamera snap = race_cam_;
        Race::RaceCamCar cc[kRaceCars] = {};
        FillRaceCamCars(cc);
        snap.Update(cc, course_id_, 1.f / 60.f, static_cast<std::uint32_t>(cam_ticks_), 0.f, true);
        Race::ResolveRaceRenderCamera(snap, &rrc);   // nodes > 0, so Update ran (primes)
        for (int i = 0; i < 3; ++i) { eye[i] = rrc.pos[i]; at[i] = rrc.pos[i] + rrc.at[i]; }
        race_basis = true;
    } else if (car_ready_ && s_basis_optout) {
        // [opt-out] pre-d1r2 default: eye = director pos, target = director
        // look-at, Y-up LookAt below; a {0,0,0} pos (director not yet run) takes
        // the WS-E s3 chase-rig geometry.
        const float* rp = race_cam_.pos();
        const float* rt = race_cam_.target();
        const bool primed = (rp[0] != 0.f || rp[1] != 0.f || rp[2] != 0.f ||
                             rt[0] != 0.f || rt[1] != 0.f || rt[2] != 0.f);
        if (primed) {
            for (int i = 0; i < 3; ++i) { eye[i] = rp[i]; at[i] = rt[i]; }
        } else {
            const float fwd[3] = {std::cos(car_yaw_), 0.f, std::sin(car_yaw_)};
            const float L = car_len_;
            eye[0] = car_pos_[0] - fwd[0] * L * 1.5f;
            eye[1] = car_pos_[1] + L * 1.0f;
            eye[2] = car_pos_[2] - fwd[2] * L * 1.5f;
            at[0]  = car_pos_[0] + fwd[0] * L * 1.5f;
            at[1]  = car_pos_[1] + car_height_ * 0.5f;
            at[2]  = car_pos_[2] + fwd[2] * L * 1.5f;
        }
    } else {
        // [d1r2] The WS-E s3 "ground chase" rig that stood here (MASHED_CHASE_RIG,
        // and the fallback on every frame the director had not run) is deleted:
        // its premise, "the shared race camera sits in a high orbit", was refuted
        // 2026-08-30 (re/analysis/race_camera/render_camera_child_c_20260830.md).
        // With a car loaded this branch is reached only if the director has never
        // run (no camera-path nodes), and then the track orbit is shown rather
        // than an invented follow camera.
        if (car_ready_ && !s_basis_optout) {
            static bool s_said_unprimed = false;
            if (!s_said_unprimed) {
                s_said_unprimed = true;
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "[d1r2] in race but the race camera has not run "
                                     "(nodes=%d): track orbit view\n", race_cam_.node_count());
                    std::fclose(lf);
                }
            }
        }
        // Auto-orbit around the racing-line focus (gate centroid/extent), not
        // the bbox midpoint — keeps the track framed and at the surface Y. Eye
        // raised to ~1.1x extent for a clear, slightly-overhead vantage.
        const float* C = track_center_;
        const float  R = track_radius_;
        const float yaw = t * 0.3f;
        eye[0] = C[0] + R * 1.60f * std::cos(yaw);
        eye[1] = C[1] + R * 1.10f;
        eye[2] = C[2] + R * 1.60f * std::sin(yaw);
        at[0] = C[0]; at[1] = C[1]; at[2] = C[2];
    }
    // P3 same-view parity: pin to the captured pose. The 12-float basis form
    // carries roll, so it is applied as a basis rather than collapsed into an
    // at-point -- collapsing would throw the roll away, which is the whole
    // reason the form exists.
    // [d1r1] MASHED_CAM_REPLAY_BASIS=1 (with MASHED_CAM_REPLAY), DIAGNOSTIC ONLY, default OFF:
    // the frame renders from the ORIGINAL's own camera frame for the trace row this frame's
    // director update consumed (M_* = the RwCamera frame modelling matrix at onLeave of
    // FUN_00446520, which R2 measured to be what renders), as a per-row MASHED_CAM_POSE. R1
    // adjudicates rendering, so the director port is taken out of the comparison by
    // construction (verify/d1r1_20261016/PREREG_d1r1.md I-3).
    static const bool s_rep_basis_on =
        GetEnvironmentVariableA("MASHED_CAM_REPLAY_BASIS", nullptr, 0) != 0;
    const bool rep_basis = s_rep_basis_on && s_rep_basis_ok;
    const float* cpb = rep_basis ? s_rep_basis : s_campose;
    const bool campose_basis = (s_have_campose && s_campose_n == 12) || rep_basis;
    if (s_have_campose || rep_basis) race_basis = false;   // the parity transplant wins over the game camera
    if (s_have_campose && !campose_basis) {
        for (int i = 0; i < 3; ++i) { eye[i] = s_campose[i]; at[i] = s_campose[3 + i]; }
    } else if (campose_basis) {
        // last_eye_/last_at_ still get a look-at pair because RaceSceneState
        // publishes a point (librw + the HUD read it). The VIEW MATRIX below is
        // built from the basis, so the roll survives where it matters; only the
        // published at-point is the lossy summary.
        for (int i = 0; i < 3; ++i) {
            eye[i] = cpb[i];
            at[i]  = cpb[i] + cpb[9 + i];
        }
    }
    for (int i = 0; i < 3; ++i) { last_eye_[i] = eye[i]; last_at_[i] = at[i]; }
    // MASHED_DBG_CAM=1 (child-C): append the verbatim race camera's raw director
    // output AND the resolved render eye/at each frame to log/cam_re.txt
    // (override MASHED_DBG_CAM_OUT). Lets the basis be compared numerically to
    // the original's captured frame basis, not just via the pixel diff.
    {
        static const bool s_dbgcam =
            GetEnvironmentVariableA("MASHED_DBG_CAM", nullptr, 0) != 0;
        if (s_dbgcam) {
            static int s_cam_frame = 0;
            char cpath[MAX_PATH] = {};
            if (GetEnvironmentVariableA("MASHED_DBG_CAM_OUT", cpath,
                                        sizeof(cpath)) == 0)
                std::strcpy(cpath, "log/cam_re.txt");
            const float* rp = race_cam_.pos();
            const float* rt = race_cam_.target();
            if (std::FILE* f = std::fopen(cpath, "a")) {
                std::fprintf(f,
                    "f%d rcpos=(%.5f,%.5f,%.5f) rctgt=(%.5f,%.5f,%.5f) "
                    "eye=(%.5f,%.5f,%.5f) at=(%.5f,%.5f,%.5f) "
                    "reqzoom=%.4f vw=%.4f car_ready=%d race_basis=%d\n",
                    s_cam_frame, rp[0], rp[1], rp[2], rt[0], rt[1], rt[2],
                    eye[0], eye[1], eye[2], at[0], at[1], at[2],
                    race_cam_.required_zoom(), race_cam_.view_window(),
                    (int)car_ready_, (int)race_basis);
                std::fclose(f);
            }
            ++s_cam_frame;
        }
    }
    // Publish the BASIS too, not just the lossy at-point. librw is the default
    // renderer and draws the static world from what it is handed; handing it the
    // pair silently dropped the roll for the world pass (RaceSceneState.h).
    last_basis_valid_ = campose_basis || race_basis;
    if (campose_basis) {
        for (int i = 0; i < 3; ++i) {
            last_right_[i] = cpb[3 + i];
            last_up_[i]    = cpb[6 + i];
            last_atdir_[i] = cpb[9 + i];
        }
    } else if (race_basis) {
        // [d1r2] the game camera's frame, verbatim (eye was set to rrc.pos above)
        for (int i = 0; i < 3; ++i) {
            last_right_[i] = rrc.right[i];
            last_up_[i]    = rrc.up[i];
            last_atdir_[i] = rrc.at[i];
        }
    }
    last_cam_src_ = rep_basis ? 4 : (campose_basis ? 3 : (race_basis ? rrc.src : 0));
    ++cam_pub_seq_;

    D3DMATRIX viewm, projm, worldm;
    if (campose_basis)
        MatViewFromBasis(&viewm, &cpb[0], &cpb[3], &cpb[6], &cpb[9]);
    else if (race_basis)
        MatViewFromBasis(&viewm, rrc.pos, rrc.right, rrc.up, rrc.at);
    else
        MatLookAtLH(&viewm, eye, at);
    // E2'b step 3: publish the resolved frustum into RaceSceneState and build
    // the D3D9 projection FROM those members, so the librw submitter reads the
    // same four numbers rather than a hand-copied second set that could drift.
    //
    // LENS: MEASURED FROM THE ORIGINAL (2026-08-16), replacing the invented
    // 60 deg that stood here since WS-E s3. RenderWare stores no angle -- the
    // lens is RwCamera::viewWindow, the half-extents at unit distance -- so the
    // measured quantity is kViewWindowY and the angle below is arithmetic on it.
    //
    // Read live from MASHED.exe (TRAINING, Quick Battle) at
    // *(DAT_00897fe0 + 0x84) + 0x6c: viewWindow = (0.6000, 0.4500), near 0.1,
    // far 360.0, projType 1. Two cross-checks passed, so the offsets are right
    // and the value is not a hopeful pointer read:
    //   * recipViewWindow (+0x70/+0x74) == 1/viewWindow;
    //   * Camera::SetupFOV (0x00441700) reproduces it exactly from the
    //     controller fields -- vw.y = 0.75 * ctrl[0x70] * ctrl[0x58]
    //     = 0.75 * 1.0 * 0.6 = 0.45, with ctrl[0x6c]/[0x70] measured at 1.0.
    // Corroborated independently by hooks.csv:559 (MinimapCameraOrthoSetup,
    // "ortho 0.6 x 0.45"). Evidence: verify/d1_lens/RESULT.md + orig_lens.json.
    //
    // NOTE the WS-E s3 comment removed from here claimed the old ~48 deg branch
    // was superseded because it "framed the whole pack from a high orbit". That
    // branch had derived 48 deg from view-window 0.6 and was RIGHT; deleting it
    // is what left an invented constant in a shipping path for two months.
    //
    // 0.6/0.45 = 4:3, consistent with last_aspect_ below.
    // MASHED_CAM_FOV=<degrees> still overrides, for A/B against a capture.
    static const float kViewWindowY = 0.45f;              // MEASURED half-extent
    last_fov_    = 2.f * std::atan(kViewWindowY);         // = 48.46 deg
    // [d1r2] Game camera: SetupFOV 0x00441700's own arithmetic on the cam+0x58 the
    // tail left (0.6 from 0x0044800d, or entry 0's +0xc0 via 0x00442a20):
    // vw.y = DAT_005cc950 (0.75) * (+0x70 * +0x58), +0x70 = 1.0 measured
    // (verify/d1_lens). vw.x / vw.y = 1 / +0x24 = 4:3, as last_aspect_.
    if (race_basis) {
        const float vwy = static_cast<float>(0.75 * static_cast<double>(1.0f * rrc.view_window));
        last_fov_ = 2.f * std::atan(vwy);
    }
    if (rep_basis) last_fov_ = 2.f * std::atan(s_rep_vwy);   // [d1r1] the row's RwCamera viewWindow.y
    {
        static const float s_fov_override = [] {
            char b[32];
            if (GetEnvironmentVariableA("MASHED_CAM_FOV", b, sizeof(b)) == 0) return 0.f;
            const float deg = static_cast<float>(std::atof(b));
            return (deg > 1.f && deg < 179.f) ? deg * 3.14159265f / 180.f : 0.f;
        }();
        if (s_fov_override > 0.f) last_fov_ = s_fov_override;
    }
    // [d1r2] MASHED_CAM_DUMP=<csv>: one row per published camera, DIAGNOSTIC ONLY,
    // default OFF. %.9g round-trips a float, so the scorer can test bit equality.
    // seq joins the librw-side readback (RwRaceSubmit.cpp, MASHED_CAM_DUMP_LIBRW);
    // row = the MASHED_CAM_REPLAY row the latest race_cam_ update consumed (-1 none).
    {
        static std::FILE* s_cd = [] {
            const char* e = std::getenv("MASHED_CAM_DUMP");
            std::FILE* f = (e && e[0]) ? std::fopen(e, "w") : nullptr;
            if (f) std::fprintf(f, "seq,row,src,valid,ex,ey,ez,rx,ry,rz,ux,uy,uz,ax,ay,az,"
                                   "vw58,fov,cpx,cpy,cpz,yaw,pitch,roll,dx,dy,dz,flag,cd\n");
            return f; }();
        if (s_cd) {
            const float* cp = race_cam_.pos();
            const float* ad = race_cam_.aim();
            std::fprintf(s_cd, "%u,%d,%d,%d,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,"
                               "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,"
                               "%.9g,%.9g,%.9g,%d,%.9g\n",
                         cam_pub_seq_, cam_replay_row_, last_cam_src_, (int)last_basis_valid_,
                         last_eye_[0], last_eye_[1], last_eye_[2],
                         last_right_[0], last_right_[1], last_right_[2],
                         last_up_[0], last_up_[1], last_up_[2],
                         last_atdir_[0], last_atdir_[1], last_atdir_[2],
                         race_basis ? rrc.view_window : 0.6f, last_fov_,
                         cp[0], cp[1], cp[2], race_cam_.yaw_deg(), race_cam_.pitch_deg(),
                         race_cam_.roll_deg(), ad[0], ad[1], ad[2],
                         *reinterpret_cast<const int*>(0x00897fe0u), countdown_);
            std::fflush(s_cd);
        }
    }
    last_aspect_ = 800.f / 600.f;
    last_near_   = 0.1f;      // MEASURED (RwCamera +0x80); was an invented 0.05
    // FAR: the original's far clip is COURSE.LUA Setup_Fog's SECOND argument.
    // Not a formula on any radius -- the original computes no world radius at
    // all; radius_ is ours, invented at :1657, and radius_*8 stood here purely
    // as a guess.
    //
    // The track loader (0x00426e10) reads the course-description fog pair
    // (Setup_Fog handler 0x0047ab30 stores near at +0x1ec, far at +0x1f0),
    // guards on near != far, then writes near to the camera's fogPlane (+0x88)
    // and far through 0x004c1b10 (the RwCameraSetFarClipPlane equivalent, which
    // writes +0x84). 0x00426810 does the same +0x88 / +0x84 pairing per frame
    // for tracks with a camera anim.
    //
    // Confirmed by measurement on TRAINING, both arguments at once, which is
    // why no second track was needed: the live camera read back
    // fogPlane = 20.0 and farPlane = 360.0, and our own COURSE.LUA asset survey
    // at :1436-1440 records training as Setup_Fog(20, 360). Exact match on the
    // pair, so the near/far operand binding is settled too. Evidence:
    // verify/d1_lens/RESULT.md + orig_lens.json.
    //
    // No Setup_Fog -> the loader's near != far guard fails and the camera keeps
    // the init-time default, which is 180.0f at every camera-init site found
    // (0x0042d560, 0x0042f660, 0x00467110 all write 0x43340000).
    last_far_    = fog_on_ ? fog_end_ : 180.f;
    MatPerspectiveFovLH(&projm, last_fov_, last_aspect_,
                        last_near_, last_far_);
    // Same source of truth for the particle FX lens-wall guard: it sizes
    // billboards against the viewport, so it must read the FOV this frame
    // actually projects with rather than keep a second copy.
    parts_.SetFovY(last_fov_);
    MatIdentity(&worldm);
    dev->SetTransform(D3DTS_WORLD, &worldm);
    dev->SetTransform(D3DTS_VIEW, &viewm);
    dev->SetTransform(D3DTS_PROJECTION, &projm);

    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    // MIP-FIX: trilinear across the now-uploaded TXD mip pyramid (MakeTexture).
    dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    dev->SetFVF(kFVF);

    // The static world, props and cars are submitted to librw (RaceSubmit_AddInstance
    // below, RaceSubmit_Render from exe_main after this returns). The D3D9 draws left
    // in this function are the ones librw does not take: sky, particles,
    // pickups. The legacy D3D9 world / prop / car draws, and the MASHED_RENDER_LIBRW=0
    // revert that selected them, were deleted in D1-residue R3 (2026-10-16).

    // F3: hand librw the SAME animation clock this function scrolls its own
    // texture transforms with. Passing anything else (a private timer, a frame
    // count) would put the two renderers at different scroll phases, which is
    // indistinguishable from a shading bug -- that is precisely how D-S3-SEA read
    // for several rounds. `t` is the single source of truth; it is not re-derived.
    // Honours MASHED_NO_UVSCROLL so the kill-switch still disables BOTH paths
    // together and stays usable as the positive control that found this.
    LibRw::RaceSubmit_SetAnimTime(s_uvscroll_off ? 0.f : t);

    // sky clump first: unfogged, no depth write (renderer-gap closure)
    if (!sky_.batches.empty()) {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        // WS-E skybox: SKY.DFF is a small CAMERA-LOCKED dome (Arctic radius
        // ~1.9, centred at origin), NOT world-scale. Drawn at world origin it
        // collapses to a distant speck in-race (camera tens of units away) ->
        // black sky. Centre it on the camera eye so it surrounds the viewer.
        // Translate only (clouds keep world orientation as the camera turns);
        // z-write is off so every world surface draws over the dome.
        // [rprops U-9696] The original draws the sky clump per frame with its frame rebuilt from
        // the camera position, root-frame offset discarded (LoadIdentity), twice on Arctic:
        //   A  0x004491e0 (called by the Arctic frame function 0x004492b0, the draw at 0x0044937d):
        //      identity, rotate 180 deg about X then 180 deg about Y = diag(-1,-1,1), translate to the
        //      camera frame position (`cam_frame + 0x40`);
        //   B  0x00426d20 / 0x00426d90 (generic, the draw at 0x00426da2): identity, rotate about Y by
        //      the global at 0x0066d720 (never written, 0), translate to the camera position.
        // Arctic's recorded frame shows both (86-triangle `sky`, 0.265 apart in y); TRAINING and City
        // show one. MASHED_NO_SKYFIX=1 keeps the single translate-only copy with the baked root offset.
        static const bool s_no_skyfix = std::getenv("MASHED_NO_SKYFIX") != nullptr;
        D3DMATRIX skym[2];
        int nsky = 0;
        if (!s_no_skyfix && course_id_ == 0) {          // A: Arctic only (slot 4 of record "arctic")
            MatIdentity(&skym[nsky]);
            skym[nsky]._11 = -1.f; skym[nsky]._22 = -1.f;
            skym[nsky]._41 = eye[0]; skym[nsky]._42 = eye[1]; skym[nsky]._43 = eye[2];
            ++nsky;
        }
        MatIdentity(&skym[nsky]);                        // B (and the pre-rprops single copy)
        skym[nsky]._41 = eye[0]; skym[nsky]._42 = eye[1]; skym[nsky]._43 = eye[2];
        ++nsky;
        // MASHED_SKY_D3D9=1 keeps the sky on the legacy D3D9 draw (fixed matrices, old route);
        // MASHED_NO_SKYFIX=1 restores the whole pre-rprops sky: that route, one translate-only copy.
        static const bool s_sky_d3d9 = std::getenv("MASHED_SKY_D3D9") != nullptr;
        if (!s_sky_d3d9 && !s_no_skyfix && LibRw::RaceSubmit_Active() && sky_.rw_model >= 0) {
            // The librw path draws the sky before its world, depth and fog off (RwRaceSubmit.cpp).
            for (int si = 0; si < nsky; ++si)
                LibRw::RaceSubmit_AddSky(sky_.rw_model, reinterpret_cast<const float*>(&skym[si]));
        } else
        for (int si = 0; si < nsky; ++si) {
            dev->SetTransform(D3DTS_WORLD, &skym[si]);
            for (std::size_t mi = 0; mi < sky_.batches.size(); ++mi) {
                const auto& b = sky_.batches[mi];
                if (b.empty()) continue;
                dev->SetTexture(0, sky_.textures[mi]);
                // F3: scroll the UV-animated sky material (SKY.DFF "bmp_Sky_M").
                const bool scroll = !s_uvscroll_off && mi < sky_.mat_scroll.size() &&
                    (sky_.mat_scroll[mi].du != 0.f || sky_.mat_scroll[mi].dv != 0.f);
                if (scroll) {
                    D3DMATRIX tm; MatIdentity(&tm);
                    tm._31 = std::fmod(sky_.mat_scroll[mi].du * t, 1.f);
                    tm._32 = std::fmod(sky_.mat_scroll[mi].dv * t, 1.f);
                    dev->SetTransform(D3DTS_TEXTURE0, &tm);
                    dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                              D3DTTFF_COUNT2);
                }
                DrawBatch(dev, b);
                ds_tally(b, sky_.textures[mi]);
                if (scroll)
                    dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                              D3DTTFF_DISABLE);
            }
        }
        dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        dev->SetTransform(D3DTS_WORLD, &worldm);  // restore identity for world
    }
    ds_flush("sky");   // zeros when no sky clump loaded = the dark-void signal
    // fog (COURSE.LUA Setup_Fog) + alpha cutouts (fence/grate textures).
    // The fog range (Setup_Fog end) is calibrated for the in-game chase camera,
    // which sits a few units behind the car. The orbit/free OVERVIEW cameras sit
    // a full track-radius back (~1.6x extent), well beyond fog_end_, so applying
    // the gameplay fog there washes the entire track to fog_color (the dark
    // sliver we saw). Apply fog only when following the car (chase/race cam);
    // the overview cameras render unfogged so the whole track is visible.
    const bool chase_cam = car_ready_ && !free_;
    // WS-E FOG (2026-06-29): restore the in-race distance fog as the DEFAULT.
    // Background: the WS-A s3 perf fix moved the device to HARDWARE vertex
    // processing (9->180 fps). Under HW T&L, *vertex* fog (D3DRS_FOGVERTEXMODE)
    // white-washes our pre-lit world/car verts, which is why the perf merge gated
    // ALL chase fog off behind a MASHED_CHASE_FOG gate (since removed; no accessor
    // exists today — dead name retired 2026-09-09). The HW-correct path is *table*
    // (pixel) fog: FOGVERTEXMODE = NONE, FOGTABLEMODE = LINEAR, computed per-pixel
    // from interpolated depth — independent of vertex format, so it does NOT wash
    // out. The earlier "table fog also measured ~221 whiteout" reading pre-dated
    // the WS-E trackfix (commit f3091cdb): the old `fa*fb` near-plane parse
    // INVERTED the linear ramp for every track with near >= 1, clamping all near
    // geometry to full fog colour. With the corrected parse (fog_start_ = near,
    // fog_end_ = far, near < far on every track) the ramp is monotone: near
    // geometry clear, distance fades to the track fog colour. The projection
    // (MatPerspectiveFovLH above) emits w = eye-space z (m._34 = 1), so on
    // WFOG-capable hardware FOGSTART/FOGEND are the Setup_Fog absolute camera
    // distances directly.
    //
    // Fog applies only to the chase/race cam (chase_cam): the orbit/overview
    // cameras sit a full track-radius back, well beyond fog_end_, and would wash
    // the whole track to fog colour — those render unfogged. Set MASHED_NO_FOG=1
    // to disable the distance fog entirely (escape hatch).
    static const bool s_no_fog = (std::getenv("MASHED_NO_FOG") != nullptr);
    if (fog_on_ && chase_cam && !s_no_fog) {
        dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
        dev->SetRenderState(D3DRS_FOGCOLOR, fog_color_);
        dev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);   // NOT vertex fog (HW T&L washes)
        dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);  // table/pixel fog (HW-correct)
        dev->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
        DWORD fs, fe;
        std::memcpy(&fs, &fog_start_, 4);
        std::memcpy(&fe, &fog_end_, 4);
        dev->SetRenderState(D3DRS_FOGSTART, fs);   // Setup_Fog near (abs cam dist)
        dev->SetRenderState(D3DRS_FOGEND, fe);     // Setup_Fog far  (abs cam dist)
    } else {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    }
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    dev->SetRenderState(D3DRS_ALPHAREF, 0x30);
    dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    MARK(d_other);   // setup + sky + fog state

    // MASHED_DBG_CARPOS=1 appends this frame's car WORLD positions to
    // log/carpos_re.txt (override MASHED_DBG_CARPOS_OUT). Built for race
    // first-frame parity: the original side already reports its car array
    // (re/frida/race_draw_burst.py CARPROJ), and without the same numbers from
    // this side there is no way to tell "the standalone did not DRAW the cars"
    // from "the standalone drew them somewhere else" -- the two look identical
    // in a screenshot taken through a transplanted camera. Off unless asked for.
    {
        static const bool s_carpos = [] {
            return GetEnvironmentVariableA("MASHED_DBG_CARPOS", nullptr, 0) != 0;
        }();
        if (s_carpos) {
            static int s_cp_frame = 0;
            char path[MAX_PATH] = {};
            if (GetEnvironmentVariableA("MASHED_DBG_CARPOS_OUT", path,
                                        sizeof(path)) == 0)
                std::strcpy(path, "log/carpos_re.txt");
            if (std::FILE* f = std::fopen(path, "a")) {
                std::fprintf(f, "f%d player_ready=%d pos=(%.4f,%.4f,%.4f) "
                                "yaw=%.4f ai_n=%d\n",
                             s_cp_frame, (int)car_ready_,
                             car_pos_[0], car_pos_[1], car_pos_[2], car_yaw_,
                             (int)ai_cars_.size());
                for (std::size_t i = 0; i < ai_cars_.size(); ++i)
                    std::fprintf(f, "f%d ai%u pos=(%.4f,%.4f,%.4f) yaw=%.4f\n",
                                 s_cp_frame, (unsigned)i,
                                 ai_cars_[i].pos[0], ai_cars_[i].pos[1],
                                 ai_cars_[i].pos[2], ai_cars_[i].yaw);
                std::fclose(f);
            }
            ++s_cp_frame;
        }
    }

    if (!prop_anims_.empty()) UpdatePropAnims();   // [rprops] Arctic ships, once per frame
    // R6: track props (tyre walls, crates, sea, freighter...), submitted to librw as
    // instanced models. The D3DMATRIX is each prop's MTS / identity world matrix; it
    // copies straight into the rw::Frame (RwRaceSubmit.cpp SetClumpTransform). A
    // model whose RaceSubmit_RegisterModel failed (rw_model < 0) is not drawn; the
    // failure is logged to log/librw_race.txt at load.
    // [rstates F8] The course load's pair loop keeps the clump's own atomic for Mts record 0 and
    // adds each later copy with RpClumpAddAtomic (0x00479ce5 -> FUN_004e6f80, head insertion),
    // so RpClumpRender (0x004e6680) draws an after-world pair from the last record to record 0.
    // MASHED_LIBRW_PAIRORDER=0 keeps record order.
    static const bool s_pair_order = [] {
        const char* e = std::getenv("MASHED_LIBRW_PAIRORDER");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    const bool prop_order_ok = (prop_order_.size() == props_.size());
    for (std::size_t k = 0; k < props_.size(); ++k) {
        const auto& p = props_[prop_order_ok ? prop_order_[k] : k];
        if (p.rw_model < 0) continue;
        if (s_pair_order && p.mts_pair && p.rw_kind == LibRw::kInstAfterWorld) {
            for (auto it = p.instances.rbegin(); it != p.instances.rend(); ++it)
                LibRw::RaceSubmit_AddInstance(p.rw_model, (const float*)&*it, p.rw_kind);
            continue;
        }
        for (const auto& inst : p.instances)
            LibRw::RaceSubmit_AddInstance(p.rw_model, (const float*)&inst, p.rw_kind);
    }

    MARK(d_props);   // instanced track props


    MARK(d_other);   // copters
    // [rfx 2026-10-16] the in-race HUD's inputs, from this standalone's own state (LibRw/RwRaceFx.h).
    // State: the real DAT_0063ba8c under MASHED_SUBSTATE; with the stand-in countdown, its first part
    // stands for phase 3 (no HUD, FUN_0040dfc0 returns below 5), its last 113 frames for states 4/5
    // (the adapter in Update), racing for 6.
    {
        LibRw::HudInputs h;
        const int ba8c = *reinterpret_cast<const int*>(0x0063ba8cu);
        if (SubState_Enabled() && ba8c != 0) h.state = ba8c;
        else h.state = (countdown_ > 113.0f / 60.0f) ? 3 : (countdown_ > 0.f ? 5 : 6);
        h.timer = *reinterpret_cast<const float*>(0x0063d588u);
        h.team_play = *reinterpret_cast<const int*>(0x0067ea64u);
        h.round_type = *reinterpret_cast<const int*>(0x0067e9fcu);
        h.mode_fd0 = *reinterpret_cast<const int*>(0x007f0fd0u);
        h.n4d0 = *reinterpret_cast<const int*>(0x008a94d0u);
        for (int i = 0; i < 4; ++i) {
            const std::uintptr_t b = 0x007f1a14u + static_cast<std::uintptr_t>(i) * 16u;
            h.slot[i] = *reinterpret_cast<const int*>(b);
            h.team[i] = *reinterpret_cast<const int*>(b + 4u);
            h.colour[i] = *reinterpret_cast<const int*>(b + 8u);
            h.score[i] = score(i);
        }
        LibRw::RaceFx_SetHud(h);
    }
    // R5: the cars, submitted through librw. Car slot s is one placed instance of its livery's
    // model (car_models_[car_slot_model_[s]], FUN_0040d110 @ 0x0040d110), built with the DFF's
    // frame hierarchy; the placement is the same D3DMATRIX the removed D3D9 car path built
    // (long axis along the heading) and goes to the clump root frame, as FUN_00420870's
    // RwFrameTransform(clump frame, body matrix, REPLACE) (0x004208ae). Each copy carries the
    // wheel / hub / body frame poses FUN_00420870 writes (BuildCarPoses, from the inputs
    // CarParts_FrameTick read) and FUN_00420050's directional-light scale (rec+0x284).
    // MASHED_CAR_PARTS=0 drops the poses, MASHED_CAR_RELIGHT=0 the light scale.
    if (livery_dirty_) ResolveCarLiveries();
    CarPartsLawReplay();   // [librwrest_checks] MASHED_CARPARTS_LAW, default OFF, one shot
    if (car_ready_ && rw_car_model_ >= 0) {
        static const bool s_relight = [] {
            const char* e = std::getenv("MASHED_CAR_RELIGHT");
            return !(e && e[0] == '0' && e[1] == '\0');
        }();
        D3DMATRIX carm;
        MatIdentity(&carm);
        const float cy = std::cos(car_yaw_), sy2 = std::sin(car_yaw_);
        if (car_long_is_x_) {
            carm._11 =  cy;  carm._13 = sy2;
            carm._31 = -sy2; carm._33 = cy;
        } else {
            carm._31 =  cy;  carm._33 = sy2;
            carm._11 = sy2;  carm._13 = -cy;
        }
        carm._41 = car_pos_[0]; carm._42 = car_pos_[1]; carm._43 = car_pos_[2];
        // [d1r1] MASHED_CAR_REPLAY=1 (with MASHED_CAM_REPLAY), DIAGNOSTIC ONLY, default OFF:
        // each car renders under the ORIGINAL's matrices for the consumed trace row (slot 0 =
        // player, ai_cars_[k] = slot k+1), so the draw-list adjudication compares rendering at
        // matched car poses (PREREG_d1r1.md I-3). [librwrest] With the frame hierarchy the
        // clump root takes the root LTM (Q{i}_*), and the body frame's pose is recovered from
        // the renderable's frame LTM (F{i}_*, part 0xe): M(body) = X^-1 * F * (Y * Q)^-1
        // (CarModel::body_X / body_Y). The wheel frames have no trace columns: they keep the
        // standalone's own inputs.
        static const bool s_car_rep =
            GetEnvironmentVariableA("MASHED_CAR_REPLAY", nullptr, 0) != 0;
        bool rep_body[4] = {false, false, false, false};
        float rep_body_m[4][12];
        auto rep_car = [&](int slot, D3DMATRIX* m) {
            if (!s_car_rep || !s_rep_car_ok) return;
            const CarModel& cm = car_models_[static_cast<std::size_t>(car_slot_model_[slot])];
            const bool hier = CarPartsEnabled() && s_rep_root_ok && cm.body_chain_ok;
            const float* f = hier ? s_rep_root[slot] : s_rep_car[slot];
            MatIdentity(m);
            m->_11 = f[0]; m->_12 = f[1];  m->_13 = f[2];
            m->_21 = f[3]; m->_22 = f[4];  m->_23 = f[5];
            m->_31 = f[6]; m->_32 = f[7];  m->_33 = f[8];
            m->_41 = f[9]; m->_42 = f[10]; m->_43 = f[11];
            if (hier) {
                float yq[12], yqi[12], xi[12], t[12];
                AffMul(cm.body_Y, s_rep_root[slot], yq);
                AffInv(yq, yqi);
                AffInv(cm.body_X, xi);
                AffMul(xi, s_rep_car[slot], t);
                AffMul(t, yqi, rep_body_m[slot]);
                rep_body[slot] = true;
            }
        };
        // [rfx 2026-10-16] car shadow / headlight beam inputs (LibRw/RwRaceFx.h), per car slot.
        // root = the clump frame (the car instance matrix; under MASHED_CAR_REPLAY the trace's
        // root LTM Q{i}_*, equal to R{i}_* for a root frame), body = the frame the beam parts
        // hang off (F{i}_* under the replay; outside it the instance matrix, without the
        // librwrest body pose [UNCERTAIN U-9703]).
        auto fx_car = [&](int slot, const D3DMATRIX& body) {
            LibRw::CarFxInput in{};
            std::memcpy(in.body, &body, sizeof(in.body));
            std::memcpy(in.root, &body, sizeof(in.root));
            if (s_car_rep && s_rep_car_ok) {
                const float* f = s_rep_car[slot];
                D3DMATRIX b;
                MatIdentity(&b);
                b._11 = f[0]; b._12 = f[1];  b._13 = f[2];
                b._21 = f[3]; b._22 = f[4];  b._23 = f[5];
                b._31 = f[6]; b._32 = f[7];  b._33 = f[8];
                b._41 = f[9]; b._42 = f[10]; b._43 = f[11];
                std::memcpy(in.body, &b, sizeof(in.body));
            }
            if (s_car_rep && s_rep_root_ok) {
                const float* f = s_rep_root[slot];
                D3DMATRIX r;
                MatIdentity(&r);
                r._11 = f[0]; r._12 = f[1];  r._13 = f[2];
                r._21 = f[3]; r._22 = f[4];  r._23 = f[5];
                r._31 = f[6]; r._32 = f[7];  r._33 = f[8];
                r._41 = f[9]; r._42 = f[10]; r._43 = f[11];
                std::memcpy(in.root, &r, sizeof(in.root));
            }
            // FUN_0041f590 @ 0x0041f590 (from FUN_00420870): when (DAT_007f101c & 3) == rec+0x290
            // (= slot, 0x0041f611), cast the clump frame position +[0x005cd058] (0.0625) to
            // -[0x005cd278] (0.9375) in y (FUN_00426dc0, 0x0041f628) and store rec+0x288 =
            // 1.0 - t ([0x5cc320], 0x0041f63e..0x0041f648), t initialised to 1.0 (0x0041f5a2).
            if ((gloss_frame_ & 3u) == static_cast<std::uint32_t>(slot)) {
                const float a[3] = { in.root[12], in.root[13] + 0.0625f, in.root[14] };
                const float b[3] = { in.root[12], in.root[13] - 0.9375f, in.root[14] };
                float s = 0.f, t = 1.f;
                CollisionPrelitSample(col_verts_, col_tris_, col_prelit_, col_tri_pl_, a, b, &s, &t);
                car_fade_[slot] = 1.0f - t;
            }
            in.fade = car_fade_[slot];
            // rec+0x294 & 0x40 = DAT_0063bc44[car] (FUN_0040da50 -> FUN_0041eeb0, 0x0040db3d): set when
            // FUN_00426c00() = DAT_00644158 (= COURSE.LUA Course_Id, course_id_) is 0, 0x1a, 0x21 or
            // 0x24 and the car's vehicle slot is live (FUN_0046c7b0 / FUN_0046c770 == 1).
            // [UNCERTAIN U-9703] liveness is aib_alive(slot) here; FUN_0041eeb0's clears of 0x40 under
            // rec+0x294 bits 0x10 / 0x400 (not set by the standalone) are not modelled.
            in.lights = (course_id_ == 0 || course_id_ == 0x1a || course_id_ == 0x21 ||
                         course_id_ == 0x24) && aib_alive(slot) == 1;
            LibRw::RaceFx_QueueCar(slot, in);
        };
        // [rtex 2026-10-16] per-car shade / env state, in the order FUN_00420870 @ 0x00420870
        // runs it each frame: env frame angle, shade window (FUN_0041f590), then the
        // coefficient FUN_00420340 -> FUN_00420290 applies (LibRw/RwCarFx.h). [librwrest] The
        // shade window runs for every car (FUN_00420050 scales the directional light by it);
        // the env coefficient only when the env pass is bound.
        auto submit = [&](int slot, const D3DMATRIX& m, const float* vel) {
            CarGloss& g = car_gloss_[slot];
            // 0x0041f5aa..0x0041f5ba: window shift w[k] = w[k-1], k = 7..1
            for (int k = 7; k > 0; --k) g.win[k] = g.win[k - 1];
            // 0x0041f5d1..0x0041f617: sample when (DAT_007f101c & 3) == rec+0x290; segment
            // from the clump frame position +[0x005cd058] (0.0625) to -[0x005cd278] (0.9375) in y
            if ((gloss_frame_ & 3u) == static_cast<std::uint32_t>(slot)) {
                const float a[3] = { m._41, m._42 + 0.0625f, m._43 };
                const float b[3] = { m._41, m._42 - 0.9375f, m._43 };
                float s = 0.f;
                if (CollisionPrelitSample(col_verts_, col_tris_, col_prelit_, col_tri_pl_, a, b, &s))
                    g.win[0] = s;
            }
            // 0x0041f658..0x0041f6b5: rec+0x284 = sum(w) * [0x005cd050] (0.125), clamped to
            // [0, 1] (0x0041f6bb..0x0041f700); x87 accumulates, so the sum is double here
            double sum = 0.0;
            for (int k = 0; k < 8; ++k) sum += g.win[k];
            double sh = sum * 0.125;
            if (sh > 1.0) sh = 1.0; else if (sh < 0.0) sh = 0.0;
            g.shade = static_cast<float>(sh);
            // [librwrest] MASHED_DBG_CARSHADE=<path>, DIAGNOSTIC ONLY, default OFF: per car per
            // frame the root position and rec+0x284 (R1 witness, PREREG_librwrest.md)
            {
                static char s_shade_path[MAX_PATH] = {};
                static const bool s_shade_log =
                    GetEnvironmentVariableA("MASHED_DBG_CARSHADE", s_shade_path, MAX_PATH) != 0;
                if (s_shade_log)
                    if (std::FILE* sf = std::fopen(s_shade_path, "a")) {
                        std::fprintf(sf, "%u,%d,%.5f,%.5f,%.5f,%.6f\n", gloss_frame_, slot,
                                     m._41, m._42, m._43, g.shade);
                        std::fclose(sf);
                    }
            }
            const CarModel& cm = car_models_[static_cast<std::size_t>(car_slot_model_[slot])];
            LibRw::FramePose poses[20];
            int np = 0;
            if (CarPartsEnabled() && car_parts_[slot].ok) np = BuildCarPoses(cm, car_parts_[slot], poses);
            if (rep_body[slot] && cm.body_frame >= 0) {
                int bi = -1;
                for (int k = 0; k < np; ++k) if (poses[k].frame == cm.body_frame) bi = k;
                if (bi < 0 && np < 20) bi = np++;
                if (bi >= 0) {
                    poses[bi].frame = cm.body_frame;
                    std::memcpy(poses[bi].m, rep_body_m[slot], sizeof(poses[bi].m));
                }
            }
            const float sun = s_relight ? g.shade : -1.f;
            if (car_env_ok_ && cm.env_ok) {
                // 0x00420870 decomp: rec+0x280 -= (v.z + v.x) * [0x005cd2ac] (0.0003), v from
                // FUN_0046cb30 (the body record's +0x9b0 velocity); then RwFrameRotate(env frame,
                // clump frame right, rec+0x280, rwCOMBINEREPLACE) (FUN_004c1520)
                g.angle = g.angle - (vel[2] + vel[0]) * 0.0003f;
                // FUN_00420290 @ 0x00420290: coef = ((hi - lo) * rec+0x284 + lo)
                //   * (1 - clamp(|part 0x13 LTM pos - camera frame pos|^2, 0, 100) * 0.01)
                const float px = car_part19_[0] * m._11 + car_part19_[1] * m._21 + car_part19_[2] * m._31 + m._41;
                const float py = car_part19_[0] * m._12 + car_part19_[1] * m._22 + car_part19_[2] * m._32 + m._42;
                const float pz = car_part19_[0] * m._13 + car_part19_[1] * m._23 + car_part19_[2] * m._33 + m._43;
                const double dx = px - last_eye_[0], dy = py - last_eye_[1], dz = pz - last_eye_[2];
                double d2 = dz * dz + dx * dx + dy * dy;   // FUN_0041f120 @ 0x0041f120 order
                if (!(d2 < 100.0)) d2 = 100.0; else if (d2 < 0.0) d2 = 0.0;
                const double lerp = (static_cast<double>(shine_hi_) - shine_lo_) * g.shade + shine_lo_;
                const float coef = static_cast<float>(lerp * (1.0 - d2 * 0.01));
                float env[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
                const float axis[3] = { m._11, m._12, m._13 };
                RwMatrixRotate(env, axis, g.angle, 0);
                LibRw::RaceSubmit_AddInstancePosed(cm.handle, (const float*)&m, poses,
                                                   static_cast<std::size_t>(np), sun, coef, env);
            } else {
                LibRw::RaceSubmit_AddInstancePosed(cm.handle, (const float*)&m, poses,
                                                   static_cast<std::size_t>(np), sun);
            }
        };
        rep_car(0, &carm);
        fx_car(0, carm);                                             // [rfx]
        submit(0, carm, car_vel_);
        int ai_slot = 1;
        for (const auto& a : ai_cars_) {
            D3DMATRIX am;
            MatIdentity(&am);
            const float ay = std::cos(a.yaw), asy = std::sin(a.yaw);
            if (car_long_is_x_) {
                am._11 =  ay;  am._13 = asy;
                am._31 = -asy; am._33 = ay;
            } else {
                am._31 =  ay;  am._33 = asy;
                am._11 = asy;  am._13 = -ay;
            }
            am._41 = a.pos[0]; am._42 = a.pos[1]; am._43 = a.pos[2];
            if (ai_slot < 4) {
                rep_car(ai_slot, &am);
                fx_car(ai_slot, am);                                 // [rfx]
                submit(ai_slot, am, a.vel);
            } else {
                LibRw::RaceSubmit_AddInstance(rw_car_model_, (const float*)&am, LibRw::kInstCar);
            }
            ++ai_slot;
        }
        // [rfx] the car shadow / beam pass runs here, after the car loop and before the copters
        // below (FUN_00410b30: FUN_0041ebb0 at 0x00410bb4, FUN_00425e40 at 0x00410c08)
        LibRw::RaceSubmit_AddCarFxMarker();
        ++gloss_frame_;   // [rtex] DAT_007f101c stand-in, +1 per race frame (0x004111c1)
    }

    // F2 / [librwrest] animated copters, through librw, after the cars as FUN_00410b30 orders
    // them (the car loop, then FUN_00425e40 @ 0x00425e40 from 0x00410c08). The clump root takes
    // FUN_004752f0's matrix at the copter's own clock (FUN_00425d50: FUN_004c1480(clump frame,
    // matrix, REPLACE)); the rotor frames carry the angles FUN_00425ca0 has added (CopterTick).
    // The model's draw order and frustum cull are FUN_00425e40's (RaceSubmit_SetModelDrawOrder).
    // MASHED_NO_COPTERS suppresses the pass (A/B verification toggle).
    static const bool s_copters_off = std::getenv("MASHED_NO_COPTERS") != nullptr;
    if (!s_copters_off)
    for (const auto& c : copters_) {
        if (c.model < 0 || c.model >= static_cast<int>(copter_models_.size()) ||
            c.anim.frames.empty())
            continue;
        const CopterModel& cm = copter_models_[static_cast<std::size_t>(c.model)];
        if (cm.rw_model < 0) continue;
        float rm[16];
        CopterAnimMatrix(c.anim, c.elapsed, rm);
        D3DMATRIX cmx;
        MatIdentity(&cmx);
        cmx._11 = rm[0];  cmx._12 = rm[1];  cmx._13 = rm[2];
        cmx._21 = rm[4];  cmx._22 = rm[5];  cmx._23 = rm[6];
        cmx._31 = rm[8];  cmx._32 = rm[9];  cmx._33 = rm[10];
        cmx._41 = rm[12]; cmx._42 = rm[13]; cmx._43 = rm[14];
        LibRw::FramePose poses[2];
        int np = 0;
        auto rotor = [&](int frame, const float* bind, float angle) {
            if (frame < 0) return;
            // FUN_00425ca0: translate by -pos, rotate about the frame's up vector (frame+0x20),
            // translate back, each rwCOMBINEPOSTCONCAT
            float mm[16] = { bind[0], bind[1], bind[2], 0.f, bind[3], bind[4], bind[5], 0.f,
                             bind[6], bind[7], bind[8], 0.f, 0.f, 0.f, 0.f, 0.f };
            const float up[3] = { bind[3], bind[4], bind[5] };
            RwMatrixRotate(mm, up, angle, 2);
            float p[3] = { bind[9], bind[10], bind[11] };
            FUN_004c51a0(mm, p, 2);
            PoseFromRwMatrix(mm, frame, &poses[np++]);
        };
        rotor(cm.rotA_frame, cm.rotA_bind, c.rotA);
        rotor(cm.rotB_frame, cm.rotB_bind, c.rotB);
        LibRw::RaceSubmit_AddInstancePosed(cm.rw_model, (const float*)&cmx, poses,
                                           static_cast<std::size_t>(np), -1.f, 0.f, nullptr,
                                           LibRw::kInstAfterWorld);   // [rstates] not in the world
    }

    dev->SetTexture(0, nullptr);
    MARK(d_car);     // player + AI cars (librw instances)

    // In-race particles (weather + car dust): update with the per-frame dt and
    // draw camera-facing billboards. Depth-test ON so geometry occludes them;
    // depth-write OFF so they blend. Only while actually racing (car present).
    // MASHED_NO_PARTICLES=1 suppresses the whole pass (A/B + the dust/snow bloom
    // is a separate WS-E tuning issue; this isolates the world/car render).
    static const bool s_no_particles = (std::getenv("MASHED_NO_PARTICLES") != nullptr);
    if (!s_no_particles && (parts_.ambient() != ParticleSystem::None || car_ready_)) {
        const float dt = (last_t_ < 0.f) ? 0.f : (t - last_t_);
        last_t_ = t;
        float fwd[3] = {at[0]-eye[0], at[1]-eye[1], at[2]-eye[2]};
        // Collision/slide FX (skids + impact sparks) for every car, before the
        // pool advances this frame. Emit once per frame; parts_.Render is the
        // per-view half (kept separate — see D-11063's particle constraint).
        if (car_ready_) {
            EmitCarFx(0, car_pos_, car_vel_, car_speed_, car_yaw_, dt);
            // AiCar carries no velocity vector on main (pos/yaw/target/speed/
            // cur_speed/lane), so pass an explicit zero. This is faithful rather
            // than lossy: the AI/kinematic cars encode motion as (speed, yaw) and
            // their lateral-slip term was already ~0, which is exactly why the
            // trigger is yaw-rate-based (see EmitCarFx).
            static const float kNoVel[3] = { 0.f, 0.f, 0.f };
            for (int i = 0; i < static_cast<int>(ai_cars_.size()) && i < 3; ++i)
                EmitCarFx(i + 1, ai_cars_[i].pos, kNoVel,
                          ai_cars_[i].cur_speed, ai_cars_[i].yaw, dt);
        }
        parts_.Update(dt, eye, fwd, car_ready_ ? car_pos_ : nullptr,
                      car_ready_ ? car_speed_ : 0.f);
        parts_.Render(dev, eye, at);
        // power-up orbs (additive glow billboards) on top of the weather.
        // D1 isolation: the orbs blend ADDITIVE (PickupField.cpp:231
        // DESTBLEND_ONE), so they are a candidate for a saturated white-out on
        // their own. MASHED_NO_PICKUPS separates them from the particle pass,
        // which MASHED_NO_PARTICLES alone cannot do (it kills both).
        static const bool s_no_pickups = (std::getenv("MASHED_NO_PICKUPS") != nullptr);
        if (!s_no_pickups && pickups_.enabled()) pickups_.Render(dev, eye, at);
    }

    MARK(d_part);    // particles + pickups

    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  // back to the 2D menu path

    if (s_rprof) {
        static std::FILE* lf = nullptr; static bool op = false; static int fn = 0;
        if (!op) { lf = std::fopen("render_prof.log", "w");
            if (lf) std::fprintf(lf, "frame,total_ms,props_ms,car_ms,part_ms,other_ms\n");
            op = true; }
        const double total = RMs(_rt0, RT());
        if (lf && fn < 6000) {
            std::fprintf(lf, "%d,%.3f,%.3f,%.3f,%.3f,%.3f\n",
                fn, total, d_props, d_car, d_part, d_other);
            std::fflush(lf);
        }
        ++fn;
    }
}

}  // namespace D3d9Render
}  // namespace mashed_re
