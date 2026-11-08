// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00563df0: hand-matched (h3). 66.7% (regs 75.6). semantically exact form. residue: original `lea [p2+p3*2+2]; shl 3`, ours folds to lea [..*8+0x10] and uses ebp. A permuter variant at 86% swapped p2/p3 in the size sum (not equivalent) and was rejected. class (a)/(b). */
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
// MATCH: 0x00563df0 FUN_00563df0
void FUN_00563df0(int param_1, unsigned int param_2, unsigned int param_3)
{
  unsigned int *puVar1;
  unsigned int uVar2;

  puVar1 = *(unsigned int **)(param_1 + 8);
  puVar1[2] = (unsigned int)(puVar1 + 4);
  *(unsigned int *)(param_1 + 8) = *(int *)(param_1 + 8) + ((param_3 * 2 + param_2 + 2) << 3);
  uVar2 = 0;
  *puVar1 = param_2;
  puVar1[1] = param_3;
  if (param_2 > 0) {
    do {
      *(short *)(puVar1[2] + uVar2 * 8) = (short)uVar2;
      *(short *)(puVar1[2] + 2 + uVar2 * 8) = (short)uVar2;
      uVar2 = uVar2 + 1;
    } while (uVar2 < param_2);
  }
  for (uVar2 = param_2; uVar2 < (param_2 - 1) + param_3 * 2; uVar2 = uVar2 + 1) {
    *(short *)(puVar1[2] + 2 + uVar2 * 8) = (short)uVar2 + 1;
  }
  *(unsigned short *)(puVar1[2] + 2 + uVar2 * 8) = 0xffff;
  *(short *)(puVar1 + 3) = (short)param_2;
}
