// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413f20 FUN_00413f20
/* h1 MATCH-S03: BYTE-IDENTICAL over the real 43 B (Ghidra size 36 is short, ret at 0x00413f4a).
   Zeroes 4 x 5 three-dword entries at 0x0063bc60..0x0063bd50. The store order +8, +0, +4 is source
   order: writing a, b, c moves one byte. */
typedef struct E413f20 { int a, b, c; } E413f20;
#ifndef MATCH_PORT
extern E413f20 DAT_0063bc60[4][5];
#endif

void FUN_00413f20(void)
{
    int i;
    int j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 5; j++) {
            DAT_0063bc60[i][j].c = 0;
            DAT_0063bc60[i][j].a = 0;
            DAT_0063bc60[i][j].b = 0;
        }
}
