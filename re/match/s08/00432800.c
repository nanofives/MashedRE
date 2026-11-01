// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x00432800 (649 B): 100% (regs 100%, ops 100%), byte-identical. Raw g2s 29.7%.
   What moved it: the 0x40-byte MenuLevel array at 0x0067ed78 (layout as in re/match/s08/0043d2a0.c; +0x04 id is
   the switch value, +0x08 selection, +0x0c..+0x3b twelve enable flags, +0x3c count) indexed directly as
   DAT_0067ed78[param_1].field (a `MenuLevel *` local gave [esi+0xc] addressing, 31.7%), and the case bodies in
   the original's layout order 8, 0x1c, 1, 2, 3, 10, 0x12, 0x18 (ascending order 60.7%). Ghidra's
   thunk_FUN_00497450 is the call target 0x00496900 (a jmp thunk), cdecl 1 arg. The DAT_00898aa0 stores are an
   int array view (indices 0..3, 0x1f, 0x2c, 0x39, 0x46). Residual class: none. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct {
  int *def;
  int id;
  int f8;
  int pad[12];
  int count;
} MenuLevel;
MGLOBALA(MenuLevel, DAT_0067ed78, 16, 0x0067ed78);
MGLOBALA(int, DAT_007f0a40, 156, 0x007f0a40);
MGLOBALA(int, DAT_00898aa0, 0x50, 0x00898aa0);
MGLOBAL(int, DAT_0067ea88, 0x0067ea88);
MGLOBAL(int, DAT_0067ea7c, 0x0067ea7c);
MGLOBAL(int, DAT_0067ea78, 0x0067ea78);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBAL(int, DAT_0067ea74, 0x0067ea74);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067ea8c, 0x0067ea8c);
MGLOBAL(int, DAT_007f0ad4, 0x007f0ad4);
MGLOBAL(int, DAT_007f0f2c, 0x007f0f2c);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
void FUN_0040e480(int p0, int p1);
int FUN_0042f500(void);
int FUN_00430830(int p0);
int FUN_00430b60(void);
int FUN_00492d10(void);
int FUN_00496900(int p0);
// MATCH: 0x00432800 FUN_00432800
void FUN_00432800(int param_1)
{
  int i;
  int sel;

  for (i = 0; i < 12; i++) {
    DAT_0067ed78[param_1].pad[i] = 1;
  }
  switch (DAT_0067ed78[param_1].id) {
  case 8:
    if (FUN_00492d10() == 1) {
      break;
    }
    DAT_0067ed78[param_1].pad[2] = 0;
    DAT_0067ed78[param_1].pad[3] = 0;
    break;
  case 0x1c:
    DAT_0067ed78[param_1].pad[0] = 1;
    DAT_0067ed78[param_1].pad[1] = 1;
    DAT_0067ed78[param_1].pad[2] = 1;
    DAT_0067ed78[param_1].pad[3] = 1;
    for (i = 0; i < 4; i++) {
      if (FUN_00496900(i) == 0) {
        DAT_0067ed78[param_1].pad[i] = 0;
      }
    }
    DAT_00898aa0[0] = 0;
    DAT_00898aa0[1] = 1;
    DAT_00898aa0[2] = 2;
    DAT_00898aa0[3] = 3;
    DAT_00898aa0[0x1f] = 0x236;
    DAT_00898aa0[0x2c] = 0x237;
    DAT_00898aa0[0x39] = 0x238;
    DAT_00898aa0[0x46] = 0x239;
    break;
  case 1:
    if (DAT_007f0f2c == 0) {
      DAT_0067ed78[param_1].pad[3] = 0;
    }
    DAT_007f0fe8 = 0;
    for (i = 0; i < 4; i++) {
      FUN_0040e480(i, 0);
    }
    break;
  case 2:
    if (DAT_007f0ad4 == 0) {
      DAT_0067ed78[param_1].pad[2] = 0;
    }
    break;
  case 3:
    DAT_0067ea74 = 1;
    for (i = 0; i < 4; i++) {
      FUN_0040e480(i, 0);
    }
    break;
  case 10:
    if (DAT_0067ea8c != 2) {
      DAT_0067ed78[param_1].pad[1] = 0;
    }
    if (DAT_0067ea8c != 3) {
      DAT_0067ed78[param_1].pad[2] = 0;
    }
    break;
  case 0x12:
    if (DAT_0067ea88 == 1) {
      DAT_0067ed78[param_1].pad[3] = 0;
      DAT_0067ea7c = 0;
    }
    else if (DAT_0067ea7c != 0) {
      DAT_0067ed78[param_1].pad[5] = 1;
      goto skip;
    }
    if (FUN_00430b60() == 2) {
      DAT_0067ed78[param_1].pad[5] = 0;
      DAT_0067ea78 = 0;
    }
skip:
    if (DAT_007f0a40[DAT_0067f17c * 12 + 6] == 0) {
      DAT_0067ed78[param_1].pad[6] = 0;
      DAT_0067ea88 = 0;
    }
    else {
      DAT_0067ed78[param_1].pad[6] = 1;
    }
    if (DAT_007f0a40[DAT_0067f17c * 12 + 4] == 0) {
      DAT_0067ed78[param_1].pad[1] = 0;
      DAT_0067ea74 = 0;
    }
    else {
      DAT_0067ed78[param_1].pad[1] = 1;
    }
    if (DAT_007f0fd0 == 2) {
      DAT_0067ea64 = 0;
    }
    if (FUN_00430b60() == 4 || FUN_0042f500() != 0) {
      DAT_0067ed78[param_1].pad[3] = 0;
    }
    break;
  case 0x18:
    if (FUN_00430830(1) == 0) {
      DAT_0067ed78[param_1].pad[3] = 0;
    }
    break;
  }
  sel = DAT_0067ed78[param_1].f8;
  if (sel == -1) {
    sel = 0;
  }
  if (DAT_0067ed78[param_1].pad[sel] != 1) {
    for (i = 0; i < DAT_0067ed78[param_1].count; i++) {
      if (DAT_0067ed78[param_1].pad[sel] == 1) {
        DAT_0067ed78[param_1].f8 = sel;
        return;
      }
      sel++;
      if (sel == DAT_0067ed78[param_1].count) {
        sel = 0;
      }
    }
    DAT_0067ed78[param_1].f8 = -1;
  }
}
