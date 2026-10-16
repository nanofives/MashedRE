// Mashed RE — D-11072 leg P2: the original's per-frame path progress (FUN_00408610).
//
// FUN_00408610(rec, world, v) is the only writer of the per-car block 0x008a9620 + v*0x30c,
// whose +0xc8 (0x008a96e8) is the node progress FUN_00414a70 reads through FUN_00408a50, and
// whose +0xcc (0x008a96ec) is the race_pct that MASHED_RACEPCT_BRIDGE approximates.
//
// The step and its 17 callees are VERBATIM naked transcriptions generated from the anchored
// bytes by re/tools/gen_naked_asm.py (spec re/tools/naked_specs/pathprog_p2.json ->
// PathProgStep_naked.inc). Only these are C++ here, each with the original's stack contract:
//   PP_StateGet     FUN_0046cbb0 (vehicle state words) -> the exe-rebound VehicleCarStateRead
//   PP_Ftol2        FUN_004a2c48 (_ftol2: truncate ST0 into EDX:EAX, touches only EAX/EDX)
//   PP_SphereQuery  FUN_00538c80 case 3 -> FUN_00539ec0 with callback FUN_00407a90, as a walk
//                   over the AI BSP triangles in stream order (see the note at the function)
//   PP_Nop / PP_ErrPass  wprintf (0x004a2cbd) and the RW error reporter (0x004d7ff0/0x004d8480)
// The .rdata constants the bytes read are exe-safe copies (the standalone's .rdata reads 0), and
// the RW sqrt LUT base of FUN_004c39b0 / FUN_004c3b90 points at Math/RwLutTables.cpp's tables.
#include "PathProgStep.h"
#include "PathNodes.h"
#include "../Math/RwLutTables.h"

#include <float.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" int          __cdecl VehicleCarStateRead(std::uint32_t carIdx, std::uint32_t* outState,
                                                    std::uint32_t* outSecondary);   // 0x0046cbb0 (exe-rebound)
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);         // 0x0046c7b0
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx);   // 0x0046d4a0
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                        // 0x0040e370

// ---- exe-safe .rdata copies (values read from the anchored image) -------------------------
extern "C" {
const std::uint32_t g_pp_c_5cc320 = 0x3f800000u;   // 1.0
const std::uint32_t g_pp_c_5cc568 = 0x42c80000u;   // 100.0
const std::uint32_t g_pp_c_5cc94c = 0x4f800000u;   // 4294967296.0
const std::uint32_t g_pp_c_5cca40 = 0xc1200000u;   // -10.0
const std::uint32_t g_pp_c_5cca54 = 0x42c60000u;   // 99.0
const std::uint32_t g_pp_c_5cca58 = 0x4a371b00u;   // 3000000.0
const std::uint32_t g_pp_c_5d757c = 0x00000000u;   // 0.0
// [*(0x007d3ff8) + *(0x007d3ffc)] = sqrt table root, [.. + 4] = inv-sqrt table root
struct { const std::uint32_t* sqrt_; const std::uint32_t* inv_; } g_pp_lutptrs = { nullptr, nullptr };
}

namespace {
bool          s_replay = false;
std::int32_t  s_replaySt0 = 0, s_replaySt1 = 0;
}

extern "C" {

int __cdecl PP_StateGet(std::uint32_t v, std::int32_t* a, std::int32_t* b) {
    if (v > 0xfu) return 0;                                            // 0x0046cbb0 bound
    if (s_replay) { *a = s_replaySt0; *b = s_replaySt1; return 1; }
    return VehicleCarStateRead(v, reinterpret_cast<std::uint32_t*>(a),
                               reinterpret_cast<std::uint32_t*>(b));
}

__declspec(naked) void PP_Ftol2() {
    __asm {
        sub    esp, 12
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        or     ax, 0x0c00                       // round toward zero
        mov    word ptr [esp + 2], ax
        fldcw  word ptr [esp + 2]
        fistp  qword ptr [esp + 4]
        fldcw  word ptr [esp]
        mov    eax, dword ptr [esp + 4]
        mov    edx, dword ptr [esp + 8]
        add    esp, 12
        ret
    }
}
__declspec(naked) void PP_Nop()     { __asm { ret } }
__declspec(naked) void PP_ErrPass() { __asm { xor eax, eax
                                              ret } }

void PP_547450();   // generated below (sphere vs triangle)

// FUN_00538c80(world, sphere{x,y,z,r,..,..,type=3}, FUN_00407a90, &{v, world}) -> FUN_00539ec0.
// The original visits sectors left-first (= stream order) and, inside a sector, triangles in
// its collision-tree order; FUN_00407a90 keeps the LAST hit. The node id of every triangle of
// one gate is the same, so the order only matters when two different gates touch the r=0.6
// sphere in one frame [UNCERTAIN U-9209] (within-sector tree order not reproduced; harmless on o_pp1).
void* __cdecl PP_SphereQuery(void* world, float* sphere, void* /*cb*/, std::int32_t* data) {
    const std::int32_t v = data[0];
    typedef int (__cdecl* tri_fn)(float*, const float*, const float*, const float*, float*, float*);
    const tri_fn test = reinterpret_cast<tri_fn>(&PP_547450);
    const int n = mashed_re::Race::PathNodes_TriCount();
    for (int t = 0; t < n; ++t) {
        const float* tv = mashed_re::Race::PathNodes_Tri(t);
        float nrm[3], dist;
        if (test(sphere, tv, tv + 3, tv + 6, nrm, &dist) == 0) continue;
        // FUN_00407a90: node id = ushort at RpMaterial+4 (colour red | green << 8), normal.
        const std::uintptr_t blk = 0x008a9620u + static_cast<std::uintptr_t>(v) * 0x30cu;
        *reinterpret_cast<std::uint32_t*>(blk + 0x20) = mashed_re::Race::PathNodes_TriNode(t);
        *reinterpret_cast<float*>(blk + 0xbc) = nrm[0];
        *reinterpret_cast<float*>(blk + 0xc0) = nrm[1];
        *reinterpret_cast<float*>(blk + 0xc4) = nrm[2];
    }
    return world;
}

}  // extern "C"

#include "PathProgStep_naked.inc"

namespace mashed_re {
namespace Race {

namespace {

typedef void (__cdecl* step_fn)(void* rec, void* world, std::int32_t v);
inline step_fn Step() { return reinterpret_cast<step_fn>(&PP_408610); }

void EnsureLut() {
    if (!g_pp_lutptrs.sqrt_) {
        g_pp_lutptrs.sqrt_ = Math::RwLutSqrtTable();
        g_pp_lutptrs.inv_  = Math::RwLutInvSqrtTable();
    }
}

// x87 precision control for the step. MASHED_PATHPROG_PC = 24 | 53 | 64; default 24.
unsigned int PcBits() {
    const char* s = std::getenv("MASHED_PATHPROG_PC");
    if (s && std::strcmp(s, "53") == 0) return _PC_53;
    if (s && std::strcmp(s, "64") == 0) return _PC_64;
    return _PC_24;
}

struct PcScope {
    unsigned int old = 0;
    // _controlfp_s returns the control word AFTER the change in its first argument, so the old
    // value must be read first (mask 0). Before 2026-10-10 this read `old` from the setting call
    // and the destructor re-applied PC_24, leaving the game thread at PC_24 for the rest of the
    // run (verify/d3_mode7_20261010/cw_check.txt).
    PcScope() { _controlfp_s(&old, 0, 0); unsigned int t; _controlfp_s(&t, PcBits(), _MCW_PC); }
    ~PcScope() { unsigned int t; _controlfp_s(&t, old & _MCW_PC, _MCW_PC); }
};

bool FromHex(const std::string& h, std::uint8_t* out, std::size_t n) {
    if (h.size() != n * 2) return false;
    for (std::size_t i = 0; i < n; ++i) {
        unsigned b = 0;
        if (std::sscanf(h.c_str() + i * 2, "%2x", &b) != 1) return false;
        out[i] = static_cast<std::uint8_t>(b);
    }
    return true;
}

}  // namespace

namespace {
// [U-9685] Arm R: the step's persistent per-car state (it reads its own previous row every call), kept off
// the real block so only race_pct reaches the game's array.
std::uint8_t s_shadow[4][0xc3u * 4u];
}  // namespace

void PathProg_RaceInit() {                               // FUN_00407a60
    if (RacePctOrig_Enabled() && !PathProg_Enabled()) {   // arm R: initialise the shadow rows the same way
        for (int v = 0; v < 4; ++v) {
            std::memset(s_shadow[v], 0, sizeof s_shadow[v]);
            *reinterpret_cast<std::int32_t*>(s_shadow[v] + 0x28) = -1;
        }
        return;
    }
    if (!PathProg_Enabled()) return;
    for (int v = 0; v < 4; ++v) {
        const std::uintptr_t blk = 0x008a9620u + static_cast<std::uintptr_t>(v) * 0x30cu;
        std::memset(reinterpret_cast<void*>(blk), 0, 0xc3u * 4u);
        *reinterpret_cast<std::int32_t*>(blk + 0x28) = -1;
    }
}

void PathProg_FrameTick() {                              // FUN_0040da50's FUN_00408610 loop
    if (!PathProg_Enabled() || PathNodes_TriCount() == 0) return;
    EnsureLut();
    PcScope pc;
    for (int v = 0; v < 4; ++v) {
        if (IsCarSlotActive(v) == 0) continue;               // *(PTR_005f2770 + 0x34 + v*4)
        // [UNCERTAIN U-9210] FUN_0040da50 also accepts FUN_0046c770(v) == 1; no exe binding yet.
        if (VehicleSlotGetter(static_cast<std::uint32_t>(v)) != 1u) continue;
        std::uint32_t rec = 0;
        PtrCompute881ec8(&rec, static_cast<std::uint32_t>(v));
        Step()(reinterpret_cast<void*>(rec), nullptr, v);   // world: unused by PP_SphereQuery
    }
    // G-P2-LIVE trace: MASHED_PATHPROG_TRACE=<csv> (frame, v, node +0x20, laps +0x28, prog +0xc8, pct +0xcc)
    static std::FILE* s_tr = nullptr; static bool s_trInit = false; static int s_fr = 0;
    if (!s_trInit) {
        s_trInit = true;
        if (const char* t = std::getenv("MASHED_PATHPROG_TRACE")) {
            s_tr = std::fopen(t, "w");
            if (s_tr) std::fprintf(s_tr, "frame,v,node,laps,prog,pct\n");
        }
    }
    if (s_tr) {
        for (int v = 0; v < 4; ++v) {
            const std::uintptr_t blk = 0x008a9620u + static_cast<std::uintptr_t>(v) * 0x30cu;
            std::fprintf(s_tr, "%d,%d,%d,%d,%.6f,%.6f\n", s_fr, v, *reinterpret_cast<const std::int32_t*>(blk + 0x20),
                         *reinterpret_cast<const std::int32_t*>(blk + 0x28), *reinterpret_cast<const float*>(blk + 0xc8),
                         *reinterpret_cast<const float*>(blk + 0xcc));
        }
        if ((s_fr & 63) == 0) std::fflush(s_tr);
    }
    ++s_fr;
}

void PathProg_RacePctTick() {
    if (!RacePctOrig_Enabled() || PathProg_Enabled() || PathNodes_TriCount() == 0) return;
    EnsureLut();
    PcScope pc;
    for (int v = 0; v < 4; ++v) {
        if (IsCarSlotActive(v) == 0) continue;               // the same two predicates as PathProg_FrameTick
        if (VehicleSlotGetter(static_cast<std::uint32_t>(v)) != 1u) continue;
        std::uint32_t rec = 0;
        PtrCompute881ec8(&rec, static_cast<std::uint32_t>(v));
        std::uint8_t* const blk = reinterpret_cast<std::uint8_t*>(0x008a9620u + static_cast<std::uintptr_t>(v) * 0x30cu);
        std::uint8_t saved[sizeof s_shadow[0]];
        std::memcpy(saved, blk, sizeof saved);                // the row the rest of the port sees
        std::memcpy(blk, s_shadow[v], sizeof saved);          // the step's own persistent row
        Step()(reinterpret_cast<void*>(rec), nullptr, v);
        std::memcpy(s_shadow[v], blk, sizeof saved);
        const std::uint32_t pct = *reinterpret_cast<const std::uint32_t*>(blk + 0xcc);   // +0xcc = 0x008a96ec: race_pct
        std::memcpy(blk, saved, sizeof saved);
        *reinterpret_cast<std::uint32_t*>(blk + 0xcc) = pct;  // only race_pct is published
    }
    static bool s_logged = false;
    if (!s_logged) {
        s_logged = true;
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {   // the game's own witness that arm R is live
            std::fprintf(lf, "[U-9685] race_pct from the verbatim step on a shadow row (opt-out MASHED_NO_RACEPCT_FUN408610)\n");
            std::fclose(lf);
        }
    }
}

void PathProg_MaybeReplay() {
    const char* in = std::getenv("MASHED_PATHPROG_REPLAY");
    if (!in || !PathProg_Enabled()) return;
    const char* outp = std::getenv("MASHED_PATHPROG_REPLAY_OUT");
    std::FILE* f = std::fopen(in, "r");
    if (!f) return;
    struct Row { int v; std::int32_t st0, st1; std::uint32_t clk; std::uint8_t rec[0x40]; std::uint8_t blk[0x30c]; };
    std::vector<Row> rows;
    std::vector<char> line(8192);
    std::fgets(line.data(), static_cast<int>(line.size()), f);         // header
    while (std::fgets(line.data(), static_cast<int>(line.size()), f)) {
        std::vector<std::string> c; std::string cur;
        for (const char* p = line.data(); *p && *p != '\n' && *p != '\r'; ++p) {
            if (*p == ',') { c.push_back(cur); cur.clear(); } else cur.push_back(*p);
        }
        c.push_back(cur);
        if (c.size() < 8) continue;
        Row r{};
        r.v = std::atoi(c[2].c_str()); r.st0 = std::atoi(c[3].c_str()); r.st1 = std::atoi(c[4].c_str());
        r.clk = static_cast<std::uint32_t>(std::strtoul(c[5].c_str(), nullptr, 10));
        if (!FromHex(c[6], r.rec, 0x40) || !FromHex(c[7], r.blk, 0x30c)) continue;
        rows.push_back(r);
    }
    std::fclose(f);

    EnsureLut();
    PcScope pc;
    s_replay = true;
    int pairs = 0, exact = 0;
    std::vector<int> firstOff(0x30c / 4, 0), anyOff(0x30c / 4, 0);
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    for (std::size_t k = 0; k < rows.size(); ++k) {
        std::size_t nx = k + 1;
        while (nx < rows.size() && rows[nx].v != rows[k].v) ++nx;
        if (nx >= rows.size()) continue;
        const Row& a = rows[k]; const Row& b = rows[nx];
        const std::uintptr_t blk = 0x008a9620u + static_cast<std::uintptr_t>(a.v) * 0x30cu;
        std::memcpy(reinterpret_cast<void*>(blk), a.blk, 0x30c);
        *reinterpret_cast<std::uint32_t*>(0x007f1010u) = a.clk;
        s_replaySt0 = a.st0; s_replaySt1 = a.st1;
        std::uint8_t rec[0x40]; std::memcpy(rec, a.rec, 0x40);
        Step()(rec, nullptr, a.v);
        ++pairs;
        int first = -1;
        for (int d = 0; d < 0x30c / 4; ++d) {
            if (std::memcmp(reinterpret_cast<const void*>(blk + d * 4), b.blk + d * 4, 4) != 0) {
                ++anyOff[d];
                if (first < 0) first = d;
            }
        }
        if (first < 0) ++exact; else ++firstOff[first];
        if (o && first >= 0 && pairs - exact <= 20)
            std::fprintf(o, "MISMATCH row %zu v=%d first_off=+0x%03x port=%08x orig=%08x\n", k, a.v, first * 4,
                         *reinterpret_cast<const std::uint32_t*>(blk + first * 4),
                         *reinterpret_cast<const std::uint32_t*>(b.blk + first * 4));
    }
    s_replay = false;
    if (o) {
        std::fprintf(o, "G-P2-REPLAY pairs=%d exact=%d mismatched=%d pc=%s\n", pairs, exact, pairs - exact,
                     std::getenv("MASHED_PATHPROG_PC") ? std::getenv("MASHED_PATHPROG_PC") : "24");
        for (int d = 0; d < 0x30c / 4; ++d)
            if (anyOff[d]) std::fprintf(o, "  off +0x%03x: any=%d first=%d\n", d * 4, anyOff[d], firstOff[d]);
        std::fclose(o);
    }
    PathProg_RaceInit();
}

}  // namespace Race
}  // namespace mashed_re
