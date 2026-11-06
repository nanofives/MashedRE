// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475d30: 100% (regs 100%), relocheck OK. By hand (raw 83.9%): DAT_007d3ff8 is RwGlobals*, the call is its +0x20 slot
   (typed as a one-field head struct); plain for loop over DAT_00691608 + i * 0x40. */
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
typedef int (*RwSetStateFn)(int state, int value);
typedef struct { unsigned char pad[0x20]; RwSetStateFn setState; } RwGlobalsHead;
MGLOBAL(RwGlobalsHead *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0069160c, 0x0069160c);
MGLOBAL(int, DAT_00691608, 0x00691608);
void FUN_004770a0(int p0);
// MATCH: 0x00475d30 FUN_00475d30
void FUN_00475d30(void)
{
  int i;
  DAT_007d3ff8->setState(8, 0);
  for (i = 0; i < DAT_0069160c; i++) {
    FUN_004770a0(DAT_00691608 + i * 0x40);
  }
  DAT_007d3ff8->setState(8, 1);
}
