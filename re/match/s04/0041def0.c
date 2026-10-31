// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041def0: matched. Whole obj symbol BYTE-IDENTICAL to the original (389 B, relocations masked,
   `match.py --bytes`). match.py's instruction score reads 96.5% only because Ghidra's size (380 B) stops
   before the last 4 instructions (`mov [0x63d60b], al; pop ebx; add esp, 0xc; ret` at 0x0041e06b..0x0041e074);
   the 111 instructions inside Ghidra's range are all equal. Raw g2s 59.5%.
   What moved it: the row of clones as a walking pointer (`row += 11`, edi) with the inner index i, and
   both atomic loops written with int indexes (the original's pointer compares are signed, jl: strength-reduced
   from `i < 32` / `j < 16`; a pointer loop gives jb). With those, cl also holds 0 in EBX as the original does.
   Layout: 0x0063d610 is 6 rows x 11 clones (0x2c B per row), cleared with 0x0063d718's 32 atomics up to
   0x0063d7e4 (FUN_004b6520, 0x1d4 B); 0x0063d7bc clump, 0x0063d7c0 its frame; atomic 0x1c = 0x0063d788
   stands in for index 10. The five RGBA colours at 0x0063d5f8 are written per channel. Residual: none. */
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
typedef struct { float x, y, z; } MatchV3_0041def0;
typedef struct { unsigned char r, g, b, a; } MatchRGBA_0041def0;
MGLOBALA(char, s_stopclock2_dff_005cd24c, 15, 0x005cd24c);
MGLOBALA(MatchRGBA_0041def0, DAT_0063d5f8, 5, 0x0063d5f8);
MGLOBALA(char *, DAT_0063d610, 66, 0x0063d610);
MGLOBALA(char *, DAT_0063d718, 32, 0x0063d718);
MGLOBAL(char *, DAT_0063d7bc, 0x0063d7bc);
MGLOBAL(char *, DAT_0063d7c0, 0x0063d7c0);
char *FUN_0042a5d0(const char *name, int a1, int a2);
void FUN_004b6520(void *p, int n);
void FUN_004b51d0(char *clump, char **out, int n, int a3, int a4);
void FUN_004b52f0(char *atomic, int flags, int on);
char *FUN_004e69a0(char *atomic);
char *FUN_004c0b30(void);
void *FUN_004e7e30(char *atomic, char *frame);
void *FUN_004c1040(char *parent, char *frame);
void *FUN_004c1340(char *frame, MatchV3_0041def0 *v, int op);

// MATCH: 0x0041def0 FUN_0041def0
void FUN_0041def0(void)
{
  char *clump;
  char **pa;
  char **row;
  int i, j;

  MatchV3_0041def0 pos;

  clump = FUN_0042a5d0(s_stopclock2_dff_005cd24c, 0, 0);
  FUN_004b6520(DAT_0063d610, 0x1d4);
  DAT_0063d7bc = clump;
  DAT_0063d7c0 = *(char **)(clump + 4);
  FUN_004b51d0(clump, DAT_0063d718, 0x20, 0, 0);
  for (i = 0; i < 32; i++) {
    if (DAT_0063d718[i] != 0)
      FUN_004b52f0(DAT_0063d718[i], 0x40, 1);
  }
  row = DAT_0063d610;
  for (j = 10; j < 16; j++) {
    pos = *(MatchV3_0041def0 *)(*(char **)(DAT_0063d718[j] + 4) + 0x40);
    for (i = 0; i < 11; i++) {
      row[i] = FUN_004e69a0(i == 10 ? DAT_0063d718[0x1c] : DAT_0063d718[i]);
      FUN_004e7e30(row[i], FUN_004c0b30());
      FUN_004c1040(DAT_0063d7c0, *(char **)(row[i] + 4));
      FUN_004c1340(*(char **)(row[i] + 4), &pos, 0);
    }
    row += 11;
  }
  DAT_0063d5f8[0].r = 0x32; DAT_0063d5f8[0].g = 0x32; DAT_0063d5f8[0].b = 0x32; DAT_0063d5f8[0].a = 0xff;
  DAT_0063d5f8[1].r = 200; DAT_0063d5f8[1].g = 0; DAT_0063d5f8[1].b = 0x12; DAT_0063d5f8[1].a = 0xff;
  DAT_0063d5f8[2].r = 0; DAT_0063d5f8[2].g = 0x96; DAT_0063d5f8[2].b = 0; DAT_0063d5f8[2].a = 0xff;
  DAT_0063d5f8[3].r = 0; DAT_0063d5f8[3].g = 0; DAT_0063d5f8[3].b = 200; DAT_0063d5f8[3].a = 0xff;
  DAT_0063d5f8[4].r = 0xff; DAT_0063d5f8[4].g = 0xff; DAT_0063d5f8[4].b = 0xff; DAT_0063d5f8[4].a = 0xff;
}
