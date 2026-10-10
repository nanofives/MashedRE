// Mashed RE — D-11072 leg P3: FUN_00414a70 + FUN_00414300, verbatim naked x87 (exe-only TU).
//
// FUN_00414a70(spline, &look, &cand, v) picks the nearest active car AHEAD on the per-car path
// progress 0x008a96e8 (FUN_00408a50, within 2.0 nodes; behind by more than nodecount-5 counts
// as ahead across the line), then
//   * if (1500 < own speed - its speed) || (own speed < 1500)   (0x00414bab / 0x00414bbc, 0x005cd0ac)
//     and FUN_00414300(v, own xz, look, its xz, r = 1.0, cand) != 0  -> return 1 (mode 3)
//   * else if FUN_0040e470(it) != 1 && |own - it|_xz < 1.5 (0x00414c0d, 0x005cc348) -> return 2
//   * else 0.
// FUN_00416250 commits mode 3 (look = cand) on 1 + LOS, and on 2 takes the immediate return at
// 0x004163f6..0x00416405 (ctrl[4] = 0, ctrl[5] = 0xff).
//
// The two bodies and six leaves are generated from the anchored bytes by re/tools/gen_naked_asm.py
// (spec re/tools/naked_specs/target_p3.json -> AiTargetClosest_naked.inc). C++ here, each with the
// original's cdecl stack contract (callee-saved EBX/ESI/EDI/EBP preserved by the compiler):
//   TG_VehPtr  FUN_0046d4a0 -> PtrCompute881ec8 (exe-rebound record array)
//   TG_Speed   FUN_0046d6d0 -> record +0x9e4 (0x00881f84 in the array-base convention)
//   TG_Fwd     FUN_0046d510 -> (0,0,1) by the record's +0x928 matrix = its `at` row. The original
//              also stores the result into record +0x9d4; this shim does NOT (the exe's physics
//              owns +0x9d4). The mismatch count is reported (G-P3-INPUTS).
// The .rdata constants are exe-safe copies; the RW sqrt LUT base of FUN_004c3ac0 / FUN_004c3b30
// points at Math/RwLutTables.cpp. PREREG: verify/d3_modes37_20261009/PREREG_P3.md.
#include "AiTargetClosest.h"
#include "../Race/PathNodes.h"
#include "../Math/RwLutTables.h"
#include "../Vehicle/VehicleRecordBase.h"

#include <float.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx);   // 0x0046d4a0

// ---- exe-safe .rdata copies (values read from the anchored image) -------------------------
extern "C" {
const std::uint32_t g_tg_c_5cc320 = 0x3f800000u;   // 1.0
const std::uint32_t g_tg_c_5cc348 = 0x3fc00000u;   // 1.5
const std::uint32_t g_tg_c_5cc558 = 0x3a83126fu;   // 0.001
const std::uint32_t g_tg_c_5cd0ac = 0x44bb8000u;   // 1500.0
const std::uint32_t g_tg_c_5d757c = 0x00000000u;   // 0.0
// [*(0x007d3ff8) + *(0x007d3ffc)] = sqrt table root
struct { const std::uint32_t* sqrt_; const std::uint32_t* inv_; } g_tg_lutptrs = { nullptr, nullptr };

// Per-call outcome for the step dump (TrackRenderer::AiStepDump, columns ret14a70 / tg3_los,
// emitted only when MASHED_TGT3 is set). -1 = not evaluated on this call.
int g_tg3Ret[4] = {-1, -1, -1, -1};
int g_tg3Los[4] = {-1, -1, -1, -1};
}

namespace {

// Replay context (G-P3-REPLAY): the inputs of one recorded original call.
bool          s_replay = false;
std::uint32_t s_rMat[4][16];       // fake +0x928 matrices: [12] = +0x30 (x), [14] = +0x38 (z)
std::uint32_t s_rSp[4];
std::uint32_t s_rAt[3];            // own `at` row
int           s_rOwn = -1;
// Diagnostic: evaluate TG_Speed as |record+0x9b0| (the unported dispatcher tail 0x004710c8).
bool          s_speedAlt = false;
long          s_atMismatch = 0, s_atCalls = 0;

inline const char* Rec(std::uint32_t idx) { return mashed_re::Vehicle::VehRecord(idx); }
inline std::uint32_t Bits(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }
inline float Flt(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }

}  // namespace

extern "C" {

std::uint32_t __cdecl TG_VehPtr(std::uint32_t* out, std::uint32_t idx) {      // 0x0046d4a0
    if (idx > 0xfu) return 0;
    if (s_replay) {
        *out = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&s_rMat[idx & 3][0]));
        return 1;
    }
    return PtrCompute881ec8(out, idx);
}

std::uint32_t __cdecl TG_Speed(std::uint32_t* out, std::uint32_t idx) {       // 0x0046d6d0
    if (idx > 0xfu) return 0;
    if (s_replay) { *out = s_rSp[idx & 3]; return 1; }
    const char* r = Rec(idx);
    if (!r) { *out = 0; return 1; }
    if (s_speedAlt) {
        const float* vel = reinterpret_cast<const float*>(r + 0x9b0);
        *out = Bits(std::sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]));
        return 1;
    }
    *out = *reinterpret_cast<const std::uint32_t*>(r + 0x9e4);
    return 1;
}

std::uint32_t __cdecl TG_Fwd(std::uint32_t* out, std::uint32_t idx) {         // 0x0046d510
    if (idx > 0xfu) return 0;
    float at[3];
    if (s_replay) {
        for (int k = 0; k < 3; ++k) at[k] = Flt(s_rAt[k]);
    } else {
        const char* r = Rec(idx);
        if (!r) { out[0] = out[1] = out[2] = 0; return 1; }
        const float* m = reinterpret_cast<const float*>(r + mashed_re::Vehicle::kOff_WheelMatrices);
        for (int k = 0; k < 3; ++k) at[k] = m[8 + k];                           // +0x20 row
        ++s_atCalls;
        if (std::memcmp(at, r + 0x9d4, 12) != 0) ++s_atMismatch;
    }
    // FUN_004c3df0(dst, &(0,0,1) 0x00614708, 1, matrix): 0*right + 0*up + 1*at, i.e. `at`.
    for (int k = 0; k < 3; ++k) out[k] = Bits(at[k]);
    return 1;
}

}  // extern "C"

#include "AiTargetClosest_naked.inc"

namespace Ai {

namespace {

typedef int (__cdecl* body_fn)(std::uint32_t spline, const float* look, float* cand, int v);
inline body_fn Body() { return reinterpret_cast<body_fn>(&TG_414a70); }

void EnsureLut() {
    if (!g_tg_lutptrs.sqrt_) {
        g_tg_lutptrs.sqrt_ = mashed_re::Math::RwLutSqrtTable();
        g_tg_lutptrs.inv_  = mashed_re::Math::RwLutInvSqrtTable();
    }
}

// x87 precision control. MASHED_TGT3_PC = 24 | 53 | 64; default 24 (the race loop's, RESULT_P2 §1).
unsigned int PcBits() {
    const char* s = std::getenv("MASHED_TGT3_PC");
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

std::FILE* Trace() {
    static std::FILE* f = nullptr; static bool init = false;
    if (!init) {
        init = true;
        if (const char* p = std::getenv("MASHED_TGT3_TRACE")) {
            f = std::fopen(p, "w");
            if (f) std::fprintf(f, "n,v,ret,cand_x,cand_z,ret_alt,prog,d6d8,sp,sp_now,at_mis,at_calls\n");
        }
    }
    return f;
}

}  // namespace

bool TargetClosest_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_TGT3");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && mashed_re::Race::PathProg_Enabled();
}

int TargetClosest(int v, const float look[2], float cand[2]) {
    if (v < 0 || v > 3 || !Rec(0)) return 0;
    EnsureLut();
    int ret;
    {
        PcScope pc;
        ret = Body()(0u, look, cand, v);
    }
    if (std::FILE* f = Trace()) {
        // G-P3-SPEEDTAIL: the same call with the speed read as |+0x9b0| (scratch cand).
        float scratch[2] = { cand[0], cand[1] };
        int retAlt;
        {
            PcScope pc;
            s_speedAlt = true;
            retAlt = Body()(0u, look, scratch, v);
            s_speedAlt = false;
        }
        static long n = 0;
        const char* r = Rec(static_cast<std::uint32_t>(v));
        const float* vel = reinterpret_cast<const float*>(r + 0x9b0);
        std::fprintf(f, "%ld,%d,%d,%.9g,%.9g,%d,%.9g,%d,%.9g,%.9g,%ld,%ld\n", n++, v, ret,
                     cand[0], cand[1], retAlt,
                     *reinterpret_cast<const float*>(0x008a96e8u + static_cast<std::uintptr_t>(v) * 0x30cu),
                     *reinterpret_cast<const std::int32_t*>(0x0066d6d8u),
                     *reinterpret_cast<const float*>(r + 0x9e4),
                     std::sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]),
                     s_atMismatch, s_atCalls);
        if ((n & 63) == 0) std::fflush(f);
    }
    return ret;
}

void TargetClosest_MaybeReplay() {
    const char* in = std::getenv("MASHED_TGT3_REPLAY");
    if (!in) return;
    const char* outp = std::getenv("MASHED_TGT3_REPLAY_OUT");
    std::FILE* f = std::fopen(in, "r");
    if (!f) return;
    std::vector<std::vector<long long>> rows;
    std::vector<char> line(4096);
    std::fgets(line.data(), static_cast<int>(line.size()), f);              // header
    while (std::fgets(line.data(), static_cast<int>(line.size()), f)) {
        std::vector<long long> c;
        const char* p = line.data();
        while (*p && *p != '\n' && *p != '\r') {
            char* e = nullptr;
            c.push_back(std::strtoll(p, &e, 10));
            p = e;
            if (*p == ',') ++p; else break;
        }
        if (c.size() == 34) rows.push_back(c);
    }
    std::fclose(f);

    // Columns (scenario_launch.py --tgt-probe): 0 n, 1 frame, 2 v, 3 d6d8, 4 look_x, 5 look_z,
    // 6 + 5*i: x_i, z_i, sp_i, pr_i, st_i (i = 0..3), 26..28 at, 29..30 cand in, 31 ret, 32..33 cand out.
    EnsureLut();
    // *(0x005f2770) is still null at course load: point it at a local slot-state table for the
    // replay (FUN_0040e370 / FUN_0040e470 read it verbatim) and restore it afterwards.
    std::int32_t** const ptrSlot = reinterpret_cast<std::int32_t**>(0x005f2770u);
    std::int32_t* const svPtr = *ptrSlot;
    static std::int32_t s_localSt[0x34 / 4 + 4] = {};
    if (!svPtr) *ptrSlot = s_localSt;
    std::int32_t* stTab = *ptrSlot;
    std::int32_t  svSt[4] = {0, 0, 0, 0};
    std::uint32_t svPr[4];
    const std::int32_t svD6 = *reinterpret_cast<std::int32_t*>(0x0066d6d8u);
    for (int i = 0; i < 4; ++i) {
        svPr[i] = *reinterpret_cast<std::uint32_t*>(0x008a96e8u + i * 0x30cu);
        if (stTab) svSt[i] = stTab[(0x34 / 4) + i];
    }
    int pairs = 0, exact = 0, retMis = 0, candMis = 0, touched = 0;
    int byRet[3] = {0, 0, 0};
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    {
        PcScope pc;
        s_replay = true;
        for (const auto& c : rows) {
            const int v = static_cast<int>(c[2]);
            if (v < 0 || v > 3 || !stTab) continue;
            *reinterpret_cast<std::int32_t*>(0x0066d6d8u) = static_cast<std::int32_t>(c[3]);
            for (int i = 0; i < 4; ++i) {
                std::memset(s_rMat[i], 0, sizeof(s_rMat[i]));
                s_rMat[i][12] = static_cast<std::uint32_t>(c[6 + 5 * i]);
                s_rMat[i][14] = static_cast<std::uint32_t>(c[7 + 5 * i]);
                s_rSp[i] = static_cast<std::uint32_t>(c[8 + 5 * i]);
                *reinterpret_cast<std::uint32_t*>(0x008a96e8u + i * 0x30cu) = static_cast<std::uint32_t>(c[9 + 5 * i]);
                stTab[(0x34 / 4) + i] = static_cast<std::int32_t>(c[10 + 5 * i]);
            }
            for (int k = 0; k < 3; ++k) s_rAt[k] = static_cast<std::uint32_t>(c[26 + k]);
            s_rOwn = v;
            std::uint32_t look[2] = { static_cast<std::uint32_t>(c[4]), static_cast<std::uint32_t>(c[5]) };
            std::uint32_t cand[2] = { static_cast<std::uint32_t>(c[29]), static_cast<std::uint32_t>(c[30]) };
            const int ret = Body()(0u, reinterpret_cast<const float*>(look), reinterpret_cast<float*>(cand), v);
            ++pairs;
            const int oret = static_cast<int>(c[31]);
            if (oret >= 0 && oret <= 2) ++byRet[oret];
            const bool rOk = (ret == oret);
            const bool cOk = cand[0] == static_cast<std::uint32_t>(c[32]) && cand[1] == static_cast<std::uint32_t>(c[33]);
            if (ret != 1 && (cand[0] != static_cast<std::uint32_t>(c[29]) || cand[1] != static_cast<std::uint32_t>(c[30]))) ++touched;
            if (!rOk) ++retMis;
            if (!cOk) ++candMis;
            if (rOk && cOk) ++exact;
            else if (o && pairs - exact <= 30)
                std::fprintf(o, "MISMATCH n=%lld frame=%lld v=%d ret port=%d orig=%d cand port=%08x,%08x orig=%08x,%08x\n",
                             c[0], c[1], v, ret, oret, cand[0], cand[1],
                             static_cast<std::uint32_t>(c[32]), static_cast<std::uint32_t>(c[33]));
        }
        s_replay = false;
    }
    *reinterpret_cast<std::int32_t*>(0x0066d6d8u) = svD6;
    if (!svPtr) { *ptrSlot = nullptr; stTab = nullptr; }
    for (int i = 0; i < 4; ++i) {
        *reinterpret_cast<std::uint32_t*>(0x008a96e8u + i * 0x30cu) = svPr[i];
        if (stTab) stTab[(0x34 / 4) + i] = svSt[i];
    }
    if (o) {
        std::fprintf(o, "G-P3-REPLAY pairs=%d exact=%d ret_mismatch=%d cand_mismatch=%d cand_touched_on_ret!=1=%d "
                        "orig_ret0=%d orig_ret1=%d orig_ret2=%d pc=%s rows_read=%zu\n",
                     pairs, exact, retMis, candMis, touched, byRet[0], byRet[1], byRet[2],
                     std::getenv("MASHED_TGT3_PC") ? std::getenv("MASHED_TGT3_PC") : "24", rows.size());
        std::fclose(o);
    }
}

}  // namespace Ai
