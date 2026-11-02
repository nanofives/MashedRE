// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429bd0 (h10, by hand): best 99.4% exact (regs 99.4%, ops 99.4%), 167/167 ins.
   Raw converter 32.9%. What moved it: FUN_00472c60 takes two 8-byte float pairs by value
   (pos {x,y}, size {w,h}) plus an RGBA by value -> the float-constant spill/reload pattern and the 0x14
   frame; FUN_00427e00 / FUN_0040dc80 / FUN_00429620 arities (0x40dc80 and 0x429620 take no args, the
   else-branch pushes are 0x427e00's). Colour init order white, black.a, white2, black.rgb fixes the
   byte-store schedule (98.8 -> 99.4). The .rdata floats are written as literals (3.0 double at
   0x5cd6e0, 480/64/80/52/50/68/3.0f): as MGLOBAL externs the compiler swaps the fld/fadd operands
   (relocheck BAD), as literals relocheck resolves them by value.
   Residual (1 ins): before the second render-state call (0x00429d1a) the original pushes 0 and 6 and
   then loads [0x7d3ff8] into edx; ours loads edx between the two pushes (relocheck reports only this
   misaligned slot, +0x14e). Tried: int-cast fn pointer, struct-member fn pointer, typed RwGlobals*
   global, enum state arg, (void *)0, void/uchar return types, inner-scope pos/size, colour-copy form,
   permuter (253 compiles, no improvement). Class (b), budget. */
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
MGLOBAL(float, DAT_008991b4, 0x008991b4);
MGLOBAL(int, DAT_008991b0, 0x008991b0);
void FUN_00401340(void);
void FUN_004013f0(void);
float FUN_0040dc80(void);
void FUN_00427e00(int id, float x, float y, S06_RGBA color, float scale, int align);
float FUN_00429620(void);
void FUN_00429b70(void);
void FUN_00472c60(S06_V2d pos, S06_V2d size, S06_RGBA color);
// MATCH: 0x00429bd0 FUN_00429bd0
void FUN_00429bd0(int param_1)
{
  S06_RGBA black;
  S06_RGBA white;
  S06_RGBA white2;
  float f;
  S06_V2d pos;
  S06_V2d size;

  white.r = 0xff; white.g = 0xff; white.b = 0xff; white.a = 0xff;
  black.a = 0xff;
  white2.r = 0xff; white2.g = 0xff; white2.b = 0xff; white2.a = 0xff;
  black.r = 0; black.g = 0; black.b = 0;
  if (param_1 != 0) {
    f = FUN_00429620();
    DAT_007d3ff8->fpRenderStateSet(8, 0);
    FUN_00427e00(0xe4, (float)(f + 3.0), 373.0f, black, 1.2f, 1);
    FUN_00427e00(0xe4, f, 370.0f, white, 1.2f, 1);
    FUN_00429b70();
    pos.x = 0.0f;
    pos.y = DAT_008991b4;
    size.x = 640.0f;
    size.y = 80.0f;
    FUN_00472c60(pos, size, black);
    pos.y = 480.0f - DAT_008991b4 - 64.0f;
    FUN_00472c60(pos, size, black);
    size.y = 1.0f;
    FUN_00472c60(pos, size, white2);
    pos.y = DAT_008991b4 + 80.0f;
    FUN_00472c60(pos, size, white2);
    DAT_007d3ff8->fpRenderStateSet(6, 0);
    FUN_00427e00(0x2d, 64.0f, 480.0f - DAT_008991b4 - 52.0f, white2, 0.6f, 0);
    FUN_00427e00(0x41, 64.0f, DAT_008991b4 + 50.0f, white2, 0.8f, 0);
    FUN_00427e00(DAT_008991b0, 64.0f, DAT_008991b4 + 68.0f, white2, 0.6f, 0);
    FUN_00401340();
    FUN_004013f0();
    return;
  }
  FUN_00427e00(0xe4, FUN_0040dc80() + 3.0f, 63.0f, black, 1.2f, 2);
  FUN_00427e00(0xe4, FUN_0040dc80(), 60.0f, white, 1.2f, 2);
}
