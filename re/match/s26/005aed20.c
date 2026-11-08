// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005aed20 (119 B). Best 100.0% (regs 100.0%), exact. FastSqrt-style: t=x*x+y*y (loads y,x,z) then t+z*z; u32 punning via (unsigned*)param_1; table MGLOBALA(short,..,0x10000). */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBALA(short, DAT_00633b48, 0x10000, 0x00633b48);
// MATCH: 0x005aed20 FUN_005aed20
void FUN_005aed20(float *param_1,float *param_2)
{
  unsigned int *u = (unsigned int *)param_1;
  unsigned int ecx, e;
  float y = param_2[1]; float x = param_2[0]; float z = param_2[2]; float t = x * x + y * y; *param_1 = t + z * z;
  if (*param_1 != DAT_005d757c) {
    ecx = *u;
    e = (ecx >> 0x17) - 0x7f;
    ecx = ecx & 0x7fffff;
    *u = ecx;
    if (e & 1) {
      ecx |= 0x800000;
      *u = ecx;
    }
    *u = ((int)DAT_00633b48[((unsigned short *)u)[1]] << 0x10) | (((short)((short)e >> 1)) + 0x7f) << 0x17;
  }
}
