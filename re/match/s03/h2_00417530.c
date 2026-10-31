// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417530 FUN_00417530
/* h2 0x00417530 (266 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 63.2%.
   Grid eraser: signed char cell table (-1 = empty, shared EBX = -1), a plain 64-iteration for loop that
   cl unrolls x8 (Ghidra showed the unrolled form), cell reset to -1 when no byte is used. Class: none (exact). */
#ifndef MATCH_PORT
extern short DAT_007f1a9c[];
extern char DAT_007f9a9c[][8][8];
#endif
int FUN_00417530(int x, int y)
{
  int cx;
  int cy;
  int i;
  int used;
  short *cell;
  short c;

  cx = (x + 0x200) / 8;
  if (cx >= 0 && cx <= 0x7f) {
    cy = (y + 0x200) / 8;
    if (cy >= 0 && cy <= 0x7f) {
      cell = &DAT_007f1a9c[cx * 0x80 + cy];
      c = *cell;
      if (c <= 0) return 1;
      DAT_007f9a9c[c][y & 7][x & 7] = -1;
      used = 0;
      for (i = 0; i < 0x40; i++) {
        if (((char *)DAT_007f9a9c[c])[i] != -1) used = 1;
      }
      if (!used) *cell = -1;
      return 1;
    }
  }
  return 0;
}
