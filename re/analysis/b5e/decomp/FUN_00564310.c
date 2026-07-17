
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Selects an octree octant (3-bit code in [0..7]) where `param_2` (loose AABB)
   overlaps `param_3` (parent AABB). The… */

uint FUN_00564310(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;

  uVar7 = _rand();
  fVar1 = *param_3 * _DAT_005e5418 + param_3[4] * _DAT_005cc318;
  fVar2 = param_3[1] * _DAT_005e5418 + param_3[5] * _DAT_005cc318;
  fVar3 = param_3[2] * _DAT_005e5418 + param_3[6] * _DAT_005cc318;
  fVar4 = param_3[4] * _DAT_005e5418 + *param_3 * _DAT_005cc318;
  fVar5 = param_3[5] * _DAT_005e5418 + param_3[1] * _DAT_005cc318;
  fVar6 = param_3[6] * _DAT_005e5418 + param_3[2] * _DAT_005cc318;
  uVar8 = (uint)(fVar1 < param_2[4]) +
          ((uint)(fVar2 < param_2[5]) + (uint)(fVar3 < param_2[6]) * 2) * 2;
  uVar10 = (uint)(*param_2 < fVar4) +
           ((uint)(param_2[1] < fVar5) + (uint)(param_2[2] < fVar6) * 2) * 2;
  uVar9 = uVar10 & uVar8;
  if ((ushort)((ushort)uVar10 | (ushort)uVar8) != 7) {
    return 0xffff;
  }
  uVar8 = ~uVar9 & uVar8 | uVar7 & uVar9;
  param_1[4] = (float)(uVar8 & 1) * fVar1 + (float)(int)(1 - (uVar8 & 1)) * param_3[4];
  param_1[5] = (float)(uVar8 >> 1 & 1) * fVar2 + (float)(int)(1 - (uVar8 >> 1 & 1)) * param_3[5];
  uVar10 = -uVar8 - 1;
  uVar7 = (int)uVar10 >> 1 & 1;
  param_1[6] = (float)(uVar8 >> 2 & 1) * fVar3 + (float)(int)(1 - (uVar8 >> 2 & 1)) * param_3[6];
  *param_1 = (float)(uVar10 & 1) * fVar4 + (float)(int)(1 - (uVar10 & 1)) * *param_3;
  param_1[1] = (float)uVar7 * fVar5 + (float)(int)(1 - uVar7) * param_3[1];
  uVar10 = (int)uVar10 >> 2 & 1;
  param_1[2] = (float)uVar10 * fVar6 + (float)(int)(1 - uVar10) * param_3[2];
  return uVar8;
}

