// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041d930: best 97.0% (regs 97.0%), raw g2s 29.9%. Written from the disassembly.
   What moved it: the identity matrix written as RenderWare's RwMatrixSetIdentity macro text (chained
   assignments right.x = up.y = at.z = 1, right.y = right.z = up.x = 0, up.z = at.x = at.y = 0,
   pos.x = pos.y = pos.z = 0, then flags |= 0x20003): 79.1% -> 91.0% (one statement per field in
   address order does not give the original store order). The 1.125 scale factor as a literal (`* 1.125f`,
   same value as 0x005cd118) instead of the global: 91.0% -> 97.0% (with the global, cl loads the constant
   first: `fld [k]; fmul [x]`).
   Residual (a) scheduling: the second multiply `fld scale.y` is issued after `push ecx` instead of before
   it (2 instructions). Tried with no effect: `*=`, `k * x`, a shared local k, chained init, pointers o/a
   (RwV3dScale macro form, worse), the frame pointer in a local (worse), double literal (worse), and the
   permuter (187 compiles, no move).
   Locals: scale V3 at esp+0, translation V3 {0, 0.35, 1} at esp+0xc, RwMatrix at esp+0x18. Calls:
   0x004c51a0(m, &pos, 0), 0x004c1480(DAT_0063d580, m, 0), 0x004c13e0(DAT_0063d580, &scale, 1).
   Constants: 0x005cd22c 2.4, 0x005cc564 0.25, 0x005cd228 0.35, 0x005cd118 1.125. */
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
typedef struct { float x, y, z; } MatchV3_0041d930;
typedef struct {
  MatchV3_0041d930 right; unsigned int flags;
  MatchV3_0041d930 up; unsigned int pad1;
  MatchV3_0041d930 at; unsigned int pad2;
  MatchV3_0041d930 pos; unsigned int pad3;
} MatchMtx_0041d930;
MGLOBAL(float, DAT_005cd22c, 0x005cd22c);
MGLOBAL(float, DAT_005cc564, 0x005cc564);
MGLOBAL(float, DAT_005cd228, 0x005cd228);
MGLOBAL(char *, DAT_0063d580, 0x0063d580);
MGLOBAL(int, DAT_0063d584, 0x0063d584);
MGLOBAL(float, DAT_0063d588, 0x0063d588);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
void *FUN_004c51a0(MatchMtx_0041d930 *m, MatchV3_0041d930 *v, int op);
void *FUN_004c1480(char *frame, MatchMtx_0041d930 *m, int op);
void *FUN_004c13e0(char *frame, MatchV3_0041d930 *v, int op);

// MATCH: 0x0041d930 FUN_0041d930
void FUN_0041d930(void)
{
  MatchV3_0041d930 scale;
  MatchV3_0041d930 pos;
  MatchMtx_0041d930 m;

  pos.x = 0.0f;
  pos.y = 0.35f;
  pos.z = 1.0f;
  scale.x = 1.0f;
  scale.y = 1.0f;
  scale.z = 1.0f;
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags |= 0x20003;
  if (DAT_0063d588 > DAT_005cd22c)
    pos.y = (DAT_0063d588 - DAT_005cd22c) * DAT_005cc564 + DAT_005cd228;
  FUN_004c51a0(&m, &pos, 0);
  FUN_004c1480(DAT_0063d580, &m, 0);
  scale.x = scale.x * 1.125f;
  scale.y = scale.y * 1.125f;
  scale.z = scale.z * 1.125f;
  FUN_004c13e0(DAT_0063d580, &scale, 1);
  DAT_0063d588 += DAT_007f100c;
  DAT_0063d584++;
}
