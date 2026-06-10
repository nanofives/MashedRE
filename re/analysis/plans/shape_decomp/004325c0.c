
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004325c0(void)

{
  int iVar1;
  undefined *puVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  bool bVar7;
  undefined4 local_8;
  
  local_8 = 1;
  piVar6 = &DAT_00898ad0;
  do {
    iVar1 = piVar6[-4];
    if (((iVar1 != 0) && (piVar6[-3] != 0x1000)) && (DAT_0067e914 != 0)) {
      local_8 = 0;
    }
    if (iVar1 < -0xeeffff) {
      if ((iVar1 == -0xef0000) || (iVar1 == -0x1000000)) goto LAB_0043277c;
      if (iVar1 != -0xfc0000) {
        bVar7 = iVar1 == -0xf00000;
        goto LAB_0043274f;
      }
      iVar1 = piVar6[-3];
      if ((iVar1 == 0) || (iVar1 == 2)) {
        iVar5 = FUN_004a2c48();
        iVar4 = *piVar6;
        *piVar6 = iVar4 + iVar5;
        if (iVar4 + iVar5 < 1) {
          if (iVar1 == 2) {
            *piVar6 = 0x1ff;
            piVar6[-3] = 0x1000;
          }
          else {
            iVar1 = piVar6[4];
            *piVar6 = 0;
            piVar6[-3] = 1;
            iVar4 = FUN_0042ac50();
            puVar2 = (undefined *)piVar6[8];
            piVar6[3] = (int)((float)iVar4 + (float)(iVar1 * 0x1e));
            if (puVar2 == &DAT_005f7370) {
              iVar4 = FUN_0042ac50();
              piVar6[3] = (int)(((float)iVar4 + (float)(iVar1 * 0x1c)) - _DAT_005cd900);
            }
            if (puVar2 == &DAT_005f72a0) {
              piVar6[3] = (int)((float)piVar6[3] - _DAT_005cd900);
            }
          }
        }
        goto LAB_004327d7;
      }
LAB_0043265c:
      iVar4 = FUN_004a2c48();
      iVar1 = *piVar6;
      *piVar6 = iVar1 + iVar4;
      if (399 < iVar1 + iVar4) {
        *piVar6 = 0x1ff;
        piVar6[-3] = 0x1000;
      }
    }
    else if ((iVar1 == -0xee0000) || (iVar1 == -0xed0000)) {
LAB_0043277c:
      iVar1 = piVar6[-3];
      if ((iVar1 != 0) && (iVar1 != 2)) goto LAB_0043265c;
      iVar5 = FUN_004a2c48();
      iVar4 = *piVar6;
      *piVar6 = iVar4 + iVar5;
      if (iVar4 + iVar5 < 1) {
        if (iVar1 == 2) {
          *piVar6 = 0x1ff;
          piVar6[-3] = 0x1000;
        }
        else {
          *piVar6 = 0;
          piVar6[-3] = 1;
        }
      }
    }
    else {
      bVar7 = iVar1 == -0xdd0000;
LAB_0043274f:
      if (bVar7) goto LAB_0043277c;
      fVar3 = DAT_007f1004 * _DAT_005cd8fc + (float)piVar6[-2];
      piVar6[-2] = (int)fVar3;
      if (_DAT_005cd09c <= fVar3) {
        piVar6[-2] = 0x43340000;
        piVar6[-3] = 0x1000;
      }
    }
LAB_004327d7:
    piVar6 = piVar6 + 0xd;
    if (0x8990e7 < (int)piVar6) {
      return local_8;
    }
  } while( true );
}

