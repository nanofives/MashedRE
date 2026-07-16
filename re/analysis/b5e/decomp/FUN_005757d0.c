
/* [C1 2026-05-20] Signature recovered: `bool FUN_005757d0(int *body_pair_ptrs, int pair)` —
   narrow-phase dispatch helper. */

bool FUN_005757d0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  float local_c;
  float local_8;
  float local_4;

  local_c = -*(float *)(param_2 + 0x100);
  local_8 = -*(float *)(param_2 + 0x104);
  iVar2 = *(int *)(param_2 + 0x50);
  local_4 = -*(float *)(param_2 + 0x108);
  iVar1 = FUN_0055c230(*(undefined4 *)(*param_1 + 8),*(undefined4 *)(*param_1 + 0x10),0,&local_c,
                       param_2);
  bVar3 = false;
  if (iVar1 != 0) {
    *(int *)(param_2 + 0xd0) = iVar2;
    if (4 < *(ushort *)(param_2 + 0x54)) {
      *(uint *)(param_2 + 0xd0) = (uint)*(ushort *)(param_2 + 0x54) * 0x10 + iVar2;
    }
    iVar2 = FUN_0055c230(*(undefined4 *)(param_1[1] + 8),*(undefined4 *)(param_1[1] + 0x10),1,
                         param_2 + 0x100,param_2 + 0x80);
    bVar3 = iVar2 != 0;
  }
  return bVar3;
}

