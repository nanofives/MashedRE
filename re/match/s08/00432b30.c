// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x00432b30 (1327 B): 74.2% (regs 74.2%, ops 89.1%), 343/357 instructions. Raw g2s 18.6%.
   Register convention: mode in EAX, int *cnt (the caller's item count, &nItems in 0x0043d2a0) in ESI, plus the
   stack args sel and menu (0x0043d797). It is a static function of the 0x0043d2a0 TU, so this file defines it
   static and adds a stand-in caller (`match_standin_00432b30`, not the real caller, excluded under MATCH_PORT) so
   the compiler emits it with that convention. The file also holds two register-arg static callees at 100%:
   FUN_0042ad10 (cnt in EAX, type in EDX, 3 stack args) and FUN_0042add0 (key in EBX, idx in EDI).
   Structure: outer switch on sel (1,2,4,5,6,8,10), each case FUN_0042ad10 + s1 code, then an inner switch on the
   FUN_0042add0 result whose case equal to sel is the "self" block (s2 = -1, state 0x1000); the 0x48-then-0x13
   double store is literal in every copy. What moved it: self case written FIRST in each inner switch (74.2% vs
   67.2% in place, 62.7% last), outer cases in ascending order (layout order 1,2,4,8,5,6,10 gave 41.1%),
   `if (mode != 0) {2 / else} else {..}`. Permuter best 75.9% only by reordering statements with a behaviour
   change (call after the s1 store, 0x13 before 0x48): rejected. Other tries: tail stores after the inner switch,
   default first, self statement order, other outer orders, constant vs r idx arg.
   Residual (b): cross-jumping of the identical case tails: the original keeps 3 copies of the self block and
   shares other tails across outer cases in a pattern our source order does not reproduce (we merge more:
   343 vs 357 instructions); plus one extra `xor edi,edi` on the mode 0 path. Under MATCH_PORT the static
   function is unused here; 0x0043d2a0.c declared it extern until RO-MENU-DIV-43DFD0 (2026-11-12), which copied this function and its two static callees into
   0x0043d2a0.c so both are in one TU. */
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
typedef struct {
  int type;
  int f4;
  int f8;
  uchar rgba[4];
  int f10;
  float scale;
  float x;
  float y;
  int idx;
  int f24;
  int s1;
  int s2;
  int *def;
} MenuItem;
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(MenuLevel, DAT_0067ed38, 17, 0x0067ed38);
MGLOBALA(MenuItem, DAT_00898ac0, 32, 0x00898ac0);
int FUN_0042a9c0(void);
int FUN_0042b920(void);
// MATCH: 0x0042ad10 FUN_0042ad10
static void FUN_0042ad10(int *cnt, int type, int x, int y, int z)
{
  DAT_00898ac0[*cnt].type = type;
  DAT_00898ac0[*cnt].rgba[0] = 0xff;
  DAT_00898ac0[*cnt].rgba[1] = 0xff;
  DAT_00898ac0[*cnt].rgba[2] = 0xff;
  DAT_00898ac0[*cnt].rgba[3] = 0xff;
  DAT_00898ac0[*cnt].scale = 0.6f;
  DAT_00898ac0[*cnt].x = (float)x;
  DAT_00898ac0[*cnt].y = (float)y;
  DAT_00898ac0[*cnt].f24 = z;
}
// MATCH: 0x0042add0 FUN_0042add0
static int FUN_0042add0(int key, int idx)
{
  int *p;
  int i;
  int k;
  p = DAT_0067ed38[DAT_0067e9f8].def;
  k = 0;
  for (i = 0; p[i] != (int)0xff070000; i++) {
    if (p[i] == key) {
      if (idx == k) {
        return p[i + 1];
      }
      k++;
    }
  }
  return -1;
}
// MATCH: 0x00432b30 FUN_00432b30
static void FUN_00432b30(int mode, int *cnt, int sel, int menu)
{
  int r;

  r = 0;
  if (mode != 0) {
    if (mode == 2) {
      sel = 0;
      r = FUN_0042add0(0xff080000, r);
    }
    else {
      DAT_0067e9f8++;
      r = FUN_0042add0(0xff080000, r);
      if (r == -1) {
        r = 0;
      }
      DAT_0067e9f8--;
    }
  }
  else if (DAT_0067e9f8 != 0) {
    r = FUN_0042add0(0xff080000, r);
    if (r == -1) {
      r = 0;
    }
  }
  switch (sel) {
  case 1:
    FUN_0042ad10(cnt, 0xff100000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x42;
    if (menu == FUN_0042b920()) {
      DAT_00898ac0[*cnt].s1 = -1;
    }
    switch (r) {
    case 1:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 2:
    FUN_0042ad10(cnt, 0xff100000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x43;
    switch (r) {
    case 2:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 4:
    FUN_0042ad10(cnt, 0xff100000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x225;
    switch (r) {
    case 4:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 5:
    FUN_0042ad10(cnt, 0xff100000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x48;
    DAT_00898ac0[*cnt].s1 = 0x13;
    switch (r) {
    case 5:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 6:
    FUN_0042ad10(cnt, 0xff110000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x58;
    switch (r) {
    case 6:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 8:
    FUN_0042ad10(cnt, 0xff230000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = FUN_0042a9c0();
    switch (r) {
    case 8:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 10:
      DAT_00898ac0[*cnt].s2 = 0x133;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  case 10:
    FUN_0042ad10(cnt, 0xff100000, 0x40, 0x1ac, 0);
    DAT_00898ac0[*cnt].s1 = 0x133;
    switch (r) {
    case 10:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 0x1000;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 1:
      DAT_00898ac0[*cnt].s2 = 0x42;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 2:
      DAT_00898ac0[*cnt].s2 = 0x43;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 4:
      DAT_00898ac0[*cnt].s2 = 0x225;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 5:
      DAT_00898ac0[*cnt].s2 = 0x48;
      DAT_00898ac0[*cnt].s2 = 0x13;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 6:
      DAT_00898ac0[*cnt].s2 = 0x58;
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    case 8:
      DAT_00898ac0[*cnt].s2 = FUN_0042a9c0();
      DAT_00898ac0[*cnt].f4 = 0;
      DAT_00898ac0[*cnt].f10 = 0x1ff;
      break;
    default:
      DAT_00898ac0[*cnt].s2 = -1;
      DAT_00898ac0[*cnt].f4 = 1;
      DAT_00898ac0[*cnt].f10 = 0;
      break;
    }
    (*cnt)++;
    break;
  }
}
#ifndef MATCH_PORT
void match_standin_00432b30(int mode, int sel, int menu)
{
  int n;
  n = 0;
  FUN_00432b30(mode, &n, sel, menu);
}
#endif
