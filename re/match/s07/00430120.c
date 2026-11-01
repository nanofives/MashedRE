// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00430120: 100% first hand pass (raw 49.4%). The colour is an RwRGBA-shaped 4-byte struct by value
   {0, 0, 0, (byte)DAT_0067e81c}; the float args are literals (0x43a00000 = 320.0f, 330/350/370/390, 0x3f19999a =
   0.6f); the +0x20 slot of the record DAT_007d3ff8 points at is called cdecl with (state, value). The records at
   0x0067ed40 are 0x40 bytes (shl 6); the array length 4 is not taken from the binary. */
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
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } Rgba00430120;
typedef int (*RenderStateSetFn00430120)(int state, int value);
typedef struct { int mode; int f4; int f8; int fc; int f10[12]; } Cfg00430120;
MGLOBAL(int, DAT_0067e818, 0x0067e818);
MGLOBAL(int, DAT_0067e81c, 0x0067e81c);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_007f0fe8, 0x007f0fe8);
MGLOBALA(Cfg00430120, DAT_0067ed40, 4, 0x0067ed40);
void FUN_00427e00(int id, float x, float y, Rgba00430120 c, float scale, int n);
// MATCH: 0x00430120 FUN_00430120
void FUN_00430120(void)
{
  Rgba00430120 c;
  if (DAT_0067e818 == 2) {
    return;
  }
  if (DAT_0067e818 == 0 && DAT_0067e81c < 0x60) {
    return;
  }
  (*(RenderStateSetFn00430120 *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(RenderStateSetFn00430120 *)(DAT_007d3ff8 + 0x20))(8, 0);
  c.r = 0;
  c.g = 0;
  c.b = 0;
  c.a = (unsigned char)DAT_0067e81c;
  if (DAT_0067ed40[DAT_0067e9f8].mode == 1) {
    FUN_00427e00(0x245, 320.0f, 330.0f, c, 0.6f, 2);
    FUN_00427e00(0x244, 320.0f, 350.0f, c, 0.6f, 2);
    FUN_00427e00(0x277, 320.0f, 370.0f, c, 0.6f, 2);
    FUN_00427e00(0x278, 320.0f, 390.0f, c, 0.6f, 2);
  } else {
    FUN_00427e00(0x247, 320.0f, 330.0f, c, 0.6f, 2);
  }
  (*(RenderStateSetFn00430120 *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(RenderStateSetFn00430120 *)(DAT_007d3ff8 + 0x20))(8, 1);
  DAT_007f0fe8 = 0;
}
