// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417450 FUN_00417450
/* h2 0x00417450 (210 B). */
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
