// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00491b20: 93.6% (regs 93.6%). Hand: 0x38-byte struct array at 0x00771558 indexed by the count directly (pointer temp gave 63.8%); permuter moved the d[] copies (natural order 89.4%). Tried: V3 struct assignments (13.5%), float fields. Residual (b): original reloads param_2 from the stack twice and reuses edx for the d[] copies; ours keeps param_2 live in edx and copies d[] through edi. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct {
  int a[3];
  int f0c;
  int b[3];
  int c[3];
  int d[3];
  int f34;
} S771558;
MGLOBAL(int, DAT_00771718, 0x00771718);
MGLOBALA(S771558, DAT_00771558, 8, 0x00771558);
// MATCH: 0x00491b20 FUN_00491b20
int FUN_00491b20(int *param_1, int param_2, int *param_3, int *param_4)
{
  if (DAT_00771718 >= 8) {
    return 0;
  }
  DAT_00771558[DAT_00771718].a[0] = param_1[0];
  DAT_00771558[DAT_00771718].a[1] = param_1[1];
  DAT_00771558[DAT_00771718].a[2] = param_1[2];
  DAT_00771558[DAT_00771718].b[0] = param_3[0];
  DAT_00771558[DAT_00771718].b[1] = param_3[1];
  DAT_00771558[DAT_00771718].b[2] = param_3[2];
  DAT_00771558[DAT_00771718].c[0] = param_4[0];
  DAT_00771558[DAT_00771718].c[1] = param_4[1];
  DAT_00771558[DAT_00771718].c[2] = param_4[2];
  DAT_00771558[DAT_00771718].d[1] = param_1[1];
  DAT_00771558[DAT_00771718].d[0] = param_1[0];
  DAT_00771558[DAT_00771718].f0c = param_2;
  DAT_00771558[DAT_00771718].d[2] = param_1[2];
  DAT_00771558[DAT_00771718].f34 = param_2;
  DAT_00771718++;
  return 1;
}
