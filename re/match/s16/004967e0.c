// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004967e0 (h3): by hand. The file also holds its register-arg callee 0x00496530 (per-player
   input cook) as a `static` function defined first (pilot finding 1): `static void FUN_00496530(int player)`
   receives the player index in EBX, which is also what moved the caller's loop counter to EBX.
   0x00496530: 100% (regs 100%). Needed: v/unused zeroed at the top (before the memset, so they are stored as
   immediates and get their own slots), dev = &DAT_007e95c0[player] computed before the copy (keeps it live
   across the rep movsd/stosd, so it and cur are spilled as in the original), memcpy/memset intrinsics, an int
   temp for the b[0]/b[1] swap (movzx).
   0x004967e0: best 98.8% (regs 98.8%). One instruction left: the original keeps an `and eax, 1` after
   `shr eax, 7` for 0x00773048 (bit 7 of byte 0x00773058); every spelling tried (mask != 0 macro and
   __inline function, >>7 &1, &0x80 >>7, ?:, uchar/char/uint bitfields, signed char cast, permuter 191
   compiles) folds the AND away. Class (b). DAT_00773000..0x00773054 written as one int[22] (the loop stores
   12, then 10 single bits); 1/255, -127, 127 are the floats at 0x005ceb90, 0x005cc570, 0x005d0008. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
MGLOBAL(int, DAT_00772ffc, 0x00772ffc);
MGLOBALA(int, DAT_00773000, 22, 0x00773000);
MGLOBALA(uchar, DAT_00773058, 32, 0x00773058);
void FUN_00495fe0(void);
void FUN_0045b350(void);
void FUN_004972b0(void);
void FUN_004b6520(void *p, int n);
int FUN_00496100(void *p);
int FUN_00497450(int i);
#define BIT(n) ((DAT_00773058[(n) >> 3] & (1 << ((n) & 7))) != 0)
typedef struct PadState { uchar b[0x14]; float x; float y; uchar c[0x30]; } PadState;
typedef struct PadDev { char pad0[0x13c]; int type; int handle; char pad144[0xbc]; } PadDev;
typedef struct MouseBtn { uchar b[4]; uchar e; } MouseBtn;
typedef struct Vec2f { float x; float y; } Vec2f;
MGLOBALA(PadState, DAT_007f1038, 4, 0x007f1038);
MGLOBALA(PadState, DAT_007f14f8, 4, 0x007f14f8);
MGLOBALA(PadDev, DAT_007e95c0, 4, 0x007e95c0);
MGLOBAL(int, DAT_007f0f30, 0x007f0f30);
MGLOBAL(char, DAT_007f1088, 0x007f1088);
MGLOBAL(MouseBtn, DAT_008989a0, 0x008989a0);
MGLOBAL(MouseBtn, DAT_008989c0, 0x008989c0);
int FUN_004957a0(int handle, int a, Vec2f *v, int b, int *c);
uchar FUN_00497310(int player, int code);
void *memset(void *p, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memset)
#pragma intrinsic(memcpy)

// MATCH: 0x00496530 FUN_00496530
static void FUN_00496530(int player)
{
  PadState *cur;
  PadDev *dev;
  Vec2f v;
  int unused;
  uchar k;
  int t;

  v.x = 0.0f;
  v.y = 0.0f;
  unused = 0;
  dev = &DAT_007e95c0[player];
  cur = &DAT_007f1038[player];
  memcpy(&DAT_007f14f8[player], cur, sizeof(PadState));
  memset(cur, 0, sizeof(PadState));
  if (dev->type == 1) {
    FUN_004957a0(dev->handle, 0, &v, 0, &unused);
  }
  DAT_007f1038[player].b[0xa] = FUN_00497310(player, 4);
  DAT_007f1038[player].b[0xb] = FUN_00497310(player, 5);
  DAT_007f1038[player].b[9] = FUN_00497310(player, 6);
  DAT_007f1038[player].b[0x11] = FUN_00497310(player, 7);
  DAT_007f1038[player].b[4] = FUN_00497310(player, 0);
  DAT_007f1038[player].b[5] = FUN_00497310(player, 1);
  DAT_007f1038[player].b[7] = FUN_00497310(player, 2);
  DAT_007f1038[player].b[8] = FUN_00497310(player, 3);
  DAT_007f1038[player].c[0x23] = FUN_00497310(player, 8);
  DAT_007f1038[player].b[0x13] = DAT_007f1038[player].b[7];
  k = FUN_00497310(player, 9);
  if (k != 0) {
    DAT_007f1038[player].b[0xc] = 0xff;
    DAT_007f1038[player].b[0] = k;
  }
  k = FUN_00497310(player, 10);
  if (k != 0) {
    DAT_007f1038[player].b[0xd] = 0xff;
    DAT_007f1038[player].b[1] = k;
  }
  k = FUN_00497310(player, 11);
  if (k != 0) {
    DAT_007f1038[player].b[0xe] = 0xff;
    DAT_007f1038[player].b[2] = k;
  }
  k = FUN_00497310(player, 12);
  if (k != 0) {
    DAT_007f1038[player].b[0xf] = 0xff;
    DAT_007f1038[player].b[3] = k;
  }
  if (dev->type == 1) {
    DAT_007f1038[player].x = v.x;
    DAT_007f1038[player].y = -v.y;
  } else if (dev->type == 2) {
    DAT_007f1038[player].x = (float)(DAT_007f1038[player].b[1] - DAT_007f1038[player].b[0]) * (1.0f / 255.0f);
    DAT_007f1038[player].y = (float)(DAT_007f1038[player].b[3] - DAT_007f1038[player].b[2]) * (1.0f / 255.0f);
  }
  if (DAT_007f0f30 != 0) {
    t = DAT_007f1038[player].b[0];
    DAT_007f1038[player].b[0] = DAT_007f1038[player].b[1];
    DAT_007f1038[player].b[1] = (uchar)t;
  }
  if (player == 1) {
    DAT_008989a0 = DAT_008989c0;
    DAT_008989c0.e = 0;
    if (DAT_007f1088 != 0) {
      DAT_008989c0.b[1] = 0;
      DAT_008989c0.b[0] = 0;
      DAT_008989c0.b[2] = (uchar)(int)(v.y * -127.0f);
      DAT_008989c0.b[3] = (uchar)(int)(v.x * 127.0f);
    } else {
      DAT_008989c0.b[1] = (uchar)(int)(v.x * 127.0f);
      DAT_008989c0.b[0] = (uchar)(int)(v.y * -127.0f);
      DAT_008989c0.b[2] = 0;
      DAT_008989c0.b[3] = 0;
    }
  }
}

// MATCH: 0x004967e0 FUN_004967e0
void FUN_004967e0(void)
{
  int i;
  FUN_00495fe0();
  FUN_0045b350();
  FUN_004972b0();
  FUN_004b6520(DAT_00773058, 0x20);
  FUN_00496100(DAT_00773058);
  DAT_00772ffc = 0;
  for (i = 0; i < 4; i++) {
    if (FUN_00497450(i) != 0) {
      FUN_00496530(i);
      DAT_00772ffc++;
    }
  }
  {
    int n = 0x3b;
    int *p = DAT_00773000;
    int c = 12;
    do {
      *p = BIT(n);
      n++;
      p++;
    } while (--c != 0);
  }
  DAT_00773000[12] = BIT(11);
  DAT_00773000[13] = BIT(2);
  DAT_00773000[14] = BIT(3);
  DAT_00773000[15] = BIT(4);
  DAT_00773000[16] = BIT(5);
  DAT_00773000[17] = BIT(6);
  DAT_00773000[18] = BIT(7);
  DAT_00773000[19] = BIT(8);
  DAT_00773000[20] = BIT(9);
  DAT_00773000[21] = BIT(10);
}
