// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h3 0x0042e3a0 (487 B): 100% (regs 100%). Raw g2s 15.9%.
   What moved it: FUN_00472c60 / FUN_00472f40 / FUN_004730b0 take two RwV2d structs by value plus an RwRGBA
   (the `mov [slot],imm; mov reg,[slot]; push reg` pairs). Which pos/size fields are re-assigned before each call
   decides which constants stay in callee-saved registers and which are rematerialized: a field kept in a register
   across calls is one the source does not re-assign. Colour stores: rgb then alpha as separate statements, white
   as one chained assignment (blue, green, red, alpha store order). Residual: none. */
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { float x, y; } RwV2d;
typedef struct { int *list; int pad[15]; } MenuRow;
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067ebc0, 0x0067ebc0);
MGLOBAL(int, DAT_007f1000, 0x007f1000);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(int, DAT_005f6980, 1, 0x005f6980);
MGLOBALA(MenuRow, DAT_0067ed38, 16, 0x0067ed38);
void FUN_0042d5a0(int t);
void FUN_00472c60(RwV2d pos, RwV2d size, RwRGBA col);
void FUN_00472f40(RwV2d pos, RwV2d size, RwRGBA col);
void FUN_004730b0(RwV2d pos, RwV2d size, RwRGBA col);
// MATCH: 0x0042e3a0 FUN_0042e3a0
void FUN_0042e3a0(void)
{
  RwRGBA col;
  RwV2d pos;
  RwV2d size;

  if (DAT_0067ed38[DAT_0067e9f8].list == DAT_005f6980) {
    DAT_0067ebc0 += DAT_007f1000;
    FUN_0042d5a0(DAT_0067ebc0 - 15000);
  }
  else {
    DAT_0067ebc0 = 0;
  }
  col.red = 0;
  col.green = 0;
  col.blue = 0;
  col.alpha = 0xa0;
  pos.x = 0.0f;
  pos.y = 0.0f;
  size.x = 640.0f;
  size.y = 64.0f;
  FUN_00472f40(pos, size, col);
  pos.x = 0.0f;
  pos.y = 416.0f;
  size.y = 64.0f;
  FUN_004730b0(pos, size, col);
  col.alpha = col.red = col.green = col.blue = 0xff;
  pos.x = 0.0f;
  size.x = 640.0f;
  size.y = 1.0f;
  FUN_00472c60(pos, size, col);
  pos.y = 64.0f;
  FUN_00472c60(pos, size, col);
  if (DAT_0067f17c > 9) {
    col.red = 0x1d;
    col.green = 0x94;
    col.blue = 0xf7;
    col.alpha = 0x80;
    pos.x = 0.0f;
    pos.y = 64.0f;
    size.x = 640.0f;
    size.y = 3.0f;
    FUN_00472c60(pos, size, col);
    col.alpha = 0x40;
    pos.y = 67.0f;
    FUN_00472c60(pos, size, col);
    col.alpha = 0x80;
    pos.y = 413.0f;
    FUN_00472c60(pos, size, col);
    col.alpha = 0x40;
    pos.y = 410.0f;
    FUN_00472c60(pos, size, col);
  }
}
