
/* [C1 2026-05-20] Signature recovered: `uint FUN_00576640(World *world, uint listA_len, uint
   listB_len, FilterMatrix… */

uint FUN_00576640(int *param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  float fVar1;
  int iVar2;
  int *piVar3;
  float *pfVar4;
  uint uVar5;
  uint uVar6;
  ushort *puVar7;
  uint uVar8;
  int *piVar9;
  float *pfVar10;
  float *local_24;
  uint local_1c;
  uint local_14;
  uint local_10;

  fVar1 = (float)param_1[0x1c];
  iVar2 = *param_1;
  piVar3 = (int *)param_1[3];
  uVar8 = 0;
  local_1c = 0;
  pfVar4 = (float *)param_1[9];
  uVar5 = param_1[2];
  local_10 = 0;
  piVar9 = piVar3;
  local_24 = pfVar4;
  if (param_2 != 0) {
    while (uVar8 < uVar5) {
      if ((piVar9[2] != 1) && (local_14 = 0, param_3 != 0)) {
        pfVar10 = pfVar4 + param_2 * 8 + 1;
        puVar7 = (ushort *)(piVar3 + param_2 * 5 + 1);
        do {
          if (((((*(int *)(puVar7 + 2) != 1) &&
                ((*piVar9 != *(int *)(puVar7 + -2) || (*(ushort *)(piVar9 + 1) != *puVar7)))) &&
               ((*(ushort *)(piVar9 + 1) != 0xffff || (*puVar7 != 0xffff)))) &&
              (((param_4 == 0 ||
                (uVar6 = (uint)*puVar7 + *(int *)(param_4 + 4) * (uint)*(ushort *)(piVar9 + 1),
                uVar8 = local_1c,
                (*(uint *)(param_4 + 0xc + (uVar6 >> 5) * 4) & 1 << ((byte)uVar6 & 0x1f)) == 0)) &&
               ((param_5 == 0 ||
                (uVar6 = (uint)*(ushort *)(*(int *)(puVar7 + 2) + 0x5a) +
                         (uint)*(ushort *)(piVar9[2] + 0x5a) * *(int *)(param_5 + 4),
                (*(uint *)(param_5 + 0xc + (uVar6 >> 5) * 4) & 1 << ((byte)uVar6 & 0x1f)) == 0))))))
             && ((local_24 == pfVar10 + -1 ||
                 (((((local_24[4] - pfVar10[-1] < fVar1 != (local_24[4] - pfVar10[-1] == fVar1) &&
                     (pfVar10[3] - *local_24 < fVar1 != (pfVar10[3] - *local_24 == fVar1))) &&
                    (local_24[5] - *pfVar10 < fVar1 != (local_24[5] - *pfVar10 == fVar1))) &&
                   ((pfVar10[4] - local_24[1] < fVar1 != (pfVar10[4] - local_24[1] == fVar1) &&
                    (local_24[6] - pfVar10[1] < fVar1 != (local_24[6] - pfVar10[1] == fVar1))))) &&
                  (pfVar10[5] - local_24[2] < fVar1 != (pfVar10[5] - local_24[2] == fVar1))))))) {
            *(int **)(iVar2 + uVar8 * 8) = piVar9;
            *(ushort **)(iVar2 + 4 + uVar8 * 8) = puVar7 + -2;
            uVar8 = uVar8 + 1;
            local_1c = uVar8;
            if (uVar5 <= uVar8) {
              param_1[0x1e] = 1;
              break;
            }
          }
          local_14 = local_14 + 1;
          pfVar10 = pfVar10 + 8;
          puVar7 = puVar7 + 10;
        } while (local_14 < param_3);
      }
      local_10 = local_10 + 1;
      local_24 = local_24 + 8;
      piVar9 = piVar9 + 5;
      if (param_2 <= local_10) {
        param_1[1] = uVar8;
        return uVar8;
      }
    }
  }
  param_1[1] = uVar8;
  return uVar8;
}

