// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042bde0 (h6, by hand): 100% (regs 100%), whole obj symbol byte-identical. Raw 18.0%. Five FUN_00472c60 rects (48/46 x, 534/26 box, 2-wide borders) with two RGBA colours built byte by byte (alpha>>1, alpha). Key finding: every float constant goes through a stack slot and an integer register (mov [slot],imm / mov reg,[slot] / push reg), which cl only emits when the floats are passed by their bits: FUN_00472c60 is declared with int params here and the inlined DrawBox helper (file-static __inline, not a real function, no MATCH line) passes *(int *)&pos->x etc. With float params cl pushes the immediates (29-45%). pos/size/full are V2 structs (full = size copy keeps 534/26 in memory). The .rdata values 0x005cc35c/0x005cc574/0x005cd784 are compiler literals 4.0f/2.0f/48.0f (as globals cl loads them first, 96%); the y offset is one temp d = full.y - 2.0f added then subtracted; call 5 assigns pos.x, size.x, then pos.y -= d, then size.y. Residual: none. */
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
typedef struct { float x; float y; } V2;
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } RGBA4;
void FUN_00472c60(int x, int y, int w, int h, RGBA4 col);
static __inline void DrawBox(V2 *pos, V2 *size, RGBA4 col)
{
  FUN_00472c60(*(int *)&pos->x, *(int *)&pos->y, *(int *)&size->x, *(int *)&size->y, col);
}
// MATCH: 0x0042bde0 FUN_0042bde0
void FUN_0042bde0(int ypos, unsigned char alpha)
{
  RGBA4 col;
  V2 pos;
  V2 size;
  V2 full;
  float d;
  pos.y = (float)(ypos - 13);
  col.r = 0xf0;
  col.g = 0x6e;
  col.b = 0x14;
  col.a = alpha >> 1;
  size.y = 26.0f;
  size.x = 534.0f;
  full = size;
  pos.x = 48.0f;
  DrawBox(&pos, &size, col);
  col.r = 0xb4;
  col.g = 0x50;
  col.b = 0x10;
  col.a = alpha;
  size.x = 2.0f;
  pos.x = 46.0f;
  DrawBox(&pos, &size, col);
  size.x = full.x + 4.0f;
  size.y = 2.0f;
  DrawBox(&pos, &size, col);
  d = full.y - 2.0f;
  pos.y += d;
  DrawBox(&pos, &size, col);
  pos.x = full.x + 48.0f;
  size.x = 2.0f;
  pos.y -= d;
  size.y = full.y;
  DrawBox(&pos, &size, col);
}
