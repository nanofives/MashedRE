// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8570 (h2): 100% (regs 100%), relocheck OK. Fills the RwGlobals string-function table
   +0xc4..+0x104 (17 slots: CRT strncpy/strncat/strstr/strcmp/strncmp/strlen/sscanf and RW's own string helpers,
   incl. 0x004d8730 strchr, 0x004d8750 strrchr, 0x004d8700 strlwr); returns 1. RwGlobals is reloaded for every
   store (stores through an int base alias it). What moved it: the slot targets as functions, not byte globals. */
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
typedef int (*RwStrFn)();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004a2b60();
int FUN_004a42c5();
int FUN_004ac560();
int FUN_004a34b0(); /* strncpy */
int FUN_004ac570();
int FUN_005c2300(); /* strncat */
int FUN_004d8750();
int FUN_004d8730();
int FUN_004a3860(); /* strstr */
int FUN_004af340(); /* strcmp */
int FUN_004a3a90(); /* strncmp */
int FUN_004d8680();
int FUN_004a9410(); /* strlen */
int FUN_004d86d0();
int FUN_004d8700();
int FUN_004a3ac9();
int FUN_005c22bd(); /* sscanf */
// MATCH: 0x004d8570 FUN_004d8570
int FUN_004d8570(void)
{
  *(RwStrFn *)(DAT_007d3ff8 + 0xc4) = FUN_004a2b60;
  *(RwStrFn *)(DAT_007d3ff8 + 0xc8) = FUN_004a42c5;
  *(RwStrFn *)(DAT_007d3ff8 + 0xcc) = FUN_004ac560;
  *(RwStrFn *)(DAT_007d3ff8 + 0xd0) = FUN_004a34b0;
  *(RwStrFn *)(DAT_007d3ff8 + 0xd4) = FUN_004ac570;
  *(RwStrFn *)(DAT_007d3ff8 + 0xd8) = FUN_005c2300;
  *(RwStrFn *)(DAT_007d3ff8 + 0xdc) = FUN_004d8750;
  *(RwStrFn *)(DAT_007d3ff8 + 0xe0) = FUN_004d8730;
  *(RwStrFn *)(DAT_007d3ff8 + 0xe4) = FUN_004a3860;
  *(RwStrFn *)(DAT_007d3ff8 + 0xe8) = FUN_004af340;
  *(RwStrFn *)(DAT_007d3ff8 + 0xec) = FUN_004a3a90;
  *(RwStrFn *)(DAT_007d3ff8 + 0xf0) = FUN_004d8680;
  *(RwStrFn *)(DAT_007d3ff8 + 0xf4) = FUN_004a9410;
  *(RwStrFn *)(DAT_007d3ff8 + 0xf8) = FUN_004d86d0;
  *(RwStrFn *)(DAT_007d3ff8 + 0xfc) = FUN_004d8700;
  *(RwStrFn *)(DAT_007d3ff8 + 0x100) = FUN_004a3ac9;
  *(RwStrFn *)(DAT_007d3ff8 + 0x104) = FUN_005c22bd;
  return 1;
}
