// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043aa30 (1187 B): best 50.5% (regs 58.7%, ops 91.1%, 314/315 ins), by hand from the raw 31.7%.
   The file holds the static callee 0x0042f8d0 at 100% (relocheck OK; alpha byte in AL, two 2-float structs by
   value, struct copy osz = size). The original's
   store-then-reload of every float constant comes from the structs passed by value (body started from h2's
   0x00434720 version, rewritten to struct args). Structure (loops, switch on the row's col,
   counters, 0x7f1a14 slot table, 0x0067e938 row table, string build) matches: ops 91%.
   Residual: stack slot layout and register assignment. The original's local order is not the declaration order
   (reversing every declaration and renaming locals moved nothing), so this compiler places locals by use, and the
   use order that reproduces it was not found in budget. Class (b). Sibling of 0x0043a610 (same TU shape). */
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
typedef struct { uchar r, g, b, a; } MatchRGBA;
typedef struct { int id; int grp; int icon; int pad; } MatchSlot;
typedef struct { int col; int f4; int f8; } MatchRow;
typedef struct { float x; float y; } MatchV2;
typedef int (*MatchRenderStateSet)(int state, int value);
MGLOBAL(uchar, DAT_0067e7e4, 0x0067e7e4);
MGLOBAL(int, DAT_0067ea7c, 0x0067ea7c);
MGLOBAL(int, DAT_008990dc, 0x008990dc);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
MGLOBALA(MatchRow, DAT_0067e938, 12, 0x0067e938);
MGLOBALA(MatchSlot, DAT_007f1a14, 4, 0x007f1a14);
void FUN_00427e00(int id, float x, float y, MatchRGBA c, float scale, int align);
void FUN_00427f00(char *s, float x, float y, MatchRGBA c, float scale, int align);
float FUN_004282a0(int id, float scale);
void FUN_0042bcb0(int id, float x, float y, MatchRGBA c, int flag);
void *FUN_0042fab0(int id);
void FUN_00472c60(MatchV2 pos, MatchV2 size, MatchRGBA c);
void FUN_004739f0(void *tex, MatchV2 pos, MatchV2 size, MatchRGBA c, float u0, float v0, float u1, float v1,
                  int p10, int p11);

// MATCH: 0x0042f8d0 FUN_0042f8d0
static void FUN_0042f8d0(uchar a, MatchV2 pos, MatchV2 size)
{
  MatchRGBA c;
  MatchV2 osz;

  osz = size;
  c.r = 0xf0;
  c.g = 0x6e;
  c.b = 0x14;
  c.a = a >> 1;
  FUN_00472c60(pos, size, c);
  c.r = 0xb4;
  c.g = 0x50;
  c.b = 0x10;
  c.a = a;
  pos.x -= 2.0f;
  size.x = 2.0f;
  FUN_00472c60(pos, size, c);
  size.x = osz.x + 4.0f;
  size.y = 2.0f;
  FUN_00472c60(pos, size, c);
  pos.y += osz.y - 2.0f;
  FUN_00472c60(pos, size, c);
  pos.x = osz.x + pos.x + 2.0f;
  size.x = 2.0f;
  size.y = osz.y;
  pos.y -= osz.y - 2.0f;
  FUN_00472c60(pos, size, c);
}

// MATCH: 0x0043aa30 FUN_0043aa30
void FUN_0043aa30(void)
{
  uchar a;
  char buf[4];
  float f;
  int row;
  MatchRGBA black;
  MatchV2 pos;
  MatchRGBA white;
  MatchRGBA black2;
  MatchV2 ipos;
  MatchRGBA c38;
  MatchRGBA c3c;
  MatchRGBA c40;
  MatchRGBA c44;
  MatchRGBA c48;
  int x2;
  int x1;
  int cnt2;
  int cnt1;
  float bx;
  MatchV2 size;
  MatchV2 tp;
  MatchV2 isize;
  MatchRGBA *pal[6];
  int j;
  int k;
  int col;
  void *tex;
  int n;

  a = DAT_0067e7e4;
  c3c.r = 0xeb;
  c3c.g = 0xa7;
  c3c.b = 0xa7;
  c3c.a = a;
  c38.r = 0xdb;
  c38.g = 0xc3;
  c38.b = 0x62;
  c38.a = a;
  c40.r = 0x61;
  c40.g = 0x76;
  c40.b = 0x56;
  c40.a = a;
  c44.r = 0x98;
  c44.g = 0x3a;
  c44.b = 0x3d;
  c44.a = a;
  c48.r = 0x4e;
  c48.g = 0x89;
  c48.b = 0xae;
  c48.a = a;
  black.r = 0;
  black.g = 0;
  black.b = 0;
  black.a = a;
  pal[0] = &c44;
  pal[1] = &c48;
  pal[2] = &c40;
  pal[3] = &c38;
  pal[4] = &c3c;
  pal[5] = &black;
  DAT_0067ea7c = 0;
  DAT_008990dc = 1;
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  size.y = 28.0f;
  size.x = 140.0f;
  pos.y = 140.0f;
  pos.x = 250.0f;
  FUN_0042f8d0(a, pos, size);
  FUN_00427e00(0x9f, 320.0f, 154.0f, black, 0.8f, 2);
  pos.x = 430.0f;
  FUN_0042f8d0(a, pos, size);
  FUN_00427e00(0xa0, 500.0f, 154.0f, black, 0.8f, 2);
  pos.x = 70.0f;
  pos.y = 200.0f;
  size.x = 510.0f;
  isize.x = 48.0f;
  isize.y = 48.0f;
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = a;
  black2.r = 0;
  black2.g = 0;
  black2.b = 0;
  black2.a = a;
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  cnt1 = 0;
  cnt2 = 0;
  for (row = 0; row < 12; row++) {
    x1 = cnt1 * 0x22 + 0x1ae;
    x2 = cnt2 * 0x22 + 0xfa;
    for (j = 0; j < 4; j++) {
      if (DAT_007f1a14[j].id == row) {
        FUN_0042f8d0(a, pos, size);
        tex = FUN_0042fab0(DAT_007f1a14[j].icon);
        col = DAT_0067e938[row].col;
        bx = (float)(col * 0xb4) + 82.0f;
        ipos.x = bx;
        f = pos.y - 2.0f;
        ipos.y = f - 8.0f;
        FUN_004739f0(tex, ipos, isize, white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        ipos.x = bx;
        ipos.y = f;
        if (col != 0) {
          if (col == 1) {
            tp.x = (float)x2;
            cnt2++;
            x2 += 0x22;
            tp.y = 108.0f;
          } else if (col == 2) {
            tp.x = (float)x1;
            cnt1++;
            x1 += 0x22;
            tp.y = 108.0f;
          }
          FUN_004739f0(tex, tp, isize, white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        }
        f = ipos.y + 15.0f;
        FUN_0042bcb0(DAT_007f1a14[j].id, ipos.x + 70.0f, f, *pal[DAT_007f1a14[j].icon],
                     DAT_007f1a14[j].icon == 5);
        n = (int)FUN_004282a0(0x36, 0.9f) / 2;
        if (DAT_007f0fe8 != 0) {
          k = row;
          if (row >= 6) {
            k = row - 6;
          }
          buf[0] = k + '1';
        } else {
          buf[0] = row + '1';
        }
        buf[1] = 0;
        FUN_00427f00(buf, (float)n + 4.0f + ipos.x + 65.0f, f, black2, 0.6f, 0);
        pos.y += 40.0f;
      }
    }
  }
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
