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
#include "../Gameplay/PickupTick.h"   // [mode 7 M4] PickupCallbacks_Enabled
#include <map>

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

// ---- mode 7 leg M4: FUN_00414c30 + FUN_00414490 (PREREG verify/d3_mode7_m4_20261010/PREREG_M4.md) ----
// Verbatim naked (spec re/tools/naked_specs/mode7_m4.json -> AiModeSeven_naked.inc): FUN_00414c30, FUN_00414490,
// FUN_00484c70, FUN_00442cc0, FUN_004c39b0 (LUT patched). Bindings, cdecl:
//   TG_SubMode      FUN_0040e350 -> the exe host (aib_game_sub_mode)
//   TG_HeldWord     the held-powerup read at 0x004144b3 ([0x0088fc88 + v*0xb4]) -> the exe's PowerupSystem
//   TG_414300_m4    FUN_00414300 -> P3's verbatim TG_414300 (replay: the captured return / cand, arguments checked)
//   TG_414490_w     FUN_00414490 -> TG_414490, return recorded
//   TG_Stub41f030 / TG_Stub48a630  the type-0x15 frustum arm (absent on Training): counted, return 0
//   TG_ErrPass / TG_Nop             FUN_004c39b0's RW error funnel
extern "C" {
const std::uint32_t g_m7_c_5cc32c = 0x3f000000u;   // 0.5
const std::uint32_t g_m7_c_5cc950 = 0x3f400000u;   // 0.75
const std::uint32_t g_m7_c_5cc9f4 = 0x41000000u;   // 8.0
const std::uint32_t g_m7_c_5ccac0 = 0x3ecccccdu;   // 0.4
const std::uint32_t g_m7_c_5ccac8 = 0x3eaaaaabu;   // 0.333333343
const std::uint32_t g_m7_c_5cd090 = 0x3f5db22du;   // 0.866
const std::uint32_t g_m7_c_5cd0b0 = 0xbf400000u;   // -0.75
const std::uint32_t g_m7_c_5cd0b4 = 0x3f600000u;   // 0.875
}

namespace {
int  (*s_m7SubMode)() = nullptr;
int  (*s_m7Held)(int) = nullptr;
bool s_m7Replay = false;
int  s_m7RpBa8c = 6;
std::uint32_t s_m7RpHeld = 0;
std::vector<std::string> s_m7Sub;          // nested calls of the current FUN_00414c30 call
std::vector<std::string> s_m7RpSub;        // replay: the captured FUN_00414300 entries, in order
std::size_t s_m7RpSubPos = 0;
long s_m7Stub = 0, s_m7SubBad = 0;
std::string HexB(const void* p, std::size_t n) {
    static const char* d = "0123456789abcdef";
    std::string s(n * 2, '0');
    const unsigned char* b = static_cast<const unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i) { s[2 * i] = d[b[i] >> 4]; s[2 * i + 1] = d[b[i] & 15]; }
    return s;
}
bool FromHexB(const std::string& h, void* out, std::size_t n) {
    if (h.size() != n * 2) return false;
    unsigned char* o = static_cast<unsigned char*>(out);
    for (std::size_t i = 0; i < n; ++i) {
        unsigned b = 0;
        if (std::sscanf(h.c_str() + i * 2, "%2x", &b) != 1) return false;
        o[i] = static_cast<unsigned char>(b);
    }
    return true;
}
std::vector<std::string> SplitB(const std::string& s, char d) {
    std::vector<std::string> o; std::size_t p = 0;
    while (true) { std::size_t q = s.find(d, p); o.push_back(s.substr(p, q == std::string::npos ? std::string::npos : q - p));
                   if (q == std::string::npos) break; p = q + 1; }
    return o;
}
}  // namespace

extern "C" {

void TG_414490();                                                                 // AiModeSeven_naked.inc
int __cdecl TG_SubMode() {                                                       // FUN_0040e350
    if (s_m7Replay) return s_m7RpBa8c;
    return s_m7SubMode ? s_m7SubMode() : 6;
}
std::uint32_t __cdecl TG_HeldWord(int v) {                                       // [0x0088fc88 + v*0xb4]
    if (s_m7Replay) return s_m7RpHeld;
    return s_m7Held ? static_cast<std::uint32_t>(s_m7Held(v)) : 0u;
}
int __cdecl TG_Stub41f030(int, void*) { ++s_m7Stub; return 0; }                 // FUN_0041f030 (type 0x15)
int __cdecl TG_Stub48a630(void*, void*) { ++s_m7Stub; return 0; }               // FUN_0048a630 (type 0x15)
__declspec(naked) void TG_Nop()     { __asm { ret } }
__declspec(naked) void TG_ErrPass() { __asm { xor eax, eax
                                              ret } }

int __cdecl TG_414300_m4(int v, const float* own, const float* look, const float* tgt, std::uint32_t r, float* cand) {
    // the probe's string: 300:v:own(8B):look(8B):tgt(8B):r:ret:cand(8B)
    char head[160];
    std::snprintf(head, sizeof head, "300:%d:%s:%s:%s:%x", v, HexB(own, 8).c_str(), HexB(look, 8).c_str(),
                  HexB(tgt, 8).c_str(), r);
    int ret;
    if (s_m7Replay) {
        ret = 0;
        if (s_m7RpSubPos < s_m7RpSub.size()) {
            const std::vector<std::string> c = SplitB(s_m7RpSub[s_m7RpSubPos++], ':');   // 300 v own look tgt r ret cand
            if (c.size() == 8) {
                const std::string exp = c[0] + ":" + c[1] + ":" + c[2] + ":" + c[3] + ":" + c[4] + ":" + c[5];
                if (exp != head) ++s_m7SubBad;
                ret = std::atoi(c[6].c_str());
                FromHexB(c[7], cand, 8);
            } else ++s_m7SubBad;
        } else ++s_m7SubBad;
    } else {
        typedef int (__cdecl* fn_t)(int, const float*, const float*, const float*, std::uint32_t, float*);
        ret = reinterpret_cast<fn_t>(&TG_414300)(v, own, look, tgt, r, cand);
    }
    s_m7Sub.push_back(std::string(head) + ":" + std::to_string(ret) + ":" + HexB(cand, 8));
    return ret;
}
int __cdecl TG_414490_w(int v, float* own, void* look, float* tgt, std::uint32_t r, float* cand) {
    typedef int (__cdecl* fn_t)(int, float*, void*, float*, std::uint32_t, float*);
    const int ret = reinterpret_cast<fn_t>(&TG_414490)(v, own, look, tgt, r, cand);
    s_m7Sub.push_back("490:" + std::to_string(ret));
    return ret;
}

}  // extern "C"

#include "AiModeSeven_naked.inc"

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


// ======================================================================================
// mode 7 leg M4: FUN_00414c30 (PREREG verify/d3_mode7_m4_20261010/PREREG_M4.md)
// ======================================================================================
namespace {
typedef int (__cdecl* c30_fn)(std::uint32_t spline, const float* look, float* cand, int v);
inline c30_fn Body7() { return reinterpret_cast<c30_fn>(&TG_414c30); }
// x87 precision: MASHED_MODE7_PC = 24 | 53 | 64; default 24 (the race loop's).
struct PcScope7 {
    unsigned int old = 0;
    PcScope7() {
        _controlfp_s(&old, 0, 0);                       // read first (U-9216)
        const char* e = std::getenv("MASHED_MODE7_PC");
        unsigned int b = _PC_24;
        if (e && std::strcmp(e, "53") == 0) b = _PC_53;
        if (e && std::strcmp(e, "64") == 0) b = _PC_64;
        unsigned int t; _controlfp_s(&t, b, _MCW_PC);
    }
    ~PcScope7() { unsigned int t; _controlfp_s(&t, old & _MCW_PC, _MCW_PC); }
};
inline std::int32_t& I32m(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
constexpr std::uintptr_t kRgSlots = 0x006dccb8u, kRgStride = 0x8cu, kRgCC = 0x006e70ccu, kRgD8 = 0x006e70d8u;
std::FILE* M7Trace() {
    static std::FILE* f = nullptr; static bool init = false;
    if (!init) {
        init = true;
        if (const char* p = std::getenv("MASHED_MODE7_TRACE")) {
            f = std::fopen(p, "w");
            if (f) std::fprintf(f, "n\tframe\tv\tcc\td8\tba8c\theld\trefdist\tmat\tmat0\tlook\tcand_in\tslots\tobjs0a\tcars958\tret\tcand_out\tsub\tc41f030_c48a630\n");
        }
    }
    return f;
}
std::string RegSlotsHex(int d8, std::string* objs) {
    std::string out;
    for (int k = 0; k < d8 && k < 300; ++k) {
        const unsigned char* sl = reinterpret_cast<const unsigned char*>(kRgSlots + k * kRgStride);
        if (k) out += ';';
        out += HexB(sl, 0x20);
        if (*reinterpret_cast<const std::uint32_t*>(sl + 0x10) == 0x0au && objs) {
            const unsigned char* dp = *reinterpret_cast<unsigned char* const*>(sl + 0x14);
            if (dp) { if (!objs->empty()) *objs += '|'; *objs += std::to_string(k) + ":exe"; }
        }
    }
    return out;
}
}  // namespace

void ModeSeven_SetHost(int (*sub_mode)(), int (*held)(int)) { s_m7SubMode = sub_mode; s_m7Held = held; }

bool ModeSeven_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_MODE7");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && mashed_re::Gameplay::PickupCallbacks_Enabled();
}

int ModeSeven(unsigned int spline, int v, const float look[2], float cand[2]) {
    if (v < 0 || v > 3 || !Rec(0)) return 0;
    EnsureLut();
    std::FILE* const tr = M7Trace();
    std::string pre;
    if (tr) {
        static long n = 0;
        const int d8 = I32m(kRgD8);
        std::uint32_t mp = 0; PtrCompute881ec8(&mp, static_cast<std::uint32_t>(v));
        std::string objs, cars;
        for (int c = 0; c < 4; ++c) { if (c) cars += ';'; cars += HexB(Rec(static_cast<std::uint32_t>(c)) + 0x958, 12); }
        char head[200];
        std::snprintf(head, sizeof head, "%ld\t%d\t%d\t%d\t%d\t%d\t%x\t%x\t", ++n, I32m(0x007f0ff4u), v, I32m(kRgCC), d8,
                      TG_SubMode(), TG_HeldWord(v), *reinterpret_cast<const std::uint32_t*>(0x008989b0u + v * 4u));
        const std::string slots = RegSlotsHex(d8, &objs);
        pre = std::string(head) + HexB(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(mp)), 0x40) + "\t" +
              HexB(Rec(static_cast<std::uint32_t>(v)) + mashed_re::Vehicle::kOff_WheelMatrices, 0x40) + "\t" +
              HexB(look, 8) + "\t" + HexB(cand, 8) + "\t" + slots + "\t" + objs + "\t" + cars + "\t";
    }
    s_m7Sub.clear();
    const long stub0 = s_m7Stub;
    int ret;
    {
        PcScope7 pc;
        ret = Body7()(spline, look, cand, v);
    }
    if (tr) {
        std::string sub;
        for (const std::string& x : s_m7Sub) { if (!sub.empty()) sub += '|'; sub += x; }
        std::fprintf(tr, "%s%d\t%s\t%s\t%ld:0\n", pre.c_str(), ret, HexB(cand, 8).c_str(), sub.c_str(), s_m7Stub - stub0);
        static int k = 0; if ((++k & 63) == 0) std::fflush(tr);
    }
    return ret;
}

void ModeSeven_MaybeReplay() {
    const char* inList = std::getenv("MASHED_C30_REPLAY");
    if (!inList) return;
    const char* outp = std::getenv("MASHED_C30_REPLAY_OUT");
    std::vector<std::vector<std::string>> rows;
    for (const std::string& path : SplitB(inList, ';')) {
        std::FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) continue;
        std::string all;
        { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
        std::fclose(f);
        std::size_t p = all.find('\n') + 1;                          // header
        while (p < all.size()) {
            std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
            std::string ln = all.substr(p, e - p); if (!ln.empty() && ln.back() == '\r') ln.pop_back();
            p = e + 1;
            std::vector<std::string> c = SplitB(ln, '\t');
            if (c.size() == 19) rows.push_back(c);
        }
    }
    // 0 n, 1 frame, 2 v, 3 cc, 4 d8, 5 ba8c, 6 held, 7 refdist, 8 mat, 9 mat0, 10 look, 11 cand_in, 12 slots, 13 objs0a,
    // 14 cars958, 15 ret, 16 cand_out, 17 sub, 18 c41f030_c48a630
    EnsureLut();
    std::vector<unsigned char> saveReg(300 * kRgStride);
    std::memcpy(saveReg.data(), reinterpret_cast<void*>(kRgSlots), saveReg.size());
    const std::int32_t saveCC = I32m(kRgCC), saveD8 = I32m(kRgD8);
    std::uint32_t saveRef[4]; std::memcpy(saveRef, reinterpret_cast<void*>(0x008989b0u), 16);
    static std::uint32_t fakeData[300][4];
    static std::uint32_t fakeObj[300][8];
    int total = 0, exact = 0, retBad = 0, candBad = 0, subBad = 0;
    std::map<std::string, int> rets;
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    const long subBad0 = s_m7SubBad, stub0 = s_m7Stub;
    {
        PcScope7 pc;
        s_replay = true; s_m7Replay = true;
        for (const auto& c : rows) {
            const int v = std::atoi(c[2].c_str()), d8 = std::atoi(c[4].c_str());
            if (v < 0 || v > 3 || d8 < 0 || d8 > 300) continue;
            const std::vector<std::string> sl = c[12].empty() ? std::vector<std::string>() : SplitB(c[12], ';');
            if (static_cast<int>(sl.size()) != d8) continue;
            ++total;
            std::memset(reinterpret_cast<void*>(kRgSlots), 0, 300 * kRgStride);
            for (int k = 0; k < d8; ++k) FromHexB(sl[k], reinterpret_cast<void*>(kRgSlots + k * kRgStride), 0x20);
            if (!c[13].empty())
                for (const std::string& ob : SplitB(c[13], '|')) {          // k:objptr:bits
                    const std::vector<std::string> q = SplitB(ob, ':');
                    if (q.size() < 2) continue;
                    const int k = std::atoi(q[0].c_str());
                    if (k < 0 || k >= d8) continue;
                    std::memset(fakeData[k], 0, sizeof fakeData[k]);
                    const bool nullObj = (q[1] == "0x0" || q[1] == "0");
                    if (!nullObj && q.size() >= 3 && !q[2].empty()) {
                        std::memset(fakeObj[k], 0, sizeof fakeObj[k]);
                        fakeObj[k][4] = static_cast<std::uint32_t>(std::strtoul(q[2].c_str(), nullptr, 16));   // +0x10
                        fakeData[k][1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(fakeObj[k]));   // +4
                    }
                    *reinterpret_cast<std::uint32_t*>(kRgSlots + k * kRgStride + 0x14) =
                        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(fakeData[k]));
                }
            I32m(kRgD8) = d8; I32m(kRgCC) = std::atoi(c[3].c_str());
            FromHexB(c[8], s_rMat[v & 3], 0x40);
            std::uint32_t m0[16]; FromHexB(c[9], m0, 0x40);
            s_rAt[0] = m0[8]; s_rAt[1] = m0[9]; s_rAt[2] = m0[10];
            *reinterpret_cast<std::uint32_t*>(0x008989b0u + v * 4u) = static_cast<std::uint32_t>(std::strtoul(c[7].c_str(), nullptr, 16));
            s_m7RpBa8c = std::atoi(c[5].c_str());
            s_m7RpHeld = static_cast<std::uint32_t>(std::strtoul(c[6].c_str(), nullptr, 16));
            s_m7RpSub.clear(); s_m7RpSubPos = 0;
            for (const std::string& x : SplitB(c[17], '|')) if (x.compare(0, 4, "300:") == 0) s_m7RpSub.push_back(x);
            float look[2], cand[2];
            FromHexB(c[10], look, 8); FromHexB(c[11], cand, 8);
            s_m7Sub.clear();
            const int ret = Body7()(0u, look, cand, v);
            std::string sub;
            for (const std::string& x : s_m7Sub) { if (!sub.empty()) sub += '|'; sub += x; }
            bool ok = true;
            if (ret != std::atoi(c[15].c_str())) { ok = false; ++retBad; }
            if (HexB(cand, 8) != c[16]) { ok = false; ++candBad; }
            if (sub != c[17]) { ok = false; ++subBad; }
            ++rets["v" + c[2] + "_r" + std::to_string(ret)];
            if (ok) ++exact;
            else if (o && total - exact <= 30)
                std::fprintf(o, "MISMATCH n=%s frame=%s v=%d ret=%d/%s cand=%s/%s\n  port_sub=[%s]\n  orig_sub=[%s]\n", c[0].c_str(), c[1].c_str(),
                             v, ret, c[15].c_str(), HexB(cand, 8).c_str(), c[16].c_str(), sub.c_str(), c[17].c_str());
        }
        s_replay = false; s_m7Replay = false;
    }
    std::memcpy(reinterpret_cast<void*>(kRgSlots), saveReg.data(), saveReg.size());
    I32m(kRgCC) = saveCC; I32m(kRgD8) = saveD8;
    std::memcpy(reinterpret_cast<void*>(0x008989b0u), saveRef, 16);
    if (o) {
        std::fprintf(o, "G-M4-REPLAY calls=%d exact=%d ret_bad=%d cand_bad=%d sub_bad=%d 300_arg_bad=%ld stubs=%ld pc=%s in=%s\n",
                     total, exact, retBad, candBad, subBad, s_m7SubBad - subBad0, s_m7Stub - stub0,
                     std::getenv("MASHED_MODE7_PC") ? std::getenv("MASHED_MODE7_PC") : "24", inList);
        std::fprintf(o, "returns:");
        for (const auto& kv : rets) std::fprintf(o, " %s=%d", kv.first.c_str(), kv.second);
        std::fprintf(o, "\n");
        std::fclose(o);
    }
}

}  // namespace Ai
