// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00443210: 100% by hand (h7). Raw does not compile (uint3). The constants are float literals
   (7.0f, 6.0f, 1.0f, 2.0f, 3.0f): as named globals `x + g` compiles to fld g; fadd x (94.8%). RwRGBA local passed by value to
   FUN_004430e0 (this file holds a prototype only; 0x004430e0 is matched in its own file). */
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
typedef struct {
  uchar r;
  uchar g;
  uchar b;
  uchar a;
} RGBA_00443210;
void FUN_004430e0(float x0, float y0, float x1, float y1, float x2, float y2, RGBA_00443210 col);
// MATCH: 0x00443210 FUN_00443210
void FUN_00443210(float param_1, float param_2, float param_3)
{
  RGBA_00443210 col;
  float y0;
  float x2;
  float y1;
  float x1;
  float x0;

  col.r = 0xff;
  col.g = 0;
  col.b = 0;
  if (param_3 == 1.0f) {
    col.g = 0x80;
  }
  if (param_3 == 2.0f) {
    col.g = 0xff;
  }
  if (param_3 == 3.0f) {
    col.r = 0x80;
    col.g = 0xff;
  }
  col.a = 0xff;
  y0 = param_2 - 7.0f;
  x2 = param_1 - 6.0f;
  y1 = param_2 + 7.0f;
  x1 = param_1 + 7.0f;
  x0 = param_1 - 7.0f;
  FUN_004430e0(x0, y0, x1, y1, x2, y0, col);
  FUN_004430e0(x0, y1, x1, y0, x2, y1, col);
}
