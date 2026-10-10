// Mashed RE — mode 7 leg M2a: the original's pickup tick FUN_00459000 (exe-only TU).
//
// Pool 0x0068b198 (25 x 0x50), count 0x0068b9a8. Entry: +0x00 RW object, +0x04 angle, +0x08 rate,
// +0x0c axis, +0x18 respawn time, +0x1c timer, +0x20 state (1 live, 2 respawning), +0x24 type,
// +0x28 re-roll flag, +0x2c anchor, +0x38 live position, +0x44 velocity.
// Verbatim naked (re/tools/gen_naked_asm.py, spec re/tools/naked_specs/pickups_m2a.json ->
// PickupTick_naked.inc): FUN_00459000, FUN_00458a40 (EDI = entry), FUN_004584e0 (EAX = entry,
// ESI = out), FUN_004576b0, FUN_004c3ac0 / FUN_004c39b0 (LUT patched). C++ shims, cdecl:
//   PK_FrameTranslate  FUN_004c1340 (RwFrameTranslate) on the fake frame: modelling matrix =
//                      identity + translation for combine 0 (replace); any other combine is counted.
//   PK_FrameRotate     FUN_004c1520 (RwFrameRotate, pre-concat): the translation row is unchanged
//                      by a pre-concatenated rotation; the rotation rows are NOT modelled (no exe
//                      reader; render only). Counted.
//   PK_TypeReroll      FUN_00458d00 (random type, FUN_00472690 RNG): reachable only with +0x28 != 0,
//                      which the exe's rank-0/1 spawn never sets. Counted; returns the entry's type
//                      unchanged [a call is a gate failure, not a value].
//   PK_Nop             FUN_00458dd0 (re-skin) and the sprite queue FUN_004769a0/d0/f0, FUN_00476a30/
//                      d00/df0 (render: PickupField draws in the exe).
// RG_484cf0 is mode-7 M1's verbatim registrar (Race/WorldRegistry.cpp).
// [0x007f100c] (the timer step) reads g_pk_dt = (float)DAT_007f1008 * _DAT_005cc948 (0x0040fc57).
#include "PickupTick.h"
#include "PickupPoolSpawn.h"
#include "../Race/WorldRegistry.h"
#include "../Math/RwLutTables.h"

#include <float.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" void RG_484cf0();   // Race/WorldRegistry_naked.inc

extern "C" {
const std::uint32_t g_pk_c_5cc990 = 0x3727c5acu;   // 1e-5
const std::uint32_t g_pk_c_5cc9a0 = 0x3d4ccccdu;   // 0.05
const std::uint32_t g_pk_c_5cc9dc = 0x3f733333u;   // 0.95
const std::uint64_t g_pk_c_5cd0c8 = 0x3ff0000000000000ull;   // 1.0 (qword)
const std::uint32_t g_pk_c_5cd7a8 = 0x3c8efa35u;   // 0.0174532924
const std::uint32_t g_pk_c_5ce2f4 = 0x40490fdbu;   // 3.14159274
const std::uint32_t g_pk_c_5ce540 = 0x3fa00000u;   // 1.25
const std::uint32_t g_pk_c_5ce544 = 0x3fa00000u;   // 1.25
const std::uint32_t g_pk_c_5ce548 = 0x3f800000u;   // 1.0
const std::uint32_t g_pk_c_5ce54c = 0x358637bdu;   // 1e-6
const std::uint32_t g_pk_c_5d757c = 0x00000000u;   // 0.0
std::uint32_t g_pk_dt = 0;
struct { const std::uint32_t* sqrt_; const std::uint32_t* inv_; } g_pk_lutptrs = { nullptr, nullptr };
}

namespace {

constexpr std::uintptr_t kPool = 0x0068b198u, kCount = 0x0068b9a8u, kStride = 0x50u;
constexpr int kMax = 25;

struct FakeObj { std::uint32_t pad; unsigned char* frame; };
FakeObj       s_obj[kMax];
unsigned char s_frame[kMax][0x50];
long s_trTranslateOther = 0, s_trRotate = 0, s_trReroll = 0;

void FrameIdentity(unsigned char* fr) {
    std::memset(fr + 0x10, 0, 0x40);
    const float one = 1.0f;
    std::memcpy(fr + 0x10, &one, 4);       // right.x
    std::memcpy(fr + 0x24, &one, 4);       // up.y
    std::memcpy(fr + 0x38, &one, 4);       // at.z
}

void BindAll() {
    for (int i = 0; i < kMax; ++i) {
        s_obj[i].frame = s_frame[i];
        *reinterpret_cast<FakeObj**>(kPool + i * kStride) = &s_obj[i];
    }
}

}  // namespace

extern "C" {

void* __cdecl PK_FrameTranslate(unsigned char* frame, const float* v, int combine) {
    if (combine != 0) { ++s_trTranslateOther; return frame; }
    FrameIdentity(frame);
    std::memcpy(frame + 0x40, v, 12);
    return frame;
}
void* __cdecl PK_FrameRotate(unsigned char* frame, const float*, float, int) { ++s_trRotate; return frame; }
std::int32_t __cdecl PK_TypeReroll() { ++s_trReroll; return 7; }
__declspec(naked) void PK_Nop()     { __asm { ret } }
__declspec(naked) void PK_ErrPass() { __asm { xor eax, eax
                                              ret } }

}  // extern "C"

#include "PickupTick_naked.inc"

namespace mashed_re {
namespace Gameplay {

namespace {

void EnsureLut() {
    if (!g_pk_lutptrs.sqrt_) {
        g_pk_lutptrs.sqrt_ = Math::RwLutSqrtTable();
        g_pk_lutptrs.inv_  = Math::RwLutInvSqrtTable();
    }
}
unsigned int PcBits() {
    const char* s = std::getenv("MASHED_PICKUPS_PC");
    if (s && std::strcmp(s, "53") == 0) return _PC_53;
    if (s && std::strcmp(s, "64") == 0) return _PC_64;
    return _PC_24;
}
struct PcScope {   // read first: the setting call returns the NEW word (U-9216)
    unsigned int old = 0;
    PcScope() { _controlfp_s(&old, 0, 0); unsigned int t; _controlfp_s(&t, PcBits(), _MCW_PC); }
    ~PcScope() { unsigned int t; _controlfp_s(&t, old & _MCW_PC, _MCW_PC); }
};
typedef void (__cdecl* void_fn)();

bool FromHex(const std::string& h, unsigned char* out, std::size_t n) {
    if (h.size() != n * 2) return false;
    for (std::size_t i = 0; i < n; ++i) {
        unsigned b = 0;
        if (std::sscanf(h.c_str() + i * 2, "%2x", &b) != 1) return false;
        out[i] = static_cast<unsigned char>(b);
    }
    return true;
}
std::vector<std::string> Split(const std::string& s, char d) {
    std::vector<std::string> o; std::size_t p = 0;
    while (true) { std::size_t q = s.find(d, p); o.push_back(s.substr(p, q == std::string::npos ? std::string::npos : q - p));
                   if (q == std::string::npos) break; p = q + 1; }
    return o;
}

}  // namespace

bool PickupsOrig_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_PICKUPS_ORIG");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && Race::Registry_Enabled();
}

void PickupsOrig_OnSpawn(unsigned char* entry) {
    if (!PickupsOrig_Enabled()) return;
    BindAll();
    FakeObj* o = *reinterpret_cast<FakeObj**>(entry);
    FrameIdentity(o->frame);                       // FUN_004c15c0 at 0x00458ebd
}

void Pickups_FrameTick() {
    if (!PickupsOrig_Enabled()) return;
    EnsureLut();
    BindAll();
    const float dt = static_cast<float>(static_cast<double>(*reinterpret_cast<const std::int32_t*>(0x007f1008u)) *
                                        static_cast<double>(0.00033333332976326346f));   // 0x0040fc57
    std::memcpy(&g_pk_dt, &dt, 4);
    PcScope pc;
    reinterpret_cast<void_fn>(&PK_459000)();
}

void Pickups_MaybeReplay() {
    const char* in = std::getenv("MASHED_PICKUPS_REPLAY");
    if (!in) return;
    const char* outp = std::getenv("MASHED_PICKUPS_REPLAY_OUT");
    std::FILE* f = std::fopen(in, "rb");
    if (!f) return;
    std::string all;
    { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
    std::fclose(f);
    // columns: 0 n, 1 frame, 2 cnt, 3 dt, 4 bm300, 5 bm254, 6 cc, 7 pool_in, 8 frames_in,
    //          9 cnt_out, 10 cc_out, 11 pool_out, 12 frames_out, 13 slots_new
    std::vector<std::vector<std::string>> rows;
    std::size_t p = all.find('\n') + 1;
    while (p < all.size()) {
        std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
        std::string ln = all.substr(p, e - p); if (!ln.empty() && ln.back() == '\r') ln.pop_back();
        std::vector<std::string> c = Split(ln, '\t');
        if (c.size() == 14) rows.push_back(c);
        p = e + 1;
    }
    EnsureLut();
    std::vector<unsigned char> savePool(0x7d4), saveBm(0x0068a294u - 0x0068a254u), saveBm2(8), saveReg(300 * 0x8c);
    std::memcpy(savePool.data(), reinterpret_cast<void*>(kPool), savePool.size());
    std::memcpy(saveBm.data(), reinterpret_cast<void*>(0x0068a254u), saveBm.size());
    std::memcpy(saveBm2.data(), reinterpret_cast<void*>(0x0068a300u), 8);
    std::memcpy(saveReg.data(), reinterpret_cast<void*>(0x006dccb8u), saveReg.size());
    const std::int32_t saveCC = *reinterpret_cast<std::int32_t*>(0x006e70ccu);
    const std::uint32_t saveDt = g_pk_dt;
    int calls = 0, exact = 0, poolBad = 0, frameBad = 0, regBad = 0, ccBad = 0;
    std::vector<int> poolOff(kStride, 0);
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    const long rr0 = s_trReroll, to0 = s_trTranslateOther;
    {
        PcScope pc;
        for (const auto& c : rows) {
            unsigned char pin[0x7d0], pout[0x7d0];
            if (!FromHex(c[7], pin, 0x7d0) || !FromHex(c[11], pout, 0x7d0)) continue;
            const int cnt = std::atoi(c[2].c_str()), cc = std::atoi(c[6].c_str());
            if (cnt < 0 || cnt > kMax || cc < 0 || cc > 300) continue;
            const std::vector<std::string> fin = Split(c[8], ';'), fout = Split(c[12], ';');
            if (fin.size() != kMax || fout.size() != kMax) continue;
            ++calls;
            // inputs
            std::memcpy(reinterpret_cast<void*>(kPool), pin, 0x7d0);
            *reinterpret_cast<std::int32_t*>(kCount) = cnt;
            std::uint32_t origObj[kMax];
            for (int i = 0; i < kMax; ++i) {
                origObj[i] = *reinterpret_cast<std::uint32_t*>(kPool + i * kStride);
                s_obj[i].frame = s_frame[i];
                std::memset(s_frame[i], 0, 0x50);
                if (fin[i].size() == 0x80) FromHex(fin[i], s_frame[i] + 0x10, 0x40);
                *reinterpret_cast<FakeObj**>(kPool + i * kStride) = &s_obj[i];
            }
            g_pk_dt = static_cast<std::uint32_t>(std::strtoul(c[3].c_str(), nullptr, 10));
            FromHex(c[4], reinterpret_cast<unsigned char*>(0x0068a300u), 8);
            FromHex(c[5], reinterpret_cast<unsigned char*>(0x0068a254u), 0x40);
            *reinterpret_cast<std::int32_t*>(0x006e70ccu) = cc;
            *reinterpret_cast<std::int32_t*>(0x006e70d4u) = 0;
            reinterpret_cast<void_fn>(&PK_459000)();
            // compare (pool with +0x00 restored to the original's pointer values)
            bool ok = true;
            for (int i = 0; i < kMax; ++i) *reinterpret_cast<std::uint32_t*>(kPool + i * kStride) = origObj[i];
            if (std::memcmp(reinterpret_cast<void*>(kPool), pout, 0x7d0) != 0) {
                ok = false; ++poolBad;
                for (int b = 0; b < 0x7d0; ++b) if (reinterpret_cast<unsigned char*>(kPool)[b] != pout[b]) ++poolOff[b % kStride];
            }
            for (int i = 0; i < kMax; ++i) {
                if (fout[i].size() != 0x80) continue;
                unsigned char fo[0x40]; FromHex(fout[i], fo, 0x40);
                if (std::memcmp(s_frame[i] + 0x40, fo + 0x30, 12) != 0) { ok = false; ++frameBad; break; }
            }
            const int ccOut = std::atoi(c[10].c_str());
            if (*reinterpret_cast<std::int32_t*>(0x006e70ccu) != ccOut) { ok = false; ++ccBad; }
            else if (ccOut > cc) {
                std::vector<unsigned char> sn((ccOut - cc) * 0x8c);
                if (FromHex(c[13], sn.data(), sn.size())) {
                    // registered descriptors: compare the 8 written dwords + the index short of each slot
                    for (int k = 0; k < ccOut - cc; ++k) {
                        const unsigned char* pp = reinterpret_cast<const unsigned char*>(0x006dccb8u + (cc + k) * 0x8c);
                        const unsigned char* po = sn.data() + k * 0x8c;
                        if (std::memcmp(pp, po, 0x22) != 0 || std::memcmp(pp + 0x24, po + 0x24, 0x8c - 0x24) != 0) { ok = false; ++regBad; break; }
                    }
                }
            }
            if (ok) ++exact;
            else if (o && calls - exact <= 20) std::fprintf(o, "MISMATCH n=%s frame=%s cnt=%d\n", c[0].c_str(), c[1].c_str(), cnt);
        }
    }
    std::memcpy(reinterpret_cast<void*>(kPool), savePool.data(), savePool.size());
    std::memcpy(reinterpret_cast<void*>(0x0068a254u), saveBm.data(), saveBm.size());
    std::memcpy(reinterpret_cast<void*>(0x0068a300u), saveBm2.data(), 8);
    std::memcpy(reinterpret_cast<void*>(0x006dccb8u), saveReg.data(), saveReg.size());
    *reinterpret_cast<std::int32_t*>(0x006e70ccu) = saveCC;
    g_pk_dt = saveDt;
    if (o) {
        std::fprintf(o, "G-M2a-REPLAY calls=%d exact=%d pool_bad=%d frame_bad=%d reg_bad=%d cc_bad=%d reroll_calls=%ld translate_other=%ld pc=%s\n",
                     calls, exact, poolBad, frameBad, regBad, ccBad, s_trReroll - rr0, s_trTranslateOther - to0,
                     std::getenv("MASHED_PICKUPS_PC") ? std::getenv("MASHED_PICKUPS_PC") : "24");
        std::fprintf(o, "pool mismatched entry offsets:");
        for (std::uintptr_t b = 0; b < kStride; ++b) if (poolOff[b]) std::fprintf(o, " +0x%02x(%d)", static_cast<unsigned>(b), poolOff[b]);
        std::fprintf(o, "\n");
        std::fclose(o);
    }
}

}  // namespace Gameplay
}  // namespace mashed_re
