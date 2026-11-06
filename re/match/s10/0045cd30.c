// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cd30 (h2): best 62.8% (regs 78.1%, ops 92.3%). Raw 29.1%. Structure is right: nested switches
   on DAT_007f1a1c[r->f04 * 4] / [r->f08 * 4] with the k=1 (`f1c + per`) case written first in outer case 0 (the
   compiler keeps the first copy of each merged tail, giving the original's block order and all five jump tables),
   `switch (per)` for the 0xd/0x19/0x23 start draws, tgt/start uninitialised on the default paths (the original
   reads them from param_1's spill home), slot loop over k = start .. start + per. The file also holds
   0x0045c820 (100%, non-static, before the caller) so EDX survives that call as in the original.
   Residual (b): the original keeps p = &DAT_0088f680[param_1] in EDX through the slot loop and runs the loop as a
   count-down with j, j - per and j * 0x2c induction variables spilled to the frame (sub esp,0x14); ours
   rematerialises p from a spilled param_1 and keeps fewer IVs (sub esp,0x10). Tried: k = 0..per with j = start + k
   and `j -= per`, a separate j/idx pair, do-while / count-down loops, `%` wrap, p computed before or after the
   0x0045c720 call; permuter 512 compiles (+1.8%: `found = j` before the store). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { int f00; int f04; int f08; int f0c; int f10; int f14; int f18; int f1c; int f20; int f24; int f28; } Slot2c;
typedef struct { int f00; int f04; int f08; int f0c; int f10; int f14; int f18; int f1c; int f20; int f24; } Rec28;
MGLOBALA(int, DAT_007f1a1c, 64, 0x007f1a1c);
MGLOBALA(Slot2c *, DAT_0088f680, 4, 0x0088f680);
MGLOBALA(Rec28 *, DAT_0088f6a0, 4, 0x0088f6a0);
MGLOBALA(int, DAT_0088f0a0, 4, 0x0088f0a0);
void FUN_0045c720(int param_1, int param_2);
int FUN_004b44f0(int param_1, int param_2);
// MATCH: 0x0045c820 FUN_0045c820
int FUN_0045c820(int param_1)
{
  int i;

  i = 0;
  do {
    if (DAT_0088f0a0[i] == 0) {
      DAT_0088f0a0[i] = param_1 + 1;
      return i + 1;
    }
    i++;
  } while (i < 4);
  return i;
}

// MATCH: 0x0045cd30 FUN_0045cd30
void FUN_0045cd30(int param_1)
{
  Rec28 *r;
  int per;
  int tgt;
  int start;
  int found;
  int k;
  int j;
  Slot2c **p;

  r = DAT_0088f6a0[param_1];
  if (r->f18 > 0) {
    per = r->f20 / r->f18;
    switch (DAT_007f1a1c[r->f04 * 4]) {
    case 0:
      switch (DAT_007f1a1c[r->f08 * 4]) {
      case 2: tgt = r->f1c + per; break;
      case 1: tgt = r->f1c; break;
      case 5: tgt = r->f1c + per * 2; break;
      case 0: case 3: case 4: return;
      }
      break;
    case 1:
      switch (DAT_007f1a1c[r->f08 * 4]) {
      case 0: tgt = r->f1c; break;
      case 2: tgt = r->f1c + per; break;
      case 5: tgt = r->f1c + per * 2; break;
      case 1: case 3: case 4: return;
      }
      break;
    case 2:
      switch (DAT_007f1a1c[r->f08 * 4]) {
      case 0: tgt = r->f1c; break;
      case 1: tgt = r->f1c + per; break;
      case 5: tgt = r->f1c + per * 2; break;
      case 2: case 3: case 4: return;
      }
      break;
    case 3:
    case 4:
      return;
    case 5:
      switch (DAT_007f1a1c[r->f08 * 4]) {
      case 0: tgt = r->f1c; break;
      case 1: tgt = r->f1c + per; break;
      case 2: tgt = r->f1c + per * 2; break;
      case 3: case 4: case 5: return;
      }
      break;
    }
  } else {
    tgt = r->f1c;
    per = r->f20;
  }
  switch (per) {
  case 0xd:
    start = FUN_004b44f0(0, 6) + FUN_004b44f0(0, 6) + FUN_004b44f0(0, 1);
    break;
  case 0x19:
    start = FUN_004b44f0(0, 0xc) + FUN_004b44f0(0, 0xc) + FUN_004b44f0(0, 1);
    break;
  case 0x23:
    start = FUN_004b44f0(0, 0x11) + FUN_004b44f0(0, 0x11) + FUN_004b44f0(0, 1);
    break;
  }
  if (tgt == -1)
    return;
  FUN_0045c720(param_1, tgt);
  p = &DAT_0088f680[param_1];
  if (*p == 0)
    return;
  found = -1;
  if (per > 0) {
    for (k = start; k < start + per; k++) {
      j = k;
      if (k >= per)
        j = k - per;
      if ((*p)[j].f10 == 0) {
        if (found == -1) {
          found = j;
          (*p)[j].f10 = 1;
        }
      } else {
        (*p)[j].f10++;
        if ((*p)[j].f10 >= per)
          (*p)[j].f10 = 0;
      }
    }
    if (found != -1) {
      *p = &(*p)[found];
      (*p)->f14 = DAT_0088f6a0[param_1]->f08;
      (*p)->f04 = DAT_0088f6a0[param_1]->f04;
    }
  }
  (*p)->f28 = DAT_0088f6a0[param_1]->f00;
  (*p)->f24 = FUN_0045c820(param_1);
}
