// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494ac0 (h1): 100% (regs 100%), by hand. Residual: none.
   The original is __thiscall (ECX = this, 2 stack args, ret 8). Written as __fastcall with an unused second
   (EDX) parameter, which gives the same entry and ret 8. The base-class constructor FUN_0049ec10 is also
   __thiscall (4 stack args): declared the same way and passed the incoming EDX through, which costs no code.
   Raw g2s did not compile (__thiscall). */
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
MGLOBAL(undefined *, PTR_LAB_005cf9d0, 0x005cf9d0);
MGLOBAL(undefined1, DAT_005cfaac, 0x005cfaac);
MGLOBAL(undefined *, PTR_FUN_005cfd30, 0x005cfd30);
MGLOBAL(undefined *, PTR_LAB_005cfd70, 0x005cfd70);
MGLOBAL(undefined *, PTR_LAB_005d0e74, 0x005d0e74);
MGLOBAL(undefined *, PTR_LAB_005d0e88, 0x005d0e88);
/* thiscall callee: ECX = this, EDX unused, 4 stack args (ret 0x10) */
undefined4 * __fastcall FUN_0049ec10(undefined4 *self, int edx_unused, void *param_2, undefined4 param_3,
                                     undefined4 param_4, undefined4 *param_5);
// MATCH: 0x00494ac0 FUN_00494ac0
undefined4 * __fastcall FUN_00494ac0(undefined4 *param_1, int edx_unused, undefined4 param_2, undefined4 *param_3)
{
  FUN_0049ec10(param_1, edx_unused, &DAT_005cfaac, 0, param_2, param_3);
  *param_1 = (undefined4)&PTR_LAB_005cfd70;
  param_1[3] = (undefined4)&PTR_FUN_005cfd30;
  param_1[4] = (undefined4)&PTR_LAB_005cf9d0;
  param_1[0x38] = (undefined4)&PTR_LAB_005d0e88;
  param_1[0x39] = (undefined4)&PTR_LAB_005d0e74;
  param_1[0x58] = 0;
  if (param_3 != (undefined4 *)0x0) {
    *param_3 = 0;
  }
  return param_1;
}
