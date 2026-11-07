// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x005581f0 (h4): 100.0% (regs 100.0%). Early return on param_1 != 0, else the RWERROR pattern
   (RwError {0x180, FUN_004d7ff0(0x80000016)} passed to FUN_004d8480). */
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
void FUN_004c7650(int p);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
// MATCH: 0x005581f0 FUN_005581f0
int FUN_005581f0(int param_1)
{
  if (param_1 != 0) {
    FUN_004c7650(param_1);
    return 1;
  }
  {
    RwError err;
    err.pluginID = 0x180;
    err.errorCode = FUN_004d7ff0(0x80000016);
    FUN_004d8480(&err);
  }
  return 0;
}
