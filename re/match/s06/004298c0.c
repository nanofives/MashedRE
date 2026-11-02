// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004298c0 (h9, by hand): 100% (regs 100%).
   Raw converter 83.3%: 0x0067d99c is a float, so its 0.0f store is `mov [m], 0` and not the shared eax zero.
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(float, DAT_0067d99c, 0x0067d99c);
MGLOBAL(int, DAT_0067d994, 0x0067d994);
MGLOBAL(int, DAT_0067d98c, 0x0067d98c);
MGLOBAL(int, DAT_008991bc, 0x008991bc);
// MATCH: 0x004298c0 FUN_004298c0
/* [C1 2026-05-15] No parameters. No callees. No branches. */
void FUN_004298c0(void)
{
  DAT_0067d99c = 0.0f;
  DAT_0067d994 = 0;
  DAT_0067d98c = 0;
  DAT_008991bc = 0;
}
