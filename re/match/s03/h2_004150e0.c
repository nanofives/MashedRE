// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004150e0 FUN_004150e0
/* h2 0x004150e0 (176 B). Best: 100% BYTE-IDENTICAL (--bytes). Start (g2s) 55.1%.
   Ghidra hid the two rounding ftols (0x004a2c48): a = round(pos[1]*4), b = round(pos[0]*4) as
   (int)(f > 0 ? f + 0.5f : f - 0.5f). Key: 0x005cc35c (4.0f), 0x005cc32c (0.5f), 0x005d757c (0.0f) are
   compiler float literals (__real@ pool), NOT extern globals: with externs cl loads the constant first
   (fld K / fmul [pos]) and 78%; with literals it is fld [pos] / fmul K -> exact. Class: none (exact). */
#ifndef MATCH_PORT
extern short DAT_007f1a9c[];
extern unsigned char DAT_007f9a9c[][8][8];
#endif
int FUN_004150e0(float *pos)
{
  float fz;
  float fx;
  int a;
  int b;
  short c;

  fz = pos[1] * 4.0f;
  fx = pos[0] * 4.0f;
  a = (int)(fz > 0.0f ? fz + 0.5f : fz - 0.5f);
  b = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f);
  c = DAT_007f1a9c[((a + 0x1f0) >> 3) * 0x80 + ((b + 0x1f0) >> 3)];
  if (c > 0 && c < 0x200 && DAT_007f9a9c[c][b & 7][a & 7] == 2) return 1;
  return 0;
}
