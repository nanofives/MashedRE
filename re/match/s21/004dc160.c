// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dc160 (h2): not matchable, class (c). SSE inline assembly in an ebp frame (same kernel as
   0x004dc280, in a loop): transforms `count` RwV3d points, out[i] = (z*at + y*up) + (x*right + pos), matrix
   loaded with movups/movaps by its 16-byte alignment, returns out. The brief forbids __asm and 13.00 C cannot
   emit SSE. The body below is a C stand-in with the same operation order (SSE rounds each op to float; x87 C
   may keep wider intermediates). Score is the stand-in's. */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at;
  unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
// MATCH: 0x004dc160 FUN_004dc160
RwV3d *FUN_004dc160(RwV3d *out, const RwV3d *in, int count, const RwMatrix *m)
{
  RwV3d *o = out;
  do {
    float x = in->x, y = in->y, z = in->z;
    o->x = (z * m->at.x + y * m->up.x) + (x * m->right.x + m->pos.x);
    o->y = (z * m->at.y + y * m->up.y) + (x * m->right.y + m->pos.y);
    o->z = (z * m->at.z + y * m->up.z) + (x * m->right.z + m->pos.z);
    in++;
    o++;
  } while (--count);
  return out;
}
