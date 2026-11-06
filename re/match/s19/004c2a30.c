// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c2a30 (h2): 49.6%, regs 49.6% (ops 76%). Hand: typed camera/frame LTM, view matrix built transposed from LTM right/up/at/pos with an RwV3d temp (struct temp lifted 14.5% raw -> 47.9%; permuter moved t.y after o, 49.6%, 524 compiles). Residual (b): x87 operand choice: the original loads the memory operand and multiplies by the register copy of the scale (fld [m]; fmul st(2)), 9176 here duplicates the register (fld st(1); fmul [m]); operand order swaps do not move it; the original also parks the dead scale under the result (fstp st(2)) and pops three values per block. */
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
typedef struct { float x, y, z; } RwV3d_t;
typedef struct { RwV3d_t right; int flags; RwV3d_t up; int pad1; RwV3d_t at; int pad2; RwV3d_t pos; int pad3; } RwMatrix_t;
typedef struct { char pad[0x50]; RwMatrix_t ltm; } RwFrame_t;
typedef struct { float x, y; } RwV2d_t;
typedef struct {
  char pad0[4]; RwFrame_t *frame; char pad8[0x18];
  RwMatrix_t viewMatrix;                /* 0x20 */
  char pad60[0x10];
  RwV2d_t recipViewWindow;              /* 0x70 */
  RwV2d_t viewOffset;                   /* 0x78 */
} RwCamera_t;
MGLOBAL(float, _DAT_005cd50c, 0x005cd50c);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
int FUN_004c4530(RwMatrix_t *p0, int p1);
// MATCH: 0x004c2a30 FUN_004c2a30
RwCamera_t *FUN_004c2a30(RwCamera_t *cam)
{
  RwMatrix_t *ltm = &cam->frame->ltm;
  RwMatrix_t *vm = &cam->viewMatrix;
  RwV3d_t t;
  float s, o;

  s = cam->recipViewWindow.x * _DAT_005cd50c;
  t.x = ltm->right.x * s;
  t.z = ltm->right.z * s;
  o = -(cam->viewOffset.x * s);
  t.y = ltm->right.y * s;
  t.x += ltm->at.x * o;
  t.y += ltm->at.y * o;
  t.z += ltm->at.z * o;
  vm->right.x = t.x;
  vm->up.x = t.y;
  vm->at.x = t.z;
  vm->pos.x = _DAT_005cc32c - (ltm->pos.x * t.x + ltm->pos.y * t.y + ltm->pos.z * t.z + o);

  s = cam->recipViewWindow.y * _DAT_005cd50c;
  t.x = ltm->up.x * s;
  t.y = ltm->up.y * s;
  t.z = ltm->up.z * s;
  o = cam->viewOffset.y * s;
  t.x += ltm->at.x * o;
  t.y += ltm->at.y * o;
  t.z += ltm->at.z * o;
  vm->right.y = t.x;
  vm->up.y = t.y;
  vm->at.y = t.z;
  vm->pos.y = _DAT_005cc32c - (ltm->pos.x * t.x + ltm->pos.y * t.y + ltm->pos.z * t.z + o);

  vm->right.z = ltm->at.x;
  vm->up.z = ltm->at.y;
  vm->at.z = ltm->at.z;
  vm->pos.z = -(ltm->pos.x * ltm->at.x + ltm->pos.y * ltm->at.y + ltm->pos.z * ltm->at.z);

  FUN_004c4530(vm, 0);
  return cam;
}
