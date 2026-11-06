// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045dff0 (h3): 100% (regs 100%), byte-identical. Raw converter did not compile (__thiscall). Not a
   thiscall: the push ecx is the float local's slot. FUN_00408ad0 is cdecl with ONE argument (Ghidra's two-argument
   first call is the deferred add esp,8 merge). Constants -15.0f @0x005ce6b4, 10.0f @0x005cc55c. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
float FUN_00408ad0(int p0);
// MATCH: 0x0045dff0 FUN_0045dff0
int FUN_0045dff0(int param_1)
{
  float a;
  float d;

  a = FUN_00408ad0(0);
  d = a - FUN_00408ad0(param_1);
  if (d > -15.0f && d < 10.0f) {
    return 1;
  }
  return 0;
}
