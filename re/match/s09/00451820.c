// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451820 (h1): 100% (regs 100%). Raw converter 29.1%: for loop, 64-byte matrix struct copy from the out-pointer, typed callee args. Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct { float m[16]; } M16_00451820;
MGLOBAL(undefined1, DAT_005f9788, 0x005f9788);
int FUN_0040e340(void);
undefined4 FUN_0046be10(void * p0, int p1, int p2, int p3);
undefined4 FUN_0046d4a0(M16_00451820 ** p0, int p1);
float * FUN_004c4dc0(M16_00451820 * p0, M16_00451820 * p1);
uint * FUN_004c52f0(M16_00451820 * p0, M16_00451820 * p1, int p2);
// MATCH: 0x00451820 FUN_00451820
void FUN_00451820(int param_1)
{
  int n;
  int i;
  M16_00451820 *src;
  M16_00451820 m;
  M16_00451820 inv;

  n = FUN_0040e340();
  for (i = 0; i < n; i++) {
    FUN_0046d4a0(&src,i);
    FUN_004c4dc0(&inv,*(M16_00451820 **)(param_1 + 0x94));
    m = *src;
    FUN_004c52f0(&m,&inv,2);
    FUN_0046be10(&DAT_005f9788,i,0,0);
    if (m.m[12] < *(float *)(param_1 + 0x14) && m.m[12] > *(float *)(param_1 + 0x20) &&
        m.m[14] < *(float *)(param_1 + 0x1c) && m.m[14] > *(float *)(param_1 + 0x28)) {
      *(undefined4 *)(param_1 + 0x48) = 1;
    }
  }
}
