// RO-EXE-TICK-TRACE (replay oracle 8): MASHED_REPLAY_TRACE=<dir>, default OFF, read only when MASHED_REPLAY is on
// (Match/PortTop_Replay.cpp, so only on the MASHED_ORIG_TOP path). EXE ONLY. Writes the files re/frida/detorig.py writes
// for the original, so re/frida/detorig_ticks.py and re/frida/detorig_report.py read an exe run directory unchanged:
//   ticks.bin        one record per tick: [tick, N, hash per block ..., raw words ...] (uint32 LE). Taken at the tick
//                    boundary, after the knob advanced the virtual clock and the tick count, before the matched timer
//                    body runs: the order of detorig.js's CModule onEnter. Image 0x005ea000..0x00915008 (detorig.js
//                    DATA_LO / DATA_HI) in MASHED_REPLAY_TRACE_BLOCK-byte blocks (default 1024), FNV-1a over dwords,
//                    a dword in [0x00995000, 0x30000000) with the low two bits clear hashed as 0 (detorig is_ptr rule).
//                    N = the knob's wrapped main-thread clock reads (scope differs from detorig's N: compare --n-ok).
//   ckptNN.data.bin  the same image at MASHED_REPLAY_TRACE_CKPT ticks (comma list), taken right after that tick's record;
//                    k = 100 at the warp before its writes (detorig.js menuCkpt).
//   meta.json        detorig.py's keys (ticks, ckpt, warp, race, boot, cfg, rc) + side "exe" + unreadable page ranges.
// Pages of the range that are not committed readable here are hashed and dumped as 0 (listed in meta.json).
// Self-test: at each checkpoint tick an independent recompute of the block hashes and raw words from the dump buffer
// must equal that tick's record. Line "[TRACE] self-test PASS|FAIL (f of n checks failed)".
// MASHED_REPLAY_TRACE_NEGCTL=<tick>:<va>: +1 on the dword at va at that tick, before the hash (negative control).
// MASHED_REPLAY_TRACE_RAW = comma list of LO-HI hex ranges (default: verify/romenu_20261112/run_arm.sh's RAW);
// MASHED_REPLAY_TRACE_CAP = max records (default 6000).
// RVA-LINT: ORIGTOP-ONLY

#ifdef MASHED_STANDALONE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

constexpr unsigned kLo = 0x005ea000u, kHi = 0x00915008u;
constexpr unsigned kNw = (kHi - kLo) / 4u;
constexpr unsigned kPage = 0x1000u;
constexpr unsigned kNPages = (kHi - kLo + kPage - 1) / kPage;

void TLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] [TRACE] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

struct Ckpt {
    unsigned k, tick, n, T, Qlo, Qhi, phase, c0, c4, frames;
    unsigned ring[31];
    int rp, wp;
    std::string file;
};

int g_on = 0;
std::string g_dir;
std::FILE* g_ticks = nullptr;
unsigned g_block = 1024, g_bw = 256, g_nblk = 0, g_cap = 6000, g_recw = 0, g_rawW = 0;
std::vector<std::pair<unsigned, unsigned>> g_raw;
std::vector<unsigned> g_ckptTicks;
unsigned g_negTick = 0, g_negVa = 0;
bool g_negDone = false;
std::vector<unsigned> g_rec;
std::vector<unsigned char> g_phases;
std::vector<unsigned char> g_pageOk;       // this tick's map
std::vector<unsigned char> g_pageEverBad;  // union over the run
unsigned g_records = 0, g_dropped = 0, g_mapChanges = 0;
std::vector<unsigned> g_dump;
std::vector<Ckpt> g_ck;
unsigned g_checks = 0, g_fails = 0;
bool g_warped = false, g_anchored = false;
unsigned g_warpTick = 0, g_warpN = 0, g_warpPhase = 0, g_raceTick = 0, g_raceN = 0;
unsigned g_raceFrames = 0, g_raceC0 = 0, g_raceC4 = 0;
const unsigned* const* g_ringSt = nullptr;   // {base, rp, wp, end} of the knob's own RW ring
unsigned long long g_lastQ = 0;
unsigned g_lastT = 0, g_lastTick = 0, g_lastN = 0;

void Check(bool ok, const char* what, unsigned a, unsigned b, unsigned tick) {
    ++g_checks;
    if (!ok && ++g_fails <= 12) TLog("self-test FAIL %s: record 0x%08X dump 0x%08X (tick %u)", what, a, b, tick);
}

void MkDirs(const std::string& p) {
    for (size_t i = 1; i <= p.size(); ++i)
        if (i == p.size() || p[i] == '/' || p[i] == '\\') {
            const std::string s = p.substr(0, i);
            if (s.size() > 2 || (s.size() == 2 && s[1] != ':')) CreateDirectoryA(s.c_str(), nullptr);
        }
}

bool Readable(DWORD prot) {
    if (prot & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    return (prot & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                    PAGE_EXECUTE_WRITECOPY)) != 0;
}

void MapPages() {
    std::vector<unsigned char> m(kNPages, 0);
    unsigned a = kLo & ~(kPage - 1);
    while (a < kHi) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(static_cast<std::uintptr_t>(a)), &mbi, sizeof(mbi))) { a += kPage; continue; }
        const unsigned end = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize);
        const bool ok = mbi.State == MEM_COMMIT && Readable(mbi.Protect);
        for (unsigned p = a; p < end && p < kHi; p += kPage) {
            const unsigned i = (p - (kLo & ~(kPage - 1))) / kPage;
            if (i < kNPages) m[i] = ok ? 1 : 0;
        }
        a = end > a ? end : a + kPage;
    }
    if (!g_pageOk.empty() && m != g_pageOk) ++g_mapChanges;
    g_pageOk.swap(m);
    for (unsigned i = 0; i < kNPages; ++i) if (!g_pageOk[i]) g_pageEverBad[i] = 1;
}

inline unsigned WordAt(unsigned w) {   // dword w of the image, 0 on a page that is not readable
    const unsigned va = kLo + 4u * w;
    if (!g_pageOk[(va - (kLo & ~(kPage - 1))) / kPage]) return 0;
    return *reinterpret_cast<const volatile unsigned*>(static_cast<std::uintptr_t>(va));
}

inline unsigned HeapRule(unsigned v) { return (v >= 0x00995000u && v < 0x30000000u && !(v & 3u)) ? 0u : v; }

// the record of this tick from live memory
void BuildRecord(unsigned tick, unsigned n) {
    unsigned* rec = g_rec.data();
    rec[0] = tick; rec[1] = n;
    unsigned w = 0;
    for (unsigned b = 0; b < g_nblk; ++b) {
        unsigned h = 2166136261u;
        for (unsigned i = 0; i < g_bw && w < kNw; ++i, ++w) h = (h ^ HeapRule(WordAt(w))) * 16777619u;
        rec[2 + b] = h;
    }
    unsigned o = 2 + g_nblk;
    for (const auto& r : g_raw)
        for (unsigned va = r.first; va < r.second; va += 4) rec[o++] = (va >= kLo && va + 4 <= kHi) ? WordAt((va - kLo) / 4u) : 0u;
}

// independent recompute from the dump buffer (bytes, memcpy per dword, its own block walk)
void SelfCheck(unsigned tick) {
    const unsigned char* d = reinterpret_cast<const unsigned char*>(g_dump.data());
    const unsigned nbytes = kNw * 4u;
    for (unsigned b = 0; b < g_nblk; ++b) {
        unsigned h = 0x811c9dc5u;
        const unsigned lo = b * g_block, hi = (lo + g_block < nbytes) ? lo + g_block : nbytes;
        for (unsigned off = lo; off < hi; off += 4) {
            unsigned v; std::memcpy(&v, d + off, 4);
            if (v >= 0x00995000u && v < 0x30000000u && (v % 4u) == 0) v = 0;
            h ^= v; h *= 0x01000193u;
        }
        Check(h == g_rec[2 + b], "block hash", g_rec[2 + b], h, tick);
    }
    unsigned o = 2 + g_nblk;
    for (const auto& r : g_raw)
        for (unsigned va = r.first; va < r.second; va += 4, ++o) {
            unsigned v = 0;
            if (va >= kLo && va + 4 <= kHi) std::memcpy(&v, d + (va - kLo), 4);
            Check(v == g_rec[o], "raw word", g_rec[o], v, tick);
        }
}

unsigned Rd(unsigned va) { return (va >= kLo && va + 4 <= kHi) ? WordAt((va - kLo) / 4u) : 0u; }
unsigned RdB(unsigned va) {
    if (va < kLo || va >= kHi || !g_pageOk[(va - (kLo & ~(kPage - 1))) / kPage]) return 0;
    return *reinterpret_cast<const volatile unsigned char*>(static_cast<std::uintptr_t>(va));
}

// ---------------------------------------------------------------- write watch (RO-FIRST-DIVERGENCE)
// MASHED_REPLAY_TRACE_WATCH=<va>[,<va>...] (at most 4, dword-aligned, inside the image): x86 debug-register WRITE
// watchpoints (DR0..DR3, length 4) on the main thread, armed at the trace init by a helper thread (SuspendThread,
// SetThreadContext, ResumeThread; the main thread waits on it). A vectored handler takes the single-step trap that
// follows each store: one hit = {tick, n, rec, eip, slot, value after the store, ebp-chain return addresses}. rec = the
// ticks.bin records written before the store, so the writes between record r - 1 and record r are those with rec == r. eip is the
// instruction after the store (the detorig.js watch convention); resolve it with mashed_re.map
// (re/frida/detorig_firstdiv.py --exe-map). Hits go to watch.jsonl at the finish: the first 4096 in full, then per-eip
// counts. Threads other than the main thread are not armed (listed as such in meta.json "watch").
// MASHED_REPLAY_TRACE_WATCH_FROM=<tick>: keep hits only from that tick on (default 0 = all).
struct WHit { unsigned tick, n, rec, eip, slot, value, bt[8]; };
unsigned g_wVa[4] = {};
unsigned g_wN = 0, g_wFrom = 0;
bool g_wArmed = false;
WHit g_wHits[4096];
volatile LONG g_wCount = 0;
struct WPc { unsigned eip, slot, count, firstTick, lastTick; };
WPc g_wPc[256];
unsigned g_wPcN = 0, g_wPcDrop = 0;
HANDLE g_wMain = nullptr;
DWORD g_wMainId = 0;
PVOID g_wVeh = nullptr;

LONG CALLBACK WatchVeh(PEXCEPTION_POINTERS ep) {
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP) return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT* c = ep->ContextRecord;
    const DWORD dr6 = c->Dr6;
    if (!(dr6 & ((1u << g_wN) - 1u))) return EXCEPTION_CONTINUE_SEARCH;
    for (unsigned s = 0; s < g_wN; ++s) {
        if (!(dr6 & (1u << s))) continue;
        if (g_lastTick < g_wFrom) continue;
        unsigned i = 0;
        for (; i < g_wPcN; ++i) if (g_wPc[i].eip == c->Eip && g_wPc[i].slot == s) break;
        if (i == g_wPcN) {
            if (g_wPcN < 256) g_wPc[g_wPcN++] = WPc{c->Eip, s, 0, g_lastTick, g_lastTick};
            else { ++g_wPcDrop; i = 256; }
        }
        if (i < 256) { ++g_wPc[i].count; g_wPc[i].lastTick = g_lastTick; }
        const LONG k = g_wCount;
        if (k < 4096) {
            WHit& h = g_wHits[k];
            h = WHit{};
            h.tick = g_lastTick; h.n = g_lastN; h.rec = g_records; h.eip = c->Eip; h.slot = s;
            h.value = *reinterpret_cast<const volatile unsigned*>(static_cast<std::uintptr_t>(g_wVa[s]));
            // frame-pointer chain, bounded by this thread's stack (best effort: a body without a frame pointer is skipped over)
            const NT_TIB* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
            const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(tib->StackLimit), hi = reinterpret_cast<std::uintptr_t>(tib->StackBase);
            std::uintptr_t fp = c->Ebp;
            for (int d = 0; d < 8 && fp >= lo && fp + 8 <= hi && !(fp & 3); ++d) {
                h.bt[d] = reinterpret_cast<const unsigned*>(fp)[1];
                const std::uintptr_t nx = reinterpret_cast<const unsigned*>(fp)[0];
                if (nx <= fp) break;
                fp = nx;
            }
        }
        g_wCount = k + 1;
    }
    c->Dr6 = 0;
    return EXCEPTION_CONTINUE_EXECUTION;
}

DWORD WINAPI WatchArmProc(LPVOID) {
    if (SuspendThread(g_wMain) == static_cast<DWORD>(-1)) return 1;
    CONTEXT c{};
    c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    DWORD rc = 2;
    if (GetThreadContext(g_wMain, &c)) {
        DWORD dr7 = c.Dr7;
        DWORD* dr[4] = {&c.Dr0, &c.Dr1, &c.Dr2, &c.Dr3};
        for (unsigned s = 0; s < g_wN; ++s) {
            *dr[s] = g_wVa[s];
            dr7 &= ~((3u << (16 + 4 * s)) | (3u << (18 + 4 * s)) | (3u << (2 * s)));
            dr7 |= (1u << (2 * s)) | (1u << (16 + 4 * s)) | (3u << (18 + 4 * s));   // local enable, RW = write, LEN = 4
        }
        c.Dr7 = dr7;
        c.Dr6 = 0;
        rc = SetThreadContext(g_wMain, &c) ? 0 : 3;
    }
    ResumeThread(g_wMain);
    return rc;
}

void WatchArm() {
    const char* v = std::getenv("MASHED_REPLAY_TRACE_WATCH");
    if (!v || !v[0]) return;
    for (const char* p = v; *p && g_wN < 4;) {
        char* e = nullptr;
        const unsigned va = static_cast<unsigned>(std::strtoul(p, &e, 16));
        if (e == p) break;
        if (va >= kLo && va + 4 <= kHi && !(va & 3)) g_wVa[g_wN++] = va;
        else TLog("watch 0x%08X refused (outside the image or not dword-aligned)", va);
        p = (*e == ',') ? e + 1 : e;
    }
    if (const char* f = std::getenv("MASHED_REPLAY_TRACE_WATCH_FROM")) g_wFrom = static_cast<unsigned>(std::strtoul(f, nullptr, 0));
    if (!g_wN) return;
    g_wMainId = GetCurrentThreadId();
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_wMain,
                         THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, 0)) {
        TLog("watch: DuplicateHandle failed (%lu), not armed", GetLastError());
        g_wN = 0;
        return;
    }
    g_wVeh = AddVectoredExceptionHandler(1, WatchVeh);
    HANDLE t = CreateThread(nullptr, 0, WatchArmProc, nullptr, 0, nullptr);
    DWORD rc = 9;
    if (t) { WaitForSingleObject(t, 5000); GetExitCodeThread(t, &rc); CloseHandle(t); }
    g_wArmed = rc == 0;
    // read back from the main thread's own context: the registers must hold what was written
    CONTEXT c{};
    c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &c);
    TLog("watch %s (rc %lu) on main thread %lu: %u slot(s) 0x%08X 0x%08X 0x%08X 0x%08X, dr7 0x%08lX, from tick %u",
         g_wArmed ? "armed" : "NOT armed", rc, g_wMainId, g_wN, g_wVa[0], g_wVa[1], g_wVa[2], g_wVa[3], c.Dr7, g_wFrom);
}

void WatchWrite() {
    if (!g_wN) return;
    const std::string path = g_dir + "/watch.jsonl";
    std::FILE* f = std::fopen(path.c_str(), "w");
    if (!f) { TLog("cannot write %s", path.c_str()); return; }
    const LONG n = g_wCount < 4096 ? g_wCount : 4096;
    for (LONG i = 0; i < n; ++i) {
        const WHit& h = g_wHits[i];
        std::fprintf(f, "{\"kind\": \"watch\", \"side\": \"exe\", \"tick\": %u, \"n\": %u, \"rec\": %u, \"slot\": %u, \"va\": \"0x%x\", "
                        "\"pc\": \"0x%x\", \"value\": \"0x%x\", \"bt\": \"", h.tick, h.n, h.rec, h.slot, g_wVa[h.slot], h.eip, h.value);
        for (int d = 0; d < 8 && h.bt[d]; ++d) std::fprintf(f, "%s0x%x", d ? "|" : "", h.bt[d]);
        std::fputs("\"}\n", f);
    }
    for (unsigned i = 0; i < g_wPcN; ++i)
        std::fprintf(f, "{\"kind\": \"watch_pc\", \"side\": \"exe\", \"slot\": %u, \"va\": \"0x%x\", \"pc\": \"0x%x\", \"count\": %u, "
                        "\"first_tick\": %u, \"last_tick\": %u}\n", g_wPc[i].slot, g_wVa[g_wPc[i].slot], g_wPc[i].eip,
                     g_wPc[i].count, g_wPc[i].firstTick, g_wPc[i].lastTick);
    std::fclose(f);
    TLog("watch: %ld hit(s), %u distinct pc, %u pc over the table, watch.jsonl written", static_cast<long>(g_wCount), g_wPcN, g_wPcDrop);
}

void WriteMeta(int rc);

void Dump(unsigned k, unsigned tick, unsigned n, unsigned long long q, unsigned t) {
    for (unsigned w = 0; w < kNw; ++w) g_dump[w] = WordAt(w);
    char name[32];
    std::snprintf(name, sizeof(name), "ckpt%02u.data.bin", k);
    const std::string path = g_dir + "/" + name;
    if (std::FILE* f = std::fopen(path.c_str(), "wb")) {
        std::fwrite(g_dump.data(), 4, kNw, f);
        std::fclose(f);
    } else TLog("cannot write %s", path.c_str());
    Ckpt c{};
    c.k = k; c.tick = tick; c.n = n; c.T = t;
    c.Qlo = static_cast<unsigned>(q); c.Qhi = static_cast<unsigned>(q >> 32);
    c.phase = RdB(0x00771968u); c.c0 = Rd(0x007f0ff0u); c.c4 = Rd(0x007f0ff4u); c.frames = Rd(0x007f101cu);
    if (g_ringSt) {
        for (int i = 0; i < 31; ++i) c.ring[i] = g_ringSt[0][i];
        c.rp = static_cast<int>(g_ringSt[1] - g_ringSt[0]) * 4; c.wp = static_cast<int>(g_ringSt[2] - g_ringSt[0]) * 4;
    }
    c.file = name;
    g_ck.push_back(c);
    TLog("checkpoint k %u at tick %u: N %u phase %u, %s", k, tick, n, c.phase, name);
    WriteMeta(-1);
}

void PhaseRuns(std::FILE* f) {
    std::fputs("[", f);
    bool first = true;
    size_t i = 0;
    while (i < g_phases.size()) {
        size_t j = i;
        while (j + 1 < g_phases.size() && g_phases[j + 1] == g_phases[i]) ++j;
        std::fprintf(f, "%s[%u, %u, %u]", first ? "" : ", ", g_phases[i], static_cast<unsigned>(i), static_cast<unsigned>(j));
        first = false; i = j + 1;
    }
    std::fputs("]", f);
}

void WriteMeta(int rc) {
    const std::string path = g_dir + "/meta.json";
    std::FILE* f = std::fopen(path.c_str(), "w");
    if (!f) { TLog("cannot write %s", path.c_str()); return; }
    char exe[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    for (char* p = exe; *p; ++p) if (*p == '\\') *p = '/';
    const char* lbl = std::strrchr(g_dir.c_str(), '/');
    std::fprintf(f, "{\n \"side\": \"exe\",\n \"label\": \"%s\",\n \"pid\": %lu,\n \"exe\": \"%s\",\n",
                 lbl ? lbl + 1 : g_dir.c_str(), GetCurrentProcessId(), exe);
    const char* rj = std::getenv("MASHED_REPLAY");
    std::string rjs = rj ? rj : "";
    for (auto& ch : rjs) if (ch == '\\') ch = '/';
    std::fprintf(f, " \"cfg\": {\"replay\": \"%s\", \"block\": %u, \"cap\": %u, \"tick_va\": \"exe:PortTop_Replay_00493390\", "
                    "\"ckpt_ticks\": [", rjs.c_str(), g_block, g_cap);
    for (size_t i = 0; i < g_ckptTicks.size(); ++i) std::fprintf(f, "%s%u", i ? ", " : "", g_ckptTicks[i]);
    std::fputs("], \"raw\": [", f);
    for (size_t i = 0; i < g_raw.size(); ++i) std::fprintf(f, "%s[%u, %u]", i ? ", " : "", g_raw[i].first, g_raw[i].second);
    if (g_negTick) std::fprintf(f, "], \"negctl\": [%u, %u]},\n", g_negTick, g_negVa);
    else std::fputs("], \"negctl\": null},\n", f);
    std::fprintf(f, " \"boot\": {\"ep\": \"exe\", \"main\": %lu},\n", GetCurrentThreadId());
    if (g_warped) std::fprintf(f, " \"warp\": {\"kind\": \"warp\", \"n\": %u, \"tick\": %u, \"phase_before\": %u},\n", g_warpN, g_warpTick, g_warpPhase);
    if (g_anchored)
        std::fprintf(f, " \"race\": {\"kind\": \"race\", \"n\": %u, \"dn_warp\": %u, \"tick\": %u, \"frames_7f101c\": %u, "
                        "\"clk_7f0ff0\": %u, \"clk_7f0ff4\": %u, \"witness\": []},\n",
                     g_raceN, g_raceN - g_warpN, g_raceTick, g_raceFrames, g_raceC0, g_raceC4);
    else std::fputs(" \"race\": {\"kind\": \"race\", \"n\": 0, \"tick\": null, \"reached\": false, \"witness\": []},\n", f);
    std::fputs(" \"ckpt\": [", f);
    for (size_t i = 0; i < g_ck.size(); ++i) {
        const Ckpt& c = g_ck[i];
        std::fprintf(f, "%s\n  {\"kind\": \"ckpt\", \"k\": %u, \"tick\": %u, \"n\": %u, \"dn\": %d, \"dn_warp\": %d, \"T\": %u, "
                        "\"Qlo\": %u, \"Qhi\": %u, \"phase\": %u, \"clk_7f0ff0\": %u, \"clk_7f0ff4\": %u, \"frames_7f101c\": %u, "
                        "\"witness\": [], \"holdrand\": null, \"rw_ring\": [",
                     i ? "," : "", c.k, c.tick, c.n, static_cast<int>(c.n - g_raceN), static_cast<int>(c.n - g_warpN), c.T, c.Qlo,
                     c.Qhi, c.phase, c.c0, c.c4, c.frames);
        for (int j = 0; j < 31; ++j) std::fprintf(f, "%s%u", j ? ", " : "", c.ring[j]);
        std::fprintf(f, "], \"rw_cur\": [%d, %d], \"file\": \"%s\", \"sha256\": null}", c.rp, c.wp, c.file.c_str());
    }
    std::fprintf(f, "\n ],\n \"ticks\": {\"total\": %u, \"nblk\": %u, \"recw\": %u, \"block\": %u, \"raw\": [", g_records, g_nblk,
                 g_recw, g_block);
    for (size_t i = 0; i < g_raw.size(); ++i) std::fprintf(f, "%s[%u, %u]", i ? ", " : "", g_raw[i].first, g_raw[i].second);
    std::fputs("], \"file\": \"ticks.bin\", \"phases\": ", f);
    PhaseRuns(f);
    std::fprintf(f, ", \"dropped\": %u},\n \"tick_phases\": ", g_dropped);
    PhaseRuns(f);
    std::fputs(",\n \"unreadable\": [", f);
    bool first = true;
    for (unsigned i = 0; i < kNPages;) {
        if (!g_pageEverBad[i]) { ++i; continue; }
        unsigned j = i;
        while (j + 1 < kNPages && g_pageEverBad[j + 1]) ++j;
        const unsigned base = kLo & ~(kPage - 1);
        std::fprintf(f, "%s[%u, %u]", first ? "" : ", ", base + i * kPage, base + (j + 1) * kPage);
        first = false; i = j + 1;
    }
    std::fputs("],\n \"watch\": {\"va\": [", f);
    for (unsigned i = 0; i < g_wN; ++i) std::fprintf(f, "%s\"0x%x\"", i ? ", " : "", g_wVa[i]);
    std::fprintf(f, "], \"armed\": %s, \"thread\": %lu, \"threads\": \"main only\", \"from_tick\": %u, \"hits\": %ld}",
                 g_wArmed ? "true" : "false", g_wMainId, g_wFrom, static_cast<long>(g_wCount));
    std::fprintf(f, ",\n \"page_map_changes\": %u,\n \"selftest\": {\"checks\": %u, \"fails\": %u},\n \"rc\": %d\n}\n",
                 g_mapChanges, g_checks, g_fails, rc < 0 ? 1 : rc);
    std::fclose(f);
}

bool ParseRaw(const char* s) {
    g_raw.clear();
    while (s && *s) {
        char* e = nullptr;
        const unsigned lo = static_cast<unsigned>(std::strtoul(s, &e, 16));
        if (!e || *e != '-') return false;
        const unsigned hi = static_cast<unsigned>(std::strtoul(e + 1, &e, 16));
        if (hi <= lo || (lo & 3) || (hi & 3)) return false;
        g_raw.emplace_back(lo, hi);
        s = (*e == ',') ? e + 1 : e;
        if (*e && *e != ',') return false;
    }
    return true;
}

}  // namespace

extern "C" {

// called by Replay_Init once the knob is on; ring = {base, rp, wp, end} of the knob's own RW ring
int ReplayTrace_Init(const unsigned* const* ring) {
    const char* v = std::getenv("MASHED_REPLAY_TRACE");
    if (!v || !v[0] || (v[0] == '0' && !v[1])) return 0;
    g_dir = v;
    for (auto& ch : g_dir) if (ch == '\\') ch = '/';
    while (g_dir.size() > 1 && g_dir.back() == '/') g_dir.pop_back();
    if (const char* b = std::getenv("MASHED_REPLAY_TRACE_BLOCK")) g_block = static_cast<unsigned>(std::strtoul(b, nullptr, 0));
    if (g_block < 4 || (g_block & 3)) g_block = 1024;
    g_bw = g_block / 4;
    if (const char* c = std::getenv("MASHED_REPLAY_TRACE_CAP")) g_cap = static_cast<unsigned>(std::strtoul(c, nullptr, 0));
    const char* raw = std::getenv("MASHED_REPLAY_TRACE_RAW");
    if (!ParseRaw(raw ? raw : "0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,"
                              "0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400")) {
        TLog("MASHED_REPLAY_TRACE_RAW=%s: bad range list, trace stays off", raw);
        return 0;
    }
    if (const char* c = std::getenv("MASHED_REPLAY_TRACE_CKPT"))
        for (const char* p = c; *p;) {
            char* e = nullptr;
            const unsigned t = static_cast<unsigned>(std::strtoul(p, &e, 0));
            if (e == p) break;
            g_ckptTicks.push_back(t);
            p = (*e == ',') ? e + 1 : e;
        }
    if (const char* ng = std::getenv("MASHED_REPLAY_TRACE_NEGCTL")) {
        char* e = nullptr;
        g_negTick = static_cast<unsigned>(std::strtoul(ng, &e, 0));
        if (e && *e == ':') g_negVa = static_cast<unsigned>(std::strtoul(e + 1, nullptr, 0));
        if (!g_negVa || g_negVa < kLo || g_negVa + 4 > kHi) g_negTick = 0;
    }
    g_nblk = (kNw + g_bw - 1) / g_bw;
    g_rawW = 0;
    for (const auto& r : g_raw) g_rawW += (r.second - r.first) / 4u;
    g_recw = 2 + g_nblk + g_rawW;
    g_rec.assign(g_recw, 0);
    g_dump.assign(kNw, 0);
    g_pageEverBad.assign(kNPages, 0);
    g_ringSt = ring;
    MkDirs(g_dir);
    g_ticks = std::fopen((g_dir + "/ticks.bin").c_str(), "wb");
    if (!g_ticks) { TLog("cannot open %s/ticks.bin, trace stays off", g_dir.c_str()); return 0; }
    std::setvbuf(g_ticks, nullptr, _IOFBF, 1u << 20);
    g_on = 1;
    MapPages();
    WatchArm();
    // RO-FIRST-DIVERGENCE: tick 0 in MASHED_REPLAY_TRACE_CKPT = a checkpoint here, at the trace init (before the first tick)
    for (size_t i = 0; i < g_ckptTicks.size(); ++i)
        if (g_ckptTicks[i] == 0) Dump(static_cast<unsigned>(i), 0, 0, 0, 0);
    TLog("MASHED_REPLAY_TRACE=%s on: image 0x%08X..0x%08X, block %u B, %u blocks, %u raw words in %u ranges, record %u "
         "dwords, cap %u, %u checkpoint ticks, negctl %s",
         g_dir.c_str(), kLo, kHi, g_block, g_nblk, g_rawW, static_cast<unsigned>(g_raw.size()), g_recw, g_cap,
         static_cast<unsigned>(g_ckptTicks.size()), g_negTick ? "on" : "off");
    WriteMeta(-1);
    return 1;
}

int ReplayTrace_On(void) { return g_on; }

// the tick boundary: after the knob's Q / T / tick advance, before the matched timer body
void ReplayTrace_Tick(unsigned tick, unsigned n, unsigned long long q, unsigned t) {
    if (!g_on) return;
    g_lastQ = q; g_lastT = t; g_lastTick = tick; g_lastN = n;
    MapPages();
    if (g_negTick && tick == g_negTick && !g_negDone) {
        const unsigned w = (g_negVa - kLo) / 4u;
        if (g_pageOk[(g_negVa - (kLo & ~(kPage - 1))) / kPage]) {
            volatile unsigned& m = *reinterpret_cast<volatile unsigned*>(static_cast<std::uintptr_t>(g_negVa));
            const unsigned before = m;
            m = before + 1u;
            TLog("negctl at tick %u: 0x%08X 0x%08X -> 0x%08X", tick, g_negVa, before, before + 1u);
        } else TLog("negctl at tick %u: 0x%08X not readable here, not applied", tick, g_negVa);
        (void)w;
        g_negDone = true;
    }
    if (g_records >= g_cap) { ++g_dropped; return; }
    BuildRecord(tick, n);
    std::fwrite(g_rec.data(), 4, g_recw, g_ticks);
    g_phases.push_back(static_cast<unsigned char>(RdB(0x00771968u)));
    ++g_records;
    unsigned k = 0;
    for (size_t i = 0; i < g_ckptTicks.size(); ++i)
        if (g_ckptTicks[i] == tick) {
            k = static_cast<unsigned>(i);
            Dump(k, tick, n, q, t);
            SelfCheck(tick);
        }
}

// before = 1: inside the warp, before its writes (checkpoint k 100); before = 0: after them
void ReplayTrace_Warp(unsigned tick, unsigned n, unsigned phaseBefore, int before) {
    if (!g_on) return;
    if (before) { MapPages(); Dump(100, tick, n, g_lastQ, g_lastT); return; }
    g_warped = true; g_warpTick = tick; g_warpN = n; g_warpPhase = phaseBefore;
    WriteMeta(-1);
}

void ReplayTrace_Anchor(unsigned tick, unsigned n) {
    if (!g_on) return;
    MapPages();
    g_anchored = true; g_raceTick = tick; g_raceN = n;
    g_raceFrames = Rd(0x007f101cu); g_raceC0 = Rd(0x007f0ff0u); g_raceC4 = Rd(0x007f0ff4u);
    WriteMeta(-1);
}

void ReplayTrace_Finish(void) {
    if (!g_on) return;
    if (g_ticks) { std::fclose(g_ticks); g_ticks = nullptr; }
    WatchWrite();
    unsigned bad = 0;
    for (unsigned i = 0; i < kNPages; ++i) bad += g_pageEverBad[i];
    WriteMeta(0);
    TLog("self-test %s (%u of %u checks failed): records %u (dropped %u over cap %u), checkpoints %u, pages not readable "
         "at some tick %u of %u, page map changes %u, warp %s at tick %u, anchor %s at tick %u (N %u)",
         g_fails ? "FAIL" : "PASS", g_fails, g_checks, g_records, g_dropped, g_cap, static_cast<unsigned>(g_ck.size()), bad,
         kNPages, g_mapChanges, g_warped ? "ran" : "not reached", g_warpTick, g_anchored ? "reached" : "not reached",
         g_raceTick, g_raceN);
    g_on = 0;
}

}  // extern "C"

#endif  // MASHED_STANDALONE
