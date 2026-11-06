// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00466100: 100% (regs 100%) by hand from raw g2s 65.9%. Guard as early returns, slot index in a
   fresh local (reusing param_2 loaded param_2 before the table entry), float clamp if/else-if then one call. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(int, DAT_00604570, 0x00604570);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
int FUN_00492d10(void);
// MATCH: 0x00466100 FUN_00466100
void FUN_00466100(int param_1,int param_2,float param_3)
{
  int iVar1;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1 && param_1 == 0x20) return;
  if (FUN_00492d10() != 3) return;
  iVar1 = (&DAT_00604570)[param_1 * 0x25] + param_2;
  if ((*(byte *)((&DAT_0068f640)[iVar1 * 2] + 0x34) & 2) != 0) {
    if (param_3 > DAT_005cc320) {
      param_3 = 1.0f;
    }
    else if (param_3 < DAT_005d757c) {
      param_3 = 0.0f;
    }
    FUN_0045e0f0(iVar1,param_3);
  }
}
