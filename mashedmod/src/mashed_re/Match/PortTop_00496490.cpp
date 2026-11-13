// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-STDIO-CALLERS (roadmap run 41, top-down row): the original's log-file cluster compiled from its byte-matched
// sources for the MASHED_ORIG_TOP knob path, bound to the BIND-CRT-STDIO CRT bodies (Match/PortTop_CrtStdio.cpp).
// EXE ONLY.
//   rva 0x00496490 log open (WM_CREATE)   re/match/s16/00496490.c (100%)  fopen("mashed.log", "wt" 0x005d0004) into
//                                          0x00772fbc, then the "%.19s\n\n" date header through 0x004963e0
//   rva 0x00496470 log close (WM_DESTROY) re/match/s16/00496470.c (100%)  fclose + 0x00772fbc = 0
//   rva 0x004963e0 ConfigLogError         re/match/s16/004963e0.c (100%)  fputs if 0x00772fbc != 0
//   rva 0x00496400 ConfigLogDebug         re/match/s16/00496400.c (100%)  vsprintf 0x004a42c5 + fputs if 0x00772fbc != 0
// Globals are the original addresses (the preamble's MGLOBAL under MATCH_PORT): 0x00772fbc, 0x00616038 (the /GS cookie
// the matched 0x00496490 reads; VS2022 compiles the read, not a cookie check). 0x005d0004 "wt" comes from the knob
// path's .rdata seed (Match/PortTop_RdataSeed.inc, gen_rdata_seed.py).
// Callees:
//   0x004a4541 fopen   -> MashedCrt_fopen_004a4541: "wt" opens <MASHED_ORIGTOP_WRITE_DIR, default origtop_rw>/mashed.log,
//                         never original/. MASHED_ORIGTOP_WRITE_DIR=off returns NULL, and the cluster behaves as the
//                         reference's disable_log patch (0x00496490 returns 0, the other three write nothing).
//   fputs 0x004a4254   -> MashedCrt_fputs_004a4254, plus a mirror of the text into mashed_re.log (port diagnostic, the
//                         "cfglog:" lines the stand-ins wrote). The mirror runs only where the body calls fputs.
//   fclose 0x004a4368  -> MashedCrt_fclose_004a4368
//   0x004a43d2 _ftime64 / 0x004a43b9 _ctime64 / 0x004a2b60 sprintf / 0x004a42c5 vsprintf -> the exe CRT (adapters below;
//                         _ctime64 through MashedCrt_ctime64_004a43b9 for the DDK day padding).
// Reference note: the reference MASHED.exe carries the disable_log patch (0x004963e7 JMP, 0x00496400 RET, 0x00496490),
// so its 0x00772fbc stays 0. Only these four functions read or write 0x00772fbc (re/match grep; hooks.csv rows
// 004963e0 / 00496470 / 00496490), and nothing else the game computes depends on it. Replaces the knob path's
// PortTop_Missing_00496490 / PortTop_Missing_00496470 and the PortTop_CfgLogError / PortTop_CfgLogDebug stand-ins.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <sys/timeb.h>

extern "C" {
std::FILE* __cdecl MashedCrt_fopen_004a4541(const char* name, const char* mode);
int __cdecl MashedCrt_fputs_004a4254(const char* str, std::FILE* f);
int __cdecl MashedCrt_fclose_004a4368(std::FILE* f);
char* __cdecl MashedCrt_ctime64_004a43b9(const __int64* t);
int PortTop_Crt_004a42c5(char* out, const char* fmt, char* ap);   // Match/PortTop_InputPort.cpp (vsprintf)

static void PortTop_LogMirror(const char* s) {
    if (!s) return;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] cfglog: %.*s\n", static_cast<int>(std::strcspn(s, "\n")), s);
        std::fclose(f);
    }
}
int PortTop_LogFputs(const char* s, std::FILE* f) {
    PortTop_LogMirror(s);
    return MashedCrt_fputs_004a4254(s, f);
}
int PortTop_LogFclose(std::FILE* f) { return MashedCrt_fclose_004a4368(f); }
void* PortTop_LogFopen(const char* name, const char* mode) { return MashedCrt_fopen_004a4541(name, mode); }
// _ftime64 0x004a43d2: the source passes its 16-byte local by address = the __timeb64 layout.
void PortTop_LogFtime64(void* tb) {
    static_assert(sizeof(__timeb64) == 16, "__timeb64 is not 16 bytes");
    _ftime64(reinterpret_cast<__timeb64*>(tb));
}
// _ctime64 0x004a43b9: the source passes the struct address; its first 8 bytes are the time.
char* PortTop_LogCtime64(void* p) { return MashedCrt_ctime64_004a43b9(static_cast<const __int64*>(p)); }
// sprintf 0x004a2b60.
int PortTop_LogSprintf(char* out, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(out, fmt, ap);
    va_end(ap);
    return n;
}

#define MATCH_PORT 1
#define FUN_004a4541 PortTop_LogFopen
#define FUN_004a43d2 PortTop_LogFtime64
#define FUN_004a43b9 PortTop_LogCtime64
#define FUN_004a2b60 PortTop_LogSprintf
#define FUN_004963e0 Orig_004963e0                // Match/PortTop_004963e0.cpp
#define FUN_00496490 Orig_00496490
#include "../../../../re/match/s16/00496490.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
