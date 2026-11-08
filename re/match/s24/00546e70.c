// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00546e70 (h2): 87.0% (regs 87.0%), 257/261 instructions. Quaternion slerp-cache setup:
   copy qpFrom, cosTheta = dot(qpFrom, qpTo) (as ((x + y) + z) + w), negate qpTo into raTo when cosTheta < 0,
   omega = an inlined fdlibm-style float acos (FUN_004c3b30 is its sqrt), nearlyZeroOm = cosTheta >= 0.99999
   (0x005e4888), and when not nearly zero both quaternions scaled by 1 / (polynomial sine of omega).
   Raw converter did not compile (C2440). The acos is written inline with each branch storing omega (constant
   branches then store immediates and jump past the common fstp, as in the original); a static __inline
   function was not inlined under /Ob1 and __forceinline kept the constants in x87 (56-65%).
   Residuals: (1) x87 operand order: the original loads the register operand first in four polynomial terms
   (fld st(1); fmul [0x005d8dd8], fld [esp+k]; fmul [0x005d8dec], fld x; fadd 1.0) and the memory operand
   first in five of the eight final scale products (fld [esi+4]; fmul st(1)) and keeps invSin to a final
   fstp st(0); ours does the reverse whatever the source operand order or *= form. (2) one `fld st(0);
   fstp [esp+0x14]` where ours writes `fst`, (3) the 1 - x term loads 1.0 first in the original.
   Class (a): x87 operand order (flags_brief residual class); tried source operand swaps, *=, six sine forms,
   eight dot-product forms. */
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
typedef struct { float x, y, z; } V3d;
typedef struct { V3d imag; float real; } Quat;
typedef struct {
  Quat raFrom;        /* +0x00 */
  Quat raTo;          /* +0x10 */
  float omega;        /* +0x20 */
  int nearlyZeroOm;   /* +0x24 */
} SlerpCache;
MGLOBAL(float, DAT_005d757c, 0x005d757c);  /* 0.0 */
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);  /* -1.0 */
MGLOBAL(float, DAT_005cc320, 0x005cc320);  /* 1.0 */
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);  /* 0.5 */
MGLOBAL(float, DAT_005e4888, 0x005e4888);  /* 0.99999 */
MGLOBAL(float, DAT_005d8dc0, 0x005d8dc0);  /* pi */
MGLOBAL(float, DAT_005d8dc4, 0x005d8dc4);  /* pi/2 hi */
MGLOBAL(float, DAT_005d8dc8, 0x005d8dc8);  /* pi/2 lo */
MGLOBAL(float, DAT_005d8dcc, 0x005d8dcc);
MGLOBAL(float, DAT_005d8dd0, 0x005d8dd0);
MGLOBAL(float, DAT_005d8dd4, 0x005d8dd4);
MGLOBAL(float, DAT_005d8dd8, 0x005d8dd8);
MGLOBAL(float, DAT_005d8ddc, 0x005d8ddc);
MGLOBAL(float, DAT_005d8de0, 0x005d8de0);
MGLOBAL(float, DAT_005d8de4, 0x005d8de4);
MGLOBAL(float, DAT_005d8de8, 0x005d8de8);
MGLOBAL(float, DAT_005d8dec, 0x005d8dec);
MGLOBAL(float, DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(float, DAT_005cc8f8, 0x005cc8f8);
MGLOBAL(float, DAT_005cc8fc, 0x005cc8fc);
MGLOBAL(float, DAT_005cc900, 0x005cc900);
MGLOBAL(float, DAT_005cc904, 0x005cc904);
MGLOBAL(float, DAT_005cc908, 0x005cc908);
float FUN_004c3b30(float x);

// MATCH: 0x00546e70 FUN_00546e70
void FUN_00546e70(Quat *qpFrom, Quat *qpTo, SlerpCache *qpCache)
{
  float cosTheta;

  qpCache->raFrom = *qpFrom;
  cosTheta = ((qpFrom->imag.x * qpTo->imag.x + qpFrom->imag.y * qpTo->imag.y) + qpFrom->imag.z * qpTo->imag.z) +
             qpFrom->real * qpTo->real;
  if (cosTheta < DAT_005d757c) {
    if (cosTheta < DAT_005cc33c) {
      cosTheta = 1.0f;
    }
    else {
      cosTheta = -cosTheta;
    }
    qpCache->raTo.real = -qpTo->real;
    qpCache->raTo.imag.x = -qpTo->imag.x;
    qpCache->raTo.imag.y = -qpTo->imag.y;
    qpCache->raTo.imag.z = -qpTo->imag.z;
  }
  else {
    if (cosTheta > DAT_005cc320) {
      cosTheta = 1.0f;
    }
    qpCache->raTo = *qpTo;
  }
  {
    float x = cosTheta;
    int hx = *(int *)&x;
    int ix = hx & 0x7fffffff;
    float z, p, q, r, w, sq, c, df;

    if (ix >= 0x3f800000) {
      if (hx > 0) {
        qpCache->omega = 0.0f;
      }
      else {
        qpCache->omega = 3.14159274f;
      }
    }
    else if (ix < 0x3f000000) {
      if (ix <= 0x23000000) {
        qpCache->omega = 1.57079637f;
      }
      else {
        z = x * x;
        p = ((((((DAT_005d8dec * z + DAT_005d8de8) * z - DAT_005d8de4) * z + DAT_005d8de0) * z - DAT_005d8ddc) * z +
              DAT_005cc8f4) * z);
        q = (((DAT_005d8dd8 * z - DAT_005d8dd4) * z + DAT_005d8dd0) * z - DAT_005d8dcc) * z + DAT_005cc320;
        r = p / q;
        qpCache->omega = DAT_005d8dc4 - (x - (DAT_005d8dc8 - x * r));
      }
    }
    else if (hx < 0) {
      z = (DAT_005cc320 + x) * DAT_005cc32c;
      sq = FUN_004c3b30(z);
      p = ((((((DAT_005d8dec * z + DAT_005d8de8) * z - DAT_005d8de4) * z + DAT_005d8de0) * z - DAT_005d8ddc) * z +
            DAT_005cc8f4) * z);
      q = (((DAT_005d8dd8 * z - DAT_005d8dd4) * z + DAT_005d8dd0) * z - DAT_005d8dcc) * z + DAT_005cc320;
      r = p / q;
      w = r * sq - DAT_005d8dc8;
      qpCache->omega = DAT_005d8dc0 - 2.0f * (sq + w);
    }
    else {
      int idf;
      z = (DAT_005cc320 - x) * DAT_005cc32c;
      sq = FUN_004c3b30(z);
      df = sq;
      idf = *(int *)&df;
      idf &= 0xfffff000;
      df = *(float *)&idf;
      c = (z - df * df) / (sq + df);
      p = ((((((DAT_005d8dec * z + DAT_005d8de8) * z - DAT_005d8de4) * z + DAT_005d8de0) * z - DAT_005d8ddc) * z +
            DAT_005cc8f4) * z);
      q = (((DAT_005d8dd8 * z - DAT_005d8dd4) * z + DAT_005d8dd0) * z - DAT_005d8dcc) * z + DAT_005cc320;
      r = p / q;
      w = r * sq + c;
      qpCache->omega = 2.0f * (df + w);
    }
  }
  qpCache->nearlyZeroOm = (cosTheta >= DAT_005e4888);
  if (!qpCache->nearlyZeroOm) {
    float w = qpCache->omega;
    float w2 = w * w;
    float invSin = DAT_005cc320 /
        ((w * w2) * (((((DAT_005cc908 * w2 - DAT_005cc904) * w2 + DAT_005cc900) * w2 - DAT_005cc8fc) * w2 +
                      DAT_005cc8f8) * w2 - DAT_005cc8f4) + w);
    qpCache->raFrom.real = invSin * qpCache->raFrom.real;
    qpCache->raFrom.imag.x = invSin * qpCache->raFrom.imag.x;
    qpCache->raFrom.imag.y = qpCache->raFrom.imag.y * invSin;
    qpCache->raFrom.imag.z = qpCache->raFrom.imag.z * invSin;
    qpCache->raTo.real = invSin * qpCache->raTo.real;
    qpCache->raTo.imag.x = qpCache->raTo.imag.x * invSin;
    qpCache->raTo.imag.y = qpCache->raTo.imag.y * invSin;
    qpCache->raTo.imag.z = qpCache->raTo.imag.z * invSin;
  }
}
