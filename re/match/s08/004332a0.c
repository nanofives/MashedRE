// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x004332a0 (835 B): 97.3% (regs 97.3%, ops 98.9%). Raw g2s 20.2%.
   Rebuilt from the listing. Holds two callees: FUN_0042ac50 (static, n in EAX / spacing in ECX, 100%, body as in
   re/match/s08/0043d2a0.c; called as FUN_0042ac50(FUN_00496910(), 0x22)) and FUN_00430760 (100%, external
   linkage, defined before the caller: the original loads DAT_0067f0c0 into ECX before the call and reuses ECX
   after it, which cl 13.00.9176 does only when it knows the leaf leaves ECX and DAT_0067f0c0 alone; Ghidra's
   extraout_ECX). The 0x4c-byte input records are ONE array of 32 at 0x007f1044 (current = [i], previous =
   [i + 16]; two separate arrays gave 47.6%); the 12-player up/down loop is a plain loop the compiler unrolls x4.
   `up = 0; dn = 0;` statement order and DAT_0067f0c8 = 220.0f stored once after the if/else (+3%).
   Permuter: `if (v != 0) {..} else {85.0f}` and `v <= -1` (97.3%). Tried for the last 2.7%: if/else, `!f`,
   continue-at-end, switch on v (92.7%), goto forms, pointer loops.
   Residual (b): block layout of the final loop: the original places the `slot.v = -1` block (FUN_00496900(i) == 0)
   after the epilogue; ours keeps it inline. Field names are offsets/roles only. */
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
  char b0;
  char b1;
  char pad[0x4a];
} InputRec;
typedef struct {
  int v;
  float x;
  float y;
} SlotRec;
typedef struct {
  int idx;
  int f4;
  int f8;
  int fc;
} PadMap;
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_0067ed70, 0x0067ed70);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBAL(int, DAT_0067f0c0, 0x0067f0c0);
MGLOBAL(float, DAT_0067f0c4, 0x0067f0c4);
MGLOBAL(float, DAT_0067f0c8, 0x0067f0c8);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(float, DAT_0067ed14, 0x0067ed14);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
MGLOBALA(SlotRec, DAT_0067eaf0, 12, 0x0067eaf0);
MGLOBALA(InputRec, DAT_007f1044, 32, 0x007f1044);
MGLOBALA(PadMap, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e470(int p0);
int FUN_00496900(int p0);
int FUN_00496910(void);
// MATCH: 0x0042ac50 FUN_0042ac50
static int FUN_0042ac50(unsigned int n, int spacing)
{
  int y;
  if ((n & 1) == 0) {
    y = 0xf0 - spacing / 2;
  } else {
    y = 0xf0;
  }
  return y - ((n - 1) >> 1) * spacing;
}
// MATCH: 0x00430760 FUN_00430760
int FUN_00430760(void)
{
  if (DAT_0067e9fc == 2 || DAT_0067e9fc == 10 || DAT_0067e9fc == 3 || DAT_0067e9fc == 4 ||
      DAT_0067e9fc == 5) {
    return 1;
  }
  return 0;
}
// MATCH: 0x004332a0 FUN_004332a0
void FUN_004332a0(void)
{
  int dn;
  int up;
  int i;
  int n;
  int v;
  float y;

  up = 0;
  dn = 0;
  DAT_0067ed70 = 0;
  if (DAT_0067eab0 != 0) {
    return;
  }
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 12; i++) {
      if (DAT_007f1044[i].b0 && !DAT_007f1044[i + 16].b0 && DAT_0067eaf0[i].v > 0) {
        DAT_0067eaf0[i].v--;
        dn = 1;
      }
      if (DAT_007f1044[i].b1 && !DAT_007f1044[i + 16].b1 && DAT_0067eaf0[i].v < 6) {
        DAT_0067eaf0[i].v++;
        up = 1;
      }
    }
    if (!dn && !up) {
      DAT_0067ed70 = 1;
    }
  }
  else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].idx != -1) {
        if (FUN_0040e470(i) != 2 && DAT_007f1044[DAT_007f1a14[i].idx].b0 &&
            !DAT_007f1044[DAT_007f1a14[i].idx + 16].b0) {
          if (DAT_0067f0c0 > 1) {
            DAT_0067f0c0--;
          }
          else {
            DAT_0067ed70 = 1;
          }
        }
        if (DAT_007f1044[DAT_007f1a14[i].idx].b1 && !DAT_007f1044[DAT_007f1a14[i].idx + 16].b1) {
          if (DAT_0067f0c0 < 6) {
            DAT_0067f0c0++;
          }
          else {
            DAT_0067ed70 = 1;
          }
        }
      }
    }
  }
  if (FUN_00430760() != 0) {
    if (DAT_0067f0c0 != 0) {
      DAT_0067f0c4 = (float)(DAT_0067f0c0 * 70 - 70) + 170.0f;
    }
    else {
      DAT_0067f0c4 = 85.0f;
    }
    DAT_0067f0c8 = 220.0f;
    return;
  }
  DAT_0067ed14 = DAT_007f1004 + DAT_0067ed14;
  y = (float)FUN_0042ac50(FUN_00496910(), 0x22);
  if (DAT_007f0fe8 != 0) {
    n = 0;
    for (i = 0; i < 12; i++) {
      if (FUN_00496900(i) != 0) {
        n++;
      }
    }
    y = (float)FUN_0042ac50(n, 0x22);
  }
  for (i = 0; i < 12; i++) {
    if (FUN_00496900(i) == 0) {
      DAT_0067eaf0[i].v = -1;
      continue;
    }
    v = DAT_0067eaf0[i].v;
    if (v == -1) {
      DAT_0067eaf0[i].v = 0;
      v = 0;
    }
    else if (v <= -1) {
      continue;
    }
    if (!(v == 0)) {
      DAT_0067eaf0[i].x = (float)(v * 70 - 70) + 170.0f;
      DAT_0067eaf0[i].y = y;
    } else {
      DAT_0067eaf0[i].x = 85.0f;
      DAT_0067eaf0[i].y = y;
    }
    y = y + 34.0f;
  }
}
