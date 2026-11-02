// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047eb30 (h7): 92.8% (regs 93.0%, ops 95.5%), 488/484 ins. Raw 4.3%. VehicleCouplingBridge.
   Shape that moved the score: 4-entry 0xd04-byte car record array at 0x008815a0 indexed by i (the compiler turns
   it into the +0x9c8 cursor and the 0x885378 bound); source variables are reused as the original's frame shows
   (v = spring target then velocity, com = CoM then forward axis, d = linear then angular velocity, hd = velocity
   heading then heading delta); the m->pos self-copy through `sv` kept apart by the d.x store (a plain self-copy
   is eliminated); linear velocity as subtract-then-scale statements; acos result cast to float before the
   57.29578f multiply (fmul dword); dot product as (x + y) then + z (one expression is reassociated);
   heading wrap as `if (<0) do {} while (<0)` (gives the aligned loop head). FUN_0047ea40(FUN_0055deb0(world), dt):
   0055deb0 takes 1 arg. Ghidra's size stops before the `mov esp, ebp; pop ebp; ret` tail.
   Residual (b): the acos clamp. The original keeps `else if (dot > 1.0f)` (fcom/test 0x41) after the outer
   `dot < 1.0f`; this compiler threads that test away, so it is spelled `1.0f < dot` (fld/fcomp st(1), +4 bytes,
   which shifts later branch targets and loop padding). Tried: double compares (qword), ternary, inline clamp
   helper, k copy, nested ifs, outer spelling variants. Also small x87/int scheduling differences in the linear
   velocity store, the spring scale, the loop-2 negation, and `mov eax,[ecx+4]; lea esi,[eax+0x10]` vs
   `mov esi,[ecx+4]; add esi,0x10` for the frame LTM copy (frame struct forms tried). Permuter (1877 compiles)
   only found non-equivalent rewrites. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } Mtx;
typedef struct { int pad[4]; Mtx ltm; } Frame;
typedef struct { int pad; Frame *frame; } Atom;
typedef struct { V3 lin; int pad0; V3 ang; int pad1; } VelSlot;
typedef struct { int pad[2]; VelSlot *slots; } VelArr;
typedef struct { int pad[4]; VelArr *arr; } VelObj;
typedef struct { VelObj *obj; int idx; } VelRef;
typedef struct { int pad0; VelRef *ref; Mtx *mtx; } BodyData;
typedef struct { int pad[4]; BodyData *data; int pad14[4]; int owner; } Body;
typedef struct {
  int pad000[5];
  int flag;
  int pad018[72];
  V3 com;
  int pad144[545];
  V3 axis;
  int pad9d4[204];
} CarRec;
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(int, DAT_006cad48, 0x006cad48);
MGLOBAL(int, DAT_0086caa0, 0x0086caa0);
MGLOBAL(int, DAT_00773920, 0x00773920);
MGLOBAL(int, DAT_0080332c, 0x0080332c);
MGLOBAL(float, DAT_0061331c, 0x0061331c);
MGLOBAL(float, DAT_007f0f8c, 0x007f0f8c);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
MGLOBAL(V3, DAT_00614708, 0x00614708);
MGLOBALA(int, DAT_006c9a78, 4, 0x006c9a78);
MGLOBALA(CarRec, DAT_008815a0, 4, 0x008815a0);
int FUN_0040e340(void);
int FUN_0040e350(void);
float FUN_004233e0(float p0, float p1);
int FUN_00426060(void);
void FUN_0046cb30(V3 *p0, int p1);
void FUN_0046d4a0(Mtx **p0, int p1);
void FUN_0046d4d0(int p0, Mtx *p1);
void FUN_0046d510(V3 *p0, int p1);
void FUN_0047d3c0(int p0, int p1);
int FUN_0047ea40(int p0, float p1);
double FUN_004a3384(double p0);
void FUN_004c0e50(int p0);
void FUN_004c3df0(V3 *p0, V3 *p1, int p2, Mtx *p3);
void FUN_004c45f0(Mtx *p0);
void FUN_0055ac00(int p0, Body *p1, int p2);
int FUN_0055deb0(int p0);
void FUN_0055dff0(Body *p0, int p1);
void FUN_00562460(int p0);
Body *FUN_0057c210(int p0);
// MATCH: 0x0047eb30 FUN_0047eb30
int FUN_0047eb30(void)
{
  int built;
  int i;
  int gm;
  int result;
  Body *body;
  VelRef *ref;
  Mtx *m;
  Mtx *rm;
  float hd;
  float dot;
  float k;
  V3 v;
  V3 com;
  V3 d;
  V3 axis;
  V3 c;
  V3 t;
  V3 t2;
  V3 sv;
  Mtx buf;

  if (DAT_006ce274 == 0) {
    return 0;
  }
  if (FUN_0040e350() != 6) {
    DAT_006cad48 = 0;
  }
  if (DAT_0086caa0 == 0x7b) {
    built = FUN_00426060();
    if ((DAT_00773920 == 1) || ((DAT_00773920 > 4) && (DAT_00773920 <= 6))) {
      FUN_00562460(DAT_0080332c);
    }
    FUN_0047d3c0(DAT_006ce274, built);
    DAT_0086caa0 = 0;
  }
  if (DAT_0061331c > 0.05f) {
    DAT_0061331c = 0.05f;
  }
  DAT_006cad48++;
  DAT_007f0f8c = (float)(FUN_0040e340() + 5000);
  for (i = 0; i < 4; i++) {
    if ((DAT_008815a0[i].flag == 0) && (i < FUN_0040e340())) {
      FUN_0046d4a0(&rm, i);
      body = FUN_0057c210(DAT_006c9a78[i]);
      ref = body->data->ref;
      m = body->data->mtx;
      if (ref != 0) {
        FUN_0046cb30(&v, i);
        v.x = v.x * 0.0002f;
        v.y = v.y * 0.0002f;
        v.z = v.z * 0.0002f;
        v.x = v.x + rm->pos.x;
        v.y = v.y + rm->pos.y;
        v.z = v.z + rm->pos.z;
        com = DAT_008815a0[i].com;
        FUN_004c3df0(&com, &com, 1, m);
        d.x = v.x - (com.x + m->pos.x);
        d.y = v.y - (com.y + m->pos.y);
        d.z = v.z - (com.z + m->pos.z);
        d.x = d.x * 20.0f;
        d.y = d.y * 20.0f;
        d.z = d.z * 20.0f;
        ref->obj->arr->slots[ref->idx].lin = d;
        FUN_0046d510(&v, i);
        hd = FUN_004233e0(v.x, v.z);
        FUN_004c3df0(&com, &DAT_00614708, 1, m);
        hd = (FUN_004233e0(com.x, com.z) - hd) + 180.0f;
        if (hd < 0.0f) {
          do {
            hd = hd + 360.0f;
          } while (hd < 0.0f);
        }
        if (hd > 180.0f) {
          hd = -(360.0f - hd);
        }
        if (hd > 0.0f) {
          hd = 180.0f - hd;
        } else {
          hd = -(hd + 180.0f);
        }
        d.x = 0.0f;
        d.y = hd;
        d.z = 0.0f;
        FUN_004c3df0(&axis, &DAT_006146fc, 1, m);
        c.x = axis.y * DAT_008815a0[i].axis.z - axis.z * DAT_008815a0[i].axis.y;
        c.y = axis.z * DAT_008815a0[i].axis.x - axis.x * DAT_008815a0[i].axis.z;
        c.z = axis.x * DAT_008815a0[i].axis.y - axis.y * DAT_008815a0[i].axis.x;
        dot = axis.x * DAT_008815a0[i].axis.x + axis.y * DAT_008815a0[i].axis.y;
        dot = dot + axis.z * DAT_008815a0[i].axis.z;
        if (((c.x != 0.0f) || (c.y != 0.0f) || (c.z != 0.0f)) && (dot < 1.0f)) {
          if (dot < -1.0f) {
            dot = -1.0f;
          } else if (1.0f < dot) {
            dot = 1.0f;
          }
          k = (float)FUN_004a3384(dot) * 57.29578f;
          sv = m->pos;
          d.x = c.x * k;
          m->pos = sv;
          d.y = c.y * k + hd;
          d.z = k * c.z;
        }
        ref->obj->arr->slots[ref->idx].ang = d;
        FUN_004c45f0(m);
        FUN_004c0e50(*(int *)(DAT_006c9a78[i] + 4));
        FUN_0055dff0(body, 0);
        FUN_0055ac00(body->owner, body, 1);
      }
      gm = FUN_0040e350();
      if ((gm == 3) || (gm == 5)) {
        t = m->pos;
        *m = *rm;
        t2 = DAT_008815a0[i].com;
        FUN_004c3df0(&t2, &t2, 1, m);
        m->pos.x = m->pos.x - t2.x;
        m->pos.y = m->pos.y - t2.y;
        m->pos.z = m->pos.z - t2.z;
        FUN_004c45f0(m);
        FUN_004c0e50(*(int *)(DAT_006c9a78[i] + 4));
        FUN_0055dff0(body, 0);
        FUN_0055ac00(body->owner, body, 1);
      }
    }
    if (i >= FUN_0040e340()) {
      body = FUN_0057c210(DAT_006c9a78[i]);
      m = body->data->mtx;
      *(int *)&m->pos.x = 0;
      *(int *)&m->pos.y = 0xc61c4000;
      *(int *)&m->pos.z = 0;
      FUN_004c45f0(m);
      FUN_004c0e50(*(int *)(DAT_006c9a78[i] + 4));
      FUN_0055dff0(body, 0);
      FUN_0055ac00(body->owner, body, 0);
    }
  }
  result = FUN_0047ea40(FUN_0055deb0(DAT_006ce274), DAT_0061331c);
  for (i = 0; i < 4; i++) {
    if (DAT_008815a0[i].flag != 0) {
      buf = ((Atom *)DAT_006c9a78[i])->frame->ltm;
      t = DAT_008815a0[i].com;
      t.x = t.x * -1.0f;
      t.y = t.y * -1.0f;
      t.z = t.z * -1.0f;
      FUN_004c3df0(&t, &t, 1, &buf);
      buf.pos.x = buf.pos.x - t.x;
      buf.pos.y = buf.pos.y - t.y;
      buf.pos.z = buf.pos.z - t.z;
      FUN_0046d4d0(i, &buf);
      FUN_0057c210(DAT_006c9a78[i]);
    }
  }
  return result != 0;
}
