// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004ebc30: hand-written. */
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
void FUN_004cba80(int p0);
void FUN_004dc750(int p0, int p1, int p2, int p3);
void FUN_004dcb80(int p0);
void FUN_004f4270(char * p0);
// MATCH: 0x004ebc30 FUN_004ebc30
void FUN_004ebc30(int param_1)
{
  char cVar1;
  int iVar2;
  unsigned int uVar3;
  char *pcVar4;
  int iVar5;
  int *piVar6;
  char *pcVar7;

  pcVar7 = *(char **)(param_1 + 0xc);
  if ((pcVar7 != (char *)0) && (cVar1 = *pcVar7, cVar1 != -1)) {
    uVar3 = 0;
    pcVar4 = (char *)(param_1 + 0x36);
    do {
      if (*pcVar4 == '\0') {
        if (cVar1 != 8) {
          if (cVar1 != 1) {
            break;
          }
          pcVar7 = *(char **)(pcVar7 + 0x18);
        }
        FUN_004f4270(pcVar7);
        break;
      }
      uVar3 = uVar3 + 1;
      pcVar4 = pcVar4 + 0x10;
    } while (uVar3 < 2);
  }
  piVar6 = *(int **)(param_1 + 0x20);
  if (piVar6 != (int *)0) {
    (**(void (**)(int *))(*piVar6 + 8))(piVar6);
    *(int *)(param_1 + 0x20) = 0;
  }
  if (*(int *)(param_1 + 0x4c) != 0) {
    FUN_004cba80(*(int *)(param_1 + 0x4c));
    *(int *)(param_1 + 0x4c) = 0;
  }
  piVar6 = (int *)(param_1 + 0x2c);
  iVar5 = 2;
  do {
    iVar2 = piVar6[-1];
    if (iVar2 != 0) {
      if (*(char *)((int)piVar6 + 10) != '\0') {
        FUN_004dc750(piVar6[1], *(int *)(param_1 + 0x54) * piVar6[1], iVar2, *piVar6);
      }
      else {
        if ((*piVar6 == 0) && (*(char *)((int)piVar6 + 0xb) == '\0')) {
          FUN_004dcb80(iVar2);
        }
      }
      piVar6[-1] = 0;
    }
    *piVar6 = 0;
    piVar6[1] = 0;
    *(short *)(piVar6 + 2) = 0;
    *(char *)((int)piVar6 + 10) = 0;
    *(char *)((int)piVar6 + 0xb) = 0;
    piVar6 = piVar6 + 4;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
}

