// FLAGS: /O2 /Ob1 /G5
// h3: 95.6% regs 95.6%. Struct local for the error record, iVar1 hoisted above the compare, count read as p[1]=p[1]+1; return p[1]-1. Residual: lea edi scheduled before cmp; class (a).
/* MATCH-RWL03 0x004f3be0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int (*Cb108_4f3be0)(int, int);
typedef int (*Cb110_4f3be0)(int, int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d7ff0(int p0, int p1);
void FUN_004d8480(int *p0);
// MATCH: 0x004f3be0 FUN_004f3be0
int FUN_004f3be0(int *param_1,int param_2)
{
  int iVar1;
  int iVar2;
  struct { int a; int b; } st;

  iVar1 = param_1[2] * 4 + 0x50;
  if (param_1[2] > param_1[1]) {
    *(int *)(*param_1 + param_1[1] * 4) = param_2;
    *(short *)(param_2 + 0x18) = *(short *)(param_2 + 0x18) + 1;
    param_1[1] = param_1[1] + 1;
    return param_1[1] - 1;
  }
  if (*param_1 != 0) {
    iVar2 = (*(Cb110_4f3be0 *)(DAT_007d3ff8 + 0x110))(*param_1,iVar1,0x1030008);
  }
  else {
    iVar2 = (*(Cb108_4f3be0 *)(DAT_007d3ff8 + 0x108))(iVar1,0x1030008);
  }
  if (iVar2 == 0) {
    st.a = 2;
    st.b = FUN_004d7ff0(0x80000013,iVar1);
    FUN_004d8480(&st.a);
    return -1;
  }
  *param_1 = iVar2;
  param_1[2] = param_1[2] + 0x14;
  *(int *)(iVar2 + param_1[1] * 4) = param_2;
  *(short *)(param_2 + 0x18) = *(short *)(param_2 + 0x18) + 1;
  param_1[1] = param_1[1] + 1;
  return param_1[1] - 1;
}
