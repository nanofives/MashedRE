// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x004325c0 (564 B): 100% (regs 100%, ops 100%), byte-identical. Raw g2s 16.4%.
   Rebuilt from the listing: speed = (DAT_0067eca4 >= 4) ? 2.0f : 1.0f held on the x87 stack for the whole loop,
   0x004a2c48 is _ftol (`(int)(DAT_007f1004 * speed * K)`), a switch on item->type (case tree 0xff110000 split),
   loop over the 30 0x34-byte items at 0x00898ac0 by INDEX (a pointer loop became a stack down-counter, 69.2%).
   FUN_0042ac50 takes (n in EAX, spacing in ECX): this file holds it as a static callee defined before the caller
   (body as in re/match/s08/0043d2a0.c, 100% there and here). DAT_0067ece0 stays in EBP across the loop because
   both callees are known. MenuItemF = the MenuItem layout of 0043d2a0.c with +0x08 as float (this function
   adds to it and stores 180.0f there); field names are offsets/roles only. Residual class: none. */
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
  int type;
  int state;
  float f8;
  int fc;
  int cnt;
  float scale;
  float x;
  float y;
  int idx;
  int f24;
  int s1;
  int s2;
  int *def;
} MenuItemF;
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(int, DAT_0067e914, 0x0067e914);
MGLOBAL(int, DAT_0067ece0, 0x0067ece0);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBALA(MenuItemF, DAT_00898ac0, 30, 0x00898ac0);
MGLOBALA(int, DAT_005f72a0, 52, 0x005f72a0);
MGLOBALA(int, DAT_005f7370, 52, 0x005f7370);
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
// MATCH: 0x004325c0 FUN_004325c0
int FUN_004325c0(void)
{
  float speed;
  int result;
  MenuItemF *it;
  int st;
  int k;
  int i;

  if (DAT_0067eca4 >= 4) {
    speed = 2.0f;
  }
  else {
    speed = 1.0f;
  }
  result = 1;
  for (i = 0; i < 30; i++) {
    it = &DAT_00898ac0[i];
    if (it->type != 0 && it->state != 0x1000 && DAT_0067e914 != 0) {
      result = 0;
    }
    switch (it->type) {
    case 0xff040000:
      st = it->state;
      if (st != 0 && st != 2) {
        it->cnt += (int)(DAT_007f1004 * speed * 1000.0f);
        if (it->cnt >= 400) {
          it->cnt = 0x1ff;
          it->state = 0x1000;
        }
      }
      else {
        it->cnt += (int)(DAT_007f1004 * speed * -2000.0f);
        if (it->cnt <= 0) {
          if (st == 2) {
            it->cnt = 0x1ff;
            it->state = 0x1000;
          }
          else {
            k = it->idx;
            it->cnt = 0;
            it->state = 1;
            it->y = (float)FUN_0042ac50(DAT_0067ece0, 0x1e) + k * 0x1e;
            if (it->def == DAT_005f7370) {
              it->y = (float)FUN_0042ac50(DAT_0067ece0, 0x1c) + k * 0x1c - 58.0f;
            }
            if (it->def == DAT_005f72a0) {
              it->y = it->y - 58.0f;
            }
          }
        }
      }
      break;
    case 0xff000000:
    case 0xff100000:
    case 0xff110000:
    case 0xff120000:
    case 0xff130000:
    case 0xff230000:
      st = it->state;
      if (st != 0 && st != 2) {
        it->cnt += (int)(DAT_007f1004 * speed * 500.0f);
        if (it->cnt >= 400) {
          it->cnt = 0x1ff;
          it->state = 0x1000;
        }
      }
      else {
        it->cnt += (int)(DAT_007f1004 * speed * -800.0f);
        if (it->cnt <= 0) {
          if (st == 2) {
            it->cnt = 0x1ff;
            it->state = 0x1000;
          }
          else {
            it->cnt = 0;
            it->state = 1;
          }
        }
      }
      break;
    default:
      it->f8 = DAT_007f1004 * 300.0f + it->f8;
      if (it->f8 >= 180.0f) {
        it->f8 = 180.0f;
        it->state = 0x1000;
      }
      break;
    }
  }
  return result;
}
