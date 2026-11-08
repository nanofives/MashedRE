// FLAGS: /O2 /Ob1 /G5
// h3: 44.2% regs 53.3%. Signature fixed to (int,int,float,int*); accumulators are the dead param slots (param_1/param_2 reused as floats, which fixed the stack slots) and the max compares are written as x > acc. Residual: loop index form and switch layout; class (b).
/* MATCH-RWL03 0x0053e430: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x0053e430 FUN_0053e430
void FUN_0053e430(int param_1,int param_2,float param_3,int *param_4)
{
  int *piVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  uint uVar7;
  uint uVar8;
  int *piVar9;
  int local_18;
  int local_14;
  float local_c;
  int local_8;

  iVar5 = param_2;
  piVar9 = (int *)(*(int *)(param_1 + 8) + *(int *)(param_1 + 0x30) * 8);
  iVar2 = *(int *)(param_1 + 4);
  local_8 = *(int *)(param_1 + 0x2c);
  local_14 = 0;
  local_18 = 0;
  local_c = 0.0;
  fVar4 = DAT_005d757c;
  if (-1 < local_8 + -1) {
    do {
      uVar7 = 3;
      (*(float *)&param_2) = 0.0;
      (*(float *)&param_1) = 0.0;
      uVar8 = 0;
      do {
        piVar1 = (int *)(iVar2 + (uint)*(ushort *)(*piVar9 + uVar8 * 2) * 0xc);
        fVar3 = *(float *)(iVar5 + *piVar1) - param_3;
        uVar7 = uVar7 & piVar1[1];
        if (-fVar3 > (*(float *)&param_1)) {
          (*(float *)&param_1) = -fVar3;
        }
        else if (fVar3 > (*(float *)&param_2)) {
          (*(float *)&param_2) = fVar3;
        }
        uVar8 = uVar8 + 1;
      } while (uVar8 < 3);
      switch(uVar7) {
      case 0:
        if ((*(float *)&param_2) <= (*(float *)&param_1)) {
          piVar9[1] = 1;
          local_18 = local_18 + 1;
          if (fVar4 < (*(float *)&param_2)) {
            fVar4 = (*(float *)&param_2);
          }
        }
        else {
          piVar9[1] = 2;
          local_14 = local_14 + 1;
          if (local_c < (*(float *)&param_1)) {
            local_c = (*(float *)&param_1);
          }
        }
        break;
      case 1:
        piVar9[1] = 1;
        local_18 = local_18 + 1;
        break;
      case 2:
      case 3:
        piVar9[1] = 2;
        local_14 = local_14 + 1;
      }
      piVar9 = piVar9 + 2;
      local_8 = local_8 + -1;
    } while (local_8 != 0);
  }
  *param_4 = local_18;
  param_4[1] = local_14;
  *(float *)&param_4[2] = fVar4 + param_3;
  *(float *)&param_4[3] = param_3 - local_c;
  return;
}
