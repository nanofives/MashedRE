// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412cf0 FUN_00412cf0
/* h2 0x00412cf0 (313 B). Best: 99.0% (regs 99.0, ops 99.0), 1 instruction pair. Start (g2s) 46.9%.
   Sibling of 0x00412e30 with a FUN_004726f0(&a.dir copy, &b.dir copy) term (struct copies of +0x20, clamped
   to <= 0) driving alpha = f*f*255*alpha. Steps: Body typed { pad 0x20; V3 dir; pad; V3 pos } (pos at +0x30);
   d = a - b via the assign-b-then-subtract form (fsubr); constant-store order f18, f14, f1c, f20 (from all 24
   permutations, 93.9 -> 98.0); alpha * (f * f * 255.0f) (-> 99.0).
   Residual: `fstp [len]` after the FUN_004c3ac0 call lands after `add esi,0x20` in ours, before it in the
   original (0x00412d33). Tried copy order, declaration order, struct-member copies, double len.
   Class: (b) one-instruction scheduling. */
typedef struct { float x, y, z; } V3;
typedef struct {
  V3 pos;                      /* +0x00 */
  float size;                  /* +0x0c */
  float f10;                   /* +0x10 */
  float f14, f18, f1c, f20;    /* +0x14..+0x20 */
  unsigned char r, g, b, a;    /* +0x24..+0x27 */
  float life;                  /* +0x28 */
} Particle;
typedef struct { char p0[0x20]; V3 dir; float pad2c; V3 pos; } Body;
#ifndef MATCH_PORT
float FUN_004c3ac0(V3 *v);
float FUN_004726f0(V3 *a, V3 *b);
#endif
void FUN_00412cf0(Particle *out, Body *a, Body *b, float alpha, float t, float life,
                  unsigned char *rgb, int *count)
{
  V3 d;
  V3 vb;
  V3 va;
  float len;
  float f;

  if (*count < 0x7f) {
    d.x = b->pos.x;
    d.x = a->pos.x - d.x;
    d.y = b->pos.y;
    d.y = a->pos.y - d.y;
    d.z = b->pos.z;
    d.z = a->pos.z - d.z;
    len = FUN_004c3ac0(&d);
    vb = b->dir;
    va = a->dir;
    f = FUN_004726f0(&va, &vb);
    if (f > 0.0f) f = 0.0f;
    d.x *= t;
    d.y *= t;
    d.z *= t;
    out->pos.x = d.x + a->pos.x;
    out->pos.y = d.y + a->pos.y;
    out->pos.z = d.z + a->pos.z;
    out->f18 = 0.0f;
    out->f14 = 1.0f;
    out->f1c = 1.0f;
    out->f20 = 0.0f;
    out->size = len * 0.025f + 0.05f;
    out->r = rgb[0];
    out->g = rgb[1];
    out->b = rgb[2];
    out->a = (unsigned char)(alpha * (f * f * 255.0f));
    out->life = life;
    (*count)++;
  }
}
