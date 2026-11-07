// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d54f0 (h2): 100% (regs 100%), relocheck OK. Two-key state cache: table at 0x007d4720 of
   {value, dirty} pairs, 33 per row (index a*33+b, 8 bytes each); on a changed value it stores it and, if the
   entry was clean, marks it dirty and appends {a, b} to the list at 0x007d62a8 (count DAT_007d6c18).
   What moved it: a 2-D array of {value, dirty} structs instead of Ghidra's two parallel dword arrays (raw 36%).
   Array bounds are declaration-only: 26 rows = the bytes up to 0x007d62a8 (0x1b88 / 0x108, rounded down),
   302 list entries = the bytes up to 0x007d6c18; the real bounds are not shown by this function [UNCERTAIN]. */
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
typedef struct StateEnt { int value; int dirty; } StateEnt;
typedef StateEnt StateRow[33];
typedef struct DirtyEnt { int stage; int type; } DirtyEnt;
MGLOBALA(StateRow, DAT_007d4720, 26, 0x007d4720);
MGLOBALA(DirtyEnt, DAT_007d62a8, 302, 0x007d62a8);
MGLOBAL(int, DAT_007d6c18, 0x007d6c18);
// MATCH: 0x004d54f0 FUN_004d54f0
void FUN_004d54f0(int stage, int type, int value)
{
  if (DAT_007d4720[stage][type].value != value) {
    DAT_007d4720[stage][type].value = value;
    if (!DAT_007d4720[stage][type].dirty) {
      DAT_007d4720[stage][type].dirty = 1;
      DAT_007d62a8[DAT_007d6c18].stage = stage;
      DAT_007d62a8[DAT_007d6c18].type = type;
      DAT_007d6c18++;
    }
  }
}
