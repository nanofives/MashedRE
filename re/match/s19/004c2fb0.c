// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c2fb0 (h2): 75.6%, regs 88.9%. Hand: Ghidra nesting (success path inline, failure falls to the stop call). Residual (b): same lea esi prologue as 0x004c2f60 and a rotated scratch register choice (ecx/edx vs eax/ecx) on the RwGlobals reloads. Permuter's 86.7% variant moves the +0x124 store before the calls (changes behaviour), rejected. h6 resume: typed RwGlobals pointer with a dev member, int return of the last request, flagscan over 19 flag sets (all 76.9 or lower): no move. Class (b). */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_00617fe0, 1, 0x00617fe0);
int FUN_004c2c90(void *p0, int p1, void *p2, int p3, int p4);
int FUN_004cf160(int p0);
int FUN_004d8000(char *p0, int p1);
// MATCH: 0x004c2fb0 FUN_004c2fb0
int FUN_004c2fb0(void)
{
  void *dev = (void *)(DAT_007d3ff8 + 0x10);
  if (FUN_004c2c90(dev, 2, 0, 0, 0)) {
    if (FUN_004d8000(DAT_00617fe0, DAT_007d3ff8)) {
      FUN_004cf160(*(int *)(DAT_007d3ff8 + 0x10));
      FUN_004c2c90(dev, 0x11, 0, 0, 0);
      *(int *)(DAT_007d3ff8 + 0x124) = 3;
      return 1;
    }
    FUN_004c2c90(dev, 3, 0, 0, 0);
  }
  return 0;
}
