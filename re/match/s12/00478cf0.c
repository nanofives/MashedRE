// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00478cf0 (h5): whole obj symbol BYTE-IDENTICAL (827 B, relocations masked); match.py prints 99.2%
   only because Ghidra's size (824) stops before the final `add esp,0x100; ret`. From raw 69.7%: real callee names
   (Ghidra's RpWorldDestroy etc. were wrong), the array load at +0x10110 read before the `!= -1` test in loop 1,
   and the variable-count loops written as `i = 0; if (0 < n) { pp = base; do {..} while }` (Ghidra's shape),
   which places the pointer init after the guard. Residual: none. */
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
int FUN_004850b0(int param_1);
void FUN_004853f0(int param_1);
void FUN_004987b0(const char *fmt, ...);
int FUN_004b3fc0(int param_1, int *param_2);
void FUN_004b4270(int param_1);
int FUN_004c5930(int param_1);
int FUN_004e4800(int param_1);
int FUN_004e45b0(int param_1, int param_2);
int FUN_004e4860(int param_1, int param_2);
int FUN_004e5700(int param_1);
int FUN_004e6e00(int param_1);
int FUN_00546320(int param_1);
int FUN_00558d30(int param_1);
int FUN_00562560(int param_1);
// MATCH: 0x00478cf0 FUN_00478cf0
void FUN_00478cf0(int param_1, int param_2)
{
  int i;
  int j;
  int n;
  int clump;
  int frame;
  char *ent;
  int *pp;
  int buf[64];

  i = 0;
  if (0 < *(int *)(param_2 + 0x6c)) {
    ent = (char *)(param_2 + 0x2c);
    do {
      clump = *(int *)(param_1 + 0x10110 + *ent * 4);
      if (*(short *)(ent + 2) != -1) {
        n = FUN_004b3fc0(clump, buf);
        for (j = 0; j < n; j++) {
          FUN_004853f0(FUN_004850b0(buf[j]));
        }
      }
      i++;
      ent += 4;
    } while (i < *(int *)(param_2 + 0x6c));
  }
  i = 0;
  if (0 < *(int *)(param_2 + 0x1c)) {
    pp = (int *)(param_1 + 0x10090);
    do {
      if (*pp != 0) {
        FUN_004e5700(*pp);
        *pp = 0;
      }
      i++;
      pp++;
    } while (i < *(int *)(param_2 + 0x1c));
  }
  i = 0;
  if (0 < *(int *)(param_2 + 0x20)) {
    pp = (int *)(param_1 + 0x100a0);
    do {
      if (*pp != 0) {
        FUN_004e5700(*pp);
        *pp = 0;
      }
      i++;
      pp++;
    } while (i < *(int *)(param_2 + 0x20));
  }
  for (i = 0, pp = (int *)(param_1 + 0x100b0); i < 0x10; i++, pp++) {
    if (*pp != 0) {
      FUN_00546320(*pp);
      *pp = 0;
    }
  }
  for (i = 0, pp = (int *)(param_1 + 0x100f0); i < 8; i++, pp++) {
    if (*pp != 0) {
      FUN_004e6e00(*pp);
      *pp = 0;
    }
  }
  if (*(int *)(param_1 + 0x105d8) != 0) {
    FUN_004e6e00(*(int *)(param_1 + 0x105d8));
  }
  if (*(int *)(param_1 + 0x105e0) != 0) {
    FUN_004e4860(*(int *)(param_1 + 0x105d4), *(int *)(param_1 + 0x105e0));
  }
  if (*(int *)(param_1 + 0x105e4) != 0) {
    FUN_004e4860(*(int *)(param_1 + 0x105d4), *(int *)(param_1 + 0x105e4));
  }
  if (*(int *)(param_1 + 0x105dc) != 0) {
    FUN_004e6e00(*(int *)(param_1 + 0x105dc));
  }
  for (i = 0, pp = (int *)(param_1 + 0x10110); i < 0x40; i++, pp++) {
    clump = *pp;
    if (clump != 0) {
      frame = FUN_004e4800(clump);
      if (frame != 0) {
        FUN_004e45b0(frame, clump);
      }
      FUN_004e6e00(clump);
    }
  }
  if (*(int *)(param_1 + 0x105f0) != 0) {
    FUN_00562560(*(int *)(param_1 + 0x105f0));
  }
  for (i = 0, pp = (int *)(param_1 + 0x105b0); i < 8; i++, pp++) {
    if (*pp != 0) {
      FUN_00562560(*pp);
    }
  }
  if (*(int *)(param_1 + 0x105d4) != 0) {
    FUN_004b4270(*(int *)(param_1 + 0x105d4));
    FUN_004e5700(*(int *)(param_1 + 0x105d4));
    *(int *)(param_1 + 0x105d4) = 0;
    FUN_004987b0("World Destroyed\n");
  }
  i = 0;
  if (0 < *(int *)(param_2 + 4)) {
    pp = (int *)(param_1 + 0x10040);
    do {
      FUN_004e5700(*pp);
      i++;
      pp++;
    } while (i < *(int *)(param_2 + 4));
  }
  FUN_004e5700(*(int *)(param_1 + 0x10080));
  for (i = 0, pp = (int *)(param_1 + 0x10618); i < 8; i++, pp++) {
    if (*pp != 0) {
      FUN_00558d30(*pp);
      *pp = 0;
    }
  }
  if (*(int *)(param_1 + 0x105d0) != 0) {
    FUN_004c5930(*(int *)(param_1 + 0x105d0));
    FUN_004987b0("TextureDictionary Destroyed\n");
  }
}
