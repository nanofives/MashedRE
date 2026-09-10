// Mashed RE — SHADOW A/B, LANE 3: page-level write tracking (no region spec, no idempotency
// assumption). Companion to ShadowAB.h; see re/analysis/promotion_lanes_assessment_20260910.md
// (Lane 3) and re/analysis/lane3_write_tracking_20260910.md.
//
// ─── THE PROBLEM IT REMOVES ─────────────────────────────────────────────────────────
//   Run()       compares a RETURN only: void ports are invisible, and a non-idempotent
//               function legitimately returns two different values on its two executions
//               (7 of the first sweep's 10 DIVERGENT rows were exactly that).
//   RunRegion() compares ONE contiguous span the author has to know and prove; 348 void
//               ports across C2/C3 have no verified span.
// This primitive compares EFFECTS: every page the call wrote, plus the caller's stack
// window, with the pre-call state restored between the two executions.
//
// ─── MECHANISM, PER SAMPLED CALL ────────────────────────────────────────────────────
//   1. enumerate every committed, writable, NON-executable page that is either private
//      (heaps, .bss) or part of MASHED.exe's image (.data), EXCLUDING: this .asi's image,
//      the tracking buffers, the current thread's stack and TEB, and PAGE_GUARD pages.
//   2. VirtualProtect all of them PAGE_READONLY; install a vectored exception handler.
//   3. call the ORIGINAL (hook uninstalled). Each first write to a page faults: the handler
//      copies the page's PRE-image into a pool, restores the page's protection, resumes.
//      Faults from OTHER threads are counted as noise and not compared.
//   4. restore protections; copy the POST-images of the touched pages; write the PRE-images
//      back  -> memory is exactly as before the call (that is what defeats idempotency).
//   5. repeat 2-3 for OUR port.
//   6. compare: for pages the original touched, post_orig vs live; for pages only the port
//      touched, pre vs live (a write of the same value is no difference). The caller's
//      stack window (4 KB above this wrapper's return address) is snapshotted/restored the
//      same way so out-parameters that live in the caller's frame are covered.
//   The port's effects are what stays in memory afterwards, exactly like Run().
//
// ─── LIMITS (do not overclaim) ──────────────────────────────────────────────────────
//   * .asi only; verifies logic against the original in MASHED's address space.
//   * Effects OUTSIDE tracked memory are invisible: files, handles, GPU, other modules'
//     .data (system DLL globals are deliberately untracked to keep noise down), and
//     writes below the caller's frame window. Irreversible side effects (file/COM) are
//     executed twice; the page restore does not undo them.
//   * Pages another thread writes during the window are "noise" and excluded, which can
//     hide a real difference on that page. The sample says how many.
//   * Allocation-heavy functions produce different heap pages on the two runs by
//     construction (different block addresses): expect DIVERGENT there, read the offsets.
//   * Heavier than Run(): two full protect/unprotect passes per sample (ms each). The
//     default budget is therefore lower (kTrackedSamples).
//
// ─── USAGE ──────────────────────────────────────────────────────────────────────────
//   extern "C" void __cdecl MyVoidFn(int a, float* out) {
//       SHADOW_AB_COUNTER(ab, "MyVoidFn", 0x0055b750, ShadowAB::kPhaseRace);
//       ShadowAB::RunTracked(ab, MyVoidFn_impl, SHADOW_STACK_WINDOW(), a, out);
//   }
// Arm with MASHED_SHADOW_AB=1 as usual. Log lines carry `pages=` and `stack=` details.
#pragma once

#include "ShadowAB.h"

#include <intrin.h>
#include <tlhelp32.h>
#include <type_traits>

// Address of the first stack argument the GAME pushed for this wrapper: the start of the
// caller-owned stack window. Must be expanded in the hook wrapper itself, not in a callee.
#define SHADOW_STACK_WINDOW() \
    (reinterpret_cast<void*>(static_cast<char*>(_AddressOfReturnAddress()) + 4))

namespace ShadowAB {
namespace Track {

constexpr std::size_t kPage        = 4096;
constexpr int         kMaxPages    = 2048;     // 8 MB pre-image pool + 8 MB post-image pool
constexpr int         kMaxRegions  = 8192;
constexpr int         kMaxNoise    = 256;
constexpr std::size_t kStackWindow = 4096;
constexpr int         kTrackedSamples = 24;

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
        || er->ExceptionInformation[0] != 1)                 // 1 = write
        return EXCEPTION_CONTINUE_SEARCH;
    const std::uintptr_t addr = static_cast<std::uintptr_t>(er->ExceptionInformation[1]);
    const Region* r = FindRegion(addr);
    if (!r) return EXCEPTION_CONTINUE_SEARCH;
    const std::uintptr_t page = addr & ~(kPage - 1);
    DWORD old = 0;
    // give the page its write permission back FIRST; if that fails, let the fault surface
    // rather than spin on it forever
    if (!VirtualProtect(reinterpret_cast<void*>(page), kPage, r->prot, &old))
        return EXCEPTION_CONTINUE_SEARCH;
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
    MEMORY_BASIC_INFORMATION mbi;
    std::uintptr_t a = 0x10000;
    while (a < 0x7fff0000u && VirtualQuery(reinterpret_cast<void*>(a), &mbi, sizeof mbi) == sizeof mbi) {
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t end  = base + mbi.RegionSize;
        const DWORD prot = mbi.Protect & 0xff;
        const bool writable = (prot == PAGE_READWRITE || prot == PAGE_WRITECOPY)
                              && !(mbi.Protect & PAGE_GUARD);
        const bool wanted = mbi.State == MEM_COMMIT && writable
            && (mbi.Type == MEM_PRIVATE
                || (mbi.Type == MEM_IMAGE
                    && reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) == s.exe_base));
        if (wanted
            && !Overlaps(base, end, s.buf_lo, s.buf_hi)
            && !Overlaps(base, end, s.asi_lo, s.asi_hi)
            && !Overlaps(base, end, stack_lo - 0x10000, stack_hi)    // stack + its guard/reserve
            && !Overlaps(base, end, teb, teb + kPage)
            && s.nregions < kMaxRegions) {
            DWORD old = 0;
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
        VirtualProtect(reinterpret_cast<void*>(s.regions[i].base), s.regions[i].size, s.regions[i].prot, &old);
    }
    s.nregions = 0;
}

// STOP THE WORLD. The first live run (2026-09-10) crashed twice at the same audio-code frame
// with a clobbered object pointer: another thread had written to a page OUR thread also
// touched, and the pre-image restore between the two executions reverted that thread's
// write. That contamination is undetectable once a page has been re-enabled (later writes
// do not fault), so the only sound fix is to suspend every other thread for the window.
// Known risk, documented not hidden: a suspended thread holding a lock the ORIGINAL or the
// PORT needs (heap, D3D runtime) deadlocks the game. Physics/gameplay leaves do not take
// those locks; anything that allocates or talks to D3D is a bad candidate for this lane.
constexpr int kMaxThreads = 128;
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
// Suspension of the other threads is done ONCE around the whole Compare() (both executions
// AND the restore between them), not per Execute(): the restore is where contamination bites.
template <typename Fn>
inline void Execute(Fn call) {
    State& s = S();
    s.tid = GetCurrentThreadId();
    s.veh = AddVectoredExceptionHandler(1, &Handler);
    Protect();
    s.active = true;
    call();
    s.active = false;
    Unprotect();
    if (s.veh) { RemoveVectoredExceptionHandler(s.veh); s.veh = nullptr; }
}

struct Result {
    int pages_orig = 0, pages_port = 0, pages_diff = 0, noise = 0, overflow = 0;
    int stack_diff_bytes = 0;
    std::uintptr_t first_page = 0; int first_off = -1;
    int stack_first_off = -1;
};

// The full A/B: original under tracking, restore, port under tracking, compare.
template <typename O, typename P>
inline Result Compare(O call_orig, P call_port, void* stackwin) {
    State& s = S();
    Result r;
    static Others others;                 // .asi .data: untracked
    struct WorldGuard {                   // resume on every exit path
        Others& o; explicit WorldGuard(Others& oo) : o(oo) { SuspendOthers(o); }
        ~WorldGuard() { ResumeOthers(o); }
    } world(others);
    const std::uintptr_t stack_hi = __readfsdword(0x04);
    std::size_t win = kStackWindow;
    if (reinterpret_cast<std::uintptr_t>(stackwin) + win > stack_hi)
        win = stack_hi - reinterpret_cast<std::uintptr_t>(stackwin);
    static unsigned char stack_pre[kStackWindow], stack_post[kStackWindow];   // .asi .data: untracked
    std::memcpy(stack_pre, stackwin, win);

    // ── A: original ─────────────────────────────────────────────────────────────
    Execute(call_orig);
    r.pages_orig = s.npages; r.noise = s.nnoise; r.overflow = s.overflow;
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

    // ── B: port ────────────────────────────────────────────────────────────────
    Execute(call_port);
    r.pages_port = s.npages; r.noise += s.nnoise; r.overflow += s.overflow;

    // ── compare ────────────────────────────────────────────────────────────────
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

// ── the primitive ───────────────────────────────────────────────────────────────────
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
    unsigned char byte_uninstalled = byte_installed;
    HookSystem::Uninstall(static_cast<std::size_t>(idx));
    byte_uninstalled = *reinterpret_cast<volatile unsigned char*>(c.rva);
    Track::Result r = Track::Compare(
        [&] { ro.call(reinterpret_cast<F>(c.rva), args...); },
        [&] { rn.call(impl, args...); },
        stackwin);
    // Re-install AFTER the tracked window: Install() writes the registry entry, which lives in
    // this module's heap (a tracked private page) and would show up as a port-only page diff.
    // Consequence: during the port's tracked run the hook is NOT installed, so a recursive
    // call from inside the port reaches the ORIGINAL (Run() differs here). Documented limit.
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
    char det[200];
    int dp = wsprintfA(det, " pages=o:%d,n:%d,diff:%d stack=%d noise=%d ovf=%d ret=%d",
                       r.pages_orig, r.pages_port, r.pages_diff, r.stack_diff_bytes, r.noise, r.overflow, rd);
    if (r.pages_diff) dp += wsprintfA(det + dp, " @%08x+%03x", static_cast<unsigned>(r.first_page), r.first_off);
    if (r.stack_diff_bytes) dp += wsprintfA(det + dp, " stk+%03x", r.stack_first_off);
    Record(c, idx, nd, det);
    return rn.get();
}

} // namespace ShadowAB
