// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004173e0 FUN_004173e0
/* h2 0x004173e0 (99 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. g2s did not compile (C2064).
   Returns unsigned char (only AL is set: `or al,0xff` miss path). short cell grid at 0x007f1a9c (0x80 per row),
   byte table at 0x007f9a9c typed [cell][8][8] indexed [c][y&7][x&7]. Class: none (exact). */
#ifndef MATCH_PORT
extern short DAT_007f1a9c[];
extern unsigned char DAT_007f9a9c[][8][8];
#endif
unsigned char FUN_004173e0(int x, int y)
{
  int ix;
  int iy;
  short c;

  ix = x + 0x200;
  if (ix >= 0 && ix <= 0x400) {
    iy = y + 0x200;
    if (iy >= 0 && iy <= 0x400) {
      c = DAT_007f1a9c[(ix >> 3) * 0x80 + (iy >> 3)];
      if (c > 0 && c < 0x200) return DAT_007f9a9c[c][y & 7][x & 7];
    }
  }
  return 0xff;
}
