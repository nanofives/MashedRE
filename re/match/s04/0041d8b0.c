// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 h4 0x0041d8b0 (96 B): 100.0% exact (regs 100, ops 100), BYTE-IDENTICAL, by hand. Raw converter: 93.5%.
   Fixes over raw: FUN_004b52f0 takes 3 args (Ghidra had 0), int index loop over the 6 atomics at 0x0063d55c,
   clump store (0x0063d57c) before the frame store (0x0063d580). String "startlights.dff" at 0x005cd218 as MGLOBALA.
   Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
MGLOBALA(int, DAT_0063d55c, 6, 0x0063d55c);
MGLOBAL(int, DAT_0063d57c, 0x0063d57c);
MGLOBAL(int, DAT_0063d580, 0x0063d580);
MGLOBALA(char, s_startlights_dff_005cd218, 16, 0x005cd218);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004b51d0(int clump, int *out, int n, int a, int b);
void FUN_004b52f0(int p0, int p1, int p2);
void FUN_004b6520(void *p, int n);

// MATCH: 0x0041d8b0 FUN_0041d8b0
void FUN_0041d8b0(void)
{
  int clump;
  int i;
  clump = FUN_0042a5d0(s_startlights_dff_005cd218, 0, 0);
  FUN_004b6520(DAT_0063d55c, 0x30);
  DAT_0063d57c = clump;
  DAT_0063d580 = *(int *)(clump + 4);
  FUN_004b51d0(clump, DAT_0063d55c, 8, 0, 0);
  for (i = 0; i < 6; i++)
    FUN_004b52f0(DAT_0063d55c[i], 0x40, 1);
}
