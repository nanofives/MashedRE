// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458a40: 87.8% (regs 87.8%) by hand (raw 23.9%). Register args: the record arrives in EDI
   (only caller 0x004591e8 in FUN_00459000); the file holds the static callee 0x004584e0 (record in EAX, out
   vector in ESI, returns float) and a stand-in caller Stand_00458a40 (not a MATCH; real caller is 0x00459000),
   which is enough for cl 9176 to give both statics the original register conventions. Literal floats (0.95f,
   1e-05f, 0.05f) instead of the .rdata globals 0x005cc9dc/0x005cc990/0x005cc9a0: the globals put the
   constant first in fmul (82.4% -> literals). pos += vel through three temporaries gives the vel-first fadd.
   Residual (b): x87 scheduling only (push 1 before the angle fadd, the y/x/z reload order of the squared-length
   compare, the last vel.z = d.z * s). Tried 6 term orders of the compare, pointer and local-copy forms, 5
   spellings of the final scale, and the permuter (no improvement).
   0x004584e0 is at 87.8% (regs 87.8%): fld of rec+0x2c scheduled after the anchor copy, and the two
   fstp st(n) pops after the compare; class (b). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00458a40 {
  float x;
  float y;
  float z;
} V3_00458a40;
typedef struct Rec00458a40 {
  int *clump;
  float angle;
  float spin;
  V3_00458a40 axis;
  undefined4 f18[8];
  V3_00458a40 pos;
  V3_00458a40 vel;
} Rec00458a40;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);
MGLOBAL(float, DAT_005cc990, 0x005cc990);
MGLOBAL(float, DAT_005cc9a0, 0x005cc9a0);
float FUN_004c3ac0(V3_00458a40 *p0);
float FUN_004c39b0(V3_00458a40 *p0, V3_00458a40 *p1);
void FUN_004c1340(int p0, V3_00458a40 *p1, int p2);
void FUN_004c1520(int p0, V3_00458a40 *p1, float p2, int p3);
// MATCH: 0x004584e0 FUN_004584e0
static float FUN_004584e0(Rec00458a40 *rec, V3_00458a40 *out)
{
  float len;
  V3_00458a40 b;

  len = DAT_005d757c;
  b = *(V3_00458a40 *)(rec->clump[1] + 0x40);
  out->x = *(float *)((char *)rec + 0x2c) - b.x;
  out->y = *(float *)((char *)rec + 0x30) - b.y;
  out->z = *(float *)((char *)rec + 0x34) - b.z;
  if (out->z * out->z + out->y * out->y + out->x * out->x > DAT_005ce54c) {
    len = FUN_004c3ac0(out);
    FUN_004c39b0(out, out);
  }
  return len;
}

// MATCH: 0x00458a40 FUN_00458a40
static void FUN_00458a40(Rec00458a40 *rec)
{
  int frame;
  float s;
  V3_00458a40 d;

  frame = rec->clump[1];
  {
    float x = rec->vel.x + rec->pos.x;
    float y = rec->vel.y + rec->pos.y;
    float z = rec->vel.z + rec->pos.z;
    rec->pos.x = x;
    rec->pos.y = y;
    rec->pos.z = z;
  }
  rec->vel.x *= 0.95f;
  rec->vel.y *= 0.95f;
  rec->vel.z *= 0.95f;
  FUN_004c1340(frame, &rec->pos, 0);
  rec->angle += rec->spin;
  FUN_004c1520(frame, &rec->axis, rec->angle, 1);
  if (rec->vel.y * rec->vel.y + rec->vel.x * rec->vel.x + rec->vel.z * rec->vel.z < 1e-05f) {
    s = FUN_004584e0(rec, &d) * 0.05f;
    rec->vel.x = d.x * s;
    rec->vel.y = d.y * s;
    rec->vel.z = s * d.z;
  }
}

/* stand-in caller (not part of the match): gives FUN_00458a40 a call site */
void Stand_00458a40(Rec00458a40 *rec)
{
  FUN_00458a40(rec);
}
