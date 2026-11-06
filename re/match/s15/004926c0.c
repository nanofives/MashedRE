// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004926c0 (h7, by hand): 100.0% (regs 100.0%). Raw 93.6%: the FUN_00496930(4) test written with
   the != 0 branch first (store 0) and the DAT_006147b8 test as its else-if; DAT_0077197c as int. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(int, DAT_006147b8, 0x006147b8);
MGLOBAL(int, DAT_00771994, 0x00771994);
MGLOBAL(int, DAT_00771978, 0x00771978);
undefined4 FUN_0045d3f0(int p0);
undefined4 FUN_0045d430(void);
void FUN_00466b50(int p0);
int FUN_004950b0(void);
int FUN_00496930(int p0);
MGLOBAL(int, DAT_0077197c, 0x0077197c);
// MATCH: 0x004926c0 FUN_004926c0
void FUN_004926c0(void)
{
  int t0;

  t0 = FUN_004950b0();
  if (FUN_00496930(3) != 0) {
    DAT_006147b8 = 1;
  }
  if (FUN_00496930(4) != 0) {
    DAT_006147b8 = 0;
  } else if (DAT_006147b8 != 0) {
    FUN_0045d3f0(0);
    FUN_00466b50(0);
    FUN_0045d430();
  }
  DAT_00771994 += FUN_004950b0() - t0;
  DAT_00771978++;
  if (DAT_00771978 == 60) {
    DAT_0077197c = DAT_00771994 / 60;
    DAT_00771994 = 0;
    DAT_00771978 = 0;
  }
}
