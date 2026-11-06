// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00477e60 (h7): best 98.3% (regs 98.3%), 60/60 ins. Hand form: plane from three points,
   V3 a = p2-p3, b = p4-p3 (b spills to the 0xc frame, a stays on the x87 stack), cross into param_1->n,
   FUN_004c39b0(n, n) (cdecl 2 args, float return, popped), d = -(z + (x + y)) (that grouping gives the
   original y, x, z load order; left-assoc x+y+z gives y, z, x at 95.0%). Residual (b): pop edi is
   scheduled one instruction later in the original (after the last faddp). Tried: 15 dot-product
   groupings/spellings, inline Dot helper, temporaries, 0-x, float* param_1, permuter 163 compiles. Raw 31.6%. */
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
typedef struct { float x, y, z; } V3;
typedef struct { V3 n; float d; } PL;
float FUN_004c39b0(V3 *out, V3 *in);
// MATCH: 0x00477e60 FUN_00477e60
void FUN_00477e60(PL *param_1, V3 *param_2, V3 *param_3, V3 *param_4)
{
  V3 a;
  V3 b;
  a.x = param_2->x - param_3->x;
  a.y = param_2->y - param_3->y;
  a.z = param_2->z - param_3->z;
  b.x = param_4->x - param_3->x;
  b.y = param_4->y - param_3->y;
  b.z = param_4->z - param_3->z;
  param_1->n.x = b.y * a.z - b.z * a.y;
  param_1->n.y = b.z * a.x - b.x * a.z;
  param_1->n.z = b.x * a.y - b.y * a.x;
  FUN_004c39b0(&param_1->n, &param_1->n);
  param_1->d = -(param_1->n.z * param_2->z + (param_1->n.x * param_2->x + param_1->n.y * param_2->y));
}
