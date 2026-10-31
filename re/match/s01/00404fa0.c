// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x00404fa0 (1063 B) best 94.6% (g2s 20.1%), every byte up to the matrix tail matches.
   Keyframe camera-path sampler: Anim {+4 count, +0xc duration, +0x10 Key*}, Key = 0x24 B {+4 time, +8 RtQuat,
   +0x18 RwV3d}. Body = RtQuatSetupSlerpCache (0x00546e70) + RW RtQuatSlerp macro (Taylor sine, 1/3!..1/13!
   at 0x005cc8f4..908) + Catmull-Rom-shaped position (0x00482ae0) + RtQuatConvertToMatrix macro + RwMatrixTranslate.
   Found: search loop as `prev = cur; cur = &keys[++i];` (88%); sine written `_x + (_z * _x) * poly` (the
   `_x * _z * poly + _x` form lets 9176 factor _x out: 91.5%); |q|^2 as `w*w + (z*z + (x*x + y*y))` (94.3%);
   `if (i <= 1) k0 = keys; else ...` layouts; all 0x005cc3xx/5cc5xx/5cc8xx/5cc9xx are .rdata literals.
   Residual (natural order 94.3%; the permuter's move of pos.x up = 94.6%): the original schedules the
   RwMatrixTranslate pushes and the pos/flags integer stores between right.y and right.z; ours keeps them
   after the FP stores. Tried pos/flags before, among and after the element stores (20 placements), flags
   first, RW macro order. Class (b) source shape not found (possibly (a) scheduler). */
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d imag; float real; } RtQuat;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct { RtQuat raFrom; RtQuat raTo; float omega; int nearlyZeroOm; } RtQuatSlerpCache;
typedef struct { int unk; float time; RtQuat q; RwV3d pos; } Key;
typedef struct { int unk0; int count; int unk8; float duration; Key *keys; } Anim;
extern float DAT_007f0f64;
extern float DAT_007f0f68;
extern float DAT_007f0f6c;
extern float DAT_007f0f70;
extern float DAT_007f0f74;
extern float DAT_007f0f78;
void FUN_004752d0(const char *fmt, ...);
void FUN_00482ae0(float t, RwV3d *out, RwV3d *p0, RwV3d *p1, RwV3d *p2, RwV3d *p3);
RwMatrix *FUN_004c51a0(RwMatrix *m, RwV3d *v, int op);
void FUN_00532a60(RwMatrix *m, Key *k);
void FUN_00546e70(RtQuat *from, RtQuat *to, RtQuatSlerpCache *cache);
#define SINPI(result, x) { float _x = (x); float _z = _x * _x; \
  (result) = _x + (_z * _x) * (((((1.5896910e-10f * _z - 2.5050760e-08f) * _z + 2.7557314e-06f) * _z - 1.9841270e-04f) * _z + 8.3333338e-03f) * _z - 0.16666667f); }
// MATCH: 0x00404fa0 FUN_00404fa0
void FUN_00404fa0(RwMatrix *out, Anim *anim, float t)
{
  Key *keys;
  int n;
  int i;
  Key *prev;
  Key *cur;
  Key *k0;
  Key *k3;
  float tween;
  RtQuatSlerpCache cache;
  RtQuat q;
  RwV3d pos;
  float rS;
  RwV3d rV, rW, square, cross;

  keys = anim->keys;
  n = anim->count;
  if (t <= 0.0f) {
    FUN_00532a60(out, keys);
    return;
  }
  if (t >= anim->duration) {
    FUN_00532a60(out, &keys[n - 1]);
    return;
  }
  i = 0;
  prev = keys;
  cur = keys;
  do {
    prev = cur;
    cur = &keys[++i];
  } while (t > cur->time);
  if (i <= 1) k0 = keys; else k0 = &keys[i - 2];
  if (i >= n - 2) k3 = &keys[i]; else k3 = &keys[i + 1];
  tween = (t - prev->time) / (cur->time - prev->time);
  if (tween < 0.0f) FUN_004752d0("tween is less than 0.0f!!\n");
  if (tween > 1.0f) FUN_004752d0("tween is greater than 1.0f!!\n");
  DAT_007f0f64 = t;
  DAT_007f0f68 = tween;
  DAT_007f0f6c = k0->time;
  DAT_007f0f70 = prev->time;
  DAT_007f0f74 = cur->time;
  DAT_007f0f78 = k3->time;
  FUN_00546e70(&prev->q, &cur->q, &cache);
  if (tween <= 0.0f) {
    q = prev->q;
  } else if (1.0f <= tween) {
    q = cur->q;
  } else {
    float rSclFrom = 1.0f - tween;
    float rSclTo = tween;
    if (!cache.nearlyZeroOm) {
      rSclFrom *= cache.omega;
      SINPI(rSclFrom, rSclFrom);
      rSclTo *= cache.omega;
      SINPI(rSclTo, rSclTo);
    }
    q.imag.x = cache.raFrom.imag.x * rSclFrom;
    q.imag.y = cache.raFrom.imag.y * rSclFrom;
    q.imag.z = cache.raFrom.imag.z * rSclFrom;
    q.imag.x += cache.raTo.imag.x * rSclTo;
    q.imag.y += cache.raTo.imag.y * rSclTo;
    q.imag.z += cache.raTo.imag.z * rSclTo;
    q.real = cache.raFrom.real * rSclFrom + cache.raTo.real * rSclTo;
  }
  FUN_00482ae0(tween, &pos, &k0->pos, &prev->pos, &cur->pos, &k3->pos);
  rS = 2.0f / (q.real * q.real + (q.imag.z * q.imag.z + (q.imag.x * q.imag.x + q.imag.y * q.imag.y)));
  rV.x = q.imag.x * rS;
  rV.y = q.imag.y * rS;
  rV.z = q.imag.z * rS;
  rW.x = rV.x * q.real;
  rW.y = rV.y * q.real;
  rW.z = rV.z * q.real;
  square.x = q.imag.x * rV.x;
  square.y = q.imag.y * rV.y;
  square.z = q.imag.z * rV.z;
  cross.x = q.imag.y * rV.z;
  cross.y = q.imag.z * rV.x;
  cross.z = q.imag.x * rV.y;
  out->right.x = 1.0f - (square.y + square.z);
  out->right.y = cross.z + rW.z;
  out->right.z = cross.y - rW.y;
  out->up.x = cross.z - rW.z;
  out->up.y = 1.0f - (square.z + square.x);
  out->pos.x = 0.0f;
  out->up.z = cross.x + rW.x;
  out->at.x = cross.y + rW.y;
  out->at.y = cross.x - rW.x;
  out->at.z = 1.0f - (square.x + square.y);
  out->pos.y = 0.0f;
  out->pos.z = 0.0f;
  out->flags = 3;
  FUN_004c51a0(out, &pos, 2);
}
