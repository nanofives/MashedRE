
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-06-01] `void __thiscall FUN_004427c0(undefined4 param_1, int param_2)` â€”
   spectator-camera */

void __thiscall FUN_004427c0(undefined4 param_1,int param_2)

{
  float fVar1;
  int iVar2;
  uint uVar3;
  float *pfVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined4 *puVar9;
  undefined4 local_4;
  
  iVar2 = param_2;
  DAT_00897fc0 = DAT_00897fc0 + 1;
  uVar3 = DAT_00897fc0 & 0xfff;
  local_4 = param_1;
  if (DAT_007f0fd0 == 5) {
    iVar5 = 0;
  }
  else if ((DAT_007f0fd0 == 10) &&
          ((FUN_0046cbb0(0,&param_2,&local_4), param_2 != 0 || (iVar5 = FUN_0046c7b0(0), iVar5 == 0)
           ))) {
    iVar5 = 0;
  }
  else if ((DAT_007f0fd0 == 8) && (iVar5 = FUN_00417740(0), iVar5 == -1)) {
    iVar5 = 0;
  }
  else {
    iVar5 = FUN_0046c7b0(*(undefined4 *)(iVar2 + 0x20));
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(&DAT_005f9550 + ((int)uVar3 >> 8) * 4);
      if (iVar5 == 0) {
        iVar5 = 0;
      }
      else if (iVar5 == 2) {
        iVar5 = DAT_0089899c;
        if ((DAT_00897fc0 & 0x100) != 0) {
          iVar5 = DAT_008964a4;
        }
      }
      else if ((iVar5 != 3) ||
              (iVar5 = DAT_00898998,
              *(float *)(&DAT_00896588 + DAT_00898998 * 0xd8) <= _DAT_005cc730)) {
        fVar1 = *(float *)(&DAT_00896588 + DAT_008964a0 * 0xd8);
        iVar5 = DAT_008964a0;
        iVar6 = DAT_008964a0;
        if (3 < (DAT_0089649c - DAT_008964a0) + 1) {
          iVar8 = DAT_008964a0 + 3;
          pfVar4 = (float *)(&DAT_00896660 + DAT_008964a0 * 0xd8);
          do {
            if (pfVar4[-0x36] < fVar1) {
              fVar1 = pfVar4[-0x36];
              iVar5 = iVar6;
            }
            if (*pfVar4 < fVar1) {
              iVar5 = iVar8 + -2;
              fVar1 = *pfVar4;
            }
            if (pfVar4[0x36] < fVar1) {
              iVar5 = iVar8 + -1;
              fVar1 = pfVar4[0x36];
            }
            if (pfVar4[0x6c] < fVar1) {
              fVar1 = pfVar4[0x6c];
              iVar5 = iVar8;
            }
            iVar8 = iVar8 + 4;
            iVar6 = iVar6 + 4;
            pfVar4 = pfVar4 + 0xd8;
          } while (iVar8 < DAT_0089649c);
        }
        if (iVar6 <= DAT_0089649c) {
          pfVar4 = (float *)(&DAT_00896588 + iVar6 * 0xd8);
          do {
            if (*pfVar4 < fVar1) {
              fVar1 = *pfVar4;
              iVar5 = iVar6;
            }
            iVar6 = iVar6 + 1;
            pfVar4 = pfVar4 + 0x36;
          } while (iVar6 <= DAT_0089649c);
        }
      }
    }
  }
  DAT_006831d4 = DAT_006831d4 + 1;
  iVar6 = iVar5;
  if (((iVar5 != DAT_006831cc) &&
      (DAT_006831d0 = DAT_006831d0 + 1, iVar6 = DAT_006831cc, 0xf < DAT_006831d0)) &&
     (0x3c < DAT_006831d4)) {
    DAT_006831d0 = 0;
    DAT_006831d4 = 0;
    iVar6 = iVar5;
    DAT_006831cc = iVar5;
  }
  DAT_00896498 = (&DAT_0089658c)[iVar6 * 0x36];
  *(undefined4 *)(iVar2 + 0x58) = (&DAT_00896580)[iVar6 * 0x36];
  FUN_00441700(iVar2);
  puVar7 = &DAT_0089650c + iVar6 * 0x36;
  puVar9 = (undefined4 *)(*(int *)(*(int *)(iVar2 + 0x84) + 4) + 0x10);
  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar9 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar9 = puVar9 + 1;
  }
  return;
}

