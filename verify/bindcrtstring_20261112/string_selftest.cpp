// BIND-CRT-STRING self-test: compiles Match/PortTop_CrtString.cpp unchanged (MASHED_STANDALONE) with the exe toolchain
// and runs each binding against the ORIGINAL's own machine code, copied from original/MASHED.exe.unpatched into
// executable memory and called natively (the three bodies have no relocations and no absolute addresses).
//   strchr  image 0x004a2fc0..0x004a308e (found_bx tail + body), entry 0x004a2fd0
//   strrchr image 0x004a3480..0x004a34ad
//   wcsncpy image 0x004a3bdf..0x004a3c1c
// usage: string_selftest.exe <path to MASHED.exe.unpatched>
#define MASHED_STANDALONE 1
#include "../../mashedmod/src/mashed_re/Match/PortTop_CrtString.cpp"
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <random>
#include <string>

static std::vector<uint8_t> g_img;
static uint32_t pe_u32(size_t o) { return *reinterpret_cast<uint32_t*>(&g_img[o]); }
static uint16_t pe_u16(size_t o) { return *reinterpret_cast<uint16_t*>(&g_img[o]); }

static const uint8_t* img_at(uint32_t va) {
    uint32_t pe = pe_u32(0x3c), base = pe_u32(pe + 24 + 28);
    uint16_t nsec = pe_u16(pe + 6), opt = pe_u16(pe + 20);
    for (uint16_t i = 0; i < nsec; ++i) {
        size_t o = pe + 24 + opt + 40 * i;
        uint32_t vsize = pe_u32(o + 8), sva = base + pe_u32(o + 12), rsize = pe_u32(o + 16), raw = pe_u32(o + 20);
        if (va >= sva && va < sva + rsize && va < sva + (vsize > rsize ? vsize : rsize)) return &g_img[raw + (va - sva)];
    }
    return nullptr;
}

static void* load_span(uint32_t lo, uint32_t hi, uint32_t entry) {
    uint8_t* m = static_cast<uint8_t*>(VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    memcpy(m, img_at(lo), hi - lo);
    return m + (entry - lo);
}

typedef char*(__cdecl* StrFn)(const char*, int);
typedef wchar_t*(__cdecl* WcsFn)(wchar_t*, const wchar_t*, unsigned int);

int main(int argc, char** argv) {
    const char* path = argc > 1 ? argv[1] : "C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\original\\MASHED.exe.unpatched";
    FILE* f = fopen(path, "rb");
    if (!f) { printf("cannot open %s\n", path); return 2; }
    fseek(f, 0, SEEK_END); g_img.resize(ftell(f)); fseek(f, 0, SEEK_SET);
    fread(g_img.data(), 1, g_img.size(), f); fclose(f);
    if (g_img.size() != 2846720) { printf("unexpected image size %zu\n", g_img.size()); return 2; }

    StrFn o_strchr = (StrFn)load_span(0x004a2fc0, 0x004a308e, 0x004a2fd0);
    StrFn o_strrchr = (StrFn)load_span(0x004a3480, 0x004a34ad, 0x004a3480);
    WcsFn o_wcsncpy = (WcsFn)load_span(0x004a3bdf, 0x004a3c1c, 0x004a3bdf);

    std::mt19937 rng(12345);
    int pass = 0, fail = 0;
    auto check = [&](const char* name, bool ok, int n) {
        printf("[%d/6] %s %s (%d cases)\n", pass + fail + 1, name, ok ? "PASS" : "FAIL", n);
        ok ? ++pass : ++fail;
    };
    // strchr / strrchr: random strings at every alignment, c = 0, c > 0xff, absent, present (first / last / repeated)
    for (int which = 0; which < 2; ++which) {
        StrFn orig = which ? o_strrchr : o_strchr;
        StrFn bind = which ? MashedCrt_strrchr_004a3480 : MashedCrt_strchr_004a2fd0;
        int bad = 0, n = 0;
        alignas(16) static char buf[512];
        for (int t = 0; t < 20000; ++t) {
            int len = (t < 400) ? t % 40 : rng() % 300;
            int alpha = rng() % 3;
            memset(buf, 0x11, sizeof buf);
            char* s = buf + 16 + (t & 3);
            for (int k = 0; k < len; ++k) {
                int ch = alpha == 0 ? "ab"[rng() % 2] : alpha == 1 ? "abc/\\."[rng() % 6] : 1 + rng() % 255;
                s[k] = (char)ch;
            }
            s[len] = 0;
            int cs[5] = {0, 0x100 + (int)(rng() % 256), 1 + (int)(rng() % 255), len ? (unsigned char)s[rng() % len] : 'a',
                         -1 - (int)(rng() % 128)};
            for (int c : cs) {
                ++n;
                if (orig(s, c) != bind(s, c)) {
                    if (!bad) printf("  mismatch t=%d len=%d c=%d\n", t, len, c);
                    ++bad;
                }
            }
        }
        check(which ? "strrchr image==binding" : "strchr image==binding", bad == 0, n);
    }
    // wcsncpy: return value and the whole destination buffer (padding with L'\0' past the source, no terminator when
    // count <= length)
    {
        int bad = 0, n = 0;
        for (int t = 0; t < 20000; ++t) {
            wchar_t src[64], d1[80], d2[80];
            int len = rng() % 60;
            for (int k = 0; k < len; ++k) src[k] = (wchar_t)(1 + rng() % 0xfffe);
            src[len] = 0;
            unsigned cnt = rng() % 70;
            for (int k = 0; k < 80; ++k) d1[k] = d2[k] = (wchar_t)0xeeee;
            wchar_t* r1 = o_wcsncpy(d1, src, cnt);
            wchar_t* r2 = MashedCrt_wcsncpy_004a3bdf(d2, src, cnt);
            ++n;
            if (r1 != d1 || r2 != d2 || memcmp(d1, d2, sizeof d1)) {
                if (!bad) printf("  mismatch t=%d len=%d cnt=%u\n", t, len, cnt);
                ++bad;
            }
        }
        check("wcsncpy image==binding", bad == 0, n);
    }
    // fixed edge cases spelled out
    {
        const char* e = "";
        bool ok = o_strchr(e, 0) == e && MashedCrt_strchr_004a2fd0(e, 0) == e && o_strrchr(e, 0) == e &&
                  MashedCrt_strrchr_004a3480(e, 0) == e && o_strchr(e, 'a') == nullptr && MashedCrt_strchr_004a2fd0(e, 'a') == nullptr;
        check("empty string, c=0 and absent", ok, 6);
        const char* p = "a/b/c";
        ok = o_strchr(p, '/' + 0x100) == p + 1 && MashedCrt_strchr_004a2fd0(p, '/' + 0x100) == p + 1 &&
             o_strrchr(p, '/' + 0x100) == p + 3 && MashedCrt_strrchr_004a3480(p, '/' + 0x100) == p + 3;
        check("c > 0xff truncates to char", ok, 4);
        ok = o_strrchr(p, 0) == p + 5 && MashedCrt_strrchr_004a3480(p, 0) == p + 5 && o_strchr(p, 0) == p + 5 &&
             MashedCrt_strchr_004a2fd0(p, 0) == p + 5;
        check("c = 0 returns the terminator", ok, 4);
    }
    printf("%d/%d PASS\n", pass, pass + fail);
    return fail ? 1 : 0;
}
