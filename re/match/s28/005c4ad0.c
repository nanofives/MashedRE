// FLAGS: /O2 /Ob1 /G5
// best 96.7 (ops 100). Residual: imul operand order ([esi+0xc]*[esi+8] vs original [esi+8]*[esi+0xc]); swapping source order, temp, permuter (148 compiles) did not move it. Class (a). ErrRec 8-byte stack struct is the fix for the sub esp,8 frame.
/* MATCH-S28 0x005c4ad0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef void *(*AllocFn)(int, unsigned int);
typedef void (*FreeFn)(void *);
typedef struct { undefined4 a; undefined4 b; } ErrRec;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 *FUN_004d8480(ErrRec *p0);
// MATCH: 0x005c4ad0 FUN_005c4ad0
int *FUN_005c4ad0(int param_1, uint param_2)
{
  int *piVar1;
  int iVar2;
  ErrRec e;

  piVar1 = (int *)(*(AllocFn *)(DAT_007d3ff8 + 0x108))(0x10, 0x30000);
  if (piVar1 == (int *)0x0) {
    e.a = 1;
    e.b = FUN_004d7ff0(0x80000013, 0x10);
    FUN_004d8480(&e);
    return (int *)0x0;
  }
  piVar1[3] = param_1;
  piVar1[1] = 0;
  piVar1[2] = 0x14;
  iVar2 = (int)(*(AllocFn *)(DAT_007d3ff8 + 0x108))(param_1 * 0x14, param_2 | 0x1000000);
  *piVar1 = iVar2;
  if (iVar2 == 0) {
    e.a = 1;
    e.b = FUN_004d7ff0(0x80000013, piVar1[3] * piVar1[2]);
    FUN_004d8480(&e);
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(piVar1);
    return (int *)0x0;
  }
  return piVar1;
}