// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558cb0 (h2): hand, 78.4%, class (b): original has an 8-byte local frame (sub esp 8) and keeps ecx=param_2 pushed before the first callee (g(f(item),param_2) order, f cdecl 1 arg, g cdecl 2 args); the sub esp 8 source is not found, permuter 180 s no gain. */
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
typedef int (*FN1)(int);
typedef int (*FN2)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int * FUN_0055dec0(int p0);
int * FUN_005c4e10(int p0);
// MATCH: 0x00558cb0 FUN_00558cb0
int FUN_00558cb0(int *param_1,int param_2)
{
  FN1 pcVar2;
  int uVar3;
  int *puVar4;
  int *puVar5;
  FN2 *puVar1;

  pcVar2 = *(FN1 *)(*param_1 + 0x20);
  puVar4 = FUN_005c4e10(param_1[1]);
  puVar5 = FUN_0055dec0(param_1[1]);
  while (puVar5 != puVar4) {
    uVar3 = *puVar5;
    puVar1 = (FN2 *)(DAT_007d3ff8 + 0xe8);
    if ((*puVar1)(pcVar2(uVar3), param_2) == 0)
      return uVar3;
    puVar5 = puVar5 + 1;
  }
  return 0;
}
