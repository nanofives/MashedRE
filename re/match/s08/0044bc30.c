// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044bc30 (279 B): 100% (regs 100%), by hand. Raw converter 33.8%.
   What moved it: the two .rdata floats are compiler literals, not globals: 0x005cc9b4 = 0.99f and
   0x005cc558 = 0.001f. As MGLOBAL externs the compiler hoisted/reordered their loads (84.0%); as literals the
   fmul [const] form matches. Locals: four 12-byte vectors (zero, x axis, z axis, frame+0x40 struct copy).
   FUN_004c1340 is cdecl (frame, V3*, int), FUN_004c1520 cdecl (frame, V3*, float, int). relocheck OK, cxx ok. */
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
typedef struct V3_bc30 {
  float x;
  float y;
  float z;
} V3_bc30;
int FUN_004c1340(int p0, V3_bc30 *p1, int p2);
int FUN_004c1520(int p0, V3_bc30 *p1, float p2, int p3);
// MATCH: 0x0044bc30 FUN_0044bc30
void FUN_0044bc30(int param_1, float param_2)
{
  int frame;
  float a;
  float b;
  V3_bc30 zero;
  V3_bc30 ax;
  V3_bc30 az;
  V3_bc30 pos;

  ax.x = 1.0f;
  ax.y = 0.0f;
  ax.z = 0.0f;
  az.x = 0.0f;
  az.y = 0.0f;
  az.z = 1.0f;
  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  frame = *(int *)(*(int *)(param_1 + 4) + 4);
  pos = *(V3_bc30 *)(frame + 0x40);
  FUN_004c1340(frame, &zero, 0);
  FUN_004c1520(frame, &ax, *(float *)(param_1 + 0x24), 2);
  FUN_004c1520(frame, &az, -*(float *)(param_1 + 0x20), 2);
  FUN_004c1340(frame, &pos, 2);
  a = param_2 * *(float *)(param_1 + 0x10) + *(float *)(param_1 + 0x20);
  *(float *)(param_1 + 0x20) = a;
  b = param_2 * *(float *)(param_1 + 0x18) + *(float *)(param_1 + 0x24);
  *(float *)(param_1 + 0x24) = b;
  *(float *)(param_1 + 0x14) = *(float *)(param_1 + 0x14) * 0.99f;
  *(float *)(param_1 + 0x10) = *(float *)(param_1 + 0x10) * 0.99f - a * param_2 * 0.001f;
  *(float *)(param_1 + 0x18) = *(float *)(param_1 + 0x18) * 0.99f - b * param_2 * 0.001f;
}
