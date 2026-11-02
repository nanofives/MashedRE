// FLAGS: /O2 /GS /Ob1
/* 0x00475210: best 93.1% (regs 93.1%, ops 100%). Tried: shared nr local (83.1%), (-info.r) per component (93.1%),
   in-place info.r negate (69%), separate tmp vector (65.5%), all operand orders of the adds/muls (no effect),
   statement reorders, float* params, pointer to out, permuter (no gain).
   Residual (b): the y/z sums load out.y/out.z before param_1->y/z (original loads param_1 first); 4 instructions. */
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
typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 v; float f0c; float m[10]; float r; float f3c; } Info40;
int FUN_0045bfe0(Vec3 *p0, Info40 *p1);
int FUN_004b4b60(int p0);
void FUN_004b5080(int p0, float *p1, Vec3 *p2);
// MATCH: 0x00475210 FUN_00475210
int FUN_00475210(Vec3 *param_1, int param_2)
{
  Vec3 out;
  Info40 info;


  if (FUN_004b4b60(FUN_0045bfe0(param_1, &info)) != 0) {
    out.x = info.v.x * (-info.r); out.y = info.v.y * (-info.r); out.z = info.v.z * (-info.r);
    out.x = param_1->x + out.x; out.y = param_1->y + out.y; out.z = param_1->z + out.z;
    out.x += info.v.x * 0.02f; out.y += info.v.y * 0.02f; out.z += info.v.z * 0.02f;
    FUN_004b5080(param_2, info.m, &out);
    return 1;
  }
  return 0;
}
