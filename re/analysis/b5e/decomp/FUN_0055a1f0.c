
/* [C1 2026-05-19] Broad-phase tick / collision-pair generator. Takes 10 args: world-state ptr,
   output pair array, max pairs, &count… */

int FUN_0055a1f0(uint param_1,int param_2,uint param_3,uint *param_4,int *param_5,uint param_6,
                uint *param_7,int param_8,uint param_9,uint *param_10)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  ushort uVar6;
  uint uVar7;
  int iVar8;
  uint *puVar9;
  uint uVar10;
  int *piVar11;
  int *piVar12;
  int *piVar13;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  int *local_8;

  iVar2 = param_1;
  uVar10 = 0;
  uVar3 = 0;
  local_1c = 0;
  local_14 = 0;
  local_18 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      uVar3 = uVar3 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x70) + -4 + uVar3 * 4) = 0;
    } while (uVar3 < *(uint *)(param_1 + 4));
  }
  uVar3 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      if (*(int *)(*(int *)(param_1 + 0x5c) + uVar3 * 4) != 0) {
        uVar5 = 0;
        uVar7 = 1;
        do {
          if ((*(uint *)(*(int *)(param_1 + 0x5c) + uVar3 * 4) & uVar7) != 0) {
            *(short *)(*(int *)(param_1 + 0x7c) + uVar10 * 2) = (short)uVar3 * 0x20 + (short)uVar5;
            uVar10 = uVar10 + 1;
          }
          uVar5 = uVar5 + 1;
          uVar7 = uVar7 * 2;
        } while (uVar5 < 0x20);
      }
      *(undefined4 *)(*(int *)(param_1 + 0x74) + uVar3 * 4) =
           *(undefined4 *)(*(int *)(param_1 + 0x5c) + uVar3 * 4);
      uVar3 = uVar3 + 1;
      local_1c = uVar10;
    } while (uVar3 < *(uint *)(param_1 + 4));
  }
  local_10 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(param_1 + 0x7c) + local_10 * 2);
      uVar3 = (uint)uVar1;
      iVar8 = *(int *)(*(int *)(param_1 + 0x50) + uVar3 * 4);
      iVar4 = *(int *)(iVar8 + 0x24);
      if ((iVar4 != 0) &&
         (((*(uint *)(*(int *)(iVar4 + 100) + (uint)(*(ushort *)(iVar8 + 0x20) >> 5) * 4) &
           1 << ((byte)*(ushort *)(iVar8 + 0x20) & 0x1f)) != 0 ||
          ((iVar4 != 0 &&
           ((*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(*(ushort *)(iVar8 + 0x20) >> 5) * 4) &
            1 << ((byte)*(ushort *)(iVar8 + 0x20) & 0x1f)) != 0)))))) {
        puVar9 = (uint *)(*(int *)(param_1 + 0x70) + (uint)(uVar1 >> 5) * 4);
        *puVar9 = *puVar9 | 1 << ((byte)uVar1 & 0x1f);
        uVar10 = FUN_00564040(*(undefined4 *)(param_1 + 0x54),uVar1,*(undefined4 *)(param_1 + 0x80),
                              *(undefined4 *)(param_1 + 0x70),*(uint *)(param_1 + 8) & 1);
        local_8 = (int *)0x0;
        if (uVar10 != 0) {
          piVar12 = param_5 + local_14 * 5 + 3;
          do {
            uVar6 = *(ushort *)(*(int *)(param_1 + 0x80) + (int)local_8 * 2);
            if (local_14 < param_6) {
              *(undefined2 *)(piVar12 + -2) = 0x8000;
              piVar12[-3] = *(int *)(*(int *)(param_1 + 0x50) + uVar3 * 4);
              *(undefined2 *)piVar12 = 0x8000;
              iVar8 = *(int *)(*(int *)(param_1 + 0x50) + (uint)uVar6 * 4);
              piVar12[1] = 0;
              piVar12[-1] = iVar8;
              local_14 = local_14 + 1;
              piVar12 = piVar12 + 5;
            }
            if (local_1c < *(uint *)(param_1 + 0x44)) {
              uVar7 = 1 << ((byte)uVar6 & 0x1f);
              iVar8 = (uint)(uVar6 >> 5) * 4;
              if ((*(uint *)(iVar8 + *(int *)(param_1 + 0x74)) & uVar7) == 0) {
                *(ushort *)(*(int *)(param_1 + 0x7c) + local_1c * 2) = uVar6;
                puVar9 = (uint *)(iVar8 + *(int *)(param_1 + 0x74));
                local_1c = local_1c + 1;
                *puVar9 = *puVar9 | uVar7;
              }
            }
            local_8 = (int *)((int)local_8 + 1);
          } while (local_8 < uVar10);
        }
        if (*(int *)(param_1 + 0x20) != 0) {
          for (piVar12 = *(int **)(*(int *)(param_1 + 0x20) + uVar3 * 4); piVar12 != (int *)0x0;
              piVar12 = (int *)piVar12[1]) {
            iVar8 = *piVar12;
            if (*(short *)(iVar8 + 0x54) == -1) {
LAB_0055a502:
              if (local_18 < param_9) {
                *(int *)(param_8 + local_18 * 4) = iVar8;
LAB_0055a515:
                local_18 = local_18 + 1;
              }
            }
            else {
              iVar4 = *(int *)(*(int *)(iVar8 + 0x50) + 0x24);
              if (((iVar4 == 0) ||
                  (uVar6 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
                  (*(uint *)(*(int *)(iVar4 + 100) + (uint)(uVar6 >> 5) * 4) &
                  1 << ((byte)uVar6 & 0x1f)) == 0)) || (*(short *)(iVar8 + 0x5c) == -1))
              goto LAB_0055a502;
              iVar4 = *(int *)(*(int *)(iVar8 + 0x58) + 0x24);
              if ((iVar4 == 0) ||
                 (uVar6 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
                 (*(uint *)(*(int *)(iVar4 + 100) + (uint)(uVar6 >> 5) * 4) &
                 1 << ((byte)uVar6 & 0x1f)) == 0)) goto LAB_0055a502;
              uVar6 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20) ^
                      *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20) ^ uVar1;
              if (local_1c < *(uint *)(param_1 + 0x44)) {
                uVar3 = 1 << ((byte)uVar6 & 0x1f);
                iVar4 = (uint)(uVar6 >> 5) * 4;
                if ((*(uint *)(iVar4 + *(int *)(param_1 + 0x74)) & uVar3) == 0) {
                  local_1c = local_1c + 1;
                  *(ushort *)(*(int *)(param_1 + 0x7c) + -2 + local_1c * 2) = uVar6;
                  puVar9 = (uint *)(iVar4 + *(int *)(param_1 + 0x74));
                  *puVar9 = *puVar9 | uVar3;
                }
              }
              if ((uVar1 < uVar6) && (local_18 < param_9)) {
                *(int *)(param_8 + local_18 * 4) = iVar8;
                goto LAB_0055a515;
              }
            }
          }
        }
      }
      local_10 = local_10 + 1;
    } while (local_10 < local_1c);
  }
  param_1 = local_18;
  uVar3 = 0;
  param_6 = local_14;
  if (*(int *)(iVar2 + 4) != 0) {
    do {
      uVar3 = uVar3 + 1;
      *(undefined4 *)(*(int *)(iVar2 + 0x78) + -4 + uVar3 * 4) = 0;
    } while (uVar3 < *(uint *)(iVar2 + 4));
  }
  uVar3 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + uVar3 * 2);
      uVar10 = (uint)(uVar1 >> 5);
      uVar3 = uVar3 + 1;
      *(uint *)(*(int *)(iVar2 + 0x78) + uVar10 * 4) =
           *(uint *)(*(int *)(iVar2 + 0x78) + uVar10 * 4) | 1 << ((byte)uVar1 & 0x1f);
    } while (uVar3 < local_1c);
  }
  param_9 = 0;
  if (*(int *)(iVar2 + 4) != 0) {
    local_8 = (int *)0x0;
    do {
      uVar3 = ~*(uint *)(*(int *)(iVar2 + 0x78) + param_9 * 4) &
              *(uint *)(*(int *)(iVar2 + 0x60) + param_9 * 4);
      if (uVar3 != 0) {
        local_c = 1;
        local_10 = 0;
        do {
          if ((local_c & uVar3) != 0) {
            piVar12 = *(int **)(*(int *)(iVar2 + 0x50) + ((int)local_8 + local_10) * 4);
            if ((*(byte *)(piVar12 + 2) & 2) != 0) {
              if ((piVar12[9] != 0) &&
                 ((*(uint *)(*(int *)(piVar12[9] + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4
                            ) & 1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f)) != 0)) {
                (**(code **)(*piVar12 + 0x14))(piVar12);
                puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4
                                 );
                *puVar9 = *puVar9 & ~(1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f));
                *(int *)(iVar2 + 0x4c) = *(int *)(iVar2 + 0x4c) + -1;
              }
              puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + ((int)local_8 + local_10 >> 5) * 4);
              *puVar9 = *puVar9 & ~(1 << ((byte)local_10 & 0x1f));
            }
          }
          local_10 = local_10 + 1;
          local_c = local_c * 2;
        } while (local_10 < 0x20);
      }
      param_9 = param_9 + 1;
      local_8 = (int *)((int)local_8 + 0x20);
    } while (param_9 < *(uint *)(iVar2 + 4));
  }
  param_9 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + param_9 * 2);
      iVar8 = (uint)(uVar1 >> 5) * 4;
      uVar3 = 1 << ((byte)uVar1 & 0x1f);
      if ((((*(uint *)(iVar8 + *(int *)(iVar2 + 0x78)) & uVar3) != 0) &&
          ((*(uint *)(iVar8 + *(int *)(iVar2 + 0x60)) & uVar3) == 0)) &&
         ((*(uint *)(iVar8 + *(int *)(iVar2 + 100)) & uVar3) != 0)) {
        local_8 = (int *)0x0;
        piVar12 = *(int **)(*(int *)(iVar2 + 0x50) + (uint)uVar1 * 4);
        if (((piVar12[9] == 0) ||
            (piVar11 = piVar12,
            (*(uint *)(*(int *)(piVar12[9] + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4) &
            1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f)) == 0)) &&
           (piVar11 = local_8, *(uint *)(iVar2 + 0x4c) < *(uint *)(iVar2 + 0x48))) {
          iVar4 = (**(code **)(*piVar12 + 0x10))(piVar12);
          if (iVar4 == 0) {
            if (DAT_007dc8c0 == 0) {
              DAT_007dc8c0 = 1;
            }
          }
          else {
            puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4);
            *puVar9 = *puVar9 | 1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f);
            DAT_007dc8c0 = 0;
            *(int *)(iVar2 + 0x4c) = *(int *)(iVar2 + 0x4c) + 1;
            piVar11 = piVar12;
          }
        }
        if (piVar11 != (int *)0x0) {
          puVar9 = (uint *)(iVar8 + *(int *)(iVar2 + 0x60));
          *puVar9 = *puVar9 | uVar3;
        }
      }
      param_9 = param_9 + 1;
    } while (param_9 < local_1c);
  }
  uVar3 = 0;
  param_9 = 0;
  if (local_1c != 0) {
    do {
      if (param_3 <= uVar3) break;
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + param_9 * 2);
      if ((*(uint *)(*(int *)(iVar2 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
          != 0) {
        uVar3 = uVar3 + 1;
        *(undefined4 *)(param_2 + -4 + uVar3 * 4) =
             *(undefined4 *)(*(int *)(iVar2 + 0x50) + (uint)uVar1 * 4);
      }
      param_9 = param_9 + 1;
    } while (param_9 < local_1c);
  }
  param_9 = 0;
  *param_4 = uVar3;
  if (local_14 != 0) {
    piVar12 = param_5 + local_14 * 5;
    do {
      iVar8 = *(int *)(*param_5 + 0x24);
      if ((iVar8 == 0) ||
         (uVar1 = *(ushort *)(*param_5 + 0x20),
         (*(uint *)(*(int *)(iVar8 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) ==
         0)) {
        iVar8 = *(int *)(param_5[2] + 0x24);
        if ((iVar8 != 0) &&
           (uVar1 = *(ushort *)(param_5[2] + 0x20),
           (*(uint *)(*(int *)(iVar8 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_0055a897;
        param_6 = param_6 - 1;
        piVar12 = piVar12 + -5;
        piVar11 = piVar12;
        piVar13 = param_5;
        for (iVar8 = 5; iVar8 != 0; iVar8 = iVar8 + -1) {
          *piVar13 = *piVar11;
          piVar11 = piVar11 + 1;
          piVar13 = piVar13 + 1;
        }
      }
      else {
LAB_0055a897:
        param_9 = param_9 + 1;
        param_5 = param_5 + 5;
      }
    } while (param_9 < param_6);
  }
  uVar3 = 0;
  *param_7 = param_6;
  if (local_18 == 0) {
    *param_10 = 0;
    return iVar2;
  }
  do {
    iVar8 = *(int *)(param_8 + uVar3 * 4);
    if (*(short *)(iVar8 + 0x54) == -1) {
LAB_0055a920:
      if (*(short *)(iVar8 + 0x5c) != -1) {
        iVar4 = *(int *)(*(int *)(iVar8 + 0x58) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_0055a94f;
      }
      param_1 = param_1 - 1;
      *(undefined4 *)(param_8 + uVar3 * 4) = *(undefined4 *)(param_8 + param_1 * 4);
    }
    else {
      iVar4 = *(int *)(*(int *)(iVar8 + 0x50) + 0x24);
      if ((iVar4 == 0) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
         (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) ==
         0)) goto LAB_0055a920;
LAB_0055a94f:
      uVar3 = uVar3 + 1;
    }
    if (param_1 <= uVar3) {
      *param_10 = param_1;
      return iVar2;
    }
  } while( true );
}

