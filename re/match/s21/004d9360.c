// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9360 (h4): 79.8% (regs 79.8%, ops 92.1%). Octree leaf accumulator rewritten with an RwRGBA local masked by (1 << (8-bits)) - 1 and an RwRGBAReal local (the original frame: rgba at esp+0, real at esp+8..0x17, blue/alpha spilled to +0x10/+0x14). That took the raw 25.8% to 79.8%. Residual is x87 operand order only: the constant multiplies (fld m; fmul st(i) in ours, fxch; fmul m in the original), the order of the squared-sum terms, and the final acc += cr adds (fld temp; fadd [acc] in ours, fld [acc]; fadd temp/st(i) with two trailing fstp st(0) in the original). Not moved by sum order, a = a*k against a *= k, acc = acc + x, or separate product temps. S19 x87 class. Class (a). */
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct LeafAcc { float weight; float r, g, b, a; float sq; } LeafAcc;
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005d8d78, 0x005d8d78);
MGLOBAL(float, DAT_005d8d74, 0x005d8d74);
typedef struct RwRGBAReal { float red, green, blue, alpha; } RwRGBAReal;
// MATCH: 0x004d9360 FUN_004d9360
void FUN_004d9360(LeafAcc *acc, RwRGBA *col, float weight)
{
  unsigned char mask = (unsigned char)((1 << (8 - DAT_00618430)) - 1);
  RwRGBA c;
  RwRGBAReal cr;
  c.red = col->red & mask;
  c.green = col->green & mask;
  c.blue = col->blue & mask;
  c.alpha = col->alpha & mask;
  cr.red = (float)c.red * DAT_005ceb90;
  cr.green = (float)c.green * DAT_005ceb90;
  cr.blue = (float)c.blue * DAT_005ceb90;
  cr.alpha = (float)c.alpha * DAT_005ceb90;
  cr.red *= DAT_005d8d78;
  cr.blue *= DAT_005d8d74;
  acc->weight += weight;
  acc->sq += (cr.red * cr.red + cr.green * cr.green + cr.blue * cr.blue + cr.alpha * cr.alpha) * weight;
  cr.red *= weight;
  cr.green *= weight;
  cr.blue *= weight;
  cr.alpha *= weight;
  acc->r += cr.red;
  acc->g += cr.green;
  acc->b += cr.blue;
  acc->a += cr.alpha;
}
