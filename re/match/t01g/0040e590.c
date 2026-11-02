// FLAGS: /O2 /GS /Ob1
/* MATCH-T01G-FU h2 0x0040e590 (2360 B, void(void), cdecl): best 84.6% exact (regs 89.1%, ops 99.0%), 786 vs 783
   instructions. Raw g2s 22.0% (28.7 / 49.1). No register-passed callee: every callee takes stack args (pushes), so
   no static callee is held. Callee prototypes from re/match/s01,s02,s04,s06,s07.
   Structure found (each step measured):
   - the "which slot holds value m" searches are 4-trip FOR loops over order[m] that 9176 fully unrolls (the k==-1
     tests fold into the original's compare chain); the duplicate counts n/v in the post-pass are 4/3/2-trip for
     loops too (that is what reloads order[3] from memory in the mode-4 counts); the "all slots still 1000" check,
     the 1000 init and the times[] = -1 init are 4-trip for loops. The fix chains (0->1, 1->2 ...) are written
     if/else-if: a loop with break or a found flag is not unrolled. 39% -> 50%.
   - order[i] = i must be a do/while (a for loop is fully unrolled, the original keeps the loop).
   - times is uint[6] (0x0041f000 fills 24 bytes): gives the 0x48 frame.
   - place is uint (stops the k+4 / 8-k strength reduction in the last loop); the per-car place switch is
     `switch (mode) { case 1: table; default: ... }` (the dec eax / je to the table at the end).
   - first switch, case 2 as a nested switch on v: gives the `mov [order+i],eax` store (no cross-jump into case 3).
   - the 3-player block sets x=y=-1 before the slot picks; DAT_007f0ff8 = 0 before the ++.
   Residual (b): (1) the last loop strength-reduces i*4+0x34 (PTR_PTR_005f2770 active flags + DAT_008a94c0 store)
   in every natural form tried (index/shift/(uint)/(long)/sizeof forms, int *, struct, local copy, inline helper,
   continue, do/while, goto loop); `(short)i` in the active-flag index is the only form found that keeps
   [ecx+edi*4+0x34] and the `mov ebp,9` hoist, at the cost of one movsx. (2) case 2 of the first switch: the nested
   switch tests -1 before 1 (original: 1, then the shared -1 tail); the if/else form gets the order right but
   cross-jumps into case 3. (3) register colouring in the mode-4 post-pass (order[2]/order[3] in ecx/edx swapped),
   the 1000-init stores scheduled before the first loop's pointer setup, the FUN_00408b00 arg registers. Permuter
   on the 67% form (before the case-2 switch): 67.0 -> 71.0% (swap, cmp), below this file. relocheck n/a below 100%. C++ (VS2022 /TP): ok. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct V3_0040e590 { float x, y, z; } V3_0040e590;
typedef struct Rec_007f0a00 { int a; int b; int c; } Rec_007f0a00;
typedef struct Slot_007f1a14 { int id; int pad[3]; } Slot_007f1a14;
MGLOBAL(uint, DAT_008a94b0, 0x008a94b0);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(int, DAT_008a94d0, 0x008a94d0);
MGLOBAL(char *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_0063ba80, 0x0063ba80);
MGLOBAL(int, DAT_007f0fd8, 0x007f0fd8);
MGLOBALA(int, DAT_0063b9b8, 48, 0x0063b9b8);
MGLOBALA(Rec_007f0a00, DAT_007f0a00, 4, 0x007f0a00);
MGLOBALA(Slot_007f1a14, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(char, DAT_00803324, 4, 0x00803324);
MGLOBALA(int, DAT_008a94c0, 4, 0x008a94c0);
int FUN_00408a70(int i, float pos);
void FUN_00408b00(int h, int place, int ncars, V3_0040e590 *out, float *out_ang);
void FUN_00409290(int *v);
void FUN_0040b250(void);
int FUN_0040b410(int i);
void FUN_0040e3a0(int i, unsigned char *p);
void FUN_0041ede0(int i, int a, int b);
void FUN_0041ee50(int i, int a, int b);
void FUN_0041ef80(int i, int on);
void FUN_0041f000(int i, void *out);
void FUN_00429820(void);
int FUN_0042f500(void);
int FUN_00430790(void);
void FUN_0045ba00(int i, int on);
int FUN_0046b1c0(int i, uint *t);
int FUN_0046b540(int i);
int FUN_0046baa0(int i);
int FUN_0046c6d0(int i, int *t);
int FUN_0046d4a0(int *p, int i);
int FUN_004704c0(int i, V3_0040e590 pos, float ang, void *p);
void FUN_0048f680(void);
void FUN_0048f740(void);

// MATCH: 0x0040e590 FUN_0040e590
void FUN_0040e590(void)
{
  int v;
  int all;
  float ang;
  int l1c;
  unsigned char l20[4];
  V3_0040e590 pos;
  uint order[4];
  uint times[6];
  int i, j, k, n, found; uint place;
  int a, b, c, d, m;

  DAT_007f0ff8 = 0;
  DAT_008a94b0++;
  if (DAT_008a94b0 >= 12) {
    DAT_008a94b0 = 0;
  }
  FUN_00429820();
  all = 1;
  FUN_0048f680();
  FUN_0048f740();
  for (i = 0; i < 4; i++) {
    order[i] = 1000;
  }
  for (i = 0; i < 4; i++) {
    DAT_007f0a00[i].a = 0;
    DAT_007f0a00[i].b = 0;
    DAT_007f0a00[i].c = 0;
    if (DAT_007f1a14[i].id > -1) {
      FUN_0041ef80(i, 0);
      FUN_0041ede0(i, 0, 0);
      FUN_0041ee50(i, 0, 0);
    }
  }
  for (i = 0; i < 4; i++) {
    v = FUN_0040b410(i);
    switch (DAT_008a94d0) {
    case 1:
      order[i] = 0;
      break;
    case 2:
      switch (v) {
      case 1: order[i] = 1; break;
      case -1: order[i] = 0; break;
      }
      break;
    case 3:
      if (v == 1) order[i] = 2;
      else if (v == 0) order[i] = 1;
      else if (v == -1) order[i] = 0;
      break;
    case 4:
      if (v == 2) order[i] = 3;
      else if (v == 1) order[i] = 2;
      else if (v == -1) order[i] = 1;
      else if (v == -2) order[i] = 0;
      break;
    }
  }
  for (i = 0; i < 4; i++) {
    if (order[i] != 1000) all = 0;
  }
  for (i = 0; i < 4; i++) {
    v = FUN_0040b410(i);
    switch (DAT_008a94d0) {
    case 2:
      if (v != 1 && v != -1) {
        found = 0;
        for (j = 0; j < 4; j++) {
          k = -1;
          for (m = 0; m < 4; m++) {
            if (order[m] == j && k == -1) k = m;
          }
          if (k == -1 && !found) {
            order[i] = j;
            found = 1;
          }
        }
      }
      break;
    case 3:
      if (v != 1 && v != 0 && v != -1) {
        found = 0;
        for (j = 0; j < 4; j++) {
          k = -1;
          for (m = 0; m < 4; m++) {
            if (order[m] == j && k == -1) k = m;
          }
          if (k == -1 && !found) {
            order[i] = j;
            found = 1;
          }
        }
      }
      break;
    case 4:
      if (v != 2 && v != 1 && v != -1 && v != -2) {
        found = 0;
        for (j = 0; j < 4; j++) {
          k = -1;
          for (m = 0; m < 4; m++) {
            if (order[m] == j && k == -1) k = m;
          }
          if (k == -1 && !found) {
            order[i] = j;
            found = 1;
          }
        }
      }
      break;
    }
  }
  switch (DAT_008a94d0) {
  case 2:
    for (;;) {
      for (;;) {
        n = 0;
        for (m = 0; m < 2; m++) {
          if (order[m] == 0) n++;
        }
        if (n <= 1) break;
        if (order[0] == 0) order[0] = 1;
        else if (order[1] == 0) order[1] = 1;
      }
      v = 0;
      for (m = 0; m < 2; m++) {
        if (order[m] == 1) v++;
      }
      if (v <= 1) break;
      if (order[0] == 1) order[0] = 0;
      else if (order[1] == 1) order[1] = 0;
    }
    break;
  case 3:
    for (;;) {
      for (;;) {
        n = 0;
        for (m = 0; m < 3; m++) {
          if (order[m] == 0) n++;
        }
        if (n <= 1) break;
        if (order[0] == 0) order[0] = 1;
        else if (order[1] == 0) order[1] = 1;
        else if (order[2] == 0) order[2] = 1;
      }
      for (;;) {
        n = 0;
        for (m = 0; m < 3; m++) {
          if (order[m] == 1) n++;
        }
        if (n <= 1) break;
        if (order[0] == 1) order[0] = 2;
        else if (order[1] == 1) order[1] = 2;
        else if (order[2] == 1) order[2] = 2;
      }
      v = 0;
      for (m = 0; m < 3; m++) {
        if (order[m] == 2) v++;
      }
      if (v <= 1) break;
      if (order[0] == 2) order[0] = 0;
      else if (order[1] == 2) order[1] = 0;
      else if (order[2] == 2) order[2] = 0;
    }
    break;
  case 4:
    for (;;) {
      for (;;) {
        n = 0;
        for (m = 0; m < 4; m++) {
          if (order[m] == 0) n++;
        }
        if (n <= 1) break;
        if (order[0] == 0) order[0] = 1;
        else if (order[1] == 0) order[1] = 1;
        else if (order[2] == 0) order[2] = 1;
        else if (order[3] == 0) order[3] = 1;
      }
      for (;;) {
        n = 0;
        for (m = 0; m < 4; m++) {
          if (order[m] == 1) n++;
        }
        if (n <= 1) break;
        if (order[0] == 1) order[0] = 2;
        else if (order[1] == 1) order[1] = 2;
        else if (order[2] == 1) order[2] = 2;
        else if (order[3] == 1) order[3] = 2;
      }
      for (;;) {
        n = 0;
        for (m = 0; m < 4; m++) {
          if (order[m] == 2) n++;
        }
        if (n <= 1) break;
        if (order[0] == 2) order[0] = 3;
        else if (order[1] == 2) order[1] = 3;
        else if (order[2] == 2) order[2] = 3;
        else if (order[3] == 2) order[3] = 3;
      }
      v = 0;
      for (m = 0; m < 4; m++) {
        if (order[m] == 3) v++;
      }
      if (v <= 1) break;
      if (order[0] == 3) order[0] = 0;
      else if (order[1] == 3) order[1] = 0;
      else if (order[2] == 3) order[2] = 0;
      else if (order[3] == 3) order[3] = 0;
    }
    break;
  }
  if (FUN_0042f500() == 1) {
    if (DAT_008a94d0 == 3) {
      i = -1;
      j = -1;
      a = 1;
      b = 2;
      c = 3;
      if (*(int *)(PTR_PTR_005f2770 + 0x38) == 0) {
        a = 0;
        b = 2;
        c = 3;
      }
      if (*(int *)(PTR_PTR_005f2770 + 0x3c) == 0) {
        a = 0;
        b = 1;
        c = 3;
      }
      if (*(int *)(PTR_PTR_005f2770 + 0x40) == 0) {
        a = 0;
        b = 1;
        c = 2;
      }
      if (order[a] == order[b]) {
        i = a;
        j = b;
      }
      if (order[a] == order[c]) {
        i = a;
        j = c;
      }
      if (order[b] == order[c]) {
        i = b;
        j = c;
      }
      if (i != -1) {
        FUN_0046c6d0(i, &v);
        if (v == 0) v = -2;
        times[i] = v;
        FUN_0046c6d0(j, &v);
        if (v == 0) v = -2;
        times[j] = v;
        if (times[i] < times[j]) {
          order[i] = 1;
        } else {
          order[j] = 1;
        }
      }
    } else {
      for (i = 0; i < 4; i++) {
        times[i] = -1;
      }
      all = 0;
      i = 0;
      do {
        order[i] = i;
        i++;
      } while (i < 4);
      for (i = 0; i < DAT_008a94d0; i++) {
        FUN_0046c6d0(i, &v);
        if (v == 0) v = -2;
        times[i] = v;
      }
      i = 0;
      while (i < 3) {
        if (times[i] > times[i + 1] && times[i] != -1 && times[i + 1] != -1) {
          n = times[i];
          times[i] = times[i + 1];
          times[i + 1] = n;
          v = order[i];
          order[i] = order[i + 1];
          order[i + 1] = v;
          i = 0;
        } else {
          i++;
        }
      }
      a = order[0];
      b = order[1];
      c = order[2];
      d = order[3];
      for (i = 0; i < 4; i++) {
        if (a == i) order[i] = 0;
        if (b == i) order[i] = 1;
        if (c == i) order[i] = 2;
        if (d == i) order[i] = 3;
      }
    }
  }
  FUN_0040b250();
  k = 0;
  for (i = 0; i < 4; i++) {
    DAT_008a94c0[i] = -1;
    if (*(int *)(PTR_PTR_005f2770 + 0x34 + (short)i * 4) != 0) {
      FUN_0041f000(i, times);
      FUN_0046b1c0(i, times);
      FUN_0046b540(i);
      FUN_0046baa0(i);
      switch (DAT_008a94d0) {
      case 1:
        place = DAT_0063b9b8[DAT_008a94b0 * 4 + k];
        break;
      default:
        place = order[i];
        if (all == 1) place = k + 4;
        if (DAT_007f0fd0 == 1) {
          place = k + 4;
        } else if (DAT_007f0fd0 == 2) {
          place = k + 4;
          if (k == DAT_007f0fd8) place = 100;
        } else {
          if (DAT_007f0fd0 == 4) {
            place = 8 - k;
            if (FUN_00430790() == 4) {
              if (place == 8) place = 7;
              else if (place == 7) place = 8;
            }
          }
          if (DAT_007f0fd0 == 9) {
            place = i ? 10 : 7;
          } else if (DAT_007f0fd0 == 8) {
            place = 8 - k;
          }
        }
        break;
      }
      FUN_00409290(&DAT_0063ba80);
      if (DAT_007f0fd0 == 9) {
        FUN_00408b00(DAT_0063ba80, place, 4, &pos, &ang);
      } else {
        FUN_00408b00(DAT_0063ba80, place, DAT_008a94d0, &pos, &ang);
      }
      FUN_00408a70(i, (float)DAT_0063ba80);
      FUN_004704c0(i, pos, ang, DAT_00803324);
      FUN_0046d4a0(&l1c, i);
      FUN_0040e3a0(i, l20);
      FUN_0045ba00(i, 1);
      k++;
    }
  }
}
