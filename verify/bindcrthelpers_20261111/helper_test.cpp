// BIND-CRT-HELPERS unit test: the DDK helper bodies (bytes from MASHED.exe.unpatched, run in place from executable
// memory) against the exe thunks of mashedmod/src/mashed_re/Match/PortTop_CrtHelpers.cpp (included below, onto the
// VS2022 CRT helpers). Prints one line per check, "[n/N] name PASS|FAIL", then a total.
// build: cl /nologo /O2 /EHsc /DMASHED_STANDALONE helper_test.cpp   (x86, vcvars32)
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>
#include <string>
#include "orig_helpers.h"
#include "../../mashedmod/src/mashed_re/Match/PortTop_CrtHelpers.cpp"

static void* Load(const unsigned char* b, size_t n) {
    void* p = VirtualAlloc(nullptr, n, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    std::memcpy(p, b, n);
    return p;
}

struct Res { unsigned lo, hi; unsigned short sw; };

// fld the double, set the control word, call fn (ST0 in, EDX:EAX out), read the status word (TOP after the call)
static Res CallFtol(void* fn, double v, unsigned short cw) {
    Res r; unsigned short old, sw; unsigned lo, hi;
    __asm {
        fnstcw old
        fldcw cw
        fld v
        call fn
        mov lo, eax
        mov hi, edx
        fnstsw sw
        fldcw old
    }
    r.lo = lo; r.hi = hi; r.sw = static_cast<unsigned short>(sw & 0x3800);  // TOP field
    return r;
}

typedef unsigned __int64(__stdcall* Fn64)(unsigned, unsigned, unsigned, unsigned);

static int Chk(void* fn, unsigned bytes) {   // ESP drop across the call
    int d;
    __asm {
        mov ecx, esp
        mov eax, bytes
        call fn
        mov edx, ecx
        sub edx, esp
        mov esp, ecx
        mov d, edx
    }
    return d;
}

static int g_n = 0, g_fail = 0, g_total = 0;
static void Report(const std::string& name, bool ok, const char* detail) {
    ++g_n; if (!ok) ++g_fail;
    std::printf("[%d/%d] %s %s%s%s\n", g_n, g_total, name.c_str(), ok ? "PASS" : "FAIL", detail[0] ? "  " : "", detail);
}

int main() {
    void* o_ftol = Load(k_orig_ftol2, sizeof k_orig_ftol2);
    void* o_div = Load(k_orig_alldiv, sizeof k_orig_alldiv);
    void* o_mul = Load(k_orig_allmul, sizeof k_orig_allmul);
    void* o_udiv = Load(k_orig_aulldiv, sizeof k_orig_aulldiv);
    void* o_urem = Load(k_orig_aullrem, sizeof k_orig_aullrem);
    void* o_chk = Load(k_orig_chkstk, sizeof k_orig_chkstk);

    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> fv = {0.0, -0.0, 0.5, -0.5, 0.49999999, 1.5, -1.5, 2.5, -2.5, 7.999999, -7.999999,
                              123456789.987, -123456789.987, 2147483647.5, -2147483648.5, 4294967296.75,
                              1e10, -1e10, 9.2e18, -9.2e18, 9.3e18, -9.3e18, 1e19, -1e19, 1e300, -1e300,
                              inf, -inf, nan, 1e-300, -1e-300, 16777217.0, 0.1f, 33.3f};
    // precision PC_24 / PC_53 / PC_64 x rounding nearest / down / up / chop, all exceptions masked
    std::vector<unsigned short> cws;
    for (unsigned short pc : {0x000, 0x200, 0x300})
        for (unsigned short rc : {0x000, 0x400, 0x800, 0xc00}) cws.push_back(0x7f | pc | rc);
    struct V { unsigned alo, ahi, blo, bhi; };
    std::vector<V> iv = {{7, 0, 2, 0}, {0xfffffff9u, 0xffffffffu, 2, 0}, {7, 0, 0xfffffffeu, 0xffffffffu},
                         {0, 1, 3, 0}, {0x12345678u, 0x9abcdef0u, 0x0fedcba9u, 0x00000001u},
                         {0xffffffffu, 0x7fffffffu, 0xffffffffu, 0xffffffffu}, {0, 0x80000000u, 1, 0},
                         {123, 0, 0x00000001u, 0x00000001u}, {0xdeadbeefu, 0x00c0ffeeu, 0x10001u, 0}};
    std::vector<unsigned> ck = {4, 0x100, 0xfff, 0x1000, 0x1001, 0x3000, 0x10000};
    g_total = static_cast<int>(fv.size() * cws.size() + iv.size() * 4 + ck.size());

    char d[256];
    for (double v : fv)
        for (unsigned short cw : cws) {
            Res a = CallFtol(o_ftol, v, cw), b = CallFtol(reinterpret_cast<void*>(&MashedCrt_ftol2_004a2c48), v, cw);
            bool ok = a.lo == b.lo && a.hi == b.hi && a.sw == b.sw;
            std::snprintf(d, sizeof d, "v=%.17g cw=%04x orig=%08x%08x top=%x thunk=%08x%08x top=%x", v, cw, a.hi, a.lo,
                          a.sw >> 11, b.hi, b.lo, b.sw >> 11);
            Report("ftol2", ok, ok ? "" : d);
        }
    struct H { const char* n; void* o; void* t; } hs[] = {
        {"alldiv", o_div, reinterpret_cast<void*>(&MashedCrt_alldiv_004a4170)},
        {"allmul", o_mul, reinterpret_cast<void*>(&MashedCrt_allmul_004a4220)},
        {"aulldiv", o_udiv, reinterpret_cast<void*>(&MashedCrt_aulldiv_004a4e40)},
        {"aullrem", o_urem, reinterpret_cast<void*>(&MashedCrt_aullrem_004aa060)}};
    for (const H& h : hs)
        for (const V& v : iv) {
            unsigned __int64 a = reinterpret_cast<Fn64>(h.o)(v.alo, v.ahi, v.blo, v.bhi);
            unsigned __int64 b = reinterpret_cast<Fn64>(h.t)(v.alo, v.ahi, v.blo, v.bhi);
            std::snprintf(d, sizeof d, "a=%08x%08x b=%08x%08x orig=%016llx thunk=%016llx", v.ahi, v.alo, v.bhi, v.blo, a, b);
            Report(h.n, a == b, a == b ? "" : d);
        }
    for (unsigned n : ck) {
        int a = Chk(o_chk, n), b = Chk(reinterpret_cast<void*>(&MashedCrt_chkstk_004a3440), n);
        std::snprintf(d, sizeof d, "eax=0x%x orig_drop=0x%x thunk_drop=0x%x", n, a, b);
        Report("chkstk", a == b, a == b ? "" : d);
    }
    std::printf("TOTAL %d checks, %d FAIL\n", g_n, g_fail);
    return g_fail ? 1 : 0;
}
