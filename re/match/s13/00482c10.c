// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00482c10 (h12): 83.3% (regs 80.0%, ops 94.2%), 1022/1018 ins, 60 min cap reached. Raw converter did
   not compile; written by hand from the decomp + disassembly. Shape: clamp time to [+0x178, +0x174], walk the frame
   list (0x24-byte frames: quat, pos, next, time) with an outer if + do/while (that form matches the original loop),
   then the RenderWare macros RwMatrixSetIdentity / RtQuatConvertToMatrix / RwMatrixTranslate (0x004c51a0, combine 2)
   on the five 0x40-byte matrices at +0x28, slerp via the 0x00546e70 cache with the RwSinMinusPiToPi polynomial,
   spline 0x00482ae0 for the position, copy matrix 0 to param_2. Found: the modulus must be written
   w*w + (z*z + (x*x + y*y)) (gives the original's y,x,z,w load order), macro temps and the four spline points in
   block scopes (frame 0x94), set-identity inside each branch of the m[3]/m[4] selects, non-null branch first,
   'param_3 >= cur->time'. Residual (b): x87/integer scheduling inside the inlined quat->matrix blocks (the
   original pushes the RwMatrixTranslate args and stores pos/flags at different points per block: pos-first in the
   macro suits block m[1], pos-last suits m[2] and m[0]), plus 4 extra instructions in the m[3]/m[4] tails.
   Permuter (566 variants) gained 0.2% on an earlier 81% base only. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct V3_482c10 { float x, y, z; } V3_482c10;
typedef struct Quat482c10 { V3_482c10 imag; float real; } Quat482c10;
typedef struct Frame482c10 {
  Quat482c10 q;                    /* +0x00 */
  V3_482c10 pos;                   /* +0x10 */
  struct Frame482c10 *next;        /* +0x1c */
  uint time;                       /* +0x20 */
} Frame482c10;
typedef struct Mat482c10 { V3_482c10 right; uint flags; V3_482c10 up; int pad1; V3_482c10 at; int pad2;
  V3_482c10 pos; int pad3; } Mat482c10;
typedef struct Replay482c10 {
  int f0[6];
  Frame482c10 *first;              /* +0x18 */
  Frame482c10 *cur;                /* +0x1c */
  int f20[2];
  Mat482c10 m[5];                  /* +0x28 */
  int f168[3];
  uint maxT;                       /* +0x174 */
  uint minT;                       /* +0x178 */
} Replay482c10;
typedef struct SlerpCache482c10 { Quat482c10 from; Quat482c10 to; float omega; int nearlyZero; } SlerpCache482c10;
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc94c, 0x005cc94c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc908, 0x005cc908);
MGLOBAL(float, DAT_005cc904, 0x005cc904);
MGLOBAL(float, DAT_005cc900, 0x005cc900);
MGLOBAL(float, DAT_005cc8fc, 0x005cc8fc);
MGLOBAL(float, DAT_005cc8f8, 0x005cc8f8);
MGLOBAL(float, DAT_005cc8f4, 0x005cc8f4);
void FUN_00482ae0(float t, V3_482c10 *out, V3_482c10 *p0, V3_482c10 *p1, V3_482c10 *p2, V3_482c10 *p3);
Mat482c10 *FUN_004c51a0(Mat482c10 *m, V3_482c10 *v, int combine);
void FUN_00546e70(Quat482c10 *from, Quat482c10 *to, SlerpCache482c10 *cache);

#define SETIDENT_482c10(m) \
  { (m)->right.x = (m)->up.y = (m)->at.z = 1.0f; \
    (m)->right.y = (m)->right.z = (m)->up.x = 0.0f; \
    (m)->up.z = (m)->at.x = (m)->at.y = 0.0f; \
    (m)->pos.x = (m)->pos.y = (m)->pos.z = 0.0f; \
    (m)->flags = (m)->flags | 0x20003; }

#define QUAT2MAT_482c10(qp, m) \
  { float rS; V3_482c10 rV; V3_482c10 rW; V3_482c10 sq; V3_482c10 cr; \
    rS = DAT_005cc574 / ((qp)->real * (qp)->real + ((qp)->imag.z * (qp)->imag.z + ((qp)->imag.x * (qp)->imag.x + (qp)->imag.y * (qp)->imag.y))); \
    rV.x = (qp)->imag.x * rS; rV.y = (qp)->imag.y * rS; rV.z = (qp)->imag.z * rS; \
    rW.x = rV.x * (qp)->real; rW.y = rV.y * (qp)->real; rW.z = rV.z * (qp)->real; \
    sq.x = (qp)->imag.x * rV.x; sq.y = (qp)->imag.y * rV.y; sq.z = (qp)->imag.z * rV.z; \
    cr.x = (qp)->imag.y * rV.z; cr.y = (qp)->imag.z * rV.x; cr.z = (qp)->imag.x * rV.y; \
    (m)->pos.x = 0.0f; (m)->pos.y = 0.0f; (m)->pos.z = 0.0f; \
    (m)->flags = 3; \
    (m)->right.x = DAT_005cc320 - (sq.y + sq.z); \
    (m)->right.y = cr.z + rW.z; \
    (m)->right.z = cr.y - rW.y; \
    (m)->up.x = cr.z - rW.z; \
    (m)->up.y = DAT_005cc320 - (sq.z + sq.x); \
    (m)->up.z = cr.x + rW.x; \
    (m)->at.x = cr.y + rW.y; \
    (m)->at.y = cr.x - rW.x; \
    (m)->at.z = DAT_005cc320 - (sq.x + sq.y); \
    }

#define SIN_482c10(r, xx) \
  { float x2 = (xx) * (xx); \
    (r) = (xx) + (((((DAT_005cc908 * x2 - DAT_005cc904) * x2 + DAT_005cc900) * x2 - DAT_005cc8fc) * x2 + DAT_005cc8f8) * x2 - DAT_005cc8f4) * (x2 * (xx)); }

// MATCH: 0x00482c10 FUN_00482c10
void FUN_00482c10(Replay482c10 *param_1, Mat482c10 *param_2, uint param_3)
{
  Frame482c10 *cur;
  Frame482c10 *prev;
  Frame482c10 *next;
  Frame482c10 *last;
  Quat482c10 q;
  SlerpCache482c10 cache;
  V3_482c10 out;
  float tt;

  cur = param_1->first;
  prev = 0;
  if (param_3 > param_1->maxT) param_3 = param_1->maxT;
  if (param_3 < param_1->minT) param_3 = param_1->minT;
  if (cur->next != 0 && cur->next->time < param_3) {
    do {
      if (cur->time >= param_3) break;
      prev = cur;
      cur = cur->next;
      if (cur->next == 0) break;
    } while (cur->next->time < param_3);
  }
  param_1->cur = cur;
  next = cur->next;
  if (next != 0) {
    FUN_00546e70(&cur->q, &next->q, &cache);
    SETIDENT_482c10(&param_1->m[1]);
    QUAT2MAT_482c10(&cur->q, &param_1->m[1]);
    FUN_004c51a0(&param_1->m[1], &cur->pos, 2);
    SETIDENT_482c10(&param_1->m[2]);
    QUAT2MAT_482c10(&next->q, &param_1->m[2]);
    FUN_004c51a0(&param_1->m[2], &next->pos, 2);
    if (param_3 >= cur->time) {
      tt = (float)(param_3 - cur->time) / (float)(next->time - cur->time);
      if (tt <= DAT_005d757c) {
        q = cur->q;
      } else if (DAT_005cc320 <= tt) {
        q = next->q;
      } else {
        float sFrom = DAT_005cc320 - tt;
        float sTo = tt;
        if (!cache.nearlyZero) {
          sFrom *= cache.omega;
          SIN_482c10(sFrom, sFrom);
          sTo *= cache.omega;
          SIN_482c10(sTo, sTo);
        }
        q.imag.x = cache.from.imag.x * sFrom;
        q.imag.y = cache.from.imag.y * sFrom;
        q.imag.z = cache.from.imag.z * sFrom;
        q.imag.x += cache.to.imag.x * sTo;
        q.imag.y += cache.to.imag.y * sTo;
        q.imag.z += cache.to.imag.z * sTo;
        q.real = cache.from.real * sFrom + cache.to.real * sTo;
      }
      QUAT2MAT_482c10(&q, &param_1->m[0]);
      if (next->next != 0) {
        SETIDENT_482c10(&param_1->m[3]);
        QUAT2MAT_482c10(&next->next->q, &param_1->m[3]);
        FUN_004c51a0(&param_1->m[3], &next->next->pos, 2);
      } else {
        SETIDENT_482c10(&param_1->m[3]);
        QUAT2MAT_482c10(&next->q, &param_1->m[3]);
        FUN_004c51a0(&param_1->m[3], &next->pos, 2);
      }
      if (prev != 0) {
        SETIDENT_482c10(&param_1->m[4]);
        QUAT2MAT_482c10(&prev->q, &param_1->m[4]);
        FUN_004c51a0(&param_1->m[4], &prev->pos, 2);
      } else {
        SETIDENT_482c10(&param_1->m[4]);
        QUAT2MAT_482c10(&cur->q, &param_1->m[4]);
        FUN_004c51a0(&param_1->m[4], &cur->pos, 2);
      }
      {
      V3_482c10 p0, p1, p2, p3;
      p0 = param_1->m[4].pos;
      p1 = param_1->m[1].pos;
      p2 = param_1->m[2].pos;
      p3 = param_1->m[3].pos;
      FUN_00482ae0(tt, &out, &p0, &p1, &p2, &p3);
      }
      FUN_004c51a0(&param_1->m[0], &out, 2);
      if (param_2 != 0) {
        *param_2 = param_1->m[0];
      }
    }
  } else {
    last = param_1->first;
    while (last->next != 0) {
      last = last->next;
    }
    SETIDENT_482c10(&param_1->m[0]);
    QUAT2MAT_482c10(&last->q, &param_1->m[0]);
    FUN_004c51a0(&param_1->m[0], &last->pos, 2);
  }
}
