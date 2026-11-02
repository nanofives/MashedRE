// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004492b0: 100% (regs 100%) by hand (h9). Raw 86.7%. The deferred-pop residual was a callee arity:
   FUN_00426060 takes no argument (mov eax, [0x0065742c]; ret) and FUN_004e4320 / FUN_004e4350 take two
   (world, camera; 0x004e4320 reads [esp + 8]), so the push eax before call 0x426060 is 4e4320's second arg.
   Local RGBA {0x50, 0x58, 0x60, 0xff}; the render-state calls go through the slot at RwGlobals (0x007d3ff8) + 0x20. */
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
typedef struct { unsigned char r, g, b, a; } MatchRGBA_004492b0;
typedef int (*MatchRenderStateSet_004492b0)(int state, int value);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
int FUN_00426060(void);
void FUN_004491e0(int p0);
void FUN_00451060(int p0);
int FUN_004671a0(int p0);
int FUN_004840d0(unsigned int p0);
void FUN_00489240(void);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
int FUN_004c1bb0(int p0, MatchRGBA_004492b0 *p1, int p2);
int FUN_004c1c80(int p0, int p1);
int FUN_004e4320(int p0, int p1);
int FUN_004e4350(int p0, int p1);
int FUN_004e6680(int p0);
#define RENDERSTATE_004492b0 (*(MatchRenderStateSet_004492b0 *)(DAT_007d3ff8 + 0x20))
// MATCH: 0x004492b0 FUN_004492b0
void FUN_004492b0(int param_1)
{
  MatchRGBA_004492b0 col;

  col.r = 0x50;
  col.g = 0x58;
  col.b = 0x60;
  col.a = 0xff;
  FUN_004c1c80(FUN_004840d0(0), FUN_004671a0(0) + 0x68);
  FUN_00451060(0);
  FUN_004c1bb0(FUN_004840d0(0), &col, 1);
  FUN_004e4320(FUN_00426060(), FUN_004840d0(0));
  FUN_004491e0(param_1);
  if (FUN_004c1a00(FUN_004840d0(0)) != 0) {
    RENDERSTATE_004492b0(0xc, 0);
    RENDERSTATE_004492b0(10, 5);
    RENDERSTATE_004492b0(0xb, 6);
    RENDERSTATE_004492b0(6, 0);
    RENDERSTATE_004492b0(8, 0);
    if (*(int *)(param_1 + 0x100f0) != 0) {
      FUN_004e6680(*(int *)(param_1 + 0x100f0));
    }
    FUN_00489240();
    RENDERSTATE_004492b0(0x14, 2);
    FUN_004e6680(*(int *)(param_1 + 0x10130));
    RENDERSTATE_004492b0(0x14, 1);
    RENDERSTATE_004492b0(10, 5);
    RENDERSTATE_004492b0(0xb, 6);
    RENDERSTATE_004492b0(0xc, 0);
    RENDERSTATE_004492b0(6, 1);
    RENDERSTATE_004492b0(8, 1);
    FUN_004c19f0(FUN_004840d0(0));
  }
  FUN_004e4350(FUN_00426060(), FUN_004840d0(0));
}
