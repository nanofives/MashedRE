// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041dca0 (464 B): best 83.2% (regs 83.2%), raw g2s 34.7%. Written from the disassembly.
   This file also holds 0x0041dc30 (103 B), its only callee (call site 0x0041dcd9): 100% (regs 100%).
   0x0041dc30 takes the record 0x0063d5a0 in EDI (Ghidra unaff_EDI), so it is a `static` callee defined
   before its caller (pilot finding 1); cl 9176 gives it EDI unsaved and the caller saves EDI around the call.
   What moved 0x0041dca0: the identity matrix as RenderWare's RwMatrixSetIdentity macro text (chained
   assignments, see 0x0041d930) and the brightness kept in scale.x (the original works on [esp] = scale.x
   in memory), the 1/3 increment as a literal (0.33333334f, same value as 0x005ccac8: with the global cl
   loads the constant first), the 1/3/2/3/1 ladder as an else-if chain, the clamp as
   `if (1.0 <= x) x = 1; else if (!(x >= 0.0)) x = 0;`.
   Residual (b) source shape, two places:
   1. the original copies scale onto itself (3 dword loads and 3 stores at esp+4..0xc) between
      0x004c1480 and the first 0x004c13e0. Our cl drops every self-assignment form tried (`s = s`,
      `*&s`, through a pointer local, through a cast); a second local copied from scale gets its own slot
      (frame 0x64 instead of 0x58, 75.2%) since this compiler does not share stack slots between
      locals here.
   2. the `x = 1.0` arm of the clamp sits after the ret in the original (jnp to it); every form tried
      keeps it inline (ternary, goto, inverted condition, Ghidra's parity form).
   Permuter: 191 compiles, no move. Constants: 0x005cc320 1.0, 0x005cc574 2.0, 0x005cc31c 3.0,
   0x005d757c 0.0, 0x005ccac8 1/3. Calls: 0x004c51a0(m, &pos {0.48, 0.35, 1}, 0) translate,
   0x004c1480(DAT_0063d5e4, m, 0), 0x004c13e0(frame, &scale, 1 / 0). */
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
typedef struct {
  char *m_p[16];     /* +0x00..+0x3c */
  char *m_40;        /* +0x40 (0x0063d5e0) */
  char *m_44;        /* +0x44 (0x0063d5e4) */
  int m_48;          /* +0x48 (0x0063d5e8) */
  float m_4c;        /* +0x4c (0x0063d5ec) */
  int m_50;          /* +0x50 (0x0063d5f0) */
} MatchRec0063d5a0;
MGLOBALA(float, DAT_0063d590, 4, 0x0063d590);
MGLOBAL(MatchRec0063d5a0, DAT_0063d5a0, 0x0063d5a0);
float FUN_00417730(int i);

// MATCH: 0x0041dc30 FUN_0041dc30
static void FUN_0041dc30(MatchRec0063d5a0 *r)
{
  int i;
  for (i = 0; i < 4; i++)
    DAT_0063d590[i] = FUN_00417730(i);
  r->m_4c = DAT_0063d590[0];
  r->m_50 = 4;
  if (r->m_4c >= DAT_0063d590[1])
    r->m_50 = 3;
  if (r->m_4c >= DAT_0063d590[2])
    r->m_50--;
  if (r->m_4c >= DAT_0063d590[3])
    r->m_50--;
}

typedef struct { float x, y, z; } MatchV3_0041dca0;
typedef struct {
  MatchV3_0041dca0 right; unsigned int flags;
  MatchV3_0041dca0 up; unsigned int pad1;
  MatchV3_0041dca0 at; unsigned int pad2;
  MatchV3_0041dca0 pos; unsigned int pad3;
} MatchMtx_0041dca0;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(unsigned int, DAT_007f101c, 0x007f101c);
void *FUN_004c51a0(MatchMtx_0041dca0 *m, MatchV3_0041dca0 *v, int op);
void *FUN_004c1480(char *frame, MatchMtx_0041dca0 *m, int op);
void *FUN_004c13e0(char *frame, MatchV3_0041dca0 *v, int op);

// MATCH: 0x0041dca0 FUN_0041dca0
void FUN_0041dca0(void)
{
  MatchV3_0041dca0 scale;
  MatchV3_0041dca0 pos;
  MatchMtx_0041dca0 m;
  pos.x = 0.48f;
  pos.y = 0.35f;
  pos.z = 1.0f;
  scale.x = 1.0f;
  scale.y = 1.0f;
  scale.z = 1.0f;
  FUN_0041dc30(&DAT_0063d5a0);
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags |= 0x20003;
  FUN_004c51a0(&m, &pos, 0);
  FUN_004c1480(DAT_0063d5a0.m_44, &m, 0);
  FUN_004c13e0(DAT_0063d5a0.m_44, &scale, 1);
  if (DAT_0063d5a0.m_4c < DAT_005cc320)
    scale.x = 0.0f;
  else if (DAT_0063d5a0.m_4c < DAT_005cc574)
    scale.x = 0.33333334f;
  else if (DAT_0063d5a0.m_4c < DAT_005cc31c)
    scale.x = 0.6666667f;
  else
    scale.x = 1.0f;
  if (DAT_007f101c & 0x10)
    scale.x += 0.33333334f;
  if (DAT_005cc320 <= scale.x)
    scale.x = 1.0f;
  else if (!(scale.x >= DAT_005d757c))
    scale.x = 0.0f;
  scale.z = 1.0f;
  scale.y = 1.0f;
  FUN_004c13e0(*(char **)(DAT_0063d5a0.m_p[2] + 4), &scale, 0);
}
