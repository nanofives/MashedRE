// RO-EXE-REPLAY-KNOB (replay oracle 7, REPLAY-ORACLE-DESIGN section 3): MASHED_REPLAY=<replay.json>, default OFF.
// EXE ONLY, ORIG_TOP path only (read in PortTop_WinMain, Match/PortTop_00492370.cpp). No matched source is included
// here: the matched bodies this knob dispatches to have their own TUs (PortTop_00493390.cpp, PortTop_00497310.cpp,
// PortTop_00534870.cpp, MatchS16_004950b0.cpp), so the object build substitutes and byte-checks each of them.
//
// What the knob does, each piece the exe counterpart of the original-side rule in re/frida/js/detorig.js (boot mode):
//  1. CLOCK. The exe's IAT slot of QueryPerformanceCounter points at Replay_QPC. A call from the main thread whose
//     return address is the one call site inside the matched 0x004950b0 body (QpcTimeScaledTo3Mhz) gets the virtual
//     Q, every other call (hand present / limiter, other threads) goes to the real API. All exe callers of 0x004950b0
//     are matched units (0x00428a30, 0x004926c0, 0x00492d30, 0x00493390, 0x00495110), so the scope is "matched units".
//     Before the first tick Q advances by clock.boot_qstep per call (detorig.js cfg.bootQstep), from the first tick on
//     only at the tick. timeGetTime / GetTickCount: their readers 0x005ab040 / 0x004a1570 have no exe body (hooks.csv
//     port none), so no slot is wrapped; T(tick) is kept and reported only.
//  2. TICK. The matched 0x00493480 calls 0x00493390 through PortTop_Replay_00493390 (MatchS15_00493480.cpp): Q += qtick,
//     T += ttick, tick += 1, then the matched body. Same order as detorig.js's CModule onEnter on FUN_00493390.
//     The knob turns MASHED_OT_TIMER on (0x00493480 is a no-op without it).
//  3. WARP at tick warp.after_ticks (from process start), run inside the first wrapped clock read of that tick after
//     the value is written (the order of detorig.js qpcStub -> onDue): refused unless DAT_00771968 == 1 (the menu);
//     selection globals 0x0063ba7c / 0x0067f17c track, 0x0067e9fc mode, 0x007f0fd0 rule, 0x0067ea98 car,
//     0x0067ea64 team; CarSlotStateSet 0x0040e480 (k, 1/2/2/2 by cars); RNG pins; clock rebased to
//     q0 + warp.clock * qtick (detorig.js cfg.warpClock); timer pin; normalisation PIN rows (warp_normalize.tsv);
//     DAT_00771968 := warp.phase.
//  4. RNG PINS at the warp and the race anchor: srand(rng.crt_holdrand) on the main thread (UCRT rand: per-thread
//     state, LCG 0x343fd / 0x269ec3 as CRT _rand 0x005c229b); the 0x00534870 port's own ring reseeded by the
//     FUN_00534990 rule (seed 0x9a319039 at 0x00534a09, LCG 0x41c64e6d / 0x3039 over 31 dwords, rp = ring + 3,
//     wp = ring, then 0x136 draws at 0x00534a46).
//  5. RACE ANCHOR: the first wrapped clock read after the warp that sees DAT_00771968 == 3: pins again, normalisation
//     at the anchor, input rows start (keyed by tick after the anchor, DESIGN section 2).
//  6. INPUT. PortTop_Replay_00497310(player, control) calls the matched 0x00497310 and, once the warp ran, returns the
//     script's value for player 0 (detorig.js CModule repl). The raw poll (0x004967e0 stand-in = the hand keyboard
//     poll) reports no device: Replay_RawPollNoDevice() makes the stand-in skip it.
//  7. SELF-TEST: init checks (slot and call site found, RW ring port against an independent model, srand/rand against
//     the LCG model, the 0x00497310 dispatcher against the body and an independent model) and one runtime check per
//     tick (DAT_007f1030 = the value the matched 0x00493390 must store from the modelled virtual clock). Summary line
//     "[REPLAY] self-test PASS|FAIL (f of n checks failed)" at WinMain exit.
// RVA-LINT: ORIGTOP-ONLY

#ifdef MASHED_STANDALONE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
unsigned int  __cdecl QpcTimeScaledTo3Mhz(void);                // 0x004950b0, Match/MatchS16_004950b0.cpp
void          __cdecl PortTop_Bound_00493390(void);              // 0x00493390, Match/PortTop_00493390.cpp
unsigned char __cdecl PortTop_Bound_00497310(int player, int action);   // 0x00497310, Match/PortTop_00497310.cpp
unsigned int  __cdecl PortTop_Bound_00534870(void);              // 0x00534870, Match/PortTop_00534870.cpp
void          __cdecl CarSlotStateSet(int slot, unsigned int state);    // 0x0040e480, Match/MatchS02_0040e480.cpp
unsigned char __cdecl PortTop_Replay_00497310(int player, int action);  // below
}

namespace {

void RLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] [REPLAY] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

// ---------------------------------------------------------------- minimal JSON reader (replay.json)
struct JVal {
    enum T { NUL, BOOL, NUM, STR, ARR, OBJ } t = NUL;
    long long i = 0;
    std::string s;
    std::vector<JVal> a;
    std::vector<std::pair<std::string, JVal>> o;
    const JVal* get(const char* k) const {
        if (t != OBJ) return nullptr;
        for (const auto& kv : o) if (kv.first == k) return &kv.second;
        return nullptr;
    }
};
struct JParse {
    const char* p; const char* e; bool ok = true;
    void ws() { while (p < e && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p; }
    bool str(std::string& out) {
        if (p >= e || *p != '"') return false;
        ++p;
        while (p < e && *p != '"') {
            if (*p == '\\' && p + 1 < e) { ++p; out += (*p == 'n') ? '\n' : (*p == 't') ? '\t' : *p; ++p; }
            else out += *p++;
        }
        if (p >= e) return false;
        ++p;
        return true;
    }
    bool val(JVal& v) {
        ws();
        if (p >= e) return false;
        if (*p == '{') {
            v.t = JVal::OBJ; ++p; ws();
            if (p < e && *p == '}') { ++p; return true; }
            for (;;) {
                ws(); std::string k; if (!str(k)) return false;
                ws(); if (p >= e || *p != ':') return false; ++p;
                JVal c; if (!val(c)) return false;
                v.o.emplace_back(k, c);
                ws(); if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == '}') { ++p; return true; }
                return false;
            }
        }
        if (*p == '[') {
            v.t = JVal::ARR; ++p; ws();
            if (p < e && *p == ']') { ++p; return true; }
            for (;;) {
                JVal c; if (!val(c)) return false;
                v.a.push_back(c);
                ws(); if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == ']') { ++p; return true; }
                return false;
            }
        }
        if (*p == '"') { v.t = JVal::STR; return str(v.s); }
        if (!std::strncmp(p, "true", 4)) { v.t = JVal::BOOL; v.i = 1; p += 4; return true; }
        if (!std::strncmp(p, "false", 5)) { v.t = JVal::BOOL; v.i = 0; p += 5; return true; }
        if (!std::strncmp(p, "null", 4)) { v.t = JVal::NUL; p += 4; return true; }
        char* end = nullptr;
        v.t = JVal::NUM; v.i = std::strtoll(p, &end, 10);
        if (end == p) return false;
        p = end;
        return true;
    }
};
// a number, or a string "0x..." / decimal
bool JNum(const JVal* v, unsigned long long& out) {
    if (!v) return false;
    if (v->t == JVal::NUM || v->t == JVal::BOOL) { out = static_cast<unsigned long long>(v->i); return true; }
    if (v->t == JVal::STR) { out = std::strtoull(v->s.c_str(), nullptr, 0); return true; }
    return false;
}
unsigned long long JGet(const JVal* obj, const char* k, unsigned long long def) {
    unsigned long long x;
    return (obj && JNum(obj->get(k), x)) ? x : def;
}

// ---------------------------------------------------------------- config and state
struct Cfg {
    int track = 0, mode = 10, rule = 0, car = 0, team = 0, cars = 4;   // DESIGN section 2 scenario
    unsigned warpTick = 0;            // 0 = no warp
    unsigned warpClock = 0;           // 0 = no rebase
    int warpPhase = 2;                // -1 = do not write DAT_00771968
    unsigned long long q0 = 0x40000000ull;   // detorig.py --q0 default
    unsigned qtick = 166667, ttick = 17;     // detorig.py --qtick / --ttick defaults
    unsigned bootQstep = 15000, bootStep = 1;  // detorig.py --boot-qstep / --boot-step defaults
    unsigned t0 = 0x01000000;         // detorig.py cfg t0
    unsigned holdrand = 0x12345678;   // detorig.py cfg holdrand
    unsigned rwSeed = 0x9a319039, rwDraws = 0x136;
    struct In { unsigned dt; int player, control, value; };
    std::vector<In> input;
    struct Pin { unsigned va, width, value; };
    std::vector<Pin> norm;
    std::string normFile, normAt = "both";
    bool rawNoDevice = true;
};
Cfg g_c;
int  g_on = 0;
DWORD g_mainTid = 0;
using QpcFn = BOOL(WINAPI*)(LARGE_INTEGER*);
QpcFn g_realQpc = nullptr;
void** g_qpcSlot = nullptr;
const unsigned char* g_qpcRet = nullptr;     // return address of the IAT call inside QpcTimeScaledTo3Mhz
unsigned long long g_Q = 0;
unsigned g_T = 0;
unsigned g_tick = 0, g_bootCalls = 0, g_wrapCalls = 0, g_passCalls = 0;
bool g_pendingWarp = false, g_warped = false, g_warpRefused = false, g_anchored = false;
unsigned g_warpAt = 0, g_anchorTick = 0, g_warpPhaseBefore = 0;
int  g_active = 0;
int  g_ctl[64] = {};
unsigned g_inputApplied = 0, g_pollSkipped = 0, g_rwCalls = 0, g_normChanged[2] = {};
unsigned g_checks = 0, g_fails = 0, g_tickChecks = 0, g_tickFails = 0;

struct St534870 { unsigned int* base; unsigned int* rp; unsigned int* wp; unsigned int* end; };
unsigned int g_ring[31];
St534870 g_rw = { g_ring, g_ring + 3, g_ring, g_ring + 31 };

unsigned& MemU(unsigned a) { return *reinterpret_cast<unsigned*>(static_cast<std::uintptr_t>(a)); }
int&      MemI(unsigned a) { return *reinterpret_cast<int*>(static_cast<std::uintptr_t>(a)); }

void Check(bool ok, const char* what, unsigned long long got, unsigned long long want) {
    ++g_checks;
    if (!ok && ++g_fails <= 12)
        RLog("self-test FAIL %s: got 0x%llX expected 0x%llX (tick %u)", what, got, want, g_tick);
}

// Self-test model (not a body): the matched clock source's arithmetic on a given Q, (Q * 3000000) / QPF in signed
// __int64, low dword (re/match/s16/004950b0.c). Also the timer pin's rule (detorig.js pinTimer).
unsigned Scale3Mhz(unsigned long long q) {
    const long long qpf = *reinterpret_cast<const long long*>(0x00771e70u);
    if (!qpf) return 0;
    return static_cast<unsigned>((static_cast<long long>(q) * 3000000LL) / qpf);
}

// ---------------------------------------------------------------- RNG pins
void SeedRwRing() {
    unsigned v = g_c.rwSeed;
    g_ring[0] = v;
    for (int i = 1; i < 31; ++i) { v = v * 0x41c64e6du + 0x3039u; g_ring[i] = v; }
    g_rw.base = g_ring; g_rw.rp = g_ring + 3; g_rw.wp = g_ring; g_rw.end = g_ring + 31;
}
}  // namespace

extern "C" unsigned int PortTop_Replay_00534870(void) {
    volatile int& off = *reinterpret_cast<volatile int*>(0x007dc578u);   // RW RNG plugin offset
    const int base = *reinterpret_cast<volatile int*>(0x007d3ff8u);       // RwGlobals pointer
    const int saved = off;
    off = static_cast<int>(reinterpret_cast<std::uintptr_t>(&g_rw)) - base;
    const unsigned r = PortTop_Bound_00534870();
    off = saved;
    ++g_rwCalls;
    return r;
}

namespace {
void PinRngs(const char* where) {
    std::srand(g_c.holdrand);
    SeedRwRing();
    for (unsigned i = 0; i < g_c.rwDraws; ++i) PortTop_Replay_00534870();
    RLog("pins at %s (tick %u): srand(0x%08X) on the main thread, RW ring seeded 0x%08X + %u draws, cursors rp %d wp %d",
         where, g_tick, g_c.holdrand, g_c.rwSeed, g_c.rwDraws, static_cast<int>(g_rw.rp - g_ring),
         static_cast<int>(g_rw.wp - g_ring));
}
void PinTimer() {
    const unsigned v = Scale3Mhz(g_Q);
    MemU(0x007f1030u) = v; MemU(0x007f102cu) = v; MemU(0x00771984u) = 0;
    MemU(0x00771998u) = v / 3000u; MemU(0x0077199cu) = v / 3000u;
    RLog("timer pin: Q 0x%llX -> %u at 0x007f1030 / 0x007f102c, 0x00771984 = 0, 0x00771998 / 0x0077199c = %u", g_Q, v, v / 3000u);
}
void PinNorm(const char* where, int idx) {
    if (g_c.norm.empty() || !(g_c.normAt == "both" || g_c.normAt == where)) return;
    unsigned changed = 0;
    for (const auto& p : g_c.norm) {
        void* a = reinterpret_cast<void*>(static_cast<std::uintptr_t>(p.va));
        unsigned old;
        if (p.width == 1) { old = *static_cast<unsigned char*>(a); *static_cast<unsigned char*>(a) = static_cast<unsigned char>(p.value); }
        else if (p.width == 2) { old = *static_cast<unsigned short*>(a); *static_cast<unsigned short*>(a) = static_cast<unsigned short>(p.value); }
        else { old = *static_cast<unsigned*>(a); *static_cast<unsigned*>(a) = p.value; }
        if (old != p.value) ++changed;
    }
    g_normChanged[idx] = changed;
    RLog("normalisation at %s: %u PIN dwords written, %u changed", where, static_cast<unsigned>(g_c.norm.size()), changed);
}

void DoWarp() {
    const unsigned ph = *reinterpret_cast<volatile unsigned char*>(0x00771968u);
    g_warpPhaseBefore = ph;
    if (ph != 1) {
        g_warpRefused = true;
        RLog("warp REFUSED at tick %u: DAT_00771968 = %u, not the menu (1)", g_tick, ph);
        return;
    }
    const Cfg& c = g_c;
    MemI(0x0063ba7cu) = c.track; MemI(0x0067f17cu) = c.track; MemI(0x0067e9fcu) = c.mode;
    MemI(0x007f0fd0u) = c.rule;  MemI(0x0067ea98u) = c.car;   MemI(0x0067ea64u) = c.team;
    for (int k = 0; k < 4; ++k) CarSlotStateSet(k, k == 0 ? 1u : (k < c.cars ? 2u : 0u));
    PinRngs("warp");
    if (c.warpClock) {
        g_Q = c.q0 + static_cast<unsigned long long>(c.warpClock) * c.qtick;
        g_T = c.t0 + c.warpClock * c.ttick;
    }
    PinTimer();
    PinNorm("warp", 0);
    if (c.warpPhase >= 0) *reinterpret_cast<volatile unsigned char*>(0x00771968u) = static_cast<unsigned char>(c.warpPhase);
    g_warped = true; g_warpAt = g_tick; g_active = 1;
    RLog("warp at tick %u: track %d mode %d rule %d car %d team %d cars %d, CarSlotStateSet x4, DAT_00771968 %u -> %d",
         g_tick, c.track, c.mode, c.rule, c.car, c.team, c.cars, ph, c.warpPhase);
}
void CheckAnchor() {
    if (*reinterpret_cast<volatile unsigned char*>(0x00771968u) != 3) return;
    g_anchored = true; g_anchorTick = g_tick;
    PinRngs("anchor");
    PinNorm("anchor", 1);
    // rows at anchor + 0 apply now (detorig.js schedules dn = 0 at the anchor's own N); later rows at the tick start
    for (const auto& in : g_c.input)
        if (in.player == 0 && in.dt == 0 && in.control >= 0 && in.control < 64) { g_ctl[in.control] = in.value; ++g_inputApplied; }
    RLog("race anchor at tick %u (warp + %u), %u input rows at anchor + 0", g_tick, g_tick - g_warpAt, g_inputApplied);
}

BOOL WINAPI Replay_QPC(LARGE_INTEGER* p) {
    if (_ReturnAddress() == g_qpcRet && GetCurrentThreadId() == g_mainTid) {
        p->QuadPart = static_cast<LONGLONG>(g_Q);
        ++g_wrapCalls;
        if (g_tick == 0) { g_Q += g_c.bootQstep; g_T += g_c.bootStep; ++g_bootCalls; }
        if (g_pendingWarp) { g_pendingWarp = false; DoWarp(); }
        else if (g_warped && !g_anchored) CheckAnchor();
        return TRUE;
    }
    ++g_passCalls;
    return g_realQpc(p);
}

// the IAT call inside the 0x004950b0 body: FF 15 <slot>; the return address is the byte after it
bool FindQpcCallSite() {
    const unsigned char* f = reinterpret_cast<const unsigned char*>(&QpcTimeScaledTo3Mhz);
    if (f[0] == 0xE9) f = f + 5 + *reinterpret_cast<const int*>(f + 1);   // incremental-link thunk
    for (int i = 0; i < 0x80; ++i) {
        if (f[i] == 0xFF && f[i + 1] == 0x15) {
            g_qpcSlot = *reinterpret_cast<void** const*>(f + i + 2);
            g_qpcRet = f + i + 6;
            RLog("0x004950b0 body at %p: IAT call at +0x%X, slot %p, return address %p", static_cast<const void*>(f), i,
                 static_cast<void*>(g_qpcSlot), static_cast<const void*>(g_qpcRet));
            return true;
        }
    }
    return false;
}

bool LoadNorm(const std::string& path) {
    std::FILE* f = std::fopen(path.c_str(), "r");
    if (!f) return false;
    char line[2048];
    while (std::fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || !std::strncmp(line, "va\t", 3)) continue;
        std::vector<std::string> fld;
        std::string cur;
        for (const char* q = line; *q && *q != '\n' && *q != '\r'; ++q) {
            if (*q == '\t') { fld.push_back(cur); cur.clear(); } else cur += *q;
        }
        fld.push_back(cur);
        if (fld.size() < 6 || fld[4] != "PIN") continue;
        const unsigned va = std::strtoul(fld[0].c_str(), nullptr, 16), cnt = std::strtoul(fld[1].c_str(), nullptr, 10);
        const unsigned stride = std::strtoul(fld[2].c_str(), nullptr, 16), w = std::strtoul(fld[3].c_str(), nullptr, 10);
        const unsigned v = std::strtoul(fld[5].c_str(), nullptr, 16);
        for (unsigned k = 0; k < cnt; ++k) g_c.norm.push_back({va + k * stride, w, v});
    }
    std::fclose(f);
    return true;
}

bool LoadCfg(const char* path) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) { RLog("cannot open %s", path); return false; }
    std::string txt;
    char buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) txt.append(buf, n);
    std::fclose(f);
    JParse jp{txt.data(), txt.data() + txt.size()};
    JVal root;
    if (!jp.val(root) || root.t != JVal::OBJ) { RLog("%s: JSON parse error near offset %d", path, static_cast<int>(jp.p - txt.data())); return false; }
    const JVal* fmt = root.get("format");
    if (!fmt || fmt->s != "mashed-replay-1") { RLog("%s: format is not mashed-replay-1", path); return false; }
    Cfg& c = g_c;
    if (const JVal* s = root.get("scenario")) {
        c.track = static_cast<int>(JGet(s, "track", c.track)); c.mode = static_cast<int>(JGet(s, "mode", c.mode));
        c.rule = static_cast<int>(JGet(s, "rule", c.rule));    c.car = static_cast<int>(JGet(s, "car", c.car));
        c.team = static_cast<int>(JGet(s, "team", c.team));    c.cars = static_cast<int>(JGet(s, "cars", c.cars));
    }
    if (const JVal* w = root.get("warp")) {
        c.warpTick = static_cast<unsigned>(JGet(w, "after_ticks", 0));
        c.warpClock = static_cast<unsigned>(JGet(w, "clock", c.warpTick));
        const JVal* ph = w->get("phase");
        c.warpPhase = (ph && ph->t == JVal::NUL) ? -1 : static_cast<int>(JGet(w, "phase", 2));
    }
    if (const JVal* k = root.get("clock")) {
        c.q0 = JGet(k, "q0", c.q0); c.qtick = static_cast<unsigned>(JGet(k, "qtick", c.qtick));
        c.t0 = static_cast<unsigned>(JGet(k, "t0", c.t0)); c.ttick = static_cast<unsigned>(JGet(k, "ttick", c.ttick));
        c.bootQstep = static_cast<unsigned>(JGet(k, "boot_qstep", c.bootQstep));
        c.bootStep = static_cast<unsigned>(JGet(k, "boot_step", c.bootStep));
    }
    if (const JVal* r = root.get("rng")) {
        c.holdrand = static_cast<unsigned>(JGet(r, "crt_holdrand", c.holdrand));
        c.rwSeed = static_cast<unsigned>(JGet(r, "rw_seed", c.rwSeed));
        c.rwDraws = static_cast<unsigned>(JGet(r, "rw_draws", c.rwDraws));
    }
    if (const JVal* in = root.get("input")) {
        for (const JVal& row : in->a) {
            unsigned long long t, p, ctl, v;
            if (row.t != JVal::ARR || row.a.size() < 4 || !JNum(&row.a[0], t) || !JNum(&row.a[1], p) || !JNum(&row.a[2], ctl) ||
                !JNum(&row.a[3], v)) { RLog("%s: bad input row skipped", path); continue; }
            c.input.push_back({static_cast<unsigned>(t), static_cast<int>(p), static_cast<int>(ctl), static_cast<int>(v)});
        }
    }
    if (const JVal* nm = root.get("normalize")) {
        if (nm->t == JVal::STR) c.normFile = nm->s;
        else if (nm->t == JVal::OBJ) {
            if (const JVal* fp = nm->get("file")) c.normFile = fp->s;
            if (const JVal* at = nm->get("at")) c.normAt = at->s;
        }
    }
    if (const JVal* rp = root.get("raw_poll")) c.rawNoDevice = !(rp->t == JVal::STR && rp->s == "device");
    if (!c.normFile.empty() && !LoadNorm(c.normFile)) RLog("normalisation file %s not readable: no PIN rows", c.normFile.c_str());
    return true;
}

// independent model of 0x00534870 on its own copy (re/match/rwl03/00534870.c, written from the source, not compiled from it)
struct RingModel {
    unsigned r[31]; int rp, wp;
    void seed(unsigned v) { r[0] = v; for (int i = 1; i < 31; ++i) { v = v * 0x41c64e6du + 0x3039u; r[i] = v; } rp = 3; wp = 0; }
    unsigned draw() {
        r[rp] += r[wp];
        const unsigned out = r[rp] >> 1;
        ++rp;
        if (rp >= 31) { rp = 0; ++wp; return out; }
        ++wp; if (wp >= 31) wp = 0;
        return out;
    }
};
// independent model of 0x00497310 for the record types the init state can hold (re/match/s16/00497310.c)
unsigned char InputModel(int player, int action) {
    const unsigned char* rec = reinterpret_cast<const unsigned char*>(static_cast<std::uintptr_t>(0x007e95c0u + 0x200u * player));
    const int type = *reinterpret_cast<const int*>(rec + 0x13c);
    const unsigned b = *reinterpret_cast<const unsigned*>(rec + 0x108 + 4 * action);
    if (type == 2) {
        const unsigned char* kb = reinterpret_cast<const unsigned char*>(0x0077313cu);
        return (kb[b >> 3] & (1u << (b & 7))) ? 0xff : 0;
    }
    if (type != 1) return 0;
    return 0xfe;   // joystick records are not modelled here: the caller skips the compare
}

void InitSelfTest() {
    // RW ring port (matched body + own ring) against the independent model, 0x136 + 512 draws from the seed
    const int off0 = *reinterpret_cast<volatile int*>(0x007dc578u);
    SeedRwRing();
    RingModel m; m.seed(g_c.rwSeed);
    unsigned bad = 0, firstBad = 0;
    for (unsigned i = 0; i < 0x136u + 512u; ++i) {
        const unsigned a = PortTop_Replay_00534870(), b = m.draw();
        if (a != b && !bad++) firstBad = i;
    }
    Check(bad == 0, "RW ring port vs model (draw index of first mismatch)", firstBad, 0);
    Check(g_rw.rp - g_ring == m.rp && g_rw.wp - g_ring == m.wp, "RW ring cursors", static_cast<unsigned>(g_rw.rp - g_ring),
          static_cast<unsigned>(m.rp));
    Check(*reinterpret_cast<volatile int*>(0x007dc578u) == off0, "RW plugin offset 0x007dc578 restored after the draws",
          static_cast<unsigned>(*reinterpret_cast<volatile int*>(0x007dc578u)), static_cast<unsigned>(off0));
    // srand / rand: UCRT rand on the main thread against the CRT _rand LCG (0x005c22a3 / 0x005c22a9)
    std::srand(g_c.holdrand);
    unsigned st = g_c.holdrand;
    for (int i = 0; i < 8; ++i) {
        st = st * 0x343fdu + 0x269ec3u;
        const unsigned want = (st >> 16) & 0x7fffu;
        const unsigned got = static_cast<unsigned>(std::rand());
        Check(got == want, "srand/rand vs CRT LCG", got, want);
    }
    std::srand(1);   // the CRT's initial state: the boot is not seeded by the self-test
    // 0x00497310 dispatcher: body vs model (keyboard / unbound records), script on player 0 only while active
    for (int p = 0; p < 4; ++p)
        for (int a = 0; a < 13; ++a) {
            const unsigned char want = InputModel(p, a);
            if (want == 0xfe) continue;
            Check(PortTop_Bound_00497310(p, a) == want, "0x00497310 body vs model", PortTop_Bound_00497310(p, a), want);
        }
    g_active = 1; g_ctl[5] = 0x7b;
    Check(PortTop_Replay_00497310(0, 5) == 0x7b, "0x00497310 dispatcher returns the script for player 0", PortTop_Replay_00497310(0, 5), 0x7b);
    Check(PortTop_Replay_00497310(1, 5) == PortTop_Bound_00497310(1, 5), "0x00497310 dispatcher leaves player 1 to the body",
          PortTop_Replay_00497310(1, 5), PortTop_Bound_00497310(1, 5));
    g_active = 0; g_ctl[5] = 0;
    Check(PortTop_Replay_00497310(0, 5) == PortTop_Bound_00497310(0, 5), "0x00497310 dispatcher inactive = body",
          PortTop_Replay_00497310(0, 5), PortTop_Bound_00497310(0, 5));
    RLog("init self-test: %u checks, %u failed (RW ring %u draws, rand 8, input dispatcher)", g_checks, g_fails, 0x136u + 512u);
    SeedRwRing();
    g_rwCalls = 0;
}
}  // namespace

extern "C" {

unsigned char PortTop_Replay_00497310(int player, int action) {
    const unsigned char r = PortTop_Bound_00497310(player, action);
    if (g_active && player == 0 && action >= 0 && action < 64) return static_cast<unsigned char>(g_ctl[action]);
    return r;
}

int Replay_On(void) { return g_on; }
unsigned Replay_WarpCount(void) { return g_warped ? 1u : 0u; }
int Replay_RawPollNoDevice(void) {
    if (g_on && g_c.rawNoDevice) { ++g_pollSkipped; return 1; }
    return 0;
}

// the tick boundary: the matched 0x00493480 calls this in place of 0x00493390 (MatchS15_00493480.cpp, exe)
void PortTop_Replay_00493390(void) {
    if (!g_on) { PortTop_Bound_00493390(); return; }
    g_Q += g_c.qtick; g_T += g_c.ttick; ++g_tick;
    if (g_c.warpTick && g_tick == g_c.warpTick && !g_warped && !g_warpRefused) g_pendingWarp = true;
    if (g_anchored)
        for (const auto& in : g_c.input)
            if (in.player == 0 && in.dt == g_tick - g_anchorTick && in.control >= 0 && in.control < 64) {
                g_ctl[in.control] = in.value; ++g_inputApplied;
            }
    // the modelled clock value the body reads this tick (independent of g_Q): boot calls, ticks, rebase at the warp
    const unsigned long long q = (g_warped && g_c.warpClock && g_tick > g_warpAt)
        ? g_c.q0 + static_cast<unsigned long long>(g_c.warpClock) * g_c.qtick + static_cast<unsigned long long>(g_tick - g_warpAt) * g_c.qtick
        : g_c.q0 + static_cast<unsigned long long>(g_bootCalls) * g_c.bootQstep + static_cast<unsigned long long>(g_tick) * g_c.qtick;
    PortTop_Bound_00493390();
    const unsigned want = Scale3Mhz(q), got = MemU(0x007f1030u);
    ++g_tickChecks;
    if (got != want) ++g_tickFails;
    Check(got == want, "0x00493390 stored DAT_007f1030 from the virtual clock", got, want);
    if (g_tick == 1 || (g_tick % 120) == 0)
        RLog("tick %u: Q 0x%llX, DAT_007f1030 %u, phase %u, wrapped QPC reads %u (boot %u), passed %u", g_tick, g_Q, got,
             *reinterpret_cast<volatile unsigned char*>(0x00771968u), g_wrapCalls, g_bootCalls, g_passCalls);
}

// WinMain entry (PortTop_WinMain): read the knob, load the script, wrap the clock slot, run the init self-test
void Replay_Init(void) {
    const char* const v = std::getenv("MASHED_REPLAY");
    if (!v || !v[0] || (v[0] == '0' && !v[1])) { g_on = 0; return; }
    if (!LoadCfg(v)) { RLog("MASHED_REPLAY=%s: not loaded, knob stays off", v); g_on = 0; return; }
    g_mainTid = GetCurrentThreadId();
    g_Q = g_c.q0; g_T = g_c.t0;
    if (!FindQpcCallSite()) { RLog("no IAT call found in the 0x004950b0 body: knob stays off"); g_on = 0; return; }
    DWORD old;
    if (!VirtualProtect(g_qpcSlot, sizeof(void*), PAGE_READWRITE, &old)) { RLog("VirtualProtect failed: knob stays off"); return; }
    g_realQpc = reinterpret_cast<QpcFn>(*g_qpcSlot);
    *g_qpcSlot = reinterpret_cast<void*>(&Replay_QPC);
    VirtualProtect(g_qpcSlot, sizeof(void*), old, &old);
    const void* k32 = reinterpret_cast<const void*>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "QueryPerformanceCounter"));
    RLog("MASHED_REPLAY=%s on: main thread %lu, QPC slot %p was %p (kernel32 export %p), now Replay_QPC", v, g_mainTid,
         static_cast<void*>(g_qpcSlot), reinterpret_cast<void*>(g_realQpc), k32);
    RLog("scenario track %d mode %d rule %d car %d team %d cars %d; warp tick %u clock %u phase %d; q0 0x%llX qtick %u "
         "boot_qstep %u t0 0x%X ttick %u; holdrand 0x%08X rw seed 0x%08X draws %u; %u input rows; normalisation %s "
         "(%u PIN dwords, at %s); raw poll %s; timeGetTime / GetTickCount: no matched exe reader, not wrapped",
         g_c.track, g_c.mode, g_c.rule, g_c.car, g_c.team, g_c.cars, g_c.warpTick, g_c.warpClock, g_c.warpPhase, g_c.q0,
         g_c.qtick, g_c.bootQstep, g_c.t0, g_c.ttick, g_c.holdrand, g_c.rwSeed, g_c.rwDraws,
         static_cast<unsigned>(g_c.input.size()), g_c.normFile.empty() ? "none" : g_c.normFile.c_str(),
         static_cast<unsigned>(g_c.norm.size()), g_c.normAt.c_str(), g_c.rawNoDevice ? "no device" : "device");
    g_on = 1;
    InitSelfTest();
}

void Replay_Summary(unsigned timerCalls480) {
    if (!g_on) return;
    if (timerCalls480 != g_tick) Check(false, "ticks = 0x00493480 calls", g_tick, timerCalls480);
    RLog("self-test %s (%u of %u checks failed): ticks %u (0x00493480 calls %u), per-tick clock checks %u failed %u, "
         "wrapped QPC reads %u (boot %u), passed through %u, T %u; warp %s at tick %u (phase before %u), anchor %s at "
         "tick %u, normalisation changed %u / %u, input rows applied %u, raw polls skipped %u, RW draws %u; bodies "
         "0x004950b0 %p, 0x00493390 %p, 0x00497310 %p, 0x00534870 %p",
         g_fails ? "FAIL" : "PASS", g_fails, g_checks, g_tick, timerCalls480, g_tickChecks, g_tickFails, g_wrapCalls,
         g_bootCalls, g_passCalls, g_T, g_warped ? "ran" : (g_warpRefused ? "REFUSED" : "not reached"), g_warpAt,
         g_warpPhaseBefore, g_anchored ? "reached" : "not reached", g_anchorTick, g_normChanged[0], g_normChanged[1],
         g_inputApplied, g_pollSkipped, g_rwCalls, reinterpret_cast<void*>(&QpcTimeScaledTo3Mhz),
         reinterpret_cast<void*>(&PortTop_Bound_00493390), reinterpret_cast<void*>(&PortTop_Bound_00497310),
         reinterpret_cast<void*>(&PortTop_Bound_00534870));
}

}  // extern "C"

#endif  // MASHED_STANDALONE
