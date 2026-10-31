// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00416060 FUN_00416060
/* h2 0x00416060 (381 B). Best: 63.6% (regs 63.6, ops 96.3). Start (g2s) 50.0%.
   Line-of-sight ray march with the 0x004150e0 grid probe inlined in the loop. ops 96.3% = the right
   instructions; the regs/offset loss is one structural difference: cl 13.00.9176 hoists the loop-invariant
   inv * d[0] / inv * d[1] products out of the loop into extra frame slots (frame 0x1c vs 0x18, len and the
   rounding temp then move), while the original recomputes fld inv / fmul d / fmul t every iteration
   (0x004160c0) with len in the dead param_1 slot. 415d00 (same loop shape but with a real call inside) matched
   exactly. Tried: all-7 declaration orders (30 samples, all 63.6), d and probe as one float[4], an __inline
   grid-probe helper (worse, 25.5%). Class: (b) unknown, (a) candidate: invariant hoisting across a loop whose
   only calls are _ftol. */
#ifndef MATCH_PORT
extern short DAT_007f1a9c[];
extern unsigned char DAT_007f9a9c[][8][8];
float FUN_004c3bf0(float *v);
#endif
int FUN_00416060(float *a, float *b)
{
  float t;
  float inv;
  float probe[2];
  float d[2];
  float len;
  float fz;
  float fx;
  int ia;
  int ib;
  short c;
  unsigned char k;

  d[0] = b[0] - a[0];
  t = 0.0f;
  d[1] = b[1] - a[1];
  len = FUN_004c3bf0(d);
  if (0.0f < len) {
    inv = 1.0f / len;
    do {
      probe[0] = inv * d[0] * t + a[0];
      probe[1] = inv * d[1] * t + a[1];
      if (probe[0] != a[0] && probe[1] != a[1]) {
        fz = probe[1] * 4.0f;
        fx = probe[0] * 4.0f;
        ia = (int)(fz > 0.0f ? fz + 0.5f : fz - 0.5f);
        ib = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f);
        c = DAT_007f1a9c[((ia + 0x1f0) >> 3) * 0x80 + ((ib + 0x1f0) >> 3)];
        if (c > 0 && c < 0x200) {
          k = DAT_007f9a9c[c][ib & 7][ia & 7];
          if (k == 0 || k == 3) return 0;
        }
      }
      t += 0.25f;
    } while (t < len);
  }
  return 1;
}
