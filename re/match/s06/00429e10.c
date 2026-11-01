// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429e10 (h10, by hand): 100% exact (regs 100%, ops 100%), 455/455 ins.
   Raw converter 33.8%. What moved it: RGBA/V2d struct locals (FUN_00472c60 takes pos {x,y} and size
   {w,h} by value; FUN_004c1c80 takes a V2d*; the font object's 8 bytes at +0x68 are saved/restored);
   FUN_0040dc80 takes no args (the pushes are FUN_00427e00's); `FUN_00417740(0) + 1 == 0` (inc eax);
   `if (DAT_008991b4 != 0.0f) ... else if` order; plain `(DAT_008991b8 & 0x3f) < 0x34` (cmp dl).
   Stack layout (81 -> 100%): MSVC orders/packs locals by use, so the frame only fits when the
   0x8991b0 block reuses `colour` (the 0xffffffff local passed by address to FUN_00429660) for its
   first {0,0,0,255} colour, rewrites it to 0xff with a right-to-left chain (a,b,g,r store order),
   and copies it into a non-address-taken local `colour2` that stays in ebx. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } S06_RGBA;
typedef struct { float x; float y; } S06_V2d;
typedef int (*S06_StateSetFn)(int, void *);
typedef struct { char pad[0x20]; S06_StateSetFn fpRenderStateSet; } S06_RwGlobals;
MGLOBAL(S06_RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007f0fcc, 0x007f0fcc);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_008991b0, 0x008991b0);
MGLOBAL(float, DAT_008991b4, 0x008991b4);
MGLOBAL(int, DAT_008991b8, 0x008991b8);
MGLOBAL(int, DAT_007f1014, 0x007f1014);
int FUN_00405890(void);
float FUN_0040dc80(void);
int FUN_0040e350(void);
int FUN_0040e4a0(void);
float FUN_00417730(int p0);
int FUN_00417740(int p0);
int FUN_0041e140(void);
void FUN_00427e00(int id, float x, float y, S06_RGBA color, float scale, int align);
void FUN_004295a0(void);
void FUN_00429660(S06_RGBA *color, int p1);
void FUN_00429bd0(int p0);
int FUN_0042f6a0(void);
int FUN_00443080(void);
int FUN_004671a0(int p0);
void FUN_00472c60(S06_V2d pos, S06_V2d size, S06_RGBA color);
void FUN_004c19f0(int font);
void FUN_004c1a00(int font);
void FUN_004c1bb0(int font, S06_RGBA *color, int p2);
void FUN_004c1c80(int font, S06_V2d *size);
// MATCH: 0x00429e10 FUN_00429e10
void FUN_00429e10(void)
{
  S06_RGBA colour;
  S06_RGBA clear;
  S06_RGBA black;
  S06_RGBA red;
  S06_V2d scale;
  S06_V2d saved;
  int font;
  int flag;

  clear.r = 0; clear.g = 0; clear.b = 0; clear.a = 0;
  black.r = 0; black.g = 0; black.b = 0; black.a = 0xff;
  red.r = 0xff; red.g = 0x40; red.b = 0x40; red.a = 0xff;
  colour.r = 0xff; colour.g = 0xff; colour.b = 0xff; colour.a = 0xff;
  font = FUN_004671a0(0);
  if (FUN_0042f6a0() == 2 && FUN_0040e350() == 7 && FUN_0041e140() == 0) {
    FUN_004c19f0(font);
    saved = *(S06_V2d *)(font + 0x68);
    scale.x = 0.8f;
    scale.y = 0.8f;
    FUN_004c1c80(font, &scale);
    FUN_004c1bb0(FUN_004671a0(0), &clear, 2);
    FUN_004c1a00(font);
    DAT_007d3ff8->fpRenderStateSet(6, 0);
    FUN_00427e00(0x251, 323.0f, 93.0f, black, 1.0f, 2);
    FUN_00427e00(0x251, 320.0f, 90.0f, red, 1.0f, 2);
    FUN_00427e00(0x252, 323.0f, 123.0f, black, 1.0f, 2);
    FUN_00427e00(0x252, 320.0f, 120.0f, red, 1.0f, 2);
    FUN_004c19f0(font);
    FUN_004c1c80(font, &saved);
    FUN_004c1a00(font);
  }
  if (FUN_00443080() == 1) {
    if (FUN_0040e350() == 0xb) {
      if ((FUN_0042f6a0() == 3 || FUN_0042f6a0() == 4 || FUN_0042f6a0() == 5) && DAT_007f0fcc != 0) {
        flag = 1;
      } else {
        flag = 0;
      }
      FUN_004c19f0(font);
      saved = *(S06_V2d *)(font + 0x68);
      scale.x = 0.8f;
      scale.y = 0.8f;
      FUN_004c1c80(font, &scale);
      FUN_004c1bb0(FUN_004671a0(0), &clear, 2);
      FUN_004c1a00(font);
      DAT_007d3ff8->fpRenderStateSet(6, 0);
      switch (DAT_007f0fd0) {
      case 5:
        if (FUN_00405890() == 0) {
          FUN_004295a0();
          break;
        }
        FUN_00429bd0(flag);
        FUN_00429660(&colour, 1);
        break;
      case 8:
        if (FUN_00417740(0) + 1 == 0) {
          FUN_004295a0();
          break;
        }
        FUN_00429bd0(flag);
        FUN_00429660(&colour, 1);
        break;
      case 9:
        if (DAT_007f0fcc == 0) {
          FUN_004295a0();
          if (FUN_00417730(1) - FUN_00417730(0) >= 0.9f) {
            FUN_00427e00(0x211, FUN_0040dc80() + 3.0f, 98.0f, black, 1.0f, 2);
            FUN_00427e00(0x211, FUN_0040dc80(), 95.0f, red, 1.0f, 2);
          }
        } else {
          FUN_00429bd0(flag);
          FUN_00429660(&colour, 1);
          FUN_00427e00(0x210, 320.0f, 98.0f, black, 1.0f, 2);
          FUN_00427e00(0x210, 320.0f, 95.0f, colour, 1.0f, 2);
        }
        break;
      case 10:
        if (DAT_007f0fcc == 0) {
          FUN_004295a0();
          FUN_00427e00(0x22c, FUN_0040dc80() + 3.0f, 98.0f, black, 1.0f, 2);
          FUN_00427e00(0x22c, FUN_0040dc80(), 95.0f, red, 1.0f, 2);
          break;
        }
        FUN_00429bd0(flag);
        FUN_00429660(&colour, 1);
        break;
      default:
        FUN_00429bd0(flag);
        FUN_00429660(&colour, flag);
        break;
      }
      FUN_004c19f0(font);
      FUN_004c1c80(font, &saved);
      FUN_004c1a00(font);
    }
  } else if (DAT_008991b0 != 0) {
    S06_RGBA colour2;
    S06_V2d pos;
    S06_V2d size;
    FUN_004c19f0(font);
    saved = *(S06_V2d *)(font + 0x68);
    scale.x = 0.8f;
    scale.y = 0.8f;
    FUN_004c1c80(font, &scale);
    FUN_004c1bb0(FUN_004671a0(0), &clear, 2);
    FUN_004c1a00(font);
    colour.r = 0; colour.g = 0; colour.b = 0; colour.a = 0xff;
    pos.x = 0.0f;
    pos.y = DAT_008991b4;
    size.x = 640.0f;
    size.y = 80.0f;
    FUN_00472c60(pos, size, colour);
    pos.y = 480.0f - DAT_008991b4 - 64.0f;
    FUN_00472c60(pos, size, colour);
    colour.r = colour.g = colour.b = colour.a = 0xff;
    colour2 = colour;
    size.y = 1.0f;
    FUN_00472c60(pos, size, colour2);
    pos.y = DAT_008991b4 + 80.0f;
    FUN_00472c60(pos, size, colour2);
    DAT_007d3ff8->fpRenderStateSet(6, 0);
    if (FUN_0040e4a0() >= 200) {
      FUN_00427e00(0x2d, 64.0f, 480.0f - DAT_008991b4 - 52.0f, colour2, 0.6f, 0);
    }
    FUN_00427e00(0x41, 64.0f, DAT_008991b4 + 50.0f, colour2, 0.8f, 0);
    FUN_00427e00(DAT_008991b0, 64.0f, DAT_008991b4 + 68.0f, colour2, 0.6f, 0);
    if (DAT_008991b4 != 0.0f) {
      DAT_008991b8 = 0;
    } else if (DAT_007f1014 != 0 && (DAT_008991b8 & 0x3f) < 0x34) {
      FUN_00427e00(0xd1, 320.0f, 400.0f, colour2, 1.0f, 2);
    }
    DAT_007d3ff8->fpRenderStateSet(6, (void *)1);
    FUN_004c19f0(font);
    FUN_004c1c80(font, &saved);
    FUN_004c1a00(font);
  }
}
