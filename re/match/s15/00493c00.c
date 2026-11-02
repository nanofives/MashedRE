// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493c00 (h8): 100% (regs 100%). Constructor, thiscall(this, a, b) ret 8. C has no __thiscall (raw
   failed with C4234), so it is __fastcall with an unused edx parameter; the thiscall base FUN_0049ec10 (ret 0x10) is
   declared __fastcall the same way and gets our edx passed through (no edx load is emitted). Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(void *, PTR_LAB_005cf9d0, 0x005cf9d0);
MGLOBAL(void *, PTR_LAB_005cf9e8, 0x005cf9e8);
MGLOBAL(undefined, DAT_005cfaac, 0x005cfaac);
MGLOBAL(void *, PTR_FUN_005cfd30, 0x005cfd30);
MGLOBAL(void *, PTR_LAB_005d0e74, 0x005d0e74);
MGLOBAL(void *, PTR_LAB_005d0e88, 0x005d0e88);
void __fastcall FUN_0049ec10(void *this_, int edx_unused, void *p0, int p1, undefined4 p2, undefined4 *p3);
// MATCH: 0x00493c00 FUN_00493c00
undefined4 * __fastcall FUN_00493c00(undefined4 *param_1, int edx_unused, undefined4 param_2, undefined4 *param_3)
{
  FUN_0049ec10(param_1, edx_unused, &DAT_005cfaac, 0, param_2, param_3);
  *param_1 = (undefined4)&PTR_LAB_005cf9e8;
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
