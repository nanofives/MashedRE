// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b42c0 (helper h1): 89.0% (regs 89.0%, ops 92.7%), 109/109 ins. Typed as a look-at: one RwMatrix-shaped local m (right/up/at/pos at +0/+0x10/+0x20/+0x30), m.pos = LTM pos (struct copy), at = target - pos, right = at x up-param, up = right x at, three normalizes, update, invert LTM, multiply, m = tmp (rep movsd), transform(frame,&m,1). Raw g2s was 37.6%. Remaining: x87 scheduling only - fstp st(0) after two normalize calls lands after the next integer op, and the up.x products are interleaved with the normalize arg pushes. Tried: RW cross-product macros (both operand orders), vector pointer locals, dead len = normalize(), double/float10 return, /G5 /G6 /Ob2, permuter 374 variants (no gain). Class (b). */
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
typedef struct { float x; float y; float z; } RwV3d_;
typedef struct { RwV3d_ right; uint flags; RwV3d_ up; uint pad1; RwV3d_ at; uint pad2; RwV3d_ pos; uint pad3; } RwMatrix_;
RwMatrix_ *FUN_004c0ed0(int p0);
int FUN_004c1480(int p0, RwMatrix_ *p1, int p2);
float FUN_004c39b0(RwV3d_ *p0, RwV3d_ *p1);
void FUN_004c45f0(RwMatrix_ *p0);
RwMatrix_ *FUN_004c4600(RwMatrix_ *p0, RwMatrix_ *p1, RwMatrix_ *p2);
RwMatrix_ *FUN_004c4dc0(RwMatrix_ *p0, RwMatrix_ *p1);
// MATCH: 0x004b42c0 FUN_004b42c0
void FUN_004b42c0(int param_1,RwV3d_ *param_2,RwV3d_ *param_3)
{
  RwMatrix_ *ltm;
  RwMatrix_ m;
  RwMatrix_ inv;
  RwMatrix_ tmp;

  ltm = FUN_004c0ed0(param_1);
  m.pos = ltm->pos;
  m.at.x = param_2->x - m.pos.x;
  m.at.y = param_2->y - m.pos.y;
  m.at.z = param_2->z - m.pos.z;
  FUN_004c39b0(&m.at,&m.at);
  m.right.x = m.at.z * param_3->y - m.at.y * param_3->z;
  m.right.y = m.at.x * param_3->z - m.at.z * param_3->x;
  m.right.z = m.at.y * param_3->x - m.at.x * param_3->y;
  FUN_004c39b0(&m.right,&m.right);
  m.up.x = m.right.z * m.at.y - m.right.y * m.at.z;
  m.up.y = m.right.x * m.at.z - m.right.z * m.at.x;
  m.up.z = m.right.y * m.at.x - m.right.x * m.at.y;
  FUN_004c39b0(&m.up,&m.up);
  FUN_004c45f0(&m);
  FUN_004c4dc0(&inv,ltm);
  FUN_004c4600(&tmp,&m,&inv);
  m = tmp;
  FUN_004c1480(param_1,&m,1);
}
