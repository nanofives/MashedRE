// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bdf40: 60.6% (regs 90.1%). Open-coded rep movs/stos loops replaced by intrinsic memcpy/memset (23.9->60.6); ptr args: eax=2nd arg in orig. Residual: orig does sete dl / mov ecx,edx / test (bool temp of p[1]==p[2], likely an inlined helper returning bool that /Ob1 does not reproduce) and a different load schedule; permuter best (77.5%) reordered the store before memcpy (semantic change, rejected). Class: b. */
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
typedef unsigned int size_t_;
void *memcpy(void *, const void *, size_t_);
void *memset(void *, int, size_t_);
#pragma intrinsic(memcpy, memset)
void FUN_005bdff0(int *p0, int p1);
int FUN_005be080(int *p0);
// MATCH: 0x005bdf40 FUN_005bdf40
void FUN_005bdf40(int *param_1, int *param_2)
{
  unsigned int uVar2;
  unsigned int uVar3;

  uVar2 = param_2[2] - param_2[1];
  uVar3 = param_1[2] - param_1[1];
  if (uVar2 <= uVar3) {
    uVar3 = uVar2;
  }
  memcpy((char *)(*param_1 + param_1[1]), (char *)(*param_2 + param_2[1]), uVar3);
  param_2[1] = param_2[1] + uVar3;
  param_1[1] = param_1[1] + uVar3;
  if ((param_2[3] != 0) && (param_2[1] == param_2[2])) {
    if (FUN_005be080(param_2) != 0) {
      uVar2 = param_1[2] - param_1[1];
      memset((char *)(*param_1 + param_1[1]), 0, uVar2);
      param_1[1] = param_1[1] + uVar2;
      FUN_005bdff0(param_1, 1);
    }
  }
}

