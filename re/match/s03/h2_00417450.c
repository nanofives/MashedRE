// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417450 FUN_00417450
/* h2 0x00417450 (210 B). Best: 73.6% (regs 73.6, ops 87.5). Start (g2s) 43.1%.
   Grid writer (counterpart to 0x004173e0). Steps: `/ 8` signed division, bounds as `<= 0x7f`, the empty-cell
   search with goto-exits (a plain inner for-loop with break gets unrolled x8 by cl, 34.7%).
   Residual: the original's outer search loop (c = 1..0x1ff) is NOT rotated: test at the top
   (0x004174c0 cmp ax,0x200 / jge fail), `inc eax / jmp top`, and the found path re-tests c >= 0x200.
   Ours always rotates the outer loop (cmp at the bottom). Tried for/break/goto, while, while(1)+if,
   explicit goto loop (w1, w2, w3), all identical 73.6%; permuter 116 compiles, no gain.
   Class: (b) unknown loop form (possibly (a): no source form tried makes cl 13.00.9176 leave this loop
   unrotated; the inner loop IS left unrotated in both). */
#ifndef MATCH_PORT
extern short DAT_007f1a9c[];
extern unsigned char DAT_007f9a9c[][8][8];
extern int DAT_00801a9c;
#endif
int FUN_00417450(int x, int y, unsigned char v)
{
  int cx;
  int cy;
  int i;
  short *cell;
  short c;

  cx = (x + 0x200) / 8;
  if (cx >= 0 && cx <= 0x7f) {
    cy = (y + 0x200) / 8;
    if (cy >= 0 && cy <= 0x7f) {
      cell = &DAT_007f1a9c[cx * 0x80 + cy];
      c = *cell;
      if (c <= 0) {
        for (c = 1; c < 0x200; c++) {
          for (i = 0; i < 0x40; i++) {
            if (((char *)DAT_007f9a9c[c])[i] != -1) goto next;
          }
          break;
next:     ;
        }
        if (c >= 0x200) return 0;
        *cell = c;
      }
      if (c > DAT_00801a9c) DAT_00801a9c = c;
      DAT_007f9a9c[c][y & 7][x & 7] = v;
      return 1;
    }
  }
  return 0;
}
