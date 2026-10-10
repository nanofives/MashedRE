// Mashed RE — mode 7 leg M1: the original's world-object registry / collision broker (exe-only TU).
//
// Array 0x006dccb8 (300 slots of 0x8c), write index DAT_006e70cc, published count DAT_006e70d8,
// grid-built flag DAT_006e70d4, grid enable DAT_006e70c8, grid 0x006cfcb8..0x006dccb0 (11 x 11 cells
// of 0x1b8) with its centre pointer DAT_006dccb0. Verbatim naked transcriptions generated from the
// anchored bytes by re/tools/gen_naked_asm.py (spec re/tools/naked_specs/registry_m1.json ->
// WorldRegistry_naked.inc):
//   FUN_00484cf0 registrar, FUN_004847d0 grid build, FUN_00484ac0 / FUN_00484a50 grid insert,
//   FUN_00485070 publish, FUN_00484de0 grid pair pass, FUN_00484f70 O(N^2) pair pass,
//   FUN_00484c90 init, FUN_004c39b0 (RwV3dNormalize, LUT base patched as in P2).
// C++ here, with the original's cdecl contract:
//   RG_CbDispatch  the four indirect callback calls (0x00484edb / 0x00484ee9 / 0x0048500e /
//                  0x0048501c) are patched to store the target in g_rg_cbTarget and call this.
//                  LEG M1: RECORD ONLY, no callback runs (leg M3 dispatches).
//   RG_CamFrame    FUN_00467210(0) (camera frame +0x10): replay buffer; live: zero matrix
//                  [UNCERTAIN] (the grid path needs DAT_006e70c8 != 0, measured 0 on 121/121
//                  original frames, o_rg1; leg M2 decides the live source if it ever runs)
//   RG_Center      FUN_00443090 (= &DAT_00897ff0)
//   RG_Ftol2 / RG_Floor  _ftol2 0x004a2c48 / CRT floor 0x004a3090
// The .rdata constants are exe-safe copies.
#include "WorldRegistry.h"
#include "../Math/RwLutTables.h"

#include <float.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
const std::uint32_t g_rg_c_5cc350 = 0x40900000u;   // 4.5
const std::uint32_t g_rg_c_5cda64 = 0x41300000u;   // 11.0
const std::uint32_t g_rg_c_5ce178 = 0x41900000u;   // 18.0
const std::uint32_t g_rg_c_5cf288 = 0x3ca57eb5u;   // 0.0202020202
const std::uint32_t g_rg_c_5cf28c = 0x3e638e39u;   // 0.2222222
const std::uint32_t g_rg_c_5cf290 = 0x41c60000u;   // 24.75
const std::uint32_t g_rg_c_5cf294 = 0x40100000u;   // 2.25
const std::uint32_t g_rg_c_5cf298 = 0x3f6e147bu;   // 0.93
const std::uint32_t g_rg_c_5d757c = 0x00000000u;   // 0.0
struct { const std::uint32_t* sqrt_; const std::uint32_t* inv_; } g_rg_lutptrs = { nullptr, nullptr };
std::uint32_t g_rg_cbTarget = 0;
}

namespace {

constexpr std::uintptr_t kSlots = 0x006dccb8u, kStride = 0x8cu;
constexpr std::uintptr_t kCC = 0x006e70ccu, kD8 = 0x006e70d8u, kD4 = 0x006e70d4u, kC8 = 0x006e70c8u;
constexpr std::uintptr_t kGrid = 0x006cfcb8u, kGridLen = 0xcffcu;   // .. 0x006dccb4 incl. DAT_006dccb0

inline std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }

bool          s_replay = false;
std::uint32_t s_camBuf[16] = {};      // fake frame: +0x20..+0x28 = at row
std::uint32_t s_ctrBuf[3] = {};
struct CbRec { std::uint32_t target; int a, b; };
std::vector<CbRec> s_cbLog;
long s_cbLive = 0;

}  // namespace

extern "C" {

void __cdecl RG_CbDispatch(void* a, void* b) {
    const int ia = static_cast<int>((reinterpret_cast<std::uintptr_t>(a) - kSlots) / kStride);
    const int ib = static_cast<int>((reinterpret_cast<std::uintptr_t>(b) - kSlots) / kStride);
    if (s_replay) s_cbLog.push_back({g_rg_cbTarget, ia, ib});
    else ++s_cbLive;                   // LEG M1: record only. Leg M3 dispatches here.
}

void* __cdecl RG_CamFrame(int) {
    if (s_replay) return s_camBuf;
    static std::uint32_t zero[16] = {};
    return zero;
}
void* __cdecl RG_Center() {
    return s_replay ? static_cast<void*>(s_ctrBuf) : reinterpret_cast<void*>(0x00897ff0u);
}
double __cdecl RG_Floor(double x) { return std::floor(x); }

__declspec(naked) void RG_Ftol2() {
    __asm {
        sub    esp, 12
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        or     ax, 0x0c00
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
__declspec(naked) void RG_Nop()     { __asm { ret } }
__declspec(naked) void RG_ErrPass() { __asm { xor eax, eax
                                              ret } }

}  // extern "C"

#include "WorldRegistry_naked.inc"

namespace mashed_re {
namespace Race {

namespace {

void EnsureLut() {
    if (!g_rg_lutptrs.sqrt_) {
        g_rg_lutptrs.sqrt_ = Math::RwLutSqrtTable();
        g_rg_lutptrs.inv_  = Math::RwLutInvSqrtTable();
    }
}
// x87 precision: MASHED_REGISTRY_PC = 24 | 53 | 64; default 24 (the race loop's, RESULT_P2 section 1).
unsigned int PcBits() {
    const char* s = std::getenv("MASHED_REGISTRY_PC");
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
typedef int  (__cdecl* reg_fn)(const std::uint32_t*);
typedef void (__cdecl* void_fn)();

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

bool Registry_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_REGISTRY");
        return s && s[0] && s[0] != '0';
    }();
    return s_on;
}

void Registry_RaceInit() {
    if (!Registry_Enabled()) return;
    reinterpret_cast<void_fn>(&RG_484c90)();
}

void Registry_FrameTick() {
    if (!Registry_Enabled()) return;
    EnsureLut();
    // G-M2a-LIVE trace: MASHED_REGISTRY_TRACE=<csv>, one row per slot [0, cc) at publish time.
    {
        static std::FILE* s_tr = nullptr; static bool s_init = false; static int s_fr = 0;
        if (!s_init) {
            s_init = true;
            if (const char* t = std::getenv("MASHED_REGISTRY_TRACE")) {
                s_tr = std::fopen(t, "w");
                if (s_tr) std::fprintf(s_tr, "frame,slot,type,radius,x,y,z,data,cb\n");
            }
        }
        if (s_tr) {
            const int cc = I32(kCC);
            for (int k = 0; k < cc && k < 300; ++k) {
                const std::uintptr_t sl = kSlots + k * kStride;
                const float* fp = reinterpret_cast<const float*>(sl);
                std::fprintf(s_tr, "%d,%d,%08x,%.9g,%.9g,%.9g,%.9g,%08x,%08x\n", s_fr, k,
                             *reinterpret_cast<const std::uint32_t*>(sl + 0x10), fp[3], fp[0], fp[1], fp[2],
                             *reinterpret_cast<const std::uint32_t*>(sl + 0x14),
                             *reinterpret_cast<const std::uint32_t*>(sl + 0x1c));
            }
            if ((s_fr & 63) == 0) std::fflush(s_tr);
            ++s_fr;
        }
    }
    PcScope pc;
    reinterpret_cast<void_fn>(&RG_485070)();
}

int Registry_Register(const std::uint32_t desc[8]) {
    if (!Registry_Enabled()) return 0;
    EnsureLut();
    PcScope pc;
    return reinterpret_cast<reg_fn>(&RG_484cf0)(desc);
}

void Registry_MaybeReplay() {
    const char* in = std::getenv("MASHED_REGISTRY_REPLAY");
    if (!in) return;
    const char* outp = std::getenv("MASHED_REGISTRY_REPLAY_OUT");
    std::FILE* f = std::fopen(in, "rb");
    if (!f) return;
    std::string all;
    { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
    std::fclose(f);
    std::vector<std::vector<std::string>> rows;
    std::size_t p = all.find('\n') + 1;                                // skip header
    while (p < all.size()) {
        std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
        std::vector<std::string> c; std::size_t q = p;
        while (true) {
            std::size_t t = all.find('\t', q);
            if (t == std::string::npos || t > e) { c.push_back(all.substr(q, e - q)); break; }
            c.push_back(all.substr(q, t - q)); q = t + 1;
        }
        if (!c.empty() && !c.back().empty() && c.back().back() == '\r') c.back().pop_back();
        if (c.size() == 12 || c.size() == 13) rows.push_back(c);
        p = e + 1;
    }
    // columns: 0 n, 1 frame, 2 cc, 3 c8, 4 d4, 5 grid_in, 6 slots_in, 7 grid, 8 d8, 9 cc_out, 10 d4_out, 11 slots_out
    EnsureLut();
    std::vector<std::uint8_t> save(0x006e70dcu - kGrid);
    std::memcpy(save.data(), reinterpret_cast<void*>(kGrid), save.size());
    const std::uint8_t kSent = 0xcd;
    int frames = 0, regOk = 0, passOk = 0, gridFrames = 0, cbTotal = 0, cbBitsBad = 0;
    int cbFrames = 0, cbSeqOk = 0, origCb = 0;
    std::vector<int> neverWritten(kStride, 0), regMisOff(kStride, 0), passMisOff(kStride, 0);
    int sentinelRows = 0;
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    s_replay = true;
    {
    PcScope pc;
    for (const auto& c : rows) {
        const int cc = std::atoi(c[2].c_str()), c8 = std::atoi(c[3].c_str()), d8 = std::atoi(c[8].c_str());
        if (cc < 0 || cc > 300 || d8 < 0 || d8 > 300) continue;
        std::vector<std::uint8_t> sin(cc * kStride), sout(d8 * kStride);
        if (!FromHex(c[6], sin.data(), sin.size()) || !FromHex(c[11], sout.data(), sout.size())) continue;
        ++frames;
        if (c8) ++gridFrames;
        // grid inputs "frame;c8;at_x;at_y;at_z;cx;cy;cz"
        if (!c[5].empty()) {
            unsigned v[8] = {};
            std::sscanf(c[5].c_str(), "%u;%u;%u;%u;%u;%u;%u;%u", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7]);
            s_camBuf[8] = v[2]; s_camBuf[9] = v[3]; s_camBuf[10] = v[4];
            s_ctrBuf[0] = v[5]; s_ctrBuf[1] = v[6]; s_ctrBuf[2] = v[7];
        }
        // ---- G-M1-REG: registrar replay from cc = d4 = 0 into a sentinel-filled array ----
        std::memset(reinterpret_cast<void*>(kSlots), kSent, 300 * kStride);
        if (c8) std::memset(reinterpret_cast<void*>(kGrid), 0, kGridLen);
        I32(kCC) = 0; I32(kD4) = 0; I32(kC8) = c8;
        for (int k = 0; k < cc; ++k) {
            std::uint32_t desc[8];
            std::memcpy(desc, sin.data() + k * kStride, 32);
            reinterpret_cast<reg_fn>(&RG_484cf0)(desc);
        }
        bool ok = I32(kCC) == cc;
        ++sentinelRows;
        for (int k = 0; k < cc; ++k) {
            const std::uint8_t* pp = reinterpret_cast<const std::uint8_t*>(kSlots + k * kStride);
            const std::uint8_t* po = sin.data() + k * kStride;
            for (std::uintptr_t b = 0; b < kStride; ++b) {
                if (pp[b] == po[b]) continue;
                if (pp[b] == kSent) { ++neverWritten[b]; continue; }      // registrar does not write it
                ++regMisOff[b]; ok = false;
            }
        }
        if (c8 && !c[7].empty()) {
            std::vector<std::uint8_t> g(kGridLen);
            if (FromHex(c[7], g.data(), g.size()) && std::memcmp(g.data(), reinterpret_cast<void*>(kGrid), kGridLen) != 0) ok = false;
        }
        if (ok) ++regOk;
        else if (o) std::fprintf(o, "REG MISMATCH n=%s frame=%s cc=%d port_cc=%d\n", c[0].c_str(), c[1].c_str(), cc, I32(kCC));
        // ---- G-M1-PASS: the original's entry state -> port FUN_00485070 -> leave state ----
        std::memset(reinterpret_cast<void*>(kSlots), 0, 300 * kStride);
        std::memcpy(reinterpret_cast<void*>(kSlots), sin.data(), sin.size());
        if (c8 && !c[7].empty()) FromHex(c[7], reinterpret_cast<std::uint8_t*>(kGrid), kGridLen);
        I32(kCC) = cc; I32(kD4) = std::atoi(c[4].c_str()); I32(kC8) = c8;
        s_cbLog.clear();
        reinterpret_cast<void_fn>(&RG_485070)();
        bool ok2 = I32(kD8) == d8 && I32(kCC) == std::atoi(c[9].c_str()) && I32(kD4) == std::atoi(c[10].c_str());
        for (int k = 0; k < d8; ++k) {
            const std::uint8_t* pp = reinterpret_cast<const std::uint8_t*>(kSlots + k * kStride);
            const std::uint8_t* po = sout.data() + k * kStride;
            for (std::uintptr_t b = 0; b < kStride; ++b) if (pp[b] != po[b]) { ++passMisOff[b]; ok2 = false; }
        }
        for (const CbRec& r : s_cbLog) {
            ++cbTotal;
            const std::uint8_t* sa = reinterpret_cast<const std::uint8_t*>(kSlots + r.a * kStride);
            const std::uint8_t* sb = reinterpret_cast<const std::uint8_t*>(kSlots + r.b * kStride);
            const bool bits = (sa[0x68 + (r.b >> 3)] & (1 << (r.b & 7))) && (sb[0x68 + (r.a >> 3)] & (1 << (r.a & 7)));
            if (c8 && !bits) ++cbBitsBad;   // only the grid pass FUN_00484de0 sets pair bits
            if (o && cbTotal <= 40) std::fprintf(o, "CB frame=%s target=%08x a=%d b=%d type_a=%08x type_b=%08x\n", c[1].c_str(),
                                                 r.target, r.a, r.b, *reinterpret_cast<const std::uint32_t*>(sa + 0x10),
                                                 *reinterpret_cast<const std::uint32_t*>(sb + 0x10));
        }
        // callback sequence vs the original's witnesses (column 12: "target:a:b|...", hex target)
        if (c.size() == 13) {
            std::string seq;
            for (const CbRec& r : s_cbLog) {
                char t[64]; std::snprintf(t, sizeof t, "%x:%d:%d", r.target, r.a, r.b);
                if (!seq.empty()) seq += '|';
                seq += t;
            }
            if (!c[12].empty() || !s_cbLog.empty()) {
                ++cbFrames;
                origCb += c[12].empty() ? 0 : 1 + static_cast<int>(std::count(c[12].begin(), c[12].end(), '|'));
                if (seq == c[12]) ++cbSeqOk;
                else { ok2 = false; if (o) std::fprintf(o, "CBSEQ MISMATCH frame=%s port=[%s] orig=[%s]\n", c[1].c_str(), seq.c_str(), c[12].c_str()); }
            }
        }
        if (ok2) ++passOk;
        else if (o) std::fprintf(o, "PASS MISMATCH n=%s frame=%s d8=%d port_d8=%d\n", c[0].c_str(), c[1].c_str(), d8, I32(kD8));
    }
    }
    s_replay = false;
    std::memcpy(reinterpret_cast<void*>(kGrid), save.data(), save.size());
    if (o) {
        std::fprintf(o, "G-M1-REG frames=%d exact=%d (grid frames %d)\n", frames, regOk, gridFrames);
        std::fprintf(o, "G-M1-PASS frames=%d exact=%d callbacks=%d callbacks_without_both_bits(grid only)=%d\n", frames, passOk, cbTotal, cbBitsBad);
        std::fprintf(o, "G-M1-CB frames_with_callbacks=%d sequence_exact=%d original_callbacks=%d\n", cbFrames, cbSeqOk, origCb);
        std::fprintf(o, "slot bytes the registrar never wrote (port left sentinel, rows=%d):", sentinelRows);
        for (std::uintptr_t b = 0; b < kStride; ++b) if (neverWritten[b]) std::fprintf(o, " +0x%02x(%d)", static_cast<unsigned>(b), neverWritten[b]);
        std::fprintf(o, "\nregistrar mismatched bytes:");
        for (std::uintptr_t b = 0; b < kStride; ++b) if (regMisOff[b]) std::fprintf(o, " +0x%02x(%d)", static_cast<unsigned>(b), regMisOff[b]);
        std::fprintf(o, "\npass mismatched bytes:");
        for (std::uintptr_t b = 0; b < kStride; ++b) if (passMisOff[b]) std::fprintf(o, " +0x%02x(%d)", static_cast<unsigned>(b), passMisOff[b]);
        std::fprintf(o, "\n");
        std::fclose(o);
    }
}

}  // namespace Race
}  // namespace mashed_re
