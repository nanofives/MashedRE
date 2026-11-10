// PORT-OT-IMAGE-FMT (MASHED_ORIG_TOP path, knob MASHED_ORIG_TOP_RWMOD): the image format registration 0x00493600.
// EXE ONLY (exe_sources.rsp, not asi_sources.rsp).
// RVA-LINT: ORIGTOP-ONLY
//
// Compiles re/match/s15/00493600.c (MATCH-S15 h8, 100%, C4 match) as Orig_00493600. It calls 0x004ce790 (Orig_004ce790,
// Match/PortTop_004ce790.cpp) with ("bmp", 0x004935a0, 0x0052d530) and ("png", 0x004935d0, 0x00513f90): the bytes at
// rva 0x005cf74c are 62 6d 70 00 and at 0x005cf748 70 6e 67 00 (original/MASHED.exe.unpatched), so &PTR_DAT_005cf74c /
// &PTR_DAT_005cf748 are the extension strings. The four functions bind to the hand ports of 0x004935a0 / 0x004935d0
// (Match/PortTop_004ce790.cpp) and the librw writers (LibRw/RwApiBind.cpp). Their prototypes in the matched source are
// void(void); the definitions take the original's arguments (C linkage, cdecl, pointers only stored here).
//
// When it runs (verify/portorigtop_20261109/PREREG_PORT-OT-IMAGE-FMT.md): 0x00493710 calls 0x00493600 right after
// RwEngineStart, before the exe installs the RwGlobals record and registers its modules (after piece 4). In the original
// the RW engine has opened its modules by then. So under the knob the call site records the call and returns 1, and
// RegisterRwModules (LibRw/RwApiBind.cpp) runs Orig_00493600 once the image module (+0x218 freelist, +0x21c list) is set up.

#ifdef MASHED_STANDALONE

#include <cstdarg>
#include <cstdio>

#define MATCH_PORT 1

extern "C" {

int MashedRw_RwModKnob(void);   // LibRw/RwApiBind.cpp: 1 = MASHED_ORIG_TOP_RWMOD=1

static void PortTop9360_Log(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

} // extern "C"

// ---- bindings of rva 0x00493600 ----
#define FUN_00493600 Orig_00493600
#define FUN_004ce790 Orig_004ce790                         // matched, Match/PortTop_004ce790.cpp
#define FUN_004935a0 PortTop_ImgRead_004935a0              // hand port, Match/PortTop_004ce790.cpp
#define FUN_004935d0 PortTop_ImgRead_004935d0              // hand port, Match/PortTop_004ce790.cpp
#define FUN_0052d530 MashedRw_RtBMPImageWrite_0052d530     // RW toolkit BMP writer -> rw::writeBMP
#define FUN_00513f90 MashedRw_RtPNGImageWrite_00513f90     // RW toolkit PNG writer -> rw::writePNG

extern "C" {
#include "../../../../re/match/s15/00493600.c"

static bool s_pending00493600 = false;

// The call site in rva 0x00493710 (re/match/s15/00493710.c). Knob OFF: the old missing stub. Knob ON: deferred to RegisterRwModules.
// Both return 1, the value the original returns when both registrations succeed.
int PortTop_Bound_00493600(void) {
    if (!MashedRw_RwModKnob()) {
        PortTop9360_Log("0x00493600: missing (MASHED_ORIG_TOP_RWMOD off: no image module), returns 1");
        return 1;
    }
    s_pending00493600 = true;
    PortTop9360_Log("0x00493600: reached before the RwGlobals modules exist; runs from RegisterRwModules, returns 1 here");
    return 1;
}

// Called by RegisterRwModules after the image module is set up. Runs the matched body once if 0x00493710 reached it.
int PortTop_RunPending_00493600(void) {
    if (!s_pending00493600) return -1;
    s_pending00493600 = false;
    const int r = Orig_00493600();
    PortTop9360_Log("0x00493600 (matched, deferred to RegisterRwModules): bmp / png registered, returned %d", r);
    return r;
}
} // extern "C"

#endif // MASHED_STANDALONE
