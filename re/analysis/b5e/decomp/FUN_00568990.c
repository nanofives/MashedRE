
/* [C1 2026-05-19] Signature: `uint FUN_00568990(int param_1, int param_2, uint param_3, int
   param_4, uint param_5)`. */

uint FUN_00568990(int param_1,int param_2,uint param_3,int param_4,uint param_5)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  short *psVar8;
  uint uVar9;
  int *piVar10;
  undefined4 *puVar11;
  uint local_54;
  uint local_50;
  uint local_48;
  undefined1 local_40 [32];
  undefined1 local_20 [32];

  local_54 = 0;
  if ((param_3 == 0) || (param_5 == 0)) {
    return 0;
  }
  piVar10 = (int *)(param_1 + 0x37c);
  piVar2 = (int *)**(int **)(param_1 + 0x70);
  *(undefined4 *)(param_1 + 0x3ec) = *(undefined4 *)(*piVar2 + 0xc014);
  *(int *)(param_1 + 0x3f0) = piVar2[3];
  uVar3 = *(undefined4 *)(*piVar2 + 0xc018);
  local_50 = 0;
  uVar7 = 0;
  if (param_3 != 0) {
    do {
      uVar7 = local_50;
      if (param_5 <= local_54) {
        return local_54;
      }
      *(undefined4 *)(param_1 + 0x3ac) = 0;
      *(undefined4 *)(param_1 + 0x3b8) = 0;
      *(undefined4 *)(param_1 + 0x3f4) = 0;
      if (local_50 < param_3) {
        puVar11 = (undefined4 *)(param_2 + 8 + local_50 * 0x14);
        do {
          if (*(int *)(param_1 + 0x3f4) != 0) break;
          puVar1 = puVar11 + -2;
          if (*(short *)(puVar11 + -1) == -1) {
            FUN_0055bd80(*puVar1,0,0,local_40);
          }
          else {
            FUN_0055abb0(piVar2,*puVar1,local_40);
          }
          if (*(short *)(puVar11 + 1) == -1) {
            FUN_0055bd80(*puVar11,0,0,local_20);
          }
          else {
            FUN_0055abb0(piVar2,*puVar11,local_20);
          }
          *(undefined4 *)(param_1 + 0x380) = 0;
          *(undefined4 *)(param_1 + 0x398) = 0;
          *(undefined4 *)(param_1 + 0x38c) = 0;
          *(undefined4 *)(param_1 + 0x3d8) = 0;
          iVar4 = FUN_00575c60(piVar10,puVar1,0,local_40,local_20,puVar11);
          if ((iVar4 != 0) &&
             (iVar5 = FUN_00575c60(piVar10,puVar11,0,local_20,local_40,puVar1), iVar5 != 0)) {
            uVar6 = FUN_00576640(piVar10,iVar4,iVar5,puVar11[2],uVar3);
            if ((*(int *)(param_1 + 0x3f4) != 0) && (uVar7 < local_50)) break;
            uVar9 = 0;
            if (uVar6 != 0) {
              do {
                FUN_00575880(piVar10,param_1,*piVar10 + uVar9 * 8,*(undefined4 *)(param_1 + 0x3ec));
                uVar9 = uVar9 + 1;
              } while (uVar9 < uVar6);
            }
          }
          local_50 = local_50 + 1;
          puVar11 = puVar11 + 5;
        } while (local_50 < param_3);
      }
      uVar7 = *(uint *)(param_1 + 0x3ac);
      iVar4 = *(int *)(param_1 + 0x3a8);
      local_48 = 0;
      if (uVar7 != 0) {
        do {
          if (param_5 <= local_54) break;
          uVar6 = 1;
          if (1 < uVar7 - local_48) {
            psVar8 = (short *)(iVar4 + 0x17c);
            do {
              if ((((*(short *)(iVar4 + 0x5c) != -1) || (*psVar8 != -1)) &&
                  ((*(int *)(iVar4 + 0x58) != *(int *)(psVar8 + -2) ||
                   (*(short *)(iVar4 + 0x5c) != *psVar8)))) ||
                 (((*(short *)(iVar4 + 0xdc) != -1 || (psVar8[0x40] != -1)) &&
                  ((*(int *)(iVar4 + 0xd8) != *(int *)(psVar8 + 0x3e) ||
                   (*(short *)(iVar4 + 0xdc) != psVar8[0x40])))))) break;
              uVar6 = uVar6 + 1;
              psVar8 = psVar8 + 0x90;
            } while (uVar6 < uVar7 - local_48);
          }
          iVar5 = FUN_00575560(piVar10,iVar4,uVar6,local_54 * 0xe0 + param_4,param_5 - local_54);
          local_54 = local_54 + iVar5;
          local_48 = local_48 + uVar6;
          iVar4 = iVar4 + uVar6 * 0x120;
          uVar7 = *(uint *)(param_1 + 0x3ac);
        } while (local_48 < uVar7);
      }
      uVar7 = local_54;
    } while (local_50 < param_3);
  }
  return uVar7;
}

