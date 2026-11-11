// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e7cc0: best 90.9% (regs 90.9%), residual class a. callee-saved edi is pushed in the prologue by the original, ours sinks the push past the first call (shrink-wrap class); permuter flat at 90.9. */
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

MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
void FUN_004c0790(int p0);
void FUN_004c0e50(int p0);
int FUN_004d8060(undefined4 *p0, int p1);
undefined4 FUN_004d8bd0(int p0);
undefined4 FUN_004e4440(int p0);
undefined4 FUN_004e8ea0(int p0);
// MATCH: 0x004e7cc0 FUN_004e7cc0
int FUN_004e7cc0(int param_1)
{
  int iVar1;
  int iVar2;
  FUN_004d8060(&DAT_00618664,param_1);
  if (*(int *)(param_1 + 0x14) != 0) {
    FUN_004d8bd0(*(int *)(param_1 + 0x14));
  }
  if (*(int *)(param_1 + 0x18) != 0) {
    FUN_004e8ea0(*(int *)(param_1 + 0x18));
    iVar1 = *(int *)(param_1 + 4);
    *(undefined4 *)(param_1 + 0x18) = 0;
    if (iVar1 != 0) {
      iVar2 = FUN_004e4440(param_1);
      if (iVar2 != 0) {
        FUN_004c0e50(iVar1);
      }
    }
  }
  FUN_004c0790(param_1);
  (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d7200 + DAT_007d3ff8),param_1);
  return param_1;
}
