// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e55d0: best 88.2% (regs 94.1%), residual class b. orig flag test is byte [p+3]&1 with range check first and unsigned compares (jb/jae); remaining: tree walk register choice (orig reuses cursor eax and edx copy); permuter flat at 88.2. */
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

MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d71cc, 0x007d71cc);
// MATCH: 0x004e55d0 FUN_004e55d0
undefined4 FUN_004e55d0(int *param_1)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  int aiStack_100 [64];
  puVar1 = (undefined4 *)(DAT_007d71cc + 4 + DAT_007d3ff8);
  puVar2 = (undefined4 *)*puVar1;
  while (puVar2 != puVar1) {
    piVar3 = (int *)puVar2[-2];
    if ((*(byte *)((int)piVar3 + 3) & 1) != 0) {
      if (!((uint)param_1 < (uint)piVar3) && (uint)param_1 < (uint)(puVar2[-1] + (int)piVar3)) {
        return puVar2[-2];
      }
    }
    else {
      iVar5 = 0;
      piVar3 = (int *)piVar3[7];
      do {
        if (*piVar3 < 0) {
          if (param_1 == piVar3) return puVar2[-2];
          piVar4 = (int *)aiStack_100[iVar5];
          iVar5 = iVar5 + -1;
        }
        else {
          piVar4 = (int *)piVar3[2];
          iVar5 = iVar5 + 1;
          aiStack_100[iVar5] = piVar3[3];
        }
        piVar3 = piVar4;
      } while (iVar5 >= 0);
    }
    puVar2 = (undefined4 *)*puVar2;
  }
  return 0;
}
