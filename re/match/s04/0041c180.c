// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c180 (311 B): best 96.7% (regs 96.7, ops 98.4), 61/61 ins, by hand. Raw: 44.3%.
   Shape: local RwV3d translate {-0.48f,-0.35f,1.0f}, scale {1,1,1}, an RwMatrix set by the RW identity macro
   (chained `right.x = up.y = at.z = 1`, `right.y = right.z = up.x = 0`, `up.z = at.x = at.y = 0`,
   `pos.x = pos.y = pos.z = 0`, flags |= 0x20003), then 0x004c51a0(&m,&v,0), 0x004c1480(DAT_0063cdb8,&m,0),
   scale *= 1.25f, 0x004c13e0(DAT_0063cdb8,&s,1), s.x = 1.0f - 0x004077e0(), s.z = s.y = 1,
   0x004c13e0(*(DAT_0063cda8 + 4),&s,0). Constants: 0x005cd074 = 1.25f, 0x005cc320 = 1.0f.
   The scale has to be the literal 1.25f with `s.x = s.x * 1.25f` (an MGLOBAL float at 0x005cd074 gives
   `fld C; fmul s` = 88.5%, `*= 1.25` double 93.4%, `(float)(s.x*1.25)` 90.2%).
   Residual (2 ins): the original schedules `fld s.y` one slot earlier (before `push ecx`, the &s argument of
   0x004c13e0). Tried: *= vs = x*k, k as float local, operand order, a pointer to s (RwV3dScale macro form),
   frame in a local, moving the scale statements (earlier positions constant-fold), flags OR first, s/v
   init order, local declaration order, return type of the callee, permuter (216 compiles, no gain).
   The same 2-instruction residual sits at the same spot in 0x0041c410 (same scale-and-call pattern).
   Class (a). relocheck reports BAD on the 1.25f literal only because the residual shifts the offsets
   (ours vs original instruction at +0xe0), not a wrong value. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct MatchV3d0041c180 { float x, y, z; } MatchV3d0041c180;
typedef struct MatchMatrix0041c180 {
  MatchV3d0041c180 right; unsigned int flags;
  MatchV3d0041c180 up; unsigned int pad1;
  MatchV3d0041c180 at; unsigned int pad2;
  MatchV3d0041c180 pos; unsigned int pad3;
} MatchMatrix0041c180;
MGLOBAL(void *, DAT_0063cdb8, 0x0063cdb8);
MGLOBAL(char *, DAT_0063cda8, 0x0063cda8);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float FUN_004077e0(void);
int FUN_004c13e0(void *frame, MatchV3d0041c180 *v, int combine);
int FUN_004c1480(void *frame, MatchMatrix0041c180 *m, int combine);
void FUN_004c51a0(MatchMatrix0041c180 *m, MatchV3d0041c180 *v, int combine);
// MATCH: 0x0041c180 FUN_0041c180
void FUN_0041c180(void)
{
  MatchV3d0041c180 s;
  MatchV3d0041c180 v;
  MatchMatrix0041c180 m;

  v.x = -0.48f;
  v.y = -0.35f;
  v.z = 1.0f;
  s.x = 1.0f;
  s.y = 1.0f;
  s.z = 1.0f;
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags |= 0x20003;
  FUN_004c51a0(&m, &v, 0);
  FUN_004c1480(DAT_0063cdb8, &m, 0);
  s.x = s.x * 1.25f;
  s.y = s.y * 1.25f;
  s.z = s.z * 1.25f;
  FUN_004c13e0(DAT_0063cdb8, &s, 1);
  s.x = DAT_005cc320 - FUN_004077e0();
  s.z = 1.0f;
  s.y = 1.0f;
  FUN_004c13e0(*(void **)(DAT_0063cda8 + 4), &s, 0);
}
