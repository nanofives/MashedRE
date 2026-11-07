// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c9770: 76.0% (regs 80.0%, ops 95%). Clamp24 helper (int return, (int)(short)(v>>8)) + 2x-unrolled frame loop; permuter flipped the tail compare to param_3 > uVar2 (+11). Residual: prologue order (orig loads param_3 first, mov ebx,0 after shr) and add ebx,ebx vs lea. Class: b. */
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
static __inline int Clamp24(int v)
{
  if (v < -0x800000) {
    return -0x8000;
  }
  if (v > 0x7fff00) {
    return 0x7fff;
  }
  return (int)(short)(v >> 8);
}
// MATCH: 0x005c9770 FUN_005c9770
void FUN_005c9770(short *param_1, int *param_2, unsigned int param_3)
{
  unsigned int uVar2;
  int iVar4;
  unsigned int uVar5;

  uVar2 = 0;
  for (uVar5 = param_3 >> 1; uVar5 != 0; uVar5 = uVar5 - 1) {
    param_1[0] = Clamp24(param_2[0]);
    param_2 = param_2 + 4;
    param_1[1] = Clamp24(param_2[1]);
    param_1[2] = Clamp24(param_2[2]);
    param_1[3] = Clamp24(param_2[3]);
    param_1 = param_1 + 4;
    uVar2 = param_3 >> 1;
  }
  uVar2 = uVar2 + uVar2;
  if (param_3 > uVar2) {
    iVar4 = param_3 - uVar2;
    do {
      param_1[0] = Clamp24(param_2[0]);
      param_1[1] = Clamp24(param_2[1]);
      param_2 = param_2 + 2;
      param_1 = param_1 + 2;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
}


