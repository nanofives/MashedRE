// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x00498860 (and its static callee 0x00498810) = the
// matched source re/match/s16/00498810.c (0x00498860 100%, 0x00498810 86.7%, static, compiled with its caller). 0x00492d30
// calls it with 0x004671a0(0) when 0x00496920(5) != 0 and DAT_007719cc == 0. Reached only on the MASHED_ORIG_TOP path.
// Callees bind by name:
//   0x004cdca0 / 0x004cdd00 / 0x004cdd60  RwCore_* matched RW core exe bodies (RwCore/)
//   0x004d52d0  MashedRw_ImageSetFromRaster_004d52d0   existing RW binding (LibRw/RwApiBind.cpp), no new binding
//   0x004ce8e0  PortTop_StandIn05_004ce8e0  stand-in: matched (re/match/s20/004ce8e0.c), not compiled (level 2)
//   0x004a43d2 / 0x004a43b9 / 0x004a2b60   CRT (_ftime64 / _ctime64 / sprintf): the VS2022 CRT functions, adapters below
//   s_fmt_005d0120 / DAT_005d0118           .rdata, zero in the exe unless PortTop_RdataSeed.inc lists them
//   DAT_007d3ff8 + 0xd4                     the RwGlobals record (LibRw/RwApiBind.cpp MashedRw_InstallRwGlobals)
// 0x00492d30's source declares the argument as undefined4; PortTop_Bound05_00498860 passes it on as the Camera pointer.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <sys/timeb.h>
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00498860 Orig_00498860
#define FUN_004cdca0 RwCore_004cdca0
#define FUN_004cdd00 RwCore_004cdd00
#define FUN_004cdd60 RwCore_004cdd60
#define FUN_004ce8e0 PortTop_StandIn05_004ce8e0
#define FUN_004d52d0 PortTop_Rw05_004d52d0
#define FUN_004a43d2 PortTop_Crt05_004a43d2
#define FUN_004a43b9 PortTop_Crt05_004a43b9
#define FUN_004a2b60 PortTop_Crt05_004a2b60
extern "C" {
#include "../../../../re/match/s16/00498810.c"

void* __cdecl MashedRw_ImageSetFromRaster_004d52d0(void* image, void* raster);
void PortTop_Rw05_004d52d0(int img, Raster* raster) {
    MashedRw_ImageSetFromRaster_004d52d0(reinterpret_cast<void*>(img), raster);
}
// CRT _ftime64 0x004a43d2: the source's Timeb64 is the __timeb64 layout (time, millitm, timezone, dstflag).
void PortTop_Crt05_004a43d2(Timeb64* tb) {
    static_assert(sizeof(Timeb64) == sizeof(__timeb64), "Timeb64 != __timeb64");
    _ftime64(reinterpret_cast<__timeb64*>(tb));
}
// CRT sprintf 0x004a2b60.
int PortTop_Crt05_004a2b60(char* out, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(out, fmt, ap);
    va_end(ap);
    return n;
}
// CRT _ctime64 0x004a43b9.
char* PortTop_Crt05_004a43b9(__int64* t) {
    return _ctime64(t);
}
void PortTop_Bound05_00498860(unsigned int cam) {
    static int once;
    if (!once) {
        once = 1;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] 0x00498860 (matched) reached, camera %08X\n", cam);
            std::fclose(f);
        }
    }
    Orig_00498860(reinterpret_cast<Camera*>(static_cast<std::uintptr_t>(cam)));
}
}
#endif // MASHED_STANDALONE
