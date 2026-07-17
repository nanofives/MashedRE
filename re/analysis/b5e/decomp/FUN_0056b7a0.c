
/* [C1 2026-05-19] Signature: `int FUN_0056b7a0(undefined4 *param_1, int param_2, uint param_3, int
   param_4)`. */

int FUN_0056b7a0(undefined4 *param_1,int param_2,uint param_3,int param_4)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *local_54;
  uint local_50;
  int local_44;
  float fStack_40;
  float fStack_3c;
  float fStack_38;
  float fStack_30;
  float fStack_2c;
  float fStack_28;
  float local_20;
  float fStack_1c;
  float fStack_18;
  float fStack_10;
  float fStack_c;
  float fStack_8;

  iVar2 = param_1[1];
  uVar3 = *(uint *)(iVar2 + 0x84);
  if (*(uint *)(iVar2 + 0x88) <= uVar3) {
    return 0;
  }
  local_54 = (int *)param_1[2];
  local_50 = 0;
  if (param_1[3] != 0) {
    do {
      iVar4 = *local_54;
      if (iVar4 != 0) {
        local_44 = CONCAT22(local_44._2_2_,0xffff);
        FUN_0055bd80(iVar4,0,0,&local_20);
        uVar7 = 0;
        if (param_3 != 0) {
          do {
            if ((((((param_4 == 0) ||
                   (iVar6 = *(int *)(param_2 + uVar7 * 4),
                   uVar5 = FUN_0055ae50(*(undefined4 *)(iVar6 + 0x24),iVar6),
                   uVar5 = *(int *)(param_4 + 4) * (uVar5 & 0xffff) +
                           (uint)*(ushort *)(*local_54 + 0x5a),
                   (*(uint *)(param_4 + 0xc + (uVar5 >> 5) * 4) & 1 << ((byte)uVar5 & 0x1f)) == 0))
                  && (iVar6 = *(int *)(param_2 + uVar7 * 4),
                     iVar6 = FUN_0055abb0(*(undefined4 *)(iVar6 + 0x24),iVar6,&fStack_40),
                     iVar6 != 0)) &&
                 ((iVar6 = *(int *)*param_1, fStack_10 - fStack_40 <= *(float *)(iVar6 + 0xc014) &&
                  (fStack_30 - local_20 <= *(float *)(iVar6 + 0xc014))))) &&
                ((fStack_c - fStack_3c <= *(float *)(iVar6 + 0xc014) &&
                 ((fStack_2c - fStack_1c <= *(float *)(iVar6 + 0xc014) &&
                  (fStack_8 - fStack_38 <= *(float *)(iVar6 + 0xc014))))))) &&
               (fStack_28 - fStack_18 <= *(float *)(iVar6 + 0xc014))) {
              iVar6 = *(int *)(iVar2 + 0x84);
              *(int *)(iVar2 + 0x84) = iVar6 + 1;
              piVar1 = (int *)(*(int *)(iVar2 + 0x80) + iVar6 * 0x14);
              *piVar1 = iVar4;
              piVar1[1] = local_44;
              *(undefined2 *)(piVar1 + 3) = 0x8000;
              piVar1[2] = *(int *)(param_2 + uVar7 * 4);
              piVar1[4] = 0;
              if (*(int *)(iVar2 + 0x84) == *(int *)(iVar2 + 0x88)) goto LAB_0056b9ae;
            }
            uVar7 = uVar7 + 1;
          } while (uVar7 < param_3);
        }
      }
      local_50 = local_50 + 1;
      local_54 = local_54 + 1;
    } while (local_50 < (uint)param_1[3]);
  }
LAB_0056b9ae:
  return *(int *)(iVar2 + 0x84) - uVar3;
}

