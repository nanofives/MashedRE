// FLAGS: /O2 /Ob1 /G5
// h3: 95.5% regs 95.5%. Second box copy read through volatile ints so it is not CSEd with the first (the original re-reads param_2). Residual: push eax scheduled before the fstp; class (a).
/* MATCH-RWL03 0x00538310: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef struct N538310 { int type; float split; struct N538310 *c0; struct N538310 *c1; } N538310;
MGLOBAL(int, DAT_00912488, 0x00912488);
float10 FUN_004c3ac0(float *p0);
// MATCH: 0x00538310 FUN_00538310
N538310 * FUN_00538310(N538310 *param_1,int *param_2)
{
  int b1[6];
  int b2[6];
  float d[3];
  char *p;
  int *n;
  int ty;
  float sp;

  n = (int *)param_1;
  ty = param_1->type;
  if (ty < 0) {
    p = (char *)param_1 + DAT_00912488;
    { float *a = (float *)(p + 0x1c);
    *(int *)(p + 0x28) = param_2[3];
    *(int *)(p + 0x2c) = param_2[4];
    *(int *)(p + 0x30) = param_2[5];
    *(int *)(p + 0x1c) = param_2[0];
    *(int *)(p + 0x20) = param_2[1];
    *(int *)(p + 0x24) = param_2[2];
    *(int *)(p + 0x40) = n[0x15];
    *(int *)(p + 0x44) = n[0x16];
    *(int *)(p + 0x48) = n[0x17];
    *(int *)(p + 0x34) = n[0x12];
    *(int *)(p + 0x38) = n[0x13];
    *(int *)(p + 0x3c) = n[0x14];
    d[0] = a[0] - a[3];
    d[1] = a[1] - a[4];
    d[2] = a[2] - a[5];
    }
    { float len = (float)FUN_004c3ac0(d);
    *(float *)(p + 0x4c) = len; }
    return param_1;
  }
  sp = param_1->split;
  b1[3] = param_2[3];
  b1[4] = param_2[4];
  b1[5] = param_2[5];
  b1[0] = param_2[0];
  b1[1] = param_2[1];
  b1[2] = param_2[2];
  b2[0] = *(volatile int *)&param_2[0];
  b2[1] = *(volatile int *)&param_2[1];
  b2[2] = *(volatile int *)&param_2[2];
  b2[3] = *(volatile int *)&param_2[3];
  b2[4] = *(volatile int *)&param_2[4];
  b2[5] = *(volatile int *)&param_2[5];
  if (ty == 0) {
    *(float *)&b1[0] = sp;
    *(float *)&b2[3] = sp;
  }
  else if (ty == 4) {
    *(float *)&b1[1] = sp;
    *(float *)&b2[4] = sp;
  }
  else if (ty == 8) {
    *(float *)&b1[2] = sp;
    *(float *)&b2[5] = sp;
  }
  FUN_00538310(param_1->c0, b1);
  FUN_00538310(param_1->c1, b2);
  return param_1;
}
