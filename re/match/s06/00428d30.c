// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428d30 (1292 B, h8, by hand): 100.0% exact (regs 100, ops 100), 350/350 ins. Raw g2s 37.7%.
   What moved it: FUN_00472c60 / 00472f40 / 004730b0 / 00473220 / 004744a0 take S06_V2d pos and size by value
   plus RGBA(s) by value (the constant spill/reload pattern); FUN_0042f0b0 takes no args (the pushes are
   FUN_00427e00's); FUN_004744a0's tail is (int 0, double 640.0, double 480.0, int 0, int 0xff).
   Then the source writes exactly the stores the original does (frame map F+0x08 c0, +0x0c c1, +0x10 colour,
   +0x18 pos, +0x20 size, +0x28 scale): only the changed pos/size fields between draw calls, plus the
   original's redundant re-stores (pos.x / size.y before 0x004730b0, pos.x / size.x before the first white
   line), c0/c1 reused for the later colours, chain assignments for the b,g,r store order
   (`c.r = c.g = c.b = 0`) and a,b,g,r (`c.r = c.g = c.b = c.a = 0xff`). With all-four-fields-per-call and
   separate colour locals the compiler cached 0 in EBX instead of 0xff (67.7%).
   0x005cd694 (.rdata, 1/60) is written as the literal 0.016666668f. Residual: none. */
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
MGLOBAL(int, DAT_00771964, 0x00771964);
MGLOBAL(float, DAT_007f1010, 0x007f1010);
void FUN_00401ee0(int p0);
int FUN_0040d250(void);
void FUN_00427e00(int id, float x, float y, S06_RGBA color, float scale, int align);
void FUN_00428450(int p0, int p1);
void FUN_00428760(int h, float x, float y, float w, float hgt, int p5);
int FUN_0042e590(int p0);
int FUN_0042f0b0(void);
int FUN_004671a0(int p0);
void FUN_00472c60(S06_V2d pos, S06_V2d size, S06_RGBA color);
void FUN_00472f40(S06_V2d pos, S06_V2d size, S06_RGBA color);
void FUN_004730b0(S06_V2d pos, S06_V2d size, S06_RGBA color);
void FUN_00473220(S06_V2d pos, S06_V2d size, S06_RGBA c0, S06_RGBA c1);
void FUN_004744a0(S06_V2d pos, S06_V2d size, int p4, double p5, double p6, int p7, int p8);
void FUN_004c19f0(int font);
int FUN_004c1a00(int font);
void FUN_004c1bb0(int font, S06_RGBA *color, int p2);
void FUN_004c1be0(int font, int p1, int p2);
void FUN_004c1c80(int font, S06_V2d *size);

// MATCH: 0x00428d30 FUN_00428d30
void FUN_00428d30(void)
{
  S06_RGBA colour;
  S06_RGBA c0;
  S06_RGBA c1;
  S06_V2d scale;
  S06_V2d pos;
  S06_V2d size;
  int h0;
  int h1;

  colour.r = 0;
  colour.g = 0;
  colour.b = 0;
  colour.a = 0xff;
  h0 = FUN_0042e590(0);
  h1 = FUN_0042e590(1);
  scale.x = 0.8f;
  scale.y = 0.8f;
  FUN_004c1c80(FUN_004671a0(0), &scale);
  FUN_004c1bb0(FUN_004671a0(0), &colour, 3);
  if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
    DAT_007d3ff8->fpRenderStateSet(0x14, (void *)1);
    DAT_007d3ff8->fpRenderStateSet(8, (void *)0);
    DAT_007d3ff8->fpRenderStateSet(6, (void *)0);
    FUN_00428760(h0, 0.0f, 64.0f, 352.0f, 352.0f, 0);
    DAT_007d3ff8->fpRenderStateSet(8, (void *)0);
    DAT_007d3ff8->fpRenderStateSet(6, (void *)0);
    c0.r = c0.g = c0.b = 0;
    c0.a = 0x80;
    c1.r = c1.g = c1.b = 0;
    c1.a = 0xc0;
    pos.x = 0.0f;
    pos.y = 0.0f;
    size.x = 256.0f;
    size.y = 480.0f;
    FUN_00473220(pos, size, c0, c1);
    c0.a = 0xc0;
    c1.a = 0xff;
    pos.x = 256.0f;
    size.x = 96.0f;
    FUN_00473220(pos, size, c0, c1);
    FUN_00401ee0(FUN_0040d250());
    if (DAT_00771964 != 0)
      FUN_00428760(DAT_00771964, 64.0f, 60.0f, 200.0f, 100.0f, 0);
    pos.x = 0.0f;
    pos.y = 0.0f;
    size.x = 512.0f;
    size.y = 512.0f;
    FUN_004744a0(pos, size, 0, 640.0, 480.0, 0, 0xff);
    c0.r = 0;
    c0.g = 0;
    c0.b = 0;
    c0.a = 0xa0;
    pos.x = 0.0f;
    pos.y = 0.0f;
    size.x = 640.0f;
    size.y = 64.0f;
    FUN_00472f40(pos, size, c0);
    pos.x = 0.0f;
    pos.y = 416.0f;
    size.y = 64.0f;
    FUN_004730b0(pos, size, c0);
    c0.r = c0.g = c0.b = 0xff;
    c0.a = 0xff;
    pos.x = 0.0f;
    size.x = 640.0f;
    size.y = 1.0f;
    FUN_00472c60(pos, size, c0);
    pos.y = 64.0f;
    FUN_00472c60(pos, size, c0);
    pos.x = 576.0f;
    pos.y = 0.0f;
    size.x = 1.0f;
    size.y = 480.0f;
    FUN_00472c60(pos, size, c0);
    pos.x = 320.0f;
    pos.y = 64.0f;
    size.y = 256.0f;
    FUN_00472c60(pos, size, c0);
    pos.y = 320.0f;
    size.x = 256.0f;
    size.y = 1.0f;
    FUN_00472c60(pos, size, c0);
    pos.x = 0.0f;
    pos.y = 152.0f;
    size.x = 640.0f;
    FUN_00472c60(pos, size, c0);
    pos.x = 480.0f;
    pos.y = 0.0f;
    size.x = 1.0f;
    size.y = 480.0f;
    FUN_00472c60(pos, size, c0);
    FUN_00428450(0x10, 0x100);
    FUN_00428760(h1, 320.0f, 65.0f, 256.0f, 255.0f, 0);
    pos.x = 320.0f;
    pos.y = 0.0f;
    size.x = 1.0f;
    size.y = 320.0f;
    FUN_00472c60(pos, size, c0);
    c1.r = c1.g = c1.b = c1.a = 0xff;
    FUN_00427e00(FUN_0042f0b0(), 310.0f, 168.0f, c1, 1.0f, 1);
    DAT_007f1010 = DAT_007f1010 + 0.016666668f;
    FUN_00427e00(0x222, 450.0f, 370.0f, c0, 1.0f, 1);
    DAT_007d3ff8->fpRenderStateSet(6, (void *)1);
    DAT_007d3ff8->fpRenderStateSet(8, (void *)1);
    DAT_007d3ff8->fpRenderStateSet(0x14, (void *)2);
    FUN_004c19f0(FUN_004671a0(0));
  }
  FUN_004c1be0(FUN_004671a0(0), 0, 1);
}
