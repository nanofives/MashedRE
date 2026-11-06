// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459000: 92.7% (regs 92.7%, ops 92.7%, 191/191 ins) by hand (raw 19.7%). Per-frame update of
   the 25 records at 0x0068b198: count-down loop, `type == 0x15 -> state 0`, then a switch on state with
   `case 0: continue;` (gives the original's `sub eax, ebp` case-0 test). State 1 calls 0x00458a40 (record in
   EDI) and state 2 runs a fsin pulse. The file holds the static callees 0x00458a40 and 0x004584e0 (bodies from
   00458a40.c, 87.8% each; their register conventions are what the caller needs).
   0x005ce540/0x005ce544/0x005ce548 are separate .rdata floats (two equal 1.25f, so not literals): MGLOBAL;
   0x005cd0c8 (1.0 double), 0x005ce2f4 (pi) and 0x005cd7a8 (deg->rad) are literals.
   The clamp is a macro-shaped double expression: v = MAX(MIN(1.0 - t, 1.0), 0) with both MINs re-evaluated;
   `(1.0 - t) >= 1.0 ? 1.0 : (1.0 - t)` inside `M >= 0 ? M : 0` is the best block layout (81.7% -> 92.7%,
   tried 60+ spellings: comparison direction, !(), literal 0.0f/0.0, double temp).
   Residual (b): (1) the three clamp compares: the original loads the constant (fld D; fcomp st(1) and
   fcomp dword [0]), ours compares against memory (fcom qword [D]; fld 0; fxch; fcompp); (2) in state 1 the
   home-pos fsub chain is interleaved differently with the frame-pointer `add ebx, 0x40` (tried frame temp,
   statement positions). Permuter (917 compiles): only semantically wrong variants scored higher. */
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
typedef struct V3_00459000 {
  float x;
  float y;
  float z;
} V3_00459000;
typedef struct V2_00459000 {
  float x;
  float y;
} V2_00459000;
typedef struct Rec00459000 {
  int *clump;
  float angle;
  float spin;
  V3_00459000 axis;
  float f18;
  float f1c;
  int state;
  int type;
  int flag;
  V3_00459000 home;
  V3_00459000 pos;
  V3_00459000 vel;
} Rec00459000;
typedef struct Query00459000 {
  V3_00459000 center;
  float radius;
  int kind;
  Rec00459000 *rec;
  int zero;
  void (*cb)(int, int);
  undefined4 rest[28];
} Query00459000;
MGLOBAL(int, DAT_0068b9a8, 0x0068b9a8);
MGLOBALA(Rec00459000, DAT_0068b198, 25, 0x0068b198);
MGLOBALA(undefined1, DAT_0068b968, 8, 0x0068b968);
MGLOBALA(undefined4, DAT_005f9838, 32, 0x005f9838);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);
MGLOBAL(float, DAT_005cc990, 0x005cc990);
MGLOBAL(float, DAT_005cc9a0, 0x005cc9a0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_005ce540, 0x005ce540);
MGLOBAL(float, DAT_005ce544, 0x005ce544);
MGLOBAL(float, DAT_005ce548, 0x005ce548);
double sin(double);
#pragma intrinsic(sin)
float FUN_004c3ac0(V3_00459000 *p0);
float FUN_004c39b0(V3_00459000 *p0, V3_00459000 *p1);
void FUN_004c1340(int p0, V3_00459000 *p1, int p2);
void FUN_004c1520(int p0, V3_00459000 *p1, float p2, int p3);
int FUN_004576b0(void);
int FUN_00458d00(void);
void FUN_00458dd0(Rec00459000 *p0);
void FUN_00458920(int p0, int p1);
void FUN_004769f0(void *p0);
void FUN_004769a0(void *p0);
void FUN_004769d0(V2_00459000 *p0);
void FUN_00476a30(float p0);
void FUN_00476d00(void *p0);
void FUN_00476df0(void *p0);
void FUN_00484cf0(Query00459000 *p0);
// MATCH: 0x004584e0 FUN_004584e0
static float FUN_004584e0(Rec00459000 *rec, V3_00459000 *out)
{
  float len;
  V3_00459000 b;

  len = DAT_005d757c;
  b = *(V3_00459000 *)(rec->clump[1] + 0x40);
  out->x = rec->home.x - b.x;
  out->y = rec->home.y - b.y;
  out->z = rec->home.z - b.z;
  if (out->z * out->z + out->y * out->y + out->x * out->x > DAT_005ce54c) {
    len = FUN_004c3ac0(out);
    FUN_004c39b0(out, out);
  }
  return len;
}

// MATCH: 0x00458a40 FUN_00458a40
static void FUN_00458a40(Rec00459000 *rec)
{
  int frame;
  float s;
  V3_00459000 d;

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


// MATCH: 0x00459000 FUN_00459000
void FUN_00459000(void)
{
  int n;
  Rec00459000 *r;
  V3_00459000 *fpos;
  float t;
  float e;
  double s;
  float v;
  V2_00459000 sc;
  V2_00459000 sc2;
  Query00459000 q;
  float dx;
  float dy;
  float dz;

  n = DAT_0068b9a8;
  while (n != 0) {
    n--;
    r = &DAT_0068b198[n];
    if (r->type == 0x15) r->state = 0;
    switch (r->state) {
    case 0:
      continue;
    case 1:
      fpos = (V3_00459000 *)(r->clump[1] + 0x40);
      dx = r->home.x - r->pos.x;
      dy = r->home.y - r->pos.y;
      dz = r->home.z - r->pos.z;
      t = dy * dy;
      t += dx * dx;
      t += dz * dz;
      if (t > DAT_005d757c) {
        v = ((((1.0 - t) >= 1.0 ? 1.0 : (1.0 - t))) >= DAT_005d757c ? (((1.0 - t) >= 1.0 ? 1.0 : (1.0 - t))) : DAT_005d757c);
        sc.x = DAT_005ce540 * v;
        sc.y = DAT_005ce544 * v;
      } else {
        sc.x = DAT_005ce540;
        sc.y = DAT_005ce544;
      }
      FUN_00458a40(r);
      FUN_004769f0(&DAT_005f9838[r->type]);
      FUN_004769a0(fpos);
      FUN_004769d0(&sc);
      FUN_00476a30(r->angle * 0.017453292f);
      FUN_00476d00(DAT_0068b968);
      q.center = *fpos;
      q.radius = 0.5f;
      q.kind = 7;
      q.rec = r;
      q.zero = 0;
      q.cb = FUN_00458920;
      FUN_00484cf0(&q);
      break;
    case 2:
      if (r->f1c > DAT_005d757c) {
        e = r->f18 - r->f1c;
        r->f1c = r->f1c - DAT_007f100c;
        if (e < DAT_005ce548) {
          s = sin(e / DAT_005ce548 * 3.1415927f);
          sc2.x = (float)(DAT_005ce540 * s);
          sc2.y = (float)(DAT_005ce544 * s);
          FUN_004769f0(&DAT_005f9838[r->type]);
          FUN_004769a0(&r->pos);
          FUN_004769d0(&sc2);
          FUN_00476a30(0);
          FUN_00476d00(DAT_0068b968);
        }
      } else if (r->type != 0x13 || FUN_004576b0() != 1) {
        r->f1c = r->f18;
        r->state = 1;
        if (r->flag != 0) {
          r->type = FUN_00458d00();
          FUN_00458dd0(r);
        }
      }
      break;
    }
  }
  FUN_00476df0(DAT_0068b968);
}
