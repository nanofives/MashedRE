// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1740 (h1): 100% (regs 100%), relocheck OK. RwBBoxCalculate shape: RwBBox {sup, inf} struct copies, verts post-incremented on the second copy and at the loop end, min/max by six float compares. FLAGS add /G5. */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwBBox { RwV3d sup; RwV3d inf; } RwBBox;
// MATCH: 0x004c1740 FUN_004c1740
RwBBox *FUN_004c1740(RwBBox *boundBox, const RwV3d *verts, int numVerts)
{
  boundBox->inf = *verts;
  boundBox->sup = *verts++;
  while (--numVerts) {
    if (boundBox->inf.x > verts->x) boundBox->inf.x = verts->x;
    if (boundBox->inf.y > verts->y) boundBox->inf.y = verts->y;
    if (boundBox->inf.z > verts->z) boundBox->inf.z = verts->z;
    if (boundBox->sup.x < verts->x) boundBox->sup.x = verts->x;
    if (boundBox->sup.y < verts->y) boundBox->sup.y = verts->y;
    if (boundBox->sup.z < verts->z) boundBox->sup.z = verts->z;
    verts++;
  }
  return boundBox;
}
