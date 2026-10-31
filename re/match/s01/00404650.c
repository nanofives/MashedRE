// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x00404650 (79 B) 100%. g2s 91.7%: the float test written `DAT_005d757c < DAT_007f0fe4`
   gave test ah,5/jp; `DAT_007f0fe4 > 0.0f` gives the original test ah,0x41/jne. Callees cdecl. DAT_005d757c is the .rdata 0.0f literal. */
extern int DAT_007f0fd0;
extern int DAT_00636c00;
extern float DAT_007f0fe4;
int FUN_0041f290(int p0, int p1);
int FUN_004c1480(int p0, int p1, int p2);
int FUN_004e6680(int p0);
// MATCH: 0x00404650 FUN_00404650
void FUN_00404650(void)
{
  if (DAT_007f0fd0 == 10 && DAT_00636c00 != 0 && DAT_007f0fe4 > 0.0f) {
    int v = FUN_0041f290(0, 0x16);
    FUN_004c1480(*(int *)(DAT_00636c00 + 4), v, 0);
    FUN_004e6680(DAT_00636c00);
  }
}
