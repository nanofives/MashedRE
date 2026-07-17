
/* [C1 2026-05-19] Marks the first stack-of-arena byte: `**(this+0xd8) = *(this+0x7c)` and
   `*(*(this+0xd8) + 4) = 0`. */

int FUN_0055ff90(int param_1)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint local_10;
  uint local_c;
  int local_8;

  **(undefined4 **)(param_1 + 0xd8) = *(undefined4 *)(param_1 + 0x7c);
  *(undefined4 *)(*(int *)(param_1 + 0xd8) + 4) = 0;
  FUN_00567c60(param_1 + 0xc4,*(undefined4 *)(param_1 + 0xd8),*(undefined4 *)(param_1 + 0xdc),
               *(undefined4 *)(param_1 + 0xe0),*(undefined4 *)(param_1 + 0x74),
               *(undefined4 *)(param_1 + 0x78),*(undefined4 *)(param_1 + 0xb4),
               *(undefined4 *)(param_1 + 0xb8),*(undefined4 *)(param_1 + 0x8c),
               *(undefined4 *)(param_1 + 0x90));
  local_c = 0;
  if (*(int *)(param_1 + 200) == 0) {
    return param_1;
  }
  local_8 = 0;
  do {
    uVar7 = 0;
    *(uint *)(local_8 + 0x18 + *(int *)(param_1 + 0xd4)) =
         *(uint *)(local_8 + 0x18 + *(int *)(param_1 + 0xd4)) & 0xfffffffe;
    uVar2 = *(uint *)(local_8 + 0xc + *(int *)(param_1 + 0xd4));
    puVar6 = (undefined4 *)(local_8 + *(int *)(param_1 + 0xd4));
    if (uVar2 != 0) {
      piVar5 = (int *)*puVar6;
      do {
        uVar1 = *(ushort *)(*piVar5 + 0x20);
        if ((*(uint *)(*(int *)(*(int *)(*piVar5 + 0x24) + 0x5c) + (uint)(uVar1 >> 5) * 4) &
            1 << ((byte)uVar1 & 0x1f)) != 0) goto LAB_005601b3;
        uVar7 = uVar7 + 1;
        piVar5 = piVar5 + 1;
      } while (uVar7 < uVar2);
    }
    for (iVar3 = puVar6[1]; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0xdc)) {
      if (*(short *)(iVar3 + 0xb8) != -1) {
        iVar4 = *(int *)(*(int *)(iVar3 + 0xb4) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar3 + 0xb4) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_005601b3;
      }
      if (*(short *)(iVar3 + 0xc0) != -1) {
        iVar4 = *(int *)(*(int *)(iVar3 + 0xbc) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar3 + 0xbc) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_005601b3;
      }
    }
    local_10 = 0;
    if (puVar6[4] != 0) {
      piVar5 = (int *)puVar6[2];
      do {
        iVar3 = *piVar5;
        if (*(short *)(iVar3 + 0x54) != -1) {
          iVar4 = *(int *)(*(int *)(iVar3 + 0x50) + 0x24);
          if ((iVar4 != 0) &&
             (uVar1 = *(ushort *)(*(int *)(iVar3 + 0x50) + 0x20),
             (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)
             ) != 0)) goto LAB_005601b3;
        }
        if (*(short *)(iVar3 + 0x5c) != -1) {
          iVar4 = *(int *)(*(int *)(iVar3 + 0x58) + 0x24);
          if ((iVar4 != 0) &&
             (uVar1 = *(ushort *)(*(int *)(iVar3 + 0x58) + 0x20),
             (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)
             ) != 0)) goto LAB_005601b3;
        }
        piVar5 = piVar5 + 1;
        local_10 = local_10 + 1;
      } while (local_10 < (uint)puVar6[4]);
    }
    puVar6[6] = puVar6[6] | 1;
LAB_005601b3:
    local_c = local_c + 1;
    local_8 = local_8 + 0x28;
    if (*(uint *)(param_1 + 200) <= local_c) {
      return param_1;
    }
  } while( true );
}

