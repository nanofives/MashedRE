// BIND-CRT-498510 self-test: compiles the knob-path binding of rva 0x00498510 (Match/PortTop_00498510.cpp, the provisional
// body from re/match/s16/00498510.c with its file-static callees 0x00497190 / 0x004971b0), its matched callees
// Match/PortTop_00495790.cpp / PortTop_00495830.cpp, the log cluster (PortTop_00496490 / 004963e0 / 00496400 / 00496470)
// and Match/PortTop_CrtStdio.cpp unchanged (MASHED_STANDALONE) into a small exe based at 0x00400000 with a zero-filled
// array over the original data range (as the exe does), so every global sits at its original address. Checks the
// record defaults for 0 and 2 joypads, the contcfg%d.bin keep / restore rule, the language pointer table, show = 1.
// Build and run: verify/portorigtop_20261113/run_contcfg_selftest.bat (cwd = a scratch directory it creates).
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

// Harness stand-ins for exe TUs this test does not link (each is a one-line getter / helper there).
extern "C" void PortTop_Fs_DiskPath(const char* name, char* path, unsigned size) {   // Match/PortTop_RwStream04F.cpp
    const char* p = name;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(path, size, "original/%s", p);
    for (char* c = path; *c; ++c) if (*c == '\\') *c = '/';
}
extern "C" int PortTop_Crt_004a42c5(char* out, const char* fmt, char* ap) {             // Match/PortTop_InputPort.cpp
    return std::vsprintf(out, fmt, reinterpret_cast<va_list>(ap));
}
extern "C" void* Global7e9584Get(void) { return *reinterpret_cast<void**>(0x007e9584u); }  // re/match/s16/00499710.c
extern "C" void* GetInputHinst(void) { return *reinterpret_cast<void**>(0x007e9580u); }    // re/match/s16/00499720.c
extern "C" void __cdecl ZeroFillWrapper(void* p, unsigned int n) { std::memset(p, 0, n); }   // Util/TimerSlot.cpp
extern "C" void Orig_00498510(int show, int lang);

struct Rec { int f0; char name[0x104]; unsigned bind[13]; int type; int joy; unsigned char rest[0x200 - 0x144]; };
static_assert(sizeof(Rec) == 0x200, "record size");
static Rec* R() { return reinterpret_cast<Rec*>(0x007e95c0u); }

static int g_n = 0, g_fail = 0;
static void Check(const char* name, bool ok, const std::string& detail = std::string()) {
    ++g_n;
    if (!ok) ++g_fail;
    std::printf("[%d] %s %s%s%s\n", g_n, name, ok ? "PASS" : "FAIL", detail.empty() ? "" : "  ", detail.c_str());
}
static bool Binds(const Rec& r, const unsigned* want, int n) {
    for (int i = 0; i < n; ++i) if (r.bind[i] != want[i]) return false;
    for (int i = n; i < 13; ++i) if (r.bind[i] != 0) return false;
    return true;
}
static void Put(const char* path, const void* p, unsigned n) {
    std::FILE* f = std::fopen(path, "wb");
    std::fwrite(p, 1, n, f);
    std::fclose(f);
}
static std::string Get(const char* path) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return "<none>";
    std::string s;
    char b[512];
    std::size_t n;
    while ((n = std::fread(b, 1, sizeof(b), f)) > 0) s.append(b, n);
    std::fclose(f);
    return s;
}
static char g_pad[0x00500000];
static const unsigned kJoy[9] = { 0, 1, 2, 3, 0, 1, 4, 2, 5 };
static const unsigned kKbd[13] = { 0x1f, 0x2d, 0x1e, 0x20, 0x1c, 1, 1, 0x1e, 0x2e, 0xcb, 0xcd, 0xc8, 0xd0 };

int main() {
    const unsigned lo = reinterpret_cast<unsigned>(g_pad), hi = lo + sizeof(g_pad);
    if (lo > 0x005cf000u || hi < 0x007ea000u) { std::printf("pad 0x%08x..0x%08x misses the range\n", lo, hi); return 2; }
    struct { unsigned va; const char* s; } seed[] = {   // Match/PortTop_RdataSeed.inc rows of 00498510.c
        { 0x005cf010u, "rb" }, { 0x005d000cu, "contcfg%d.bin" },
        { 0x005d001cu, "\tUnable to load controller config %s\n" }, { 0x005d0044u, "\tLoaded controller config %s\n" },
        { 0x005d0064u, "Attempting to load controller config %s\n" } };
    for (auto& s : seed) std::memcpy(reinterpret_cast<char*>(s.va), s.s, std::strlen(s.s) + 1);
    _mkdir("original");
    _putenv("MASHED_ORIGTOP_WRITE_DIR=");
    *reinterpret_cast<HINSTANCE*>(0x007e9580u) = GetModuleHandleA(nullptr);   // the exe module, as on the knob path
    std::strcpy(reinterpret_cast<char*>(0x007730b4u), "KBSENTINEL");
    std::strcpy(reinterpret_cast<char*>(0x007730f8u), "NONESENTINEL");
    *reinterpret_cast<int*>(0x007719e8u) = 0;                                 // language index

    // 1. no joypad, no contcfg file, lang 3
    *reinterpret_cast<int*>(0x00772facu) = 0;
    std::memset(R(), 0xcc, 0x800);
    Orig_00498510(0, 3);
    const unsigned* tbl = reinterpret_cast<const unsigned*>(0x00773098u);
    Check("pointer table 0x00773098 = 0x006147e8 + k*0x400", tbl[0] == 0x006147e8u && tbl[1] == 0x00614be8u &&
          tbl[2] == 0x00614fe8u && tbl[3] == 0x006153e8u && tbl[4] == 0x006157e8u && tbl[5] == 0x00615be8u);
    Check("0x00773118 = table[lang 3]", *reinterpret_cast<unsigned*>(0x00773118u) == 0x006153e8u);
    const std::string kb = reinterpret_cast<const char*>(0x007730b4u);
    const std::string no = reinterpret_cast<const char*>(0x007730f8u);
    std::printf("    LoadStringA(exe module, 0x82a) -> 0x007730b4 \"%s\", (0x834) -> 0x007730f8 \"%s\"\n", kb.c_str(), no.c_str());
    Check("rec0 keyboard: f0 6, type 2, name = 0x007730b4, 13 binds", R()[0].f0 == 6 && R()[0].type == 2 &&
          kb == R()[0].name && Binds(R()[0], kKbd, 13));
    Check("rec1..3 none: f0 6, type 0, rest zero", R()[1].f0 == 6 && R()[1].type == 0 && R()[2].type == 0 &&
          R()[3].type == 0 && R()[3].f0 == 6 && Binds(R()[3], kKbd, 0) && R()[3].name[0] == 0);
    Check("contcfg name buffer 0x007730e4 = contcfg3.bin", std::strcmp(reinterpret_cast<char*>(0x007730e4u), "contcfg3.bin") == 0);

    // 2. two joypads (names at 0x00771eb4 + i*0x448), contcfg files in the overlay
    *reinterpret_cast<int*>(0x00772facu) = 2;
    std::strcpy(reinterpret_cast<char*>(0x00771eb4u), "Pad A");
    std::strcpy(reinterpret_cast<char*>(0x00771eb4u + 0x448u), "Pad B");
    _mkdir("origtop_rw");
    Rec keep{}; keep.f0 = 6; std::strcpy(keep.name, "Pad B"); keep.type = 1; keep.joy = 1; keep.bind[0] = 0x77;
    Put("origtop_rw/contcfg1.bin", &keep, sizeof(keep));            // same f0 and name: the loaded record stays
    Rec bad{}; bad.f0 = 7; std::strcpy(bad.name, kb.c_str()); bad.bind[0] = 0x55;
    Put("origtop_rw/contcfg2.bin", &bad, sizeof(bad));              // f0 differs: the defaults come back
    Rec ren{}; ren.f0 = 6; std::strcpy(ren.name, "Other"); ren.bind[0] = 0x66;
    Put("original/contcfg3.bin", &ren, sizeof(ren));                // name differs (read from original/): defaults
    std::memset(R(), 0xcc, 0x800);
    Orig_00498510(0, 0);
    Check("rec0 joypad 0: type 1, joy 0, name Pad A, 9 binds", R()[0].type == 1 && R()[0].joy == 0 &&
          std::strcmp(R()[0].name, "Pad A") == 0 && Binds(R()[0], kJoy, 9));
    Check("rec1 loaded from origtop_rw/contcfg1.bin and kept", R()[1].bind[0] == 0x77 && R()[1].joy == 1 &&
          std::strcmp(R()[1].name, "Pad B") == 0);
    Check("rec2 keyboard: loaded f0 7 != 6, defaults restored", R()[2].type == 2 && R()[2].f0 == 6 &&
          Binds(R()[2], kKbd, 13));
    Check("rec3 none: loaded name differs, defaults restored", R()[3].type == 0 && R()[3].f0 == 6 && R()[3].bind[0] == 0);
    Check("0x00773118 = table[0]", *reinterpret_cast<unsigned*>(0x00773118u) == 0x006147e8u);
    Check("original/contcfg*.bin untouched, no write", Get("original/contcfg3.bin").size() == 0x200 &&
          Get("origtop_rw/contcfg0.bin") == "<none>");

    // 3. show = 1: dialog template 0x67 is absent from this module; the call returns and the proc stand-in is not run.
    std::remove("mashed_re.log");
    Orig_00498510(1, 0);
    Check("show 1 returns; 0x00498360 stand-in not reached", Get("mashed_re.log").find("0x00498360") == std::string::npos);

    std::printf("%d/%d PASS\n", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
