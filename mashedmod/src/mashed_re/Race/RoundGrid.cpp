// Mashed RE — round-restart grid, Stage A. See RoundGrid.h and verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md.
//
// FUN_00409290 is verbatim naked x86 (re/tools/naked_specs/roundgrid_a.json -> RoundGrid_naked.inc); its callees
// FUN_00426bc0 / FUN_00426bd0 bind to the exe's C3 exports GetDat0066d6e0 / GetTableEntry0066d658, which read the
// Safe_Start_Lines table PathNodes.cpp builds at 0x0066d658 / 0x0066d6e0 (PATHPROG P1).
#include "RoundGrid.h"
#include "PathNodes.h"
#include "../Math/RwLutTables.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" std::uint32_t __cdecl GetDat0066d6e0();                                                  // 0x00426bc0
extern "C" void __cdecl GetTableEntry0066d658(int i, std::uint16_t* lo, std::uint16_t* hi);        // 0x00426bd0

#include "RoundGrid_naked.inc"

namespace mashed_re {
namespace Race {

namespace {
constexpr std::uintptr_t kNode = 0x0063ba80u;          // DAT_0063ba80
constexpr std::uintptr_t kRec  = 0x00663658u;          // node records, stride 0x4c
constexpr std::uintptr_t kNodeCount = 0x0066d6d8u;
inline std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
inline const float*  FP(std::uintptr_t a)  { return reinterpret_cast<const float*>(a); }
typedef void (__cdecl* snap_fn)(std::int32_t* p);
inline snap_fn Snap() { return reinterpret_cast<snap_fn>(&RG2_409290); }

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
    return s_on && PathProg_Enabled();
}

void RoundGrid_RaceInit() {
    if (!RoundGrid_Enabled()) return;
    I32(kNode) = 0;                                                    // 0x0040fef0 mov [0x0063ba80], ebx (0)
    if (std::FILE* f = Trace()) { std::fprintf(f, "race-init node=0\n"); std::fflush(f); }
}

void RoundGrid_RoundEnd(const bool alive[4], int exeWinner) {
    if (!RoundGrid_Enabled()) return;
    // FUN_00410860 0x00410890..0x004108b3: the LAST slot i in 0..3 that is active and whose FUN_0046c7b0 == 1.
    int surv = 0;                                                      // ebx starts 0 (0x00410880)
    for (int i = 0; i < 4; ++i) if (alive[i]) surv = i;
    const float prog = *FP(0x008a96e8u + static_cast<std::uintptr_t>(surv) * 0x30cu);   // FUN_00408a50
    I32(kNode) = static_cast<std::int32_t>(prog);                      // __ftol (truncate) -> 0x004108c5
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "round-end alive=%d%d%d%d survivor(orig rule)=%d exe_winner=%d prog=%.6g node=%d\n",
                     alive[0], alive[1], alive[2], alive[3], surv, exeWinner, prog, I32(kNode));
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

bool RoundGrid_Anchor(float A[3], float lat[2], float dir[2], int* node) {
    if (!RoundGrid_Enabled()) return false;
    std::int32_t v = I32(kNode);
    Snap()(&v);                                                        // FUN_00409290(&DAT_0063ba80)
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

// ---- G-RG-SNAP replay. Row: n  node_in  count  table(34 x u16 lo, u16 hi as 136 B hex)  node_out ----
void RoundGrid_MaybeReplay() {
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
        Snap()(&v);
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
