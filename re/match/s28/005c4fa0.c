// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005c4fa0 : 44.7% (regs 63, ops 67): raw converter output kept as the best (permuter 300 s reached 44.9, not worth the churn). Tree walk over a node table (param_1[0x51] is a stack top, nodes 0x10 bytes, float compares against the walker arrays). Not reworked by hand: x87 compare/flag idioms (fcom + test ah) in the raw text are the main residue. Residual class (b), ran out of budget. */

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

typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef signed char sbyte;
#ifndef __cplusplus
#define true 1
#endif
// MATCH: 0x005c4fa0 FUN_005c4fa0
void FUN_005c4fa0(int *param_1,undefined2 *param_2,ushort *param_3,undefined4 *param_4)

{
  float fVar1;
  char cVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = param_1[0x51];
  param_1[0x51] = iVar7 + -8;
  iVar7 = *(int *)(iVar7 + -8);
  cVar2 = *(char *)(iVar7 + 1);
  while( true ) {
    if (cVar2 != -1) {
      *param_2 = *(undefined2 *)(iVar7 + 2);
      *param_3 = (ushort)*(byte *)(iVar7 + 1);
      *param_4 = *(undefined4 *)(param_1[0x51] + 4);
      return;
    }
    piVar3 = (int *)param_1[0x51];
    if (piVar3[1] == 0) break;
    iVar7 = (uint)*(ushort *)(iVar7 + 2) * 0x10;
    iVar8 = iVar7 + *(int *)(*param_1 + 0x20);
    uVar4 = *(byte *)(iVar7 + *(int *)(*param_1 + 0x20)) & 0xc;
    if (*(float *)(iVar8 + 0xc) <= *(float *)(uVar4 + 4 + (int)param_1)) {
      *piVar3 = iVar8 + 8;
      fVar1 = *(float *)(uVar4 + 0x10 + (int)param_1);
      if (fVar1 < *(float *)(iVar8 + 4) != (fVar1 == *(float *)(iVar8 + 4))) {
        iVar7 = param_1[0x51];
        param_1[0x51] = iVar7 + 8;
        *(int *)(iVar7 + 8) = iVar8;
        *(undefined4 *)(param_1[0x51] + 4) = *(undefined4 *)(param_1[0x51] + -4);
        if (*(float *)(iVar8 + 4) <= *(float *)(uVar4 + 0x1c + (int)param_1)) {
          *(uint *)(param_1[0x51] + 4) = *(uint *)(param_1[0x51] + 4) & ~(2 << (sbyte)(uVar4 >> 1));
        }
        fVar1 = *(float *)(uVar4 + 0x28 + (int)param_1);
        if (fVar1 < *(float *)(iVar8 + 0xc) != (fVar1 == *(float *)(iVar8 + 0xc))) {
          *(uint *)(param_1[0x51] + -4) =
               *(uint *)(param_1[0x51] + -4) & ~(1 << (sbyte)(uVar4 >> 1));
        }
      }
    }
    else {
      *piVar3 = iVar8;
    }
    iVar7 = *(int *)param_1[0x51];
    cVar2 = *(char *)(iVar7 + 1);
  }
  iVar8 = *(int *)(*param_1 + 0x20);
  iVar5 = (uint)*(ushort *)(iVar7 + 2) * 0x10 + iVar8;
  cVar2 = *(char *)(iVar5 + 1);
  iVar7 = iVar5;
  while (cVar2 == -1) {
    iVar7 = (uint)*(ushort *)(iVar7 + 2) * 0x10 + iVar8;
    cVar2 = *(char *)(iVar7 + 1);
  }
  iVar6 = iVar5 + 8;
  cVar2 = *(char *)(iVar5 + 9);
  while (cVar2 == -1) {
    iVar5 = (uint)*(ushort *)(iVar6 + 2) * 0x10;
    iVar6 = iVar5 + 8 + iVar8;
    cVar2 = *(char *)(iVar5 + 9 + iVar8);
  }
  *param_2 = *(undefined2 *)(iVar7 + 2);
  *param_3 = ((ushort)*(byte *)(iVar6 + 1) + *(short *)(iVar6 + 2)) - *(short *)(iVar7 + 2);
  *param_4 = 0;
  return;
}

