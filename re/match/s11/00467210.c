// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00467210 (h1): 100% (regs 100%), relocheck OK. Nested if with three explicit returns (the raw
   converter's flipped-condition form scored under 60%). */
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
MGLOBAL(int, DAT_006905b0, 0x006905b0);
int FUN_0042b930(void);
int FUN_0042f510(void);

// MATCH: 0x00467210 FUN_00467210
int FUN_00467210(int param_1)
{
  int iVar1;

  if (FUN_0042b930() == 3) {
    if (param_1 == -1) {
      return *(int *)(DAT_006905b0 + 4) + 0x10;
    }
    iVar1 = FUN_0042f510();
    return *(int *)(iVar1 + 4) + 0x10;
  }
  return *(int *)(DAT_006905b0 + 4) + 0x10;
}
