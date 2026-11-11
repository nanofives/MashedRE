// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 88.0% (regs 88.0%): original has a separate `pop esi; ret` for the first failed test (eax already 0), ours merges both zero returns into one `xor eax,eax` tail. Tried: iVar1 returns, compound ||/&& condition, nested-if forms (72%). Class (b).
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

MGLOBAL(undefined4, DAT_007d7234, 0x007d7234);
MGLOBAL(undefined4, DAT_007d725c, 0x007d725c);
int FUN_004cc790(undefined4 p0, undefined4 *p1, int p2);
// MATCH: 0x004e7e50 FUN_004e7e50
undefined4 FUN_004e7e50(undefined4 param_1, int param_2)
{
  if (FUN_004cc790(param_1, &DAT_007d725c, 4) == 0) {
    return 0;
  }
  if (param_2 == 8 && FUN_004cc790(param_1, &DAT_007d7234, 4) == 0) {
    return 0;
  }
  return param_1;
}
