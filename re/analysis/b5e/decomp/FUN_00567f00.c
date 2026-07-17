
/* [C1 2026-05-19] Signature: `void FUN_00567f00(undefined4 param_1, int param_2)` (other args
   passed on stack — Ghidra rendering quirk). */

void FUN_00567f00(undefined4 param_1,int param_2)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  int *piVar9;
  int *piVar10;
  int iVar11;
  int *piVar12;
  int *piVar13;
  bool bVar14;
  int in_stack_00000018;
  uint in_stack_0000001c;
  int in_stack_00000020;
  int in_stack_00000024;
  uint local_4;

  local_4 = 0;
  if (in_stack_0000001c != 0) {
    do {
      iVar8 = *(int *)(in_stack_00000018 + local_4 * 4);
      if ((*(short *)(iVar8 + 0x54) == -1) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
         (*(uint *)(*(int *)(*(int *)(*(int *)(iVar8 + 0x50) + 0x24) + 0x60) +
                   (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) == 0)) {
        iVar11 = 0;
      }
      else {
        iVar11 = 1;
      }
      if ((*(short *)(iVar8 + 0x5c) == -1) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
         (*(uint *)(*(int *)(*(int *)(*(int *)(iVar8 + 0x58) + 0x24) + 0x60) +
                   (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) == 0)) {
        iVar3 = 0;
      }
      else {
        iVar3 = 1;
      }
      if (iVar11 == iVar3) {
        iVar3 = *(int *)(iVar8 + 0x58);
        iVar11 = iVar8 + 8;
        piVar5 = (int *)((uint)*(ushort *)(*(int *)(iVar8 + 0x50) + 0x20) * 0x10 + param_2);
        piVar4 = (int *)((uint)*(ushort *)(iVar3 + 0x20) * 0x10 + param_2);
        bVar14 = *piVar5 == 0;
        iVar2 = *piVar4;
        if (bVar14) {
          piVar5[3] = *(int *)(iVar8 + 0x50);
        }
        if (iVar2 == 0) {
          piVar4[3] = iVar3;
        }
        if (bVar14 == (iVar2 == 0)) {
          if (bVar14) {
            FUN_005684c0(param_1,piVar5,piVar4,iVar11,0);
          }
          else {
            piVar7 = (int *)*piVar5;
            if (piVar7 == (int *)0x0) {
              iVar8 = 0;
            }
            else if (piVar7 == piVar5) {
              iVar8 = piVar5[1];
            }
            else {
              piVar6 = piVar7;
              piVar10 = (int *)*piVar7;
              if (piVar7 != (int *)*piVar7) {
                do {
                  piVar6 = piVar10;
                  piVar10 = (int *)*piVar6;
                } while (piVar6 != (int *)*piVar6);
              }
              if (piVar5 != piVar7) {
                do {
                  *piVar5 = (int)piVar6;
                  bVar14 = piVar7 != (int *)*piVar7;
                  piVar5 = piVar7;
                  piVar7 = (int *)*piVar7;
                } while (bVar14);
              }
              iVar8 = piVar6[1];
            }
            piVar5 = (int *)*piVar4;
            if (piVar5 == (int *)0x0) {
              iVar3 = 0;
            }
            else if (piVar5 == piVar4) {
              iVar3 = piVar4[1];
            }
            else {
              piVar7 = piVar5;
              piVar6 = (int *)*piVar5;
              if (piVar5 != (int *)*piVar5) {
                do {
                  piVar7 = piVar6;
                  piVar6 = (int *)*piVar7;
                } while (piVar7 != (int *)*piVar7);
              }
              if (piVar4 != piVar5) {
                do {
                  *piVar4 = (int)piVar7;
                  bVar14 = piVar5 != (int *)*piVar5;
                  piVar4 = piVar5;
                  piVar5 = (int *)*piVar5;
                } while (bVar14);
              }
              iVar3 = piVar7[1];
            }
            FUN_005685f0(param_1,iVar8,iVar3,iVar11,0);
          }
        }
        else if (bVar14) {
          piVar7 = (int *)*piVar4;
          if (piVar7 == (int *)0x0) {
            FUN_00568560(0,piVar4,piVar5,iVar11,0);
          }
          else if (piVar7 == piVar4) {
            FUN_00568560(piVar4[1],piVar4,piVar5,iVar11,0);
          }
          else {
            piVar6 = piVar7;
            piVar10 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar6 = piVar10;
                piVar10 = (int *)*piVar6;
              } while (piVar6 != (int *)*piVar6);
            }
            piVar10 = piVar4;
            if (piVar4 != piVar7) {
              do {
                piVar12 = piVar7;
                *piVar10 = (int)piVar6;
                piVar7 = (int *)*piVar12;
                piVar10 = piVar12;
              } while (piVar12 != (int *)*piVar12);
            }
            FUN_00568560(piVar6[1],piVar4,piVar5,iVar11,0);
          }
        }
        else {
          piVar7 = (int *)*piVar5;
          if (piVar7 == (int *)0x0) {
            FUN_00568560(0,piVar5,piVar4,iVar11,0);
          }
          else if (piVar7 == piVar5) {
            FUN_00568560(piVar5[1],piVar5,piVar4,iVar11,0);
          }
          else {
            piVar6 = piVar7;
            piVar10 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar6 = piVar10;
                piVar10 = (int *)*piVar6;
              } while (piVar6 != (int *)*piVar6);
            }
            piVar10 = piVar5;
            if (piVar5 != piVar7) {
              do {
                piVar12 = piVar7;
                *piVar10 = (int)piVar6;
                piVar7 = (int *)*piVar12;
                piVar10 = piVar12;
              } while (piVar12 != (int *)*piVar12);
            }
            FUN_00568560(piVar6[1],piVar5,piVar4,iVar11,0);
          }
        }
      }
      else {
        if (iVar11 == 0) {
          iVar11 = *(int *)(iVar8 + 0x58);
        }
        else {
          iVar11 = *(int *)(iVar8 + 0x50);
        }
        piVar4 = (int *)((uint)*(ushort *)(iVar11 + 0x20) * 0x10 + param_2);
        iVar8 = iVar8 + 8;
        piVar5 = (int *)*piVar4;
        if (piVar5 == (int *)0x0) {
          piVar4[3] = iVar11;
          FUN_005684c0(param_1,piVar4,0,iVar8,0);
        }
        else if (piVar5 == piVar4) {
          FUN_00568560(piVar4[1],0,piVar4,iVar8,0);
        }
        else {
          piVar7 = piVar5;
          piVar6 = (int *)*piVar5;
          if (piVar5 != (int *)*piVar5) {
            do {
              piVar7 = piVar6;
              piVar6 = (int *)*piVar7;
            } while (piVar7 != (int *)*piVar7);
          }
          piVar6 = piVar4;
          if (piVar4 != piVar5) {
            do {
              piVar10 = piVar5;
              *piVar6 = (int)piVar7;
              piVar5 = (int *)*piVar10;
              piVar6 = piVar10;
            } while (piVar10 != (int *)*piVar10);
          }
          FUN_00568560(piVar7[1],0,piVar4,iVar8,0);
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < in_stack_0000001c);
  }
  if (in_stack_00000024 != 0) {
    in_stack_00000018 = in_stack_00000024;
    piVar5 = (int *)(in_stack_00000020 + 0xb4);
    do {
      if (piVar5[-2] != 0) {
        if (((short)piVar5[1] == -1) ||
           (uVar1 = *(ushort *)(*piVar5 + 0x20),
           (*(uint *)(*(int *)(*piVar5 + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
           1 << ((byte)uVar1 & 0x1f)) == 0)) {
          iVar8 = 0;
        }
        else {
          iVar8 = 1;
        }
        if (((short)piVar5[3] == -1) ||
           (uVar1 = *(ushort *)(piVar5[2] + 0x20),
           (*(uint *)(*(int *)(piVar5[2] + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
           1 << ((byte)uVar1 & 0x1f)) == 0)) {
          iVar11 = 0;
        }
        else {
          iVar11 = 1;
        }
        if (iVar8 == iVar11) {
          iVar8 = piVar5[2];
          piVar6 = piVar5 + 8;
          piVar4 = (int *)((uint)*(ushort *)(*piVar5 + 0x20) * 0x10 + param_2);
          piVar7 = (int *)((uint)*(ushort *)(iVar8 + 0x20) * 0x10 + param_2);
          bVar14 = *piVar4 == 0;
          iVar11 = *piVar7;
          if (bVar14) {
            piVar4[3] = *piVar5;
          }
          if (iVar11 == 0) {
            piVar7[3] = iVar8;
          }
          if (bVar14 == (iVar11 == 0)) {
            if (bVar14) {
              FUN_005684c0(param_1,piVar4,piVar7,0,piVar6);
            }
            else {
              piVar10 = (int *)*piVar4;
              if (piVar10 == (int *)0x0) {
                iVar8 = 0;
              }
              else if (piVar10 == piVar4) {
                iVar8 = piVar4[1];
              }
              else {
                piVar12 = piVar10;
                piVar13 = (int *)*piVar10;
                if (piVar10 != (int *)*piVar10) {
                  do {
                    piVar12 = piVar13;
                    piVar13 = (int *)*piVar12;
                  } while (piVar12 != (int *)*piVar12);
                }
                if (piVar4 != piVar10) {
                  do {
                    *piVar4 = (int)piVar12;
                    bVar14 = piVar10 != (int *)*piVar10;
                    piVar4 = piVar10;
                    piVar10 = (int *)*piVar10;
                  } while (bVar14);
                }
                iVar8 = piVar12[1];
              }
              piVar4 = (int *)*piVar7;
              if (piVar4 == (int *)0x0) {
                iVar11 = 0;
              }
              else if (piVar4 == piVar7) {
                iVar11 = piVar7[1];
              }
              else {
                piVar10 = piVar4;
                piVar12 = (int *)*piVar4;
                if (piVar4 != (int *)*piVar4) {
                  do {
                    piVar10 = piVar12;
                    piVar12 = (int *)*piVar10;
                  } while (piVar10 != (int *)*piVar10);
                }
                if (piVar7 != piVar4) {
                  do {
                    *piVar7 = (int)piVar10;
                    bVar14 = piVar4 != (int *)*piVar4;
                    piVar7 = piVar4;
                    piVar4 = (int *)*piVar4;
                  } while (bVar14);
                }
                iVar11 = piVar10[1];
              }
              FUN_005685f0(param_1,iVar8,iVar11,0,piVar6);
            }
          }
          else if (bVar14) {
            piVar10 = (int *)*piVar7;
            if (piVar10 == (int *)0x0) {
              FUN_00568560(0,piVar7,piVar4,0,piVar6);
            }
            else if (piVar10 == piVar7) {
              FUN_00568560(piVar7[1],piVar7,piVar4,0,piVar6);
            }
            else {
              piVar12 = piVar10;
              piVar13 = (int *)*piVar10;
              if (piVar10 != (int *)*piVar10) {
                do {
                  piVar12 = piVar13;
                  piVar13 = (int *)*piVar12;
                } while (piVar12 != (int *)*piVar12);
              }
              piVar13 = piVar7;
              if (piVar7 != piVar10) {
                do {
                  piVar9 = piVar10;
                  *piVar13 = (int)piVar12;
                  piVar10 = (int *)*piVar9;
                  piVar13 = piVar9;
                } while (piVar9 != (int *)*piVar9);
              }
              FUN_00568560(piVar12[1],piVar7,piVar4,0,piVar6);
            }
          }
          else {
            piVar10 = (int *)*piVar4;
            if (piVar10 == (int *)0x0) {
              FUN_00568560(0,piVar4,piVar7,0,piVar6);
            }
            else if (piVar10 == piVar4) {
              FUN_00568560(piVar4[1],piVar4,piVar7,0,piVar6);
            }
            else {
              piVar12 = piVar10;
              piVar13 = (int *)*piVar10;
              if (piVar10 != (int *)*piVar10) {
                do {
                  piVar12 = piVar13;
                  piVar13 = (int *)*piVar12;
                } while (piVar12 != (int *)*piVar12);
              }
              piVar13 = piVar4;
              if (piVar4 != piVar10) {
                do {
                  piVar9 = piVar10;
                  *piVar13 = (int)piVar12;
                  piVar10 = (int *)*piVar9;
                  piVar13 = piVar9;
                } while (piVar9 != (int *)*piVar9);
              }
              FUN_00568560(piVar12[1],piVar4,piVar7,0,piVar6);
            }
          }
        }
        else {
          if (iVar8 == 0) {
            iVar8 = piVar5[2];
          }
          else {
            iVar8 = *piVar5;
          }
          piVar6 = (int *)((uint)*(ushort *)(iVar8 + 0x20) * 0x10 + param_2);
          piVar4 = piVar5 + 8;
          piVar7 = (int *)*piVar6;
          if (piVar7 == (int *)0x0) {
            piVar6[3] = iVar8;
            FUN_005684c0(param_1,piVar6,0,0,piVar4);
          }
          else if (piVar7 == piVar6) {
            FUN_00568560(piVar6[1],0,piVar6,0,piVar4);
          }
          else {
            piVar10 = piVar7;
            piVar12 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar10 = piVar12;
                piVar12 = (int *)*piVar10;
              } while (piVar10 != (int *)*piVar10);
            }
            piVar12 = piVar6;
            if (piVar6 != piVar7) {
              do {
                piVar13 = piVar7;
                *piVar12 = (int)piVar10;
                piVar7 = (int *)*piVar13;
                piVar12 = piVar13;
              } while (piVar13 != (int *)*piVar13);
            }
            FUN_00568560(piVar10[1],0,piVar6,0,piVar4);
          }
        }
      }
      piVar5 = piVar5 + 0x38;
      in_stack_00000018 = in_stack_00000018 + -1;
    } while (in_stack_00000018 != 0);
  }
  return;
}

