// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cbb00 (h2): 100% (regs 100%). D3D device COM call (slot 0x178) through a __stdcall function
   pointer typedef. Raw 91.7% (cdecl add esp). Residual: none. relocheck OK. */
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
typedef unsigned int undefined4;
typedef long (__stdcall *vfn4)(int *, undefined4, undefined4, undefined4);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
// MATCH: 0x004cbb00 FUN_004cbb00
void FUN_004cbb00(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  (*(vfn4 *)(*DAT_007d4110 + 0x178))(DAT_007d4110,param_1,param_2,param_3);
  return;
}
