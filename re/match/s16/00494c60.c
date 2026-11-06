// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494c60 (h1): 100% (regs 100%), by hand. Residual: none.
   Scalar deleting destructor, __thiscall (ECX = this, 1 stack arg, ret 4): written as __fastcall with an
   unused EDX parameter. Calls the destructor body FUN_00494b20 (ECX = this) and the CRT free (0x004a3bda,
   kept by name, relocheck unchecked) when bit 0 of the flag is set. */
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
void __fastcall FUN_00494b20(void *self);
void __cdecl _free(void *_Memory);
// MATCH: 0x00494c60 FUN_00494c60
void * __fastcall FUN_00494c60(void *param_1, int edx_unused, byte param_2)
{
  FUN_00494b20(param_1);
  if ((param_2 & 1) != 0) {
    _free(param_1);
  }
  return param_1;
}
