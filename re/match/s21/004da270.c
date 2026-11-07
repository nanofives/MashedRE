// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004da270 (h2): 81.2% (regs 81.2%, ops 93.8%). Written from odis.py (the raw converter did not
   compile). Octree seed pass: depth > 0 clears the node (+0x18 byte, +0..+0x14 floats) and recurses over the 16
   children with the colour offset by bit i>>3/i>>2/i>>1/i (shift = depth - DAT_00618430 + 7), merging each
   non-NULL result with FUN_004d9ee0(node, node, r); depth 0 finalises var (+0x14) = var - (s1^2+s2^2+s3^2+s4^2)/n,
   clamps it to 0 below DAT_005d757c, and adds n * colour/255-scaled (0x005ceb90; red *0x005d8d78, blue
   *0x005d8d74) into s1..s4. NULL node returns NULL.
   What moved it: a 4-float RwRGBAReal local (the 0x10 frame) and a result variable (65% -> 81.2%).
   Residual: (1) the int->float fild temp uses the dead `depth` slot in the original, the dead `node` slot in
   ours; (2) x87 operand order: `fmul m32` against our `fld m32; fmul st(i)` for the 0x005d8d78/74 scales and
   `fld s; fadd c` against our `fld c; fadd s` in the four sums (the S19 x87 operand-order class). Tried
   compound vs explicit assignment, a cached n, and the permuter (506 compiles: 82.5% only by moving the
   0x005d8d78 multiply after the *n, which changes the rounding, so not kept). Class (b) for (1), (a) for (2). */
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
typedef struct RwRGBAReal { float red, green, blue, alpha; } RwRGBAReal;
typedef struct OctNode { float n; float s1, s2, s3, s4; float var; unsigned char index; char pad[3];
  struct OctNode *child[16]; } OctNode;
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005d8d78, 0x005d8d78);
MGLOBAL(float, DAT_005d8d74, 0x005d8d74);
void FUN_004d9ee0(OctNode *out, OctNode *a, OctNode *b);
// MATCH: 0x004da270 FUN_004da270
OctNode *FUN_004da270(OctNode *node, RwRGBA *color, int depth)
{
  int i;
  RwRGBAReal c;
  OctNode *res = 0;
  if (node) {
    if (depth > 0) {
      node->index = 0;
      node->n = 0.0f;
      node->s1 = 0.0f;
      node->s2 = 0.0f;
      node->s3 = 0.0f;
      node->s4 = 0.0f;
      node->var = 0.0f;
      for (i = 0; i < 16; i++) {
        RwRGBA col;
        OctNode *r;
        int shift = depth - DAT_00618430 + 7;
        col.red = color->red + (unsigned char)(((i >> 3) & 1) << shift);
        col.green = color->green + (unsigned char)(((i >> 2) & 1) << shift);
        col.blue = color->blue + (unsigned char)(((i >> 1) & 1) << shift);
        col.alpha = color->alpha + (unsigned char)((i & 1) << shift);
        r = FUN_004da270(node->child[i], &col, depth - 1);
        if (r) FUN_004d9ee0(node, node, r);
      }
      return node;
    }
    node->var = node->var - (node->s1 * node->s1 + node->s2 * node->s2 + node->s3 * node->s3 +
                             node->s4 * node->s4) / node->n;
    if (node->var < DAT_005d757c) node->var = 0.0f;
    c.red = (float)color->red * DAT_005ceb90;
    c.green = (float)color->green * DAT_005ceb90;
    c.blue = (float)color->blue * DAT_005ceb90;
    c.alpha = (float)color->alpha * DAT_005ceb90;
    c.red *= DAT_005d8d78;
    c.blue *= DAT_005d8d74;
    c.red *= node->n;
    c.green *= node->n;
    c.blue *= node->n;
    c.alpha *= node->n;
    node->s1 += c.red;
    node->s2 += c.green;
    node->s3 += c.blue;
    node->s4 += c.alpha;
    res = node;
  }
  return res;
}
