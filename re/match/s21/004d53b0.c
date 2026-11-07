// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d53b0 (h1, by hand): 100.0% exact, regs 100.0% (raw 50.8%). D3D state-cache flush: pending render
   states (list 0x007d5168, {value,changed} 0x007d57f8, cache 0x007d54b0) through device slot +0xe4, then pending
   texture-stage states (pairs 0x007d62a8, [8][33] {value,changed} 0x007d4720, [8][33] cache 0x007d5e88) through
   +0x10c. Moves: __stdcall vtable slot types, the {value,changed} struct, and real [stage][33] 2D arrays (the flat
   stage*33+type index was 93.4%). Array bounds come from the gaps between the globals. relocheck OK, C++ ok. */
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
typedef struct { int value; int changed; } FUN_004d53b0_cache;
typedef struct { int stage; int type; } FUN_004d53b0_tss;
typedef int (__stdcall *FUN_004d53b0_setrs)(int *self, int state, int value);
typedef int (__stdcall *FUN_004d53b0_settss)(int *self, int stage, int type, int value);
MGLOBAL(uint, DAT_007d6c14, 0x007d6c14);
MGLOBAL(uint, DAT_007d6c18, 0x007d6c18);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
MGLOBALA(int, DAT_007d54b0, 210, 0x007d54b0);
MGLOBALA(FUN_004d53b0_cache, DAT_007d57f8, 210, 0x007d57f8);
MGLOBALA(FUN_004d53b0_tss, DAT_007d62a8, 264, 0x007d62a8);
typedef FUN_004d53b0_cache FUN_004d53b0_row[33];
MGLOBALA(FUN_004d53b0_row, DAT_007d4720, 8, 0x007d4720);
typedef int FUN_004d53b0_irow[33];
MGLOBALA(FUN_004d53b0_irow, DAT_007d5e88, 8, 0x007d5e88);
// MATCH: 0x004d53b0 FUN_004d53b0
void FUN_004d53b0(void)

{
  uint i;
  int state;
  int value;
  int stage;
  int type;

  if (DAT_007d6c14 != 0) {
    i = 0;
    do {
      state = DAT_007d5168[i];
      value = DAT_007d57f8[state].value;
      DAT_007d57f8[state].changed = 0;
      if (DAT_007d54b0[state] != value) {
        DAT_007d54b0[state] = value;
        (*(FUN_004d53b0_setrs *)(*DAT_007d4110 + 0xe4))(DAT_007d4110,state,value);
      }
      i++;
    } while (i < DAT_007d6c14);
    DAT_007d6c14 = 0;
  }
  if (DAT_007d6c18 != 0) {
    i = 0;
    do {
      stage = DAT_007d62a8[i].stage;
      type = DAT_007d62a8[i].type;
      value = DAT_007d4720[stage][type].value;
      DAT_007d4720[stage][type].changed = 0;
      if (DAT_007d5e88[stage][type] != value) {
        DAT_007d5e88[stage][type] = value;
        (*(FUN_004d53b0_settss *)(*DAT_007d4110 + 0x10c))(DAT_007d4110,stage,type,value);
      }
      i++;
    } while (i < DAT_007d6c18);
    DAT_007d6c18 = 0;
  }
  return;
}
