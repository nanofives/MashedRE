// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e8750: hand-written + permuter. */
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
int FUN_004d7ff0(int p0, int p1);
void FUN_004d8480(int * p0);
typedef int (*AllocFn)(int, int);
typedef int (*ReallocFn)(int, int, int);
// MATCH: 0x004e8750 FUN_004e8750
int FUN_004e8750(int param_1, int param_2)
{
  int iVar1;
  int iVar2;
  int *piVar3;
  unsigned char *puVar4;
  unsigned char *puVar5;
  int iVar6;
  int iVar7;
  int uStack[2];
  unsigned int uFlags;

  uFlags = *(unsigned int *)(param_1 + 8);
  if (!((uFlags & 0x1000000) == 0)) {
    iVar6 = 0x1c;
  } else {
    iVar7 = *(int *)(param_1 + 0x14) * 0xc;
    iVar6 = iVar7 + 0x1c;
    if ((uFlags & 0x10) != 0) {
      iVar6 = iVar6 + iVar7;
    }
  }
  iVar7 = (*(int *)(param_1 + 0x18) + param_2) * iVar6;
  if (*(int *)(param_1 + 0x5c) != 0) {
    iVar1 = (*(ReallocFn *)(DAT_007d3ff8 + 0x110))(*(int *)(param_1 + 0x5c), iVar7, 0x3000f);
    if (iVar1 == 0) {
      uStack[0] = 2;
      uStack[1] = FUN_004d7ff0(0x80000013, iVar7);
      FUN_004d8480(uStack);
      return -1;
    }
    puVar4 = (unsigned char *)(*(int *)(param_1 + 0x18) * iVar6 + -1 + iVar1);
    puVar5 = puVar4 + param_2 * 0x1c;
    for (iVar6 = (iVar6 + -0x1c) * *(int *)(param_1 + 0x18); iVar6 != 0; iVar6 = iVar6 + -1) {
      *puVar5 = *puVar4;
      puVar5 = puVar5 + -1;
      puVar4 = puVar4 + -1;
    }
  }
  else {
    iVar1 = (*(AllocFn *)(DAT_007d3ff8 + 0x108))(iVar7, 0x3000f);
    if (iVar1 == 0) {
      uStack[0] = 2;
      uStack[1] = FUN_004d7ff0(0x80000013, iVar7);
      FUN_004d8480(uStack);
      return -1;
    }
  }
  iVar6 = *(int *)(param_1 + 0x18) + param_2;
  iVar7 = 0;
  *(int *)(param_1 + 0x18) = iVar6;
  *(int *)(param_1 + 0x5c) = iVar1;
  iVar1 = iVar1 + iVar6 * 0x1c;
  if (0 < iVar6) {
    iVar6 = 0;
    do {
      iVar2 = *(int *)(param_1 + 0x5c);
      iVar2 = iVar2 + iVar6;
      *(int *)(iVar2 + 0x18) = 0;
      *(int *)(iVar2 + 0x14 + iVar6) = 0;
      uFlags = *(unsigned int *)(param_1 + 8);
      if (((uFlags & 0x1000000) == 0) && (*(int *)(param_1 + 0x14) != 0)) {
        *(int *)(iVar2 + 0x14) = iVar1;
        iVar1 = iVar1 + *(int *)(param_1 + 0x14) * 0xc;
        if ((*(unsigned char *)(param_1 + 8) & 0x10) != 0) {
          *(int *)(iVar2 + 0x18) = iVar1;
          iVar1 = iVar1 + *(int *)(param_1 + 0x14) * 0xc;
        }
      }
      iVar7 = iVar7 + 1;
      iVar6 = iVar6 + 0x1c;
    } while (iVar7 < *(int *)(param_1 + 0x18));
  }
  iVar6 = *(int *)(param_1 + 0x18) - param_2;
  if (iVar6 < *(int *)(param_1 + 0x18)) {
    iVar7 = iVar6 * 0x1c;
    do {
      iVar1 = *(int *)(param_1 + 0x5c);
      *(int *)(iVar1 + 4 + iVar7) = 0;
      piVar3 = (int *)(iVar1 + iVar7);
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 0x1c;
      piVar3[2] = 0;
      piVar3[3] = 0;
      piVar3[4] = 0;
      *piVar3 = param_1;
    } while (iVar6 < *(int *)(param_1 + 0x18));
  }
  return *(int *)(param_1 + 0x18) - param_2;
}


