// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ccef0: RO-AUDIOINIT-45D460-REACH-5, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the listing 0x004ccef0..0x004ccefd (14 B, pc_funcs.csv;
   hooks.csv C1, no re/match source before this row): mov eax,[esp+4] / push eax / call 0x004a45fb / add esp,4 / ret.
   0x005aea60 stores its address at RwGlobals+0x108 (re/match/s26/005aea60.c). */
void *FUN_004a45fb(unsigned int n);

// MATCH: 0x004ccef0 FUN_004ccef0
void *FUN_004ccef0(unsigned int param_1)
{
  return FUN_004a45fb(param_1);
}
