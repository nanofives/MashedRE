// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d400 (helper h4): 100% (regs 100%). _DAT_005cc31c/_DAT_005cd0a0 are float literals 3.0f/6.0f in .rdata (as globals the fadd operands swapped, 92.5%); DAT_005d757c (0.0) stays a global since 0-3 is not folded. Permuter fixed the end.x/end.z store order. Isect = RW line intersection {start,end,type=1}. */
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
typedef struct { float x, y, z; } V3_0046d400;
typedef struct { V3_0046d400 start; V3_0046d400 end; int type; } Isect_0046d400;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
int FUN_00538c80(int world, Isect_0046d400 *is, void *cb, float *data);
void *FUN_0046d3a0(void);
// MATCH: 0x0046d400 FUN_0046d400
float FUN_0046d400(float param_1,float param_2,float param_3,int param_4)
{
  Isect_0046d400 is;
  is.end.x = param_1;
  is.end.z = param_3;
  is.end.y = param_2 - 3.0f;
  is.start.y = param_2 + 3.0f;
  is.start.x = param_1;
  is.start.z = param_3;
  is.type = 1;
  param_1 = 6.0f;
  FUN_00538c80(*(int *)(param_4 + 8), &is, (void *)FUN_0046d3a0, &param_1);
  if (param_1 != 6.0f) {
    return param_1 - 3.0f;
  }
  return DAT_005d757c - 3.0f;
}
