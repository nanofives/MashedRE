// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad9d0 (h3): 88.9% exact, regs 88.9%. Residual class (b) epilogue layout: original puts a ret block right after the call (jne to the fixup tail); if(r!=0){..} and early-return forms both give je-to-end. Loops are while(a==0 && d!=0){a=[+0xc];d=[+0x18];} and *(p3+0x14) is read first into a temp. */
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
int FUN_005ad770(undefined4 p0, int p1, int p2, int p3, undefined4 p4);
// MATCH: 0x005ad9d0 FUN_005ad9d0
void FUN_005ad9d0(int param_1,int param_2,int param_3,int param_4,undefined4 param_5)
{
  int iVar1;
  undefined4 uVar2;
  uVar2 = *(undefined4 *)(param_3 + 0x14);
  while (param_1 == 0 && param_4 != 0) {
    param_1 = *(int *)(param_3 + 0xc);
    param_4 = *(int *)(param_3 + 0x18);
  }
  while (param_2 == 0 && param_4 != 0) {
    param_2 = *(int *)(param_3 + 0x10);
    param_4 = *(int *)(param_3 + 0x18);
  }
  iVar1 = FUN_005ad770(uVar2,param_1,param_2,param_4,param_5);
  if (iVar1 != 0) {
    *(int *)(iVar1 + 0x18) = param_3;
    *(uint *)(iVar1 + 0x14) = *(uint *)(iVar1 + 0x14) | 0x4000;
  }
}
