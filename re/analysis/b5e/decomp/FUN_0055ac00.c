
void FUN_0055ac00(int param_1,int param_2,int param_3)

{
  uint *puVar1;
  ushort uVar2;

  uVar2 = *(ushort *)(param_2 + 0x20);
  if (param_3 != 0) {
    puVar1 = (uint *)(*(int *)(param_1 + 0x5c) + (uint)(uVar2 >> 5) * 4);
    *puVar1 = *puVar1 | 1 << ((byte)uVar2 & 0x1f);
    return;
  }
  puVar1 = (uint *)(*(int *)(param_1 + 0x5c) + (uint)(uVar2 >> 5) * 4);
  *puVar1 = *puVar1 & ~(1 << ((byte)uVar2 & 0x1f));
  return;
}

