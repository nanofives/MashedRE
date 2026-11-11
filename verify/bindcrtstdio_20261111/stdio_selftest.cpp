// BIND-CRT-STDIO self-test: compiles Match/PortTop_CrtStdio.cpp unchanged (MASHED_STANDALONE) into a small exe based
// away from 0x00400000, points the _tzset mirror (MASHED_CRT_TZ_BASE, 0x00616670 in the exe) at a buffer, and checks the write overlay, the read
// preference, ctime64, _vsnprintf and the mirrored time-zone values against the DDK libcmt values (tz_probe / pf_probe).
// Build and run: verify/bindcrtstdio_20261111/run_selftest.bat (cwd = a scratch directory it creates).
#define MASHED_STANDALONE 1
static long g_tz[3];
#define MASHED_CRT_TZ_BASE (reinterpret_cast<unsigned long>(g_tz))
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <string>
#include <direct.h>
#include "../../mashedmod/src/mashed_re/Match/PortTop_CrtStdio.cpp"

// The one disk-path rule, copied from Match/PortTop_RwStream04F.cpp (that TU pulls in the piz layer).
extern "C" void PortTop_Fs_DiskPath(const char* name, char* path, unsigned size) {
    const char* p = name;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(path, size, "original/%s", p);
    for (char* c = path; *c; ++c) if (*c == '\\') *c = '/';
}

static int g_n = 0, g_fail = 0;
static void Check(const char* name, bool ok, const char* detail) {
    ++g_n;
    if (!ok) ++g_fail;
    std::printf("[%d] %s %s%s%s\n", g_n, name, ok ? "PASS" : "FAIL", detail ? "  " : "", detail ? detail : "");
}
static void Put(const char* path, const char* text) {
    std::FILE* f = std::fopen(path, "wb");
    std::fputs(text, f);
    std::fclose(f);
}
static std::string Get(const char* path) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return "<none>";
    char b[256] = {};
    std::size_t n = std::fread(b, 1, sizeof(b) - 1, f);
    std::fclose(f);
    return std::string(b, n);
}
static std::string ReadVia(const char* name, const char* mode) {
    std::FILE* f = MashedCrt_fopen_004a4541(name, mode);
    if (!f) return "<NULL>";
    char b[256] = {};
    unsigned n = MashedCrt_fread_004a49cf(b, 1, sizeof(b) - 1, f);
    MashedCrt_fclose_004a4368(f);
    return std::string(b, n);
}
static int Vs(char* buf, unsigned count, const char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    int r = MashedCrt_vsnprintf_004a3b84(buf, count, fmt, reinterpret_cast<char*>(a));
    va_end(a);
    return r;
}

int main() {
    _mkdir("original");
    _mkdir("original\\sub");
    Put("original/videocfg.bin", "ORIG-VIDEO");
    Put("original/sub/log.txt", "ORIG-LOG|");
    _putenv("MASHED_ORIGTOP_WRITE_DIR=");

    Check("read, no overlay -> original/", ReadVia("videocfg.bin", "rb") == "ORIG-VIDEO", nullptr);
    {
        std::FILE* f = MashedCrt_fopen_004a4541("videocfg.bin", "wb");
        bool ok = f != nullptr;
        if (f) { MashedCrt_fwrite_004a4b22("NEW-VIDEO", 1, 9, f); MashedCrt_fclose_004a4368(f); }
        Check("wb -> overlay file opened", ok, nullptr);
    }
    Check("wb leaves original/ unchanged", Get("original/videocfg.bin") == "ORIG-VIDEO", Get("original/videocfg.bin").c_str());
    Check("wb wrote origtop_rw/videocfg.bin", Get("origtop_rw/videocfg.bin") == "NEW-VIDEO", Get("origtop_rw/videocfg.bin").c_str());
    Check("read prefers overlay", ReadVia("C:\\videocfg.bin", "rb") == "NEW-VIDEO", nullptr);
    {
        std::FILE* f = MashedCrt_fopen_004a4541("sub\\log.txt", "at");
        bool ok = f && MashedCrt_fputs_004a4254("APPENDED", f) == 0;
        if (f) MashedCrt_fclose_004a4368(f);
        Check("at copies original/ then appends", ok && Get("origtop_rw/sub/log.txt") == "ORIG-LOG|APPENDED",
              Get("origtop_rw/sub/log.txt").c_str());
    }
    Check("at leaves original/ unchanged", Get("original/sub/log.txt") == "ORIG-LOG|", nullptr);
    {
        std::FILE* f = MashedCrt_fopen_004a4541("deep\\er\\new.bin", "wb");
        bool ok = f != nullptr;
        if (f) MashedCrt_fclose_004a4368(f);
        Check("wb creates nested overlay dirs", ok && Get("origtop_rw/deep/er/new.bin") == "", nullptr);
    }
    Check("read of a missing file -> NULL", ReadVia("missing.bin", "rb") == "<NULL>", nullptr);
    _putenv("MASHED_ORIGTOP_WRITE_DIR=custom_rw");
    {
        std::FILE* f = MashedCrt_fopen_004a4541("videocfg.bin", "wb");
        if (f) MashedCrt_fclose_004a4368(f);
        Check("MASHED_ORIGTOP_WRITE_DIR moves the overlay", f && GetFileAttributesA("custom_rw\\videocfg.bin") != INVALID_FILE_ATTRIBUTES, nullptr);
    }
    _putenv("MASHED_ORIGTOP_WRITE_DIR=off");
    Check("off: write mode -> NULL", MashedCrt_fopen_004a4541("videocfg.bin", "wb") == nullptr, nullptr);
    Check("off: read -> original/", ReadVia("videocfg.bin", "rb") == "ORIG-VIDEO", nullptr);
    Check("NULL name / mode -> NULL", !MashedCrt_fopen_004a4541(nullptr, "rb") && !MashedCrt_fopen_004a4541("x", nullptr), nullptr);

    // _vsnprintf: DDK libcmt values (pf_probe, pf3_ddk.exe).
    char b[64];
    std::memset(b, 0, sizeof(b));
    Vs(b, 16, "%e", 1.0);
    Check("vsnprintf %e three-digit exponent", !std::strcmp(b, "1.000000e+000"), b);
    std::memset(b, 0, sizeof(b));
    Vs(b, 16, "%.0f", 2.5);
    Check("vsnprintf %.0f 2.5 -> 3", !std::strcmp(b, "3"), b);
    std::memset(b, 0, sizeof(b));
    int r = Vs(b, 16, "0123456789abcdefXYZ");
    Check("vsnprintf truncation -> -1, no terminator", r == -1 && !std::memcmp(b, "0123456789abcdef", 16) && b[16] == 0, nullptr);
    volatile double z = 0.0, o = 1.0;
    std::memset(b, 0, sizeof(b));
    Vs(b, 16, "%f", o / z);
    Check("vsnprintf inf -> 1.#INF00", !std::strcmp(b, "1.#INF00"), b);

    // _ctime64 and _tzset (TZ=PST8PDT, DDK values from tz_probe: timezone 28800, daylight 80, dstbias -3600).
    _putenv("TZ=PST8PDT");
    {
        MashedCrt_tzset_004aa00c();
        long tz = g_tz[0];
        int day = static_cast<int>(g_tz[1]);
        long dst = g_tz[2];
        char d[96];
        std::snprintf(d, sizeof(d), "timezone=%ld daylight=%d dstbias=%ld", tz, day, dst);
        Check("tzset mirror = DDK (PST8PDT)", tz == 28800 && day == 80 && dst == -3600, d);
        _putenv("TZ=JST-9");
        MashedCrt_tzset_004aa00c();
        tz = g_tz[0];
        day = static_cast<int>(g_tz[1]);
        std::snprintf(d, sizeof(d), "timezone=%ld daylight=%d", tz, day);
        Check("tzset mirror = DDK (JST-9)", tz == -32400 && day == 0, d);
    }
    _putenv("TZ=PST8PDT");
    _tzset();
    __int64 t = 1100000000;
    const char* c = MashedCrt_ctime64_004a43b9(&t);
    Check("ctime64 = DDK text", c && !std::strcmp(c, "Tue Nov 09 03:33:20 2004\n"), c);
    t = 1120000000;
    c = MashedCrt_ctime64_004a43b9(&t);
    Check("ctime64 two-digit day untouched", c && !std::strcmp(c, "Tue Jun 28 16:06:40 2005\n"), c);
    t = -1;
    Check("ctime64 negative -> NULL", MashedCrt_ctime64_004a43b9(&t) == nullptr, nullptr);

    std::printf("SELFTEST %s: %d/%d passed\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
