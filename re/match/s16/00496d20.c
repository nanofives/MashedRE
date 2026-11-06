// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496d20 (h3): by hand, 100% (regs 100%), relocheck OK. Two RpWorldForAllWorldSectors
   (0x004e5c70) calls with callback FUN_00496d10, data DAT_00773088 or 0. */
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
typedef void *(__cdecl *PFN_SectorCB)(void *sector, void *data);
MGLOBAL(void *, DAT_00773088, 0x00773088);
void *FUN_00496d10(void *sector, void *data);
void *FUN_004e5c70(void *world, PFN_SectorCB cb, void *data);
// MATCH: 0x00496d20 FUN_00496d20
void FUN_00496d20(void *world, int param_2)
{
  if (param_2 != 0) {
    FUN_004e5c70(world, FUN_00496d10, DAT_00773088);
    return;
  }
  FUN_004e5c70(world, FUN_00496d10, 0);
}
