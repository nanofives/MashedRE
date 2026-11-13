// RVA-LINT: ORIGTOP-ONLY
// BIND-CRT-498510 (roadmap run 43, top-down row): rva 0x00498510 controller defaults + contcfg%d.bin load, compiled from
// re/match/s16/00498510.c for the MASHED_ORIG_TOP knob path. EXE ONLY.
// L-126 status: PROVISIONAL body (re/match/PROVISIONAL.tsv). match.py best 95.3% (11 differing bytes, one instruction
// order in the record restore copy), equiv.py NOT-PROVEN ("esp-unknown": the /GS frame aligns esp with and esp,-8).
// Knob path only, never C4 by this status (L-126, L-128). The object build keeps this VS2022 body (not 100%).
// The source file also holds its two file-static callees, compiled here as part of the same text:
//   rva 0x00497190 contcfg%d.bin name (100%), rva 0x004971b0 contcfg load (100%, called with the index on the stack
//   here, as a static of this TU). Their asi bodies are Match/MatchS16_004971b0.cpp (ORIGTOP-ONLY CROSS-TARGET pair).
// Globals are the original addresses (preamble MGLOBAL under MATCH_PORT): records 0x007e95c0 (4 x 0x200), language
// pointer table 0x00773098 / 0x00773118, strings 0x007730b4 / 0x007730f8, name buffer 0x007730e4, language 0x007719e8.
// The .rdata strings it reads (0x005d000c "contcfg%d.bin", 0x005cf010 mode, the three log formats) come from the knob
// path's .rdata seed (Match/PortTop_RdataSeed.inc, verify/porttop_20261108/gen_rdata_seed.py).
// Callees:
//   0x004a2b60 sprintf   -> PortTop_LogSprintf (Match/PortTop_00496490.cpp, exe vsprintf)
//   0x00496400 cfg log   -> Orig_00496400 (matched, Match/PortTop_00496400.cpp)
//   0x004a4541 fopen / 0x004a49cf fread / 0x004a4368 fclose -> MashedCrt_* (Match/PortTop_CrtStdio.cpp): reads the
//                           overlay copy when one exists, else original/<name>
//   0x00499710 / 0x00499720 -> Global7e9584Get / GetInputHinst (matched, exe)
//   0x00495790 / 0x00495830 -> Orig_00495790 / Orig_00495830 (matched, Match/PortTop_00495790.cpp / PortTop_00495830.cpp)
//   0x004b6520 zero fill  -> ZeroFillWrapper (Util/TimerSlot.cpp)
//   LoadStringA / DialogBoxParamA -> user32. The exe carries no string table, so LoadStringA on the exe HINSTANCE
//                           (0x007e9580) loads no string and 0x007730b4 / 0x007730f8 stay as they were (zero). The
//                           original loads its own resource strings there.
//   0x00498360 dialog proc -> PortTop_Missing_00498360 (no source). Reached only with show != 0. The caller 0x00495150
//                           passes 0x006147c0, and the reference carries skip_controller_dialog (0x004951aa, show = 0).
// Replaces the knob path's PortTop_Missing_00498510 stub (Match/PortTop_InputPort.cpp).
#ifdef MASHED_STANDALONE
#include <cstdio>

extern "C" {
std::FILE* __cdecl MashedCrt_fopen_004a4541(const char* name, const char* mode);
unsigned int __cdecl MashedCrt_fread_004a49cf(void* buf, unsigned int size, unsigned int n, std::FILE* f);
int __cdecl MashedCrt_fclose_004a4368(std::FILE* f);
}

#define MATCH_PORT 1
#define FUN_00498510 Orig_00498510
#define FUN_00497190 PortTop_498510_00497190
#define FUN_004971b0 PortTop_498510_004971b0
#define FUN_004a2b60 PortTop_LogSprintf
#define FUN_00496400 Orig_00496400
#define FUN_004a4541 PortTop_ContCfg_Fopen
#define FUN_004a49cf PortTop_ContCfg_Fread
#define FUN_004a4368 PortTop_ContCfg_Fclose
#define FUN_00499710 Global7e9584Get
#define FUN_00499720 GetInputHinst
#define FUN_00495790 Orig_00495790
#define FUN_00495830 Orig_00495830
#define FUN_004b6520 ZeroFillWrapper
#define FUN_00498360 PortTop_Missing_00498360
extern "C" {
#include "../../../../re/match/s16/00498510.c"

// The source only passes the FILE pointer back to these three (it never reads a FILE field), so the exe CRT's FILE*
// travels as the source's FILE_h4*.
FILE_h4* PortTop_ContCfg_Fopen(const char* name, const char* mode) {
    return reinterpret_cast<FILE_h4*>(MashedCrt_fopen_004a4541(name, mode));
}
unsigned int PortTop_ContCfg_Fread(void* buf, unsigned int size, unsigned int n, FILE_h4* f) {
    return MashedCrt_fread_004a49cf(buf, size, n, reinterpret_cast<std::FILE*>(f));
}
int PortTop_ContCfg_Fclose(FILE_h4* f) { return MashedCrt_fclose_004a4368(reinterpret_cast<std::FILE*>(f)); }

// rva 0x00498360: the controller dialog's proc (no matched source). Missing: logs and returns 0 (FALSE). With the exe's
// HINSTANCE the template 0x67 does not exist, so DialogBoxParamA returns before any message reaches it.
int __stdcall PortTop_Missing_00498360(HWND h, unsigned int m, unsigned int w, long l) {
    (void)h; (void)w; (void)l;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] 0x00498360 controller dialog proc: missing (msg 0x%x)\n", m);
        std::fclose(f);
    }
    return 0;
}
}  // extern "C"
#endif  // MASHED_STANDALONE
