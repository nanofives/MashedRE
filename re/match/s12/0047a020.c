// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a020: best 98.0% (regs 98.0%, ops 100%) by hand (h6), raw converter 36.2%. match.py's
   Ghidra size (196 B) stops 3 B before the real end (mov eax, ebp / pop ebp / ret, 199 B), so match.py reports
   92.2%; scored over 199 B: 98.0%, 13 differing bytes in the loop body.
   Fixes: the &param_2 Ghidra shows is a separate dword local in param_2's dead home slot (pass &local, return
   param_2); DAT_0086cac0 is a 32 x 0x110 B struct array written by an int index loop (the compiler biases the
   pointer at +0xe4 and compares signed against 0x0086eda4); float fields stored as float constants.
   Tried: pointer loops (45-56%), 6 source orders of the per-entry stores, chained assignments, permuter
   312 compiles. Residual (b): the store of 0.5f to +0xf4 is scheduled second in the original (after +0x0) and
   fifth in ours (after the +0xe8..+0xf0 zeros) whatever the source order; struct layout of the 0x110 B entry
   not explored further. */
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
typedef struct {
  int f0;
  char pad04[0xe0];
  float fe4;
  float fe8[3];
  float ff4;
  float ff8;
  float ffc;
  char pad100[0x10];
} Entry110;
MGLOBAL(undefined4, DAT_006bf1c8, 0x006bf1c8);
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
MGLOBAL(int, DAT_006bf1d0, 0x006bf1d0);
MGLOBAL(undefined4, DAT_006bf1d4, 0x006bf1d4);
MGLOBALA(Entry110, DAT_0086cac0, 32, 0x0086cac0);
void FUN_00440bc0(void);
void FUN_00478cb0(undefined4 p0);
undefined4 FUN_0047b9b0(undefined4 p0, void *p1, void (*p2)(void));
void FUN_004b6520(void *p0, int p1);
// MATCH: 0x0047a020 FUN_0047a020
undefined4 FUN_0047a020(undefined4 param_1,undefined4 param_2,int param_3)
{
  undefined4 local;
  int iVar2;
  int i;

  iVar2 = param_3;
  DAT_006bf1c8 = param_2;
  DAT_006bf1cc = param_3;
  DAT_006bf1d0 = 0;
  DAT_006bf1d4 = 0;
  FUN_00478cb0(param_2);
  FUN_004b6520((void *)iVar2,0x21c);
  *(int *)(iVar2 + 0x204) = 0;
  *(float *)(iVar2 + 0x208) = 1.0f;
  *(float *)(iVar2 + 0x20c) = 1.0f;
  *(float *)(iVar2 + 0x210) = 1.0f;
  FUN_004b6520(DAT_0086cac0,0x2200);
  for (i = 0; i < 32; i++) {
    DAT_0086cac0[i].f0 = 0;
    DAT_0086cac0[i].fe4 = 1.0f;
    DAT_0086cac0[i].fe8[0] = 0.0f;
    DAT_0086cac0[i].fe8[1] = 0.0f;
    DAT_0086cac0[i].fe8[2] = 0.0f;
    DAT_0086cac0[i].ff4 = 0.5f;
    DAT_0086cac0[i].ff8 = 0.5f;
    DAT_0086cac0[i].ffc = 0.01f;
  }
  FUN_0047b9b0(param_1,&local,FUN_00440bc0);
  return param_2;
}
