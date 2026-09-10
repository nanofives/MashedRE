// Mashed RE — SHADOW A/B, LANE 3: page-level write tracking (no region spec, no idempotency
// assumption). Companion to ShadowAB.h; see re/analysis/promotion_lanes_assessment_20260910.md
// (Lane 3) and re/analysis/lane3_write_tracking_20260910.md.
//
// --- THE PROBLEM IT REMOVES ---------------------------------------------------------
//   Run()       compares a RETURN only: void ports are invisible, and a non-idempotent
//               function legitimately returns two different values on its two executions
//               (7 of the first sweep's 10 DIVERGENT rows were exactly that).
//   RunRegion() compares ONE contiguous span the author has to know and prove; 348 void
//               ports across C2/C3 have no verified span.
// This primitive compares EFFECTS: every page the call wrote, plus the caller's stack
// window, with the pre-call state restored between the two executions.
//
// --- MECHANISM, PER SAMPLED CALL ----------------------------------------------------
//   0. suspend every other thread of the process (see STOP THE WORLD below)
//   1. enumerate every committed, writable, NON-executable page that is either private
//      (heaps, .bss) or part of MASHED.exe's image (.data), EXCLUDING: this .asi's image,
//      the tracking buffers, the current thread's stack and TEB, and PAGE_GUARD pages.
//   2. VirtualProtect all of them PAGE_READONLY; install a vectored exception handler.
//   3. call the ORIGINAL (hook uninstalled). Each first write to a page faults: the handler
//      copies the page's PRE-image into a pool, restores the page's protection, resumes.
//   4. restore protections; copy the POST-images of the touched pages; write the PRE-images
//      back  -> memory is exactly as before the call (that is what defeats idempotency).
//   5. repeat 2-3 for OUR port.
//   6. compare: for pages the original touched, post_orig vs live; for pages only the port
//      touched, pre vs live (a write of the same value is no difference). The caller's
//      stack window (4 KB above this wrapper's return address) is snapshotted/restored the
//      same way so out-parameters that live in the caller's frame are covered.
//   The port's effects are what stays in memory afterwards, exactly like Run().
//
// --- LIMITS (do not overclaim) ------------------------------------------------------
//   * .asi only; verifies logic against the original in MASHED's address space.
//   * Effects OUTSIDE tracked memory are invisible: files, handles, GPU, other modules'
//     .data (system DLL globals are deliberately untracked to keep noise down), and
//     writes below the caller's frame window. Irreversible side effects (file/COM) are
//     executed twice; the page restore does not undo them.
//   * Allocation-heavy functions produce different heap pages on the two runs by
//     construction (different block addresses): expect DIVERGENT there, read the offsets.
//   * The hook is re-installed only AFTER the window (the registry entry lives in a tracked
//     heap page), so a recursive call from inside the port reaches the original.
//   * Heavier than Run(): two full protect/unprotect passes per sample (ms each). The
//     default budget is therefore lower (kTrackedSamples).
//   * STOP THE WORLD: other threads are suspended for the whole compare. A suspended thread
//     holding a lock the original or the port needs (heap, D3D runtime) deadlocks the game.
//     Physics/gameplay leaves do not take those locks; anything that allocates or talks to
//     D3D is a bad candidate for this lane.
//
// --- USAGE ---------------------------------------------------------------------------
//   extern "C" void __cdecl MyVoidFn(int a, float* out) {
//       SHADOW_AB_COUNTER(ab, "MyVoidFn", 0x0055b750, ShadowAB::kPhaseRace);
//       ShadowAB::RunTracked(ab, MyVoidFn_impl, SHADOW_STACK_WINDOW(), a, out);
//   }
// Arm with MASHED_SHADOW_AB=1 as usual. MASHED_SHADOW_TRACE=1 adds per-stage breadcrumbs.
#pragma once

#include "ShadowAB.h"

#include <intrin.h>
#include <cstdlib>
#include <tlhelp32.h>
#include <type_traits>

// Address of the first stack argument the GAME pushed for this wrapper: the start of the
// caller-owned stack window. Must be expanded in the hook wrapper itself, not in a callee.
#define SHADOW_STACK_WINDOW() \
    (reinterpret_cast<void*>(static_cast<char*>(_AddressOfReturnAddress()) + 4))

namespace ShadowAB {
namespace Track {

constexpr std::size_t kPage           = 4096;
constexpr int         kMaxPages       = 2048;     // 8 MB pre-image pool + 8 MB post-image pool
constexpr int         kMaxRegions     = 8192;
constexpr int         kMaxNoise       = 256;
constexpr std::size_t kStackWindow    = 4096;
constexpr int         kTrackedSamples = 24;
constexpr int         kMaxThreads     = 128;

struct Region { std::uintptr_t base; std::size_t size; DWORD prot; };

struct State {
    bool            active   = false;
    DWORD           tid      = 0;
    Region*         regions  = nullptr;  int nregions = 0;
    std::uintptr_t* pages    = nullptr;  int npages   = 0;  int overflow = 0;
    unsigned char*  pre      = nullptr;              // npages * kPage pre-images
    unsigned char*  post     = nullptr;              // post-images of the ORIGINAL's pages
    std::uintptr_t* pagesA   = nullptr;  int npagesA = 0;
    std::uintptr_t* noise    = nullptr;  int nnoise  = 0;
    std::uintptr_t  buf_lo = 0, buf_hi = 0;          // the pools themselves (excluded)
    std::uintptr_t  asi_lo = 0, asi_hi = 0;          // this module's image (excluded)
    std::uintptr_t  exe_base = 0;                    // MASHED.exe image base (tracked)
    PVOID           veh      = nullptr;
    bool            ready    = false;
};

inline State& S() { static State s; return s; }

inline bool Init() {
    State& s = S();
    if (s.ready) return true;
    const std::size_t bytes = sizeof(Region) * kMaxRegions
                            + sizeof(std::uintptr_t) * kMaxPages * 2
                            + sizeof(std::uintptr_t) * kMaxNoise
                            + kPage * kMaxPages * 2;
    unsigned char* p = static_cast<unsigned char*>(
        VirtualAlloc(nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!p) return false;
    s.buf_lo = reinterpret_cast<std::uintptr_t>(p);
    s.buf_hi = s.buf_lo + bytes;
    s.regions = reinterpret_cast<Region*>(p);            p += sizeof(Region) * kMaxRegions;
    s.pages   = reinterpret_cast<std::uintptr_t*>(p);    p += sizeof(std::uintptr_t) * kMaxPages;
    s.pagesA  = reinterpret_cast<std::uintptr_t*>(p);    p += sizeof(std::uintptr_t) * kMaxPages;
    s.noise   = reinterpret_cast<std::uintptr_t*>(p);    p += sizeof(std::uintptr_t) * kMaxNoise;
    s.pre     = p;                                        p += kPage * kMaxPages;
    s.post    = p;
    HMODULE self = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCSTR>(&Init), &self);
    if (self) {
        const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(self);
        const IMAGE_NT_HEADERS* nt  = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            reinterpret_cast<const unsigned char*>(self) + dos->e_lfanew);
        s.asi_lo = reinterpret_cast<std::uintptr_t>(self);
        s.asi_hi = s.asi_lo + nt->OptionalHeader.SizeOfImage;
    }
    s.exe_base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    s.ready = true;
    return true;
}

// Breadcrumbs (MASHED_SHADOW_TRACE=1): one line per stage. A crash leaves the last stage in
// shadow_ab.log, which a dead process cannot. Log() is heap-free (CreateFile/WriteFile).
inline bool Tracing() { static int v = -1; if (v < 0) v = EnvOn("MASHED_SHADOW_TRACE") ? 1 : 0; return v != 0; }
inline void Crumb(const char* fn, const char* stage, int a = 0, int b = 0) {
    if (!Tracing()) return;
    char line[160];
    wsprintfA(line, "[trace] fn=%s stage=%s a=%d b=%d\r\n", fn, stage, a, b);
    Log(line);
}

// Heap-free crumb for INSIDE the protected window: the handle is opened beforehand and only
// WriteFile (a syscall) runs while pages are read-only. wsprintfA writes its own stack only.
inline HANDLE& RawLogHandle() { static HANDLE h = INVALID_HANDLE_VALUE; return h; }
inline void RawOpen() {
    if (!Tracing() || RawLogHandle() != INVALID_HANDLE_VALUE) return;
    RawLogHandle() = CreateFileA(LogPath(), FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                                 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}
inline void RawCrumb(const char* stage, unsigned a = 0, unsigned b = 0) {
    if (!Tracing() || RawLogHandle() == INVALID_HANDLE_VALUE) return;
    char line[128];
    const int n = wsprintfA(line, "[trace] raw stage=%s a=%08x b=%08x\r\n", stage, a, b);
    DWORD wrote;
    WriteFile(RawLogHandle(), line, static_cast<DWORD>(n), &wrote, nullptr);
    FlushFileBuffers(RawLogHandle());
}


// Syscall-only crumb for use INSIDE Protect(): no wsprintfA, no CRT -- manual hex into a
// stack buffer, then WriteFile on the pre-opened handle.
inline void HexCrumb(const char* tag, unsigned a, unsigned b, unsigned c) {
    if (!Tracing() || RawLogHandle() == INVALID_HANDLE_VALUE) return;
    char line[96]; int n = 0;
    const char* pfx = "[trace] hex ";
    for (const char* q = pfx; *q; ++q) line[n++] = *q;
    for (const char* q = tag; *q && n < 40; ++q) line[n++] = *q;
    const unsigned vals[3] = { a, b, c };
    for (int k = 0; k < 3; ++k) {
        line[n++] = ' ';
        for (int sh = 28; sh >= 0; sh -= 4) line[n++] = "0123456789abcdef"[(vals[k] >> sh) & 0xf];
    }
    line[n++] = 13; line[n++] = 10;
    DWORD wrote;
    WriteFile(RawLogHandle(), line, static_cast<DWORD>(n), &wrote, nullptr);
}

inline const Region* FindRegion(std::uintptr_t a) {
    const State& s = S();
    for (int i = 0; i < s.nregions; ++i)
        if (a >= s.regions[i].base && a < s.regions[i].base + s.regions[i].size) return &s.regions[i];
    return nullptr;
}

inline LONG CALLBACK Handler(EXCEPTION_POINTERS* ep) {
    State& s = S();
    if (!s.active) return EXCEPTION_CONTINUE_SEARCH;
    const EXCEPTION_RECORD* er = ep->ExceptionRecord;
    if (er->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || er->NumberParameters < 2
        || er->ExceptionInformation[0] != 1) {               // 1 = write
        // Anything else seen while the window is open is worth a line: a DEP fault (info0=8)
        // on a page we made read-only, a guard-page hit, a breakpoint...
        char line[160];
        wsprintfA(line, "[trace] OTHER-EXC code=%08x info0=%u addr=%08x eip=%08x tid=%u\r\n",
                  static_cast<unsigned>(er->ExceptionCode),
                  er->NumberParameters ? static_cast<unsigned>(er->ExceptionInformation[0]) : 0u,
                  er->NumberParameters > 1 ? static_cast<unsigned>(er->ExceptionInformation[1]) : 0u,
                  static_cast<unsigned>(ep->ContextRecord->Eip), GetCurrentThreadId());
        Log(line);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    const std::uintptr_t addr = static_cast<std::uintptr_t>(er->ExceptionInformation[1]);
    const Region* r = FindRegion(addr);
    const std::uintptr_t page = addr & ~(kPage - 1);
    if (!r) {
        // Not ours: the process is about to die on it anyway, so say what it was.
        char line[160];
        wsprintfA(line, "[trace] UNTRACKED-FAULT addr=%08x eip=%08x tid=%u\r\n",
                  static_cast<unsigned>(addr), static_cast<unsigned>(ep->ContextRecord->Eip),
                  GetCurrentThreadId());
        Log(line);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    DWORD old = 0;
    // give the page its write permission back FIRST. Restoring PAGE_WRITECOPY on an image page
    // that is already a private copy can be refused, so fall back to PAGE_READWRITE; only if
    // both fail let the fault surface (and say so) rather than spin on it forever.
    if (!VirtualProtect(reinterpret_cast<void*>(page), kPage, r->prot, &old)
        && !VirtualProtect(reinterpret_cast<void*>(page), kPage, PAGE_READWRITE, &old)) {
        char line[160];
        wsprintfA(line, "[trace] REPROTECT-FAILED addr=%08x prot=%x err=%u eip=%08x\r\n",
                  static_cast<unsigned>(addr), static_cast<unsigned>(r->prot), GetLastError(),
                  static_cast<unsigned>(ep->ContextRecord->Eip));
        Log(line);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (GetCurrentThreadId() == s.tid) {
        if (s.npages < kMaxPages) {
            s.pages[s.npages] = page;
            std::memcpy(s.pre + static_cast<std::size_t>(s.npages) * kPage,
                        reinterpret_cast<const void*>(page), kPage);
            ++s.npages;
        } else {
            ++s.overflow;
        }
    } else if (s.nnoise < kMaxNoise) {
        s.noise[s.nnoise++] = page;
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

inline bool Overlaps(std::uintptr_t lo, std::uintptr_t hi, std::uintptr_t a, std::uintptr_t b) {
    return lo < b && a < hi;
}

// Enumerate + protect. Returns the number of regions protected.
inline int Protect() {
    State& s = S();
    s.nregions = 0; s.npages = 0; s.overflow = 0; s.nnoise = 0;
    const std::uintptr_t stack_hi = __readfsdword(0x04);   // TEB.NtTib.StackBase
    const std::uintptr_t stack_lo = __readfsdword(0x08);   // TEB.NtTib.StackLimit
    const std::uintptr_t teb      = __readfsdword(0x18);   // TEB self
    HexCrumb("stack", static_cast<unsigned>(stack_lo), static_cast<unsigned>(stack_hi), static_cast<unsigned>(teb));
    HexCrumb("excl", static_cast<unsigned>(s.buf_lo), static_cast<unsigned>(s.buf_hi), static_cast<unsigned>(s.asi_lo));
    HexCrumb("excl2", static_cast<unsigned>(s.asi_hi), static_cast<unsigned>(s.exe_base), 0);
    MEMORY_BASIC_INFORMATION mbi;
    std::uintptr_t a = 0x10000;
    while (a < 0x7fff0000u && VirtualQuery(reinterpret_cast<void*>(a), &mbi, sizeof mbi) == sizeof mbi) {
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t end  = base + mbi.RegionSize;
        const DWORD prot = mbi.Protect & 0xff;
        const bool writable = (prot == PAGE_READWRITE || prot == PAGE_WRITECOPY)
                              && !(mbi.Protect & PAGE_GUARD);
        // Private memory (heaps, Frida, WoW64 bookkeeping) is tracked only when
        // MASHED_SHADOW_PRIVATE=1, optionally limited to [MASHED_SHADOW_PRIV_LO, MASHED_SHADOW_PRIV_HI).
        // Diagnostic bisection 2026-09-10: with everything private protected the process died
        // silently inside the original's run (no fault ever reached the handler).
        static const bool  priv_on = EnvOn("MASHED_SHADOW_PRIVATE");
        static const std::uintptr_t priv_lo = [] { char b[16]; return (GetEnvironmentVariableA("MASHED_SHADOW_PRIV_LO", b, sizeof b) ? static_cast<std::uintptr_t>(strtoul(b, nullptr, 0)) : 0u); }();
        static const std::uintptr_t priv_hi = [] { char b[16]; return (GetEnvironmentVariableA("MASHED_SHADOW_PRIV_HI", b, sizeof b) ? static_cast<std::uintptr_t>(strtoul(b, nullptr, 0)) : 0xffffffffu); }();
        const bool is_priv = mbi.Type == MEM_PRIVATE && priv_on && base >= priv_lo && base < priv_hi;
        const bool wanted = mbi.State == MEM_COMMIT && writable
            && (is_priv
                || (mbi.Type == MEM_IMAGE
                    && reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) == s.exe_base));
        // The vectored-handler NODE lives in ntdll's private heap: RtlpCallVectoredHandlers
        // increments its refcount BEFORE calling us, so a write fault there can never reach
        // the handler -> nested fault inside exception dispatch -> instant death, no dump.
        // Measured 2026-09-10: the first protected region (0x000a3000, private RW) was that
        // heap, and the process died before the next crumb. Never protect that region.
        const std::uintptr_t veh_node = reinterpret_cast<std::uintptr_t>(s.veh);
        if (wanted
            && !Overlaps(base, end, s.buf_lo, s.buf_hi)
            && !Overlaps(base, end, s.asi_lo, s.asi_hi)
            && !Overlaps(base, end, veh_node, veh_node + 1)
            // EXPERIMENT 2026-09-10: everything below the exe image is loader/WoW64/process
            // bookkeeping (the 0x000a3000 region killed the process the instant it went
            // read-only, before any 32-bit fault could reach the handler). Skip it.
            && base >= s.exe_base
            && !Overlaps(base, end, stack_lo - 0x10000, stack_hi)    // stack + its guard/reserve
            && !Overlaps(base, end, teb, teb + kPage)
            && s.nregions < kMaxRegions) {
            DWORD old = 0;
            HexCrumb("prot", static_cast<unsigned>(base), static_cast<unsigned>(mbi.RegionSize), mbi.Protect | (mbi.Type << 8));
            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READONLY, &old)) {
                s.regions[s.nregions++] = Region{ base, static_cast<std::size_t>(mbi.RegionSize), old };
            }
        }
        a = end;
    }
    return s.nregions;
}

inline void Unprotect() {
    State& s = S();
    for (int i = 0; i < s.nregions; ++i) {
        DWORD old = 0;
        if (!VirtualProtect(reinterpret_cast<void*>(s.regions[i].base), s.regions[i].size, s.regions[i].prot, &old))
            VirtualProtect(reinterpret_cast<void*>(s.regions[i].base), s.regions[i].size, PAGE_READWRITE, &old);
    }
    s.nregions = 0;
}

// STOP THE WORLD. Contamination (another thread writing to a page our thread also touched,
// then our pre-image restore reverting that write) is undetectable once a page has been
// re-enabled, so every other thread is suspended for the whole compare. Deadlock caveat in
// the header comment. The toolhelp snapshot allocates, so it runs OUTSIDE the windows.
struct Others { HANDLE h[kMaxThreads]; int n = 0; };

inline void SuspendOthers(Others& o) {
    o.n = 0;
    const DWORD me = GetCurrentThreadId(), pid = GetCurrentProcessId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    THREADENTRY32 te; te.dwSize = sizeof te;
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid || te.th32ThreadID == me) continue;
            HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
            if (!h) continue;
            if (SuspendThread(h) == (DWORD)-1 || o.n >= kMaxThreads) { CloseHandle(h); continue; }
            o.h[o.n++] = h;
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
}
inline void ResumeOthers(Others& o) {
    for (int i = 0; i < o.n; ++i) { ResumeThread(o.h[i]); CloseHandle(o.h[i]); }
    o.n = 0;
}

// One tracked execution of `call`. Fills pages/pre; the caller decides what to do with them.
template <typename Fn>
inline void Execute(Fn call) {
    State& s = S();
    s.tid = GetCurrentThreadId();
    RawOpen();
    s.veh = AddVectoredExceptionHandler(1, &Handler);
    RawCrumb("veh-added", reinterpret_cast<unsigned>(s.veh));
    // The handler must be LIVE before the first page goes read-only and until the last one
    // is writable again: with threads running, a write by any other thread in that gap is an
    // unhandled fault and kills the process silently (2026-09-10: every "silent death" of the
    // first live runs was this -- three samples survived by luck, the fourth did not).
    s.active = true;
    const int nreg = Protect();
    RawCrumb("protected", static_cast<unsigned>(nreg));
    call();
    RawCrumb("call-returned", static_cast<unsigned>(s.npages));
    Unprotect();
    s.active = false;
    if (s.veh) { RemoveVectoredExceptionHandler(s.veh); s.veh = nullptr; }
    Crumb("-", "execute-done", nreg, s.npages);
}

struct Result {
    int pages_orig = 0, pages_port = 0, pages_diff = 0, noise = 0, overflow = 0;
    int stack_diff_bytes = 0;
    std::uintptr_t first_page = 0; int first_off = -1;
    int stack_first_off = -1;
};

// The full A/B: original under tracking, restore, port under tracking, compare.
template <typename O, typename P>
inline Result Compare(const char* fn, O call_orig, P call_port, void* stackwin) {
    State& s = S();
    Result r;
    Crumb(fn, "enter");
    static Others others;                 // .asi .data: untracked
    // STOP THE WORLD is opt-in (MASHED_SHADOW_STW=1). Measured 2026-09-10: with 59 threads
    // suspended, the physics-scene init root deadlocked inside the original (CPU flat,
    // window unresponsive) -- a suspended thread held a lock it needed. Default is to keep
    // threads running and DISCARD any sample where another thread faulted on a tracked page
    // (reported as NOISY, see RunTracked); contamination without a fault stays a known limit.
    static const bool stw = EnvOn("MASHED_SHADOW_STW");
    struct WorldGuard {                   // resume on every exit path
        Others& o; bool on;
        WorldGuard(Others& oo, bool en) : o(oo), on(en) { if (on) SuspendOthers(o); }
        ~WorldGuard() { if (on) ResumeOthers(o); }
    } world(others, stw);
    const std::uintptr_t stack_hi = __readfsdword(0x04);
    std::size_t win = kStackWindow;
    if (reinterpret_cast<std::uintptr_t>(stackwin) + win > stack_hi)
        win = stack_hi - reinterpret_cast<std::uintptr_t>(stackwin);
    static unsigned char stack_pre[kStackWindow], stack_post[kStackWindow];   // .asi .data: untracked
    std::memcpy(stack_pre, stackwin, win);
    Crumb(fn, "suspended+stacksnap", static_cast<int>(win), others.n);

    // -- A: original ------------------------------------------------------------
    Execute(call_orig);
    r.pages_orig = s.npages; r.noise = s.nnoise; r.overflow = s.overflow;
    Crumb(fn, "A-done", s.npages, s.nregions);
    std::memcpy(stack_post, stackwin, win);
    for (int i = 0; i < s.npages; ++i) {
        s.pagesA[i] = s.pages[i];
        std::memcpy(s.post + static_cast<std::size_t>(i) * kPage, reinterpret_cast<const void*>(s.pages[i]), kPage);
    }
    s.npagesA = s.npages;
    // restore: memory exactly as before the original ran
    for (int i = s.npages - 1; i >= 0; --i)
        std::memcpy(reinterpret_cast<void*>(s.pages[i]), s.pre + static_cast<std::size_t>(i) * kPage, kPage);
    std::memcpy(stackwin, stack_pre, win);
    Crumb(fn, "A-restored", s.npagesA);

    // -- B: port ----------------------------------------------------------------
    Execute(call_port);
    r.pages_port = s.npages; r.noise += s.nnoise; r.overflow += s.overflow;
    Crumb(fn, "B-done", s.npages);

    // -- compare ----------------------------------------------------------------
    for (int i = 0; i < s.npagesA; ++i) {
        const unsigned char* expect = s.post + static_cast<std::size_t>(i) * kPage;
        const unsigned char* live   = reinterpret_cast<const unsigned char*>(s.pagesA[i]);
        if (std::memcmp(expect, live, kPage) != 0) {
            ++r.pages_diff;
            if (r.first_off < 0) {
                r.first_page = s.pagesA[i];
                for (std::size_t k = 0; k < kPage; k += 4)
                    if (std::memcmp(expect + k, live + k, 4) != 0) { r.first_off = static_cast<int>(k); break; }
            }
        }
    }
    for (int i = 0; i < s.npages; ++i) {                     // pages only the PORT touched
        bool inA = false;
        for (int j = 0; j < s.npagesA; ++j) if (s.pagesA[j] == s.pages[i]) { inA = true; break; }
        if (inA) continue;
        const unsigned char* expect = s.pre + static_cast<std::size_t>(i) * kPage;   // == what the original left there
        const unsigned char* live   = reinterpret_cast<const unsigned char*>(s.pages[i]);
        if (std::memcmp(expect, live, kPage) != 0) {
            ++r.pages_diff;
            if (r.first_off < 0) {
                r.first_page = s.pages[i];
                for (std::size_t k = 0; k < kPage; k += 4)
                    if (std::memcmp(expect + k, live + k, 4) != 0) { r.first_off = static_cast<int>(k); break; }
            }
        }
    }
    for (std::size_t k = 0; k < win; ++k) {
        if (stack_post[k] != static_cast<const unsigned char*>(stackwin)[k]) {
            if (r.stack_first_off < 0) r.stack_first_off = static_cast<int>(k);
            ++r.stack_diff_bytes;
        }
    }
    if (RawLogHandle() != INVALID_HANDLE_VALUE) { CloseHandle(RawLogHandle()); RawLogHandle() = INVALID_HANDLE_VALUE; }
    return r;
}

template <typename Ret>
struct RetBox {
    Ret v{};
    template <typename F, typename... A> void call(F f, A... a) { v = f(a...); }
    int diff(const RetBox& o) const { return std::memcmp(&v, &o.v, sizeof(Ret)) != 0 ? 1 : 0; }
    Ret get() const { return v; }
};
template <>
struct RetBox<void> {
    template <typename F, typename... A> void call(F f, A... a) { f(a...); }
    int diff(const RetBox&) const { return 0; }
    void get() const {}
};

} // namespace Track

// -- the primitive --------------------------------------------------------------------
template <typename F, typename... A>
auto RunTracked(Counter& c, F impl, void* stackwin, A... args) -> decltype(impl(args...)) {
    using Ret = decltype(impl(args...));
    Track::RetBox<Ret> ro, rn;
    if (c.max_samples == kDefaultSamples) c.max_samples = Track::kTrackedSamples;
    if (!Armed(c) || !Track::Init()) { rn.call(impl, args...); return rn.get(); }
    Reentry guard;
    const int idx = HookIndex(c.rva);
    if (idx < 0) {
        if (!c.noted) { c.noted = true; Record(c, idx, 0, " SKIP:no-hook-index"); }
        rn.call(impl, args...); return rn.get();
    }
    const unsigned char byte_installed = *reinterpret_cast<volatile unsigned char*>(c.rva);
    HookSystem::Uninstall(static_cast<std::size_t>(idx));
    const unsigned char byte_uninstalled = *reinterpret_cast<volatile unsigned char*>(c.rva);
    Track::Result r = Track::Compare(c.name,
        [&] { ro.call(reinterpret_cast<F>(c.rva), args...); },
        [&] { rn.call(impl, args...); },
        stackwin);
    // Re-install AFTER the tracked window: Install() writes the registry entry, which lives in
    // this module's heap (a tracked private page) and would show up as a port-only page diff.
    HookSystem::Install(static_cast<std::size_t>(idx));
    if (!c.proved) {
        c.proved = true;
        char pb[112];
        wsprintfA(pb, "[--] fn=%s PATCHBYTE installed=%02x uninstalled=%02x %s", c.name,
                  byte_installed, byte_uninstalled,
                  (byte_installed == 0xE9 && byte_uninstalled != 0xE9) ? "A/B-IS-REAL" : "SUSPECT-no-restore");
        Log(pb); Log("\r\n");
    }
    const int rd = ro.diff(rn);
    const int nd = r.pages_diff + (r.stack_diff_bytes ? 1 : 0) + rd;
    if (r.noise) {
        // another thread wrote a tracked page during the window: the compare may include its
        // effect. Say so and do not count the sample (n is not advanced, ndiff untouched).
        char nl[160];
        wsprintfA(nl, "[~~] fn=%s NOISY noise=%d pages=o:%d,n:%d diff:%d (sample discarded)\r\n",
                  c.name, r.noise, r.pages_orig, r.pages_port, r.pages_diff);
        Log(nl);
        return rn.get();
    }
    char det[200];
    int dp = wsprintfA(det, " pages=o:%d,n:%d,diff:%d stack=%d noise=%d ovf=%d ret=%d",
                       r.pages_orig, r.pages_port, r.pages_diff, r.stack_diff_bytes, r.noise, r.overflow, rd);
    if (r.pages_diff) dp += wsprintfA(det + dp, " @%08x+%03x", static_cast<unsigned>(r.first_page), r.first_off);
    if (r.stack_diff_bytes) dp += wsprintfA(det + dp, " stk+%03x", r.stack_first_off);
    Record(c, idx, nd, det);
    return rn.get();
}

} // namespace ShadowAB
