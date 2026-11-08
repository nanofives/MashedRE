// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ada40 (h3): 100.0% exact (regs 100.0%). do-while form (while(iVar1 != 0)) stops the loop rotation; callee types fixed. */
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
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_009146f0, 0x009146f0);
void FUN_005ad8b0(int p0);
void FUN_005ad8d0(int p0);
undefined4 FUN_005ae030(undefined4 p0);
void FUN_005b35e0(void * p0, undefined4 p1);
int FUN_005b3670(void * p0, undefined4 p1);
// MATCH: 0x005ada40 FUN_005ada40
void FUN_005ada40(undefined4 param_1,int param_2)
{
  int iVar1;
  do {
    iVar1 = FUN_005b3670(&DAT_009146f0,param_1);
    if (iVar1 != 0) {
      if (param_2 == 1) FUN_005ada40(iVar1,1);
    }
  } while (iVar1 != 0);
  FUN_005ad8b0((int)param_1);
  FUN_005b35e0(&DAT_009146f0,param_1);
  FUN_005ae030(param_1);
  FUN_005ad8d0((int)param_1);
}
