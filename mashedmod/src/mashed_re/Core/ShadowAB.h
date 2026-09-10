// Mashed RE — SHADOW A/B: in-process original-vs-port bit comparison at the REAL call site.
// TT-11 (re/TOOLING_TODO.md). Generalised 2026-09-09 from the hand-written B5c pattern in
// Collision/RwpIntegrator.cpp, which proved the mechanism but cost ~50 lines per function.
//
// ─── WHY THIS LANE EXISTS ───────────────────────────────────────────────────────────
// The Frida synthetic-A/B lane (run_diff.py) dies on four things that dominate the
// remaining C2 pool. This lane sidesteps all four because it runs INSIDE the process at
// the function's natural call site:
//
//   Frida blocker                                   | here
//   ------------------------------------------------|--------------------------------------
//   needs an arg_type in diff_template.js           | none — the compiler passes the args
//   synthetic call corrupts live state              | state is real; the write region is restored
//   Interceptor on >1000 calls/s wedges MASHED ~6 s | zero Frida; this is compiled C++
//   one boot per function                           | every armed function samples in ONE boot
//
// ─── WHAT IT DOES, PER SAMPLED CALL ─────────────────────────────────────────────────
//   1. snapshot the output region (RunRegion only)
//   2. HookSystem::Uninstall our inline-JMP at the RVA
//   3. call the ORIGINAL at its RVA with the SAME arguments
//   4. capture the original's result, restore the region
//   5. HookSystem::Install our JMP back
//   6. run OUR port on the same inputs
//   7. bit-compare (memcmp — never ==, so float NaN/-0.0 compare correctly)
//
// ─── LIMITS (do not overclaim) ──────────────────────────────────────────────────────
//   * .asi ONLY. It verifies the port's LOGIC against the original in MASHED's address
//     space; it says nothing about standalone runtime (memory
//     `feedback_c4_verifies_logic_not_standalone`). Inert on the exe: the env gate is
//     unset and HookSystemNoOp's Uninstall/Install/At/Count return 0/false.
//   * The Uninstall/Install window must be safe. Physics/menu code is single-threaded,
//     which is why the B5c lane gates on phase 3; keep the phase gate honest per function.
//   * Only covers what the scenario actually REACHES. A function that never fires produces
//     no samples, and no samples is NOT evidence (memory `feedback_evidence_discipline`).
//   * The original must be re-entrant with respect to itself for the duration of the call.
//     A function whose side effects are irreversible (file/COM/handle) is NOT a candidate
//     for RunRegion — restoring a memory region does not un-write a file.
//   * Run() executes the function TWICE on the same inputs and compares only the return.
//     A NON-IDEMPOTENT function (CRT rand(), pool allocators, counters) legitimately
//     returns two different values, so a DIVERGENT row from Run() means "port differs OR
//     function is stateful" until a reviewer has read the body. Measured 2026-09-10:
//     FUN_00564310 (octree octant pick) diverged 20/48 purely because it calls rand()
//     (re/analysis/shadow_lane_20260910.md). CLEAN rows are not weakened by this.
//   * PATCHBYTE has THREE shapes, not two. installed=E9/uninstalled!=E9 is the intended
//     proof. installed!=E9 with both bytes equal means the hook was NEVER installed and the
//     wrapper was reached through OUR ported caller — the code at the RVA is the untouched
//     original, so the comparison is still real (re/tools/shadow_ab_report.py labels it
//     "A/B-IS-REAL(hook-not-installed)"). E9 on both sides is the vacuous case.
//
// ─── MASS GENERATION ────────────────────────────────────────────────────────────────
//   re/tools/shadow_gen.py rewrites an installed port into `Name_impl` + this wrapper
//   mechanically (return-value ports; void ports need --region from the analysis note).
//   re/tools/shadow_batch.py boots groups of sites and bisects crashers;
//   re/tools/shadow_ab_report.py turns shadow_ab.log into per-function verdicts.
//
// ─── USAGE ──────────────────────────────────────────────────────────────────────────
//   // return-value leaf
//   static int MyFn_impl(int key) { ... }                       // the port
//   extern "C" int __cdecl MyFn(int key) {
//       SHADOW_AB_COUNTER(ab, "MyFn", 0x0057c210, ShadowAB::kPhaseRace);
//       return ShadowAB::Run(ab, MyFn_impl, key);
//   }
//   RH_ScopedInstall(MyFn, 0x0057c210);
//
//   // void function that writes a known region
//   extern "C" void __cdecl MyWriter(Body* b) {
//       SHADOW_AB_COUNTER(ab, "MyWriter", 0x0055b800, ShadowAB::kPhaseRace);
//       ShadowAB::RunRegion(ab, MyWriter_impl, &b->matrix, sizeof(b->matrix), b);
//   }
//
// Arm with MASHED_SHADOW_AB=1; results land in shadow_ab.log (process CWD).
#pragma once

#include "HookSystem.h"

#include <windows.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace ShadowAB {

// Session-phase global: 1 = menu, 2 = load+spawn, 3 = race (re/frida/scenario_launch.py:41).
constexpr std::uintptr_t kPhaseGlobal = 0x00771968u;
constexpr int kPhaseAny  = 0;   // no gate
constexpr int kPhaseMenu = 1;
constexpr int kPhaseRace = 3;

// Armed by MASHED_SHADOW_AB, and ALSO by the legacy MASHED_PHYS_C4_SELFTEST so that
// converting a B5c site to this header does not silently drop it out of the existing
// physics self-test runs (Collision/RwpIntegrator.cpp still gates its unconverted sites
// on that variable). Either name arms the lane.
inline bool EnvOn(const char* name) {
    char buf[8];
    DWORD n = GetEnvironmentVariableA(name, buf, sizeof buf);
    return n > 0 && n < sizeof buf && buf[0] && buf[0] != '0';
}

inline bool Enabled() {
    static int v = -1;
    if (v < 0) v = (EnvOn("MASHED_SHADOW_AB") || EnvOn("MASHED_PHYS_C4_SELFTEST")) ? 1 : 0;
    return v != 0;
}

// Reading the phase byte is only safe once MASHED's data section is live; on the exe the
// address is unmapped, but Enabled() short-circuits first so this never runs there.
inline int Phase() { return *reinterpret_cast<volatile unsigned char*>(kPhaseGlobal); }

inline const char* LogPath() { return "shadow_ab.log"; }

inline void Log(const char* s) {
    HANDLE h = CreateFileA(LogPath(), FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote;
    WriteFile(h, s, static_cast<DWORD>(lstrlenA(s)), &wrote, nullptr);
    CloseHandle(h);
}

// ONE global re-entrancy flag, shared by every armed function: while we are running the
// A/B for function X, a nested call to armed function Y must take the plain path, or the
// Uninstall of X races the Uninstall of Y.
inline bool& InTest() { static bool f = false; return f; }

struct Reentry {
    Reentry()  { InTest() = true;  }
    ~Reentry() { InTest() = false; }
};

// Per-function state. Declare it as a function-static via SHADOW_AB_COUNTER so each
// hot function keeps its own budget and a chatty one cannot starve the others.
constexpr int kDefaultSamples = 48;

struct Counter {
    const char*    name;
    std::uintptr_t rva;
    int            min_phase;
    int            max_samples;
    int            n      = 0;
    int            ndiff  = 0;
    bool           noted  = false;   // logged the "no hook index" case once
    bool           proved = false;   // logged the patch-byte proof once
};

#define SHADOW_AB_COUNTER(var, name_, rva_, min_phase_)                       \
    static ::ShadowAB::Counter var { (name_), (rva_), (min_phase_),           \
                                     ::ShadowAB::kDefaultSamples }

inline int HookIndex(std::uintptr_t rva) {
    for (std::size_t i = 0; i < HookSystem::Count(); ++i)
        if (HookSystem::At(i).target_rva == rva) return static_cast<int>(i);
    return -1;
}

// Sampling gate: cheap checks first so the un-armed path costs one bool test.
inline bool Armed(Counter& c) {
    if (!Enabled() || InTest() || c.n >= c.max_samples) return false;
    if (c.min_phase > kPhaseAny && Phase() < c.min_phase) return false;
    return true;
}

inline void Record(Counter& c, int idx, int ndiff, const char* detail) {
    c.ndiff += (ndiff != 0);
    char line[256];
    wsprintfA(line, "[%d] fn=%s rva=%08x idx=%d ndiff=%d%s%s\r\n",
              c.n++, c.name, static_cast<unsigned>(c.rva), idx, ndiff,
              ndiff ? "" : " OK", detail ? detail : "");
    Log(line);
}

// ── OriginalWindow: the uninstall/reinstall window on its own ───────────────────────
// Run()/RunRegion() impose a shape: one return value, or one contiguous output region.
// Several real sites fit neither -- RwpBodyMatrixRefresh writes TWO discontiguous regions
// (0x40 matrix + 0x10 quaternion) AND returns a pointer. Forcing those into a generic
// signature would be worse code than the bespoke compare they already have.
//
// What they were missing is only the PROOF that the A/B is real. This RAII window gives
// them exactly that and nothing else: uninstall on entry, reinstall on exit, patch-byte
// proof logged once per function. The site keeps its own snapshot/compare/log.
//
//   {
//       ShadowAB::OriginalWindow win(ab);            // our JMP is OFF inside this scope
//       if (win.ok()) reinterpret_cast<Fn>(RVA)(args...);
//   }                                                // reinstalled here, even on early exit
//
// win.idx is -1 when the RVA is not in the registry; callers must check ok() before calling
// the "original", because without the uninstall that call re-enters our own hook.
struct OriginalWindow {
    Counter& c;
    int      idx;

    explicit OriginalWindow(Counter& ctr) : c(ctr), idx(HookIndex(ctr.rva)) {
        if (idx < 0) {
            if (!c.noted) { c.noted = true; Record(c, idx, 0, " SKIP:no-hook-index"); }
            return;
        }
        const unsigned char before = *reinterpret_cast<volatile unsigned char*>(c.rva);
        HookSystem::Uninstall(static_cast<std::size_t>(idx));
        const unsigned char after = *reinterpret_cast<volatile unsigned char*>(c.rva);
        if (!c.proved) {
            c.proved = true;
            char pb[112];
            wsprintfA(pb, "[--] fn=%s PATCHBYTE installed=%02x uninstalled=%02x %s",
                      c.name, before, after,
                      (before == 0xE9 && after != 0xE9) ? "A/B-IS-REAL" : "SUSPECT-no-restore");
            Log(pb);
            Log("\r\n");
        }
    }
    ~OriginalWindow() {
        if (idx >= 0) HookSystem::Install(static_cast<std::size_t>(idx));
    }
    bool ok() const { return idx >= 0; }

    OriginalWindow(const OriginalWindow&) = delete;
    OriginalWindow& operator=(const OriginalWindow&) = delete;
};

// ── return-value comparison ─────────────────────────────────────────────────────────
// Ret must be trivially copyable and compared BITWISE: `==` on a float would call NaN
// unequal to itself and +0.0 equal to -0.0, both of which hide real divergence.
template <typename F, typename... A>
auto Run(Counter& c, F impl, A... args) -> decltype(impl(args...)) {
    using Ret = decltype(impl(args...));
    if (!Armed(c)) return impl(args...);

    Reentry guard;
    const int idx = HookIndex(c.rva);
    if (idx < 0) {
        // Not registered (or registry stubbed): running the "original" would re-enter our
        // own JMP and recurse. Take the plain path and say so once.
        if (!c.noted) { c.noted = true; Record(c, idx, 0, " SKIP:no-hook-index"); }
        return impl(args...);
    }

    // PROOF THE A/B IS REAL, logged once per function. If Uninstall silently failed, the
    // call below would re-enter OUR OWN inline-JMP and we would be comparing the port to
    // itself -- trivially equal, i.e. a vacuous "all OK". Sampling the patch byte at the
    // RVA inside the window settles it: installed it is 0xE9 (our JMP); uninstalled it must
    // be the original prologue byte. Cheap, harmless and decisive. A control that instead
    // corrupts the return value is NOT usable here: tried 2026-09-09, +1 on this body-pointer
    // lookup crashes the solver before the first sample is ever written.
    const unsigned char byte_installed = *reinterpret_cast<volatile unsigned char*>(c.rva);

    Ret o{};
    HookSystem::Uninstall(static_cast<std::size_t>(idx));
    const unsigned char byte_uninstalled = *reinterpret_cast<volatile unsigned char*>(c.rva);
    o = reinterpret_cast<F>(c.rva)(args...);
    HookSystem::Install(static_cast<std::size_t>(idx));

    if (!c.proved) {
        c.proved = true;
        char pb[112];
        wsprintfA(pb, "[--] fn=%s PATCHBYTE installed=%02x uninstalled=%02x %s",
                  c.name, byte_installed, byte_uninstalled,
                  (byte_installed == 0xE9 && byte_uninstalled != 0xE9)
                      ? "A/B-IS-REAL" : "SUSPECT-no-restore");
        Log(pb);
        Log("\r\n");
    }

    Ret n = impl(args...);

    const int nd = std::memcmp(&o, &n, sizeof(Ret)) != 0 ? 1 : 0;
    char det[96] = {};
    if (nd) {
        std::uint32_t ob = 0, nb = 0;
        std::memcpy(&ob, &o, sizeof(Ret) < 4 ? sizeof(Ret) : 4);
        std::memcpy(&nb, &n, sizeof(Ret) < 4 ? sizeof(Ret) : 4);
        wsprintfA(det, " o=%08x,n=%08x", ob, nb);
    }
    Record(c, idx, nd, det);
    return n;
}

// ── output-region comparison (void functions that write through a pointer) ──────────
// Order matters: snapshot -> original -> capture its output -> RESTORE -> ours -> compare.
// Without the restore, our port would run against state the original already mutated.
template <typename F, typename... A>
void RunRegion(Counter& c, F impl, void* region, std::size_t bytes, A... args) {
    if (!Armed(c) || region == nullptr || bytes == 0 || bytes > 1024) {
        impl(args...);
        return;
    }
    Reentry guard;
    const int idx = HookIndex(c.rva);
    if (idx < 0) {
        if (!c.noted) { c.noted = true; Record(c, idx, 0, " SKIP:no-hook-index"); }
        impl(args...);
        return;
    }

    unsigned char before[1024], orig_out[1024];
    std::memcpy(before, region, bytes);

    {   // same window (and therefore the same PATCHBYTE proof) as Run(); RunRegion used to
        // inline its own uninstall/install and silently produced results with no proof.
        OriginalWindow win(c);
        if (win.ok()) reinterpret_cast<F>(c.rva)(args...);
    }

    std::memcpy(orig_out, region, bytes);
    std::memcpy(region, before, bytes);          // restore before running ours

    impl(args...);

    // count DIFFERING 32-bit fields, so the log localises the divergence
    int nd = 0;
    const std::size_t words = bytes / 4;
    for (std::size_t i = 0; i < words; ++i)
        if (std::memcmp(orig_out + i * 4, static_cast<unsigned char*>(region) + i * 4, 4) != 0)
            ++nd;
    if (words * 4 != bytes && std::memcmp(orig_out + words * 4,
                                          static_cast<unsigned char*>(region) + words * 4,
                                          bytes - words * 4) != 0)
        ++nd;

    // Name the offsets. "5 of 32 differ" cannot be triaged; "+0x00,+0x04,..." can, and it is
    // what separates a real port defect from a region whose bounds were guessed wrong.
    char det[160];
    int dp = wsprintfA(det, " fields=%d/%d", nd, static_cast<int>(words));
    if (nd) {
        int shown = 0;
        for (std::size_t i = 0; i < words && shown < 8; ++i) {
            if (std::memcmp(orig_out + i * 4,
                            static_cast<unsigned char*>(region) + i * 4, 4) != 0) {
                dp += wsprintfA(det + dp, "%s+%02x", shown ? "," : " @", (int)(i * 4));
                ++shown;
            }
        }
    }
    Record(c, idx, nd, det);
}

} // namespace ShadowAB
