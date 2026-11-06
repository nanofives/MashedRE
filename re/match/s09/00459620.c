// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459620: 63.7% (regs 72.8%, ops 90.6%), 758/776 ins, by hand + permuter (raw was 11.2%).
   Structure: param_1 indexes the 0x58-byte slot array at 0x0068b9f8; 4 game slots then FUN_004075a0() extra
   targets fill a 4-entry 0x1c-byte candidate array (memset, rep stosd); pick min angle; else-branch (no
   candidate) probes a vertical line with FUN_0045bfe0/FUN_004b4cd0; then a line test from slot+0x48 to
   slot+0x3c and a scan over FUN_0047ce70() objects; ends with the frame update and a 64-byte matrix copy.
   What moved it: stack frame 0x110 reached only with the candidate array, the FUN_0045bfe0 result in the
   n==0 branch and the FUN_0055bde0 result sharing one union at frame 0x70 (the original uses the same 0x70
   bytes for all three), the FUN_0041f030 result buffer (V4 at 0x3c) reused as the post-part direction, the
   else-path direction and the best-hit point sharing one V3 (0x54), the n==0 branch scale temporaries as
   scalars (the original spills only the z one); `(float)acos * 57.29578f` (dword fmul); the .rdata floats are
   literals (1e-05f, 57.29578f, 0.2f, 0.02f, 0.7f, 1e-06f, 0.0f, 1.0f); permuter: `if (n != 0)` first and the
   slot+0x08 store moved. Declaration order has no effect on the layout here.
   Residual (b) source shape: register allocation in the first two loops (original keeps param_2 in EDI, the
   loop index in EBX and the candidate count n in memory at frame 0x1c; ours enregisters n and reloads
   param_2), which shifts every later register choice; the frame slot packing of the scalars (0x1c/0x20/0x4c/
   0x50 each hold 3-5 variables in the original) follows. Ran out of budget (60 min). */
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
typedef struct { float x, y, z; } V3_459620;
typedef struct { V3_459620 p; float w; } V4_459620;
typedef struct { V3_459620 start; V3_459620 end; } Line_459620;
typedef struct { unsigned char r, g, b, a; } Rgba_459620;
typedef struct {
  V3_459620 right; unsigned int flags;
  V3_459620 up; unsigned int pad1;
  V3_459620 at; unsigned int pad2;
  V3_459620 pos; unsigned int pad3;
} Mat_459620;
/* 0x40-byte result FUN_0045bfe0 / FUN_0041f2c0 write (+0x00 vector, +0x38 parameter) */
typedef struct { V3_459620 n; float f0c[11]; float t; float f3c; } Hit_459620;
/* 0x1c-byte candidate record */
typedef struct { V3_459620 pos; int idx; int idx2; float dist; float ang; } Cand_459620;
/* 0x14-byte result FUN_0055bde0 writes (+0x04 point) */
typedef struct { float f00; V3_459620 p; float f10; } Pt_459620;
typedef struct {
  int idx;              /* +0x00 */
  float f04;            /* +0x04 */
  int f08;              /* +0x08 */
  int f0c;              /* +0x0c */
  int f10;              /* +0x10 */
  int f14;              /* +0x14 */
  float f18;            /* +0x18 */
  float f1c;            /* +0x1c */
  int f20;              /* +0x20 */
  V3_459620 f24;        /* +0x24 */
  float f30;            /* +0x30 */
  Rgba_459620 f34;      /* +0x34 */
  Mat_459620 *mat;      /* +0x38 */
  V3_459620 f3c;        /* +0x3c */
  V3_459620 f48;        /* +0x48 */
  int clump;            /* +0x54 */
} Slot_459620;
typedef struct { char pad[0x64]; unsigned int *bits; } Obj24_459620;
typedef struct { char pad[0x10]; int *f10; char pad14[0xc]; unsigned short f20; short f22; Obj24_459620 *f24; } Obj_459620;
MGLOBALA(Slot_459620, DAT_0068b9f8, 4, 0x0068b9f8);
void *memset(void *p, int c, unsigned int n);
double FUN_004a3384(double x);
int FUN_004075a0(void);
int FUN_004075b0(int i, V4_459620 *out);
int FUN_0040e370(int i);
void FUN_0041f030(int i, V4_459620 *out);
int FUN_0041f1c0(int i, int seg);
int FUN_0041f2c0(int i, int seg, Line_459620 *line, Hit_459620 *hit);
void FUN_0041f220(int i, Mat_459620 *m);
int FUN_0041f300(int i);
void FUN_00425fa0(int i, V4_459620 *out);
int FUN_0045bfe0(Line_459620 *line, Hit_459620 *hit);
float FUN_004726f0(V3_459620 *a, V3_459620 *b);
int FUN_0047ce70(void);
unsigned char *FUN_0047d130(int i);
void FUN_004b42c0(int frame, V3_459620 *pos, V3_459620 *up);
void FUN_004b4650(V3_459620 *out, V3_459620 *a, V3_459620 *b, float t);
int FUN_004b4cd0(int a);
void FUN_004c1340(int frame, V3_459620 *v, int op);
void FUN_004c15c0(int frame);
float FUN_004c39b0(V3_459620 *out, V3_459620 *in);
float FUN_004c3ac0(V3_459620 *v);
float FUN_004c3b30(float x);
void FUN_004c45f0(Mat_459620 *m);
void FUN_00558b40(V3_459620 *a, V4_459620 *b, float *c);
void FUN_0055bb70(int a, int b, V3_459620 *out);
int FUN_0055bde0(int a, int b, V3_459620 *s, V3_459620 *e, Pt_459620 *out);
Obj_459620 *FUN_0057c210(unsigned char *e);
// MATCH: 0x00459620 FUN_00459620
void FUN_00459620(int param_1, V3_459620 *param_2, float param_3, float param_4, Rgba_459620 *param_5)
{
  Slot_459620 *slot;
  V3_459620 d;
  int n;
  int bi;
  Line_459620 ln;
  V4_459620 pos;
  float best;
  float dist;
  V3_459620 dir;
  V3_459620 c;
  union { Cand_459620 cand[4]; Hit_459620 hit0; Pt_459620 pt; } u;
  Hit_459620 hit;
  int i;
  int id;
  int seg;
  int flag;
  float ang;
  float len;
  float s;
  float t;
  float r2;
  float bestd;
  float dd;
  int cnt;
  float tx;
  float ty;
  float tz;
  unsigned char *e;
  Obj_459620 *g;
  int a;
  int b;

  slot = &DAT_0068b9f8[param_1];
  memset(u.cand, 0, sizeof(u.cand));
  slot->f34 = *param_5;
  slot->f04 = param_4;
  slot->f18 = param_3;
  slot->f14 = 0;
  slot->f0c = -1;
  bi = -1;
  slot->f48 = *param_2;
  best = 1000.0f;
  n = 0;
  i = 4;
  FUN_0041f220(param_1, slot->mat);
  FUN_004c45f0(slot->mat);
  while (i--) {
    if (param_1 != i && FUN_0040e370(i) != 0) {
      FUN_0041f030(i, &pos);
      d.x = pos.p.x - param_2->x;
      d.y = 0.0f;
      d.z = pos.p.z - param_2->z;
      dist = FUN_004c3ac0(&d);
      if (d.x * d.x + d.y * d.y + d.z * d.z > 1e-05f) {
        FUN_004c39b0(&d, &d);
      }
      ang = (float)FUN_004a3384(FUN_004726f0(&d, &slot->mat->at)) * 57.29578f;
      if (ang <= param_4 && dist <= param_3) {
        u.cand[n].pos = pos.p;
        u.cand[n].idx = i;
        u.cand[n].idx2 = -1;
        u.cand[n].dist = dist;
        u.cand[n].ang = ang;
        n++;
      }
    }
  }
  i = FUN_004075a0();
  while (i--) {
    if (FUN_004075b0(i, &pos) != 0) {
      d.x = pos.p.x - param_2->x;
      d.y = 0.0f;
      d.z = pos.p.z - param_2->z;
      dist = FUN_004c3ac0(&d);
      if (d.x * d.x + d.y * d.y + d.z * d.z > 1e-05f) {
        FUN_004c39b0(&d, &d);
      }
      ang = (float)FUN_004a3384(FUN_004726f0(&d, &slot->mat->at)) * 57.29578f;
      if (ang <= param_4 && dist <= param_3) {
        u.cand[n].pos = pos.p;
        u.cand[n].idx = -1;
        u.cand[n].idx2 = i;
        u.cand[n].dist = dist;
        u.cand[n].ang = ang;
        n++;
      }
    }
  }
  if (n != 0) {
    i = n;
    do {
      i--;
      if (u.cand[i].ang < best) {
        best = u.cand[i].ang;
        bi = i;
      }
    } while (i != 0);
    if (bi != -1) {
      id = -1;
      flag = 0;
      t = 0.0f;
      seg = -1;
      if (u.cand[bi].idx != -1) {
        id = u.cand[bi].idx;
        FUN_0041f030(id, &pos);
        seg = FUN_0041f300(id);
        flag = 1;
      } else if (u.cand[bi].idx2 != -1) {
        FUN_00425fa0(u.cand[bi].idx2, &pos);
      }
      ln.start = *param_2;
      ln.end = pos.p;
      ln.end.y = pos.w * 0.2f + pos.p.y;
      if (flag == 0 || FUN_0041f1c0(id, seg) == 0) {
        FUN_00558b40(&ln.start, &pos, &t);
      } else if (FUN_0041f2c0(id, seg, &ln, &hit) != 0) {
        dir.x = ln.end.x - ln.start.x;
        dir.y = ln.end.y - ln.start.y;
        dir.z = ln.end.z - ln.start.z;
        len = FUN_004c3ac0(&dir);
        FUN_004c39b0(&dir, &dir);
        s = hit.t * len;
        d = ln.start;
        d.x = dir.x * s + d.x;
        u.cand[bi].pos.x = d.x;
        d.y = dir.y * s + d.y;
        u.cand[bi].pos.y = d.y;
        d.z = dir.z * s + d.z;
        u.cand[bi].pos.z = d.z;
      }
      slot->f08 = u.cand[bi].idx;
      slot->f24 = u.cand[bi].pos;
      slot->f3c = u.cand[bi].pos;
      slot->f1c = u.cand[bi].dist;
      slot->f14 = 1;
    }
  } else {
    slot->f1c = slot->f18;
    slot->f08 = -1;
    memset(&ln, 0, sizeof(ln));
    d = slot->mat->at;
    tx = d.x * param_3;
    ty = param_3 * 0.0f;
    tz = d.z * param_3;
    ln.start.x = tx + param_2->x;
    ln.start.y = ty + param_2->y;
    ln.start.z = tz + param_2->z;
    ln.end = ln.start;
    ln.start.y = ln.start.y + 1.0f;
    ln.end.y = ln.end.y - 1.0f;
    if (FUN_004b4cd0(FUN_0045bfe0(&ln, &u.hit0)) != 0) {
      FUN_004b4650(&d, &ln.start, &ln.end, u.hit0.t);
      d.x = u.hit0.n.x * 0.02f + d.x;
      d.y = u.hit0.n.y * 0.02f + d.y;
      d.z = u.hit0.n.z * 0.02f + d.z;
      slot->f14 = 1;
      slot->f3c = d;
    } else {
      d = slot->mat->at;
      tx = d.x * param_3;
      ty = d.y * param_3;
      tz = d.z * param_3;
      slot->f3c.x = tx + param_2->x;
      slot->f3c.y = ty + param_2->y;
      slot->f3c.z = tz + param_2->z;
      slot->f14 = 0;
    }
    slot->f24 = slot->f3c;
  }
  ln.start = slot->f48;
  ln.end = slot->f3c;
  if (FUN_004b4cd0(FUN_0045bfe0(&ln, &hit)) != 0) {
    t = hit.t;
    pos.p.x = ln.end.x - ln.start.x;
    pos.p.y = ln.end.y - ln.start.y;
    pos.p.z = ln.end.z - ln.start.z;
    len = FUN_004c3ac0(&pos.p);
    FUN_004b4650(&d, &ln.start, &ln.end, t);
    slot->f14 = 1;
    slot->f08 = -1;
    d.x = hit.n.x * 0.02f + d.x;
    d.y = hit.n.y * 0.02f + d.y;
    d.z = hit.n.z * 0.02f + d.z;
    slot->f3c = d;
    slot->f24 = d;
    slot->f1c = len * t;
  }
  cnt = FUN_0047ce70();
  r2 = param_3 * param_3;
  pos.p.x = ln.end.x - ln.start.x;
  pos.p.y = ln.end.y - ln.start.y;
  pos.p.z = ln.end.z - ln.start.z;
  bi = -1;
  bestd = 100000.0f;
  dir.x = 0.0f;
  dir.y = 0.0f;
  dir.z = 0.0f;
  FUN_004c39b0(&pos.p, &pos.p);
  for (i = 0; i < cnt; i++) {
    e = FUN_0047d130(i);
    g = FUN_0057c210(e);
    if ((e[2] & 4) != 0 && g->f24 != 0 &&
        (g->f24->bits[g->f20 >> 5] & (1 << (g->f20 & 0x1f))) != 0) {
      a = g->f10[0];
      b = g->f10[2];
      FUN_0055bb70(a, b, &c);
      d.x = c.x - slot->f48.x;
      d.y = c.y - slot->f48.y;
      d.z = c.z - slot->f48.z;
      if (d.x * d.x + d.y * d.y + d.z * d.z < r2) {
        FUN_004c39b0(&d, &d);
        if (d.x * pos.p.x + d.y * pos.p.y + d.z * pos.p.z > 0.7f &&
            FUN_0055bde0(a, b, &ln.start, &ln.end, &u.pt) != 0) {
          dd = (u.pt.p.x - ln.start.x) * (u.pt.p.x - ln.start.x) + (u.pt.p.y - ln.start.y) * (u.pt.p.y - ln.start.y) +
               (u.pt.p.z - ln.start.z) * (u.pt.p.z - ln.start.z);
          if (dd < bestd) {
            bestd = dd;
            dir = u.pt.p;
            bi = i;
          }
        }
      }
    }
  }
  if (bi != -1 && bestd > 1e-06f) {
    slot->f1c = FUN_004c3b30(bestd);
    slot->f3c = dir;
    slot->f24 = dir;
    slot->f14 = 1;
    slot->f08 = -1;
    slot->f0c = bi;
  }
  {
    int frame = *(int *)(slot->clump + 4);
    FUN_004c15c0(frame);
    FUN_004c1340(frame, &slot->f48, 0);
    FUN_004b42c0(frame, &slot->f3c, &slot->mat->up);
    *slot->mat = *(Mat_459620 *)(frame + 0x10);
  }
}
