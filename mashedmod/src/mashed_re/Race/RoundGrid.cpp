// Mashed RE — round-restart grid, Stage A. See RoundGrid.h and verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md.
//
// FUN_00409290 is SnapNodeToSafeStartLines (Race/RoundGridSnap.cpp, a TU shared with the .asi, so one body per RVA);
// its callees FUN_00426bc0 / FUN_00426bd0 are the C3 exports GetDat0066d6e0 / GetTableEntry0066d658, which read the
// Safe_Start_Lines table PathNodes.cpp builds at 0x0066d658 / 0x0066d6e0 (PATHPROG P1).
#include "RoundGrid.h"
#include "PathNodes.h"
#include "PathProgStep.h"   // PathProg_PublishMask
#include "../Math/RwLutTables.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" void __cdecl SnapNodeToSafeStartLines(std::int32_t* p);                                // 0x00409290

namespace mashed_re {
namespace Vehicle { void BodyOrient_NoteExactBasis(float yaw, float atX, float atZ); }   // BodyOrientationIntegrate.cpp [lk21]
namespace Race {

namespace {
constexpr std::uintptr_t kNode = 0x0063ba80u;          // DAT_0063ba80
constexpr std::uintptr_t kRec  = 0x00663658u;          // node records, stride 0x4c
constexpr std::uintptr_t kNodeCount = 0x0066d6d8u;
inline std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
inline const float*  FP(std::uintptr_t a)  { return reinterpret_cast<const float*>(a); }
std::FILE* Trace() {
    static std::FILE* f = nullptr; static bool init = false;
    if (!init) {
        init = true;
        if (const char* p = std::getenv("MASHED_ROUND_GRID_TRACE")) f = std::fopen(p, "w");
    }
    return f;
}
}  // namespace

bool RoundGrid_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_ROUND_GRID");
        return s && s[0] && s[0] != '0';
    }();
    // [PATHPROG split] the progress it reads is live under wholesale MASHED_PATHPROG or under the shadow-row tick with
    // MASHED_PP_PUBLISH bit 0x2 (node progress published); both build the course block.
    return s_on && (PathProg_Enabled() || (RacePctOrig_Enabled() && (PathProg_PublishMask() & 0x2u)));
}

void RoundGrid_RaceInit() {
    if (!RoundGrid_Enabled()) return;
    I32(kNode) = 0;                                                    // 0x0040fef0 mov [0x0063ba80], ebx (0)
    if (std::FILE* f = Trace()) { std::fprintf(f, "race-init node=0\n"); std::fflush(f); }
}

namespace { int s_codes[4] = { -1000, -1000, -1000, -1000 }; }   // FUN_0040b180 seed 0xfffffc18

void RoundGrid_RoundEnd(const bool alive[4], int exeWinner, const int codes[4]) {
    if (!RoundGrid_Enabled()) return;
    for (int i = 0; i < 4; ++i) s_codes[i] = codes[i];
    // FUN_00410860 0x00410890..0x004108b3: the LAST slot i in 0..3 that is active and whose FUN_0046c7b0 == 1.
    int surv = 0;                                                      // ebx starts 0 (0x00410880)
    for (int i = 0; i < 4; ++i) if (alive[i]) surv = i;
    const float prog = *FP(0x008a96e8u + static_cast<std::uintptr_t>(surv) * 0x30cu);   // FUN_00408a50
    I32(kNode) = static_cast<std::int32_t>(prog);                      // __ftol (truncate) -> 0x004108c5
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "round-end alive=%d%d%d%d survivor(orig rule)=%d exe_winner=%d prog=%.6g node=%d codes=%d,%d,%d,%d\n",
                     alive[0], alive[1], alive[2], alive[3], surv, exeWinner, prog, I32(kNode),
                     codes[0], codes[1], codes[2], codes[3]);
        std::fflush(f);
    }
}

bool RoundGrid_AnchorAt(int n, float A[3], float lat[2], float dir[2]) {
    const int count = I32(kNodeCount);
    if (count <= 0 || n < 0 || n >= count) return false;
    const std::uintptr_t r = kRec + static_cast<std::uintptr_t>(n) * 0x4cu;
    const float* c = FP(r + 0x0c);                                     // FUN_00426cb0
    const float* d = FP(r + 0x00);                                     // FUN_00426cc0
    const float* k0 = FP(r + 0x18);                                    // FUN_00426d00(n, 0) = r + 0x18
    const float* k3 = FP(r + 0x18 + 3 * 12);                           // FUN_00426d00(n, 3)
    for (int i = 0; i < 3; ++i) A[i] = c[i];
    float f3[3] = { k0[0] - k3[0], k0[1] - k3[1], k0[2] - k3[2] }, fn[3];
    Math::LutNormalize(fn, f3);                                        // FUN_004c39b0
    // FUN_00408b00: the lateral axis is the x/z of the 3-D-normalized corner difference (FUN_004c39b0), and the
    // rank offsets multiply the RAW record direction (local_30/local_2c/local_28 * 1.1 / 2.2 / 3.3, taken before
    // FUN_004c3c60 normalizes the copy local_68 used only for the yaw). No 2-D renormalization of either.
    if ((fn[0] == 0.f && fn[2] == 0.f) || (d[0] == 0.f && d[2] == 0.f)) return false;
    lat[0] = fn[0]; lat[1] = fn[2];
    dir[0] = d[0]; dir[1] = d[2];
    return true;
}

// [u9755] FUN_00408b00's grid point for slot 0..3 (param_2 < 4 block, verify/grid_orig_ts/FUN_00408b00_decomp.txt), per axis:
// fn * _DAT_005cc9bc (0.8) and (fn * _DAT_005ccac0 (0.4)) * 0.8, rank term = d * 1.1 / 2.2 / 3.3 (_DAT_005ccabc / b8 / b4),
// evaluated in the decomp's order. place() combines the lateral terms into one multiplier (0.48 / 1.12) and grounds y with
// GroundHeight; FUN_004704c0 takes this point as param_2..4 (MASHED_SPAWN_Y_LINE, o_sdrop1: y 1.17499995 for every car).
bool RoundGrid_GridPoint(int n, int slot, float out[3]) {
    const int count = I32(kNodeCount);
    if (count <= 0 || n < 0 || n >= count || slot < 0 || slot > 3) return false;
    const std::uintptr_t r = kRec + static_cast<std::uintptr_t>(n) * 0x4cu;
    const float* a = FP(r + 0x0c);                                     // FUN_00426cb0 -> local_48 / 44 / 40
    const float* d = FP(r + 0x00);                                     // FUN_00426cc0 -> local_30 / 2c / 28
    const float* k0 = FP(r + 0x18);
    const float* k3 = FP(r + 0x18 + 3 * 12);
    float f3[3] = { k0[0] - k3[0], k0[1] - k3[1], k0[2] - k3[2] }, fn[3];
    Math::LutNormalize(fn, f3);                                        // FUN_004c39b0(&fStack_54, &fStack_54)
    for (int i = 0; i < 3; ++i) {
        const float s = fn[i] * 0.8f;                                  // fStack_54.. * _DAT_005cc9bc
        const float l = (fn[i] * 0.4f) * 0.8f;                         // fStack_3c.. = fn * _DAT_005ccac0; local_60.. = * 0.8
        switch (slot) {
        case 0: out[i] = (a[i] + s) - l; break;
        case 1: out[i] = ((a[i] - s) - l) - d[i] * 1.1f; break;        // fStack_18 / 14 / 10
        case 2: out[i] = ((a[i] + s) + l) - d[i] * 2.2f; break;        // fStack_24 / 20 / 1c
        default: out[i] = ((a[i] - s) + l) - d[i] * 3.3f; break;       // fStack_c / 8 / 4
        }
    }
    return true;
}

bool RoundGrid_Anchor(float A[3], float lat[2], float dir[2], int* node) {
    if (!RoundGrid_Enabled()) return false;
    std::int32_t v = I32(kNode);
    SnapNodeToSafeStartLines(&v);                                      // FUN_00409290(&DAT_0063ba80)
    I32(kNode) = v;
    if (node) *node = v;
    const bool ok = RoundGrid_AnchorAt(v, A, lat, dir);
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "round-start node=%d ok=%d A=(%.6g,%.6g,%.6g) lat=(%.6g,%.6g) dir=(%.6g,%.6g)\n", v, ok,
                     A[0], A[1], A[2], lat[0], lat[1], dir[0], dir[1]);
        std::fflush(f);
    }
    return ok;
}

// ---- [gridyaw 2026-10-16] MASHED_GRID_DIR: FUN_00408b00's grid frame at the node the port grids at ----
bool GridDir_Enabled() {
    static const bool s_on = [] {
        // [d2ref flip 2026-10-17] default ON (user decision); MASHED_NO_GRID_DIR=1 opts out (the gate0 -> gate1 stand-in)
        const char* no = std::getenv("MASHED_NO_GRID_DIR");
        return !(no && no[0] && no[0] != '0');
    }();
    return s_on;
}

// FUN_00408b00 (verify/grid_orig_ts/FUN_00408b00_decomp.txt): local_68/local_64 = L.x / L.z; FUN_004c3c60 2-D
// normalizes them in place (LUT); FUN_004726b0(&(0.0, 1.0), &local_68) = the 2-D dot, clamped to
// [_DAT_005cc33c -1.0, _DAT_005cc320 1.0]; FUN_004a3384 (CRT acos) * _DAT_005cc98c (57.29578, 0x42652ee1); if the
// normalized x < DAT_005d757c (0.0): _DAT_005ccac4 (360.0) - that. Stored to *param_5 as a float.
float GridDir_LawDeg(float lx, float lz) {
    const float in[2] = { lx, lz };
    float n[2];
    Math::LutNormalize2D(n, in);                                       // FUN_004c3c60
    double d = 0.0 * static_cast<double>(n[0]) + 1.0 * static_cast<double>(n[1]);   // FUN_004726b0
    if (d < -1.0) d = -1.0;
    if (1.0 < d) d = 1.0;
    double a = std::acos(d) * static_cast<double>(57.29578f);          // FUN_004a3384 * _DAT_005cc98c
    if (n[0] < 0.0f) a = static_cast<double>(360.0f) - a;              // _DAT_005ccac4
    return static_cast<float>(a);
}

// FUN_004704c0: RwMatrixRotate(m, &DAT_006146fc = (0, 1, 0), yaw, 1) on a translation-only matrix, so the car faces
// R_y(yaw) (0, 0, 1) = (sin r, 0, cos r), r = yaw * 0x3c8efa35 rounded to float (the race loop runs at PC_24,
// memory track-load-runs-at-pc24). The port's basis is built from yaw with fwd = (cos, 0, sin).
float GridDir_PortYaw(float lx, float lz) {
    const float deg = GridDir_LawDeg(lx, lz);
    const float r = deg * 0.017453292f;                                // DAT_005cd7a8 = 0x3c8efa35
    const float s = static_cast<float>(std::sin(static_cast<double>(r)));
    const float c = static_cast<float>(std::cos(static_cast<double>(r)));
    const float yaw = static_cast<float>(std::atan2(static_cast<double>(c), static_cast<double>(s)));
    // [lk21] FUN_004c4a50's at row for axis (0,1,0): at.x = sin r, at.z = 1 - omc with omc = (float)(1 - cos r)
    // (_DAT_005cc320 - fcos, 0x004c4d20). Used by BodyOrient_Init under MASHED_SPAWN_BASIS_RW only.
    const float omc = static_cast<float>(1.0 - std::cos(static_cast<double>(r)));
    Vehicle::BodyOrient_NoteExactBasis(yaw, s, 1.0f - omc);
    return yaw;
}

namespace {
void GridDirTrace(int node, float lx, float lz, float yaw) {
    static std::FILE* f = nullptr; static bool init = false;
    if (!init) { init = true; if (const char* p = std::getenv("MASHED_GRID_DIR_TRACE")) f = std::fopen(p, "w"); }
    if (!f) return;
    std::uint32_t bx, bz; std::memcpy(&bx, &lx, 4); std::memcpy(&bz, &lz, 4);
    std::fprintf(f, "griddir node=%d L=(%.9g,%.9g) bits=%08x/%08x law_deg=%.9g port_yaw=%.9g\n", node, lx, lz,
                 bx, bz, GridDir_LawDeg(lx, lz), yaw);
    std::fflush(f);
}
}  // namespace

bool GridDir_YawAt(int node, float* yawPort) {
    const int count = I32(kNodeCount);
    if (count <= 0 || node < 0 || node >= count) return false;
    const float* d = FP(kRec + static_cast<std::uintptr_t>(node) * 0x4cu);   // FUN_00426cc0(node)
    *yawPort = GridDir_PortYaw(d[0], d[2]);
    GridDirTrace(node, d[0], d[2], *yawPort);
    return true;
}

bool GridDir_Anchor(float A[3], float lat[2], float dir[2], float* yawPort, int* node) {
    if (!GridDir_Enabled()) return false;
    std::int32_t v = 0;                                                // DAT_0063ba80 at race init (0x0040fef0)
    SnapNodeToSafeStartLines(&v);                                      // FUN_00409290
    if (node) *node = v;
    if (!RoundGrid_AnchorAt(v, A, lat, dir)) return false;
    return GridDir_YawAt(v, yawPort);
}

bool RoundGrid_OrderEnabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_ROUND_GRID_ORDER");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && RoundGrid_Enabled();
}

// FUN_0040e590, DAT_008a94d0 == 4 path (decomp verify/d3_bulk_20261011/roundgrid/stageb/FUN_0040e590_decomp.txt):
//   C  switch case 4: code 2 -> 3, 1 -> 2, -1 -> 1, -2 -> 0, else 1000 (unassigned); local_44 = 1 iff none assigned
//   D  mode-4 fill: each car whose code is not 2/1/-1/-2 takes the first value 0..3 no car holds now
//   E  mode-4 de-dup: while >= 2 hold 0, the first -> 1; then 1 -> 2; then 2 -> 3; if >= 2 hold 3, the first -> 0, repeat
//   placement (rule 0): value = local_44 ? active index + 4 : local_2c[i]
// Mirror checked 1296/1296 against the original bytes (re/tools/slotorder_unicorn.py).
void RoundGrid_AssignMode4(const int c[4], int out[4]) {
    int s[4];
    bool any = false;
    for (int i = 0; i < 4; ++i) {
        s[i] = c[i] == 2 ? 3 : c[i] == 1 ? 2 : c[i] == -1 ? 1 : c[i] == -2 ? 0 : 1000;
        if (s[i] != 1000) any = true;
    }
    auto inS = [&](int v) { for (int k = 0; k < 4; ++k) if (s[k] == v) return true; return false; };
    auto cnt = [&](int v) { int n = 0; for (int k = 0; k < 4; ++k) if (s[k] == v) ++n; return n; };
    auto first = [&](int v) { for (int k = 0; k < 4; ++k) if (s[k] == v) return k; return -1; };
    for (int i = 0; i < 4; ++i) {
        if (c[i] == 2 || c[i] == 1 || c[i] == -1 || c[i] == -2) continue;
        for (int v = 0; v < 4; ++v) if (!inS(v)) { s[i] = v; break; }
    }
    for (;;) {
        for (int a = 0; a < 3; ++a) while (cnt(a) >= 2) s[first(a)] = a + 1;
        if (cnt(3) < 2) break;
        s[first(3)] = 0;
    }
    for (int i = 0; i < 4; ++i) out[i] = any ? s[i] : i + 4;
}

bool RoundGrid_SlotOrder(int out[4]) {
    if (!RoundGrid_OrderEnabled()) return false;
    RoundGrid_AssignMode4(s_codes, out);
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "round-order codes=%d,%d,%d,%d slots=%d,%d,%d,%d\n", s_codes[0], s_codes[1], s_codes[2], s_codes[3],
                     out[0], out[1], out[2], out[3]);
        std::fflush(f);
    }
    for (int i = 0; i < 4; ++i) s_codes[i] = -1000;                 // FUN_0040b250 (0x0040b266) before placement
    return true;
}

// ---- G-RG-ORDER replay: MASHED_ROUND_GRID_ORDER_REPLAY=<slotorder tsv> (c0..c3 s0..s3), report to
// MASHED_ROUND_GRID_ORDER_REPLAY_OUT. ----
namespace {
void OrderReplay() {
    const char* in = std::getenv("MASHED_ROUND_GRID_ORDER_REPLAY");
    const char* op = std::getenv("MASHED_ROUND_GRID_ORDER_REPLAY_OUT");
    if (!in || !op) return;
    std::FILE* f = std::fopen(in, "r"); std::FILE* o = std::fopen(op, "w");
    if (!f || !o) { if (f) std::fclose(f); if (o) std::fclose(o); return; }
    long rows = 0, exact = 0, bad = 0;
    char line[256];
    while (std::fgets(line, sizeof line, f)) {
        if (line[0] == '#') continue;
        int c[4], e[4], g[4];
        if (std::sscanf(line, "%d %d %d %d %d %d %d %d", &c[0], &c[1], &c[2], &c[3], &e[0], &e[1], &e[2], &e[3]) != 8) continue;
        RoundGrid_AssignMode4(c, g);
        ++rows;
        if (g[0] == e[0] && g[1] == e[1] && g[2] == e[2] && g[3] == e[3]) ++exact;
        else if (++bad <= 20) std::fprintf(o, "MISMATCH codes=%d,%d,%d,%d got=%d,%d,%d,%d exp=%d,%d,%d,%d\n", c[0], c[1], c[2], c[3],
                                          g[0], g[1], g[2], g[3], e[0], e[1], e[2], e[3]);
    }
    std::fprintf(o, "G-RG-ORDER rows=%ld exact=%ld bad=%ld\n", rows, exact, bad);
    std::fclose(f); std::fclose(o);
}
}  // namespace

// ---- G-RG-SNAP replay. Row: n  node_in  count  table(34 x u16 lo, u16 hi as 136 B hex)  node_out ----
void RoundGrid_MaybeReplay() {
    OrderReplay();
    const char* in = std::getenv("MASHED_ROUND_GRID_REPLAY");
    if (!in) return;
    const char* op = std::getenv("MASHED_ROUND_GRID_REPLAY_OUT");
    std::FILE* o = op ? std::fopen(op, "w") : nullptr;
    std::FILE* f = std::fopen(in, "r");
    if (!o || !f) { if (o) std::fclose(o); if (f) std::fclose(f); return; }
    unsigned char saveTab[136]; std::int32_t saveCount = I32(0x0066d6e0u);
    std::memcpy(saveTab, reinterpret_cast<void*>(0x0066d658u), 136);
    long rows = 0, exact = 0, bad = 0, parse = 0;
    char line[1024];
    while (std::fgets(line, sizeof line, f)) {
        if (line[0] == '#') continue;
        long n = 0; int nodeIn = 0, cnt = 0, outExp = 0; char hex[300] = {0};
        if (std::sscanf(line, "%ld %d %d %290s %d", &n, &nodeIn, &cnt, hex, &outExp) != 5 || std::strlen(hex) != 272) { ++parse; continue; }
        unsigned char tab[136];
        for (int i = 0; i < 136; ++i) { unsigned b = 0; std::sscanf(hex + 2 * i, "%2x", &b); tab[i] = static_cast<unsigned char>(b); }
        std::memcpy(reinterpret_cast<void*>(0x0066d658u), tab, 136);
        I32(0x0066d6e0u) = cnt;
        std::int32_t v = nodeIn;
        SnapNodeToSafeStartLines(&v);
        ++rows;
        if (v == outExp) ++exact;
        else if (++bad <= 20) std::fprintf(o, "MISMATCH n=%ld in=%d count=%d got=%d exp=%d\n", n, nodeIn, cnt, v, outExp);
    }
    std::memcpy(reinterpret_cast<void*>(0x0066d658u), saveTab, 136);
    I32(0x0066d6e0u) = saveCount;
    std::fprintf(o, "G-RG-SNAP rows=%ld exact=%ld bad=%ld parse_bad=%ld\n", rows, exact, bad, parse);
    std::fclose(o); std::fclose(f);
}

}  // namespace Race
}  // namespace mashed_re
