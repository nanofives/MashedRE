// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e5020: hand-written + permuter. 70.5%. Residual (b): original puts the generic-frame (child) path before the -1 path and the -2 path last, with ebp=0 hoisted for both; ours orders blocks differently (permuter 316 compiles). */
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
MGLOBAL(unsigned int, DAT_006189e4, 0x006189e4);
void FUN_004d8060(void * p0, int * p1);
void FUN_004d8bd0(int p0);
void FUN_004e33c0(int * p0);
void FUN_004e3410(int * p0);
void FUN_004f0c10(int p0);
void FUN_004e5020(int * param_1);
typedef void (*FreeFn)(int);
// MATCH: 0x004e5020 FUN_004e5020
void FUN_004e5020(int *param_1)
{
  int *piVar1;
  int iVar2;
  int *piVar3;

  if (*param_1 == -2) {
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))((int)param_1);
  } else if (!(*param_1 != -1)) {
    if (param_1[0xd] != 0) {
      FUN_004d8bd0(param_1[0xd]);
    }
    piVar3 = (int *)param_1[0xe];
    while (piVar3 != param_1 + 0xe) {
      piVar1 = (int *)*piVar3;
      piVar3 = piVar1;
      FUN_004e3410(piVar3);
    }
    piVar3 = (int *)param_1[0x10];
    while (piVar3 != param_1 + 0x10) {
      piVar1 = (int *)*piVar3;
      piVar3 = piVar1;
      FUN_004e33c0(piVar3);
    }
    FUN_004d8060(&DAT_006189e4, param_1);
    if (param_1[2] != 0) {
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(param_1[2]);
      param_1[2] = 0;
    }
    if (param_1[3] != 0) {
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(param_1[3]);
      param_1[3] = 0;
    }
    if (param_1[0xc] != 0) {
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(param_1[0xc]);
      param_1[0xc] = 0;
    }
    if (param_1[1] != 0) {
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(param_1[1]);
      param_1[1] = 0;
    }
    piVar3 = param_1 + 4;
    iVar2 = 8;
    do {
      if (*piVar3 != 0) {
        (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(*piVar3);
        *piVar3 = 0;
      }
      piVar3 = piVar3 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (param_1[0x1e] != 0) {
      FUN_004f0c10(param_1[0x1e]);
      param_1[0x1e] = 0;
    }
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))((int)param_1);
    return;
  } else {
    if (param_1[2] != 0) {
      FUN_004e5020((int *)param_1[2]);
    }
    param_1[2] = 0;
    if (param_1[3] != 0) {
      FUN_004e5020((int *)param_1[3]);
    }
    param_1[3] = 0;
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))((int)param_1);
  }
}


