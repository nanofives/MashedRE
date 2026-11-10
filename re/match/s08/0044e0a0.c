// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044e0a0: 98.4% by hand (raw 37.5%), regs 98.4%. 0x005cc32c (0.5f) and 0x005cd8fc (300.0f) are
   .rdata literals; the fabs compare is `(float)fabs(x) > 0.5f` (plain fabs gave fcomp qword). FUN_004b4510 is a
   cdecl (float, float) -> float, not Ghidra's fastcall. Residual (b): after the FUN_004b4510 call ours loads
   param_1->id (mov edx) before the fstp to b.x, the original stores first. Tried: double return, struct vs int
   param, id in a local, b as float[3], b.x via an expression, the permuter (172 compiles, no gain). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_0044e0a0;
double __cdecl fabs(double x);
#pragma intrinsic(fabs)
int FUN_0046d4a0(char **p0, int p1);
int FUN_00480720(int p0);
void FUN_00481750(int p0, V3_0044e0a0 *p1, V3_0044e0a0 *p2);
void FUN_0048aa20(V3_0044e0a0 *p0, int p1, int p2, int p3, int p4, float p5);
float FUN_004b4510(float p0, float p1);

typedef struct { char pad00[0x38]; float f38; int id; char pad40[4]; int i44; } Obj_0044e0a0;
void FUN_004b4550(V3_0044e0a0 *p0, Obj_0044e0a0 *p1, int p2);
// MATCH: 0x0044e0a0 FUN_0044e0a0
void FUN_0044e0a0(Obj_0044e0a0 *param_1)
{
  char *h;
  V3_0044e0a0 b;
  V3_0044e0a0 a;

  h = 0;
  a.x = 0.0f;
  a.y = 0.0f;
  a.z = 0.0f;
  b.x = 0.0f;
  b.y = 0.0f;
  b.z = 0.0f;
  FUN_004b4550(&a, param_1, param_1->i44);
  if ((float)fabs(param_1->f38) > 0.5f) {
    FUN_0048aa20(&a, 10, 0, 0, 0, 2.0f);
    FUN_0046d4a0(&h, param_1->id);
    b.x = *(float *)(h + 0x20) * 300.0f;
    b.y = *(float *)(h + 0x24) * 300.0f;
    { double tz = *(float *)(h + 0x28) * 300.0f; b.z = (float)tz; }  /* named double temporary (MATCH-X87-CASTLEVER-PILOT) */
    b.x = FUN_004b4510(-50.0f, 50.0f);
    FUN_00480720(param_1->id);
    FUN_00481750(param_1->id, &b, &a);
  }
}
