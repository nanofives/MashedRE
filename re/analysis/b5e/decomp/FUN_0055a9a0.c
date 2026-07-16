
/* [C1 2026-05-19] Variant of 0x0055a1f0 inner phase 3 — generates pair candidates for a list of
   shapes (`param_2` array of `param_3` body… */

int FUN_0055a9a0(uint param_1,int param_2,uint param_3,int param_4,int param_5,uint param_6)

{
  uint *puVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  int local_10;
  uint local_8;

  iVar5 = param_1;
  local_10 = 0;
  if (param_6 == 0) {
    return 0;
  }
  uVar6 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      uVar6 = uVar6 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x70) + -4 + uVar6 * 4) = 0;
    } while (uVar6 < *(uint *)(param_1 + 4));
  }
  local_8 = 0;
  if (param_3 != 0) {
    do {
      uVar2 = *(ushort *)(*(int *)(param_2 + local_8 * 4) + 0x20);
      uVar6 = FUN_00564040(*(undefined4 *)(iVar5 + 0x54),uVar2,*(undefined4 *)(iVar5 + 0x80),
                           *(undefined4 *)(iVar5 + 0x70),*(uint *)(iVar5 + 8) & 1);
      puVar1 = (uint *)(*(int *)(iVar5 + 0x70) + (uint)(uVar2 >> 5) * 4);
      param_1 = 0;
      *puVar1 = *puVar1 | 1 << ((byte)uVar2 & 0x1f);
      if (uVar6 != 0) {
        puVar8 = (undefined4 *)(param_4 + 8 + local_10 * 0x14);
        do {
          uVar3 = *(ushort *)(*(int *)(iVar5 + 0x80) + param_1 * 2);
          uVar7 = 1 << ((byte)uVar3 & 0x1f);
          iVar10 = (uint)(uVar3 >> 5) * 4;
          puVar9 = puVar8;
          if (((*(uint *)(iVar10 + *(int *)(iVar5 + 0x70)) & uVar7) == 0) &&
             ((param_6 == 0xf ||
              ((param_6 &
               2 - (uint)((*(uint *)(iVar10 + *(int *)(iVar5 + 100)) & uVar7) != 0) <<
               (('\x01' - ((*(uint *)(iVar10 + *(int *)(iVar5 + 0x5c)) & uVar7) != 0)) * '\x02' &
               0x1fU)) != 0)))) {
            puVar9 = puVar8 + 5;
            puVar8[-2] = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar2 * 4);
            *(undefined2 *)(puVar8 + -1) = 0x8000;
            uVar4 = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar3 * 4);
            *(undefined2 *)(puVar8 + 1) = 0x8000;
            *puVar8 = uVar4;
            puVar8[2] = 0;
            local_10 = local_10 + 1;
            if (local_10 == param_5) {
              return local_10;
            }
          }
          param_1 = param_1 + 1;
          puVar8 = puVar9;
        } while (param_1 < uVar6);
      }
      local_8 = local_8 + 1;
    } while (local_8 < param_3);
  }
  return local_10;
}

