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
//     For allocations through the RwGlobals slots +0x108 / +0x114 MASHED_SHADOW_ALLOC=1
//     pairs and masks them (ALLOCATOR-AWARE COMPARE below). Other allocators are not covered.
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
#include <algorithm>
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
    std::uintptr_t  exe_hi   = 0;                    // MASHED.exe image end
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
    {
        const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(s.exe_base);
        const IMAGE_NT_HEADERS* nt  = reinterpret_cast<const IMAGE_NT_HEADERS*>(s.exe_base + dos->e_lfanew);
        s.exe_hi = s.exe_base + nt->OptionalHeader.SizeOfImage;
    }
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

// [D4-WSB-SHADOW-PRIVATE] VEH HOLES. RtlpCallVectoredHandlers holds the VEH list's SRW lock
// while it bumps the entry's refcount, and on Win10+ that refcount is a SEPARATE heap block
// (entry+8 on x86), not part of the node. With heap pages protected, the bump faulted, the nested
// dispatch asked for the same SRW lock, and the thread waited on itself: 0 faults reached Handler,
// EIP in the NtWaitForAlertByThreadId stub (verify/wsbshadowpriv_20261027, diag1, 2026-10-27).
// Walk the list from our node (Flink until the image-resident list head) and keep writable, as
// page holes, every entry and every block a dword of it points into. Layout-free: a dword is
// taken only if it points into committed private memory (the encoded handler rarely does, and a
// spare hole only costs coverage of one page).
constexpr int kMaxHoles = 64;
inline bool IsPrivateCommitted(std::uintptr_t a) {
    MEMORY_BASIC_INFORMATION m;
    return a >= 0x10000 && VirtualQuery(reinterpret_cast<void*>(a), &m, sizeof m) == sizeof m
        && m.State == MEM_COMMIT && m.Type == MEM_PRIVATE && !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS));
}
inline void AddHole(std::uintptr_t* holes, int& n, std::uintptr_t lo, std::size_t len) {
    for (std::uintptr_t p = lo & ~(kPage - 1); p < lo + len && n < kMaxHoles; p += kPage) {
        bool dup = false;
        for (int k = 0; k < n; ++k) if (holes[k] == p) { dup = true; break; }
        if (!dup) holes[n++] = p;
    }
}
inline int CollectVehHoles(std::uintptr_t node, std::uintptr_t* holes) {
    int n = 0;
    std::uintptr_t e = node;
    for (int hop = 0; hop < 16 && e && IsPrivateCommitted(e) && IsPrivateCommitted(e + 0x1f); ++hop) {
        const std::uintptr_t* d = reinterpret_cast<const std::uintptr_t*>(e);
        AddHole(holes, n, e, 0x20);
        for (int k = 0; k < 8; ++k)
            if (IsPrivateCommitted(d[k])) AddHole(holes, n, d[k], 4);
        if (hop == 0) HexCrumb("veh-node", static_cast<unsigned>(d[0]), static_cast<unsigned>(d[1]), static_cast<unsigned>(d[2]));
        e = d[0];                                    // Flink; the list head is in ntdll's image -> stop
        if (e == node) break;
    }
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
    static std::uintptr_t holes[kMaxHoles];
    const int nholes = CollectVehHoles(reinterpret_cast<std::uintptr_t>(s.veh), holes);
    HexCrumb("veh-holes", static_cast<unsigned>(nholes), nholes ? static_cast<unsigned>(holes[0]) : 0u,
             nholes > 1 ? static_cast<unsigned>(holes[1]) : 0u);
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
        // The vectored-handler NODE and its refcount block live in the process heap:
        // RtlpCallVectoredHandlers writes them BEFORE calling us, so a write fault there can
        // never reach the handler. Measured 2026-09-10: the first protected region (0x000a3000,
        // private RW) was that heap, and the process died before the next crumb. Those pages
        // are holes (CollectVehHoles); the rest of their region is protected around them.
        if (wanted
            && !Overlaps(base, end, s.buf_lo, s.buf_hi)
            && !Overlaps(base, end, s.asi_lo, s.asi_hi)
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
            HexCrumb("prot", static_cast<unsigned>(base), static_cast<unsigned>(mbi.RegionSize), mbi.Protect | (mbi.Type << 8));
            // protect [base, end) in runs that skip the hole pages
            std::uintptr_t run = base;
            for (std::uintptr_t p = base; p <= end && s.nregions < kMaxRegions; p += kPage) {
                bool hole = false;
                if (p < end) for (int k = 0; k < nholes; ++k) if (holes[k] == p) { hole = true; break; }
                if (p < end && !hole) continue;
                if (p > run) {
                    DWORD old = 0;
                    if (VirtualProtect(reinterpret_cast<void*>(run), p - run, PAGE_READONLY, &old))
                        s.regions[s.nregions++] = Region{ run, static_cast<std::size_t>(p - run), old };
                }
                run = p + kPage;
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

// [HB-SHADOW-ALLOC-COMPARE] ALLOCATOR-AWARE COMPARE (MASHED_SHADOW_ALLOC=1, default off).
// Restoring the pre-images restores the heap's bookkeeping, but the heap still hands the second
// run a different block for the same request (D4-WSB-PORT-BROAD-2: 0x0055ded0 got 0x0f35d6d8 on
// the original's run, 0x0f35d648 on the port's, from the same restored state). So a body that
// allocates through the RwGlobals (DAT_007d3ff8) slots +0x108 (size, hint) or +0x114 (n, size,
// hint) cannot match bit for bit. With the knob on, for the length of the compare:
//   1. both slots point at the wrappers below; calls from the tracked thread inside a window are
//      logged (slot, args, result) and every dword the ALLOCATOR wrote outside the block it
//      returned is added to a mask (heap headers, bitmaps, counters);
//   2. the k-th allocation of the original's run is paired with the k-th of the port's. Count,
//      slot and arguments must agree, otherwise the sample counts one difference (alloc mismatch);
//   3. dwords inside a paired block are left out of the page compare and compared block against
//      block instead (original's block k after its run vs the port's block k now);
//   4. any compared dword passes when original = A_k + d and port = B_k + d, 0 <= d <= size_k
//      (pointers to paired blocks), in pages, block contents, the stack window and the return;
//   5. a block dword that NEITHER run changed from its pre-allocation value passes (uninitialised
//      memory: stale heap data, different in the two blocks by construction).
// LIMITS: a dword the body writes inside the mask is not compared; a block dword the original
// writes with the exact stale value of its own block while the port leaves its own stale value
// passes (5); allocations by any other route are not paired.
constexpr int           kMaxAllocs    = 64;
constexpr int           kMaxMask      = 16384;
constexpr std::uintptr_t kRwGlobalsPtr = 0x007d3ff8;    // DAT_007d3ff8 = RwGlobals*
constexpr std::uint32_t kSlotMalloc   = 0x108;           // (size, hint)
constexpr std::uint32_t kSlotCalloc   = 0x114;           // (n, size, hint)
constexpr std::uint32_t kSlotFree     = 0x10c;           // (ptr): L2_0057c550.cpp 0x0057c577..0x0057c584

struct AllocCall { std::uint32_t slot, a0, a1, a2, size, ret; };
struct AllocLog  { AllocCall c[kMaxAllocs]; int n; int ovf; };
struct AllocState {                                      // .asi .data: untracked
    bool            on = false;
    int             run = -1;                            // 0 = original's window, 1 = port's, -1 = none
    std::uintptr_t  g = 0;
    std::uint32_t   real_malloc = 0, real_calloc = 0, real_free = 0;   // free = slot +0x10c (ptr), read, never swapped
    int             pin_tries = 0, pin_miss = 0;         // [Amendment 1] residue pin (port's run)
    AllocLog        log[2];
    int             npairs = 0;
    std::uintptr_t* mask = nullptr;  int nmask = 0;  int mask_ovf = 0;
    unsigned char*  snap = nullptr;                      // kMaxPages pages: touched pages before an allocator call
};
inline AllocState& AS() { static AllocState a; return a; }

inline int AllocSnap() {
    State& s = S(); AllocState& a = AS();
    const int np = s.npages;
    for (int i = 0; i < np; ++i)
        std::memcpy(a.snap + static_cast<std::size_t>(i) * kPage, reinterpret_cast<const void*>(s.pages[i]), kPage);
    return np;
}
inline void AllocNote(std::uint32_t slot, std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                      std::uint32_t size, std::uint32_t ret, int np0) {
    State& s = S(); AllocState& a = AS();
    for (int i = 0; i < s.npages; ++i) {
        const unsigned char* before = i < np0 ? a.snap + static_cast<std::size_t>(i) * kPage
                                              : s.pre + static_cast<std::size_t>(i) * kPage;
        const unsigned char* now = reinterpret_cast<const unsigned char*>(s.pages[i]);
        for (std::size_t k = 0; k < kPage; k += 4) {
            if (std::memcmp(before + k, now + k, 4) == 0) continue;
            const std::uintptr_t x = s.pages[i] + k;
            if (ret && x - ret < size) continue;          // the block itself: compared block vs block
            if (a.nmask < kMaxMask) a.mask[a.nmask++] = x; else ++a.mask_ovf;
        }
    }
    AllocLog& L = a.log[a.run];
    if (L.n < kMaxAllocs) L.c[L.n++] = AllocCall{ slot, a0, a1, a2, size, ret };
    else ++L.ovf;
}
inline bool AllocMine() {
    const State& s = S();
    return AS().run >= 0 && s.active && GetCurrentThreadId() == s.tid;
}
// [Amendment 1] RESIDUE PIN, port's run only. An allocator layered on the slot can derive its
// result from the block ADDRESS: 0x00564130 takes size+align from slot +0x108 and returns
// raw + (align - raw % align) (pad = align when the remainder is 0) with the pad byte at ret-1, so
// a block at ...a08 hands back +0x8 and one at ...8a0 hands back +0x10 (0x0057c500, arm S). When
// the port's k-th request matches the original's k-th, the wrapper re-asks the allocator until the
// block's address mod 16 equals the original's (at most kPinTries extra blocks), then frees the
// misses through slot +0x10c. All of it runs inside the wrapper, so its writes land in the mask.
constexpr int kPinTries = 8;
template <typename Call>
inline std::uint32_t AllocPinned(std::uint32_t slot, std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, Call call) {
    AllocState& a = AS();
    std::uint32_t r = call();
    const int k = a.log[1].n;
    if (a.run != 1 || !r || k >= a.log[0].n) return r;
    const AllocCall& A = a.log[0].c[k];
    if (A.slot != slot || A.a0 != a0 || A.a1 != a1 || A.a2 != a2 || !A.ret) return r;
    std::uint32_t held[kPinTries]; int nh = 0;
    while (r && (r & 0xf) != (A.ret & 0xf) && nh < kPinTries) { held[nh++] = r; r = call(); }
    typedef void (__cdecl* Free)(std::uint32_t);
    for (int i = 0; i < nh; ++i) reinterpret_cast<Free>(a.real_free)(held[i]);
    a.pin_tries += nh;
    if (r && (r & 0xf) != (A.ret & 0xf)) ++a.pin_miss;
    return r;
}
inline std::uint32_t __cdecl AllocWrapMalloc(std::uint32_t a0, std::uint32_t a1) {
    typedef std::uint32_t (__cdecl* F)(std::uint32_t, std::uint32_t);
    const F real = reinterpret_cast<F>(AS().real_malloc);
    if (!AllocMine()) return real(a0, a1);
    const int np0 = AllocSnap();
    const std::uint32_t r = AllocPinned(kSlotMalloc, a0, a1, 0, [&] { return real(a0, a1); });
    AllocNote(kSlotMalloc, a0, a1, 0, a0, r, np0);
    return r;
}
inline std::uint32_t __cdecl AllocWrapCalloc(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) {
    typedef std::uint32_t (__cdecl* F)(std::uint32_t, std::uint32_t, std::uint32_t);
    const F real = reinterpret_cast<F>(AS().real_calloc);
    if (!AllocMine()) return real(a0, a1, a2);
    const int np0 = AllocSnap();
    const std::uint32_t r = AllocPinned(kSlotCalloc, a0, a1, a2, [&] { return real(a0, a1, a2); });
    AllocNote(kSlotCalloc, a0, a1, a2, a0 * a1, r, np0);
    return r;
}

// Swap the slots in (outside the windows). False = knob off or RwGlobals not up yet.
inline bool AllocArm() {
    static const bool want = EnvOn("MASHED_SHADOW_ALLOC");
    AllocState& a = AS();
    a.on = false; a.run = -1; a.npairs = 0; a.nmask = 0; a.mask_ovf = 0; a.pin_tries = 0; a.pin_miss = 0;
    a.log[0].n = a.log[0].ovf = a.log[1].n = a.log[1].ovf = 0;
    if (!want) return false;
    if (!a.snap) {
        a.snap = static_cast<unsigned char*>(VirtualAlloc(nullptr, kPage * kMaxPages + sizeof(std::uintptr_t) * kMaxMask,
                                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (!a.snap) return false;
        a.mask = reinterpret_cast<std::uintptr_t*>(a.snap + kPage * kMaxPages);
    }
    a.g = *reinterpret_cast<const std::uintptr_t*>(kRwGlobalsPtr);
    if (!a.g) return false;
    std::uint32_t* sm = reinterpret_cast<std::uint32_t*>(a.g + kSlotMalloc);
    std::uint32_t* sc = reinterpret_cast<std::uint32_t*>(a.g + kSlotCalloc);
    a.real_malloc = *sm; a.real_calloc = *sc;
    a.real_free = *reinterpret_cast<const std::uint32_t*>(a.g + kSlotFree);
    *sm = reinterpret_cast<std::uint32_t>(&AllocWrapMalloc);
    *sc = reinterpret_cast<std::uint32_t>(&AllocWrapCalloc);
    a.on = true;
    return true;
}
inline void AllocDisarm() {
    AllocState& a = AS();
    if (!a.on) return;
    *reinterpret_cast<std::uint32_t*>(a.g + kSlotMalloc) = a.real_malloc;
    *reinterpret_cast<std::uint32_t*>(a.g + kSlotCalloc) = a.real_calloc;
    a.run = -1;
}
// Pair run A with run B. Returns 1 on a mismatch (count, slot, arguments, or log overflow);
// pairs are used only when this returns 0. The mask is sorted for AllocMasked.
inline int AllocPair() {
    AllocState& a = AS();
    std::sort(a.mask, a.mask + a.nmask);
    a.npairs = 0;
    const AllocLog& A = a.log[0]; const AllocLog& B = a.log[1];
    if (A.ovf || B.ovf || A.n != B.n) return 1;
    for (int k = 0; k < A.n; ++k)
        if (A.c[k].slot != B.c[k].slot || A.c[k].a0 != B.c[k].a0 || A.c[k].a1 != B.c[k].a1 || A.c[k].a2 != B.c[k].a2)
            return 1;
    a.npairs = A.n;
    return 0;
}
inline bool AllocMasked(std::uintptr_t x) {
    const AllocState& a = AS();
    return a.on && a.nmask && std::binary_search(a.mask, a.mask + a.nmask, x);
}
inline bool AllocInBlock(std::uintptr_t x) {
    const AllocState& a = AS();
    if (!a.on) return false;
    for (int k = 0; k < a.npairs; ++k) {
        const AllocCall& A = a.log[0].c[k]; const AllocCall& B = a.log[1].c[k];
        if ((A.ret && x - A.ret < A.size) || (B.ret && x - B.ret < B.size)) return true;
    }
    return false;
}
// o = the original's value, n = the port's: equal, or the same offset into paired blocks.
inline bool AllocSame(std::uint32_t o, std::uint32_t n) {
    if (o == n) return true;
    const AllocState& a = AS();
    if (!a.on) return false;
    for (int k = 0; k < a.npairs; ++k) {
        const AllocCall& A = a.log[0].c[k]; const AllocCall& B = a.log[1].c[k];
        if (A.ret && B.ret && o - A.ret <= A.size && n - B.ret == o - A.ret) return true;
    }
    return false;
}

struct Result {
    int pages_orig = 0, pages_port = 0, pages_diff = 0, noise = 0, overflow = 0;
    int heap_orig = 0, heap_port = 0;                    // [D4-WSB-SHADOW-PRIVATE] pages outside MASHED.exe's image
    int stack_diff_bytes = 0;
    std::uintptr_t first_page = 0; int first_off = -1;
    int stack_first_off = -1;
    // [D4-WSB-BVH-FU] values on the first differing page: how many dwords differ, and the first
    // kDw of them as (offset, pre, original's, port's). pre = the value before either run.
    static const int kDw = 4;
    int ndw = 0, nlog = 0;
    struct Dw { int off; std::uint32_t pre, o, n; } dw[kDw];
    // [HB-SHADOW-ALLOC-COMPARE] allocator-aware compare (MASHED_SHADOW_ALLOC=1)
    bool alloc_on = false;
    int alloc_n = 0, alloc_mm = 0, alloc_mask = 0, alloc_mask_ovf = 0, alloc_mut = 0;
    int blk_dw = 0;                                      // differing dwords in paired blocks
    std::uintptr_t blk_first = 0; std::uint32_t blk_o = 0, blk_n = 0;
};

// One dword of a compared page. Masked allocator writes and paired blocks are skipped (blocks are
// compared on their own); otherwise equal, or the same offset into a paired block. With the
// allocator compare off this is plain equality.
inline bool DwordSame(std::uintptr_t x, const unsigned char* expect, const unsigned char* live) {
    std::uint32_t o, n;
    std::memcpy(&o, expect, 4); std::memcpy(&n, live, 4);
    if (o == n) return true;
    return AllocMasked(x) || AllocInBlock(x) || AllocSame(o, n);
}

// Fill r's value fields for one differing page. expect = the original's post state, live = the
// port's, pre = the state before either run (or nullptr when that equals live: B never touched it).
inline void NoteDwords(Result& r, std::uintptr_t page, const unsigned char* expect, const unsigned char* live,
                       const unsigned char* pre) {
    for (std::size_t k = 0; k < kPage; k += 4) {
        if (DwordSame(page + k, expect + k, live + k)) continue;
        if (r.nlog < Result::kDw) {
            Result::Dw& d = r.dw[r.nlog++];
            d.off = static_cast<int>(k);
            std::memcpy(&d.o, expect + k, 4);
            std::memcpy(&d.n, live + k, 4);
            std::memcpy(&d.pre, (pre ? pre : live) + k, 4);
        }
        ++r.ndw;
    }
}

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

    const bool alloc = AllocArm();                       // [HB-SHADOW-ALLOC-COMPARE]
    struct AllocGuard { bool on; ~AllocGuard() { if (on) AllocDisarm(); } } alloc_guard{ alloc };

    // -- A: original ------------------------------------------------------------
    if (alloc) AS().run = 0;
    Execute(call_orig);
    if (alloc) AS().run = -1;
    r.pages_orig = s.npages; r.noise = s.nnoise; r.overflow = s.overflow;
    for (int i = 0; i < s.npages; ++i) if (s.pages[i] < s.exe_base || s.pages[i] >= s.exe_hi) ++r.heap_orig;
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
    if (alloc) AS().run = 1;
    Execute(call_port);
    if (alloc) AS().run = -1;
    r.pages_port = s.npages; r.noise += s.nnoise; r.overflow += s.overflow;
    for (int i = 0; i < s.npages; ++i) if (s.pages[i] < s.exe_base || s.pages[i] >= s.exe_hi) ++r.heap_port;
    Crumb(fn, "B-done", s.npages);
    if (alloc) {
        r.alloc_on = true;
        r.alloc_mm = AllocPair();
        r.alloc_n = AS().log[0].n > AS().log[1].n ? AS().log[0].n : AS().log[1].n;
        r.alloc_mask = AS().nmask; r.alloc_mask_ovf = AS().mask_ovf;
    }

    // [HB-SHADOW-ALLOC-COMPARE] NEGATIVE CONTROL, MASHED_SHADOW_ALLOC_MUT=1|2 (harness only, off by
    // default). After the port's run and before the compare: 1 = add 1 to the first dword of the
    // port's first paired block; 2 = add 4 to the first dword in the port's touched pages (outside
    // blocks and mask) that holds the port's first block address. Reverted right after the compare,
    // so the game keeps the port's real state. Expected: the sample DIVERGES (blk / page diff).
    std::uint32_t* mut_at = nullptr; std::uint32_t mut_old = 0; int mut_kind = 0;
    if (alloc && AS().npairs && !r.alloc_mm) {
        static const int mut = [] { char b[8]; return GetEnvironmentVariableA("MASHED_SHADOW_ALLOC_MUT", b, sizeof b) ? atoi(b) : 0; }();
        const std::uint32_t b0 = AS().log[1].c[0].ret;
        if (mut == 1 && b0 && AS().log[1].c[0].size >= 4) {
            mut_at = reinterpret_cast<std::uint32_t*>(b0);
        } else if (mut == 2 && b0) {
            for (int i = 0; i < s.npages && !mut_at; ++i)
                for (std::size_t k = 0; k < kPage && !mut_at; k += 4) {
                    const std::uintptr_t x = s.pages[i] + k;
                    if (*reinterpret_cast<const std::uint32_t*>(x) == b0 && !AllocInBlock(x) && !AllocMasked(x))
                        mut_at = reinterpret_cast<std::uint32_t*>(x);
                }
        }
        if (mut_at) { mut_old = *mut_at; *mut_at = mut_old + (mut == 1 ? 1u : 4u); mut_kind = mut; }
    }
    struct MutGuard { std::uint32_t* at; std::uint32_t v; ~MutGuard() { if (at) *at = v; } } mut_guard{ mut_at, mut_old };
    r.alloc_mut = mut_kind;

    // -- compare ----------------------------------------------------------------
    // A page differs when one of its dwords is not DwordSame (plain equality with the allocator
    // compare off, so this is the memcmp verdict of the earlier code, dword by dword).
    auto page_diff = [&](std::uintptr_t page, const unsigned char* expect, const unsigned char* live) -> int {
        for (std::size_t k = 0; k < kPage; k += 4)
            if (!DwordSame(page + k, expect + k, live + k)) return static_cast<int>(k);
        return -1;
    };
    for (int i = 0; i < s.npagesA; ++i) {
        const unsigned char* expect = s.post + static_cast<std::size_t>(i) * kPage;
        const unsigned char* live   = reinterpret_cast<const unsigned char*>(s.pagesA[i]);
        const int off = page_diff(s.pagesA[i], expect, live);
        if (off >= 0) {
            ++r.pages_diff;
            if (r.first_off < 0) {
                r.first_page = s.pagesA[i];
                r.first_off = off;
                const unsigned char* pre = nullptr;              // B's pre snapshot of this page, if B touched it
                for (int j = 0; j < s.npages; ++j)
                    if (s.pages[j] == s.pagesA[i]) { pre = s.pre + static_cast<std::size_t>(j) * kPage; break; }
                NoteDwords(r, s.pagesA[i], expect, live, pre);
            }
        }
    }
    for (int i = 0; i < s.npages; ++i) {                     // pages only the PORT touched
        bool inA = false;
        for (int j = 0; j < s.npagesA; ++j) if (s.pagesA[j] == s.pages[i]) { inA = true; break; }
        if (inA) continue;
        const unsigned char* expect = s.pre + static_cast<std::size_t>(i) * kPage;   // == what the original left there
        const unsigned char* live   = reinterpret_cast<const unsigned char*>(s.pages[i]);
        const int off = page_diff(s.pages[i], expect, live);
        if (off >= 0) {
            ++r.pages_diff;
            if (r.first_off < 0) {
                r.first_page = s.pages[i];
                r.first_off = off;
                NoteDwords(r, s.pages[i], expect, live, expect); // the original never wrote it: pre == its post
            }
        }
    }
    if (alloc) {                                             // paired blocks: A_k after A vs B_k now
        // pre(x): the state before either run = B's pre-image if B touched the page, else live
        // (A's pages were restored). postA(x): A's post-image if A touched the page, else pre(x).
        auto find = [](const std::uintptr_t* pages, int n, std::uintptr_t pg) {
            for (int j = 0; j < n; ++j) if (pages[j] == pg) return j;
            return -1;
        };
        auto pre_at = [&](std::uintptr_t x, std::uint32_t& v) {
            const int j = find(s.pages, s.npages, x & ~(kPage - 1));
            std::memcpy(&v, j >= 0 ? s.pre + static_cast<std::size_t>(j) * kPage + (x & (kPage - 1))
                                   : reinterpret_cast<const unsigned char*>(x), 4);
        };
        auto postA_at = [&](std::uintptr_t x, std::uint32_t& v) {
            const int j = find(s.pagesA, s.npagesA, x & ~(kPage - 1));
            if (j >= 0) std::memcpy(&v, s.post + static_cast<std::size_t>(j) * kPage + (x & (kPage - 1)), 4);
            else pre_at(x, v);
        };
        const AllocState& a = AS();
        for (int k = 0; k < a.npairs; ++k) {
            const AllocCall& A = a.log[0].c[k]; const AllocCall& B = a.log[1].c[k];
            if (!A.ret || !B.ret) { if (A.ret != B.ret) ++r.blk_dw; continue; }
            for (std::uint32_t d = 0; d + 4 <= A.size; d += 4) {
                std::uint32_t o, po, n, pn;
                postA_at(A.ret + d, o); pre_at(A.ret + d, po);
                std::memcpy(&n, reinterpret_cast<const void*>(B.ret + d), 4); pre_at(B.ret + d, pn);
                if (AllocSame(o, n) || (o == po && n == pn)) continue;
                if (!r.blk_dw) { r.blk_first = A.ret + d; r.blk_o = o; r.blk_n = n; }
                ++r.blk_dw;
            }
            for (std::uint32_t d = A.size & ~3u; d < A.size; ++d) {      // tail bytes (size not a multiple of 4)
                std::uint32_t o, po, n, pn;
                postA_at((A.ret + d) & ~3u, o); pre_at((A.ret + d) & ~3u, po);
                std::memcpy(&n, reinterpret_cast<const void*>((B.ret + d) & ~3u), 4); pre_at((B.ret + d) & ~3u, pn);
                const int sh = static_cast<int>(((A.ret + d) & 3u) * 8);
                const int shn = static_cast<int>(((B.ret + d) & 3u) * 8);
                const unsigned ob = (o >> sh) & 0xff, pob = (po >> sh) & 0xff, nb = (n >> shn) & 0xff, pnb = (pn >> shn) & 0xff;
                if (ob == nb || (ob == pob && nb == pnb)) continue;
                if (!r.blk_dw) { r.blk_first = A.ret + d; r.blk_o = ob; r.blk_n = nb; }
                ++r.blk_dw;
            }
        }
    }
    if (alloc) {                                             // stack window dword by dword (4-aligned)
        std::size_t k = 0;
        for (; k + 4 <= win; k += 4) {
            std::uint32_t o, n;
            std::memcpy(&o, stack_post + k, 4); std::memcpy(&n, static_cast<const unsigned char*>(stackwin) + k, 4);
            if (AllocSame(o, n)) continue;
            for (int b = 0; b < 4; ++b)
                if (stack_post[k + b] != static_cast<const unsigned char*>(stackwin)[k + b]) {
                    if (r.stack_first_off < 0) r.stack_first_off = static_cast<int>(k + b);
                    ++r.stack_diff_bytes;
                }
        }
        for (; k < win; ++k)
            if (stack_post[k] != static_cast<const unsigned char*>(stackwin)[k]) {
                if (r.stack_first_off < 0) r.stack_first_off = static_cast<int>(k);
                ++r.stack_diff_bytes;
            }
    } else {
        for (std::size_t k = 0; k < win; ++k) {
            if (stack_post[k] != static_cast<const unsigned char*>(stackwin)[k]) {
                if (r.stack_first_off < 0) r.stack_first_off = static_cast<int>(k);
                ++r.stack_diff_bytes;
            }
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
    std::uint32_t raw() const { std::uint32_t u = 0; std::memcpy(&u, &v, sizeof(Ret) < 4 ? sizeof(Ret) : 4); return u; }
    static constexpr bool kHas = true;
    static constexpr bool kWord = sizeof(Ret) == 4;     // [HB-SHADOW-ALLOC-COMPARE] return can be a block pointer
};
template <>
struct RetBox<void> {
    template <typename F, typename... A> void call(F f, A... a) { f(a...); }
    int diff(const RetBox&) const { return 0; }
    void get() const {}
    std::uint32_t raw() const { return 0; }
    static constexpr bool kHas = false;
    static constexpr bool kWord = false;
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
    int rd = ro.diff(rn);
    // [HB-SHADOW-ALLOC-COMPARE] a 4-byte return at the same offset into paired blocks is equal
    if (rd && r.alloc_on && Track::RetBox<Ret>::kWord && Track::AllocSame(ro.raw(), rn.raw())) rd = 0;
    // [D4-WSB-SHADOW-PRIVATE] MASHED_SHADOW_NEED_PAGES=1: a sample in which neither run wrote a
    // tracked page (and stack + return agree) is discarded, so the budget goes to calls that
    // write. At most 2000 discards per function (then sampling stops), the first 64 logged.
    static const bool need_pages = EnvOn("MASHED_SHADOW_NEED_PAGES");
    if (need_pages && !r.noise && !r.pages_orig && !r.pages_port && !rd && !r.stack_diff_bytes) {
        static int nz = 0;                                // [addendum 2] rv != 0 discards, all logged up to 256
        const bool nonzero = ro.raw() != 0 && nz < 256;
        if (nonzero) ++nz;
        if (c.empty < 64 || nonzero || (c.empty % 250) == 0) {
            char el[160];
            wsprintfA(el, "[..] fn=%s EMPTY rv=%08x (sample discarded, %d)\r\n", c.name, ro.raw(), c.empty);
            Log(el);
        }
        if (++c.empty >= 2000) c.n = c.max_samples;
        return rn.get();
    }
    const int nd = r.pages_diff + (r.stack_diff_bytes ? 1 : 0) + rd + r.alloc_mm + (r.blk_dw ? 1 : 0);
    if (r.noise) {
        // another thread wrote a tracked page during the window: the compare may include its
        // effect. Say so and do not count the sample (n is not advanced, ndiff untouched).
        char nl[160];
        wsprintfA(nl, "[~~] fn=%s NOISY noise=%d pages=o:%d,n:%d diff:%d (sample discarded)\r\n",
                  c.name, r.noise, r.pages_orig, r.pages_port, r.pages_diff);
        Log(nl);
        return rn.get();
    }
    char det[400];
    int dp = wsprintfA(det, " pages=o:%d,n:%d,diff:%d stack=%d noise=%d ovf=%d ret=%d",
                       r.pages_orig, r.pages_port, r.pages_diff, r.stack_diff_bytes, r.noise, r.overflow, rd);
    dp += wsprintfA(det + dp, " heap=o:%d,n:%d", r.heap_orig, r.heap_port);   // [D4-WSB-SHADOW-PRIVATE]
    if (Track::RetBox<Ret>::kHas) dp += wsprintfA(det + dp, " rv=%08x", ro.raw());
    if (r.pages_diff) dp += wsprintfA(det + dp, " @%08x+%03x", static_cast<unsigned>(r.first_page), r.first_off);
    if (r.pages_diff) {                                      // [D4-WSB-BVH-FU] off:pre/orig/port per dword
        dp += wsprintfA(det + dp, " ndw=%d v=", r.ndw);
        for (int i = 0; i < r.nlog; ++i)
            dp += wsprintfA(det + dp, "%s%03x:%08x/%08x/%08x", i ? "," : "", r.dw[i].off,
                            r.dw[i].pre, r.dw[i].o, r.dw[i].n);
    }
    if (r.stack_diff_bytes) dp += wsprintfA(det + dp, " stk+%03x", r.stack_first_off);
    if (r.alloc_on) {                                        // [HB-SHADOW-ALLOC-COMPARE]
        const Track::AllocState& as = Track::AS();
        dp += wsprintfA(det + dp, " alloc=n:%d,mm:%d,mask:%d,movf:%d,blk:%d,mut:%d", r.alloc_n, r.alloc_mm,
                        r.alloc_mask, r.alloc_mask_ovf, r.blk_dw, r.alloc_mut);
        if (r.blk_dw) dp += wsprintfA(det + dp, " blk@%08x:%08x/%08x", static_cast<unsigned>(r.blk_first), r.blk_o, r.blk_n);
        // the pairs on their own line (Record's line buffer is 512): slot:a0,a1,a2 -> original/port
        char al[480];
        int ap = wsprintfA(al, "[al] fn=%s sample=%d nA=%d nB=%d pin=%d/%d", c.name, c.n, as.log[0].n, as.log[1].n,
                           as.pin_tries, as.pin_miss);
        for (int k = 0; k < as.log[0].n && k < 6; ++k)
            ap += wsprintfA(al + ap, " %x:%x,%x,%x->%08x/%08x", as.log[0].c[k].slot, as.log[0].c[k].a0, as.log[0].c[k].a1,
                            as.log[0].c[k].a2, as.log[0].c[k].ret, k < as.log[1].n ? as.log[1].c[k].ret : 0u);
        Log(al); Log("\r\n");
    }
    Record(c, idx, nd, det);
    return rn.get();
}

} // namespace ShadowAB
