// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c4e20: 95.0% (regs 95.0%, ops 100%). AABB node test: float compares as n[4]<=a[0].. and n[1]>=a[3].., intrinsic memcpy(24) for the two rep movsd copies, ternary flag bits. Residual: orig keeps the two flag temps in the arg1/arg2 stack slots (esp+0x14/0x18), ours in fresh slots (0x18/0x1c); assigning to params (as ints) scored 41%. Class: b. */
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
typedef unsigned int size_t_;
void *memcpy(void *, const void *, size_t_);
#pragma intrinsic(memcpy)
// MATCH: 0x005c4e20 FUN_005c4e20
void FUN_005c4e20(char *param_1, float *param_2, float *param_3, float *param_4)
{
  unsigned int f1, f2, f3, f4, f5, f6;
  int i;
  unsigned int *dst;
  unsigned int *src;

  if (param_2[4] <= param_3[0] && param_2[5] <= param_3[1] && param_2[6] <= param_3[2] &&
      param_2[1] >= param_3[3] && param_2[2] >= param_3[4] && param_2[3] >= param_3[5]) {
    *(float **)param_1 = param_2;
    *(unsigned short *)(param_1 + 0x36) = 0;
    *(unsigned char *)(param_1 + 0x35) = 0xff;
    if (*(short *)((char *)param_2 + 0x1e) == 0) {
      *(unsigned char *)(param_1 + 0x35) = *(unsigned char *)((char *)param_2 + 0x1c);
    }
    memcpy(param_1 + 4, param_3, 24);
    memcpy(param_1 + 0x1c, param_4, 24);
    *(char **)(param_1 + 0x3c) = param_1 + 0x34;
    f1 = param_2[4] < param_4[3] ? 1 : 0;
    f2 = param_2[5] < param_4[4] ? 4 : 0;
    f3 = param_2[6] < param_4[5] ? 0x10 : 0;
    f4 = param_2[1] > param_4[0] ? 2 : 0;
    f5 = param_2[2] > param_4[1] ? 8 : 0;
    f6 = param_2[3] > param_4[2] ? 0x20 : 0;
    *(unsigned int *)(param_1 + 0x40) = f6 | f5 | f4 | f3 | f2 | f1;
    *(char **)(param_1 + 0x144) = param_1 + 0x44;
    return;
  }
  *(char **)(param_1 + 0x144) = param_1 + 0x3c;
}



