// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00427580 (h1, by hand): 100% / regs 100%, whole 65 B obj symbol BYTE-IDENTICAL.
   Moves: float globals, the .rdata constant on the left of both multiplies (relocheck BAD otherwise), and the
   last store reads the just-written DAT_008991cc back (fsub from memory). Residual: none. */
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
MGLOBAL(float, DAT_008991c0, 0x008991c0);
MGLOBAL(float, DAT_008991c4, 0x008991c4);
MGLOBAL(float, DAT_008991c8, 0x008991c8);
MGLOBAL(float, DAT_008991cc, 0x008991cc);
MGLOBAL(float, DAT_0067d830, 0x0067d830);
MGLOBAL(float, DAT_0067d834, 0x0067d834);
MGLOBAL(float, DAT_005cd5f0, 0x005cd5f0);
MGLOBAL(float, DAT_005cd5f4, 0x005cd5f4);
MGLOBAL(float, DAT_005cd5f8, 0x005cd5f8);
// MATCH: 0x00427580 FUN_00427580
void FUN_00427580(void)
{
  DAT_008991c0 = 0.224609375f;
  DAT_008991c8 = DAT_005cd5f8 * DAT_0067d830;
  DAT_008991cc = DAT_005cd5f4 * DAT_0067d834;
  DAT_008991c4 = DAT_005cd5f0 - DAT_008991cc;
}
