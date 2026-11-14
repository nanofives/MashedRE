// FLAGS: /O2 /Ob1 /G5 /Oy-
/* RWC-DRIVER 2026-11-11: 18.6% -> 64.9% (56.1% under plain /O2 /Ob1 /G5). D3D SetTransform(state, matrix) cache, sibling of s20/004cb4c0. The ebp frame is a compiler flag: the original's frame holds one 4-byte slot ([ebp-4], the spilled compare counter of the matrix arm) and nothing in the body needs a frame (no alloca, no local array, no setjmp, no __try), so /Oy- as in 004cb4c0. Rewritten: Mat16 struct, cache array 0x007d4158[state], dword compare as static __inline helper, rep movsd struct copy, +0xb0 __stdcall, flag 0x007d4154 = 0 (matrix) / 1 (identity 0x005d8b90) when state == 0x100. Residual: the matrix arm keeps param_2 in ebx for the whole arm; the original uses no ebx, spills the counter to [ebp-4] and reloads param_2 from its slot. Permuter 264 compiles: no gain. */
/* h3 hand-match: best 24.6% (regs 45.6%), class b. Not worked beyond raw output (budget spent on higher-yield rows). Two source copies are visible in the original (param_2 given vs default palette 0x005d8b90, different register allocation, flag 0x007d4154 = 0 vs 1 only when index 0x100): needs a hand rewrite as two specialised bodies; compare loop keeps its counter in [ebp-4] in the first copy. */
/* MATCH-RWL01 0x004cb330: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
typedef struct { int m[16]; } Mat16;
typedef long (__stdcall *setxf_fn)(int *, int, Mat16 *);
MGLOBAL(undefined4, DAT_007d4568, 0x007d4568);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d4154, 0x007d4154);
typedef __declspec(align(8)) struct { int m[16]; } Mat16A;
MGLOBAL(Mat16A, DAT_005d8b90, 0x005d8b90);
MGLOBALA(Mat16 *, DAT_007d4158, 0x104, 0x007d4158);
static __inline int s_cmp(const int *a, const int *b, unsigned int n)
{
  while (n != 0) {
    if (*a != *b) return 1;
    a++;
    b++;
    n--;
  }
  return 0;
}
static __inline int s_cmp2(const int *a, const int *b, unsigned int n)
{
  while (n != 0) {
    if (*a != *b) return 1;
    b++;
    a++;
    n--;
  }
  return 0;
}
// MATCH: 0x004cb330 FUN_004cb330
int FUN_004cb330(int param_1,Mat16 *param_2)
{
  long r;
  if (param_2 != 0) {
    if (DAT_007d4158[param_1] != 0 && s_cmp2(DAT_007d4158[param_1]->m, param_2->m, 16) == 0) {
      return 1;
    }
    if (DAT_007d4158[param_1] == 0) {
      DAT_007d4158[param_1] = (Mat16 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d4568,0x30411);
    }
    *DAT_007d4158[param_1] = *param_2;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110,param_1,param_2);
    if (param_1 == 0x100) {
      DAT_007d4154 = 0;
    }
  }
  else {
    if (DAT_007d4158[param_1] != 0 && s_cmp(DAT_007d4158[param_1]->m, DAT_005d8b90.m, 16) == 0) {
      return 1;
    }
    if (DAT_007d4158[param_1] == 0) {
      DAT_007d4158[param_1] = (Mat16 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d4568,0x30411);
    }
    *(Mat16A *)DAT_007d4158[param_1] = DAT_005d8b90;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110,param_1,(Mat16 *)&DAT_005d8b90);
    if (param_1 == 0x100) {
      DAT_007d4154 = 1;
    }
  }
  return r >= 0;
}
