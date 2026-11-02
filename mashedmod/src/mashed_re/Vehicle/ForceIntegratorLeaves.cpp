// Mashed RE - .asi install + MASHED_LEAF_SELFTEST A/B for the two rubber-band leaves A5 (0x0046ddb0) calls: 0x00442ce0
// and 0x00442c80. [MATCH-S08-FU 2026-11-02] Their bodies are the byte-matched sources in Match/MatchS08_00442ce0.cpp and
// Match/MatchS08_00442c80.cpp (both targets), which also define the C++ names mashed_re::Vehicle::RubberBandGrip /
// RubberBandGate. This TU keeps only the .asi entries below (in both rsp files; the exe part is empty).
// Original call sites (scan of .text): 0x00442c80 from 0x0041340a and 0x0046e233; 0x00442ce0 from 0x0046e20f.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include "ForceIntegrator.h"
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

extern "C" std::uint32_t __cdecl VehicleRacePositionGet(int param_1);   // FUN_0046dbe0, Vehicle/VehicleState.cpp


#ifndef MASHED_STANDALONE
// ===========================================================================================================================
// .asi install + in-process A/B. With MASHED_LEAF_SELFTEST=1 every call runs the original twice (noo) and the shared body
// once on the same inputs (both leaves are pure: they read globals and write nothing) and logs to leaf_selftest.log in the
// CWD; the GAME gets the shared body's value either way, so the install is live. MASHED_LEAF_SELFTEST_CAP (default 3000) =
// per-leaf calls logged one line each; past it only mismatches and a summary every 1000 calls.
// ===========================================================================================================================
namespace {

// Trampolines into the original. The 5-byte E9 hook covers 0x00442c80..0x00442c84 = the whole CALL 0x0040e350, so re-run
// that call and resume at 0x00442c85. At 0x00442ce0 it covers PUSH ECX (1 byte) and 4 of the 5 bytes of MOV EAX,[0x007f0fd0];
// re-run both and resume at 0x00442ce6.
void* g_leafGateResume = reinterpret_cast<void*>(0x00442c85);
void* g_leafGripResume = reinterpret_cast<void*>(0x00442ce6);
__declspec(naked) int __cdecl LeafOrigGate(int) {
    __asm {
        mov  eax, 0x0040e350
        call eax
        jmp  dword ptr [g_leafGateResume]
    }
}
__declspec(naked) float __cdecl LeafOrigGrip(int, float, float) {
    __asm {
        push ecx
        mov  eax, 0x007f0fd0
        mov  eax, dword ptr [eax]
        jmp  dword ptr [g_leafGripResume]
    }
}

int LeafSelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
long LeafSelfTestCap() {
    static long v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST_CAP"); v = (s && s[0]) ? std::atol(s) : 3000; }
    return v;
}
void LeafLog(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
std::uint32_t Bits(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

long g_gateCalls = 0, g_gateNoo = 0, g_gateNdiff = 0, g_gateOnes = 0;
long g_gripCalls = 0, g_gripNoo = 0, g_gripNdiff = 0, g_gripTier = 0;

__declspec(noinline) int __cdecl LeafGateDispatch(int car) {
    using mashed_re::Vehicle::RubberBandGate;
    if (!LeafSelfTestOn()) return RubberBandGate(car);
    const int o  = LeafOrigGate(car);
    const int o2 = LeafOrigGate(car);
    const int m  = RubberBandGate(car);
    ++g_gateCalls;
    const int noo = (o != o2), nd = (m != o);
    g_gateNoo += noo; g_gateNdiff += nd; g_gateOnes += (o == 1);
    if (g_gateCalls <= LeafSelfTestCap() || noo || nd) {
        char line[200];
        wsprintfA(line, "[%ld] fn=442c80 car=%d gm=%d fd0=%d rd=%08x o=%d o2=%d m=%d noo=%d ndiff=%d\r\n", g_gateCalls, car,
                  reinterpret_cast<int (__cdecl*)()>(0x0040e350u)(), *reinterpret_cast<const int*>(0x007f0fd0u),
                  Bits(reinterpret_cast<const float*>(0x008989b0u)[car]), o, o2, m, noo, nd);
        LeafLog(line);
    }
    if (g_gateCalls % 1000 == 0) {
        char line[160];
        wsprintfA(line, "SUM fn=442c80 calls=%ld noo=%ld ndiff=%ld ones=%ld\r\n", g_gateCalls, g_gateNoo, g_gateNdiff, g_gateOnes);
        LeafLog(line);
    }
    return m;
}

// Branch sweep (MASHED_LEAF_SELFTEST_SWEEP=<n>): no recorded race reaches the tier branches (refdist == 0 with record +0x08
// non-zero on both cars), so on the first n self-tested calls the inputs the leaf reads are seeded IN PLACE, for every
// (i1, i2) in 0..3 x 0..3 with refdist 0, then put back bit for bit before the call returns: record +0x08 of `car` and of the
// reference car (0x008815a8 + slot*0xd04) and refdist 0x008989b0[car]. The band and the fd0 / game-mode words are the live
// ones. Synthetic seeding: path1-grade evidence for those branches, not a natural sample.
long LeafSweepN() {
    static long v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST_SWEEP"); v = (s && s[0]) ? std::atol(s) : 0; }
    return v;
}
long g_sweepCases = 0, g_sweepNoo = 0, g_sweepNdiff = 0, g_sweepTier = 0;
void LeafGripSweep(int car, float cur, float band) {
    using mashed_re::Vehicle::RubberBandGrip;
    const int rc = *reinterpret_cast<const int*>(0x008989c8u);
    if (car < 0 || car > 15 || rc < 0 || rc > 15) return;
    std::uint32_t* f1 = reinterpret_cast<std::uint32_t*>(0x008815a8u + static_cast<std::uintptr_t>(car) * 0xd04u);
    std::uint32_t* f2 = reinterpret_cast<std::uint32_t*>(0x008815a8u + static_cast<std::uintptr_t>(rc) * 0xd04u);
    float* rd = reinterpret_cast<float*>(0x008989b0u) + car;
    const std::uint32_t s1 = *f1, s2 = *f2; const float srd = *rd;
    // band: the live value, then fixed bit patterns (every recorded race passes 1.0): 0.2, pi, 0.001, -2.5, FLT_MAX, the
    // smallest denormal, +inf.
    static const std::uint32_t kBandBits[] = {0, 0x3e4ccccdu, 0x40490fdbu, 0x3a83126fu, 0xc0200000u, 0x7f7fffffu, 0x00000001u,
                                              0x7f800000u};
    for (int k = 0; k < 8; ++k) {
    float bd = band;
    if (k) std::memcpy(&bd, &kBandBits[k], 4);
    for (std::uint32_t a = 0; a < 4; ++a) {
        for (std::uint32_t b = 0; b < 4; ++b) {
            *rd = 0.0f; *f1 = a; *f2 = b;                    // car == rc: the second store wins, as the original reads it
            const float o  = LeafOrigGrip(car, cur, bd);
            const float o2 = LeafOrigGrip(car, cur, bd);
            const float m  = RubberBandGrip(car, cur, bd);
            const int noo = (Bits(o) != Bits(o2)), nd = (Bits(m) != Bits(o));
            ++g_sweepCases; g_sweepNoo += noo; g_sweepNdiff += nd; g_sweepTier += (Bits(o) != Bits(1.0f));
            char line[200];
            wsprintfA(line, "[sw%ld] fn=442ce0 car=%d ref=%d i1=%u i2=%u band=%08x o=%08x m=%08x noo=%d ndiff=%d\r\n",
                      g_sweepCases, car, rc, *f1, *f2, Bits(bd), Bits(o), Bits(m), noo, nd);
            LeafLog(line);
        }
    }
    }
    *f1 = s1; *f2 = s2; *rd = srd;                           // f2 last: when car == rc the saved s2 == s1
    char line[160];
    wsprintfA(line, "SWEEP fn=442ce0 cases=%ld noo=%ld ndiff=%ld tier=%ld\r\n", g_sweepCases, g_sweepNoo, g_sweepNdiff, g_sweepTier);
    LeafLog(line);
}

__declspec(noinline) float __cdecl LeafGripDispatch(int car, float cur, float band) {
    using mashed_re::Vehicle::RubberBandGrip;
    if (!LeafSelfTestOn()) return RubberBandGrip(car, cur, band);
    if (g_gripCalls < LeafSweepN()) LeafGripSweep(car, cur, band);
    const float o  = LeafOrigGrip(car, cur, band);
    const float o2 = LeafOrigGrip(car, cur, band);
    const float m  = RubberBandGrip(car, cur, band);
    ++g_gripCalls;
    const int noo = (Bits(o) != Bits(o2)), nd = (Bits(m) != Bits(o));
    g_gripNoo += noo; g_gripNdiff += nd; g_gripTier += (Bits(o) != Bits(1.0f));
    if (g_gripCalls <= LeafSelfTestCap() || noo || nd) {
        const int rc = *reinterpret_cast<const int*>(0x008989c8u);
        char line[260];
        wsprintfA(line, "[%ld] fn=442ce0 car=%d fd0=%d rd=%08x ref=%d i1=%u i2=%u band=%08x o=%08x o2=%08x m=%08x noo=%d ndiff=%d\r\n",
                  g_gripCalls, car, *reinterpret_cast<const int*>(0x007f0fd0u),
                  Bits(reinterpret_cast<const float*>(0x008989b0u)[car]), rc, VehicleRacePositionGet(car),
                  VehicleRacePositionGet(rc), Bits(band), Bits(o), Bits(o2), Bits(m), noo, nd);
        LeafLog(line);
    }
    if (g_gripCalls % 1000 == 0) {
        char line[160];
        wsprintfA(line, "SUM fn=442ce0 calls=%ld noo=%ld ndiff=%ld tier=%ld\r\n", g_gripCalls, g_gripNoo, g_gripNdiff, g_gripTier);
        LeafLog(line);
    }
    return m;
}

}  // namespace

// Exports: the installed entries (also the path1 targets in re/frida/hooks_registry.py).
extern "C" __declspec(dllexport) int __cdecl RubberBandGateEntry(int car) { return LeafGateDispatch(car); }
extern "C" __declspec(dllexport) float __cdecl RubberBandGripEntry(int car, float cur, float band) {
    return LeafGripDispatch(car, cur, band);
}
RH_ScopedInstall(RubberBandGateEntry, 0x00442c80);
RH_ScopedInstall(RubberBandGripEntry, 0x00442ce0);
#endif
