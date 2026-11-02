// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043a610 (1050 B): best 35.1% (regs 42.9%, ops 84.0%, 282/282 ins), by hand from the raw 28.0%.
   The file holds the static callee 0x0042f8d0 at 100% (relocheck OK): alpha byte in AL (static-callee
   convention), two {x,y} float structs by value, and a struct copy `osz = size` (that copy is why the original
   reloads ow/oh from memory instead of folding them). Struct-by-value args are what make every float constant a
   store-then-reload in the callers. Loops are index loops over the 0x0067e850 row table (12) and the 0x007f1a14
   slot table (4); the compiler strength-reduces them to the pointers and signed pointer compares seen.
   Residual: register assignment. The original keeps the alpha in a stack byte (B+0x13) and ebx for the
   0xff/0 constants; this compiler puts the alpha in bl. Wrapping it in an RGBA struct, declaration order, and the
   permuter (1266 compiles; its 43.6% variant reorders calls/stores and changes behaviour, rejected) did not move it.
   Class (b). Sibling of 0x0043aa30. */
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
MGLOBAL(uchar, DAT_0067e7dc, 0x0067e7dc);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
MGLOBALA(MatchRow, DAT_0067e850, 12, 0x0067e850);
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

// MATCH: 0x0043a610 FUN_0043a610
void FUN_0043a610(void)
{
  MatchRGBA cA;
  char buf[4];
  int row;
  MatchRGBA black;
  MatchRGBA c68;
  MatchRGBA c64;
  MatchRGBA c60;
  MatchRGBA c5c;
  MatchRGBA c58;
  MatchRGBA c54;
  MatchRGBA white;
  MatchRGBA black2;
  float fy2;
  MatchV2 base;
  MatchV2 pos;
  MatchV2 ipos;
  MatchV2 size;
  MatchV2 isize;
  MatchRGBA *pal[6];
  int j;
  int k;
  void *tex;
  int n;

  cA.a = DAT_0067e7dc;
  c68.r = 0xff;
  c68.g = 0xff;
  c68.b = 0xff;
  c68.a = cA.a;
  c64.r = 0xeb;
  c64.g = 0xa7;
  c64.b = 0xa7;
  c64.a = cA.a;
  c60.r = 0xdb;
  c60.g = 0xc3;
  c60.b = 0x62;
  c60.a = cA.a;
  c5c.r = 0x61;
  c5c.g = 0x76;
  c5c.b = 0x56;
  c5c.a = cA.a;
  c58.r = 0x4e;
  c58.g = 0x89;
  c58.b = 0xae;
  c58.a = cA.a;
  c54.r = 0x98;
  c54.g = 0x3a;
  c54.b = 0x3d;
  c54.a = cA.a;
  black2.a = cA.a;
  pal[0] = &c54;
  pal[1] = &c58;
  pal[2] = &c5c;
  pal[3] = &c60;
  pal[4] = &c64;
  pal[5] = &c68;
  size.y = 28.0f;
  size.x = 120.0f;
  pos.y = 140.0f;
  pos.x = 65.0f;
  FUN_0042f8d0(cA.a, pos, size);
  pos.x = 195.0f;
  FUN_0042f8d0(cA.a, pos, size);
  pos.x = 325.0f;
  FUN_0042f8d0(cA.a, pos, size);
  pos.x = 455.0f;
  FUN_0042f8d0(cA.a, pos, size);
  pos.x = 60.0f;
  pos.y = 200.0f;
  size.x = 523.0f;
  isize.x = 48.0f;
  isize.y = 48.0f;
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = cA.a;
  black.r = 0;
  black.g = 0;
  black.b = 0;
  black.a = cA.a;
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  for (row = 0; row < 12; row++) {
    for (j = 0; j < 4; j++) {
      if (DAT_007f1a14[j].id == row) {
        FUN_0042f8d0(cA.a, pos, size);
        tex = FUN_0042fab0(DAT_007f1a14[j].icon);
        base.x = (float)(DAT_0067e850[row].col * 0x82) + 64.0f;
        ipos.x = base.x;
        base.y = pos.y - 2.0f;
        fy2 = base.y + 15.0f;
        FUN_0042bcb0(DAT_007f1a14[j].id, ipos.x + 65.0f, fy2, *pal[DAT_007f1a14[j].icon],
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
        FUN_00427f00(buf, (float)n + ipos.x + 65.0f, fy2, black, 0.6f, 0);
        ipos.x = base.x;
        ipos.y = base.y - 8.0f;
        FUN_004739f0(tex, ipos, isize, white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        pos.y += 40.0f;
      }
    }
  }
  black2.r = 0;
  black2.g = 0;
  black2.b = 0;
  FUN_00427e00(0xe3, 125.0f, 156.0f, black2, 0.75f, 2);
  FUN_00427e00(0xd2, 255.0f, 156.0f, black2, 0.75f, 2);
  FUN_00427e00(0xd3, 385.0f, 156.0f, black2, 0.75f, 2);
  FUN_00427e00(0xd4, 515.0f, 156.0f, black2, 0.75f, 2);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
