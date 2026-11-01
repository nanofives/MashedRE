// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004335f0 (2376 B): best 64.8% (regs 75.3%, ops 92.4%, 654/668 ins), by hand from the raw 25.1%
   (to_s08.py had stopped on "global NAN": that was the record id -1 compare, now an int field).
   Shape: 12 records {int id; float x; float y} at 0x0067eaf0 (index loop, strength-reduced to the pointer the
   original compares against 0x0067eb40), 7 RGBA pal pointers, {x,y} structs by value to 0x00472c60/0x004739f0,
   the 0x0042f8d0 frame body written out here with its struct copy (osz = isz), FUN_0042bde0(int, uchar),
   sin intrinsic, tex fetched before the 0x004739f0 pushes, `if (id != 0)` with the id==0 case last, and two
   FUN_0042bcb0 calls per flag value as the original.
   Residual: stack slot layout and register assignment (alpha byte at B+0x20 in memory in the original), plus the
   id==0 branch: the original indexes pal[id] there, this compiler forwards pal[0] = &cE0. Permuter: 1896 compiles,
   best 71.4% but its second move sets c.a = a before the first frame draw (behaviour change, rejected); its
   first move (isz.y store order) is kept. Class (b). */
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
typedef struct { int id; int grp; int icon; int pad; } MatchSlot;
typedef struct { int id; float x; float y; } MatchRec;
typedef int (*MatchRenderStateSet)(int state, int value);
MGLOBAL(uchar, DAT_0067e7ac, 0x0067e7ac);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_008990dc, 0x008990dc);
MGLOBAL(float, DAT_0067ed14, 0x0067ed14);
MGLOBAL(float, DAT_0067f0c8, 0x0067f0c8);
MGLOBAL(float, DAT_0067f0c4, 0x0067f0c4);
MGLOBAL(int, DAT_0067f0c0, 0x0067f0c0);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBALA(MatchSlot, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(MatchRec, DAT_0067eaf0, 12, 0x0067eaf0);
#pragma intrinsic(sin)
double sin(double);
int FUN_00430760(void);
void FUN_00427ad0(int id, float x, float y, float w, float h, MatchRGBA c, float scale);
void FUN_00427e00(int id, float x, float y, MatchRGBA c, float scale, int align);
void FUN_00427f00(char *s, float x, float y, MatchRGBA c, float scale, int align);
float FUN_004282a0(int id, float scale);
void FUN_0042bcb0(int id, float x, float y, MatchRGBA c, int flag);
void FUN_0042bde0(int v, uchar a);
void *FUN_0042fab0(int id);
void FUN_00472c60(MatchV2 pos, MatchV2 size, MatchRGBA c);
void FUN_004739f0(void *tex, MatchV2 pos, MatchV2 size, MatchRGBA c, float u0, float v0, float u1, float v1,
                  int p10, int p11);

// MATCH: 0x004335f0 FUN_004335f0
void FUN_004335f0(void)
{
  MatchRGBA c;
  MatchRGBA cFF;
  MatchRGBA cD7;
  uchar a;
  MatchRGBA cE0;
  MatchRGBA c28;
  MatchRGBA c2c;
  MatchRGBA c30;
  MatchRGBA c34;
  MatchRGBA c38;
  MatchRGBA c3c;
  int n;
  int first;
  float pulse;
  MatchV2 ipos;
  MatchV2 isz;
  MatchV2 rsz;
  MatchV2 rpos;
  MatchRGBA *pal[7];
  char buf[4];
  float sum;
  MatchV2 osz;
  int i;
  int j;
  int k;
  int id;
  int dup;
  char d;
  void *tex;

  cD7.r = 0xd7;
  cD7.g = 0xd7;
  cD7.b = 0xd7;
  cE0.r = 0xe0;
  cE0.g = 0xe0;
  cE0.b = 0xe0;
  c2c.g = 0xa7;
  c2c.b = 0xa7;
  pal[0] = &cE0;
  pal[3] = &c30;
  pal[6] = &c28;
  pal[1] = &c38;
  cFF.r = 0xff;
  cFF.g = 0xff;
  cFF.b = 0xff;
  cFF.a = 0xff;
  pal[2] = &c34;
  pal[4] = &c3c;
  pal[5] = &c2c;
  c3c.r = 0xdb;
  c3c.g = 0xc3;
  c3c.b = 0x62;
  c38.r = 0x98;
  c38.g = 0x3a;
  c38.b = 0x3d;
  c34.r = 0x4e;
  c34.g = 0x89;
  c34.b = 0xae;
  c30.r = 0x61;
  c30.g = 0x76;
  c30.b = 0x56;
  c28.r = 0;
  c28.g = 0;
  c28.b = 0;
  c2c.r = 0xeb;
  DAT_008990dc = 0;
  a = DAT_0067e7ac;
  cE0.a = a;
  c28.a = a;
  c2c.a = a;
  cD7.a = a;
  c30.a = a;
  c34.a = a;
  c38.a = a;
  c3c.a = a;
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  pulse = (float)sin(DAT_0067ed14 * 7.0f) * 20.0f;
  if (FUN_00430760() != 0) {
    cFF.a = a;
    ipos.x = 146.0f;
    isz.x = 64.0f;
    rpos.x = 143.0f;
    isz.y = 64.0f;
    ipos.y = DAT_0067f0c8 - 41.0f - 40.0f;
    rsz.x = 69.0f;
    rsz.y = 20.0f;
    rpos.y = DAT_0067f0c8 - 38.0f;
    for (i = 0; i < 6; i++) {
      FUN_00472c60(rpos, rsz, *pal[i + 1]);
      tex = FUN_0042fab0(i);
      FUN_004739f0(tex, ipos, isz, cFF, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
      ipos.x += 70.0f;
      rpos.x += 70.0f;
    }
    ipos.y = DAT_0067f0c8 - 13.0f;
    isz.y = 26.0f;
    isz.x = 534.0f;
    osz = isz;
    ipos.x = 48.0f;
    c.r = 0xf0;
    c.g = 0x6e;
    c.b = 0x14;
    c.a = a >> 1;
    FUN_00472c60(ipos, isz, c);
    c.r = 0xb4;
    c.g = 0x50;
    c.b = 0x10;
    c.a = a;
    ipos.x -= 2.0f;
    isz.x = 2.0f;
    FUN_00472c60(ipos, isz, c);
    isz.x = osz.x + 4.0f;
    isz.y = 2.0f;
    FUN_00472c60(ipos, isz, c);
    ipos.y += osz.y - 2.0f;
    FUN_00472c60(ipos, isz, c);
    ipos.x = osz.x + ipos.x + 2.0f;
    isz.x = 2.0f;
    isz.y = osz.y;
    ipos.y -= osz.y - 2.0f;
    FUN_00472c60(ipos, isz, c);
    if (DAT_0067f0c0 == 6) {
      c = cD7;
      FUN_0042bcb0(DAT_007f1a14[0].id, DAT_0067f0c4, DAT_0067f0c8 - 1.0f, c, 1);
    } else {
      c = *pal[DAT_0067f0c0];
      FUN_0042bcb0(DAT_007f1a14[0].id, DAT_0067f0c4, DAT_0067f0c8 - 1.0f, c, 0);
    }
    n = (int)FUN_004282a0(0x36, 0.8f) / 2 + 4;
    buf[0] = DAT_007f1a14[0].id + '1';
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.a = a;
    buf[1] = 0;
    FUN_00427f00(buf, (float)n + DAT_0067f0c4 + 15.0f, DAT_0067f0c8, c, 0.6f, 0);
    (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
    (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
    return;
  }
  c.r = 0;
  c.g = 0;
  c.b = 0;
  c.a = a;
  if (DAT_007f0fe8 != 0) {
    ipos.x = 320.0f;
    ipos.y = 330.0f;
    FUN_00427e00(0x245, ipos.x, ipos.y, c, 0.6f, 2);
    FUN_00427e00(0x244, ipos.x, ipos.y + 20.0f, c, 0.6f, 2);
    FUN_00427e00(0x277, ipos.x, ipos.y + 40.0f, c, 0.6f, 2);
    FUN_00427e00(0x278, ipos.x, ipos.y + 60.0f, c, 0.6f, 2);
  }
  sum = DAT_005d757c;
  for (i = 0; i < 12; i++) {
    if (DAT_0067eaf0[i].id != -1) {
      sum += 1.0f;
    }
  }
  if (sum < 2.0f) {
    ipos.y = 80.0f;
    FUN_00427ad0(0x31, 80.0f, ipos.y, 480.0f, 100.0f, c, 0.7f);
  }
  first = 0;
  for (i = 0; i < 12; i++) {
    if (DAT_0067eaf0[i].id == -1) {
      continue;
    }
    if (first == 0) {
      cFF.a = a;
      ipos.x = 146.0f;
      isz.x = 64.0f;
      rpos.x = 143.0f;
      isz.y = 64.0f;
      ipos.y = DAT_0067eaf0[i].y - 41.0f - 40.0f;
      rsz.x = 69.0f;
      rsz.y = 20.0f;
      rpos.y = DAT_0067eaf0[i].y - 38.0f;
      for (j = 0; j < 6; j++) {
        FUN_00472c60(rpos, rsz, *pal[j + 1]);
        tex = FUN_0042fab0(j);
      FUN_004739f0(tex, ipos, isz, cFF, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        ipos.x += 70.0f;
        rpos.x += 70.0f;
      }
      first = 1;
    }
    FUN_0042bde0((int)DAT_0067eaf0[i].y, a);
    id = DAT_0067eaf0[i].id;
    if (id != 0) {
      dup = 0;
      for (j = 0; j < 12; j++) {
        if (id == DAT_0067eaf0[j].id && id > 0 && j != i) {
          dup = 1;
        }
      }
      if (id == 6) {
        c = cD7;
      } else {
        c = *pal[id];
      }
      if (dup) {
        d = (char)(int)pulse;
        if (id == 6) {
          d <<= 1;
        }
        c.r += d;
        c.g += d;
        c.b += d;
      }
      if (id == 6) {
        FUN_0042bcb0(i, DAT_0067eaf0[i].x, DAT_0067eaf0[i].y - 1.0f, c, 1);
      } else {
        FUN_0042bcb0(i, DAT_0067eaf0[i].x, DAT_0067eaf0[i].y - 1.0f, c, 0);
      }
    } else {
      FUN_0042bcb0(i, DAT_0067eaf0[i].x, DAT_0067eaf0[i].y - 1.0f, *pal[id], 0);
    }
    n = (int)FUN_004282a0(0x36, 0.8f) / 2 + 4;
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.a = a;
    if (DAT_007f0fe8 != 0) {
      k = i >= 6 ? i - 6 : i;
      if (k >= 6) {
        k -= 6;
      }
      buf[0] = k + '1';
      buf[1] = 0;
      FUN_00427f00(buf, (float)n + DAT_0067eaf0[i].x + 15.0f, DAT_0067eaf0[i].y, c, 0.6f, 0);
    } else {
      buf[0] = i + '1';
      buf[1] = 0;
      FUN_00427f00(buf, (float)n + DAT_0067eaf0[i].x + 15.0f, DAT_0067eaf0[i].y, c, 0.6f, 0);
    }
  }
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
