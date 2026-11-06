// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004722e0 (h7): 100%. RW-style set-callback with NULL fallback (store, then if (cb==0) store default). Residual: none. */
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
typedef void *(*RenderCB_004722e0)(void *);
MGLOBAL(undefined4, DAT_0086ecd0, 0x0086ecd0);
void *FUN_004721e0(void *p0);
void *FUN_004e5f90(void *p0);
// MATCH: 0x004722e0 FUN_004722e0
void FUN_004722e0(int param_1)
{
  DAT_0086ecd0 = *(undefined4 *)(param_1 + 0x48);
  *(RenderCB_004722e0 *)(param_1 + 0x48) = FUN_004721e0;
  if (FUN_004721e0 == 0) {
    *(RenderCB_004722e0 *)(param_1 + 0x48) = FUN_004e5f90;
  }
}
