// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1a70 (h2 hand form, h6): 90.7%, regs 90.7%, cxx ok. switch(projection) with case 2 + default gives the original sub ecx,2/je; /G5 (h6) took it from 88.4 to 90.7. Residual (b): the original copies device zBufferNear/zBufferFar (RwGlobals+0x18/+0x1c) to the stack with fld/fstp, ours with mov pairs. Tried: (float)(double) casts, double locals, a 2-float struct local (copy and per-field), /Op, permuter 128 compiles. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
// MATCH: 0x004c1a70 FUN_004c1a70
void FUN_004c1a70(int param_1)
{
  float zNear, zFar, nearZ, farZ, range, scale;
  zNear = *(float *)(DAT_007d3ff8 + 0x18);
  zFar = *(float *)(DAT_007d3ff8 + 0x1c);
  farZ = *(float *)(param_1 + 0x84);
  switch (*(int *)(param_1 + 0x14)) {
  case 2:
    nearZ = *(float *)(param_1 + 0x80);
    break;
  default:
    farZ = _DAT_005cc320 / farZ;
    nearZ = *(float *)(param_1 + 0x80);
    nearZ = _DAT_005cc320 / nearZ;
    break;
  }
  range = (zFar - zNear) * _DAT_005cd03c;
  zFar -= range;
  zNear += range;
  scale = (zFar - zNear) / (farZ - nearZ);
  *(float *)(param_1 + 0x8c) = scale;
  *(float *)(param_1 + 0x90) = ((zFar + zNear) - (nearZ + farZ) * scale) * _DAT_005cc32c;
}
