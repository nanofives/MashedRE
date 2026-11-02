// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004943d0 (h8): 100% (regs 100%). Scalar deleting destructor (thiscall this, byte flag, ret 4). C has no
   __thiscall (raw failed with C4234), so it is written __fastcall with an unused edx parameter: ecx = this, the flag
   on the stack, same ret 4. FUN_00493c60 is __fastcall(this) (re/match/s15/00493c60.c). Residual: none. */
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
void __fastcall FUN_00493c60(void *param_1);
void __cdecl free(void *p);
// MATCH: 0x004943d0 FUN_004943d0
void * __fastcall FUN_004943d0(void *param_1, int edx_unused, byte param_2)
{
  FUN_00493c60(param_1);
  if ((param_2 & 1) != 0) {
    free(param_1);
  }
  return param_1;
}
