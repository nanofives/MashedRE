// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 77.9% (regs 77.9%, ops 90.7%). Morph-target bounding sphere (BBox + Sphere structs, struct copy to out). Residual: original computes all three (inf+sup) sums before the three *0.5 scalings (z spilled to [esp+0x14]); ours interleaves add/mul per axis; dot-product term order and final maxd*k load order differ by x87 scheduling. Temps for the sums did not move it. Class (b).
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

typedef struct { float x; float y; float z; } V3;
typedef struct { V3 sup; V3 inf; } BBox;
typedef struct { V3 c; float r; } Sphere;
typedef struct { int n[0x14 / 4]; int cnt; } Hdr;
typedef struct { Hdr *hdr; int pad[4]; V3 *verts; } MT;
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005d8df0, 0x005d8df0);
float *FUN_004c1740(BBox *p0, V3 *p1, int p2);
float FUN_004c3b30(float p0);
// MATCH: 0x004e8640 FUN_004e8640
MT *FUN_004e8640(MT *param_1, Sphere *param_2)
{
  float maxd = 0.0f;
  Sphere s;
  BBox bb;
  int n = param_1->hdr->cnt;
  V3 *v;
  float sx;
  float sy;
  float sz;

  FUN_004c1740(&bb, param_1->verts, n);
  v = param_1->verts;
  sx = bb.inf.x + bb.sup.x;
  sy = bb.inf.y + bb.sup.y;
  sz = bb.inf.z + bb.sup.z;
  s.c.x = sx * DAT_005cc32c;
  s.c.y = sy * DAT_005cc32c;
  s.c.z = sz * DAT_005cc32c;
  if (n != 0) {
    do {
      float dx = v->x - s.c.x;
      float dy = v->y - s.c.y;
      float dz = v->z - s.c.z;
      float d = dy * dy + dx * dx + dz * dz;
      if (d > maxd) {
        maxd = d;
      }
      v++;
      n--;
    } while (n != 0);
    if (maxd > DAT_005d757c) {
      maxd = FUN_004c3b30(maxd);
    }
  }
  s.r = DAT_005d8df0 * maxd;
  *param_2 = s;
  return param_1;
}
