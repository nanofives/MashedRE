// FLAGS: /O2 /Ob1 /G5
// best 91.9 (ops 100, regs 100). Residual: post-increment temp lands in ecx, original edx. Class (a)/(b): register choice, permuter 79 compiles no gain.
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
typedef unsigned char undefined1;
typedef struct _CRITICAL_SECTION CRITICAL_SECTION;
MGLOBAL(undefined4 *, PTR_FUN_006358fc, 0x006358fc);
MGLOBAL(int, DAT_007de0b8, 0x007de0b8);
MGLOBAL(undefined1, DAT_007de094, 0x007de094);
__declspec(dllimport) void __stdcall InitializeCriticalSection(CRITICAL_SECTION *p0);
undefined4 *FUN_005bf610(undefined4 *p0, int p1);
void FUN_005beb50(int p0);
uint *FUN_005ae650(int p0, uint p1, uint p2, int p3, undefined1 *p4, undefined4 p5);
// MATCH: 0x005beae0 FUN_005beae0
undefined4 *FUN_005beae0(undefined4 param_1, int param_2)
{
  undefined4 *puVar1;
  int iVar2;

  if (param_2 == 0) {
    param_2 = 0x158;
  }
  puVar1 = FUN_005bf610((undefined4 *)param_1, param_2);
  if (puVar1 != (undefined4 *)0x0) {
    FUN_005beb50((int)puVar1);
    *puVar1 = (undefined4)&PTR_FUN_006358fc;
    InitializeCriticalSection((CRITICAL_SECTION *)(puVar1 + 0x4b));
    iVar2 = DAT_007de0b8++;
    if (iVar2 == 0) {
      FUN_005ae650(0x14, 8, 1, 0, &DAT_007de094, 0x30800);
    }
  }
  return puVar1;
}
