// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048e7b0: 100% (whole obj symbol byte-identical, match.py --bytes). Ghidra's body size stops before the real end, so the per-instruction score reads 91.7%. Raw converter body, C++ fixes only (rsfn_t pointer type for the RwGlobals slot +0x20, FUN_004770a0 takes void*). relocheck OK. Residual: none. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef void (*rsfn_t)(int,int);
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_00722250, 0x00722250);
MGLOBAL(undefined1, DAT_00722470, 0x00722470);
MGLOBAL(undefined1, DAT_007228b0, 0x007228b0);
void FUN_004770a0(void *p0);
// MATCH: 0x0048e7b0 FUN_0048e7b0
void FUN_0048e7b0(void)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  undefined *puVar4;
  
  (**(rsfn_t *)(DAT_007d3ff8 + 0x20))(8,0);
  puVar4 = &DAT_007228b0;
  piVar3 = (int *)&DAT_00722470;
  do {
    piVar3 = piVar3 + -8;
    puVar4 = puVar4 + -0x40;
    iVar1 = 0;
    piVar2 = piVar3;
    do {
      if (*piVar2 != 0) {
        FUN_004770a0(puVar4);
        break;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < 8);
    if (piVar3 == (int *)&DAT_00722250) {
      (**(rsfn_t *)(DAT_007d3ff8 + 0x20))(8,1);
      return;
    }
  } while( true );
}
