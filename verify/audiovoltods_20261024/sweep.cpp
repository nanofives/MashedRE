// D4-AUDIO-VOLTODS offline sweep (PREREG_voltods.md arms S1-S3). Build: build_sweep.bat. Run: sweep.exe pages.bin
// Maps the original's 0x005aeb90 page (.text) and the two .rdata pages at their own addresses, then calls the
// original's 97 bytes on this CPU against:
//   S1  the exe body (Audio/AudioLinearToLog.cpp compiled /DMASHED_STANDALONE): every 2^32 input, per precision control;
//   S2  the port VolToDsPort (verbatim copy of AudioEngine.cpp's, g_focusGain = 1): every 2^32 input, per range;
//   S3  the original at PC_24 against the original at PC_53 (does the caller's x87 precision change the result?).
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <float.h>

namespace HookSystem { void Register(std::uintptr_t, void*, const char*) {} }
#include "../../mashedmod/src/mashed_re/Audio/AudioLinearToLog.cpp"

// Verbatim copy of AudioEngine.cpp VolToDsPort (DSBVOLUME_MIN = -10000, DSBVOLUME_MAX = 0).
static LONG VolToDsPort(float v) {
    if (v <= 0.0001f) return -10000;
    if (v >= 1.0f)    return 0;
    LONG db = static_cast<LONG>(2000.0 * std::log10(static_cast<double>(v)));
    if (db < -10000) db = -10000;
    if (db > 0) db = 0;
    return db;
}

typedef uint32_t(__cdecl* Fn)(float);
static Fn g_orig = nullptr;

// Range classes of the input bit pattern.
enum { R_NEG, R_ZERO, R_LT_1EM5, R_1EM5_1EM4, R_1EM4_1, R_GE_1, R_INF, R_NAN, R_N };
static const char* kRange[R_N] = {"negative (finite, <0)", "+0", "(0, 1e-5f)", "[1e-5f, 1e-4f]", "(1e-4f, 1)", "[1, FLT_MAX]",
                                  "+inf", "NaN"};
static int RangeOf(uint32_t b) {
    float v; std::memcpy(&v, &b, 4);
    if (v != v) return R_NAN;
    if (b == 0x7f800000u) return R_INF;
    if (b & 0x80000000u) return R_NEG;   // includes -0 and -inf
    if (b == 0) return R_ZERO;
    if (v < 1e-5f) return R_LT_1EM5;
    if (v <= 1e-4f) return R_1EM5_1EM4;
    if (v < 1.0f) return R_1EM4_1;
    return R_GE_1;
}

struct Acc {
    uint64_t s1_mis = 0, s3_mis = 0;
    uint64_t n[R_N] = {}, s2_mis[R_N] = {};
    int64_t  s2_maxabs[R_N] = {};
    uint32_t s2_first[R_N] = {}, s2_last[R_N] = {};
    std::map<int64_t, uint64_t> s2_delta;          // orig - port, over the (0, 1] domain only
    std::vector<uint32_t> s1_ex, s3_ex;
};

static void Work(unsigned pc, uint64_t lo, uint64_t hi, Acc* a, bool do_s2) {
    unsigned cw = 0;
    _controlfp_s(&cw, pc, _MCW_PC);
    for (uint64_t i = lo; i < hi; ++i) {
        const uint32_t b = static_cast<uint32_t>(i);
        float v; std::memcpy(&v, &b, 4);
        const uint32_t o = g_orig(v);
        const uint32_t p = AudioLinearToLog(v);
        if (o != p) { ++a->s1_mis; if (a->s1_ex.size() < 8) a->s1_ex.push_back(b); }
        if (!do_s2) continue;
        const int r = RangeOf(b);
        ++a->n[r];
        const int64_t q = VolToDsPort(v);
        const int64_t oi = static_cast<int32_t>(o);
        if (oi != q) {
            if (!a->s2_mis[r]) a->s2_first[r] = b;
            a->s2_last[r] = b;
            ++a->s2_mis[r];
            const int64_t d = oi - q, ad = d < 0 ? -d : d;
            if (ad > a->s2_maxabs[r]) a->s2_maxabs[r] = ad;
            if (b <= 0x3f800000u) ++a->s2_delta[d];
        }
    }
}

static void WorkS3(uint64_t lo, uint64_t hi, Acc* a) {
    for (uint64_t i = lo; i < hi; ++i) {
        const uint32_t b = static_cast<uint32_t>(i);
        float v; std::memcpy(&v, &b, 4);
        unsigned cw = 0;
        _controlfp_s(&cw, _PC_24, _MCW_PC);
        const uint32_t o24 = g_orig(v);
        _controlfp_s(&cw, _PC_53, _MCW_PC);
        const uint32_t o53 = g_orig(v);
        if (o24 != o53) { ++a->s3_mis; if (a->s3_ex.size() < 8) a->s3_ex.push_back(b); }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) { std::printf("usage: sweep.exe pages.bin\n"); return 2; }
    FILE* f = std::fopen(argv[1], "rb");
    if (!f) { std::printf("cannot open %s\n", argv[1]); return 2; }
    for (int k = 0; k < 3; ++k) {
        uint32_t va = 0; unsigned char page[0x1000];
        if (std::fread(&va, 4, 1, f) != 1 || std::fread(page, 1, 0x1000, f) != 0x1000) { std::printf("short read\n"); return 2; }
        // Reservations are 64 KB aligned: reserve the enclosing region once (0x005a0000, 0x005c0000), then commit the page.
        VirtualAlloc(reinterpret_cast<void*>(va & ~0xffffu), 0x10000, MEM_RESERVE, PAGE_NOACCESS);
        void* m = VirtualAlloc(reinterpret_cast<void*>(va), 0x1000, MEM_COMMIT, PAGE_READWRITE);
        if (m != reinterpret_cast<void*>(va)) { std::printf("VirtualAlloc %08x failed (%p, err %lu)\n", va, m, GetLastError()); return 3; }
        std::memcpy(m, page, 0x1000);
        DWORD old = 0;
        VirtualProtect(m, 0x1000, k == 0 ? PAGE_EXECUTE_READ : PAGE_READONLY, &old);
    }
    std::fclose(f);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(0x005ae000), 0x1000);
    g_orig = reinterpret_cast<Fn>(0x005aeb90);

    const unsigned T = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4;
    const uint64_t N = 1ull << 32;
    struct Pass { const char* name; unsigned pc; bool s2; } passes[] = {{"PC_53", _PC_53, true}, {"PC_24", _PC_24, false},
                                                                          {"PC_64", _PC_64, false}};
    int step = 0;
    const int steps = 5;
    Acc s2acc;
    for (const Pass& ps : passes) {
        std::vector<Acc> acc(T);
        std::vector<std::thread> th;
        for (unsigned t = 0; t < T; ++t) th.emplace_back(Work, ps.pc, N * t / T, N * (t + 1) / T, &acc[t], ps.s2);
        for (auto& x : th) x.join();
        uint64_t mis = 0; std::vector<uint32_t> ex;
        for (auto& a : acc) { mis += a.s1_mis; for (uint32_t b : a.s1_ex) if (ex.size() < 8) ex.push_back(b); }
        std::printf("[%d/%d] S1 %s orig vs exe body: %llu / %llu mismatches %s\n", ++step, steps, ps.name,
                    (unsigned long long)mis, (unsigned long long)N, mis ? "RED" : "GREEN");
        for (uint32_t b : ex) std::printf("    ex %08x\n", b);
        if (ps.s2) {
            for (auto& a : acc) {
                for (int r = 0; r < R_N; ++r) {
                    if (a.s2_mis[r] && (!s2acc.s2_mis[r] || a.s2_first[r] < s2acc.s2_first[r])) s2acc.s2_first[r] = a.s2_first[r];
                    if (a.s2_mis[r] && a.s2_last[r] > s2acc.s2_last[r]) s2acc.s2_last[r] = a.s2_last[r];
                    s2acc.n[r] += a.n[r]; s2acc.s2_mis[r] += a.s2_mis[r];
                    if (a.s2_maxabs[r] > s2acc.s2_maxabs[r]) s2acc.s2_maxabs[r] = a.s2_maxabs[r];
                }
                for (auto& kv : a.s2_delta) s2acc.s2_delta[kv.first] += kv.second;
            }
        }
    }
    std::printf("[%d/%d] S2 PC_53 original vs port VolToDs, by input range (orig - port):\n", ++step, steps);
    std::printf("    %-24s %12s %12s %8s %10s %10s\n", "range", "inputs", "differ", "max|d|", "first", "last");
    for (int r = 0; r < R_N; ++r)
        std::printf("    %-24s %12llu %12llu %8lld %10s%08x %s%08x\n", kRange[r], (unsigned long long)s2acc.n[r],
                    (unsigned long long)s2acc.s2_mis[r], (long long)s2acc.s2_maxabs[r], "", s2acc.s2_first[r], "", s2acc.s2_last[r]);
    std::printf("    delta histogram over [+0, 1.0f] (orig - port : count):\n");
    for (auto& kv : s2acc.s2_delta) std::printf("      %lld : %llu\n", (long long)kv.first, (unsigned long long)kv.second);
    {
        std::vector<Acc> acc(T);
        std::vector<std::thread> th;
        for (unsigned t = 0; t < T; ++t) th.emplace_back(WorkS3, N * t / T, N * (t + 1) / T, &acc[t]);
        for (auto& x : th) x.join();
        uint64_t mis = 0; std::vector<uint32_t> ex;
        for (auto& a : acc) { mis += a.s3_mis; for (uint32_t b : a.s3_ex) if (ex.size() < 8) ex.push_back(b); }
        std::printf("[%d/%d] S3 original PC_24 vs PC_53: %llu / %llu inputs differ\n", ++step, steps, (unsigned long long)mis,
                    (unsigned long long)N);
        for (uint32_t b : ex) std::printf("    ex %08x\n", b);
    }
    return 0;
}
