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
//
// Mode 7 leg M3 (PREREG verify/d3_mode7_m3_20261010/PREREG_M3.md): the registry's pickup callback.
// Verbatim naked (spec re/tools/naked_specs/pickups_m3.json -> PickupCollect_naked.inc): FUN_00458920,
// FUN_00458570 (EAX = car, ESI = out; its ST0 is discarded at 0x004589e2 and its out buffer is a dead
// local), FUN_0045c330, FUN_0046d4a0, FUN_00405890, FUN_00407580; FUN_004c3ac0 / FUN_004c39b0 are the
// PK_ bodies above. C bindings, cdecl:
//   PK_SubMode       FUN_0040e350 -> the exe's FUN_0040e350 host (aib_game_sub_mode, TrackRenderer.cpp)
//   PK_SlotEmpty     FUN_0045bff0 ([0x0088fc88 + v*0xb4] == 0) -> !PowerupSystem::Armed(v)
//   PK_Grant         FUN_0045c010(v, type) -> PowerupSystem::Activate(v, type) at the exe thread's PC_53
//   PK_Burst         FUN_004588c0 (10 particles; 120 ring draws in the original, not reproduced): no-op
//   PK_Sfx           FUN_00465ca0 (audio; its inc [0x008aa2d0] has no other reference): no-op
//   PK_458570_w      witness (EAX = car) in front of PK_458570, for the replay's call list
#include "PickupTick.h"
#include "PickupPoolSpawn.h"
#include "../Race/WorldRegistry.h"
#include "../Race/CarRegistrant.h"
#include "../Math/RwLutTables.h"

#include <float.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
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

// ---- mode 7 leg M3: FUN_00458920's bound callees (see the header comment) ----
namespace {
mashed_re::Gameplay::PickupCbHost s_cbHost = {};
bool          s_cbReplay = false;     // G-M3-REPLAY: inputs come from the capture row
int           s_rpBa8c = 6;
std::uint32_t s_rpA8 = 0;
std::vector<std::string> s_wit;       // calls made by the current FUN_00458920 call, in order
long s_grants = 0, s_bursts = 0, s_sfx = 0, s_pfCollects = 0;
}  // namespace

extern "C" {

int __cdecl PK_SubMode() {                                         // FUN_0040e350
    if (s_cbReplay) return s_rpBa8c;
    return s_cbHost.sub_mode ? s_cbHost.sub_mode() : 6;
}
int __cdecl PK_SlotEmpty(int v) {                                  // FUN_0045bff0
    if (s_cbReplay) return s_rpA8 == 0 ? 1 : 0;
    return s_cbHost.slot_empty ? s_cbHost.slot_empty(v) : 1;
}
void __cdecl PK_Grant(int v, int type) {                           // FUN_0045c010
    char t[32]; std::snprintf(t, sizeof t, "c010:%d:%d", v, type); s_wit.push_back(t);
    if (s_cbReplay) return;
    ++s_grants;
    if (!s_cbHost.grant) return;
    // The exe's PowerupSystem runs at the exe thread's PC_53 everywhere else; this call sits inside the
    // registry's PC_24 scope (verify/d3_mode7_20261010/cw_check_fixed.txt).
    unsigned int cw = 0, tmp = 0;
    _controlfp_s(&cw, 0, 0);
    _controlfp_s(&tmp, _PC_53, _MCW_PC);
    s_cbHost.grant(v, type);
    _controlfp_s(&tmp, cw & _MCW_PC, _MCW_PC);
}
void __cdecl PK_Burst(void*) { s_wit.push_back("88c0"); if (!s_cbReplay) ++s_bursts; }   // FUN_004588c0
void __cdecl PK_Sfx(int id, void*) {                                                      // FUN_00465ca0
    char t[24]; std::snprintf(t, sizeof t, "5ca0:%d", id); s_wit.push_back(t);
    if (!s_cbReplay) ++s_sfx;
}
void __cdecl PK_Note8570(int v) { char t[24]; std::snprintf(t, sizeof t, "8570:%d", v); s_wit.push_back(t); }
void PK_458570();
// EAX = car is FUN_00458570's register argument; ECX/EDX are not live at its call (0x004589dd) nor read
// before written at its entry (0x00458570..0x00458578), ESI/EDI are preserved by the cdecl note.
__declspec(naked) void PK_458570_w() {
    __asm {
        push eax
        call PK_Note8570
        pop  eax
        jmp  PK_458570
    }
}

}  // extern "C"

#include "PickupTick_naked.inc"
#include "PickupCollect_naked.inc"

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

// ---- mode 7 leg M3 ----
namespace {
typedef void (__cdecl* cb_fn)(void*, void*);
std::FILE* s_cbTrace = nullptr;
bool       s_cbTraceInit = false;
long       s_tick = 0, s_cbN = 0;
int        s_lastSt[kMax] = {};
int        s_lastCnt = -1;
long       s_since[kMax] = {};
double     s_dtsum[kMax] = {};

std::FILE* CbTrace() {
    if (!s_cbTraceInit) {
        s_cbTraceInit = true;
        if (const char* t = std::getenv("MASHED_PKCB_TRACE")) {
            s_cbTrace = std::fopen(t, "w");
            if (s_cbTrace)
                std::fprintf(s_cbTrace, "# exe trace (Gameplay/PickupTick.cpp), re/tools/pu_cb_report.py format: PK / ST rows as "
                                        "scenario_launch.py --cb-probe (frame = pickup tick index); PF tick car type = PickupField "
                                        "collection; CNT tick grants bursts sfx pf_collects callbacks unported\n");
        }
    }
    return s_cbTrace;
}
std::string Hex(const void* p, std::size_t n) {
    static const char* d = "0123456789abcdef";
    std::string s(n * 2, '0');
    const unsigned char* b = static_cast<const unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i) { s[2 * i] = d[b[i] >> 4]; s[2 * i + 1] = d[b[i] & 15]; }
    return s;
}
std::string JoinWit() {
    std::string w;
    for (const std::string& x : s_wit) { if (!w.empty()) w += '|'; w += x; }
    return w;
}
}  // namespace

// Per pickup tick: pool state transitions (+0x20) with the ticks and summed timer step since the entry's
// last transition, as the original-side probe records them; counters every 600 ticks.
void CbTraceTick(float dt) {
    ++s_tick;
    std::FILE* tr = CbTrace();
    if (!tr) return;
    int cnt = *reinterpret_cast<const std::int32_t*>(kCount);
    cnt = cnt < 0 ? 0 : (cnt > kMax ? kMax : cnt);
    for (int i = 0; i < cnt; ++i) {
        const unsigned char* e = reinterpret_cast<const unsigned char*>(kPool + i * kStride);
        const int st = *reinterpret_cast<const std::int32_t*>(e + 0x20);
        ++s_since[i]; s_dtsum[i] += dt;
        if (s_lastCnt >= 0 && i < s_lastCnt && st != s_lastSt[i]) {
            std::fprintf(tr, "ST\t0\t%ld\t%d\t%d\t%d\t%08x\t%08x\t%d\t%ld\t%.6f\n", s_tick, i, s_lastSt[i], st,
                         *reinterpret_cast<const std::uint32_t*>(e + 0x18), *reinterpret_cast<const std::uint32_t*>(e + 0x1c),
                         *reinterpret_cast<const std::int32_t*>(e + 0x24), s_since[i], s_dtsum[i]);
            s_since[i] = 0; s_dtsum[i] = 0;
        }
        s_lastSt[i] = st;
    }
    s_lastCnt = cnt;
    if (s_tick == 1) {   // G-M3b-SEED: each entry's respawn time (+0x18) and type at the first tick
        std::fprintf(tr, "SEED\t0\t%ld\t%d", s_tick, cnt);
        for (int i = 0; i < cnt; ++i)
            std::fprintf(tr, "\t%08x:%d", *reinterpret_cast<const std::uint32_t*>(kPool + i * kStride + 0x18),
                         *reinterpret_cast<const std::int32_t*>(kPool + i * kStride + 0x24));
        std::fprintf(tr, "\n");
    }
    if ((s_tick % 600) == 0) {
        std::fprintf(tr, "CNT\t0\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", s_tick, s_grants, s_bursts, s_sfx, s_pfCollects, s_cbN,
                     Race::Registry_UnportedCallbacks());
        std::fflush(tr);
    }
}

bool PickupCallbacks_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_REG_CALLBACKS");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && PickupsOrig_Enabled() && Race::CarsReg_Enabled();
}

void Pickups_SetCbHost(const PickupCbHost& host) { s_cbHost = host; }

int PickupsOrig_EntryState(int i) {
    if (!PickupsOrig_Enabled() || i < 0 || i >= kMax) return -1;
    return *reinterpret_cast<const std::int32_t*>(kPool + i * kStride + 0x20);
}

void PickupCb_NoteFieldCollect(int car, int type) {
    ++s_pfCollects;
    if (std::FILE* tr = CbTrace()) std::fprintf(tr, "PF\t0\t%ld\t%d\t%d\n", s_tick, car, type);
}

void Pickups_Callback(void* a, void* b) {
    unsigned char* const sa = static_cast<unsigned char*>(a);
    unsigned char* const sb = static_cast<unsigned char*>(b);
    unsigned char* const ent = *reinterpret_cast<unsigned char**>(sa + 0x14);
    std::FILE* const tr = CbTrace();
    const std::uint32_t tb = *reinterpret_cast<const std::uint32_t*>(sb + 0x10);
    const int v = (tb >= 0xff00u && tb <= 0xff07u) ? static_cast<int>(tb - 0xff00u) : -1;
    std::string inHex, saHex, sbHex;
    int heldIn = 0, subMode = 0;
    if (tr) {
        if (ent) inHex = Hex(ent, 0x50);
        saHex = Hex(sa, 0x20); sbHex = Hex(sb, 0x20);
        if (v >= 0 && s_cbHost.held_code) heldIn = s_cbHost.held_code(v);
        subMode = PK_SubMode();
    }
    s_wit.clear();
    reinterpret_cast<cb_fn>(&PK_458920)(a, b);
    ++s_cbN;
    if (!tr) return;
    const int heldOut = (v >= 0 && s_cbHost.held_code) ? s_cbHost.held_code(v) : 0;
    const long slotA = static_cast<long>((reinterpret_cast<std::uintptr_t>(a) - 0x006dccb8u) / 0x8cu);
    const long slotB = static_cast<long>((reinterpret_cast<std::uintptr_t>(b) - 0x006dccb8u) / 0x8cu);
    const long pidx = ent ? static_cast<long>((reinterpret_cast<std::uintptr_t>(ent) - kPool) / kStride) : -1;
    std::fprintf(tr, "PK\t%ld\t%ld\t%ld\t%ld\t%x\t%x\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t-1\t\t%ld\t%s\t%s\t%s\t%s\t\t%s\t%s\t%s\n",
                 s_cbN, s_tick, slotA, slotB, *reinterpret_cast<const std::uint32_t*>(sa + 0x10), tb,
                 *reinterpret_cast<const std::int32_t*>(0x007f0fd0u), subMode,
                 v >= 0 ? std::to_string(heldIn).c_str() : "", v >= 0 ? std::to_string(heldIn).c_str() : "",
                 *reinterpret_cast<const std::int32_t*>(0x0063a5d0u), *reinterpret_cast<const std::int32_t*>(0x0063a5d4u),
                 *reinterpret_cast<const std::int32_t*>(0x00639dc4u), pidx, inHex.c_str(), saHex.c_str(), sbHex.c_str(),
                 ent ? Hex(ent, 0x50).c_str() : "", v >= 0 ? std::to_string(heldOut).c_str() : "",
                 v >= 0 ? std::to_string(heldOut).c_str() : "", JoinWit().c_str());
    std::fflush(tr);
}

void PickupCb_MaybeReplay() {
    const char* inList = std::getenv("MASHED_PKCB_REPLAY");
    if (!inList) return;
    const char* outp = std::getenv("MASHED_PKCB_REPLAY_OUT");
    std::vector<std::vector<std::string>> rows;
    for (const std::string& path : Split(inList, ';')) {
        std::FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) continue;
        std::string all;
        { char buf[1 << 16]; std::size_t n; while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) all.append(buf, n); }
        std::fclose(f);
        std::size_t p = 0;
        while (p < all.size()) {
            std::size_t e = all.find('\n', p); if (e == std::string::npos) e = all.size();
            std::string ln = all.substr(p, e - p); if (!ln.empty() && ln.back() == '\r') ln.pop_back();
            p = e + 1;
            if (ln.compare(0, 3, "PK\t") != 0) continue;
            std::vector<std::string> c = Split(ln, '\t');
            if (c.size() >= 25) rows.push_back(c);
        }
    }
    // columns: 0 kind, 1 n, 2 frame, 3 a, 4 b, 5 type_a, 6 type_b, 7 fd0, 8 ba8c, 9 slot_a8, 10 slot_ac, 11 a5d0,
    //          12 a5d4, 13 9dc4, 14 free, 15 ring, 16 pool_idx, 17 entry_in, 18 slot_a, 19 slot_b, 20 entry_out,
    //          21 ring_out, 22 slot_a8_out, 23 slot_ac_out, 24 witnesses
    EnsureLut();
    BindAll();
    std::vector<unsigned char> savePool(0x7d4);
    std::memcpy(savePool.data(), reinterpret_cast<void*>(kPool), savePool.size());
    const std::uintptr_t gAddr[4] = {0x007f0fd0u, 0x0063a5d0u, 0x0063a5d4u, 0x00639dc4u};
    std::int32_t gSave[4];
    for (int k = 0; k < 4; ++k) gSave[k] = *reinterpret_cast<std::int32_t*>(gAddr[k]);
    int total = 0, exact = 0, entBad = 0, witBad = 0;
    std::map<std::string, int> arms;
    std::vector<int> entOff(kStride, 0);
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    const long g0 = s_grants;
    {
        PcScope pc;
        s_cbReplay = true;
        for (const auto& c : rows) {
            const int idx = std::atoi(c[16].c_str());
            if (idx < 0 || idx >= kMax) continue;
            unsigned char ein[0x50], eout[0x50], sa[0x8c] = {}, sb[0x8c] = {};
            if (!FromHex(c[17], ein, 0x50) || !FromHex(c[20], eout, 0x50) ||
                !FromHex(c[18], sa, 0x20) || !FromHex(c[19], sb, 0x20)) continue;
            ++total;
            unsigned char* const ent = reinterpret_cast<unsigned char*>(kPool + idx * kStride);
            std::memcpy(ent + 4, ein + 4, 0x4c);                         // +0x00 stays the bound fake object
            FrameIdentity(s_frame[idx]);
            std::memcpy(s_frame[idx] + 0x40, ein + 0x38, 12);            // RW frame translation = +0x38 (M2a)
            *reinterpret_cast<unsigned char**>(sa + 0x14) = ent;
            const std::int32_t gv[4] = {std::atoi(c[7].c_str()), std::atoi(c[11].c_str()),
                                        std::atoi(c[12].c_str()), std::atoi(c[13].c_str())};
            for (int k = 0; k < 4; ++k) *reinterpret_cast<std::int32_t*>(gAddr[k]) = gv[k];
            s_rpBa8c = std::atoi(c[8].c_str());
            s_rpA8 = c[9].empty() ? 0u : static_cast<std::uint32_t>(std::strtoul(c[9].c_str(), nullptr, 16));
            s_wit.clear();
            reinterpret_cast<cb_fn>(&PK_458920)(sa, sb);
            // expected call list: the probe's "88c0:free=N" carries the particle count, not part of the call
            std::string exp;
            for (const std::string& w : Split(c[24], '|')) {
                if (w.empty()) continue;
                std::string x = w.compare(0, 5, "88c0:") == 0 ? std::string("88c0") : w;
                if (!exp.empty()) exp += '|';
                exp += x;
            }
            const std::string got = JoinWit();
            bool ok = true;
            if (std::memcmp(ent + 4, eout + 4, 0x4c) != 0) {
                ok = false; ++entBad;
                for (int bb = 4; bb < 0x50; ++bb) if (ent[bb] != eout[bb]) ++entOff[bb];
            }
            if (got != exp) { ok = false; ++witBad; }
            const char* arm = got.find("c010") != std::string::npos ? "collect"
                            : got.find("8570") != std::string::npos ? "bump"
                            : std::memcmp(ein + 4, eout + 4, 0x4c) != 0 ? "vanish" : "no-op";
            ++arms[arm];
            if (ok) ++exact;
            else if (o && total - exact <= 30)
                std::fprintf(o, "MISMATCH n=%s frame=%s arm=%s port=[%s] orig=[%s]\n", c[1].c_str(), c[2].c_str(), arm, got.c_str(), exp.c_str());
        }
        s_cbReplay = false;
    }
    std::memcpy(reinterpret_cast<void*>(kPool), savePool.data(), savePool.size());
    for (int k = 0; k < 4; ++k) *reinterpret_cast<std::int32_t*>(gAddr[k]) = gSave[k];
    if (o) {
        std::fprintf(o, "G-M3-REPLAY rows=%d exact=%d entry_bad=%d calls_bad=%d live_grants_during_replay=%ld pc=%s in=%s\n",
                     total, exact, entBad, witBad, s_grants - g0,
                     std::getenv("MASHED_PICKUPS_PC") ? std::getenv("MASHED_PICKUPS_PC") : "24", inList);
        std::fprintf(o, "arms:");
        for (const auto& kv : arms) std::fprintf(o, " %s=%d", kv.first.c_str(), kv.second);
        std::fprintf(o, "\nentry mismatched offsets:");
        for (std::uintptr_t bb = 0; bb < kStride; ++bb) if (entOff[bb]) std::fprintf(o, " +0x%02x(%d)", static_cast<unsigned>(bb), entOff[bb]);
        std::fprintf(o, "\n");
        std::fclose(o);
    }
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
    {
        PcScope pc;
        reinterpret_cast<void_fn>(&PK_459000)();
    }
    CbTraceTick(dt);
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
