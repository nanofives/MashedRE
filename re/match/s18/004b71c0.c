// FLAGS: /O2 /Ob1 /G5
/* MATCH-S18 0x004b71c0 (h4): 54.2%, regs 66.7% with /O2 /Ob1 /G5 (52.0% with /O2 /GS /Ob1). By hand: if (s == NULL) FUN_004b7110(L) else FUN_004b7180(L, s, strlen(s)) (the Lua 4.0 lua_pushstring shape). Residual: the original inlines strlen as or ecx,-1 / xor eax,eax / repne scasb / not ecx / dec ecx. 9176 emits a byte loop under every flag set tried (/O2, /Ox, /O1, /Os, /Og /Oi /Os /Oy, /G3../G7, flagscan). The repne scasb strlen sequence occurs 16 times in .text, only here, in Lua (0x004b85fc, 0x004b9aa7) and in library code at 0x00516xxx..0x0059xxxx, never in game code (0x004b6940 in this slice inlines strcpy as the 9176 byte loop). Class (a): codegen of a different compiler build for the Lua TU. */
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
void FUN_004b7110(void *p0);
void FUN_004b7180(void *p0, const char *p1, unsigned int p2);
unsigned int strlen(const char *);
#pragma intrinsic(strlen)

// MATCH: 0x004b71c0 FUN_004b71c0
void FUN_004b71c0(void *param_1, const char *param_2)
{
  if (param_2 == 0) {
    FUN_004b7110(param_1);
  } else {
    FUN_004b7180(param_1, param_2, strlen(param_2));
  }
}
