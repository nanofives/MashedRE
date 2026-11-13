// RO-ORIG-ASI (2026-11-13): replay mode of mashed_re_dev.asi on the original MASHED.exe. Knob MASHED_REPLAY_ASI=<cfg.txt>,
// default OFF (dll_main.cpp). A port of the boot mode of re/frida/js/detorig.js (RO-MENU-REPLAY) with no Frida in the
// process: same virtual clock, tick listener, warp, RNG / timer / boot-counter pins, player-0 script, per-tick block
// hashes and checkpoints, same output files (re/frida/detorig.py --asi builds meta.json from the event log written here).
//
// Install moment = detorig.js cfg.boot: the PE entry point (0x004a4bb7), on the main thread, after the loader has bound
// the IAT and run every DllMain. ReplayAsi_Init (in this DLL's DllMain) only patches a JMP there.
//
// Hook sites (bytes read from the anchored MASHED.exe, capstone):
//   0x004a4bb7 entry  : push 0x60 (2) ; push 0x5d1290 (5)                 -> 7 bytes restored by the entry stub
//   0x00493390 tick   : push ecx ; push edi ; call 0x004950b0             -> listener, then the 7 bytes re-executed
//   0x00497310 input  : push ecx ; mov eax,[esp+8]                        -> trampoline = these 5 bytes + jmp 0x00497315
//   IAT 0x005cc2b4 timeGetTime, 0x005cc0c8 GetTickCount, 0x005cc0c4 QueryPerformanceCounter
//
// Config (written by detorig.py --asi), one "key values..." per line:
//   out DIR | t0 | q0 | qtick | ttick | bootstep | bootqstep | cap | block | warptick | holdrand | negctl | negtick
//   scen track mode rule car team cars | raw LO HI | input DN CTL VAL | ckpt DN | norm VA WIDTH VALUE | normat warp|anchor|both
//   witness VA | loadsleep MS
#include <windows.h>
#include <intrin.h>
#include <cstdio>
#include <cstdarg>
#include <share.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------- clock block (detorig.js D, same roles)
extern "C" {
volatile uint32_t g_rT, g_rStep, g_rDue = 0xffffffffu, g_rMain, g_rQlo, g_rQhi, g_rQstep;
volatile uint32_t g_rTgt, g_rQpc, g_rFwd, g_rN;
void* g_rRealTgtA;   // timeGetTime
void* g_rRealTgtB;   // GetTickCount
void* g_rRealQpc;
void* g_rFn4950b0;
void* g_rRet493397;
void* g_rRet497315;
void* g_rEntry;
}


namespace {

const uint32_t IMG = 0x00400000;
const uint32_t DATA_LO = 0x005ea000, DATA_HI = 0x00915008;   // .data + _rwdseg (detorig.js DATA_LO / DATA_HI)
uint32_t g_base = IMG;
inline uint8_t* ga(uint32_t va) { return reinterpret_cast<uint8_t*>(g_base + (va - IMG)); }
inline uint32_t& U32(uint32_t va) { return *reinterpret_cast<uint32_t*>(ga(va)); }

struct Cfg {
    char out[MAX_PATH] = {0};
    uint32_t t0 = 0x01000000, q0 = 0xC0000000, qtick = 166667, ttick = 17, bootstep = 1, bootqstep = 15000;
    uint32_t cap = 4000, block = 1024, warptick = 600, holdrand = 0x12345678, negctl = 0, negtick = 0, loadsleep = 0;
    int scen[6] = {0, 10, 0, 0, 0, 4};
    std::vector<std::pair<uint32_t, uint32_t>> raw;
    struct In { uint32_t dn; int c, v; };
    std::vector<In> input;
    std::vector<uint32_t> ckpts;
    struct Norm { uint32_t va, w, v; };
    std::vector<Norm> norm;
    char normat[16] = "both";
    std::vector<uint32_t> witness;
} C;

FILE* g_ev = nullptr;
void Ev(const char* fmt, ...) {
    if (!g_ev) return;
    va_list ap; va_start(ap, fmt); vfprintf(g_ev, fmt, ap); va_end(ap);
    fputc('\n', g_ev); fflush(g_ev);
}
void OutPath(char* dst, const char* name) { _snprintf_s(dst, MAX_PATH, _TRUNCATE, "%s/%s", C.out, name); }

bool LoadCfg(const char* path) {
    FILE* f = nullptr;
    if (fopen_s(&f, path, "r") || !f) return false;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        char k[32] = {0};
        if (sscanf_s(line, "%31s", k, (unsigned)sizeof k) != 1) continue;
        const char* v = line + strlen(k);
        unsigned a = 0, b = 0, c = 0;
        if (!strcmp(k, "out")) { sscanf_s(v, "%259s", C.out, (unsigned)sizeof C.out); }
        else if (!strcmp(k, "normat")) { sscanf_s(v, "%15s", C.normat, (unsigned)sizeof C.normat); }
        else if (!strcmp(k, "scen")) { sscanf_s(v, "%i %i %i %i %i %i", &C.scen[0], &C.scen[1], &C.scen[2], &C.scen[3], &C.scen[4], &C.scen[5]); }
        else if (!strcmp(k, "raw")) { if (sscanf_s(v, "%i %i", &a, &b) == 2) C.raw.push_back({a, b}); }
        else if (!strcmp(k, "input")) { int cc = 0, vv = 0; if (sscanf_s(v, "%i %i %i", &a, &cc, &vv) == 3) C.input.push_back({a, cc, vv}); }
        else if (!strcmp(k, "ckpt")) { if (sscanf_s(v, "%i", &a) == 1) C.ckpts.push_back(a); }
        else if (!strcmp(k, "norm")) { if (sscanf_s(v, "%i %i %i", &a, &b, &c) == 3) C.norm.push_back({a, b, c}); }
        else if (!strcmp(k, "witness")) { if (sscanf_s(v, "%i", &a) == 1) C.witness.push_back(a); }
        else if (sscanf_s(v, "%i", &a) == 1) {
            if (!strcmp(k, "t0")) C.t0 = a; else if (!strcmp(k, "q0")) C.q0 = a;
            else if (!strcmp(k, "qtick")) C.qtick = a; else if (!strcmp(k, "ttick")) C.ttick = a;
            else if (!strcmp(k, "bootstep")) C.bootstep = a; else if (!strcmp(k, "bootqstep")) C.bootqstep = a;
            else if (!strcmp(k, "cap")) C.cap = a; else if (!strcmp(k, "block")) C.block = a;
            else if (!strcmp(k, "warptick")) C.warptick = a; else if (!strcmp(k, "holdrand")) C.holdrand = a;
            else if (!strcmp(k, "negctl")) C.negctl = a; else if (!strcmp(k, "negtick")) C.negtick = a;
            else if (!strcmp(k, "loadsleep")) C.loadsleep = a;
        }
    }
    fclose(f);
    return C.out[0] != 0;
}

// ---------------------------------------------------------------- tick state (detorig.js tst[] / tbuf / tph)
uint32_t s_hashOn = 0, s_nrec = 0, s_tick = 0, s_tdue = 0, s_bootSteps = 1;
uint32_t s_nw = 0, s_bw = 0, s_nblk = 0, s_rawW = 0, s_recw = 0;
uint32_t* s_tbuf = nullptr;
uint8_t* s_tph = nullptr;

// ---------------------------------------------------------------- events on N and on ticks (detorig.js schedule / tickSchedule)
enum EvKind { E_WARP, E_NEGTICK, E_WATCH, E_INPUT, E_CKPT, E_DONE };
struct Event { uint32_t at, seq; EvKind kind; uint32_t a, b; };
std::vector<Event> s_events, s_tevents;
uint32_t s_seq = 0, s_warpN = 0, s_raceN = 0;
bool s_done = false;

void SetDue(uint32_t v) { g_rDue = v; }
void Schedule(uint32_t n, EvKind k, uint32_t a = 0, uint32_t b = 0) {
    s_events.push_back({n, s_seq++, k, a, b});
    std::stable_sort(s_events.begin(), s_events.end(), [](const Event& x, const Event& y) { return x.at != y.at ? x.at < y.at : x.seq < y.seq; });
    SetDue(s_events.empty() ? 0xffffffffu : s_events[0].at);
}
void TickSchedule(uint32_t t, EvKind k) {
    s_tevents.push_back({t, s_seq++, k, 0, 0});
    std::stable_sort(s_tevents.begin(), s_tevents.end(), [](const Event& x, const Event& y) { return x.at != y.at ? x.at < y.at : x.seq < y.seq; });
    s_tdue = s_tevents[0].at;
}

// ---------------------------------------------------------------- scripted input (detorig.js CModule repl)
volatile int s_ctl[64];
volatile int s_active = 0;
typedef int(__cdecl* ResolverFn)(int, int);

__declspec(naked) int __cdecl ResolverTramp(int, int) {
    __asm {
        push ecx
        mov eax, dword ptr [esp + 8]
        jmp dword ptr [g_rRet497315]
    }
}
int __cdecl ResolverRepl(int p, int c) {
    int r = ResolverTramp(p, c);
    if (s_active && p == 0 && c >= 0 && c < 64) return s_ctl[c];
    return r;
}

// ---------------------------------------------------------------- game functions called on the main thread
typedef uint8_t*(__cdecl* GetPtdFn)();
typedef uint32_t(__cdecl* RwDrawFn)();
typedef void(__cdecl* E480Fn)(int, int);

uint8_t* RwRngPtr() { return reinterpret_cast<uint8_t*>(U32(0x007d3ff8) + U32(0x007dc578)); }

void PinRngs(const char* where) {
    uint8_t* ptd = reinterpret_cast<GetPtdFn>(ga(0x004a8828))();
    uint32_t before = *reinterpret_cast<uint32_t*>(ptd + 0x14);
    *reinterpret_cast<uint32_t*>(ptd + 0x14) = C.holdrand;
    uint8_t* P = RwRngPtr();
    uint32_t* buf = *reinterpret_cast<uint32_t**>(P);
    uint32_t cb0 = *reinterpret_cast<uint32_t*>(P + 4), cb1 = *reinterpret_cast<uint32_t*>(P + 8);
    uint32_t v = 0x9a319039u;   // 0x005349cf..0x00534a6d, seed 0x00534a09
    buf[0] = v;
    for (int i = 1; i < 31; i++) { v = v * 0x41c64e6du + 0x3039u; buf[i] = v; }
    *reinterpret_cast<uint32_t*>(P + 4) = reinterpret_cast<uint32_t>(buf) + 0xc;
    *reinterpret_cast<uint32_t*>(P + 8) = reinterpret_cast<uint32_t>(buf);
    RwDrawFn draw = reinterpret_cast<RwDrawFn>(ga(0x00534870));
    for (int i = 0; i < 0x136; i++) draw();
    Ev("{\"kind\":\"pin\",\"at\":\"%s\",\"ptd\":\"0x%x\",\"holdrand_before\":%u,\"rw_ring\":\"0x%x\",\"rw_cursors_before\":[%u,%u]}",
       where, (uint32_t)ptd, before, (uint32_t)buf, cb0, cb1);
}

void PinTimer() {
    unsigned long long q = ((unsigned long long)g_rQhi << 32) | g_rQlo;
    unsigned long long qpf = ((unsigned long long)U32(0x00771e74) << 32) | U32(0x00771e70);
    uint32_t v = qpf ? (uint32_t)((q * 3000000ull) / qpf) : 0;
    uint32_t b[5] = {U32(0x007f1030), U32(0x007f102c), U32(0x00771984), U32(0x00771998), U32(0x0077199c)};
    U32(0x007f1030) = v; U32(0x007f102c) = v; U32(0x00771984) = 0;
    U32(0x00771998) = v / 3000; U32(0x0077199c) = v / 3000;
    Ev("{\"kind\":\"timer\",\"v\":%u,\"before\":[%u,%u,%u,%u,%u]}", v, b[0], b[1], b[2], b[3], b[4]);
}

void PinNorm(const char* where) {
    if (C.norm.empty() || !(strcmp(C.normat, "both") == 0 || strcmp(C.normat, where) == 0)) return;
    unsigned changed = 0;
    for (const auto& r : C.norm) {
        uint8_t* p = ga(r.va);
        uint32_t old = r.w == 1 ? *p : (r.w == 2 ? *reinterpret_cast<uint16_t*>(p) : *reinterpret_cast<uint32_t*>(p));
        if (old != r.v) changed++;
        if (r.w == 1) *p = (uint8_t)r.v; else if (r.w == 2) *reinterpret_cast<uint16_t*>(p) = (uint16_t)r.v; else *reinterpret_cast<uint32_t*>(p) = r.v;
    }
    Ev("{\"kind\":\"norm\",\"at\":\"%s\",\"rows\":%u,\"changed\":%u}", where, (unsigned)C.norm.size(), changed);
}

void Witness(char* dst, size_t n) {
    size_t o = _snprintf_s(dst, n, _TRUNCATE, "[");
    for (size_t i = 0; i < C.witness.size(); i++)
        o += _snprintf_s(dst + o, n - o, _TRUNCATE, "%s[\"%x\",%u]", i ? "," : "", C.witness[i], *ga(C.witness[i]));
    _snprintf_s(dst + o, n - o, _TRUNCATE, "]");
}

void NegCtl() {
    if (C.negctl == 1) { uint32_t* c = *reinterpret_cast<uint32_t**>(RwRngPtr() + 4); *c += 1; }
    else if (C.negctl) U32(C.negctl) += 1;
}

bool WriteFileAll(const char* name, const void* p, size_t n) {
    char path[MAX_PATH]; OutPath(path, name);
    HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    const uint8_t* b = static_cast<const uint8_t*>(p);
    while (n) { DWORD w = 0, k = (DWORD)std::min<size_t>(n, 1u << 24); if (!WriteFile(h, b, k, &w, nullptr) || !w) break; b += w; n -= w; }
    CloseHandle(h);
    return n == 0;
}

void Checkpoint(uint32_t k) {
    uint32_t n = g_rN;
    char name[32]; _snprintf_s(name, sizeof name, _TRUNCATE, "ckpt%02u.data.bin", k);
    WriteFileAll(name, ga(DATA_LO), DATA_HI - DATA_LO);
    uint8_t* ptd = reinterpret_cast<GetPtdFn>(ga(0x004a8828))();
    uint8_t* P = RwRngPtr();
    uint32_t buf = *reinterpret_cast<uint32_t*>(P);
    char ring[31 * 12 + 4]; size_t o = 0;
    for (int i = 0; i < 31; i++) o += _snprintf_s(ring + o, sizeof ring - o, _TRUNCATE, "%s%u", i ? "," : "", reinterpret_cast<uint32_t*>(buf)[i]);
    char wit[256]; Witness(wit, sizeof wit);
    Ev("{\"kind\":\"ckpt\",\"k\":%u,\"n\":%u,\"dn\":%d,\"dn_warp\":%d,\"T\":%u,\"Qlo\":%u,\"Qhi\":%u,\"tgt\":%u,\"qpc\":%u,\"fwd\":%u,"
       "\"phase\":%u,\"clk_7f0ff0\":%u,\"clk_7f0ff4\":%u,\"frames_7f101c\":%u,\"witness\":%s,\"holdrand\":%u,\"rw_ring\":[%s],\"rw_cur\":[%u,%u],\"file\":\"%s\"}",
       k, n, (int)(n - s_raceN), (int)(n - s_warpN), g_rT, g_rQlo, g_rQhi, g_rTgt, g_rQpc, g_rFwd, *ga(0x00771968), U32(0x007f0ff0),
       U32(0x007f0ff4), U32(0x007f101c), wit, *reinterpret_cast<uint32_t*>(ptd + 0x14), ring,
       *reinterpret_cast<uint32_t*>(P + 4) - buf, *reinterpret_cast<uint32_t*>(P + 8) - buf, name);
}

void Done() {
    SetDue(0xffffffffu);
    s_hashOn = 0;
    WriteFileAll("ticks.bin", s_tbuf, (size_t)s_nrec * s_recw * 4);
    WriteFileAll("tick_phases.bin", s_tph, s_nrec);
    Ev("{\"kind\":\"ticks\",\"total\":%u,\"nblk\":%u,\"recw\":%u,\"block\":%u}", s_nrec, s_nblk, s_recw, C.block);
    Ev("{\"kind\":\"done\",\"n\":%u}", g_rN);
    s_done = true;
    char path[MAX_PATH]; OutPath(path, "asi_done");
    FILE* f = nullptr; if (!fopen_s(&f, path, "w") && f) { fputs("done\n", f); fclose(f); }
}

void Warp() {
    s_warpN = g_rN;
    uint8_t ph = *ga(0x00771968);
    if (ph != 1) { Ev("{\"kind\":\"err\",\"msg\":\"boot warp at tick %u: phase %u, not the menu (1)\"}", s_tick, ph); return; }
    U32(0x0063ba7c) = C.scen[0]; U32(0x0067f17c) = C.scen[0]; U32(0x0067e9fc) = C.scen[1];
    U32(0x007f0fd0) = C.scen[2]; U32(0x0067ea98) = C.scen[3]; U32(0x0067ea64) = C.scen[4];
    E480Fn e480 = reinterpret_cast<E480Fn>(ga(0x0040e480));
    for (int k = 0; k < 4; k++) e480(k, k == 0 ? 1 : (k < C.scen[5] ? 2 : 0));
    PinRngs("warp");
    PinTimer();
    PinNorm("warp");
    *ga(0x00771968) = 2;
    Ev("{\"kind\":\"warp\",\"n\":%u,\"tick\":%u}", s_warpN, s_tick);
    s_active = 1;
    Schedule(s_warpN + 1, E_WATCH);
}

void Watch() {
    if (*ga(0x00771968) != 3) {
        if (C.loadsleep) Sleep(C.loadsleep);
        Schedule(g_rN + 1, E_WATCH);
        return;
    }
    s_raceN = g_rN;
    s_hashOn = 1;
    PinRngs("anchor");
    PinNorm("anchor");
    if (!C.negtick) NegCtl();
    char wit[256]; Witness(wit, sizeof wit);
    Ev("{\"kind\":\"race\",\"n\":%u,\"dn_warp\":%d,\"tick\":%u,\"frames_7f101c\":%u,\"clk_7f0ff0\":%u,\"clk_7f0ff4\":%u,\"witness\":%s}",
       s_raceN, (int)(s_raceN - s_warpN), s_tick, U32(0x007f101c), U32(0x007f0ff0), U32(0x007f0ff4), wit);
    for (const auto& in : C.input) Schedule(s_raceN + in.dn, E_INPUT, (uint32_t)in.c, (uint32_t)in.v);
    for (size_t k = 0; k < C.ckpts.size(); k++) Schedule(s_raceN + C.ckpts[k], E_CKPT, (uint32_t)k);
    Schedule(s_raceN + (C.ckpts.empty() ? 0 : C.ckpts.back()) + 1, E_DONE);
}

void RunEvent(const Event& e) {
    switch (e.kind) {
    case E_WARP: Warp(); break;
    case E_NEGTICK: NegCtl(); Ev("{\"kind\":\"info\",\"msg\":\"negctl at tick %u\"}", s_tick); break;
    case E_WATCH: Watch(); break;
    case E_INPUT: if (e.a < 64) s_ctl[e.a] = (int)e.b; break;
    case E_CKPT: Checkpoint(e.a); break;
    case E_DONE: Done(); break;
    }
}

}   // namespace

// detorig.js onDue: tick events first (due was set to 0 by the tick listener), then events on N.
extern "C" void __cdecl ReplayAsi_OnDue() {
    if (!s_tevents.empty()) {
        uint32_t t = s_tick;
        while (!s_tevents.empty() && s_tevents[0].at <= t) { Event e = s_tevents.front(); s_tevents.erase(s_tevents.begin()); RunEvent(e); }
        s_tdue = s_tevents.empty() ? 0 : s_tevents[0].at;
    }
    uint32_t n = g_rN;
    while (!s_events.empty() && s_events[0].at <= n) { Event e = s_events.front(); s_events.erase(s_events.begin()); RunEvent(e); }
    SetDue(s_events.empty() ? 0xffffffffu : s_events[0].at);
}

// detorig.js tick CModule onEnter, at the entry of FUN_00493390.
extern "C" void __cdecl ReplayAsi_TickEnter() {
    unsigned long long q = ((unsigned long long)g_rQhi << 32) | g_rQlo;
    q += C.qtick;
    g_rQlo = (uint32_t)q; g_rQhi = (uint32_t)(q >> 32);
    g_rT += C.ttick;
    s_tick++;
    if (s_tick == 1 && s_bootSteps) { g_rStep = 0; g_rQstep = 0; }
    if (s_tdue && s_tick >= s_tdue) { g_rDue = 0; s_tdue = 0; }
    if (!s_hashOn || s_nrec >= C.cap) return;
    s_tph[s_nrec] = *ga(0x00771968);
    uint32_t* rec = s_tbuf + (size_t)s_nrec * s_recw;
    rec[0] = s_tick; rec[1] = g_rN;
    const uint32_t* p = reinterpret_cast<const uint32_t*>(ga(DATA_LO));
    uint32_t w = 0;
    for (uint32_t b = 0; b < s_nblk; b++) {
        uint32_t h = 2166136261u;
        for (uint32_t i = 0; i < s_bw && w < s_nw; i++, w++) {
            uint32_t v = p[w];
            if (v >= 0x00995000u && v < 0x30000000u && !(v & 3u)) v = 0;
            h = (h ^ v) * 16777619u;
        }
        rec[2 + b] = h;
    }
    uint32_t o = 2 + s_nblk;
    for (const auto& r : C.raw) {
        const uint32_t* s = reinterpret_cast<const uint32_t*>(ga(r.first));
        for (uint32_t i = 0; i < (r.second - r.first) / 4; i++) rec[o++] = s[i];
    }
    s_nrec++;
}

// ---------------------------------------------------------------- naked stubs (instruction-level twins of detorig.js tgtStub / qpcStub)
__declspec(naked) void ReplayAsi_TgtA() {
    __asm {
        mov eax, dword ptr fs:[0x24]
        cmp eax, dword ptr [g_rMain]
        jne real
        mov eax, dword ptr [g_rT]
        mov ecx, dword ptr [g_rStep]
        add dword ptr [g_rT], ecx
        inc dword ptr [g_rTgt]
        inc dword ptr [g_rN]
        mov ecx, dword ptr [g_rN]
        cmp ecx, dword ptr [g_rDue]
        jae slow
        ret
    slow:
        push eax
        push ebp
        mov ebp, esp
        sub esp, 512
        and esp, 0xFFFFFFF0
        fxsave [esp]
        call ReplayAsi_OnDue
        fxrstor [esp]
        mov esp, ebp
        pop ebp
        pop eax
        ret
    real:
        lock inc dword ptr [g_rFwd]
        jmp dword ptr [g_rRealTgtA]
    }
}
__declspec(naked) void ReplayAsi_TgtB() {
    __asm {
        mov eax, dword ptr fs:[0x24]
        cmp eax, dword ptr [g_rMain]
        jne real
        mov eax, dword ptr [g_rT]
        mov ecx, dword ptr [g_rStep]
        add dword ptr [g_rT], ecx
        inc dword ptr [g_rTgt]
        inc dword ptr [g_rN]
        mov ecx, dword ptr [g_rN]
        cmp ecx, dword ptr [g_rDue]
        jae slow
        ret
    slow:
        push eax
        push ebp
        mov ebp, esp
        sub esp, 512
        and esp, 0xFFFFFFF0
        fxsave [esp]
        call ReplayAsi_OnDue
        fxrstor [esp]
        mov esp, ebp
        pop ebp
        pop eax
        ret
    real:
        lock inc dword ptr [g_rFwd]
        jmp dword ptr [g_rRealTgtB]
    }
}
__declspec(naked) void ReplayAsi_Qpc() {
    __asm {
        mov eax, dword ptr fs:[0x24]
        cmp eax, dword ptr [g_rMain]
        jne real
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [g_rQlo]
        mov dword ptr [eax], ecx
        mov ecx, dword ptr [g_rQhi]
        mov dword ptr [eax + 4], ecx
        mov ecx, dword ptr [g_rQstep]
        add dword ptr [g_rQlo], ecx
        adc dword ptr [g_rQhi], 0
        inc dword ptr [g_rQpc]
        inc dword ptr [g_rN]
        mov ecx, dword ptr [g_rN]
        cmp ecx, dword ptr [g_rDue]
        jb fast
        push ebp
        mov ebp, esp
        sub esp, 512
        and esp, 0xFFFFFFF0
        fxsave [esp]
        call ReplayAsi_OnDue
        fxrstor [esp]
        mov esp, ebp
        pop ebp
    fast:
        mov eax, 1
        ret 4
    real:
        lock inc dword ptr [g_rFwd]
        jmp dword ptr [g_rRealQpc]
    }
}
// Frame-timer entry detour (site table in the file header): listener, then the three overwritten instructions, then back.
__declspec(naked) void ReplayAsi_TickDetour() {
    __asm {
        pushad
        pushfd
        mov ebp, esp
        sub esp, 512
        and esp, 0xFFFFFFF0
        fxsave [esp]
        call ReplayAsi_TickEnter
        fxrstor [esp]
        mov esp, ebp
        popfd
        popad
        push ecx
        push edi
        call dword ptr [g_rFn4950b0]
        jmp dword ptr [g_rRet493397]
    }
}

namespace {
uint8_t s_entrySaved[7];

void WriteJmp(uint32_t at, const void* to) {
    DWORD old = 0;
    uint8_t* p = reinterpret_cast<uint8_t*>(at);
    VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9;
    *reinterpret_cast<int32_t*>(p + 1) = (int32_t)(reinterpret_cast<uint32_t>(to) - (at + 5));
    VirtualProtect(p, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), p, 5);
}
void WriteSlot(uint32_t va, void* stub, void** real) {
    DWORD old = 0;
    void** slot = reinterpret_cast<void**>(ga(va));
    *real = *slot;
    VirtualProtect(slot, 4, PAGE_READWRITE, &old);
    *slot = stub;
    Ev("{\"kind\":\"slot\",\"va\":\"0x%x\",\"real\":\"0x%x\",\"stub\":\"0x%x\"}", va, (uint32_t)*real, (uint32_t)stub);
}
}   // namespace

// At the PE entry point, on the main thread (detorig.js cfg.boot listener + install()).
extern "C" void __cdecl ReplayAsi_BootInstall() {
    DWORD old = 0;
    uint8_t* ep = reinterpret_cast<uint8_t*>(g_rEntry);
    VirtualProtect(ep, 7, PAGE_EXECUTE_READWRITE, &old);
    memcpy(ep, s_entrySaved, 7);
    VirtualProtect(ep, 7, old, &old);
    FlushInstructionCache(GetCurrentProcess(), ep, 7);

    g_rMain = GetCurrentThreadId();
    Ev("{\"kind\":\"boot\",\"boot\":{\"ep\":\"0x%x\",\"main\":%u,\"mode\":\"asi\"}}", (uint32_t)ep - g_base + IMG, g_rMain);
    // input first (inactive until the warp), then the clock slots, then the tick listener (detorig.js install order)
    g_rRet497315 = ga(0x00497315);
    WriteJmp((uint32_t)ga(0x00497310), reinterpret_cast<void*>(&ResolverRepl));
    WriteSlot(0x005cc2b4, reinterpret_cast<void*>(&ReplayAsi_TgtA), &g_rRealTgtA);
    WriteSlot(0x005cc0c8, reinterpret_cast<void*>(&ReplayAsi_TgtB), &g_rRealTgtB);
    WriteSlot(0x005cc0c4, reinterpret_cast<void*>(&ReplayAsi_Qpc), &g_rRealQpc);
    g_rFn4950b0 = ga(0x004950b0);
    g_rRet493397 = ga(0x00493397);
    WriteJmp((uint32_t)ga(0x00493390), reinterpret_cast<void*>(&ReplayAsi_TickDetour));
    // boot mode: hash from the first tick, per-call steps until it, warp at C.warptick
    s_hashOn = 1;
    g_rStep = C.bootstep; g_rQstep = C.bootqstep; s_bootSteps = 1;
    TickSchedule(C.warptick, E_WARP);
    if (C.negtick) TickSchedule(C.negtick, E_NEGTICK);
    Ev("{\"kind\":\"installed\"}");
}

__declspec(naked) void ReplayAsi_EntryStub() {
    __asm {
        pushad
        pushfd
        call ReplayAsi_BootInstall
        popfd
        popad
        jmp dword ptr [g_rEntry]
    }
}

// Called from DllMain (dll_main.cpp). Returns false only when MASHED_REPLAY_ASI is unset (or "0"): the default path
// then installs the hooks as before. With the knob set it returns true even when the config is unusable (nothing patched).
bool ReplayAsi_Init() {
    char path[MAX_PATH] = {0};
    DWORD r = GetEnvironmentVariableA("MASHED_REPLAY_ASI", path, sizeof path);
    if (r == 0 || r >= sizeof path || !path[0] || (path[0] == '0' && !path[1])) return false;
    if (!LoadCfg(path)) return true;   // knob set: never fall through to InjectHooks
    char ev[MAX_PATH]; OutPath(ev, "asi_events.jsonl");
    g_ev = _fsopen(ev, "w", _SH_DENYNO);   // readable by the driver while the game runs
    g_base = reinterpret_cast<uint32_t>(GetModuleHandleW(nullptr));
    g_rT = C.t0; g_rQlo = C.q0; g_rQhi = 0; g_rDue = 0xffffffffu;
    for (int i = 0; i < 64; i++) s_ctl[i] = 0;
    s_nw = (DATA_HI - DATA_LO) >> 2;
    s_bw = C.block >> 2;
    s_nblk = (s_nw + s_bw - 1) / s_bw;
    s_rawW = 0;
    for (const auto& x : C.raw) s_rawW += (x.second - x.first) >> 2;
    s_recw = 2 + s_nblk + s_rawW;
    s_tbuf = static_cast<uint32_t*>(VirtualAlloc(nullptr, (size_t)C.cap * s_recw * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    s_tph = static_cast<uint8_t*>(VirtualAlloc(nullptr, C.cap + 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!s_tbuf || !s_tph) { Ev("{\"kind\":\"err\",\"msg\":\"tick buffer alloc failed\"}"); return true; }
    const uint8_t* b = reinterpret_cast<const uint8_t*>(g_base);
    uint32_t lfanew = *reinterpret_cast<const uint32_t*>(b + 0x3c);
    uint32_t aep = *reinterpret_cast<const uint32_t*>(b + lfanew + 0x28);
    g_rEntry = const_cast<uint8_t*>(b + aep);
    static const uint8_t kEntry[7] = {0x6a, 0x60, 0x68, 0x90, 0x12, 0x5d, 0x00};   // push 0x60 ; push 0x5d1290
    if (memcmp(g_rEntry, kEntry, 7) != 0) { Ev("{\"kind\":\"err\",\"msg\":\"entry bytes differ: not the anchored MASHED.exe\"}"); return true; }
    memcpy(s_entrySaved, g_rEntry, 7);
    Ev("{\"kind\":\"init\",\"cfg\":\"%s\",\"nblk\":%u,\"recw\":%u,\"cap\":%u,\"warptick\":%u,\"norm\":%u,\"input\":%u,\"ckpts\":%u}",
       "MASHED_REPLAY_ASI", s_nblk, s_recw, C.cap, C.warptick, (unsigned)C.norm.size(), (unsigned)C.input.size(), (unsigned)C.ckpts.size());
    WriteJmp(reinterpret_cast<uint32_t>(g_rEntry), reinterpret_cast<void*>(&ReplayAsi_EntryStub));
    return true;
}
