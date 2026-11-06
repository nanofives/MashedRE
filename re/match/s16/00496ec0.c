// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496ec0 (h3): by hand, 100% (regs 100%), relocheck OK. Stack RwMatrix set to identity with the
   RwMatrixSetIdentity macro shape (chained assignments, flags |= 0x20003), inverted (0x004c4dc0) from the frame
   LTM (0x004c0ed0 on camera+4), transposed into a 16-float view matrix with negated row 0, struct-copied out. */
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
typedef struct RwMatrix { RwV3d right; uint flags; RwV3d up; uint pad1; RwV3d at; uint pad2; RwV3d pos; uint pad3; } RwMatrix;
typedef struct Mat44 { float m[16]; } Mat44;
typedef struct RwObjectHdr { int type_etc; void *parent; } RwObjectHdr;
RwMatrix *FUN_004c0ed0(void *frame);
RwMatrix *FUN_004c4dc0(RwMatrix *dst, RwMatrix *src);
// MATCH: 0x00496ec0 FUN_00496ec0
void FUN_00496ec0(Mat44 *out, RwObjectHdr *camera)
{
  RwMatrix m;
  Mat44 r;
  RwMatrix *ltm;

  ltm = FUN_004c0ed0(camera->parent);
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags = m.flags | 0x20003;
  FUN_004c4dc0(&m, ltm);
  r.m[0] = -m.right.x;
  r.m[1] = -m.up.x;
  r.m[2] = -m.at.x;
  r.m[3] = -m.pos.x;
  r.m[4] = m.right.y;
  r.m[5] = m.up.y;
  r.m[6] = m.at.y;
  r.m[7] = m.pos.y;
  r.m[8] = m.right.z;
  r.m[9] = m.up.z;
  r.m[10] = m.at.z;
  r.m[11] = m.pos.z;
  r.m[12] = 0.0f;
  r.m[13] = 0.0f;
  r.m[14] = 0.0f;
  r.m[15] = 1.0f;
  *out = r;
}
