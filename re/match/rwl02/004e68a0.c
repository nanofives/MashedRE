// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e68a0: hand-written. */
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
typedef struct V4 { int a, b, c, d; } V4;
void FUN_004c0e50(int p0);
int FUN_004e4440(int p0);
void FUN_004e8e90(int p0);
void FUN_004e8ea0(int p0);
// MATCH: 0x004e68a0 FUN_004e68a0
int FUN_004e68a0(int param_1, int param_2, unsigned char param_3)
{
  int iVar1;
  int iVar2;

  if (param_2 != *(int *)(param_1 + 0x18)) {
    if (param_2 != 0) {
      FUN_004e8e90(param_2);
    }
    if (*(int *)(param_1 + 0x18) != 0) {
      FUN_004e8ea0(*(int *)(param_1 + 0x18));
    }
    *(int *)(param_1 + 0x18) = param_2;
    if ((param_3 & 1) == 0) {
      if (param_2 != 0) {
        *(V4 *)(param_1 + 0x1c) = *(V4 *)(*(int *)(param_2 + 0x5c) + 4);
      }
      iVar1 = *(int *)(param_1 + 4);
      if (iVar1 != 0) {
        iVar2 = FUN_004e4440(param_1);
        if (iVar2 != 0) {
          FUN_004c0e50(iVar1);
        }
      }
    }
  }
  return param_1;
}

