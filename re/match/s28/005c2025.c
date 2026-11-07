// FLAGS: /O1
// CRT _strcoll, hand-written (raw: _ptiddata type unaddressed)
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
MGLOBAL(int, PTR_DAT_00616be4, 0x00616be4);
MGLOBAL(int, DAT_00773da0, 0x00773da0);
char *FUN_004a8828(void);
int *FUN_004ae5fa(void);
int *FUN_004aa23d(void);
int FUN_004af340(char *p0, char *p1);
int FUN_004b319b(int p0, unsigned long p1, char *p2, int p3, char *p4, int p5, unsigned int p6);
// MATCH: 0x005c2025 FUN_005c2025
int FUN_005c2025(char *_Str1, char *_Str2)
{
  int iVar4;
  int *ptVar3;
  int pcVar1;

  ptVar3 = *(int **)(FUN_004a8828() + 0x64);
  if (ptVar3 != (int *)PTR_DAT_00616be4) {
    ptVar3 = FUN_004ae5fa();
  }
  pcVar1 = ptVar3[4];
  if ((pcVar1 == 0) && (DAT_00773da0 == 0)) {
    return FUN_004af340(_Str1, _Str2);
  }
  iVar4 = FUN_004b319b(pcVar1, 0x1000, _Str1, -1, _Str2, -1, ptVar3[2]);
  if (iVar4 == 0) {
    *FUN_004aa23d() = 0x16;
    return 0x7fffffff;
  }
  return iVar4 + -2;
}
