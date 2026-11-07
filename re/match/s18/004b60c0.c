// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b60c0 (h3): hand, 100%. The identity is RW's RwMatrixSetIdentity chained-assignment order
   (at.z = up.y = right.x first), the 0.5 is a float literal (a DAT_005cc32c global gets CSE'd into a register, 89.3%;
   0x005cc32c holds 0.5f), and the flags |= 0x20003 sits after the centre (any earlier place 92.9%). Box (min/max pair) -> half extent + identity matrix translated to the centre. */
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
typedef struct { float x, y, z; } V3_4b60c0;
typedef struct { V3_4b60c0 right; unsigned int flags; V3_4b60c0 up; unsigned int pad1; V3_4b60c0 at; unsigned int pad2; V3_4b60c0 pos; unsigned int pad3; } M_4b60c0;
void FUN_004c51a0(M_4b60c0 *m, const V3_4b60c0 *v, int op);
void FUN_004b5d90(const V3_4b60c0 *half, const M_4b60c0 *m, void *color);
// MATCH: 0x004b60c0 FUN_004b60c0
void FUN_004b60c0(V3_4b60c0 *param_1, void *param_2)
{
  V3_4b60c0 half;
  V3_4b60c0 centre;
  M_4b60c0 m;

  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  half.x = param_1[0].x - param_1[1].x;
  half.y = param_1[0].y - param_1[1].y;
  half.z = param_1[0].z - param_1[1].z;
  half.x = half.x * 0.5f;
  half.y = half.y * 0.5f;
  half.z = half.z * 0.5f;
  centre.x = half.x + param_1[1].x;
  centre.y = half.y + param_1[1].y;
  centre.z = half.z + param_1[1].z;
  m.flags |= 0x20003;
  FUN_004c51a0(&m, &centre, 0);
  FUN_004b5d90(&half, &m, param_2);
}
