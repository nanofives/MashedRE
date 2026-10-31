// FLAGS: /O2 /GS /Ob1
/* 0x0040e500 (95 B): 100.0% exact, regs 100.0%.
   Raw g2s 88.6% (Ghidra's merged while condition). A plain for (i < 4) loop with two early-return
   tests (state == 8 under modes 2/3 or FUN_0042f500, state >= 0xc under mode 4). Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_005f29bc, 0x005f29bc);
MGLOBAL(int, DAT_008a94d0, 0x008a94d0);

int FUN_0040b6d0(int p0);
int FUN_0042f500(void);

// MATCH: 0x0040e500 FUN_0040e500
int FUN_0040e500(void)
{
  int i;
  int state;

  DAT_005f29bc = -1;
  for (i = 0; i < 4; i++) {
    state = FUN_0040b6d0(i);
    if ((DAT_008a94d0 == 2 || DAT_008a94d0 == 3 || FUN_0042f500() != 0) && state == 8) {
      DAT_005f29bc = i;
      return i;
    }
    if (DAT_008a94d0 == 4 && state >= 0xc) {
      DAT_005f29bc = i;
      return i;
    }
  }
  return DAT_005f29bc;
}
