// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e19f0: 100% regs 100%. Single local reused for the second object (failure path frees the reassigned value). class -. */
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
typedef unsigned char undefined;
typedef unsigned int undefined4;
typedef unsigned int uint;
int FUN_004cd550(int p0, undefined4 p1);
int FUN_004d4170(void);
void FUN_004d41e0(int p0);
int FUN_004d4380(int p0);
int FUN_004d4dd0(int p0);
int FUN_004d4f90(int p0, int p1, int p2);
undefined4 FUN_004e0920(void);
int FUN_004e12a0(int p0);
// MATCH: 0x004e19f0 FUN_004e19f0
undefined4 FUN_004e19f0(int *param_1)
{
  int iVar1;
  int iVar2;
  int uVar3;

  iVar1 = FUN_004d4170();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x2c) = 1;
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_004e12a0(0);
      uVar3 = FUN_004d4f90(iVar2,0,uVar3);
      iVar1 = FUN_004d4380(uVar3);
      if (iVar1 != 0) {
        *param_1 = iVar1;
        param_1[1] = iVar1;
        param_1[2] = iVar1;
        param_1[3] = iVar1;
        param_1[4] = iVar1;
        FUN_004cd550(iVar1,3);
        FUN_004cd550(iVar1,5);
        FUN_004cd550(iVar1,4);
        FUN_004cd550(iVar1,1);
        FUN_004cd550(iVar1,2);
        uVar3 = FUN_004e0920();
        return uVar3;
      }
    }
    FUN_004d41e0(iVar1);
  }
  return 0;
}
