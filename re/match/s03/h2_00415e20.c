// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415e20 FUN_00415e20
/* h2 0x00415e20 (Ghidra 551 B, real 567 B). Best: 93.8% (regs 93.8, ops 93.8), --bytes 18 of 567 B differ.
   Start (g2s) 37.1%. Returns float (ST0). 0x004a3384 = CRT acos called with a double on the stack
   (declared as an extern double fn so /Oi does not turn it into _CIacos; the port can call acos only if it
   keeps the non-intrinsic form). 0x005cd0d0 / 0x005cd0c8 are double -1.0 / 1.0 (c < -1.0 compares with a
   double literal), 0x005cc970 = (double)57.29578f, 0x005ccac4 = 360.0f, 0x005d757c = 0.0f.
   Steps: the dot v.x*1 + v.y*0 + v.z*0 is factored by cl to (v.y + v.z)*0 + v.x unless written
   `v.z * 0.0f + (v.y * 0.0f + v.x)`; with an extern zero instead of the literal cl loads the constant first
   (fld K / fmul v), so literal 0.0f. Sign test written `-(v.x * 0.0f - v.z) < 0.0f` (93.8%) because the
   plain `v.z - v.x * 0.0f` gives fsubr (90.7%).
   Residual (both halves, 4 instructions): original 0x00415ec7 fld v.z / fld v.x / fmul 0 / fsubp (v.z first),
   ours fld v.x / fmul 0 / fsub v.z / fchs. Tried v.z*1.0f - ..., -(x) + z, z + -(x), 0.0f*v.x, extern zero,
   (v.z*0 + v.z) - ...: none loads v.z first. Class: (b) unknown source form (same x87 operand-order family
   as the pilot's fsubr residue and h2 0x00412e30); no evidence it is compiler-version. */
typedef struct { float x, y, z; } V3;
#ifndef MATCH_PORT
int FUN_0046d4a0(int *out, int car);
int FUN_0046d510(V3 *out, int car);
float FUN_004c39b0(V3 *out, V3 *in);
double FUN_004a3384(double x);
#endif
float FUN_00415e20(int car, float tx, float tz)
{
  int rec;
  V3 v;
  float c;
  float a1;
  float a2;
  float d;

  FUN_0046d4a0(&rec, car);
  v.x = tx - *(float *)(rec + 0x30);
  v.y = 0.0f;
  v.z = -(tz - *(float *)(rec + 0x38));
  FUN_004c39b0(&v, &v);
  c = v.z * 0.0f + (v.y * 0.0f + v.x);
  if (c < -1.0) c = -1.0f;
  if (c > 1.0) c = 1.0f;
  a1 = (float)(FUN_004a3384(c) * 57.29578f);
  if (-(v.x * 0.0f - v.z) < 0.0f) a1 = -a1;
  while (a1 <= 0.0f) a1 += 360.0f;

  FUN_0046d510(&v, car);
  v.z = -v.z;
  v.y = 0.0f;
  FUN_004c39b0(&v, &v);
  c = v.z * 0.0f + (v.y * 0.0f + v.x);
  if (c < -1.0) c = -1.0f;
  if (c > 1.0) c = 1.0f;
  a2 = (float)(FUN_004a3384(c) * 57.29578f);
  if (-(v.x * 0.0f - v.z) < 0.0f) a2 = -a2;
  while (a2 <= 0.0f) a2 += 360.0f;

  d = a1 - a2;
  while (d <= 0.0f) d += 360.0f;
  while (d < 0.0f) d += 360.0f;
  while (d > 360.0f) d -= 360.0f;
  return d;
}
