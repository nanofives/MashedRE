// FLAGS: /O2 /Ob1 /G5
/* RWC-MATCH-S03 2026-11-12: 87.0 -> 92.0% by writing the 23 .rdata float constants (0x005cc320..0x005e4888, all
   in .rdata 0x005cc000..0x005e9044, read-only) as literals (ddk-match-levers real@). Residual: the original loads
   six of the eight final scaled fields first (fld [esi+4]; fmul st(1)) and pops invSin with fstp st(0); df = sq
   is fld st(0); fstp. Operand order, compound assignment, comma and double forms do not move it. */
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
  if (cosTheta < 0.0f) {
    if (cosTheta < (-1.0f)) {
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
    if (cosTheta > 1.0f) {
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
        p = ((((((3.47933092e-05f * z + 0.000791535014f) * z - 0.0400555357f) * z + 0.201212525f) * z - 0.325565815f) * z +
              0.166666672f) * z);
        q = (((0.077038154f * z - 0.68828398f) * z + 2.02094579f) * z - 2.40339494f) * z + 1.0f;
        r = p / q;
        qpCache->omega = 1.57079625f - (x - (7.54978942e-08f - x * r));
      }
    }
    else if (hx < 0) {
      z = (1.0f + x) * 0.5f;
      sq = FUN_004c3b30(z);
      p = ((((((3.47933092e-05f * z + 0.000791535014f) * z - 0.0400555357f) * z + 0.201212525f) * z - 0.325565815f) * z +
            0.166666672f) * z);
      q = (((0.077038154f * z - 0.68828398f) * z + 2.02094579f) * z - 2.40339494f) * z + 1.0f;
      r = p / q;
      w = r * sq - 7.54978942e-08f;
      qpCache->omega = 3.1415925f - 2.0f * (sq + w);
    }
    else {
      int idf;
      z = (1.0f - x) * 0.5f;
      sq = FUN_004c3b30(z);
      df = sq;
      idf = *(int *)&df;
      idf &= 0xfffff000;
      df = *(float *)&idf;
      c = (z - df * df) / (sq + df);
      p = ((((((3.47933092e-05f * z + 0.000791535014f) * z - 0.0400555357f) * z + 0.201212525f) * z - 0.325565815f) * z +
            0.166666672f) * z);
      q = (((0.077038154f * z - 0.68828398f) * z + 2.02094579f) * z - 2.40339494f) * z + 1.0f;
      r = p / q;
      w = r * sq + c;
      qpCache->omega = 2.0f * (df + w);
    }
  }
  qpCache->nearlyZeroOm = (cosTheta >= 0.999989986f);
  if (!qpCache->nearlyZeroOm) {
    float w = qpCache->omega;
    float w2 = w * w;
    float invSin = 1.0f /
        ((w * w2) * (((((1.58969102e-10f * w2 - 2.50507597e-08f) * w2 + 2.75573143e-06f) * w2 - 0.000198412701f) * w2 +
                      0.00833333377f) * w2 - 0.166666672f) + w);
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
