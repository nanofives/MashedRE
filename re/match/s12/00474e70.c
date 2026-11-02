// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00474e70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs12_20261102/results.tsv. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef struct { float x, y, z; } Vec3;
float FUN_004c3ac0(Vec3 *p0);
// MATCH: 0x00474e70 FUN_00474e70
float FUN_00474e70(Vec3 *param_1, Vec3 *param_2, Vec3 *param_3)
{
  Vec3 b;
  Vec3 a;
  Vec3 c;
  float len;

  a.x = param_2->x - param_1->x;
  a.y = param_2->y - param_1->y;
  a.z = param_2->z - param_1->z;
  len = FUN_004c3ac0(&a);
  b.x = param_1->x - param_3->x;
  b.y = param_1->y - param_3->y;
  b.z = param_1->z - param_3->z;
  FUN_004c3ac0(&b);
  c.x = b.z * a.y - b.y * a.z;
  c.y = b.x * a.z - b.z * a.x;
  c.z = b.y * a.x - b.x * a.y;
  return FUN_004c3ac0(&c) / len;
}
