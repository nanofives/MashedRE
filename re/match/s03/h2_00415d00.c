// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415d00 FUN_00415d00
/* h2 0x00415d00 (282 B). Best: 100% BYTE-IDENTICAL (--bytes). Start (g2s) 77.1%.
   Locals declared in frame order (t, len, inv, rec, p[2], dir[2], probe[2], V3 v); `0.0f < len` operand order;
   1.0f / 0.25f literals. The dir = (v + p) - p round trip needs a separate end-point temp e[2]
   (e = v + p; dir = e - p): the one-expression form gave 96.4% (no interleave). Class: none (exact). */
typedef struct { float x, y, z; } V3;
#ifndef MATCH_PORT
int FUN_0046d4a0(int *out, int car);
int FUN_0046d510(V3 *out, int car);
float FUN_004c3bf0(float *v);
char FUN_00443d10(float x, float z);
#endif
int FUN_00415d00(int car)
{
  float t;
  float len;
  float inv;
  int rec;
  float p[2];
  float dir[2];
  float probe[2];
  float e[2];
  V3 v;
  char c;

  FUN_0046d4a0(&rec, car);
  p[0] = *(float *)(rec + 0x30);
  p[1] = *(float *)(rec + 0x38);
  FUN_0046d510(&v, car);
  v.x = v.x + v.x;
  t = 0.0f;
  v.y = v.y + v.y;
  v.z = v.z + v.z;
  e[0] = v.x + p[0];
  e[1] = v.z + p[1];
  dir[0] = e[0] - p[0];
  dir[1] = e[1] - p[1];
  len = FUN_004c3bf0(dir);
  if (0.0f < len) {
    inv = 1.0f / len;
    do {
      probe[0] = inv * dir[0] * t + p[0];
      probe[1] = inv * dir[1] * t + p[1];
      c = FUN_00443d10(probe[0], probe[1]);
      if (c == 0 || c == 3) return 1;
      t += 0.25f;
    } while (t < len);
  }
  return 0;
}
