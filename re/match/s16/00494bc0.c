// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494bc0 (h1): 100% (regs 100%), by hand. Residual: none.
   Six COM Release-and-clear blocks (vtable +8), with a vtable +0x24 call on 0x00771a48 first. The
   p = g; g = 0; Release(p) order gives the store before the call. */
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
typedef int (__stdcall *ComFn)(int *self);
MGLOBAL(int *, DAT_00771a38, 0x00771a38);
MGLOBAL(int *, DAT_00771a3c, 0x00771a3c);
MGLOBAL(int *, DAT_00771a40, 0x00771a40);
MGLOBAL(int *, DAT_00771a44, 0x00771a44);
MGLOBAL(int *, DAT_00771a48, 0x00771a48);
MGLOBAL(int *, DAT_00771a4c, 0x00771a4c);
// MATCH: 0x00494bc0 FUN_00494bc0
void FUN_00494bc0(void)
{
  int *p;
  if (DAT_00771a48 != 0) {
    (*(ComFn *)(*DAT_00771a48 + 0x24))(DAT_00771a48);
    if (DAT_00771a48 != 0) {
      p = DAT_00771a48;
      DAT_00771a48 = 0;
      (*(ComFn *)(*p + 8))(p);
    }
  }
  if (DAT_00771a3c != 0) {
    p = DAT_00771a3c;
    DAT_00771a3c = 0;
    (*(ComFn *)(*p + 8))(p);
  }
  if (DAT_00771a40 != 0) {
    p = DAT_00771a40;
    DAT_00771a40 = 0;
    (*(ComFn *)(*p + 8))(p);
  }
  if (DAT_00771a38 != 0) {
    p = DAT_00771a38;
    DAT_00771a38 = 0;
    (*(ComFn *)(*p + 8))(p);
  }
  if (DAT_00771a44 != 0) {
    p = DAT_00771a44;
    DAT_00771a44 = 0;
    (*(ComFn *)(*p + 8))(p);
  }
  if (DAT_00771a4c != 0) {
    p = DAT_00771a4c;
    DAT_00771a4c = 0;
    (*(ComFn *)(*p + 8))(p);
  }
}
