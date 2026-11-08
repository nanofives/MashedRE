// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005439d0 (h2): 100.0% (regs 100). Raw: switch (state) { case 1: case 3: } form gives dec/je/sub 2/jne; CB3_t fn-ptr for DAT_007dc630. */
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
typedef unsigned int uint;
typedef void (__cdecl *CB3_t)(undefined4, int, undefined4);
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(int, DAT_007dc628, 0x007dc628);
MGLOBAL(CB3_t, DAT_007dc630, 0x007dc630);
void FUN_004f8670(undefined4 p0, uint p1);
uint FUN_004f8690(undefined4 p0);
// MATCH: 0x005439d0 FUN_005439d0
void FUN_005439d0(undefined4 param_1,int param_2,undefined4 param_3)
{
  int iVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;

  iVar3 = *(int *)(param_2 + 4);
  piVar4 = (int *)(param_2 + 0x48);
  do {
    int iVar4 = *(int *)(DAT_007dc5dc + *piVar4);
    if (iVar4 != 0) {
      switch (*(int *)(iVar4 + 0x30)) {
      case 1:
      case 3:
        if (DAT_007dc628 != 0) {
          uVar2 = FUN_004f8690(param_1);
          FUN_004f8670(param_1,uVar2 | 0x10000);
        }
      }
    }
    piVar4 = piVar4 + 9;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  (*DAT_007dc630)(param_1,param_2,param_3);
}
