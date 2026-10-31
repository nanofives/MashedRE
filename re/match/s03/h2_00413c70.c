// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413c70 FUN_00413c70
/* h2 0x00413c70 (59 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 68.4%.
   Table at 0x0063bc60 typed as Ent12[][5] (12-byte entries: float +0, int +8), row*5+col indexing;
   0x3c23d70a = 0.01f stored when entry.a == DAT_005d757c. Class: none (exact). */
typedef struct { float a; int b; int c; } Ent12;
#ifndef MATCH_PORT
extern Ent12 DAT_0063bc60[][5];
extern float DAT_005d757c;
#endif
void FUN_00413c70(int row, int col, int v)
{
  DAT_0063bc60[row][col].c = v;
  if (DAT_0063bc60[row][col].a == DAT_005d757c) DAT_0063bc60[row][col].a = 0.01f;
}
