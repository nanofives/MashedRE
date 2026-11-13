// BIND-CRT-STDIO-CALLERS self-test: compiles the four knob-path log wrappers (Match/PortTop_00496490.cpp,
// PortTop_004963e0.cpp, PortTop_00496400.cpp, PortTop_00496470.cpp) and Match/PortTop_CrtStdio.cpp unchanged
// (MASHED_STANDALONE) into a small exe based at 0x00400000 with a zero-filled array over 0x005d0000..0x00773000 (as the exe does), so
// the matched bodies' globals (0x005d0004 "wt", 0x00616038, 0x00772fbc) sit at their original addresses, and checks:
// log open / header / error line / debug line / close against the write overlay, the post-close no-ops, the mashed_re.log
// mirror, original/ untouched, and the MASHED_ORIGTOP_WRITE_DIR=off arm (= the reference's disable_log values).
// Build and run: verify/portorigtop_20261112/run_logcluster_selftest.bat (cwd = a scratch directory it creates).
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

extern "C" void PortTop_Fs_DiskPath(const char* name, char* path, unsigned size) {
    const char* p = name;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(path, size, "original/%s", p);
    for (char* c = path; *c; ++c) if (*c == '\\') *c = '/';
}
// Match/PortTop_InputPort.cpp's vsprintf binding (that TU pulls in DirectInput).
extern "C" int PortTop_Crt_004a42c5(char* out, const char* fmt, char* ap) {
    return std::vsprintf(out, fmt, reinterpret_cast<va_list>(ap));
}
extern "C" {
int Orig_00496490(void);
void Orig_00496470(void);
void Orig_004963e0(char* msg);
void Orig_00496400(const char* fmt, ...);
}

static int g_n = 0, g_fail = 0;
static void Check(const char* name, bool ok, const std::string& detail = std::string()) {
    ++g_n;
    if (!ok) ++g_fail;
    std::printf("[%d] %s %s%s%s\n", g_n, name, ok ? "PASS" : "FAIL", detail.empty() ? "" : "  ", detail.c_str());
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
// Same arrangement as the exe (exe_main.cpp g_b17_low_arena_pad): the image is based at 0x00400000 and this zero-filled
// array spans the original data addresses the matched bodies use.
static char g_pad[0x00500000];
static bool Exists(const char* p) { return GetFileAttributesA(p) != INVALID_FILE_ATTRIBUTES; }
static void*& Handle() { return *reinterpret_cast<void**>(0x00772fbcu); }

int main() {
    const unsigned lo = reinterpret_cast<unsigned>(g_pad), hi = lo + sizeof(g_pad);
    if (lo > 0x005d0000u || hi < 0x00773000u) { std::printf("pad 0x%08x..0x%08x misses the range\n", lo, hi); return 2; }
    std::memcpy(reinterpret_cast<char*>(0x005d0004u), "wt", 3);           // Match/PortTop_RdataSeed.inc row
    *reinterpret_cast<unsigned*>(0x00616038u) = 0xbb40e64eu;
    _mkdir("original");
    _putenv("MASHED_ORIGTOP_WRITE_DIR=");

    // 1. open
    Handle() = nullptr;
    const int r = Orig_00496490();
    Check("0x00496490 returns 1", r == 1, std::to_string(r));
    Check("0x00772fbc set", Handle() != nullptr);
    // 2. error and debug lines
    char err[] = "err line\n";
    Orig_004963e0(err);
    Orig_00496400("dbg %d %s %.2f\n", 42, "xy", 1.5);
    // 3. close
    Orig_00496470();
    Check("0x00496470 clears 0x00772fbc", Handle() == nullptr);
    const std::string log = Get("origtop_rw/mashed.log");
    // header: "%.19s\n\n" of the ctime text, DDK day padding ("Thu Nov 09 ..."), so 19 chars then two newlines, CRLF in text mode ("wt")
    bool hdr = log.size() >= 23 && log.compare(19, 4, "\r\n\r\n") == 0 && log[3] == ' ' && log[7] == ' ' &&
               log[10] == ' ' && log[13] == ':' && log[16] == ':' && log[8] != ' ';
    Check("header = 19-char ctime + 2 text-mode newlines (CRLF), day zero-padded", hdr, log.substr(0, 19));
    Check("body = error line then debug line", log.size() >= 23 && log.substr(23) == "err line\r\ndbg 42 xy 1.50\r\n",
          log.size() >= 23 ? log.substr(23) : log);
    Check("original/mashed.log not created", !Exists("original/mashed.log"));
    // 4. after close: both writers are no-ops
    char late[] = "late\n";
    Orig_004963e0(late);
    Orig_00496400("late %d\n", 1);
    Check("writers after close leave the file unchanged", Get("origtop_rw/mashed.log") == log);
    Orig_00496470();
    Check("second close is a no-op", Handle() == nullptr);
    // 5. mirror
    const std::string mir = Get("mashed_re.log");
    Check("mirror holds the error and debug lines", mir.find("cfglog: err line") != std::string::npos &&
                                                     mir.find("cfglog: dbg 42 xy 1.50") != std::string::npos);
    Check("mirror holds nothing after close", mir.find("late") == std::string::npos);
    // 6. reopen truncates ("wt")
    Check("reopen returns 1", Orig_00496490() == 1);
    Orig_00496470();
    Check("reopen truncates to the header", Get("origtop_rw/mashed.log").size() == 23);
    // 7. MASHED_ORIGTOP_WRITE_DIR=off = the reference's disable_log values
    _putenv("MASHED_ORIGTOP_WRITE_DIR=off");
    Check("off: 0x00496490 returns 0", Orig_00496490() == 0);
    Check("off: 0x00772fbc stays 0", Handle() == nullptr);
    Orig_004963e0(err);
    Orig_00496400("x\n");
    Orig_00496470();
    Check("off: nothing written", Get("origtop_rw/mashed.log").size() == 23 && !Exists("original/mashed.log"));

    std::printf("%d/%d PASS\n", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
