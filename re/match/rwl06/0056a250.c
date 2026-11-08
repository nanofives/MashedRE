// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056a250: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
// MATCH: 0x0056a250 FUN_0056a250
void FUN_0056a250(uint *param_1,undefined4 *param_2,int param_3,uint param_4)

{
  float *pfVar1;
  float *pfVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int in_stack_0000003c;
  float *in_stack_00000048;
  float in_stack_00000054;
  float *local_28;
  
  uVar6 = 0xffffffff;
  uVar3 = 0xffffffff;
  uVar5 = 0;
  if (param_4 != 0) {
    local_28 = in_stack_00000048;
    iVar4 = param_3 - in_stack_0000003c;
    pfVar2 = (float *)(in_stack_0000003c + 8);
    pfVar1 = (float *)(param_3 + 0xc);
    do {
      if ((in_stack_00000054 < pfVar2[-2] * pfVar1[-3]) ||
         (*local_28 * pfVar1[-3] < -in_stack_00000054)) {
        in_stack_00000054 = ABS(pfVar1[-3]);
        uVar3 = 0;
        uVar6 = uVar5;
      }
      if ((in_stack_00000054 < pfVar2[-1] * pfVar1[-2]) ||
         (*(float *)(((int)in_stack_00000048 - param_3) + -8 + (int)pfVar1) * pfVar1[-2] <
          -in_stack_00000054)) {
        in_stack_00000054 = ABS(pfVar1[-2]);
        uVar3 = 1;
        uVar6 = uVar5;
      }
      if ((in_stack_00000054 < *pfVar2 * *(float *)((int)pfVar2 + iVar4)) ||
         (*(float *)(((int)in_stack_00000048 - in_stack_0000003c) + (int)pfVar2) *
          *(float *)((int)pfVar2 + iVar4) < -in_stack_00000054)) {
        in_stack_00000054 = ABS(*(float *)((int)pfVar2 + iVar4));
        uVar3 = 2;
        uVar6 = uVar5;
      }
      if ((in_stack_00000054 < pfVar2[1] * *pfVar1) ||
         (*pfVar1 * *(float *)(((int)in_stack_00000048 - param_3) + (int)pfVar1) <
          -in_stack_00000054)) {
        in_stack_00000054 = ABS(*pfVar1);
        uVar3 = 3;
        uVar6 = uVar5;
      }
      uVar5 = uVar5 + 1;
      local_28 = local_28 + 4;
      pfVar2 = pfVar2 + 4;
      pfVar1 = pfVar1 + 4;
    } while (uVar5 < param_4);
    *param_1 = uVar6;
    *param_2 = uVar3;
    return;
  }
  *param_1 = 0xffffffff;
  *param_2 = 0xffffffff;
  return;
}
