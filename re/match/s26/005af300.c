// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af300: 100% by hand: single exit (ret variable, ret=7 via goto done), outer switch lists dead case 6 as its own ret=0 (jump table 2..6), inner switch on +0x38 cases 1/2/3, node temp assigned inside the call arg ((node = p-0xb), node[10]) so arg2 loads before the lea, flag=1 before ret=6. cxx pending. */
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
int FUN_005b1110(int p0, int p1, int p2);
// MATCH: 0x005af300 FUN_005af300
int FUN_005af300(int param_1)
{
  int *piVar1;
  int ret;
  int flag;
  int iVar3;
  int *piVar4;
  int *node;

  switch(*(int *)(param_1 + 8)) {
  case 2:
  case 3:
  case 4:
    ret = 2;
    break;
  case 5:
    piVar4 = *(int **)(param_1 + 0x20);
    piVar1 = (int *)(param_1 + 0x20);
    switch (*(int *)(param_1 + 0x38)) {
    case 1:
      ret = 3;
      if (piVar4 != piVar1) {
        do {
          if (FUN_005b1110((int)(node = piVar4 - 0xb),node[10],0) != 3) {
            ret = 8;
            break;
          }
          piVar4 = (int *)*piVar4;
        } while (piVar4 != piVar1);
      }
      break;
    case 2:
      flag = 1;
      ret = 6;
      if (piVar4 != piVar1) {
        do {
          iVar3 = FUN_005b1110((int)(node = piVar4 - 0xb),node[10],0);
          if (iVar3 == 7) {
            ret = 7;
            goto done;
          }
          if (iVar3 != 3) {
            flag = 0;
          }
          piVar4 = (int *)*piVar4;
        } while (piVar4 != piVar1);
      }
      if (flag == 1) {
        ret = 3;
      }
      break;
    case 3:
      ret = 5;
      if (piVar4 != piVar1) {
        do {
          if (FUN_005b1110((int)(node = piVar4 - 0xb),node[10],0) != 5) {
            ret = 4;
            break;
          }
          piVar4 = (int *)*piVar4;
        } while (piVar4 != piVar1);
      }
      break;
    default:
      ret = 0;
      break;
    }
    break;
  case 6:
    ret = 0;
    break;
  default:
    ret = 0;
    break;
  }
done:
  return ret;
}
