// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004850e0 (h2): 100% (raw 34.9%). Moved: typed structs for the clump (+4 frame, +0x1c 16-byte
   sphere copied whole to a local, +0x4c flags), the body record (16 dwords) and the 0x1c-byte table at
   0x006e70e8; matrix pointer m = &clump->frame->ltm taken first (ebp); box copied max then min; FUN_00485a70
   returns unsigned short (movzx). Table length 8 = (0x006e71c8 - 0x006e70e8) / 0x1c, from the next global's
   address only. */
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
typedef struct Vec3 { float x, y, z; } Vec3;
typedef struct Sphere { Vec3 center; float radius; } Sphere;
typedef struct Aabb { Vec3 min; Vec3 max; } Aabb;
typedef struct Matrix { Vec3 right; int flags; Vec3 up; int pad1; Vec3 at; int pad2; Vec3 pos; int pad3; } Matrix;
typedef struct Frame { char f00[0x10]; Matrix ltm; } Frame;
typedef struct Clump {
  int f00;
  Frame *frame;
  char f08[0x14];
  Sphere sphere;
  char f2c[0x20];
  unsigned char flags;
} Clump;
typedef struct BodyState {
  Vec3 pos;
  Vec3 vel;
  char f18[0x18];
  float f30;
  float f34;
} BodyState;
typedef struct Body {
  char f00[0x50];
  float f50;
  float f54;
  int f58;
  float f5c;
  BodyState *state;
} Body;
typedef struct BodyRec {
  Body *body;
  void *grid;
  Clump *clump;
  int handle;
  int type;
  int f14;
  int f18;
  int f1c;
  int f20;
  int f24;
  int f28;
  int f2c;
  int f30;
  int f34;
  int f38;
  int f3c;
} BodyRec;
typedef struct BodyType {
  float f00;
  float f04;
  float f08;
  float f0c;
  float f10;
  int f14;
  int f18;
} BodyType;
MGLOBAL(int, DAT_006e71c8, 0x006e71c8);
MGLOBAL(int, DAT_006e71cc, 0x006e71cc);
MGLOBALA(BodyType, DAT_006e70e8, 8, 0x006e70e8);
void FUN_004e5fc0(Clump *clump);
BodyRec *FUN_00485bd0(int pool);
Frame *FUN_004c0b30(void);
void FUN_004c15c0(Frame *frame);
void FUN_004e7e30(Clump *clump, Frame *frame);
void FUN_00485460(Clump *clump, Aabb *box);
void FUN_004c0c20(Frame *frame);
Body *FUN_00485a00(Clump *clump, Matrix *m, Body *body);
void *FUN_004858c0(Aabb *box, float radius, Matrix *m, void *grid);
unsigned short FUN_00485a70(int list, BodyRec *rec);
// MATCH: 0x004850e0 FUN_004850e0
int FUN_004850e0(Clump *clump, Vec3 *vel, Aabb *box, int type)
{
  Matrix *m;
  Sphere sphere;
  BodyRec *rec;
  Vec3 v;
  Aabb b;
  Frame *saved;
  Frame *tmp;
  unsigned short h;

  m = &clump->frame->ltm;
  if (clump->flags & 2) {
    FUN_004e5fc0(clump);
  }
  sphere = clump->sphere;
  rec = FUN_00485bd0(DAT_006e71c8);
  if (rec == 0) {
    return -1;
  }
  if (vel == 0) {
    v.z = 0;
    v.y = 0;
    v.x = 0;
  } else {
    v = *vel;
  }
  if (box == 0) {
    saved = clump->frame;
    tmp = FUN_004c0b30();
    FUN_004c15c0(tmp);
    FUN_004e7e30(clump, tmp);
    FUN_00485460(clump, &b);
    FUN_004e7e30(clump, saved);
    FUN_004c0c20(tmp);
  } else {
    b.max = box->max;
    b.min = box->min;
  }
  rec->f18 = 0;
  rec->f14 = 0;
  rec->f1c = 0;
  rec->f20 = 0;
  rec->clump = clump;
  rec->body = FUN_00485a00(clump, m, rec->body);
  rec->grid = FUN_004858c0(&b, sphere.radius, m, rec->grid);
  rec->f2c = 0;
  rec->f24 = 0;
  rec->f28 = 0;
  rec->f38 = 0;
  rec->f30 = 0;
  rec->f34 = 0;
  rec->f3c = 1;
  rec->body->state->vel = v;
  rec->body->state->pos = m->pos;
  rec->body->state->f30 = DAT_006e70e8[type].f00;
  rec->body->state->f34 = DAT_006e70e8[type].f04;
  rec->body->f50 = DAT_006e70e8[type].f08;
  rec->body->f54 = DAT_006e70e8[type].f0c;
  rec->body->f5c = DAT_006e70e8[type].f10;
  h = FUN_00485a70(DAT_006e71cc, rec);
  rec->type = type;
  rec->handle = h;
  return h;
}
