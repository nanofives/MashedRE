// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b0df0: 95.1% (regs 95.1%, ops 97.6%). Whole stream identical except stack cleanup: original emits add esp,4 right after the 4-arg call (before push eax for the nested 1-arg call) and add esp,0x10 at the end; the DDK emits one add esp,0x14. Same effect as 005af690. Tried nested expression, K&R () callee protos, split statements. Class (a). cxx pending. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined1, DAT_005e6de0, 0x005e6de0);
int FUN_005ad0b0(int p0);
int FUN_005ad6a0(undefined1 *p0, int p1, int p2, int p3);
// MATCH: 0x005b0df0 FUN_005b0df0
int * FUN_005b0df0(int *param_1,int param_2,int *param_3)
{
  int *puVar1;
  int iVar3;

  iVar3 = param_3[1];
  puVar1 = (int *)param_1[8];
  if (iVar3 < 0) {
    iVar3 = 1;
  }
  puVar1[7] = iVar3;
  iVar3 = *param_3;
  if (iVar3 < 0) {
    iVar3 = 1;
  }
  puVar1[3] = iVar3;
  puVar1[4] = 0;
  puVar1[5] = 0;
  puVar1[6] = 0;
  puVar1[9] = 0;
  puVar1[0xb] = param_3[4];
  puVar1[8] = 1;
  puVar1[10] = 2;
  *puVar1 = FUN_005ad0b0(FUN_005ad6a0(&DAT_005e6de0,*param_1,param_3[5],param_3[6]));
  return param_1;
}
