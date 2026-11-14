// FLAGS: /O2 /GS
// TOOLCHAIN ddk
/* IMG-5-TEARDOWN-BODIES 0x004555d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees, function
   created in the master project 2026-11-13 by CreateMissedFunctions.java), body unchanged; globals moved to MGLOBAL;
   RpClumpDestroy renamed FUN_004e6e00 so relocheck checks the call. Teardown target in the powerup type table
   0x005f99b0 (stride 0x40), called by 0x0045b930. Score: verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_00688418, 0x00688418);
MGLOBAL(undefined4, DAT_006885e8, 0x006885e8);
void FUN_00489c60(int p0);   /* IMG-5: the body passes a dword; same push as (undefined4 *) under C, valid C++ */
undefined4 FUN_004e6e00(int p0);
// MATCH: 0x004555d0 FUN_004555d0

void FUN_004555d0(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar2 = &DAT_006885e8;
  puVar1 = &DAT_00688418;
  do {
    FUN_004e6e00(*puVar2);
    FUN_004e6e00(puVar1[-3]);
    FUN_00489c60(*puVar1);
    puVar1 = puVar1 + 0x1b;
    puVar2 = puVar2 + 0xb;
  } while ((int)puVar1 < 0x688634);
  return;
}

