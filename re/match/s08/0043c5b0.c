// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043c5b0 (MenuDrawLoop): 40.5% (regs 61.1%), raw g2s did not compile (C2120). By hand from the listing: RwRGBA locals (white/black/0x40f8d0e8) and V2 pos/size pairs initialised at declaration, passed by value to FUN_00472c60 / FUN_00473540 / FUN_004739f0 (that is why the original keeps the dead {0,0,639,479} and 184/0/128/76 stores), 0x34-byte MenuItem table at 0x00898ac0 walked by index (30 rows), sparse switch on the item type, FUN_0042aad0 as a static eax-arg callee (&colour; also used by 0x0043af10). Tried: pointer vs index loop, struct vs scalar rects, scalar vs V2 uv args, 145 random local declaration orders (32.8 -> 40.5%). Residual (b): local stack slot assignment (colours at 0/4/8, row at 0xc, rects at 0x18..0x34 in the original) and the loop pointer bias (+0x2c in the original, +4 here) still differ, out of budget (60 min). */
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
typedef struct { uchar r, g, b, a; } RwRGBA;
typedef struct { float x, y; } V2;
typedef struct {
  int *def;
  int id;
  int sel[13];
  int count;
} MenuLevel;
typedef struct {
  int type;
  int f4;
  int f8;
  RwRGBA rgba;
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
typedef int (*RwStateSetFn)(int state, int value);
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
MGLOBAL(int, DAT_0067e914, 0x0067e914);
MGLOBAL(int, DAT_008990e4, 0x008990e4);
MGLOBALA(int, DAT_005f6da0, 22, 0x005f6da0);
MGLOBALA(int, DAT_005f6df8, 22, 0x005f6df8);
MGLOBALA(int, DAT_005f72a0, 52, 0x005f72a0);
MGLOBALA(int, DAT_005f7370, 52, 0x005f7370);
MGLOBALA(MenuLevel, DAT_0067ed78, 16, 0x0067ed78);
MGLOBALA(MenuItem, DAT_00898ac0, 30, 0x00898ac0);
MGLOBALA(char, s_Button_005cda7c, 7, 0x005cda7c);
void *FUN_0040bb50(char *name);
void FUN_00427e00(int id, float x, float y, RwRGBA col, float scale, int flags);
void FUN_00428140(int id, float x, float y, RwRGBA col, float scale, int align, int f10);
void FUN_0042aae0(int a);
int FUN_0042b930(void);
void FUN_0042e3a0(void);
void FUN_0042e5b0(int a);
void FUN_0043bf30(void);
void FUN_00472c60(V2 pos, V2 size, RwRGBA col);
void FUN_00472dc0(float x0, float y0, float x1, float y1, float x2, float y2, RwRGBA col);
void FUN_00473540(V2 pos, V2 size, RwRGBA col);
void FUN_004739f0(void *tex, V2 pos, V2 size, RwRGBA col, float u0, float v0, float u1, float v1, int a, int b);

#define RS(s, v) (*(RwStateSetFn *)(DAT_007d3ff8 + 0x20))(s, v)

// MATCH: 0x0042aad0 FUN_0042aad0
static void FUN_0042aad0(RwRGBA *col)
{
  col->a = 0x30;
  DAT_008990e4 = 1;
}

// MATCH: 0x0043c5b0 FUN_0043c5b0
void FUN_0043c5b0(void)
{
  float t;
  int s;
  V2 size1 = { 639.0f, 479.0f };
  int *prev;
  int i;
  int d;
  RwRGBA colC = { 0, 0, 0, 0xff };
  int row;
  int n;
  V2 pos1 = { 0.0f, 0.0f };
  V2 pos2;
  V2 bsize;
  int lvl;
  V2 bpos;
  V2 size2;
  int back;
  RwRGBA colA = { 0xff, 0xff, 0xff, 0xff };
  RwRGBA colB = { 0xe8, 0xd0, 0xf8, 0x40 };

  if (DAT_0067eca4 == 0) {
    return;
  }
  if (DAT_0067eca4 == 1) {
    FUN_0042aae0(1);
    RS(6, 1);
    RS(8, 1);
    return;
  }
  RS(6, 0);
  RS(8, 1);
  if (DAT_0067eca4 < 5) {
    FUN_0042e5b0(0);
    FUN_0042e3a0();
    if (FUN_0042b930() != 0x21) {
      size1.y = 184.0f;
      size2.x = 0.0f;
      size2.y = 128.0f;
      pos2.x = 76.0f;
      FUN_00427e00(0x41, 600.0f, 52.0f, colC, 0.8f, 1);
      colA.r = 0xff;
      colA.g = 0xff;
      colA.b = 0xff;
      colA.a = 0xff;
      FUN_00427e00(0x41, 596.0f, 48.0f, colA, 0.8f, 1);
    }
  }
  FUN_0042aae0(1);
  if (DAT_0067eca4 < 4) {
    row = 0;
    for (i = 0; i < 30; i++) {
      switch (DAT_00898ac0[i].type) {
      case 0xff040000:
        pos2.y = (DAT_00898ac0[i].y - 13.0f) + 1.0f;
        pos1.y = pos2.y;
        row++;
        prev = DAT_0067ed78[DAT_0067e9f8 - 1].def;
        colA.a = 0xff;
        pos1.x = 60.0f;
        if (prev == DAT_005f72a0 || prev == DAT_005f7370 || prev == DAT_005f6da0 ||
            (prev == DAT_005f6df8 && DAT_0067e844 != 0)) {
          size1.x = 166.0f;
          size2.x = 84.0f;
          pos2.x = 226.0f;
        } else {
          size1.x = 210.0f;
          pos2.x = 270.0f;
          size2.x = 100.0f;
        }
        size2.y = 26.0f;
        size1.y = 26.0f;
        if (DAT_00898ac0[i].f10 < 0x1ff) {
          d = DAT_00898ac0[i].f10 / 10;
          if (d < 13) {
            pos2.y = DAT_00898ac0[i].y - (float)d;
            pos1.y = pos2.y;
            size2.y = size1.y = (float)d + (float)d;
          }
        }
        if (DAT_0067e914 != 0) {
          back = 1;
        } else {
          back = (DAT_0067e844 != 0) - 1 & 2;
        }
        if (DAT_00898ac0[i].f4 == 0 || DAT_00898ac0[i].f4 == 2) {
          s = DAT_00898ac0[i].s2;
          if (s == -1) {
            break;
          }
          lvl = DAT_0067e9f8 - back;
          if (DAT_0067ed78[lvl].sel[row] == 0) {
            FUN_0042aad0(&colA);
          }
          if (row - 1 == DAT_0067ed78[lvl].sel[0]) {
            if (s != 0x224) {
              colA.r = 0xf0;
              colA.g = 0x6e;
              colA.b = 0x14;
              colA.a = 0xa0;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              colA.r = 0xb4;
              colA.g = 0x50;
              colA.b = 0x10;
              colA.a = 0xff;
              if (size1.y > 2.0f) {
                n = (int)size2.y;
                size1.y = 2.0f;
                size2.y = 2.0f;
                FUN_00472c60(pos1, size1, colA);
                FUN_00473540(pos2, size2, colA);
                t = (float)(n - 2);
                pos1.y = t + pos1.y;
                pos2.y = t + pos2.y;
                FUN_00472c60(pos1, size1, colA);
                FUN_00473540(pos2, size2, colA);
                pos1.y = pos1.y - t;
                size1.x = 2.0f;
                pos1.x = 58.0f;
                size1.y = (float)n;
                FUN_00472c60(pos1, size1, colA);
              }
              bpos.x = pos1.x - 13.0f;
              bpos.y = pos1.y;
              bsize.x = 13.0f;
              bsize.y = size1.y;
              FUN_004739f0(FUN_0040bb50(s_Button_005cda7c), bpos, bsize, colA, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
            }
            colA.a = 0xff;
          } else {
            FUN_00472c60(pos1, size1, colB);
            FUN_00473540(pos2, size2, colB);
            colA.r = 0x13;
            colA.g = 0x15;
            colA.b = 0x50;
            if (size1.y > 2.0f) {
              n = (int)size2.y;
              size1.y = 2.0f;
              size2.y = 2.0f;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              t = (float)(n - 2);
              pos1.y = t + pos1.y;
              pos2.y = t + pos2.y;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              pos1.y = pos1.y - t;
              size1.x = 2.0f;
              pos1.x = 58.0f;
              size1.y = (float)n;
              FUN_00472c60(pos1, size1, colA);
            }
          }
          colA.r = 0;
          colA.g = 0;
          colA.b = 0;
          if (DAT_008990e4 != 0) {
            colA.b = 0;
            colA.g = 0;
            colA.r = 0;
            colA.a = 0x80;
            FUN_00428140(DAT_00898ac0[i].s2, DAT_00898ac0[i].x, DAT_00898ac0[i].y, colA, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, 0x80);
          } else if (DAT_00898ac0[i].s2 == 0x224) {
            FUN_00428140(0x224, 320.0f, DAT_00898ac0[i].y, colA, 0.7f, 2, DAT_00898ac0[i].f10);
          } else {
            FUN_00428140(DAT_00898ac0[i].s2, DAT_00898ac0[i].x, DAT_00898ac0[i].y, colA, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
          }
        }
        if (DAT_00898ac0[i].f4 != 0 && DAT_00898ac0[i].f4 != 2) {
          s = DAT_00898ac0[i].s1;
          if (s == -1) {
            break;
          }
          lvl = DAT_0067e9f8;
          if (DAT_0067ed78[lvl - 1].sel[row] == 0) {
            FUN_0042aad0(&colA);
          }
          if (row - 1 == DAT_0067ed78[lvl - 1].sel[0]) {
            if (s == 0x224) {
              colA.r = 0xf0;
              colA.g = 0x6e;
              colA.b = 0x14;
              colA.a = 0xa0;
              FUN_00472dc0(280.0f, 0.0f, 360.0f, 0.0f, 320.0f, 40.0f, colA);
              FUN_00472dc0(280.0f, 480.0f, 360.0f, 480.0f, 320.0f, 440.0f, colA);
              FUN_00472dc0(0.0f, 200.0f, 0.0f, 280.0f, 40.0f, 240.0f, colA);
              FUN_00472dc0(640.0f, 200.0f, 640.0f, 280.0f, 600.0f, 240.0f, colA);
            }
            if (DAT_00898ac0[i].s1 != 0x224) {
              colA.r = 0xf0;
              colA.g = 0x6e;
              colA.b = 0x14;
              colA.a = 0xa0;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              colA.r = 0xb4;
              colA.g = 0x50;
              colA.b = 0x10;
              colA.a = 0xff;
              if (size1.y > 2.0f) {
                n = (int)size2.y;
                size1.y = 2.0f;
                size2.y = 2.0f;
                FUN_00472c60(pos1, size1, colA);
                FUN_00473540(pos2, size2, colA);
                t = (float)(n - 2);
                pos1.y = t + pos1.y;
                pos2.y = t + pos2.y;
                FUN_00472c60(pos1, size1, colA);
                FUN_00473540(pos2, size2, colA);
                pos1.y = pos1.y - t;
                size1.x = 2.0f;
                pos1.x = 58.0f;
                size1.y = (float)n;
                FUN_00472c60(pos1, size1, colA);
              }
              bpos.x = pos1.x - 13.0f;
              bpos.y = pos1.y;
              bsize.x = 13.0f;
              bsize.y = size1.y;
              FUN_004739f0(FUN_0040bb50(s_Button_005cda7c), bpos, bsize, colA, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
            }
            colA.a = 0xff;
          } else {
            FUN_00472c60(pos1, size1, colB);
            FUN_00473540(pos2, size2, colB);
            colA.r = 0x13;
            colA.g = 0x15;
            colA.b = 0x50;
            if (size1.y > 2.0f) {
              n = (int)size2.y;
              size1.y = 2.0f;
              size2.y = 2.0f;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              t = (float)(n - 2);
              pos1.y = t + pos1.y;
              pos2.y = t + pos2.y;
              FUN_00472c60(pos1, size1, colA);
              FUN_00473540(pos2, size2, colA);
              pos1.y = pos1.y - t;
              size1.x = 2.0f;
              pos1.x = 58.0f;
              size1.y = (float)n;
              FUN_00472c60(pos1, size1, colA);
            }
          }
          colA.r = 0;
          colA.g = 0;
          colA.b = 0;
          if (DAT_008990e4 != 0) {
            colA.b = 0;
            colA.g = 0;
            colA.r = 0;
            colA.a = 0x80;
            FUN_00428140(DAT_00898ac0[i].s1, DAT_00898ac0[i].x, DAT_00898ac0[i].y, colA, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, 0x80);
          } else if (DAT_00898ac0[i].s1 == 0x224) {
            FUN_00428140(0x224, 320.0f, DAT_00898ac0[i].y, colA, 0.7f, 2, DAT_00898ac0[i].f10);
          } else {
            FUN_00428140(DAT_00898ac0[i].s1, DAT_00898ac0[i].x, DAT_00898ac0[i].y, colA, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
          }
        }
        break;
      case 0xff000000:
      case 0xff100000:
      case 0xff110000:
      case 0xff120000:
      case 0xff130000:
      case 0xff230000:
        if (DAT_00898ac0[i].f4 == 0 || DAT_00898ac0[i].f4 == 2) {
          if (DAT_00898ac0[i].s2 == -1) {
            break;
          }
          FUN_00428140(DAT_00898ac0[i].s2, DAT_00898ac0[i].x + 3.0f, DAT_00898ac0[i].y + 3.0f, colC, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
          FUN_00428140(DAT_00898ac0[i].s2, DAT_00898ac0[i].x, DAT_00898ac0[i].y, DAT_00898ac0[i].rgba, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
        }
        if (DAT_00898ac0[i].f4 != 0 && DAT_00898ac0[i].f4 != 2 && DAT_00898ac0[i].s1 != -1) {
          FUN_00428140(DAT_00898ac0[i].s1, DAT_00898ac0[i].x + 3.0f, DAT_00898ac0[i].y + 3.0f, colC, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
          FUN_00428140(DAT_00898ac0[i].s1, DAT_00898ac0[i].x, DAT_00898ac0[i].y, DAT_00898ac0[i].rgba, DAT_00898ac0[i].scale, DAT_00898ac0[i].f24, DAT_00898ac0[i].f10);
        }
        break;
      }
    }
    FUN_0043bf30();
  }
  RS(6, 1);
  RS(8, 1);
}
