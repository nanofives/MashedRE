// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414490 FUN_00414490
/* h2 0x00414490 (214 B). Best: 100% BYTE-IDENTICAL (--bytes). Start (g2s) 82.6%.
   Two V3 locals (h from FUN_0046d510, d = target - pos with y zeroed), both normalised by FUN_004c39b0
   (float return discarded). The dot product only matches as `h.z*d.z + (d.y*h.y + h.x*d.x)`; all five
   unparenthesised orders gave 94.2% (h.x*d.x and h.z*d.z products swapped). Class: none (exact). */
typedef struct { float x, y, z; } V3;
#ifndef MATCH_PORT
extern int DAT_0088fc88[];
extern float DAT_005cd090;
int FUN_0040e350(void);
int FUN_0046d510(V3 *out, int car);
float FUN_004c39b0(V3 *out, V3 *in);
#endif
int FUN_00414490(int car, float *pos, int p3, float *target, int p5, float *out)
{
  V3 d;
  V3 h;

  if (FUN_0040e350() == 0xb) return 0;
  if (FUN_0040e350() == 10) return 0;
  if (DAT_0088fc88[car * 0x2d] != 0) return 0;
  FUN_0046d510(&h, car);
  d.x = target[0] - pos[0];
  h.y = 0.0f;
  d.y = 0.0f;
  d.z = target[1] - pos[1];
  FUN_004c39b0(&h, &h);
  FUN_004c39b0(&d, &d);
  if (h.z * d.z + (d.y * h.y + h.x * d.x) > DAT_005cd090) {
    out[0] = target[0];
    out[1] = target[1];
    return 1;
  }
  return 0;
}
