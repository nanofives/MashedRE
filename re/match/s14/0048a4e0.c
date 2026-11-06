// FLAGS: /O2 /GS /Ob1
/* 0x0048a4e0: 98.8% (regs 98.8%, ops 98.8%), 84/84 ins. Raw converter 40.0%. By hand: FUN_00472650 is a
   2-arg cdecl float(lo, hi) (Ghidra had a fastcall ECX arg: its `push ecx` is a local), FUN_00490020 takes 7
   args (param_2 is its 7th, pushed before the FUN_00472650 args, whose slot takes the fstp result), RGBA
   local, i kept in memory by an `if (count > 0) do { } while (i < count)` loop with i = 0 at the top (a for
   loop puts i in a register: 64%). Residual, 1 ins, class (b): the guard is `test ecx,ecx` here, the
   original `cmp ecx,ebp` (ebp = n = 0). Tried count > n, 0 < count, i < count, a zero local, comma
   expression guard, for(; i < count;) (97.6%), x != n; permuter 173 compiles no gain. */
/* MATCH-S14 0x0048a4e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef struct { uchar r, g, b, a; } Rgba;
float FUN_00472650(float lo, float hi);
void FUN_00490020(float *pos, Rgba *color, int p3, int p4, int p5, float p6, float *p7);
int FUN_004b40f0(int p0);
int FUN_004b4120(int p0);
int FUN_004c0ed0(int p0);
void FUN_004c3d60(float *out, int in, int mtx);
// MATCH: 0x0048a4e0 FUN_0048a4e0
void FUN_0048a4e0(int param_1, float *param_2)
{
  int tex;
  int x;
  int count;
  int i;
  int n;
  int v;
  Rgba color;
  float pos[3];

  n = 0;
  color.r = 0xff; color.g = 0xff; color.b = 0xff; color.a = 0xff;
  i = 0;
  tex = FUN_004b4120(param_1);
  FUN_004c0ed0(*(int *)(param_1 + 4));
  x = FUN_004b40f0(param_1);
  if (x != 0) {
    color = *(Rgba *)(x + 4);
  }
  color.a = 0xff;
  count = *(int *)(*(int *)(param_1 + 0x18) + 0x14);
  v = *(int *)(*(int *)(*(int *)(param_1 + 0x18) + 0x5c) + 0x14);
  if (count > 0) {
   do {
    if (i % 4 == 0) {
      FUN_004c3d60(pos, v, FUN_004c0ed0(*(int *)(param_1 + 4)));
      FUN_00490020(pos, &color, 0, 0, tex, FUN_00472650(0.8f, 1.3f), param_2);
      n++;
    }
    if (n >= 16) {
      return;
    }
    i++;
    v += 0xc;
   } while (i < count);
  }
}
