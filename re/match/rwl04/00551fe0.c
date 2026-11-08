// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 h1 */
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
MGLOBAL(int, DAT_009124f8, 0x009124f8);
int FUN_0052e0e0(int p0, int p1);
// MATCH: 0x00551fe0 FUN_00551fe0
int FUN_00551fe0(int param_1,int param_2)
{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)(DAT_009124f8 + 8 + param_1);
  for (iVar2 = 8; iVar2 != 0; iVar2--) {
    if (*piVar1 != 0) {
      FUN_0052e0e0(*piVar1,param_2);
    }
    piVar1++;
  }
  return param_1;
}
