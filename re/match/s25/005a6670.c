// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a6670: by hand. 100%: base pointer local iVar3 = param_1 + 0x28 (esi) used for +8 / +0x5c offsets. */
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
int FUN_005a6e10(int p0, int p1, int p2, int p3);
void FUN_005a7520(int p0, undefined4 p1);
// MATCH: 0x005a6670 FUN_005a6670
int FUN_005a6670(int param_1)
{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  iVar3 = param_1 + 0x28;
  if ((*(byte *)(param_1 + 0x34) & 2) != 0) {
    uVar2 = 2;
    if (*(int *)(iVar3 + 8) != 0) {
      iVar1 = FUN_005a6e10(*(int *)(iVar3 + 8),6,2,0);
      if (iVar1 == 0) {
        uVar2 = 1;
      }
    }
    FUN_005a7520(iVar3,uVar2);
  }
  *(undefined4 *)(iVar3 + 0x5c) = 0;
  return param_1;
}
