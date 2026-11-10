// PORT-TOP-04G (L-96 top-down lane): exe-only port body of rva 0x0040bbb0 (sfx.piz: four texture dictionaries into
// DAT_0063b8f8 / fc / 900 / 904, then the effect subsystems' inits) = the byte-matched source re/match/s02/0040bbb0.c
// (match.py 100.0%, regs 100.0%). Reached only on the MASHED_ORIG_TOP path, from the matched 0x00402750
// (Match/PortTop_00402750.cpp). Every callee binds by name, no raw-address call:
//  - piz mount / unmount 0x00495280 / 0x004952f0: the piz file system of Match/PortTop_00402750_Pieces.cpp.
//  - 0x0042a6b0 TXD search + load (bound by its own name): the exe body Match/MatchS06_0042a6b0.cpp (its exe callees bound since PORT-TOP-04G).
//  - 0x0048bbe0 (image path "", RwTextureRead "sun"): the matched exe body Match/PortTop_0048bbe0.cpp.
//  - 0x0040bb30: the matched body Match/MatchS02_0040bb30.cpp.
//  - RW: RwTextureRead 0x004c5cb0, RwTexDictionaryAddTexture 0x004c5bc0, RwTexDictionarySetCurrent 0x004c5c80
//    (LibRw/RwApiBind.cpp).
//  - printf (FID_conflict__wprintf) and exit 0x004a332b: the exe CRT (the "cant open sfx dic" branch only).
//  - stand-ins (below): the effect inits and the DFF / clump loaders not moved yet. Each logs its first call and does
//    nothing, so the original globals they would write stay 0 (the value they had while 0x0040bbb0 was a stand-in).
// Evidence: verify/porttop_20261108/RESULT_PORT-TOP-04G.md.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace {
void Log04G(const char* fmt, ...) {
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
void StandInLog04G(const char* rva, const char* job, int* once) {
    if (*once) return;
    *once = 1;
    Log04G("rva %s: stand-in, not run (%s) (PORT-TOP-04G)", rva, job);
}
}  // namespace

extern "C" {
// printf: the original writes to the console the game does not have; the binding writes the text to the log.
int PortTop_Printf04G(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    Log04G("0x0040bbb0 printf: %s", buf);
    return n;
}
// rva 0x004a332b (exit, CRT): the exe CRT's exit with the same code.
__declspec(noreturn) void PortTop_Exit04G_004a332b(int code) {
    Log04G("0x0040bbb0 exit(%d) (rva 0x004a332b -> CRT exit)", code);
    std::exit(code);
}

#define SI(rva, sig, args, job) void PortTop_StandIn04G_##rva sig { static int once; (void)args; StandInLog04G("0x" #rva, job, &once); }
SI(004759b0, (int p0), p0, "0x00691600 array alloc (RwMallocArrayZeroFill 0x00474db0, .asi-only body)")
SI(00475a00, (int p0, int p1), (p0, p1), "no matched source (C2)")
SI(00476390, (void), 0, "effect init (0x00475ef0, emitter create)")
SI(004775b0, (void), 0, "atomic / clump builder (RpAtomicCreate 0x004e67b0 tree, 0x004b3f90 no source)")
SI(00477e40, (void), 0, "effect init (0x00477b40)")
SI(00485d90, (void), 0, "effect init (0x004892c0, 0x004893a0)")
SI(004862d0, (void), 0, "effect init (emitter create)")
SI(00487e00, (int p0, int p1), (p0, p1), "object-shadow init (0x00487150)")
SI(00489290, (void), 0, "effect init (.asi-only body)")
SI(0048a460, (void), 0, "strided clear 0x00709238 (.asi-only body)")
SI(0048a830, (int p0), p0, "zero fill (.asi-only body, BitArrayClear)")
SI(0048ae00, (void), 0, "clump builder (RpClumpDestroy, 0x00544a70.. no source)")
SI(0048e820, (int p0), p0, "no matched source (C2)")
SI(0048eac0, (void), 0, "effect init (emitter create)")
SI(0048ff20, (void), 0, "file-backed table loader (0x004b65b0, 0x004b3e40 stream reads)")
#undef SI
// rva 0x004b3d20 (TXD read, chunk 0x16 + 0x004cf7d0): called by 0x0042a6b0 for a name found under a platform folder
// (search kinds 2..5). 0x004cf7d0 has no matched source (C2). The sfx.piz names are found at the root (kind 1, 0x004b3d80).
int PortTop_StandIn04G_004b3d20(char* p0) {
    static int once;
    if (!once) Log04G("rva 0x004b3d20(\"%s\"): stand-in, not run (chunk-0x16 reader 0x004cf7d0 has no matched source), "
                      "returns 0 (PORT-TOP-04G)", p0 ? p0 : "");
    once = 1;
    return 0;
}
}  // extern "C"

#define MATCH_PORT 1
#define FUN_0040bbb0 Orig_0040bbb0
#define FID_conflict__wprintf PortTop_Printf04G
#define FUN_004a332b PortTop_Exit04G_004a332b
#define FUN_00495280 PortTop_PizFs_Mount_00495280
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0
#define FUN_0048bbe0 Orig_0048bbe0                         // matched, Match/PortTop_0048bbe0.cpp
#define FUN_0040bb30 Search40bb30                          // matched, Match/MatchS02_0040bb30.cpp
#define FUN_004c5cb0 MashedRw_TextureRead_004c5cb0         // RW: RwTextureRead
#define FUN_004c5bc0 MashedRw_TexDictionaryAddTexture_004c5bc0  // RW: RwTexDictionaryAddTexture
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80  // RW: RwTexDictionarySetCurrent
#define FUN_00401000 Orig_00401000                     // matched, Match/PortTop_00401000.cpp, since PORT-TOP-04J
#define FUN_00413b80 Orig_00413b80                       // matched, Match/PortTop_00413b80.cpp, since PORT-OT-HUD
#define FUN_004210b0 Orig_004210b0                       // matched, Match/PortTop_004210b0.cpp, since PORT-OT-HUD-2
#define FUN_004759b0 PortTop_StandIn04G_004759b0
#define FUN_00475a00 PortTop_StandIn04G_00475a00
#define FUN_00476390 PortTop_StandIn04G_00476390
#define FUN_004775b0 PortTop_StandIn04G_004775b0
#define FUN_00477e40 PortTop_StandIn04G_00477e40
#define FUN_00485d90 PortTop_StandIn04G_00485d90
#define FUN_004862d0 PortTop_StandIn04G_004862d0
#define FUN_00487e00 PortTop_StandIn04G_00487e00
#define FUN_00489290 PortTop_StandIn04G_00489290
#define FUN_0048a460 PortTop_StandIn04G_0048a460
#define FUN_0048a830 PortTop_StandIn04G_0048a830
#define FUN_0048ae00 PortTop_StandIn04G_0048ae00
#define FUN_0048e820 PortTop_StandIn04G_0048e820
#define FUN_0048eac0 PortTop_StandIn04G_0048eac0
#define FUN_0048ff20 PortTop_StandIn04G_0048ff20
// Rebound off the stand-ins above (their SI definitions stay, unreferenced). Kept in one block below the list so a row
// that edits a neighbouring line of the list does not touch the same lines.
#undef FUN_004759b0
#define FUN_004759b0 Orig_004759b0                       // matched, Match/PortTop_004759b0.cpp, since PORT-OT-RENDER
#undef FUN_00476390
#define FUN_00476390 Orig_00476390                       // matched, Match/PortTop_00476390.cpp, since PORT-OT-RENDER
#undef FUN_00477e40
#define FUN_00477e40 Orig_00477e40                       // matched, Match/PortTop_00477e40.cpp, since PORT-OT-RENDER
#undef FUN_00489290
#define FUN_00489290 Orig_00489290                       // matched, Match/PortTop_00489290.cpp, since PORT-OT-OTHER
#undef FUN_00487e00
#define FUN_00487e00 Orig_00487e00                       // matched, Match/PortTop_00487e00.cpp, since PORT-OT-OTHER
#undef FUN_00485d90
#define FUN_00485d90 Orig_00485d90                       // matched, Match/PortTop_00485d90.cpp, since PORT-OT-OTHER
#undef FUN_004862d0
#define FUN_004862d0 Orig_004862d0                       // matched, Match/PortTop_004862d0.cpp, since PORT-OT-OTHER
#undef FUN_0048eac0
#define FUN_0048eac0 Orig_0048eac0                       // matched, Match/PortTop_0048eac0.cpp, since PORT-OT-OTHER
#undef FUN_0048a460
#define FUN_0048a460 Orig_0048a460                       // matched, Match/PortTop_0048a460.cpp, since PORT-OT-OTHER
extern "C" {
#include "../../../../re/match/s02/0040bbb0.c"
}
#endif  // MASHED_STANDALONE
