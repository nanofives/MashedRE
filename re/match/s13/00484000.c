// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00484000 (h12): 100% (regs 100%). Key: one early-return 'if' per test (a merged && / || or a
   nested body puts the shared return-0 block in the wrong place, 81%). Globals as int[4] arrays. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBALA(int, DAT_006ce81c, 4, 0x006ce81c);
MGLOBALA(int, DAT_006ce82c, 4, 0x006ce82c);
int FUN_00483ed0(int *param_1, undefined4 param_2, undefined4 param_3, uint param_4);
// MATCH: 0x00484000 FUN_00484000
int FUN_00484000(uint param_1, undefined4 param_2, undefined4 param_3, uint param_4)
{
  if (param_1 >= 4) return 0;
  if (DAT_006ce81c[param_1] != 0) return 0;
  if (DAT_006ce82c[param_1] != 0) return 0;
  if ((DAT_006ce81c[param_1] = FUN_00483ed0(&DAT_006ce82c[param_1], param_2, param_3, param_4)) == 0) return 0;
  if (DAT_006ce82c[param_1] == 0) return 0;
  return 1;
}
