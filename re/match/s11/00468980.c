// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00468980 (h1): 90.1% (regs 90.1%, ops 99.2%), 121/121 ins. Raw: did not compile (thiscall).
   Convention: car in ECX, matrix in ESI, float + one unused int on the stack, plain `ret`: a static callee
   (pilot finding 1). The real caller is 0x00470670 (car in EDI, `mov ecx, edi`, ESI = its own stack arg); the
   file holds a stand-in caller shaped like its tail (no MATCH line, built only outside MATCH_PORT).
   0x006146f0/0x006146fc/0x00614708 are the X/Y/Z unit vectors (.data); acos is the CRT stack entry
   FUN_004a3384; .rdata floats as literals (0.001, -1, 1, 57.29578f, 90.0 double, 90.0f, 0.05).
   What moved it: the angle in a named local (frame 0x20, stored then pushed via a register), the first angle
   as (float)-(acos*K - 90.0) * s, the second as ((float)(acos*K) - 90.0f) * s (fsub dword).
   Residual (b, (a) candidate): both 3-term dots of a matrix row with the Y axis (12 of 121 ins): the original
   emits y,x,z with the matrix loaded first except the x term of the second dot; ours always loads the global
   first. All term/operand orders, a V3 view of the row, file-static axis vectors and an array of three axes
   give identical code (cl canonicalises the sum, same as S05 0x0041faf0). Permuter 166 compiles: nothing. */
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
typedef struct V3_00468980 { float x, y, z; } V3_00468980;
MGLOBAL(V3_00468980, DAT_006146f0, 0x006146f0);
MGLOBAL(V3_00468980, DAT_006146fc, 0x006146fc);
MGLOBAL(V3_00468980, DAT_00614708, 0x00614708);
double FUN_004a3384(double x);
float FUN_004c39b0(V3_00468980 *out, V3_00468980 *in);
void *FUN_004c4d20(float *m, V3_00468980 *axis, float angle, int combine);

// MATCH: 0x00468980 FUN_00468980
static void FUN_00468980(int car, float *m, float param_3, int param_4)
{
  float s;
  float d;
  float ang;
  V3_00468980 v;
  V3_00468980 a;

  if (*(float *)(car + 0x9e0) == 0.0f) {
    if (*(int *)(car + 0x9f0) == 0) {
      s = param_3 * 0.001f;
      d = m[9] * DAT_006146fc.y + m[8] * DAT_006146fc.x + m[10] * DAT_006146fc.z;
      if (d < -1.0f) d = -1.0f;
      else if (d > 1.0f) d = 1.0f;
      ang = (float)-(FUN_004a3384(d) * 57.29578f - 90.0) * s;
      FUN_004c4d20(m, &DAT_006146f0, ang, 1);
      d = m[1] * DAT_006146fc.y + DAT_006146fc.x * m[0] + m[2] * DAT_006146fc.z;
      if (d < -1.0f) d = -1.0f;
      else if (d > 1.0f) d = 1.0f;
      ang = ((float)(FUN_004a3384(d) * 57.29578f) - 90.0f) * s;
      FUN_004c4d20(m, &DAT_00614708, ang, 1);
      return;
    }
    *(int *)(car + 0x9c4) = 0;
    *(int *)(car + 0x9c0) = 0;
    *(int *)(car + 0x9bc) = 0;
    v = *(V3_00468980 *)(car + 0x9b0);
    FUN_004c39b0(&v, &v);
    a.x = v.z;
    a.y = v.y;
    a.z = v.x;
    FUN_004c4d20(m, &a, param_3 * 0.05f, 1);
  }
}

#ifndef MATCH_PORT
/* stand-in caller: the tail of 0x00470670 (car in EDI, matrix from its stack args) */
void StandIn_00468980(int car, int x, float *m, float f, int k)
{
  FUN_00468980(car, m, f, k);
  if (*(int *)(car + 0x9f0) == 2) {
    *(float *)(car + 0x9b0) = *(float *)(car + 0x9b0) * 0.5f;
  }
}
#endif
