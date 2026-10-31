// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417370 FUN_00417370
// MATCH: 0x004173b0 FUN_004173b0
/* MATCH-S03 (worker). 100% both, byte-identical. Raw converter 80%: Ghidra dropped the x87 operand of
   FUN_004a2c48 (= _ftol, takes ST0): the int is (int)(DAT_0089a360 - 10.0f), 10.0f = [0x005cc55c] (read
   from the image). Records at 0x0089a4fc, stride 0x74. */
#ifndef MATCH_PORT
extern float DAT_0089a360;
extern char DAT_0089a4fc[];
#endif

void FUN_00417370(int param_1)
{
  *(int *)(DAT_0089a4fc + param_1 * 0x74) = 0xb4 - (int)(DAT_0089a360 - 10.0f) * 0xb4;
}

void FUN_004173b0(int param_1)
{
  *(int *)(DAT_0089a4fc + param_1 * 0x74) = 0x78 - (int)(DAT_0089a360 - 10.0f) * 0x78;
}
