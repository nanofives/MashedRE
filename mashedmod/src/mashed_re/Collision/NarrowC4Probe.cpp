// Mashed RE — D4-WSB-C4-NARROW: full-output in-process A/B for the NARROW-band C3 rows
// (RenderWare-Physics-3.7, 0x00570000-0x0057bfff). .asi ONLY (asi_sources.rsp); never in
// mashed_re.exe, so the default build is untouched by construction.
//
// WHY NOT ShadowAB::Run: Run() compares the return value only, and it runs the original and
// then the port on the SAME live state with no rollback. Every target here writes through
// pointer arguments (out-params, contact appends, arena counters), so Run() both misses the
// outputs and double-applies the side effects. This probe snapshots the function's write set
// (one region list per target, from the port body and its callees), and per sampled call:
//
//   1. snapshot S of the write set
//   2. uninstall OUR inline JMP, call the ORIGINAL       -> return r1, region bytes O1
//   3. restore S, call the ORIGINAL again (no-op control) -> r2, O2
//   4. restore S, reinstall the JMP, call OUR PORT        -> rp, P   (the game keeps these)
//   5. noo  = (r1,O1) != (r2,O2)   -> the sample is not comparable (write set incomplete or
//                                     the original is not idempotent), counted, never OK
//      diff = (r1,O1) != (rp,P)    -> the port differs
//
// The game consumes the PORT's result and writes, so every sample is a real execution of the
// port body behind its installed inline JMP on canonical-scenario state.
//
// Hooks are registered with the "L2_" prefix: HookSystem installs those only when
// MASHED_HOOK_ONLY names them (Core/HookSystem.cpp:248), so a default .asi run never installs
// a probe. Arm with MASHED_HOOK_ONLY=L2_C4N_<rva> and MASHED_C4N=1. MASHED_C4N_STRIDE=<k>
// samples every k-th race-phase call (default 1). Log: c4n_ab.log in the process CWD.
//
// The port leg calls the extern FUN_<rva> wrapper from RwpSolverCore*.cpp while ShadowAB's
// re-entry flag is held, so that wrapper takes its plain path (its own Run() would otherwise
// Uninstall/Install the FUN_<rva> registry entry, which sits on the same RVA as this probe).
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"

#include <windows.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef unsigned int uint;
typedef unsigned int undefined4;

namespace {

constexpr int           kMaxRegions = 16;
constexpr std::uint32_t kMaxBytes   = 0x80000;

bool EnvOn(const char* name) {
    char buf[8];
    DWORD n = GetEnvironmentVariableA(name, buf, sizeof buf);
    return n > 0 && n < sizeof buf && buf[0] && buf[0] != '0';
}

bool Enabled() {
    static int v = -1;
    if (v < 0) v = EnvOn("MASHED_C4N") ? 1 : 0;
    return v != 0;
}

long Stride() {
    static long v = -1;
    if (v < 0) {
        char buf[16];
        DWORD n = GetEnvironmentVariableA("MASHED_C4N_STRIDE", buf, sizeof buf);
        v = (n > 0 && n < sizeof buf) ? atol(buf) : 1;
        if (v < 1) v = 1;
    }
    return v;
}

void Log(const char* s) {
    HANDLE h = CreateFileA("c4n_ab.log", FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote;
    WriteFile(h, s, static_cast<DWORD>(lstrlenA(s)), &wrote, nullptr);
    CloseHandle(h);
}

// Committed, writable, and not a guard page, for every byte of [p, p+n).
bool Writable(const void* p, std::uint32_t n) {
    auto a = reinterpret_cast<std::uintptr_t>(p);
    if (a < 0x10000 || n == 0) return false;
    const std::uintptr_t end = a + n;
    while (a < end) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<void*>(a), &mbi, sizeof mbi)) return false;
        if (mbi.State != MEM_COMMIT) return false;
        const DWORD pr = mbi.Protect & 0xff;
        if (mbi.Protect & PAGE_GUARD) return false;
        if (pr != PAGE_READWRITE && pr != PAGE_EXECUTE_READWRITE && pr != PAGE_WRITECOPY &&
            pr != PAGE_EXECUTE_WRITECOPY) return false;
        a = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    }
    return true;
}

}  // namespace

struct C4nRegions {
    int           n     = 0;
    std::uint32_t total = 0;
    bool          bad   = false;   // a region was unreadable or the budget overflowed
    unsigned char* p[kMaxRegions];
    std::uint32_t len[kMaxRegions];

    // A null base is "not written on this call" (callers pass optional out-params as 0).
    void add(const void* base, std::uint32_t bytes) {
        if (base == nullptr || bytes == 0) return;
        if (n >= kMaxRegions || total + bytes > kMaxBytes || !Writable(base, bytes)) {
            bad = true;
            return;
        }
        p[n] = static_cast<unsigned char*>(const_cast<void*>(base));
        len[n] = bytes;
        total += bytes;
        ++n;
    }
    void save(unsigned char* dst) const {
        for (int i = 0; i < n; ++i) { std::memcpy(dst, p[i], len[i]); dst += len[i]; }
    }
    void load(const unsigned char* src) const {
        for (int i = 0; i < n; ++i) { std::memcpy(p[i], src, len[i]); src += len[i]; }
    }
};

struct C4nProbe {
    const char*    name;     // registry name, "L2_C4N_<rva>"
    std::uintptr_t rva;
    int            idx       = -2;
    long           calls     = 0;   // race-phase calls seen
    long           samples   = 0;
    long           ok        = 0;
    long           diff      = 0;
    long           noo       = 0;
    long           skip      = 0;   // region list unusable on this call (port ran alone)
    long           wrote     = 0;   // samples where the original changed >=1 region byte
    long           ret_diff  = 0;
    long           reg_diff  = 0;
    long           first_reg[kMaxRegions] = {};   // diff samples by region of the first differing byte
    int            diff_logged = 0;
    int            noo_logged  = 0;
    bool           proved    = false;
};

static int C4nFindIdx(const char* name) {
    for (std::size_t i = 0; i < HookSystem::Count(); ++i) {
        const HookSystem::HookEntry& e = HookSystem::At(i);
        if (e.name && std::strcmp(e.name, name) == 0) return static_cast<int>(i);
    }
    return -1;
}

static void C4nSummary(const C4nProbe& pr) {
    char line[256];
    wsprintfA(line, "SUM fn=%s rva=%08x calls=%ld samples=%ld ok=%ld diff=%ld noo=%ld skip=%ld "
                    "wrote=%ld ret_diff=%ld reg_diff=%ld\r\n",
              pr.name, static_cast<unsigned>(pr.rva), pr.calls, pr.samples, pr.ok, pr.diff,
              pr.noo, pr.skip, pr.wrote, pr.ret_diff, pr.reg_diff);
    Log(line);
    char hist[256];
    int hp = wsprintfA(hist, "REG fn=%s first-diff by region:", pr.name);
    for (int k = 0; k < kMaxRegions; ++k)
        if (pr.first_reg[k]) hp += wsprintfA(hist + hp, " r%d=%ld", k, pr.first_reg[k]);
    wsprintfA(hist + hp, "\r\n");
    Log(hist);
}

// First differing byte offset inside the concatenated region image, -1 if equal.
static long C4nFirstDiff(const unsigned char* a, const unsigned char* b, std::uint32_t n) {
    for (std::uint32_t i = 0; i < n; ++i) if (a[i] != b[i]) return static_cast<long>(i);
    return -1;
}

template <typename R, typename... A>
R C4nRun(C4nProbe& pr, R (__cdecl* port)(A...), void (*regs)(C4nRegions&, A...), A... args) {
    if (!Enabled() || ShadowAB::InTest() || ShadowAB::Phase() < ShadowAB::kPhaseRace)
        return port(args...);
    ++pr.calls;
    if ((pr.calls % Stride()) != 0) return port(args...);
    if (pr.idx == -2) pr.idx = C4nFindIdx(pr.name);
    if (pr.idx < 0) return port(args...);

    ShadowAB::Reentry guard;
    C4nRegions rg;
    regs(rg, args...);
    if (rg.bad) { ++pr.skip; return port(args...); }

    static unsigned char s_before[kMaxBytes], s_o1[kMaxBytes], s_o2[kMaxBytes], s_p[kMaxBytes];
    rg.save(s_before);

    using Fn = R (__cdecl*)(A...);
    const Fn orig = reinterpret_cast<Fn>(pr.rva);
    const unsigned char b_in = *reinterpret_cast<volatile unsigned char*>(pr.rva);
    HookSystem::Uninstall(static_cast<std::size_t>(pr.idx));
    const unsigned char b_out = *reinterpret_cast<volatile unsigned char*>(pr.rva);

    R r1 = orig(args...);
    rg.save(s_o1);
    rg.load(s_before);
    R r2 = orig(args...);
    rg.save(s_o2);
    rg.load(s_before);

    HookSystem::Install(static_cast<std::size_t>(pr.idx));
    if (!pr.proved) {
        pr.proved = true;
        char pb[160];
        wsprintfA(pb, "[--] fn=%s PATCHBYTE installed=%02x uninstalled=%02x reinstalled=%02x %s\r\n",
                  pr.name, b_in, b_out, *reinterpret_cast<volatile unsigned char*>(pr.rva),
                  (b_in == 0xE9 && b_out != 0xE9) ? "A/B-IS-REAL" : "SUSPECT");
        Log(pb);
    }

    R rp = port(args...);
    rg.save(s_p);

    ++pr.samples;
    if (std::memcmp(s_before, s_o1, rg.total) != 0) ++pr.wrote;
    const bool noo = std::memcmp(&r1, &r2, sizeof(R)) != 0 ||
                     std::memcmp(s_o1, s_o2, rg.total) != 0;
    const bool rd = std::memcmp(&r1, &rp, sizeof(R)) != 0;
    const long fd = C4nFirstDiff(s_o1, s_p, rg.total);
    if (noo) {
        ++pr.noo;
        if (pr.noo_logged < 10) {
            ++pr.noo_logged;
            char line[200];
            wsprintfA(line, "NOO fn=%s call=%ld first=%ld bytes=%lu\r\n", pr.name, pr.calls,
                      C4nFirstDiff(s_o1, s_o2, rg.total), static_cast<unsigned long>(rg.total));
            Log(line);
        }
    } else if (rd || fd >= 0) {
        ++pr.diff;
        pr.ret_diff += rd;
        pr.reg_diff += (fd >= 0);
        if (fd >= 0) {
            long off = fd;
            for (int k = 0; k < rg.n; ++k) {
                if (off < static_cast<long>(rg.len[k])) { ++pr.first_reg[k]; break; }
                off -= static_cast<long>(rg.len[k]);
            }
        }
        if (pr.diff_logged < 20) {
            ++pr.diff_logged;
            std::uint32_t o = 0, n = 0;
            std::memcpy(&o, &r1, sizeof(R) < 4 ? sizeof(R) : 4);
            std::memcpy(&n, &rp, sizeof(R) < 4 ? sizeof(R) : 4);
            char line[256];
            unsigned ob = 0, nb = 0;
            if (fd >= 0) {
                const long w = fd & ~3L;
                std::memcpy(&ob, s_o1 + w, 4);
                std::memcpy(&nb, s_p + w, 4);
            }
            int reg = -1;
            long roff = fd;
            for (int k = 0; fd >= 0 && k < rg.n; ++k) {
                if (roff < static_cast<long>(rg.len[k])) { reg = k; break; }
                roff -= static_cast<long>(rg.len[k]);
            }
            wsprintfA(line, "DIFF fn=%s call=%ld ret o=%08x n=%08x first=%ld region=%d+%lx o=%08x n=%08x bytes=%lu\r\n",
                      pr.name, pr.calls, o, n, fd, reg, reg >= 0 ? roff : 0L, ob, nb,
                      static_cast<unsigned long>(rg.total));
            Log(line);
        }
    } else {
        ++pr.ok;
    }
    if ((pr.samples & 0x3f) == 1) C4nSummary(pr);
    return rp;
}

// ─── targets ────────────────────────────────────────────────────────────────────────
// Each block: the extern port wrapper (RwpSolverCore*.cpp), the write-set region list, the
// probe and its L2_ registration. Region lists cite the write that each region covers
// (write sets read from the port bodies and their ported callees; writes made by the
// original shape-method slots +0x10/+0x14/+0x18/+0x1c/+0x20/+0x44/+0x48 are not readable
// from the port, and the no-op control is what catches a region list that misses them).

extern "C" {
bool       __cdecl FUN_005757d0(int* param_1, int param_2);
undefined4 __cdecl FUN_00575880(int param_1, int param_2, int* param_3, float param_4);
int        __cdecl FUN_00575c60(undefined4* param_1, uint* param_2, float* param_3, float* param_4,
                                float* param_5, undefined4 param_6);
uint       __cdecl FUN_00575fe0(int param_1, uint* param_2, undefined4 param_3, int param_4,
                                uint* param_5, int param_6, uint* param_7, float* param_8,
                                float* param_9, uint param_10);
uint       __cdecl FUN_00576640(int* param_1, uint param_2, uint param_3, int param_4, int param_5);
undefined4 __cdecl FUN_00577ec0(float* param_1, float* param_2, float param_3, float param_4,
                                float* param_5, float* param_6, float param_7, float param_8,
                                float* param_9, float* param_10, float* param_11);
undefined4 __cdecl FUN_005784a0(uint* param_1, float* param_2, float* param_3, float* param_4,
                                float param_5);
undefined4 __cdecl FUN_00578610(undefined4 param_1, int* param_2, float param_3, float* param_4,
                                float* param_5, float* param_6);
undefined4 __cdecl FUN_00578e50(undefined4 param_1, undefined4 param_2, float param_3, int param_4,
                                undefined4 param_5);
undefined4 __cdecl FUN_00579b50(float* param_1, int param_2, float param_3);
}

namespace {

inline std::uint32_t U32(std::uintptr_t a) { return *reinterpret_cast<const std::uint32_t*>(a); }

inline void* At(std::uintptr_t a) { return reinterpret_cast<void*>(a); }

// Contact record: +0x00..+0x10f header / support / TOI, +0x110 depth, 0x120 stride
// (0x00575880 indexes *(W+0x2c) + i*0x120). 0x005757d0 sets +0xd0 = the +0x50 base, plus
// count*0x10 when the u16 at +0x54 is above 4 (RwpSolverCore18.cpp:123-125), so both support
// arrays lie inside 0x200 bytes from the +0x50 base.
void AddContactRecord(C4nRegions& rg, std::uintptr_t rec) {
    rg.add(At(rec), 0x120);
    if (rg.bad) return;
    rg.add(At(U32(rec + 0x50)), 0x200);
}

// World arenas written by 0x00575c60 / 0x00575fe0 (RwpSolverCore18.cpp:475, Core17.cpp:460):
// W header [0,0x80) (counts +0x10/+0x1c/+0x5c, overflow +0x78, face list +0x44/+0x48),
// PA = *(W+0xc) stride 0x14 cap +0x14, MA = *(W+0x18) stride 0x40 cap +0x20,
// BA = *(W+0x24) stride 0x20 indexed like PA, faces = *(W+0x58) stride 0x60 cap +0x60.
// Each arena is covered from the first index the call can write up to its capacity.
void AddWorldArenas(C4nRegions& rg, std::uintptr_t W, std::uint32_t pStart, std::uint32_t mStart) {
    const std::uint32_t pCap = U32(W + 0x14), mCap = U32(W + 0x20);
    const std::uint32_t fCnt = U32(W + 0x5c), fCap = U32(W + 0x60);
    if (pStart > pCap || mStart > mCap || fCnt > fCap) { rg.bad = true; return; }
    rg.add(At(W), 0x80);
    if (pCap > pStart) {
        rg.add(At(U32(W + 0x0c) + pStart * 0x14), (pCap - pStart) * 0x14);
        rg.add(At(U32(W + 0x24) + pStart * 0x20), (pCap - pStart) * 0x20);
    }
    if (mCap > mStart) rg.add(At(U32(W + 0x18) + mStart * 0x40), (mCap - mStart) * 0x40);
    if (fCap > fCnt)   rg.add(At(U32(W + 0x58) + fCnt * 0x60), (fCap - fCnt) * 0x60);
}

// Index of an arena element pointer, or `fallback` (the current count) when the pointer is not
// inside the arena; never above the current count.
std::uint32_t IndexIn(std::uintptr_t base, std::uint32_t stride, std::uint32_t cap,
                      std::uintptr_t p, std::uint32_t fallback) {
    if (p < base || p >= base + static_cast<std::uintptr_t>(cap) * stride) return fallback;
    const std::uint32_t i = static_cast<std::uint32_t>((p - base) / stride);
    return i < fallback ? i : fallback;
}

// Write set of 0x005757d0: param_2 +0xd0, and through support slot +0x18 / FUN_0055c0f0 the records at
// param_2 and param_2+0x80 plus their point arrays (RwpSolverCore18.cpp:113-131).
void Regs_005757d0(C4nRegions& rg, int*, int p2) {
    if (!Writable(At(p2), 0x120)) { rg.bad = true; return; }
    AddContactRecord(rg, static_cast<std::uintptr_t>(p2));
}

// Write set of 0x00575880: W header (+0x30/+0x3c/+0x78), the record *(W+0x2c)+*(W+0x30)*0x120, and the
// point pool from *(W+0x38)+*(W+0x3c)*0x10 up to its capacity *(W+0x40) (RwpSolverCore20.cpp:95).
void Regs_00575880(C4nRegions& rg, int W, int, int*, float) {
    const auto w = static_cast<std::uintptr_t>(W);
    if (!Writable(At(w), 0x80)) { rg.bad = true; return; }
    rg.add(At(w), 0x80);
    rg.add(At(U32(w + 0x2c) + U32(w + 0x30) * 0x120), 0x120);
    const std::uint32_t cnt = U32(w + 0x3c), cap = U32(w + 0x40);
    if (cap > cnt) {
        std::uint32_t n = (cap - cnt) * 0x10;
        if (n > 0x400) n = 0x400;
        rg.add(At(U32(w + 0x38) + cnt * 0x10), n);
    }
}

// Write set of 0x00575c60: appends from the current PA / MA counts (RwpSolverCore18.cpp:475-639).
void Regs_00575c60(C4nRegions& rg, undefined4* p1, uint*, float*, float*, float*, undefined4) {
    const auto w = reinterpret_cast<std::uintptr_t>(p1);
    if (!Writable(At(w), 0x80)) { rg.bad = true; return; }
    AddWorldArenas(rg, w, U32(w + 0x10), U32(w + 0x1c));
}

// Write set of 0x00575fe0: resets the counts to the indices of param_7 (PA) and param_5 (MA) at entry
// (RwpSolverCore17.cpp:474-475), so the arenas are covered from those indices.
void Regs_00575fe0(C4nRegions& rg, int W, uint*, undefined4, int, uint* p5, int, uint* p7,
                   float*, float*, uint) {
    const auto w = static_cast<std::uintptr_t>(W);
    if (!Writable(At(w), 0x80)) { rg.bad = true; return; }
    const std::uint32_t pS = IndexIn(U32(w + 0x0c), 0x14, U32(w + 0x14),
                                     reinterpret_cast<std::uintptr_t>(p7), U32(w + 0x10));
    const std::uint32_t mS = IndexIn(U32(w + 0x18), 0x40, U32(w + 0x20),
                                     reinterpret_cast<std::uintptr_t>(p5), U32(w + 0x1c));
    AddWorldArenas(rg, w, pS, mS);
}

// Write set of 0x00576640: param_1[1], param_1[0x1e], and the pair array *param_1 for param_1[2]*8 bytes
// (RwpSolverCore16.cpp:209-232).
void Regs_00576640(C4nRegions& rg, int* p1, uint, uint, int, int) {
    if (!Writable(p1, 0x80)) { rg.bad = true; return; }
    rg.add(p1, 0x80);
    rg.add(At(static_cast<std::uint32_t>(p1[0])), static_cast<std::uint32_t>(p1[2]) * 8);
}

// Write set of 0x00577ec0: the three scalar outputs; param_10 may alias param_9 (RwpSolverCore14.cpp:235).
void Regs_00577ec0(C4nRegions& rg, float*, float*, float, float, float*, float*, float, float,
                   float* p9, float* p10, float* p11) {
    rg.add(p9, 4);
    if (p10 != p9) rg.add(p10, 4);
    if (p11 != p9 && p11 != p10) rg.add(p11, 4);
}

// Write set of 0x005784a0: the count *param_1 and the element appended at param_2 + count*0x10
// (RwpSolverCore14.cpp:322-325).
void Regs_005784a0(C4nRegions& rg, uint* p1, float* p2, float*, float*, float) {
    if (!Writable(p1, 4)) { rg.bad = true; return; }
    rg.add(p1, 4);
    rg.add(reinterpret_cast<unsigned char*>(p2) + static_cast<std::uintptr_t>(*p1) * 0x10, 0x10);
}

// Write set of 0x00578610: param_4 (12 bytes), param_5, param_6 (RwpSolverCore18.cpp:679-818).
void Regs_00578610(C4nRegions& rg, undefined4, int*, float, float* p4, float* p5, float* p6) {
    rg.add(p4, 12);
    rg.add(p5, 4);
    rg.add(p6, 4);
}

// Write set of 0x00578e50: param_4 +0x100..+0x113 via 0x00578610, then 0x005757d0's set on the same record
// (RwpSolverCore19.cpp:79-82).
void Regs_00578e50(C4nRegions& rg, undefined4, undefined4, float, int p4, undefined4) {
    if (!Writable(At(p4), 0x120)) { rg.bad = true; return; }
    AddContactRecord(rg, static_cast<std::uintptr_t>(p4));
}

// Write set of 0x00579b50: no writes (RwpSolverCore16.cpp:241-257); return value only.
void Regs_00579b50(C4nRegions&, float*, int, float) {}

}  // namespace

// Registration. These are OPT-IN probes: the "L2_" name prefix keeps HookSystem from
// installing them unless MASHED_HOOK_ONLY names one (Core/HookSystem.cpp:248-255), and a probe
// and the FUN_<rva> port hook at the same RVA are never both selected by the C4 runs. The
// rva-body lint's DUP-INSTALL rule (scripts/lint_rva_bodies.py) guards against a body that is
// silently dead in a DEFAULT .asi run, which cannot happen here, so the probes register through
// HookSystem::Register directly rather than RH_ScopedInstall.
#define C4N_REGISTER(Method, Rva)                                                      static struct C4nAutoRegister_##Method {                                               C4nAutoRegister_##Method() {                                                           ::HookSystem::Register((Rva), reinterpret_cast<void*>(&Method), #Method);         }                                                                              } s_c4nAutoReg_##Method

// probe for 0x005757d0
static C4nProbe s_c4n_005757d0{"L2_C4N_005757d0", 0x005757d0};
extern "C" bool __cdecl L2_C4N_005757d0(int* a, int b) {
    return C4nRun(s_c4n_005757d0, &FUN_005757d0, &Regs_005757d0, a, b);
}
C4N_REGISTER(L2_C4N_005757d0, 0x005757d0u);

// probe for 0x00575880
static C4nProbe s_c4n_00575880{"L2_C4N_00575880", 0x00575880};
extern "C" undefined4 __cdecl L2_C4N_00575880(int a, int b, int* c, float d) {
    return C4nRun(s_c4n_00575880, &FUN_00575880, &Regs_00575880, a, b, c, d);
}
C4N_REGISTER(L2_C4N_00575880, 0x00575880u);

// probe for 0x00575c60
static C4nProbe s_c4n_00575c60{"L2_C4N_00575c60", 0x00575c60};
extern "C" int __cdecl L2_C4N_00575c60(undefined4* a, uint* b, float* c, float* d, float* e,
                                       undefined4 f) {
    return C4nRun(s_c4n_00575c60, &FUN_00575c60, &Regs_00575c60, a, b, c, d, e, f);
}
C4N_REGISTER(L2_C4N_00575c60, 0x00575c60u);

// probe for 0x00575fe0
static C4nProbe s_c4n_00575fe0{"L2_C4N_00575fe0", 0x00575fe0};
extern "C" uint __cdecl L2_C4N_00575fe0(int a, uint* b, undefined4 c, int d, uint* e, int f,
                                        uint* g, float* h, float* i, uint j) {
    return C4nRun(s_c4n_00575fe0, &FUN_00575fe0, &Regs_00575fe0, a, b, c, d, e, f, g, h, i, j);
}
C4N_REGISTER(L2_C4N_00575fe0, 0x00575fe0u);

// probe for 0x00576640
static C4nProbe s_c4n_00576640{"L2_C4N_00576640", 0x00576640};
extern "C" uint __cdecl L2_C4N_00576640(int* a, uint b, uint c, int d, int e) {
    return C4nRun(s_c4n_00576640, &FUN_00576640, &Regs_00576640, a, b, c, d, e);
}
C4N_REGISTER(L2_C4N_00576640, 0x00576640u);

// probe for 0x00577ec0
static C4nProbe s_c4n_00577ec0{"L2_C4N_00577ec0", 0x00577ec0};
extern "C" undefined4 __cdecl L2_C4N_00577ec0(float* a, float* b, float c, float d, float* e,
                                              float* f, float g, float h, float* i, float* j,
                                              float* k) {
    return C4nRun(s_c4n_00577ec0, &FUN_00577ec0, &Regs_00577ec0, a, b, c, d, e, f, g, h, i, j, k);
}
C4N_REGISTER(L2_C4N_00577ec0, 0x00577ec0u);

// probe for 0x005784a0
static C4nProbe s_c4n_005784a0{"L2_C4N_005784a0", 0x005784a0};
extern "C" undefined4 __cdecl L2_C4N_005784a0(uint* a, float* b, float* c, float* d, float e) {
    return C4nRun(s_c4n_005784a0, &FUN_005784a0, &Regs_005784a0, a, b, c, d, e);
}
C4N_REGISTER(L2_C4N_005784a0, 0x005784a0u);

// probe for 0x00578610
static C4nProbe s_c4n_00578610{"L2_C4N_00578610", 0x00578610};
extern "C" undefined4 __cdecl L2_C4N_00578610(undefined4 a, int* b, float c, float* d, float* e,
                                              float* f) {
    return C4nRun(s_c4n_00578610, &FUN_00578610, &Regs_00578610, a, b, c, d, e, f);
}
C4N_REGISTER(L2_C4N_00578610, 0x00578610u);

// probe for 0x00578e50
static C4nProbe s_c4n_00578e50{"L2_C4N_00578e50", 0x00578e50};
extern "C" undefined4 __cdecl L2_C4N_00578e50(undefined4 a, undefined4 b, float c, int d,
                                              undefined4 e) {
    return C4nRun(s_c4n_00578e50, &FUN_00578e50, &Regs_00578e50, a, b, c, d, e);
}
C4N_REGISTER(L2_C4N_00578e50, 0x00578e50u);

// probe for 0x00579b50
static C4nProbe s_c4n_00579b50{"L2_C4N_00579b50", 0x00579b50};
extern "C" undefined4 __cdecl L2_C4N_00579b50(float* a, int b, float c) {
    return C4nRun(s_c4n_00579b50, &FUN_00579b50, &Regs_00579b50, a, b, c);
}
C4N_REGISTER(L2_C4N_00579b50, 0x00579b50u);
