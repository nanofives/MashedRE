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
    volatile DWORD  active_since = 0;               // GetTickCount at s.active=true (watchdog)
    DWORD           watchdog_tid = 0;               // never suspended by stop-the-world
    volatile int    hang_logged  = 0;
};

inline State& S() { static State s; return s; }
inline void HexCrumb(const char* tag, unsigned a, unsigned b, unsigned c);

// HANG WATCHDOG (MASHED_SHADOW_TRACE=1 only). If a tracked window stays open > 5 s, suspend
// the tracked thread, log EIP/ESP/EBP + 8 stack words + 4 code bytes at EIP, resume it. A
// process stuck under protection cannot be attached by Frida (thread injection needs loader
// structures that are read-only), so this is the only way to see where it sits.
inline DWORD WINAPI Watchdog(LPVOID) {
    for (;;) {
        Sleep(250);
        State& s = S();
        if (!s.active || s.hang_logged || !s.active_since) continue;
        if (GetTickCount() - s.active_since < 5000) continue;
        s.hang_logged = 1;
        HANDLE h = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, s.tid);
        if (!h) { HexCrumb("hang-openthread-failed", GetLastError(), 0, 0); continue; }
        if (SuspendThread(h) == (DWORD)-1) { HexCrumb("hang-suspend-failed", GetLastError(), 0, 0); CloseHandle(h); continue; }
        CONTEXT ctx; ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
        if (GetThreadContext(h, &ctx)) {
            HexCrumb("hang-eip-esp-ebp", ctx.Eip, ctx.Esp, ctx.Ebp);
            HexCrumb("hang-eax-ecx-edx", ctx.Eax, ctx.Ecx, ctx.Edx);
            const unsigned* sp = reinterpret_cast<const unsigned*>(ctx.Esp);
            MEMORY_BASIC_INFORMATION m;
            if (VirtualQuery(sp, &m, sizeof m) == sizeof m && m.State == MEM_COMMIT) {
                HexCrumb("hang-stack0-2", sp[0], sp[1], sp[2]);
                HexCrumb("hang-stack3-5", sp[3], sp[4], sp[5]);
                HexCrumb("hang-stack6-8", sp[6], sp[7], sp[8]);
            }
            const unsigned char* ip = reinterpret_cast<const unsigned char*>(ctx.Eip);
            if (VirtualQuery(ip, &m, sizeof m) == sizeof m && m.State == MEM_COMMIT)
                HexCrumb("hang-code", (ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3],
                         (ip[4] << 24) | (ip[5] << 16) | (ip[6] << 8) | ip[7], 0);
        } else {
            HexCrumb("hang-getcontext-failed", GetLastError(), 0, 0);
        }
        ResumeThread(h);
        CloseHandle(h);
    }
}

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
    if (EnvOn("MASHED_SHADOW_TRACE")) {
        DWORD wtid = 0;
        HANDLE wt = CreateThread(nullptr, 0, &Watchdog, nullptr, 0, &wtid);
        if (wt) { s.watchdog_tid = wtid; CloseHandle(wt); }
    }
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

inline void HexCrumb(const char* tag, unsigned a, unsigned b, unsigned c);

inline LONG CALLBACK Handler(EXCEPTION_POINTERS* ep) {
    State& s = S();
    if (!s.active) return EXCEPTION_CONTINUE_SEARCH;
    // trace: every fault the handler sees (first 4000 per boot), syscall-only writer
    static int traced = 0;
    if (traced < 4000 && ep->ExceptionRecord->NumberParameters > 1) {
        ++traced;
        HexCrumb("fault", static_cast<unsigned>(ep->ExceptionRecord->ExceptionInformation[1]),
                 static_cast<unsigned>(ep->ContextRecord->Eip),
                 (GetCurrentThreadId() & 0xffff) | (static_cast<unsigned>(ep->ExceptionRecord->ExceptionInformation[0]) << 16));
    }
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


// EVERY thread's 32-bit stack and TEB must stay writable, not just ours. Under WoW64 the
// 64-bit layer writes a thread's 32-bit context onto its 32-bit stack (syscall return, APC,
// exception delivery); that write happens in 64-bit code and a fault there is dispatched to
// the 64-bit chain, never to this 32-bit handler -> instant death. Measured 2026-09-10: with
// private memory protected the process died right after a 64 KB private region (a stack) went
// read-only, and with all threads suspended it deadlocked instead (suspended threads do not
// fault). Enumerate threads, read each TEB's StackLimit/StackBase/DeallocationStack.
typedef LONG (NTAPI* NtQueryInformationThread_t)(HANDLE, ULONG, PVOID, ULONG, PULONG);
struct ThreadBasicInfo32 { LONG ExitStatus; PVOID TebBaseAddress; DWORD ClientIdProcess, ClientIdThread;
                           ULONG_PTR AffinityMask; LONG Priority; LONG BasePriority; };
struct Excl { std::uintptr_t lo, hi; };
constexpr int kMaxExcl = 2 * kMaxThreads;

inline int CollectThreadExclusions(Excl* out) {
    int n = 0;
    static NtQueryInformationThread_t q = reinterpret_cast<NtQueryInformationThread_t>(
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread"));
    if (!q) return 0;
    const DWORD pid = GetCurrentProcessId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    THREADENTRY32 te; te.dwSize = sizeof te;
    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid) continue;
            HANDLE h = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
            if (!h) continue;
            ThreadBasicInfo32 tbi{};
            if (q(h, 0 /*ThreadBasicInformation*/, &tbi, sizeof tbi, nullptr) >= 0 && tbi.TebBaseAddress && n + 2 <= kMaxExcl) {
                const std::uintptr_t teb = reinterpret_cast<std::uintptr_t>(tbi.TebBaseAddress);
                const std::uintptr_t stack_base  = *reinterpret_cast<std::uintptr_t*>(teb + 0x04);
                const std::uintptr_t stack_limit = *reinterpret_cast<std::uintptr_t*>(teb + 0x08);
                const std::uintptr_t dealloc     = *reinterpret_cast<std::uintptr_t*>(teb + 0xE0C);   // DeallocationStack
                const std::uintptr_t lo = dealloc ? dealloc : (stack_limit - 0x10000);
                out[n++] = Excl{ lo, stack_base };
                out[n++] = Excl{ teb - 0x2000, teb + 0x1000 };   // TEB64 (2 pages, precedes) + TEB32
            }
            CloseHandle(h);
        } while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
    return n;
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
    static Excl excl[kMaxExcl];                      // .asi .data: untracked
    const int nexcl = CollectThreadExclusions(excl);
    // PEB32 (fs:[0x30]) and the PEB64 that precedes it: written by the 64-bit side.
    const std::uintptr_t peb = __readfsdword(0x30);
    // Any allocation that contains a PAGE_GUARD page is a stack (32-bit or the WoW64 64-bit
    // one, which no 32-bit API enumerates); the 64-bit side writes those from 64-bit code.
    static std::uintptr_t guardAllocs[512];
    int nguard = 0;
    {
        MEMORY_BASIC_INFORMATION g;
        std::uintptr_t ga = 0x10000;
        while (ga < 0x7fff0000u && VirtualQuery(reinterpret_cast<void*>(ga), &g, sizeof g) == sizeof g) {
            if (g.State == MEM_COMMIT && (g.Protect & PAGE_GUARD) && nguard < 512)
                guardAllocs[nguard++] = reinterpret_cast<std::uintptr_t>(g.AllocationBase);
            ga = reinterpret_cast<std::uintptr_t>(g.BaseAddress) + g.RegionSize;
        }
    }
    // Private memory: ONLY the committed regions of the 32-bit heaps (GetProcessHeaps +
    // HeapWalk, valid because the heaps are locked by this thread under stop-the-world).
    // Everything else private -- WoW64 / 64-bit ntdll heaps, Frida, loader bookkeeping -- is
    // memory the 64-bit exception path itself may write; protecting it made the very first
    // fault loop inside 64-bit code and never reach this 32-bit handler (2026-09-10, 0 faults,
    // thread parked in a wait). The game's CRT and RenderWare allocations live in these heaps.
    static Excl heapRanges[1024];
    int nheap = 0;
    if (EnvOn("MASHED_SHADOW_PRIVATE")) {
        HANDLE hs[64];
        const int nh = static_cast<int>(GetProcessHeaps(64, hs));
        for (int i = 0; i < nh && i < 64; ++i) {
            PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
            while (HeapWalk(hs[i], &e)) {
                if ((e.wFlags & PROCESS_HEAP_REGION) && nheap < 1024) {
                    const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(e.Region.lpFirstBlock);
                    const std::uintptr_t hi = reinterpret_cast<std::uintptr_t>(e.Region.lpLastBlock);
                    if (hi > lo) heapRanges[nheap++] = Excl{ lo & ~(kPage - 1), (hi + kPage - 1) & ~(kPage - 1) };
                }
            }
        }
    }
    HexCrumb("threads", static_cast<unsigned>(nexcl / 2), static_cast<unsigned>(nguard), static_cast<unsigned>(nheap));
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
        const bool in_heap = [&] { for (int k = 0; k < nheap; ++k) if (Overlaps(base, end, heapRanges[k].lo, heapRanges[k].hi)) return true; return false; }();
        const bool is_priv = mbi.Type == MEM_PRIVATE && priv_on && in_heap && base >= priv_lo && base < priv_hi;
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
            && [&] { for (int k = 0; k < nexcl; ++k) if (Overlaps(base, end, excl[k].lo, excl[k].hi)) return false; return true; }()
            && !Overlaps(base, end, peb - 0x2000, peb + 0x1000)
            && [&] { const std::uintptr_t ab = reinterpret_cast<std::uintptr_t>(mbi.AllocationBase);
                     for (int k = 0; k < nguard; ++k) if (guardAllocs[k] == ab) return false; return true; }()
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
            if (te.th32OwnerProcessID != pid || te.th32ThreadID == me || te.th32ThreadID == S().watchdog_tid) continue;
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

// HEAP LOCKS BEFORE STOP-THE-WORLD. Both deadlocks measured 2026-09-10 were the heap lock:
// threads running -> the tracked thread blocked in an ntdll wait (watchdog: EIP in a syscall
// stub, stack full of ntdll frames) because another thread that had faulted held it; threads
// suspended -> the original blocked on a lock a suspended thread held. Heap critical sections
// are recursive: if OUR thread acquires every heap's lock first, no other thread can be inside
// a heap when we suspend them, and the original/port on our thread still allocate freely.
struct Heaps { HANDLE h[64]; int n = 0; int locked = 0; };
inline void LockHeaps(Heaps& hp) {
    hp.n = static_cast<int>(GetProcessHeaps(64, hp.h));
    if (hp.n > 64) hp.n = 64;
    hp.locked = 0;
    for (int i = 0; i < hp.n; ++i) if (HeapLock(hp.h[i])) ++hp.locked;
}
inline void UnlockHeaps(Heaps& hp) {
    for (int i = 0; i < hp.n; ++i) HeapUnlock(hp.h[i]);
    hp.n = 0; hp.locked = 0;
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
    s.active_since = GetTickCount(); s.hang_logged = 0;
    const int nreg = Protect();
    RawCrumb("protected", static_cast<unsigned>(nreg));
    call();
    RawCrumb("call-returned", static_cast<unsigned>(s.npages));
    Unprotect();
    s.active = false; s.active_since = 0;
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
    // Stop-the-world is ON whenever private memory is tracked (the only configuration in which
    // other threads' heap writes and the WoW64 64-bit side can corrupt the compare), and
    // otherwise opt-in via MASHED_SHADOW_STW=1. Heap locks are taken first (see LockHeaps).
    static const bool stw = EnvOn("MASHED_SHADOW_STW");   // private mode now filters to 32-bit heap pages; try threads running first
    static Heaps heaps;                   // .asi .data: untracked
    struct WorldGuard {                   // resume + unlock on every exit path
        Others& o; Heaps& hp; bool on;
        WorldGuard(Others& oo, Heaps& hh, bool en) : o(oo), hp(hh), on(en) {
            if (on) { LockHeaps(hp); SuspendOthers(o); }
        }
        ~WorldGuard() { if (on) { ResumeOthers(o); UnlockHeaps(hp); } }
    } world(others, heaps, stw);
    const std::uintptr_t stack_hi = __readfsdword(0x04);
    std::size_t win = kStackWindow;
    if (reinterpret_cast<std::uintptr_t>(stackwin) + win > stack_hi)
        win = stack_hi - reinterpret_cast<std::uintptr_t>(stackwin);
    static unsigned char stack_pre[kStackWindow], stack_post[kStackWindow];   // .asi .data: untracked
    std::memcpy(stack_pre, stackwin, win);
    Crumb(fn, "suspended+stacksnap", static_cast<int>(win), others.n);
    if (stw) Crumb(fn, "heaps-locked", heaps.locked, heaps.n);

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
