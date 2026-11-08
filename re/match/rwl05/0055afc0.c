// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055afc0: hand-matched (h1). 100%. ushort field loaded into an unsigned local inside the condition (gives xor ecx,ecx; mov cx), '> 0' for jbe. */
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
typedef int * (__cdecl *code)(int *, int);
// MATCH: 0x0055afc0 FUN_0055afc0
int FUN_0055afc0(int param_1,code param_2,int param_3)
{
  unsigned int uVar1;
  unsigned int uVar3;
  int *piVar4;

  uVar3 = 0;
  piVar4 = *(int **)(param_1 + 0x34);
  if (*(unsigned int *)(param_1 + 0x40) > 0) {
    do {
      if ((param_1 == *piVar4) &&
          ((uVar1 = *(unsigned short *)(piVar4 + 1)) < *(unsigned int *)(param_1 + 0x14)) &&
          ((*(unsigned int *)(*(int *)(param_1 + 0x6c) + (uVar1 >> 5) * 4) & (1 << (uVar1 & 0x1f))) != 0) &&
          ((*param_2)(piVar4,param_3) != piVar4)) {
        return param_1;
      }
      uVar3 = uVar3 + 1;
      piVar4 = piVar4 + 0x27;
    } while (uVar3 < *(unsigned int *)(param_1 + 0x40));
  }
  return param_1;
}
