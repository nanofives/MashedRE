// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd070 (h3): 100% (regs 100%). size > 0x10000 -> RWERROR (pluginID 1, FUN_004d7ff0(0x32)); else fills the request record at module+0x38 (short size, ptr, 0x24, data at +0x10, flags|0x18 at +0xc) with module = RwGlobals + DAT_00911d00 (offset second), calls FUN_004d40d0(module->handle, &req, 1), returns arg 1 on nonzero. Moved from 60.8%: RwError struct, if/else order (error branch first in source), no early return in the error branch. Relocheck OK. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct Req { short size; short pad; void *ptr; int type; unsigned int flags; void *data; } Req;
typedef struct Mod { void *handle; int pad[13]; Req req; } Mod;
MGLOBAL(int, DAT_00911d00, 0x00911d00);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d40d0(void *p0, Req *p1, int p2);
int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd070 FUN_004cd070
void *FUN_004cd070(void *param_1, unsigned int param_2, void *param_3, unsigned int param_4)
{
  RwError err;
  if (param_2 > 0x10000) {
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x32);
    FUN_004d8480(&err);
  } else {
    MOD->req.size = (short)param_2;
    MOD->req.ptr = param_1;
    MOD->req.type = 0x24;
    MOD->req.data = param_3;
    MOD->req.flags = param_4 | 0x18;
    if (FUN_004d40d0(MOD->handle, &MOD->req, 1)) {
      return param_1;
    }
  }
  return 0;
}
