
/* [C1 2026-05-19] **Remove primitive `param_2` from the broadphase octree** (counterpart to the
   INSERT at FUN_00564c80). */

void FUN_00565260(int param_1,uint param_2)

{
  uint *puVar1;
  ushort uVar2;
  uint uVar3;
  ushort uVar4;
  int iVar5;
  ushort uVar6;
  uint uVar7;
  ushort *puVar8;
  ushort *puVar9;
  uint local_8;
  ushort *local_4;

  uVar3 = param_2;
  uVar7 = param_2 & 0xffff;
  FUN_00563f60(*(undefined4 *)(param_1 + 0xc010),uVar7);
  uVar6 = *(ushort *)(param_1 + 0x8020 + uVar7 * 2);
  uVar2 = uVar6 & 0x3f;
  uVar6 = uVar6 >> 6;
  uVar7 = (uint)uVar6;
  param_2 = uVar7;
  if ((uVar2 == 0x3f) || (uVar2 == 0xffff)) {
    puVar1 = (uint *)(param_1 + (uVar7 * 5 + 0x2607) * 4);
    puVar8 = (ushort *)&param_2;
    param_2 = (ushort)*puVar1 & 0x3ff;
    uVar2 = *(ushort *)(param_1 + 0x881e + param_2 * 4);
    while (((uVar2 & 0x3ff) != (ushort)uVar3 && ((short)param_2 != 0x3ff))) {
      puVar8 = (ushort *)(param_1 + 0x8820 + param_2 * 4);
      param_2 = (uint)*(ushort *)(param_1 + 0x8820 + param_2 * 4);
      uVar2 = *(ushort *)(param_1 + 0x881e + param_2 * 4);
    }
    uVar2 = *puVar8;
    uVar3 = (uint)uVar2;
    if ((uint *)puVar8 == &param_2) {
      puVar9 = (ushort *)(param_1 + 0x8820 + uVar3 * 4);
      *puVar1 = *puVar1 & 0xfffffc00 | (uint)*(ushort *)(param_1 + 0x8820 + uVar3 * 4);
    }
    else {
      puVar9 = (ushort *)(param_1 + 0x8820 + uVar3 * 4);
      *puVar8 = *(ushort *)(param_1 + 0x8820 + uVar3 * 4);
    }
  }
  else {
    puVar1 = &local_8;
    puVar8 = (ushort *)(param_1 + 0x9820 + ((uint)uVar2 + uVar7 * 10) * 2);
    local_4 = puVar8;
    local_8 = *puVar8 & 0x3ff;
    uVar2 = *(ushort *)(param_1 + 0x881e + local_8 * 4);
    while (((uVar2 & 0x3ff) != (ushort)uVar3 && ((short)local_8 != 0x3ff))) {
      puVar1 = (uint *)(param_1 + 0x8820 + local_8 * 4);
      local_8 = (uint)*(ushort *)(param_1 + 0x8820 + local_8 * 4);
      uVar2 = *(ushort *)(param_1 + 0x881e + local_8 * 4);
    }
    uVar2 = (ushort)*puVar1;
    puVar9 = (ushort *)(param_1 + 0x8820 + (uint)uVar2 * 4);
    if (puVar1 == &local_8) {
      *puVar8 = *puVar8 & 0xfc00 | *puVar9;
    }
    else {
      *(ushort *)puVar1 = *puVar9;
    }
    if ((*(byte *)(param_1 + 0x881f + (uint)uVar2 * 4) & 0x80) != 0) {
      uVar4 = *puVar8;
      if ((uVar4 & 0x7c00) == 0x7c00) {
        iVar5 = 0;
        uVar3 = uVar4 & 0x3ff;
        if ((short)uVar3 != 0x3ff) {
          while (iVar5 < 0x1f) {
            puVar8 = local_4;
            if ((*(byte *)(param_1 + 0x881f + uVar3 * 4) & 0x80) != 0) {
              iVar5 = iVar5 + 1;
            }
          }
        }
        uVar4 = uVar4 & 0x83ff | (ushort)(iVar5 << 10);
      }
      else {
        uVar4 = uVar4 - 0x400;
      }
      *puVar8 = uVar4;
    }
  }
  *puVar9 = *(ushort *)(param_1 + 0x981a);
  *(ushort *)(param_1 + 0x981a) = uVar2;
  if ((uVar6 != 0) &&
     (iVar5 = uVar7 * 5 + 0x2607, puVar1 = (uint *)(param_1 + iVar5 * 4),
     (*(ushort *)(param_1 + iVar5 * 4) & 0x3ff) == 0x3ff)) {
    uVar3 = 0;
    iVar5 = param_1 + 0x9820 + uVar7 * 0x14;
    do {
      if ((*(byte *)(iVar5 + 1) & 0x80) == 0) {
        return;
      }
      if ((*(ushort *)(param_1 + 0x9820 + ((uVar3 & 0xffff) + uVar7 * 10) * 2) & 0x3ff) != 0x3ff) {
        return;
      }
      uVar3 = uVar3 + 1;
      iVar5 = iVar5 + 2;
    } while ((int)uVar3 < 8);
    uVar3 = *puVar1;
    *(undefined2 *)(param_1 + 0x9820 + ((uVar3 >> 0x14 & 7) + (uVar3 >> 10 & 0x3ff) * 10) * 2) =
         0x83ff;
    *puVar1 = (uint)*(ushort *)(param_1 + 0xc00e);
    *(ushort *)(param_1 + 0xc00e) = uVar6;
  }
  return;
}

