// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056ac40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005e5a40, 0x005e5a40);
MGLOBAL(undefined1, UNK_005e5a44, 0x005e5a44);
MGLOBAL(undefined1, UNK_005e5a48, 0x005e5a48);
MGLOBAL(undefined1, UNK_005e5a4c, 0x005e5a4c);
MGLOBAL(undefined1, DAT_005e5a70, 0x005e5a70);
MGLOBAL(undefined1, UNK_005e5a74, 0x005e5a74);
MGLOBAL(undefined1, UNK_005e5a78, 0x005e5a78);
MGLOBAL(undefined1, UNK_005e5a7c, 0x005e5a7c);
MGLOBAL(int, _UNK_005e5a74, 0x005e5a74);
MGLOBAL(int, _DAT_005e5a70, 0x005e5a70);
MGLOBAL(int, _UNK_005e5a78, 0x005e5a78);
MGLOBAL(int, _UNK_005e5a7c, 0x005e5a7c);
MGLOBAL(int, _DAT_005e5a40, 0x005e5a40);
MGLOBAL(int, _UNK_005e5a44, 0x005e5a44);
MGLOBAL(int, _UNK_005e5a48, 0x005e5a48);
MGLOBAL(int, _UNK_005e5a4c, 0x005e5a4c);
// MATCH: 0x0056ac40 FUN_0056ac40
/* [C1 2026-05-19] Signature: `float10 FUN_0056ac40(int *param_1, uint *param_2, float *param_3, int
   param_4)`. */

float10 FUN_0056ac40(int *param_1,uint *param_2,float *param_3,int param_4)

{
  uint *puVar1;
  undefined1 auVar2 [16];
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  float10 fVar9;
  uint uVar10;
  uint uVar12;
  uint uVar13;
  undefined1 auVar11 [16];
  uint uVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  int in_stack_00000024;
  uint *in_stack_00000030;
  float in_stack_0000003c;
  float local_40;
  int local_3c;
  float local_30 [4];
  float local_20;
  uint uStack_1c;
  uint uStack_18;
  uint uStack_14;
  
  local_40 = in_stack_0000003c;
  local_20 = 0.0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  fVar15 = 0.0;
  fVar16 = 0.0;
  fVar17 = 0.0;
  fVar18 = 0.0;
  local_30[1] = in_stack_0000003c;
  local_30[2] = in_stack_0000003c;
  local_30[3] = in_stack_0000003c;
  if (param_4 != 0) {
    local_3c = param_4;
    iVar5 = (int)param_3 - (int)in_stack_00000030;
    in_stack_00000024 = in_stack_00000024 - (int)in_stack_00000030;
    do {
      puVar1 = (uint *)(iVar5 + (int)in_stack_00000030);
      uVar7 = *puVar1;
      uVar6 = puVar1[1];
      uVar3 = puVar1[2];
      uVar4 = puVar1[3];
      puVar1 = (uint *)(in_stack_00000024 + (int)in_stack_00000030);
      (*(uint *)((char *)&auVar2 + 4)) = -(uint)(local_30[1] < (float)(_UNK_005e5a74 ^ uVar6 & in_stack_00000030[1]));
      (*(uint *)((char *)&auVar2 + 0)) = -(uint)(in_stack_0000003c < (float)(_DAT_005e5a70 ^ uVar7 & *in_stack_00000030)
                            );
      (*(uint *)((char *)&auVar11 + 4)) = -(uint)(local_30[1] < (float)(uVar6 & puVar1[1]));
      (*(uint *)((char *)&auVar11 + 0)) = -(uint)(in_stack_0000003c < (float)(uVar7 & *puVar1));
      (*(uint *)((char *)&auVar11 + 8)) = -(uint)(local_30[2] < (float)(uVar3 & puVar1[2]));
      (*(uint *)((char *)&auVar11 + 12)) = -(uint)(local_30[3] < (float)(uVar4 & puVar1[3]));
      (*(uint *)((char *)&auVar2 + 8)) = -(uint)(local_30[2] < (float)(_UNK_005e5a78 ^ uVar3 & in_stack_00000030[2]));
      (*(uint *)((char *)&auVar2 + 12)) = -(uint)(local_30[3] < (float)(_UNK_005e5a7c ^ uVar4 & in_stack_00000030[3]));
      auVar11 = auVar11 | auVar2;
      iVar8 = movmskps(in_stack_00000024 + (int)in_stack_00000030,auVar11);
      if (iVar8 != 0) {
        uVar10 = (*(uint *)((char *)&auVar11 + 0));
        uVar12 = (*(uint *)((char *)&auVar11 + 4));
        uVar13 = (*(uint *)((char *)&auVar11 + 8));
        uVar14 = (*(uint *)((char *)&auVar11 + 12));
        local_20 = (float)(uVar10 & (uint)fVar15 | ~uVar10 & (uint)local_20);
        uStack_1c = uVar12 & (uint)fVar16 | ~uVar12 & uStack_1c;
        uStack_18 = uVar13 & (uint)fVar17 | ~uVar13 & uStack_18;
        uStack_14 = uVar14 & (uint)fVar18 | ~uVar14 & uStack_14;
        in_stack_0000003c =
             (float)(uVar10 & ~_DAT_005e5a70 & uVar7 | ~uVar10 & (uint)in_stack_0000003c);
        local_30[1] = (float)(uVar12 & ~_UNK_005e5a74 & uVar6 | ~uVar12 & (uint)local_30[1]);
        local_30[2] = (float)(uVar13 & ~_UNK_005e5a78 & uVar3 | ~uVar13 & (uint)local_30[2]);
        local_30[3] = (float)(uVar14 & ~_UNK_005e5a7c & uVar4 | ~uVar14 & (uint)local_30[3]);
      }
      fVar15 = fVar15 + _DAT_005e5a40;
      fVar16 = fVar16 + _UNK_005e5a44;
      fVar17 = fVar17 + _UNK_005e5a48;
      fVar18 = fVar18 + _UNK_005e5a4c;
      in_stack_00000030 = in_stack_00000030 + 4;
      local_3c = local_3c + -1;
    } while (local_3c != 0);
  }
  iVar5 = -1;
  uVar7 = 0xffffffff;
  fVar9 = (float10)DAT_005d757c;
  local_30[0] = in_stack_0000003c;
  for (; param_4 != 0; param_4 = param_4 + -1) {
    iVar8 = 4;
    do {
      fVar15 = *param_3;
      param_3 = param_3 + 1;
      iVar8 = iVar8 + -1;
      fVar9 = (float10)fVar15 * (float10)fVar15 + fVar9;
    } while (iVar8 != 0);
  }
  uVar6 = 0;
  do {
    if (local_40 < local_30[uVar6]) {
      local_40 = local_30[uVar6];
      uVar7 = uVar6;
    }
    uVar6 = uVar6 + 1;
  } while (uVar6 < 4);
  if (uVar7 != 0xffffffff) {
    iVar5 = (int)ROUND((&local_20)[uVar7]);
  }
  *param_1 = iVar5;
  *param_2 = uVar7;
  return fVar9;
}
