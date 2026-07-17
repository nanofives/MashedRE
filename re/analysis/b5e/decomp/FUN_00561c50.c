
/* [C1 2026-05-19] Iterates pair records (count `*(this+0xc8)`, stride 0x28, base `*(this+0xd4)`)
   and for active ones (`!(rec[6] & 3)`): */

int FUN_00561c50(int param_1)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  float *pfVar11;
  int iVar12;
  int *piVar13;
  int *piVar14;
  uint local_28;
  uint local_24;
  int local_20;
  uint local_1c;
  uint local_14;

  fVar2 = *(float *)(param_1 + 0xc);
  local_14 = 0;
  fVar3 = *(float *)(param_1 + 0x10);
  fVar4 = *(float *)(param_1 + 0x14);
  fVar5 = *(float *)(param_1 + 0x18);
  if (*(int *)(param_1 + 200) != 0) {
    local_20 = 0;
    do {
      piVar13 = (int *)(*(int *)(param_1 + 0xd4) + local_20);
      if (((*(byte *)(piVar13 + 6) & 3) == 0) && (local_24 = 0, piVar13[3] != 0)) {
        do {
          iVar8 = *(int *)(*piVar13 + local_24 * 4);
          local_28 = *(uint *)(iVar8 + 8) & 1;
          if (local_28 != 0) {
            local_1c = 0;
            if (*(ushort *)(iVar8 + 0xc) != 0) {
              piVar14 = (int *)(*(int *)(iVar8 + 0x10) + 4);
              do {
                iVar9 = *(int *)(*(int *)*piVar14 + 0x10);
                iVar12 = ((int *)*piVar14)[1] * 0x20;
                iVar10 = *(int *)(iVar9 + 8);
                fVar6 = *(float *)(iVar10 + 8 + iVar12);
                fVar7 = *(float *)(iVar10 + 4 + iVar12);
                iVar1 = iVar10 + iVar12;
                if ((((fVar2 * fVar2 <
                       fVar6 * fVar6 +
                       fVar7 * fVar7 + *(float *)(iVar10 + iVar12) * *(float *)(iVar10 + iVar12)) ||
                     (fVar3 * fVar3 <
                      *(float *)(iVar1 + 0x18) * *(float *)(iVar1 + 0x18) +
                      *(float *)(iVar1 + 0x14) * *(float *)(iVar1 + 0x14) +
                      *(float *)(iVar1 + 0x10) * *(float *)(iVar1 + 0x10))) ||
                    (pfVar11 = (float *)(*(int *)(iVar9 + 0x40) + iVar12),
                    fVar4 * fVar4 <
                    pfVar11[2] * pfVar11[2] + pfVar11[1] * pfVar11[1] + *pfVar11 * *pfVar11)) ||
                   (fVar5 * fVar5 <
                    pfVar11[6] * pfVar11[6] + pfVar11[5] * pfVar11[5] + pfVar11[4] * pfVar11[4])) {
                  local_28 = 0;
                  break;
                }
                piVar14 = piVar14 + 3;
                local_1c = local_1c + 1;
              } while (local_1c < *(ushort *)(iVar8 + 0xc));
            }
          }
          FUN_0055ac00(*(undefined4 *)(iVar8 + 0x24),iVar8,local_28 == 0);
          local_24 = local_24 + 1;
        } while (local_24 < (uint)piVar13[3]);
      }
      local_14 = local_14 + 1;
      local_20 = local_20 + 0x28;
    } while (local_14 < *(uint *)(param_1 + 200));
  }
  return param_1;
}

