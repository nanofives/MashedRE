
/* [C1 2026-05-20] Signature recovered: `void FUN_00579ee0(int *simplex)` — incremental Gram
   (cross-product) matrix update… */

void FUN_00579ee0(float *param_1)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  float *pfVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  float *local_40;
  int local_38;
  float *local_34;
  float *local_30;
  float *local_28;
  float *local_24;
  float *local_20;
  float *local_1c;
  uint local_14;

  iVar7 = (int)param_1;
  iVar5 = *(int *)((int)param_1 + 0x1a8);
  uVar6 = *(uint *)((int)param_1 + 0x1ac);
  local_34 = (float *)((int)param_1 + 0x60);
  *(undefined4 *)((int)param_1 + 0xa0 + (iVar5 + uVar6 * 4) * 4) = 0x3f800000;
  uVar10 = *(uint *)((int)param_1 + 0x1a4);
  local_38 = 0;
  local_14 = 1;
  if (0 < (int)uVar10) {
    pfVar1 = local_34 + iVar5;
    local_30 = pfVar1;
    local_20 = local_34;
    local_1c = local_34;
    do {
      if ((local_14 & uVar10) != 0) {
        iVar8 = (local_14 | uVar6) * 4;
        pfVar3 = (float *)(iVar7 + 0xa0 + (iVar8 + iVar5) * 4);
        param_1 = (float *)(iVar7 + 0x60);
        pfVar4 = (float *)(iVar7 + 0xa0 + (iVar8 + local_38) * 4);
        *pfVar3 = *local_34 - *local_30;
        iVar8 = local_38 + iVar5 * 4;
        pfVar2 = param_1 + iVar8;
        uVar13 = 1;
        iVar12 = 0;
        *pfVar4 = param_1[iVar5 * 5] - param_1[iVar8];
        uVar10 = *(uint *)(iVar7 + 0x1a4);
        if (0 < (int)uVar10) {
          local_40 = local_20;
          pfVar9 = param_1 + iVar5 * 4;
          local_24 = local_1c;
          local_28 = pfVar1;
          do {
            if ((uVar13 & uVar10) != 0) {
              if ((int)local_14 <= (int)uVar13) break;
              uVar10 = uVar13 | local_14;
              iVar8 = (uVar10 | uVar6) * 4;
              *(float *)(iVar7 + 0xa0 + (iVar5 + iVar8) * 4) =
                   (*local_40 - *local_30) * *(float *)(iVar7 + 0xa0 + (local_38 + uVar10 * 4) * 4)
                   + (*param_1 - *local_28) * *(float *)(iVar7 + 0xa0 + (iVar12 + uVar10 * 4) * 4);
              iVar11 = (uVar13 | uVar6) * 4;
              *(float *)(iVar7 + 0xa0 + (local_38 + iVar8) * 4) =
                   (*pfVar9 - *pfVar2) * *(float *)(iVar7 + 0xa0 + (iVar11 + iVar5) * 4) +
                   (*param_1 - *local_24) * *(float *)(iVar7 + 0xa0 + (iVar11 + iVar12) * 4);
              *(float *)(iVar7 + 0xa0 + (iVar8 + iVar12) * 4) =
                   (*local_34 - *local_40) * *pfVar4 + (*pfVar2 - *pfVar9) * *pfVar3;
            }
            param_1 = param_1 + 5;
            uVar13 = uVar13 * 2;
            local_28 = local_28 + 4;
            iVar12 = iVar12 + 1;
            local_24 = local_24 + 4;
            local_40 = local_40 + 1;
            pfVar9 = pfVar9 + 1;
            uVar10 = *(uint *)(iVar7 + 0x1a4);
          } while ((int)uVar13 <= (int)uVar10);
        }
      }
      local_34 = local_34 + 5;
      local_14 = local_14 * 2;
      local_20 = local_20 + 4;
      local_30 = local_30 + 4;
      uVar10 = *(uint *)(iVar7 + 0x1a4);
      local_38 = local_38 + 1;
      local_1c = local_1c + 1;
    } while ((int)local_14 <= (int)uVar10);
  }
  if ((*(uint *)(iVar7 + 0x1a4) | uVar6) == 0xf) {
    *(float *)(iVar7 + 400) =
         (*(float *)(iVar7 + 0x84) - *(float *)(iVar7 + 0x80)) * *(float *)(iVar7 + 0x188) +
         (*(float *)(iVar7 + 0x94) - *(float *)(iVar7 + 0x90)) * *(float *)(iVar7 + 0x18c) +
         (*(float *)(iVar7 + 0x74) - *(float *)(iVar7 + 0x70)) * *(float *)(iVar7 + 0x184);
    *(float *)(iVar7 + 0x194) =
         (*(float *)(iVar7 + 0x80) - *(float *)(iVar7 + 0x84)) * *(float *)(iVar7 + 0x178) +
         (*(float *)(iVar7 + 0x90) - *(float *)(iVar7 + 0x94)) * *(float *)(iVar7 + 0x17c) +
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 100)) * *(float *)(iVar7 + 0x170);
    *(float *)(iVar7 + 0x198) =
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 0x68)) * *(float *)(iVar7 + 0x150) +
         (*(float *)(iVar7 + 0x90) - *(float *)(iVar7 + 0x98)) * *(float *)(iVar7 + 0x15c) +
         (*(float *)(iVar7 + 0x70) - *(float *)(iVar7 + 0x78)) * *(float *)(iVar7 + 0x154);
    *(float *)(iVar7 + 0x19c) =
         (*(float *)(iVar7 + 0x60) - *(float *)(iVar7 + 0x6c)) * *(float *)(iVar7 + 0x110) +
         (*(float *)(iVar7 + 0x80) - *(float *)(iVar7 + 0x8c)) * *(float *)(iVar7 + 0x118) +
         (*(float *)(iVar7 + 0x70) - *(float *)(iVar7 + 0x7c)) * *(float *)(iVar7 + 0x114);
  }
  return;
}

