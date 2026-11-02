// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00439210 (5109 B): best 40.5% (regs 58.2%, ops 83.2%, 1433/1468 ins), by hand (raw did not compile).
   The original keeps ECX and x87 stack values live across calls to 0x0042b8b0 / 0x0042b8c0 / 0x00430ab0, so
   those callees are static functions of this TU, defined before it (the compiler knows their clobber set).
   The file holds them, all at 100%: 0x0042b8b0, 0x0042b8c0 (short screen-size getters), 0x00430a10, 0x00430a60,
   0x00430ab0 (switches on DAT_0067e9fc with no return on the default path; one `return` per case keeps the one-level
   jump table), and 0x004391b0 (EAX arg, real caller here). {x,y} float structs by value for 0x00472c60 and
   0x00473870 (same as 0x0043a610). Structure: lobby header switch 6/7/8 (case 8 first, goto into case 7's tail),
   row count, scroll clamp, highlight bar, two row loops (text rows; icon slots with the 2-pass selected-on-top
   draw), 0x007f0a40 as 12-int records.
   Residual: stack slot layout, register assignment and some x87 scheduling in the two row loops; the second loop's
   FPU-carried subexpressions (sx-hw, sy-hh, dw, dh) are written as plain expressions here. Ran out of the 60 min
   budget: class (b). */
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
typedef struct { float x; float y; } MatchV2;
typedef struct { int f[12]; } MatchProfile;
typedef int (*MatchRenderStateSet)(int state, int value);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(uchar, DAT_0067e7bc, 0x0067e7bc);
MGLOBAL(int, DAT_0067f1a8, 0x0067f1a8);
MGLOBAL(float, DAT_0067ed30, 0x0067ed30);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBAL(int, DAT_0067f180, 0x0067f180);
MGLOBAL(int, DAT_0067f184, 0x0067f184);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
MGLOBALA(MatchProfile, DAT_007f0a40, 12, 0x007f0a40);
MGLOBALA(char, s_check_005cd7ac, 6, 0x005cd7ac);
MGLOBALA(char, s_lock_005cd7b8, 5, 0x005cd7b8);
MGLOBALA(char, s_tick_005cda3c, 5, 0x005cda3c);
MGLOBALA(char, s_Lock_005cda44, 5, 0x005cda44);
MGLOBALA(char, s_Star_005cd970, 5, 0x005cd970);
#pragma intrinsic(sin)
double sin(double);
int FUN_0040bb50(const char *name);
int FUN_0040bb90(const char *name);
void FUN_00427e00(int id, float x, float y, MatchRGBA c, float scale, int align);
int FUN_0042ebe0(int row);
int FUN_0042ee00(int v);
int FUN_0042ee40(int v);
int FUN_0042ef40(int row, int v);
int FUN_0042f500(void);
int FUN_00430760(void);
int FUN_00436810(int row);
void FUN_004368e0(uchar a);
void FUN_00472c60(MatchV2 pos, MatchV2 size, MatchRGBA c);
void FUN_00472dc0(float x0, float y0, float x1, float y1, float x2, float y2, MatchRGBA c);
void FUN_00473870(int tex, MatchV2 pos, MatchV2 size, MatchRGBA c, int n);

// MATCH: 0x0042b8b0 FUN_0042b8b0
static short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}

// MATCH: 0x0042b8c0 FUN_0042b8c0
static short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}

// MATCH: 0x00430a10 FUN_00430a10
static int FUN_00430a10(void)
{
  switch (DAT_0067e9fc) {
  case 2:
    return 0;
  case 3:
  case 4:
  case 5:
    return 1;
  case 6:
  case 7:
  case 8:
  case 9:
    return 3;
  case 10:
    return 0xb;
  }
}

// MATCH: 0x00430a60 FUN_00430a60
static int FUN_00430a60(void)
{
  switch (DAT_0067e9fc) {
  case 2:
    return 0;
  case 3:
    return 2;
  case 4:
    return 2;
  case 5:
    return 2;
  case 6:
    return 0;
  case 7:
    return 0;
  case 8:
    return 0;
  case 9:
    return 0;
  case 10:
    return 0;
  }
}

// MATCH: 0x00430ab0 FUN_00430ab0
static int FUN_00430ab0(void)
{
  switch (DAT_0067e9fc) {
  case 2:
    return 0;
  case 3:
    return 5;
  case 4:
    return 5;
  case 5:
    return 5;
  case 6:
    return 0;
  case 7:
    return 0;
  case 8:
    return 0;
  case 9:
    return 0;
  case 10:
    return 0;
  }
}

// MATCH: 0x004391b0 FUN_004391b0
static int FUN_004391b0(int kind)
{
  if (kind == 0) {
    return FUN_0040bb90(s_Lock_005cda44);
  }
  if (kind == 1) {
    return FUN_0040bb90(s_Star_005cd970);
  }
  if (kind == 2) {
    if (FUN_00430760() != 0 && DAT_0067e9fc != 10 && DAT_0067e9fc != 2) {
      return FUN_0040bb90(s_tick_005cda3c);
    }
    return 0;
  }
  if (kind == 3) {
    return FUN_0040bb90(s_Star_005cd970);
  }
  return 0;
}

// MATCH: 0x00439210 FUN_00439210
void FUN_00439210(void)
{
  uchar a;
  int ia;
  MatchRGBA shade;
  MatchRGBA white;
  MatchRGBA txt;
  float pulse;
  float p;
  MatchV2 pos;
  MatchV2 size;
  MatchV2 size2;
  MatchV2 bpos;
  MatchV2 bsize;
  MatchV2 spos;
  MatchV2 ssz;
  MatchV2 dsz;
  float base;
  float top;
  float y0;
  float fy;
  float fa;
  float rowy;
  float hw;
  float hh;
  int n;
  int i;
  int j;
  int t;
  int d;
  int alpha;
  int kind;
  int m;
  int v;
  int which;
  int tex;
  int rec;
  int yy;
  uchar sa;

  shade.r = 0;
  shade.g = 0;
  shade.b = 0;
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  a = DAT_0067e7bc;
  if (a == 0xff) {
    DAT_0067f1a8 = 0;
  }
  ia = a;
  DAT_0067ed30 = DAT_007f1004 + DAT_0067ed30;
  shade.a = (uchar)((ia * 0xa0) / 0xff);
  pulse = ((float)sin(DAT_0067ed30 * 10.0f) + 1.0f) * 12.0f;
  FUN_004368e0(DAT_0067f1a8 != 0 ? 0xff : a);
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = a;
  size.x = (float)FUN_0042b8b0() * 0.0375f;
  size.y = (float)FUN_0042b8c0() * 0.05f;
  pos.x = (float)((FUN_0042b8b0() * 0x208) / 0x280);
  pos.y = (float)((FUN_0042b8c0() * 0x140) / 0x1e0);
  if (FUN_00430760() == 0) {
    switch (DAT_0067e9fc) {
    case 6:
      FUN_00427e00(DAT_0067ea64 != 0 ? 0x140 : 0x22, 330.0f, 316.0f, white, 0.6f, 0);
      FUN_00427e00(0x56, 330.0f, 334.0f, white, 0.6f, 0);
      if (DAT_007f0a40[DAT_0067f17c].f[4] != 0) {
        FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      } else {
        FUN_00473870(FUN_0040bb50(s_lock_005cd7b8), pos, size, white, 1);
      }
      FUN_00427e00(0x24b, 330.0f, 350.0f, white, 0.6f, 0);
      pos.y += (float)((FUN_0042b8c0() * 0x10) / 0x1e0);
      if (DAT_007f0a40[DAT_0067f17c].f[6] != 0) {
        FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      } else {
        FUN_00473870(FUN_0040bb50(s_lock_005cd7b8), pos, size, white, 1);
      }
      FUN_00427e00(0x141, 330.0f, 366.0f, white, 0.6f, 0);
      pos.y += (float)((FUN_0042b8c0() * 0x10) / 0x1e0);
      if (DAT_007f0a40[DAT_0067f17c].f[7] != 0 && FUN_0042f500() == 0) {
        FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      } else {
        FUN_00473870(FUN_0040bb50(s_lock_005cd7b8), pos, size, white, 1);
      }
      break;
    case 8:
      FUN_00427e00(0x143, 330.0f, 316.0f, white, 0.6f, 0);
      goto common78;
    case 7:
      FUN_00427e00(0x142, 330.0f, 316.0f, white, 0.6f, 0);
    common78:
      FUN_00427e00(0x151, 332.0f, 334.0f, white, 0.6f, 0);
      FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      FUN_00427e00(0x152, 332.0f, 350.0f, white, 0.6f, 0);
      pos.y += (float)((FUN_0042b8c0() * 0x10) / 0x1e0);
      FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      FUN_00427e00(0x24b, 332.0f, 366.0f, white, 0.6f, 0);
      pos.y += (float)((FUN_0042b8c0() * 0x10) / 0x1e0);
      FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      FUN_00427e00(0x141, 332.0f, 382.0f, white, 0.6f, 0);
      pos.y += (float)((FUN_0042b8c0() * 0x10) / 0x1e0);
      FUN_00473870(FUN_0040bb50(s_check_005cd7ac), pos, size, white, 1);
      break;
    }
  }
  n = 0;
  for (i = 0; i < 13; i++) {
    if (i >= 10) {
      if (FUN_0042ebe0(i) != 0) {
        n++;
      }
    } else if (i <= 0 || FUN_00436810(i) != 0) {
      n++;
    }
  }
  base = 118.0f;
  if (n < 7) {
    base = (float)(((7 - n) * 20) / 2) + 118.0f;
  }
  top = (float)DAT_0067f180 + base;
  t = DAT_0067f17c * 0x16 + DAT_0067f180;
  if (t > 0x84) {
    DAT_0067f180 -= 3;
  }
  if (t < 0) {
    DAT_0067f180 += 3;
  }
  y0 = top - 11.0f;
  pos.x = 0.0f;
  size.x = 310.0f;
  pos.y = (float)DAT_0067f17c * 22.0f + y0;
  if (DAT_0067e9fc == 2 || DAT_0067e9fc == 10 || DAT_0067e9fc == 6) {
    size.x = 270.0f;
  }
  size.y = 22.0f;
  white.r = 0xf7;
  white.g = 0x94;
  white.b = 0x1d;
  if (DAT_0067f17c > 9) {
    white.r = 0x1d;
    white.g = 0x94;
    white.b = 0xf7;
  }
  white.a = a >> 1;
  FUN_00472c60(pos, size, white);
  white.r = 0xb4;
  white.g = 0x64;
  white.b = 0x0f;
  if (DAT_0067f17c > 9) {
    white.r = 0x0f;
    white.g = 0x64;
    white.b = 0xb4;
  }
  white.a = a;
  size.y = 2.0f;
  FUN_00472c60(pos, size, white);
  pos.y += 20.0f;
  FUN_00472c60(pos, size, white);
  pos.x = size.x;
  pos.y -= 20.0f;
  size.y = 22.0f;
  size.x = 2.0f;
  FUN_00472c60(pos, size, white);
  if (DAT_0067f17c != 0) {
    FUN_00472dc0(152.0f, 96.0f, 200.0f, 96.0f, 176.0f, 72.0f, shade);
  }
  if (DAT_0067f17c < n - 1) {
    FUN_00472dc0(152.0f, 272.0f, 200.0f, 272.0f, 176.0f, 296.0f, shade);
  }
  for (i = 0, yy = 0; yy < 0x11e; yy += 0x16, i++) {
    fy = (float)yy;
    fa = fy + top;
    t = (int)fa;
    alpha = 0x100;
    if (t < 0x76) {
      d = 0x76 - t;
      if (d > 0x16) {
        d = 0x16;
      }
      alpha = ((0x16 - d) * 0x100) / 0x16;
    }
    if (t > 0xfa) {
      d = t - 0xfa;
      if (d > 0x16) {
        d = 0x16;
      }
      alpha = ((0x16 - d) * 0x100) / 0x16;
    }
    txt.r = 0;
    txt.g = 0;
    txt.b = 0;
    txt.a = (uchar)((alpha * ia) / 0x100);
    kind = 2;
    if (yy >= 0xdc) {
      if (FUN_0042ebe0(i) == 0) {
        kind = 0;
      }
    } else if (yy > 0 && FUN_00436810(i) == 0) {
      kind = 1;
    }
    p = 0.6f;
    if (i != DAT_0067f17c) {
      p = 0.58f;
    }
    if (kind == 2) {
      FUN_00427e00(i + 0x49, 64.0f, fa, txt, p, 0);
      m = DAT_0067e9fc;
      if (m == 4 || m == 5) {
        m = 3;
      }
      if (FUN_0042ef40(i, m) != 0 && yy < 0xdc) {
        txt.r = 0xff;
        txt.g = 0xff;
        txt.b = 0xff;
        txt.a = (uchar)(((uchar)(int)((((float)sin(DAT_0067ed30 * 8.0f) + 1.0f) * 0.33333334f + 0.333f) * ia) * alpha) / 0x100);
        tex = FUN_004391b0(3);
        pos.x = 32.0f;
        pos.y = (top - 20.0f + fy) * (float)FUN_0042b8c0() * 0.0020833334f;
        size2.x = (float)FUN_0042b8b0() * 0.05f;
        size2.y = (float)FUN_0042b8c0() * 0.06666667f;
        if (tex != 0) {
          FUN_00473870(tex, pos, size2, txt, 1);
        }
      }
      if (m == 3 && yy > 0xc6) {
        p = ((float)sin(DAT_0067ed30 * 8.0f) + 1.0f) * 12.0f;
        if (i != DAT_0067f17c) {
          p = 0.0f;
        }
        txt.r = 0xff;
        txt.g = 0xff;
        txt.b = 0xff;
        tex = FUN_0042ee40(i + 0x3de);
        fy = (top - 12.0f + fy) * (float)FUN_0042b8c0() * 0.0020833334f;
        pos.x = 36.0f - p * 0.5f;
        pos.y = fy - p * 0.5f;
        size2.x = (float)FUN_0042b8b0() * (p + 20.0f) * 0.0015625f;
        size2.y = (float)FUN_0042b8c0() * (p + 20.0f) * 0.0020833334f;
        if (tex != 0) {
          FUN_00473870(tex, pos, size2, txt, 1);
        }
      }
    }
  }
  ssz.x = (float)FUN_0042b8b0() * 0.034375f;
  ssz.y = (float)FUN_0042b8c0() * 0.045833334f;
  shade.r = 0;
  shade.g = 0;
  shade.b = 0;
  shade.a = a;
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = a;
  which = 0;
  if (DAT_0067f184 == FUN_00430a60()) {
    which = 1;
  }
  if (DAT_0067f184 == FUN_00430ab0()) {
    which = 2;
  }
  for (i = 0, yy = 0; yy < 0x11e; yy += 0x16, i++) {
    if (yy >= 0xdc && FUN_0042ebe0(i) == 0) {
      continue;
    }
    kind = 2;
    if (yy >= 0xdc) {
      if (FUN_0042ebe0(i) == 0) {
        kind = 0;
      }
    } else if (yy > 0 && FUN_00436810(i) == 0) {
      kind = 1;
    }
    t = (int)((float)yy + top);
    alpha = 0x100;
    if (t < 0x76) {
      d = 0x76 - t;
      if (d > 0x16) {
        d = 0x16;
      }
      alpha = ((0x16 - d) * 0x100) / 0x16;
    }
    if (t > 0xfa) {
      d = t - 0xfa;
      if (d > 0x16) {
        d = 0x16;
      }
      alpha = ((0x16 - d) * 0x100) / 0x16;
    }
    shade.r = 0;
    shade.g = 0;
    shade.b = 0;
    if (DAT_0067e9fc < 2 || DAT_0067e9fc > 10) {
      continue;
    }
    rec = FUN_00436810(i) != 0;
    sa = shade.a >> 2;
    if (sa > shade.a) {
      sa = shade.a;
    }
    if (!rec && sa > 0x20) {
      sa = 0x20;
    }
    rowy = (float)i * 22.0f + y0;
    shade.a = (uchar)((sa * alpha) / 0x100);
    hw = ssz.x * 0.5f;
    hh = ssz.y * 0.5f;
    dsz.x = ssz.x + ssz.x;
    dsz.y = ssz.y + ssz.y;
    for (j = 0; j < 2; j++) {
      spos.x = (float)((FUN_0042b8b0() * 0xdc) / 0x280);
      spos.y = (float)FUN_0042b8c0() * rowy * 0.0020833334f;
      pos.x = spos.x - hw;
      pos.y = spos.y - hh;
      size = dsz;
      bsize.x = dsz.x + (float)FUN_0042b8b0() * pulse * 0.0015625f;
      bsize.y = dsz.y + (float)FUN_0042b8c0() * pulse * 0.0020833334f;
      bpos.x = pos.x - (float)FUN_0042b8b0() * pulse * 0.00078125f;
      bpos.y = pos.y - (float)FUN_0042b8c0() * pulse * 0.0010416667f;
      if (j == 0 ? which != 0 : which == 0) {
        v = FUN_00430a10();
        if (i == DAT_0067f17c) {
          tex = FUN_0042ee40(0);
          if (DAT_0067f184 == v) {
            FUN_00473870(tex, bpos, bsize, white, 1);
          } else {
            FUN_00473870(tex, pos, size, white, 1);
          }
          if ((FUN_0042ef40(i, v + 1000) != 0 || DAT_007f0a40[i].f[v] != 1) && (tex = FUN_004391b0(v)) != 0) {
            FUN_00473870(tex, spos, dsz, white, 1);
          }
        } else if (kind != 1) {
          FUN_00473870(FUN_0042ee00(DAT_007f0a40[i].f[v]), spos, ssz, shade, 1);
        }
      }
      spos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      pos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      bpos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      if (j == 0 ? which != 1 : which == 1) {
        v = FUN_00430a60();
        if (v != 0) {
          if (i == DAT_0067f17c) {
            tex = FUN_0042ee40(1);
            if (DAT_007f0a40[i].f[v] == 0) {
              white.a >>= 1;
            }
            if (DAT_0067f184 == v) {
              FUN_00473870(tex, bpos, bsize, white, 1);
            } else {
              FUN_00473870(tex, pos, size, white, 1);
            }
            rec = DAT_007f0a40[i].f[v];
            if (rec == 0) {
              white.a <<= 1;
            }
            if ((FUN_0042ef40(i, v + 1000) != 0 || rec != 1) && (tex = FUN_004391b0(v)) != 0) {
              FUN_00473870(tex, spos, dsz, white, 1);
            }
          } else if (kind != 1) {
            FUN_00473870(FUN_0042ee00(DAT_007f0a40[i].f[v]), spos, ssz, shade, 1);
          }
        }
      }
      pos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      bpos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      spos.x += (float)((FUN_0042b8b0() * 0x1e) / 0x280);
      if (j == 0 ? which != 2 : which == 2) {
        v = FUN_00430ab0();
        if (v != 0) {
          if (i == DAT_0067f17c) {
            tex = FUN_0042ee40(2);
            if (DAT_007f0a40[i].f[v] == 0) {
              white.a >>= 1;
            }
            if (DAT_0067f184 == v) {
              FUN_00473870(tex, bpos, bsize, white, 1);
            } else {
              FUN_00473870(tex, pos, size, white, 1);
            }
            rec = DAT_007f0a40[i].f[v];
            if (rec == 0) {
              white.a <<= 1;
            }
            if ((FUN_0042ef40(i, v + 1000) != 0 || rec != 1) && (tex = FUN_004391b0(v)) != 0) {
              FUN_00473870(tex, spos, dsz, white, 1);
            }
          } else if (kind != 1) {
            FUN_00473870(FUN_0042ee00(DAT_007f0a40[i].f[v]), spos, ssz, shade, 1);
          }
        }
      }
    }
  }
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
