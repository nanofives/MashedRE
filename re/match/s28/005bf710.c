// FLAGS: /O2 /Ob1 /G5
// best 83.6 (ops 89.6). Residual: block layout of the tail: original places the success epilogue (mov eax,ebx) right after the FUN_005aeea0 test and the free call after it falling into the shared return-0 epilogue; ours has them the other way round. Tried nested/early-return/goto/or-form and permuter (123 compiles). Class (b).
// hand-fixed from raw (callee arities/types)
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
MGLOBAL(undefined *, PTR_LAB_00635a38, 0x00635a38);
MGLOBAL(undefined *, PTR_LAB_00635a80, 0x00635a80);
undefined4 *FUN_005aea00(int p0, int p1);
uint FUN_005aeea0(undefined4 *p0, int p1, int p2);
int FUN_005c0140(undefined4 p0, undefined4 *p1, int p2);
int __stdcall FUN_005c02c0(int p0);
// MATCH: 0x005bf710 FUN_005bf710
int FUN_005bf710(undefined4 param_1, undefined4 param_2, undefined4 param_3, int param_4,
                 undefined4 *param_5, int param_6)
{
  int iVar1;
  int iVar2;
  undefined4 uVar3;

  uVar3 = 0;
  if (param_6 == 0) {
    param_6 = 0x164;
  }
  if (param_5 == (undefined4 *)0x0) {
    param_5 = FUN_005aea00(param_6, 0x30800);
    uVar3 = 1;
  }
  if (param_5 != (undefined4 *)0x0) {
    iVar1 = FUN_005c0140(param_1, param_5 + 1, param_6);
    if (iVar1 != 0) {
      param_5[0x51] = param_2;
      *param_5 = (undefined4)&PTR_LAB_00635a80;
      param_5[1] = (undefined4)&PTR_LAB_00635a38;
      param_5[0x50] = 0;
      param_5[0x52] = param_4;
      param_5[0x53] = param_3;
      param_5[0x58] = 0;
      param_5[0x57] = 0;
      param_5[3] = uVar3;
      if ((param_4 != 2) || (iVar2 = FUN_005aeea0(param_5 + 0x54, 0, 1), iVar2 != 0)) {
        return iVar1;
      }
      FUN_005c02c0(iVar1);
    }
  }
  return 0;
}
