// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420420 by hand (h2): best 79.8% (regs 93.4%, ops 99.4%), 317/317 ins. Raw g2s 49.8%.
   The file holds three file-`static` callees (pilot finding 1), each at 100% here: FUN_0041f330 (id in ESI, same body
   as re/match/s04/0041f330.c), FUN_0041f360 (same body as re/match/s04/0041f360.c) and FUN_0041fe10 (record in EAX;
   h1's slice function, written here from the disassembly and byte-identical, 117/117 ins; its float test is the
   literal chain ((((a==b)==c)==d)==e)==f the code computes).
   Moved it: PSlot record (0x0063d9e0, 0x2ac B: atomics by part id +0x000, spheres +0x248/+0x258, car entry +0x268,
   clump +0x26c, atomic count +0x270, ..., +0x2a0..+0x2a8), Atomic/Geom/Mat views, two separate RGBA locals (black
   for parts 0x3b..0x3e, white for 0x11/0x15/0x18; one shared local gave a 0x264 frame), unsigned file-exists slot
   +0xf4 compared `> 0` (the original's jbe). Strings via MGLOBALA: ".dff" 0x005cd29c, ".txd" 0x005cd2a4, "" 0x005cc4de.
   Residual: register allocation only. The original keeps param_1 in EBX and 0 in ESI (reloads param_1 into ESI for
   the FUN_0041f330 call), and in the atomic loop the atomic in EBP and its geometry in ESI; ours swaps both pairs.
   Tried: declaration orders, statement orders for a/g/v48, ent local vs s->ent, permuter (6 rounds, 2258 compiles,
   80.1% only by swapping two calls, rejected: it changes behaviour). Class (b). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float x, y, z, r; } Sph;
typedef struct { int u0; RGBA color; } Mat;
typedef struct { char pad[0x20]; Mat **mats; int nmat; } Geom;
typedef struct {
  char pad[0x18];
  Geom *geom;          /* +0x18 */
  Sph sph;             /* +0x1c */
  char pad2[0x1c];     /* +0x2c */
  int u48;             /* +0x48 */
  unsigned char flags; /* +0x4c */
} Atomic;
typedef struct {
  Atomic *ev[0x8c];    /* +0x000 */
  float v230[3];       /* +0x230 */
  float v23c[3];       /* +0x23c */
  Sph sph248;          /* +0x248 */
  Sph sph258;          /* +0x258 */
  char *ent;           /* +0x268 */
  int obj;             /* +0x26c */
  int natom;           /* +0x270 */
  float f274;          /* +0x274 */
  int u278;            /* +0x278 */
  int u27c;            /* +0x27c */
  int u280;            /* +0x280 */
  float f284;          /* +0x284 */
  int u288[2];         /* +0x288 */
  int slot;            /* +0x290 */
  unsigned int flags;  /* +0x294 */
  int u298;            /* +0x298 */
  char *car;           /* +0x29c */
  int *u2a0;           /* +0x2a0 */
  int u2a4;            /* +0x2a4 */
  int u2a8;            /* +0x2a8 */
} PSlot;
typedef struct { char name[0x80]; int id; } NamedEntry;
typedef struct {
  char pad[0xcc];
  void (*strcpy_)(char *, const char *);  /* +0xcc */
  int pad2;
  void (*strcat_)(char *, const char *);  /* +0xd4 */
  int pad3[7];
  unsigned int (*fexist)(const char *);   /* +0xf4 */
} RwGlobalsLike;
MGLOBAL(int, DAT_005f5fe0, 0x005f5fe0);
MGLOBALA(NamedEntry, s_Shuriken_005f37a8, 78, 0x005f37a8);
MGLOBAL(int, DAT_00636ad0, 0x00636ad0);
MGLOBAL(RwGlobalsLike *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
MGLOBAL(int, DAT_0063d7e8, 0x0063d7e8);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(int *, DAT_0063e490, 0x0063e490);
MGLOBAL(int, DAT_0063d854, 0x0063d854);
MGLOBAL(int, DAT_0063d850, 0x0063d850);
MGLOBAL(int, DAT_0063d7ec, 0x0063d7ec);
MGLOBAL(int, DAT_0063e498, 0x0063e498);
MGLOBALA(int, DAT_0063d840, 4, 0x0063d840);
MGLOBALA(int, DAT_0063d830, 4, 0x0063d830);
MGLOBALA(char, DAT_005cd29c, 5, 0x005cd29c);
MGLOBALA(char, DAT_005cd2a4, 5, 0x005cd2a4);
MGLOBALA(char, DAT_005cc4de, 1, 0x005cc4de);
void FUN_004b6520(void *p, int n);
int FUN_0042a6b0(char *name, int a, int b);
void FUN_004c5c80(int txd);
int FUN_0042a5d0(char *name, int a, int b);
int FUN_004b3fc0(int clump, Atomic **out);
void FUN_004c15c0(int v);
void FUN_00420290(int slot, float t);
int FUN_004c5c00(int txd, char *name);
int FUN_00543b90(Geom *g, char *name, int fmt, int n);
int FUN_00543d70(Geom *g, int idx);
void FUN_00543e10(int arr, int idx, int v);
int FUN_004b5190(Atomic *a, int b, int c);
void FUN_0053fb70(Atomic *a);
void FUN_0053fbb0(Mat *m, int v);
void FUN_0053fd30(Mat *m, int a, int b, int c, float d);
void FUN_004b52c0(Geom *g, int flag, int on);
void FUN_005449f0(Geom *g, int v);
void FUN_00544a70(Geom *g, int a, int b, int c, float d);
void FUN_00544ad0(Atomic *a, int v);
void FUN_0053d090(Geom *g);
void FUN_0053d400(Geom *g, int v);
void FUN_004224d0(int slot, int *cols, int *p);
int FUN_004260b0(void);
int FUN_004260a0(void);
int FUN_004e6ab0(int obj);
void FUN_004e6fb0(int clump, Atomic *a);
void FUN_004e6920(Atomic *a);
void FUN_004223b0(float *v, int clump);
void FUN_004e6e00(int clump);
void FUN_004e5fc0(Atomic *a);
// MATCH: 0x0041f330 FUN_0041f330
static char *FUN_0041f330(int id)
{
  int i;
  for (i = 0; i < DAT_005f5fe0; i++) {
    if (s_Shuriken_005f37a8[i].id == id)
      return s_Shuriken_005f37a8[i].name;
  }
  return 0;
}
// MATCH: 0x0041f360 FUN_0041f360
static int FUN_0041f360(int param_1)
{
  if (DAT_00636ad0 == 0) {
    switch (param_1) {
    case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf:
    case 0x25: case 0x26: case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30:
      return 1;
    }
  }
  return 0;
}
// MATCH: 0x0041fe10 FUN_0041fe10
static void FUN_0041fe10(PSlot *s)
{
  int clump;
  int n, i;
  Atomic *a;
  Atomic *atoms[128];
  clump = FUN_004e6ab0(s->obj);
  n = FUN_004b3fc0(clump, atoms);
  for (i = 0; i < n; i++) {
    a = atoms[i];
    switch (FUN_004b5190(a, 0, 0)) {
    case 4: case 6: case 8: case 9: case 10: case 0xc:
      break;
    default:
      FUN_004e6fb0(clump, a);
      FUN_004e6920(a);
      atoms[i] = 0;
    }
  }
  if ((((((s->v23c[0] == s->v23c[1]) == s->v23c[2]) == s->v230[0]) == s->v230[1]) == s->v230[2]))
    FUN_004223b0(s->v230, clump);
  FUN_004e6e00(clump);
  if (s->ev[0x3b]->flags & 2)
    FUN_004e5fc0(s->ev[0x3b]);
  s->sph248 = s->ev[0x3b]->sph;
  s->sph258 = s->ev[0x3b]->sph;
}
// MATCH: 0x00420420 FUN_00420420
void FUN_00420420(int param_1)
{
  PSlot *s;
  char *ent;
  int i, j, id, ud, v48;
  Atomic *a;
  Geom *g;
  Mat *m;
  RGBA black;
  RGBA white;
  int cols[4];
  char buf[64];
  Atomic *atoms[128];
  s = &DAT_0063d9e0[param_1];
  FUN_004b6520(s, 0x2ac);
  ent = FUN_0041f330(param_1);
  s->ent = ent;
  s->slot = param_1;
  s->u298 = 0;
  if (DAT_0063d7e8 == 0) {
    DAT_007d3ff8->strcpy_(buf, ent + 0x20);
    DAT_007d3ff8->strcat_(buf, DAT_005cd2a4);
    DAT_0063d7e8 = FUN_0042a6b0(s->ent + 0x20, 0, 0);
  }
  FUN_004c5c80(DAT_0063d7e8);
  DAT_007d3ff8->strcpy_(buf, s->ent + 0x10);
  DAT_007d3ff8->strcat_(buf, DAT_005cd29c);
  s->obj = FUN_0042a5d0(s->ent + 0x10, 0, 0);
  s->natom = FUN_004b3fc0(s->obj, atoms);
  FUN_004c15c0(DAT_0063d840[param_1]);
  s->u27c = DAT_0063d840[param_1];
  s->flags |= 4;
  FUN_00420290(param_1, 1.0f);
  s->u280 = 0;
  if (DAT_007d3ff8->fexist(s->ent + 0x30) > 0)    s->u278 = FUN_004c5c00(DAT_0063d7e8, s->ent + 0x30);
  for (i = 0; i < s->natom; i++) {
    a = atoms[i];
    g = a->geom;
    v48 = a->u48;
    ud = FUN_00543d70(g, FUN_00543b90(g, DAT_005cc4de, 1, 2));
    FUN_00543e10(ud, 0, (int)s);
    FUN_00543e10(ud, 1, v48);
    id = FUN_004b5190(a, 0, 0);
    s->ev[id] = a;
    if (FUN_0041f360(id) != 0) {
      FUN_0053fb70(a);
      for (j = 0; j < g->nmat; j++) {
        m = g->mats[j];
        FUN_0053fbb0(m, 2);
        FUN_0053fd30(m, s->u278, s->u27c, 0, s->f274);
      }
    }
    if (id >= 0x3b && id <= 0x3e) {
      black.r = 0; black.g = 0; black.b = 0; black.a = 0xff;
      FUN_004b52c0(g, 0x20, 0);
      FUN_004b52c0(g, 0x40, 1);
      g->mats[0]->color = black;
    }
    if (id == 0x11 || id == 0x15 || id == 0x18) {
      white.r = 0xff; white.g = 0xff; white.b = 0xff; white.a = 0xff;
      FUN_004b52c0(g, 0x20, 0);
      FUN_004b52c0(g, 0x40, 1);
      g->mats[0]->color = white;
    }
    if (id == 0x12) {
      FUN_005449f0(g, 1);
      FUN_00544a70(g, 0, 0, 1, 1.0f);
      FUN_00544ad0(a, 0);
    } else if (id >= 0x3b && (id <= 0x3e || id == 0x40)) {
      FUN_0053d090(a->geom);
      FUN_0053d400(a->geom, 0);
    }
  }
  FUN_0041fe10(s);
  cols[0] = FUN_004c5c00(DAT_0063d7e8, s->ent + 0x40);
  cols[1] = FUN_004c5c00(DAT_0063d7e8, s->ent + 0x50);
  cols[2] = FUN_004c5c00(DAT_0063d7e8, s->ent + 0x60);
  cols[3] = FUN_004c5c00(DAT_0063d7e8, s->ent + 0x70);
  if (DAT_00636acc == 0) {
    s->u2a0 = DAT_0063e490;
    s->u2a4 = DAT_0063e490[1];
    s->u2a8 = DAT_0063d854;
    cols[0] = DAT_0063d854;
    cols[1] = DAT_0063d854;
    cols[2] = DAT_0063d854;
    cols[3] = DAT_0063d854;
    FUN_004224d0(param_1, cols, DAT_0063e490);
  }
  DAT_0063d850 = FUN_004260b0();
  DAT_0063d7ec = FUN_004260a0();
  DAT_0063e498++;
  DAT_0063d830[param_1] = 1;
  FUN_004c5c80(0);
}
