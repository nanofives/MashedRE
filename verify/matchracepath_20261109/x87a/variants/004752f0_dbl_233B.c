// FLAGS: /O2 /GS /Ob1
/* 0x004752f0: 95.0% (regs 95.0%), hand pass h9 (MATCH-S12); raw converter 20.5%. Keyframe sampler: frames of
   0x24 B (time +4, RtQuat +8, pos +0x18), RtQuatSetupSlerpCache 0x00546e70 + the inlined RtQuatSlerp macro with
   the RwSinMinusPiToPi series, spline 0x00482ae0, RtQuatConvertToMatrix macro, RwMatrixTranslate 0x004c51a0
   (combine 2); macro shapes as in S13's 0x00482c10. What moved the score: `prev = cur; i++; cur = &frames[i];`
   in a do/while (74% -> 94%: gives the original's three registers prev/cur/iterator), `i <= 1` and
   `i >= n - 2 ? frames : &frames[i+1]` branch order, `t >= duration` for the end clamp, matrix flags store after
   at.z. Ghidra's size stops before `add esp,0x80 / ret` (2 of the misses). Residual (b): inside the quat->matrix
   block the original schedules the three RwMatrixTranslate pushes and the pos zero stores between the right.y and
   right.z stores; ours puts the pos stores right after the fdivr and the pushes after up.y (same residual S13
   reports for 0x00482c10). Tried: pos/flags at every statement position, chained/reversed pos zeroes, local
   order, permuter (280 variants, no gain). relocheck BAD lines (4) are offset shifts past the rescheduled
   block (orig bytes mid-instruction), not wrong globals. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d imag; float real; } RtQuat;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; int pad1; RwV3d at; int pad2; RwV3d pos; int pad3; } RwMatrix;
typedef struct { int f0; float time; RtQuat q; RwV3d pos; } KeyFrame;
typedef struct { int f0; int n; int f8; float duration; KeyFrame *frames; } KeyAnim;
typedef struct { RtQuat from; RtQuat to; float omega; int nearlyZero; } SlerpCache;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(float, DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(float, DAT_005cc8f8, 0x005cc8f8);
MGLOBAL(float, DAT_005cc8fc, 0x005cc8fc);
MGLOBAL(float, DAT_005cc900, 0x005cc900);
MGLOBAL(float, DAT_005cc904, 0x005cc904);
MGLOBAL(float, DAT_005cc908, 0x005cc908);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_004752d0(const char *msg);
void FUN_00482ae0(float t, RwV3d *out, RwV3d *p0, RwV3d *p1, RwV3d *p2, RwV3d *p3);
RwMatrix *FUN_004c51a0(RwMatrix *m, RwV3d *v, int combine);
void FUN_00532a60(RwMatrix *m, KeyFrame *f);
void FUN_00546e70(RtQuat *from, RtQuat *to, SlerpCache *cache);

#define SIN_SERIES(r, xx) \
  { float x2 = (xx) * (xx); \
    (r) = (xx) + (((((DAT_005cc908 * x2 - DAT_005cc904) * x2 + DAT_005cc900) * x2 - DAT_005cc8fc) * x2 + DAT_005cc8f8) * x2 - DAT_005cc8f4) * (x2 * (xx)); }

// MATCH: 0x004752f0 FUN_004752f0
void FUN_004752f0(RwMatrix *m, KeyAnim *anim, float t)
{
  int n;
  KeyFrame *frames;
  KeyFrame *cur;
  KeyFrame *prev;
  KeyFrame *p0;
  KeyFrame *p3;
  int i;
  float tween;
  RtQuat q;
  SlerpCache cache;
  RwV3d out;

  n = anim->n;
  frames = anim->frames;
  if (t <= DAT_005d757c) {
    FUN_00532a60(m, frames);
    return;
  }
  if (t >= anim->duration) {
    FUN_00532a60(m, &frames[n - 1]);
    return;
  }
  i = 0;
  cur = frames;
  do {
    prev = cur;
    i++;
    cur = &frames[i];
  } while (t > cur->time);
  if (i <= 1)
    p0 = &frames[n - 1];
  else
    p0 = &frames[i - 2];
  if (i >= n - 2)
    p3 = frames;
  else
    p3 = &frames[i + 1];
  tween = (t - prev->time) / (cur->time - prev->time);
  if (tween < DAT_005d757c)
    FUN_004752d0("tween is less than 0.0f!!\n");
  if (tween > DAT_005cc320)
    FUN_004752d0("tween is greater than 1.0f!!\n");
  FUN_00546e70(&prev->q, &cur->q, &cache);
  if (tween <= DAT_005d757c) {
    q = prev->q;
  } else if (DAT_005cc320 <= tween) {
    q = cur->q;
  } else {
    float sFrom = DAT_005cc320 - tween;
    float sTo = tween;
    if (!cache.nearlyZero) {
      sFrom *= cache.omega;
      SIN_SERIES(sFrom, sFrom);
      sTo *= cache.omega;
      SIN_SERIES(sTo, sTo);
    }
    q.imag.x = cache.from.imag.x * sFrom;
    q.imag.y = cache.from.imag.y * sFrom;
    q.imag.z = cache.from.imag.z * sFrom;
    q.imag.x += cache.to.imag.x * sTo;
    q.imag.y += cache.to.imag.y * sTo;
    q.imag.z += cache.to.imag.z * sTo;
    q.real = cache.from.real * sFrom + cache.to.real * sTo;
  }
  FUN_00482ae0(tween, &out, &p0->pos, &prev->pos, &cur->pos, &p3->pos);
  {
    float rS; RwV3d rV; RwV3d rW; RwV3d sq; RwV3d cr;
    rS = DAT_005cc574 / (q.real * q.real + (q.imag.z * q.imag.z + (q.imag.x * q.imag.x + q.imag.y * q.imag.y)));
    rV.x = q.imag.x * rS; rV.y = q.imag.y * rS; rV.z = q.imag.z * rS;
    rW.x = rV.x * q.real; rW.y = rV.y * q.real; rW.z = rV.z * q.real;
    sq.x = q.imag.x * rV.x; sq.y = q.imag.y * rV.y; sq.z = q.imag.z * rV.z;
    cr.x = q.imag.y * rV.z; cr.y = q.imag.z * rV.x; cr.z = q.imag.x * rV.y;
    m->pos.x = 0.0f; m->pos.y = 0.0f; m->pos.z = 0.0f;
    m->right.x = -((sq.y + sq.z)) + DAT_005cc320;
    m->right.y = cr.z + rW.z;
    m->right.z = cr.y - rW.y;
    m->up.x = cr.z - rW.z;
    { double tW_ = DAT_005cc320 - (sq.z + sq.x); m->up.y = (float)tW_; }
    m->up.z = (float)(cr.x + (double)(rW.x));
    m->at.x = cr.y + rW.y;
    m->at.y = cr.x - rW.x;
    m->at.z = DAT_005cc320 - (sq.x + sq.y);
    m->flags = 3;
  }
  FUN_004c51a0(m, &out, 2);
}
