
/* [C1 2026-05-20] Signature recovered: `uint FUN_00575fe0(World *world, ContactNode *node, uint
   subkey, Shape *shape,… */

uint FUN_00575fe0(int param_1,undefined4 *param_2,undefined4 param_3,int param_4,undefined4 *param_5
                 ,int param_6,undefined4 *param_7,float *param_8,float *param_9,uint param_10)

{
  short *psVar1;
  float fVar2;
  uint uVar3;
  undefined4 *puVar4;
  uint uVar5;
  float *pfVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  float *pfVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  float *pfVar13;
  undefined4 *puVar14;
  float *pfVar15;
  uint local_30;
  int local_2c;
  float local_20;
  float local_1c;
  float local_18;
  float local_10;
  float local_c;
  float local_8;

  uVar5 = ((int)param_7 - *(int *)(param_1 + 0xc)) / 0x14;
  uVar3 = (int)param_5 - *(int *)(param_1 + 0x18) >> 6;
  *(uint *)(param_1 + 0x10) = uVar5;
  *(uint *)(param_1 + 0x1c) = uVar3;
  if ((uVar3 < *(uint *)(param_1 + 0x20)) && (uVar5 < *(uint *)(param_1 + 0x14))) {
    *(uint *)(param_1 + 0x1c) = uVar3 + 1;
    psVar1 = *(short **)(param_4 + 0x5c);
    if ((*(byte *)(psVar1 + 0x20) & 1) == 0) {
      if (*psVar1 != 0) {
        param_7[4] = param_5;
        *param_7 = *param_2;
        param_7[1] = param_2[1];
        *(undefined2 *)(param_7 + 1) = (undefined2)param_3;
        param_7[2] = param_4;
        param_7[3] = param_6;
        *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
        return 1;
      }
    }
    else {
      fVar2 = *(float *)(param_1 + 0x70);
      param_6 = param_6 * 0x10000;
      if (*psVar1 == 8) {
        local_30 = 0;
        puVar9 = (undefined4 *)**(undefined4 **)(param_4 + 0x40);
        uVar5 = (*(undefined4 **)(param_4 + 0x40))[1];
        puVar11 = param_7;
        if (uVar5 != 0) {
          while( true ) {
            iVar8 = puVar9[0x17];
            if ((*(uint *)(param_1 + 0x20) <= *(uint *)(param_1 + 0x1c)) ||
               (*(uint *)(param_1 + 0x14) <= *(uint *)(param_1 + 0x10))) break;
            puVar4 = (undefined4 *)(*(uint *)(param_1 + 0x1c) * 0x40 + *(int *)(param_1 + 0x18));
            pfVar6 = (float *)(*(uint *)(param_1 + 0x10) * 0x20 + *(int *)(param_1 + 0x24));
            if ((*(byte *)(iVar8 + 0x40) & 2) == 0) {
              if (param_5 == (undefined4 *)0x0) {
                puVar12 = puVar9;
                puVar14 = puVar4;
                for (iVar7 = 0x10; puVar11 = param_7, iVar7 != 0; iVar7 = iVar7 + -1) {
                  *puVar14 = *puVar12;
                  puVar12 = puVar12 + 1;
                  puVar14 = puVar14 + 1;
                }
              }
              else {
                FUN_004c4600(puVar4,puVar9,param_5);
              }
            }
            else {
              puVar12 = param_5;
              puVar14 = puVar4;
              for (iVar7 = 0x10; puVar11 = param_7, iVar7 != 0; iVar7 = iVar7 + -1) {
                *puVar14 = *puVar12;
                puVar12 = puVar12 + 1;
                puVar14 = puVar14 + 1;
              }
            }
            if (param_8 == (float *)0x0) {
              (**(code **)(iVar8 + 0x10))(puVar9,puVar4,0,pfVar6);
              if (((uVar5 < 2) || (pfVar6 == param_9)) ||
                 ((pfVar6[4] - *param_9 < fVar2 != (pfVar6[4] - *param_9 == fVar2) &&
                  ((((param_9[4] - *pfVar6 < fVar2 != (param_9[4] - *pfVar6 == fVar2) &&
                     (pfVar6[5] - param_9[1] < fVar2 != (pfVar6[5] - param_9[1] == fVar2))) &&
                    (param_9[5] - pfVar6[1] < fVar2 != (param_9[5] - pfVar6[1] == fVar2))) &&
                   ((pfVar6[6] - param_9[2] < fVar2 != (pfVar6[6] - param_9[2] == fVar2) &&
                    (param_9[6] - pfVar6[2] < fVar2 != (param_9[6] - pfVar6[2] == fVar2)))))))))
              goto LAB_005761fb;
            }
            else {
              pfVar13 = param_8;
              for (iVar8 = 8; puVar11 = param_7, iVar8 != 0; iVar8 = iVar8 + -1) {
                *pfVar6 = *pfVar13;
                pfVar13 = pfVar13 + 1;
                pfVar6 = pfVar6 + 1;
              }
LAB_005761fb:
              FUN_00575fe0(param_1,param_2,param_3,puVar9,puVar4,local_30 + 1 + param_6,puVar11,
                           param_8,param_9,param_10);
              puVar11 = (undefined4 *)(*(int *)(param_1 + 0xc) + *(int *)(param_1 + 0x10) * 0x14);
              param_7 = puVar11;
            }
            local_30 = local_30 + 1;
            puVar9 = puVar9 + 0x18;
            if (uVar5 <= local_30) {
              return uVar5;
            }
          }
          *(undefined4 *)(param_1 + 0x78) = 1;
        }
        return uVar5;
      }
      local_20 = fVar2 + *param_9;
      local_1c = param_9[1] + fVar2;
      local_18 = param_9[2] + fVar2;
      local_10 = param_9[4] - fVar2;
      local_c = param_9[5] - fVar2;
      local_8 = param_9[6] - fVar2;
      if ((*(byte *)(psVar1 + 0x20) & 4) == 0) {
        FUN_00566200(&local_20,&local_20,param_5);
        uVar5 = (**(code **)(param_4 + 0x44))(param_4,&local_20,param_10,(int *)(param_1 + 0xc));
        iVar8 = *(int *)(param_1 + 0xc);
        local_2c = (((int)param_7 - iVar8) / 0x14) * 0x20 + *(int *)(param_1 + 0x24);
        local_30 = 0;
        puVar11 = param_7;
        param_7 = (undefined4 *)(iVar8 + *(int *)(param_1 + 0x10) * 0x14);
        if (uVar5 != 0) {
          while (*(uint *)(param_1 + 0x1c) < *(uint *)(param_1 + 0x20)) {
            puVar9 = (undefined4 *)puVar11[2];
            puVar4 = (undefined4 *)(*(uint *)(param_1 + 0x1c) * 0x40 + *(int *)(param_1 + 0x18));
            iVar8 = puVar9[0x17];
            *puVar11 = *param_2;
            *(undefined2 *)(puVar11 + 1) = (undefined2)param_3;
            puVar11[4] = puVar4;
            puVar11[3] = puVar11[3] + param_6 + 1;
            if ((*(byte *)(iVar8 + 0x40) & 2) == 0) {
              if (param_5 == (undefined4 *)0x0) {
                puVar12 = puVar9;
                puVar14 = puVar4;
                for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
                  *puVar14 = *puVar12;
                  puVar12 = puVar12 + 1;
                  puVar14 = puVar14 + 1;
                }
              }
              else {
                FUN_004c4600(puVar4,puVar9,param_5);
              }
            }
            else {
              puVar12 = param_5;
              puVar14 = puVar4;
              for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
                *puVar14 = *puVar12;
                puVar12 = puVar12 + 1;
                puVar14 = puVar14 + 1;
              }
            }
            (**(code **)(iVar8 + 0x10))(puVar9,puVar4,0,local_2c);
            if ((*(byte *)(iVar8 + 0x40) & 1) == 0) {
              *(int *)(param_1 + 0x1c) = *(int *)(param_1 + 0x1c) + 1;
            }
            else {
              FUN_00575fe0(param_1,param_2,param_3,puVar9,puVar4,puVar11[3],param_7,0,param_9,
                           param_10);
              iVar8 = *(int *)(param_1 + 0x10);
              iVar7 = *(int *)(param_1 + 0xc);
              puVar11[2] = 1;
              param_7 = (undefined4 *)(iVar7 + iVar8 * 0x14);
            }
            local_30 = local_30 + 1;
            puVar11 = puVar11 + 5;
            local_2c = local_2c + 0x20;
            if (uVar5 <= local_30) {
              return uVar5;
            }
          }
          *(undefined4 *)(param_1 + 0x78) = 1;
        }
        return uVar5;
      }
      iVar8 = (**(code **)(param_4 + 0x48))
                        (param_4,param_5,&local_20,param_10,(undefined4 *)(param_1 + 0x44));
      if (iVar8 != 0) {
        pfVar6 = *(float **)(param_1 + 0x44);
        param_9 = (float *)0x0;
        param_10 = 0;
        if (*(int *)(param_1 + 0x48) == 0) {
          return 0;
        }
        do {
          if (((uint)pfVar6[0x12] & 0x10) == 0) {
            uVar5 = *(uint *)(param_1 + 0x5c);
            if ((*(uint *)(param_1 + 0x60) <= uVar5) ||
               (*(uint *)(param_1 + 0x14) <= *(uint *)(param_1 + 0x10))) {
              *(undefined4 *)(param_1 + 0x78) = 1;
              return (uint)param_9;
            }
            pfVar10 = (float *)(uVar5 * 0x60 + *(int *)(param_1 + 0x58));
            *(uint *)(param_1 + 0x5c) = uVar5 + 1;
            pfVar13 = pfVar6;
            pfVar15 = pfVar10;
            for (iVar8 = 0x13; iVar8 != 0; iVar8 = iVar8 + -1) {
              *pfVar15 = *pfVar13;
              pfVar13 = pfVar13 + 1;
              pfVar15 = pfVar15 + 1;
            }
            pfVar10[0x17] = (float)&DAT_005e5db0;
            pfVar10[0x13] = *(float *)(param_4 + 0x4c);
            pfVar10[0x14] = *(float *)(param_4 + 0x50);
            pfVar10[0x15] = *(float *)(param_4 + 0x54);
            *(undefined2 *)((int)pfVar10 + 0x5a) = *(undefined2 *)(param_4 + 0x5a);
            if ((*(byte *)(param_1 + 0x50) & 2) != 0) {
              pfVar13 = pfVar10 + 0xc;
              *pfVar13 = (pfVar6[10] - pfVar6[2]) * pfVar6[5] +
                         (pfVar6[2] - pfVar6[6]) * pfVar6[9] + (pfVar6[6] - pfVar6[10]) * pfVar6[1];
              pfVar10[0xd] = (*pfVar6 - pfVar6[4]) * pfVar6[10] +
                             (pfVar6[8] - *pfVar6) * pfVar6[6] + (pfVar6[4] - pfVar6[8]) * pfVar6[2]
              ;
              pfVar10[0xe] = (pfVar6[9] - pfVar6[1]) * pfVar6[4] +
                             (pfVar6[5] - pfVar6[9]) * *pfVar6 + (pfVar6[1] - pfVar6[5]) * pfVar6[8]
              ;
              FUN_005667c0(pfVar13,pfVar13);
            }
            pfVar10[7] = pfVar10[0xe] * pfVar6[2] +
                         pfVar10[0xc] * *pfVar6 + pfVar10[0xd] * pfVar6[1];
            iVar8 = *(int *)(param_1 + 0x10);
            iVar7 = *(int *)(param_1 + 0x24);
            puVar11 = (undefined4 *)(*(int *)(param_1 + 0xc) + iVar8 * 0x14);
            *puVar11 = *param_2;
            puVar11[1] = param_2[1];
            *(undefined2 *)(puVar11 + 1) = (undefined2)param_3;
            puVar11[2] = pfVar10;
            puVar11[4] = param_5;
            FUN_0055bd70(pfVar10,param_5,0,iVar8 * 0x20 + iVar7);
            puVar11[3] = (int)pfVar6[0x11] + param_6;
            param_9 = (float *)((int)param_9 + 1);
            *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
          }
          param_10 = param_10 + 1;
          pfVar6 = pfVar6 + 0x13;
          if (*(uint *)(param_1 + 0x48) <= param_10) {
            return (uint)param_9;
          }
        } while( true );
      }
    }
  }
  else {
    *(undefined4 *)(param_1 + 0x78) = 1;
  }
  return 0;
}

