// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412e30 FUN_00412e30
/* h2 0x00412e30 (244 B). Best: 94.7% (regs 94.7, ops 97.3), --bytes see report. Start (g2s) 70.1%.
   Particle spawn (0x2c-byte record). Steps: the a - b vector subtract only gives the original
   fld [b] / fsubr [a] when b's component is first assigned into d and then d = a - d (85.3% -> 93.3%;
   -b + a, -(b - a), struct copy, __inline Sub() did not). Store order of the constant fields
   (f20, size, f14, f18, f1c, picked from all 120 permutations) -> 94.7%. 0x005cc55c/568/9a4/9a0/0x005cd04c =
   10, 100, 0.025, 0.05, 255 as literals.
   Residual (4 instructions): fmul t scheduled before `mov esi,[out]` in the original, after it in ours
   (0x00412e77), and the f18 zero store / size fstp pair swapped. Class: (b) scheduling not reached by
   statement order; permuter made no variants. */
typedef struct { float x, y, z; } V3;
typedef struct {
  V3 pos;                      /* +0x00 */
  float size;                  /* +0x0c */
  float f10;                   /* +0x10 */
  float f14, f18, f1c, f20;    /* +0x14..+0x20 */
  unsigned char r, g, b, a;    /* +0x24..+0x27 */
  float life;                  /* +0x28 */
} Particle;
typedef struct { char p0[0x30]; V3 pos; } Body;
#ifndef MATCH_PORT
float FUN_004c3ac0(V3 *v);
#endif
void FUN_00412e30(Particle *out, Body *a, Body *b, float alpha, float t, float life,
                  unsigned char *rgb, int *count)
{
  V3 d;
  float len;

  if (*count < 0x7f) {
    d.x = b->pos.x;
    d.x = a->pos.x - d.x;
    d.y = b->pos.y;
    d.y = a->pos.y - d.y;
    d.z = b->pos.z;
    d.z = a->pos.z - d.z;
    len = FUN_004c3ac0(&d);
    d.x *= t;
    d.y *= t;
    d.z *= t;
    out->pos.x = d.x + a->pos.x;
    out->pos.y = d.y + a->pos.y;
    out->pos.z = d.z + a->pos.z;
    len *= 10.0f;
    if (len > 100.0f) len = 100.0f;
    out->f20 = 0.0f;
    out->size = len * 0.025f + 0.05f;
    out->f14 = 1.0f;
    out->f18 = 0.0f;
    out->f1c = 1.0f;
    out->r = rgb[0];
    out->g = rgb[1];
    out->b = rgb[2];
    out->a = (unsigned char)(alpha * 255.0f);
    out->life = life;
    (*count)++;
  }
}
