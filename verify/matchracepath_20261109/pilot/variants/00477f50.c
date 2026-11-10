// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00477f50 (h7): best 91.7% (regs 94.4%), 72/72 ins. Hand form: plane {V3 n; float d} at
   param_1, dist = n.p + d (kept in the param_1 slot), optional distance from p to its projection
   q = p + (-dist)*n into *param_3 via FUN_004c3ac0 (cdecl 1 arg, float return), result 0 if dist > K,
   2 if dist < K, else 1 (K = DAT_005d757c). Raw converter was 18.1%. Residual (b): in the dot product the
   original loads n.z before p.z (ours p.z first) and schedules the y fld after test esi; the q.y fld st(1)
   sits one slot earlier; fstp [param_3] comes before add esp,4 in the original. Tried: 7 dot groupings,
   float* params, inline Dot/AddScaled/Sub helpers, += form, component copy, len temp, ternary/>= returns,
   permuter 157 compiles: none moved it. */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_004c3ac0(V3 *v);
// MATCH: 0x00477f50 FUN_00477f50
int FUN_00477f50(PL *param_1, V3 *param_2, float *param_3)
{
  float dist;
  float t;
  V3 q;
  V3 diff;

  dist = (float)((double)(param_1->n.x * param_2->x + param_1->n.y * param_2->y) + param_1->n.z * param_2->z + param_1->d);
  if (param_3 != 0) {
    t = -dist;
    q = *param_2;
    { float tA_ = t * param_1->n.x; q.x = tA_ + q.x; }
    q.y = t * param_1->n.y + q.y;
    q.z = t * param_1->n.z + q.z;
    diff.x = param_2->x - q.x;
    diff.y = param_2->y - q.y;
    diff.z = (float)(param_2->z - (double)(q.z));
    *param_3 = FUN_004c3ac0(&diff);
  }
  if (dist > DAT_005d757c) {
    return 0;
  }
  if (dist < DAT_005d757c) {
    return 2;
  }
  return 1;
}
