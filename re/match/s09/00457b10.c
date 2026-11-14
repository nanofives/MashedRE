// FLAGS: /O2 /GS
// TOOLCHAIN ddk
/* IMG-5-TEARDOWN-BODIES 0x00457b10: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees, function
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_0068b134, 0x0068b134);
MGLOBAL(undefined4, DAT_0068b130, 0x0068b130);
MGLOBAL(undefined4, DAT_0068a368, 0x0068a368);
MGLOBAL(undefined1, DAT_0068b090, 0x0068b090);
MGLOBAL(undefined4, DAT_0068b0e4, 0x0068b0e4);
void FUN_004768c0(void *p0);   /* IMG-5: the body passes &DAT_...; same push as (int) under C, valid C++ */
undefined4 FUN_004e6e00(int p0);
// MATCH: 0x00457b10 FUN_00457b10

void FUN_00457b10(void)

{
  undefined4 *puVar1;
  
  puVar1 = &DAT_0068b0e4;
  do {
    FUN_004e6e00(*puVar1);
    puVar1 = puVar1 + 6;
  } while ((int)puVar1 < 0x68b144);
  puVar1 = &DAT_0068a368;
  do {
    FUN_004e6e00(*puVar1);
    puVar1 = puVar1 + 0x1b;
  } while ((int)puVar1 < 0x68b0e8);
  FUN_004e6e00(DAT_0068b134);
  FUN_004e6e00(DAT_0068b130);
  FUN_004768c0(&DAT_0068b090);
  return;
}

