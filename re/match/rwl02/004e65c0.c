// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e65c0: best 100.0% (regs 100.0%), residual class -. callbacks declared as functions, int return, r >= 0 result. */
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

MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
void FUN_004e6610(void);
void FUN_004e6630(void);
void FUN_004d7ff0(void);
int FUN_004c1cc0(int p0, int p1, void (*p2)(void), void (*p3)(void), int p4);
int FUN_004e4bd0(int p0, int p1, void (*p2)(void), void (*p3)(void), int p4);
// MATCH: 0x004e65c0 FUN_004e65c0
int FUN_004e65c0(void)
{
  int r;
  r = FUN_004c1cc0(0xc,0x10,FUN_004e6610,FUN_004d7ff0,0);
  DAT_007d722c = r;
  if (r < 0) {
    return 0;
  }
  r = FUN_004e4bd0(0xc,0x10,FUN_004e6630,FUN_004d7ff0,0);
  DAT_007d7230 = r;
  return r >= 0;
}
