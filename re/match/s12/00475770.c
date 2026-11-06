// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475770 (h7): best 54.4% (regs 55.4%), natural order 52.6%. Hand form: float locals for
   m[0..2], double m6/m10 (gives the two fld before the branch), float |m2|, condition written as > (fcomp/test
   0x41/jne), atan2/fabs intrinsic (fpatan/fabs), FUN_004a39d4 = CRT double(double) stack entry (asin-shaped,
   called on -m2). Permuter (350 compiles) moved m10 below am2 and the param_1[0]=0 store below z: 54.4.
   Residual (b): the original keeps m6/m10 on the x87 stack across the compare (ours spills them to qword
   slots), pops |m2| at the fcomp and re-reads it from memory for the divide (ours keeps it and reassociates
   the divide), and multiplies y/z by the constant in place (fmul m32) where ours loads the constant and
   multiplies st(2)/st(1). Tried: float/double m6/m10/y/z/x, atan2 hoisted before the if, per-branch stores
   of param_1[1..2], 120 declaration/assignment orders (all 42-44%). Raw converter was 6.5%. */
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
MGLOBAL(float, DAT_005ceae4, 0x005ceae4);
MGLOBAL(float, DAT_005ceae0, 0x005ceae0);
MGLOBAL(float, DAT_005cc98c, 0x005cc98c);
double FUN_004a39d4(double x);
double atan2(double y, double x);
double fabs(double x);
#pragma intrinsic(atan2)
#pragma intrinsic(fabs)
// MATCH: 0x00475770 FUN_00475770
void FUN_00475770(float *param_1, float *param_2)
{
  float m0;
  float m1;
  float m2;
  double m6;
  double m10;
  float am2;
  float x;
  float y;
  float z;

  if (param_2 != 0) {
    m0 = param_2[0];
    m1 = param_2[1];
    m2 = param_2[2];
    m6 = param_2[6];
    am2 = (float)fabs(m2);
    m10 = param_2[10];
    if (am2 > DAT_005ceae4) {
      y = m2 / am2 * DAT_005ceae0;
      z = (float)atan2(-param_2[4], -(param_2[8] * m2));
      param_1[0] = 0.0f;
    }
    else {
      x = (float)atan2(m6, m10);
      y = (float)FUN_004a39d4(-m2);
      z = (float)atan2(m1, m0);
      param_1[0] = x * DAT_005cc98c;
    }
    param_1[1] = y * DAT_005cc98c;
    param_1[2] = z * DAT_005cc98c;
  }
}
