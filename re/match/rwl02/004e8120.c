// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e8120: best 50.2% (regs 43.4%), residual class b. structure only (724 B), raw converter form with C++ casts; 50.2. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7260, 0x007d7260);
MGLOBAL(int, DAT_006186bc, 0x006186bc);
MGLOBAL(int, DAT_006186c0, 0x006186c0);
MGLOBAL(int, DAT_006186c4, 0x006186c4);
MGLOBAL(int, DAT_007d725c, 0x007d725c);
MGLOBAL(undefined4, DAT_007d7234, 0x007d7234);
MGLOBAL(undefined4, DAT_006186a4, 0x006186a4);
undefined4 FUN_004c5a60(int p0);
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(uint p0, int p1, void * p2, void * p3);
int FUN_004cf5a0(undefined4 p0);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
int FUN_004d8000(void * p0, void * p1);
int FUN_004d8060(void * p0, void * p1);
undefined4 * FUN_004d8480(void * p0);
int FUN_004e1b60(void * p0, int p1, void * p2);
uint FUN_004e1c90(void * p0, int p1, void * p2, undefined4 p3);
// MATCH: 0x004e8120 FUN_004e8120
/* [C1 2026-05-18] Signature `int *FUN_004e8120(undefined4 stream)`. RpGeometry stream-read entry.
    */

int * FUN_004e8120(undefined4 param_1)

{
  short sVar1;
  int iVar2;
  int *piVar3;
  uint local_2c;
  int local_28 [3];
  int local_1c [4];
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar2 = FUN_004cc5e0(param_1,1,local_28,&local_2c);
  if (iVar2 == 0) {
    return (int *)0x0;
  }
  if ((local_2c < 0x35000) || (0x37002 < local_2c)) {
LAB_004e838b:
    local_28[1] = 2;
    local_28[2] = FUN_004d7ff0(0x80000004);
    FUN_004d8480(local_28 + 1);
    return (int *)0x0;
  }
  piVar3 = local_1c;
  for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
    *piVar3 = 0;
    piVar3 = piVar3 + 1;
  }
  iVar2 = FUN_004cbd30(param_1,local_1c,local_28[0]);
  if (iVar2 != local_28[0]) {
    return (int *)0x0;
  }
  piVar3 = (int *)(**(void * (**)(int,int))(DAT_007d3ff8 + 0x118))
                            (*(undefined4 *)(DAT_007d7260 + DAT_007d3ff8),0x30007);
  if (piVar3 == (int *)0x0) {
    return (int *)0x0;
  }
  *(undefined2 *)(piVar3 + 6) = 1;
  piVar3[1] = -1;
  *piVar3 = 0;
  piVar3[2] = 0;
  piVar3[3] = DAT_006186bc;
  piVar3[4] = DAT_006186c0;
  piVar3[5] = DAT_006186c4;
  FUN_004d8000(&DAT_006186a4,piVar3);
  piVar3[1] = local_1c[1];
  piVar3[3] = iStack_c;
  piVar3[4] = iStack_8;
  piVar3[5] = iStack_4;
  *piVar3 = 0;
  if (local_1c[3] != 0) {
    iVar2 = FUN_004cc5e0(param_1,6,0,&local_2c);
    if (iVar2 == 0) {
      sVar1 = (short)piVar3[6];
      if (sVar1 == 1) {
        FUN_004d8060(&DAT_006186a4,piVar3);
        if (*piVar3 != 0) {
          FUN_004c5a60(*piVar3);
        }
        *piVar3 = 0;
        (**(void (**)(int,void *))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7260 + DAT_007d3ff8),piVar3);
        return (int *)0x0;
      }
      goto LAB_004e83be;
    }
    if ((local_2c < 0x35000) || (0x37002 < local_2c)) {
      if ((short)piVar3[6] == 1) {
        FUN_004d8060(&DAT_006186a4,piVar3);
        if (*piVar3 != 0) {
          FUN_004c5a60(*piVar3);
        }
        *piVar3 = 0;
        (**(void (**)(int,void *))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7260 + DAT_007d3ff8),piVar3);
      }
      else {
        *(short *)(piVar3 + 6) = (short)piVar3[6] + -1;
      }
      goto LAB_004e838b;
    }
    iVar2 = FUN_004cf5a0(param_1);
    *piVar3 = iVar2;
  }
  DAT_007d725c = 0;
  DAT_007d7234 = 0;
  iVar2 = FUN_004e1b60(&DAT_006186a4,param_1,piVar3);
  if (iVar2 != 0) {
    if (DAT_007d725c != 0) {
      FUN_004e1c90(&DAT_006186a4,DAT_007d725c,piVar3,DAT_007d7234);
    }
    return piVar3;
  }
  sVar1 = (short)piVar3[6];
  if (sVar1 == 1) {
    FUN_004d8060(&DAT_006186a4,piVar3);
    if (*piVar3 != 0) {
      FUN_004c5a60(*piVar3);
    }
    *piVar3 = 0;
    (**(void (**)(int,void *))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7260 + DAT_007d3ff8),piVar3);
    return (int *)0x0;
  }
LAB_004e83be:
  *(short *)(piVar3 + 6) = sVar1 + -1;
  return (int *)0x0;
}
