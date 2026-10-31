// FLAGS: /O2 /GS /Ob1
/* 0x0040cea0 (158 B): best 82.8% exact, regs 87.5%, ops 96.9%. Raw g2s 78.1%.
   Form: open toastart/vehicles/dirt.piz, two name lookups on *param_1[param_2] (FUN_00404e00 /
   FUN_00404e20), FUN_004c5cb0 each if non-zero, publish to 0x0063ba90 / 0x0063ba94, then
   FUN_0041ecc0(i, tex1, tex2) for i < 4 when both are set. Init order tex1 = 0; tex2 = 0 (permuter).
   Residual (b) register allocation: the original loads param_2 (the index) into ebp at entry and
   reloads param_1 (the base) from the stack each time; 9176 makes the opposite choice (ebp = param_1),
   which also moves the loop alignment pad (lea ebx,[ebx]) and the branch targets. Tried: int base with
   Ghidra's byte arithmetic, param_2[param_1], *(param_2 + param_1), param_1[param_2][0], a copied base
   local, a pointer-to-array base type, call/assignment orders, do-while and early-return loop forms;
   permuter 2 runs (429 compiles) found only the init order. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063ba90, 0x0063ba90);
MGLOBAL(int, DAT_0063ba94, 0x0063ba94);

int FUN_00404e00(int p0);
int FUN_00404e20(int p0, undefined4 p1);
void FUN_0041ecc0(int p0, int p1, int p2);
int FUN_0042a8d0(const char *p0);
void FUN_00495280(int p0);
void FUN_004952f0(void);
void FUN_004c5c80(int p0);
int FUN_004c5cb0(int p0, int p1);

// MATCH: 0x0040cea0 FUN_0040cea0
void FUN_0040cea0(int **param_1, int param_2, undefined4 param_3)
{
  int name1;
  int name2;
  int tex1;
  int tex2;
  int i;

  tex1 = 0;
  tex2 = 0;
  FUN_004c5c80(0);
  FUN_00495280(FUN_0042a8d0("toastart/vehicles/dirt.piz"));
  name1 = FUN_00404e00(*param_1[param_2]);
  name2 = FUN_00404e20(*param_1[param_2], param_3);
  if (name1 != 0) {
    tex1 = FUN_004c5cb0(name1, 0);
  }
  if (name2 != 0) {
    tex2 = FUN_004c5cb0(name2, 0);
  }
  FUN_004952f0();
  DAT_0063ba90 = tex1;
  DAT_0063ba94 = tex2;
  if (tex1 != 0 && tex2 != 0) {
    for (i = 0; i < 4; i++) {
      FUN_0041ecc0(i, tex1, tex2);
    }
  }
}
