// FLAGS: /O2 /GS /Ob1
/* TL-MATCH-479330 (by hand, 2026-11-10): 97.2% (regs 99.5%, ops 99.6%), 826/825 ins. Raw g2s 35.6%.
   Shape: typed Course (param_1), CourseNames (param_2, 0x40-byte names) and CourseDef (param_3, float fog/amb
   fields: fld, not the raw fild) records, one index loop per name table, per-branch printf tails, switch on the
   light type byte, HELD static copy of the register-arg 0x004790e0 (world in EAX). Moves that scored: typed records
   35.6 -> 50.5%; uva out-params at function scope (frame 0x444) 53.5%; flag store as if/else + one `return 1`
   (fixes a whole-function esi/edi/ebx rotation) 91.9%; count + FUN_00485460 buffer in one block struct 96.0%;
   `buf += 0x40` in the for header 97.2%. Residual: instance loop 0x00479c40..0x00479d59 only. The original keeps
   count (edi) and buf (esi) in registers until the inner-loop entry and stores them there; here s.count is
   address-taken (reloaded after the calls) and buf is stored at once. A plain `int count` gets that register
   form but its home leaves the lights/tmp block slot (frame 0x448, 92.8%). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_005ea098, 0x005ea098);
MGLOBAL(int, DAT_00636ad4, 0x00636ad4);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_006132dc, 0x006132dc);
MGLOBAL(undefined1, DAT_006132ec, 0x006132ec);
MGLOBAL(void *, PTR_s_UVAnim_00623df0, 0x00623df0);
int FUN_004a2cbd(const char *p0, ...);
int FUN_0042a5d0(char *p0, int p1, int p2);
int FUN_0042a640(char *p0, int p1, int p2);
int FUN_0042a6b0(char *p0, int p1, int p2);
int FUN_0042a740(char *p0, void *p1, void *p2);
int FUN_0042a7f0(char *p0, int p1, int p2);
int FUN_0042a860(char *p0, int p1, int p2);
int FUN_00474fd0(int p0);
int FUN_00478110(char *p0, int p1);
void FUN_00478200(int p0);
void FUN_00478cc0(int p0);
void FUN_004790e0(void);
int FUN_004850e0(int p0, void *p1, void *p2, int p3);
void FUN_00485460(int p0, void *p1);
void FUN_004987b0(const char *p0, ...);
int FUN_004b3f90(int p0);
void FUN_004b4000(int p0);
int FUN_004b4010(int p0, int *p1);
int FUN_004b41f0(int p0, int *p1);
int FUN_004b4280(int p0, int *p1);
void FUN_004c0740(int p0, int p1);
int FUN_004c0b30(void);
int FUN_004c1040(int p0, int p1);
int FUN_004c1340(int p0, void *p1, int p2);
int FUN_004c1480(int p0, int p1, int p2);
int FUN_004c1520(int p0, void *p1, float p2, int p3);
int FUN_004c5c00(int p0, char *p1);
void FUN_004c5c80(int p0);
int FUN_004e4900(int p0, void *p1);
int FUN_004e6650(int p0);
int FUN_004e69a0(int p0);
void FUN_004e6f80(int p0, int p1);
int FUN_004e7e30(int p0, int p1);
int FUN_00538a80(int p0);
int FUN_0053fbb0(int p0, int p1);
void FUN_00558ca0(void *p0, int p1);
int FUN_004e4dd0(int p0);
int FUN_004e4810(int p0, int p1);
int FUN_004e4450(int p0, int p1);
int FUN_004e5c70(int p0, int (*p1)(int), int p2);
int FUN_00479030(int p0);

typedef struct { float x, y, z; } V3;
typedef struct { signed char clump; signed char mts; short anim; } CourseInst;   /* 4 B, desc +0x2c */

/* param_3 (int-indexed in the raw source) */
typedef struct {
  int type;              /* 0x000 */
  int nOther;            /* 0x004 */
  int nClumps;           /* 0x008 */
  int pad00c;            /* 0x00c */
  int nSplines;          /* 0x010 */
  int pad014;            /* 0x014 */
  int nAidata;           /* 0x018 */
  int nColl;             /* 0x01c */
  int nHoppy;            /* 0x020 */
  int pad024[2];         /* 0x024 */
  CourseInst inst[16];   /* 0x02c */
  int nInst;             /* 0x06c */
  int pad070[25];        /* 0x070 */
  int noWorld[64];       /* 0x0d4 */
  int pad1d4;            /* 0x1d4 */
  V3 amb;                /* 0x1d8 */
  int pad1e4;            /* 0x1e4 */
  float fogA[3];         /* 0x1e8 */
  float fogB[3];         /* 0x1f4 */
} CourseDef;

/* param_2: 0x40-byte names */
typedef struct {
  char texdic[0x40];     /* 0x0000 */
  char sky[4][0x40];     /* 0x0040 */
  char world[0x40];      /* 0x0140 */
  char coll[4][0x40];    /* 0x0180 */
  char hoppy[4][0x40];   /* 0x0280 */
  char other[16][0x40];  /* 0x0380 */
  char aidata[8][0x40];  /* 0x0780 */
  char clump[64][0x40];  /* 0x0980 */
  char extra[0x40];      /* 0x1980 */
  char mts[16][0x40];    /* 0x19c0 */
  char pad1dc0[0x40];    /* 0x1dc0 */
  char spline[16][0x40]; /* 0x1e00 */
  char camPath[0x40];    /* 0x2200 */
  char anim[8][0x40];    /* 0x2240 */
  char pad2440[8][0x40]; /* 0x2440 */
  char lights[0x40];     /* 0x2640 */
  char tex1[0x20];       /* 0x2680 */
  char tex2[0x20];       /* 0x26a0 */
  char uva[8][0x40];     /* 0x26c0 */
} CourseNames;

/* param_1 */
typedef struct {
  char mtsBuf[16][0x1000]; /* 0x00000 */
  int mts[16];             /* 0x10000 */
  int other[16];           /* 0x10040 */
  int aidata[4];           /* 0x10080 */
  int coll[4];             /* 0x10090 */
  int hoppy[4];            /* 0x100a0 */
  int spline[16];          /* 0x100b0 */
  int sky[4];              /* 0x100f0 */
  int pad10100[4];         /* 0x10100 */
  int clump[64];           /* 0x10110 */
  int skyX[4];             /* 0x10210 */
  int pad10220[4];         /* 0x10220 */
  int clumpX[64];          /* 0x10230 */
  int otherOn[16];         /* 0x10330 */
  int pad10370[16];        /* 0x10370 */
  int clumpOn[64];         /* 0x103b0 */
  int clumpFlag[64];       /* 0x104b0 */
  int anim[8];             /* 0x105b0 */
  int texdic;              /* 0x105d0 */
  int world;               /* 0x105d4 */
  int extra;               /* 0x105d8 */
  int lights;              /* 0x105dc */
  int light2;              /* 0x105e0 */
  int light1;              /* 0x105e4 */
  int usePvs;              /* 0x105e8 */
  int pvs;                 /* 0x105ec */
  int camPath;             /* 0x105f0 */
  float fogA[3];           /* 0x105f4 */
  float fogB[3];           /* 0x10600 */
  int pad1060c;            /* 0x1060c */
  int tex1;                /* 0x10610 */
  int tex2;                /* 0x10614 */
  int uva[8];              /* 0x10618 */
  int flag;                /* 0x10638 */
} Course;

#ifndef MATCH_PORT
// MATCH-HELD: 0x004790e0 (body in re/match/s12/004790e0.c; register-arg, world in EAX; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
static void FUN_004790e0_h(int world)
{
  FUN_004e5c70(world, FUN_00479030, 0);
}
#define FUN_004790e0_CALL(w) FUN_004790e0_h(w)
#else
#define FUN_004790e0_CALL(w) FUN_004790e0()
#endif

// MATCH: 0x00479330 FUN_00479330
int FUN_00479330(Course *c, CourseNames *nm, CourseDef *cd)
{
  int i;
  int ntex;
  int nwtex;
  int nl;
  int type;
  int h;
  V3 v;
  int uvaVer;
  int uvaLen;
  int tex[256];

  FUN_004987b0("Creating course from description\n");
  FUN_004987b0("\tResetting Course vars to zero\n");
  FUN_00478cc0((int)c);
  FUN_004987b0("\tLoading texdic [%s]\n", nm->texdic);
  h = FUN_0042a6b0(nm->texdic, 0, 0);
  c->texdic = h;
  if (h != 0) {
    FUN_004987b0("\tLoaded course texture dictionary successfully\n");
  } else {
    FUN_004987b0("failed to load texture dictionary\n");
  }
  if (DAT_005ea098 != 0 && c->texdic != 0) {
    ntex = FUN_004b4280(c->texdic, tex);
    for (i = 0; i < ntex; i++) {
      if (*(char *)(tex[i] + 0x50) == 4) {
        *(char *)(tex[i] + 0x50) = 6;
      }
    }
  }
  if (c->texdic != 0) {
    FUN_004c5c80(c->texdic);
  }
  FUN_004987b0("\tLoading world [%s]\n", nm->world);
  h = FUN_0042a640(nm->world, 0, 0);
  c->world = h;
  if (h != 0) {
    FUN_004987b0("\tLoaded bsp successfully\n");
  } else {
    FUN_004987b0("failed to load bsp\n");
  }
  if (c->world == 0) {
    FUN_004987b0("\nCOURSES ERROR: loading world\n");
    FUN_00478200(0);
  }
  if (DAT_00636ad4 != 0) {
    nwtex = FUN_004b41f0(c->world, tex);
    for (i = 0; i < nwtex; i++) {
      FUN_0053fbb0(tex[i], 0);
    }
  }
  FUN_004987b0("\tloading %d collision bsps\n", cd->nColl);
  for (i = 0; i < cd->nColl; i++) {
    if (nm->coll[i][0] != 0) {
      FUN_004987b0("\t\tLoading collision bsp [%s]\n", nm->coll[i]);
      h = FUN_0042a640(nm->coll[i], 0, 0);
      c->coll[i] = h;
      if (h != 0) {
        *(uint *)(h + 8) &= ~0x40u;
        FUN_004987b0("\t\tcollision %d loaded succsessfully\n", i);
      } else {
        FUN_004987b0("failed to load collision %d\n", i);
        FUN_00478200(2);
      }
    } else {
      FUN_004987b0("\nCOURSES ERROR: no name for invisible collision %d\n", i);
    }
  }
  if (c->coll[1] == 0) {
    c->coll[1] = c->coll[0];
  }
  if (c->coll[2] == 0) {
    c->coll[2] = c->coll[0];
  }
  if (c->coll[3] == 0) {
    c->coll[3] = c->coll[0];
  }
  for (i = 0; i < cd->nHoppy; i++) {
    if (nm->hoppy[i][0] != 0) {
      c->hoppy[i] = FUN_0042a640(nm->hoppy[i], 0, 0);
    } else {
      FUN_004987b0("\nCOURSES ERROR: no name for hoppy collision %d\n", i);
    }
  }
  for (i = 0; i < cd->nOther; i++) {
    if (nm->other[i][0] != 0) {
      h = FUN_0042a640(nm->other[i], 0, 0);
      c->other[i] = h;
      if (h != 0) {
        c->otherOn[i] = 1;
      }
    }
  }
  for (i = 0; i < cd->nAidata; i++) {
    if (nm->aidata[i][0] != 0) {
      c->aidata[i] = FUN_0042a640(nm->aidata[i], 0, 0);
    } else {
      FUN_004987b0("COURSES ERROR: no aidata name for aidata %d\n", i);
    }
    if (c->aidata[i] != 0) {
      FUN_004987b0("Ai data %d loaded\n", i);
    }
  }
  for (i = 0; i < 8; i++) {
    if (nm->uva[i][0] != 0) {
      FUN_004a2cbd("uva file exists\n");
      h = FUN_0042a740(nm->uva[i], &uvaLen, &uvaVer);
      c->uva[i] = h;
      if (h != 0) {
        FUN_00558ca0(&PTR_s_UVAnim_00623df0, h);
        FUN_004a2cbd("Loaded uva dict length (%d), version (%d)\n", uvaLen, uvaVer);
      }
    }
  }
  FUN_004987b0("Loading %d clumps for the world\n", cd->nClumps);
  for (i = 0; i < 64; i++) {
    if (nm->clump[i][0] != 0) {
      FUN_004987b0("LOADING Clump [%s] with index of [%d]\n", nm->clump[i], i);
      h = FUN_0042a5d0(nm->clump[i], 0, 0);
      c->clump[i] = h;
      if (h == 0) {
        FUN_004987b0("COURSES ERROR: Cannot load dff model %s\n", nm->clump[i]);
        return 0;
      }
      c->clumpOn[i] = 1;
      c->clumpFlag[i] = 0;
    }
  }
  if (nm->extra[0] != 0) {
    c->extra = FUN_0042a5d0(nm->extra, 0, 0);
  }
  if (cd->fogA[1] != cd->fogA[2]) {
    c->fogA[1] = cd->fogA[1];
    c->fogA[2] = cd->fogA[2];
    c->fogA[0] = cd->fogA[0];
  }
  if (cd->fogB[1] != cd->fogB[2]) {
    c->fogB[1] = cd->fogB[1];
    c->fogB[2] = cd->fogB[2];
    c->fogB[0] = cd->fogB[0];
  }
  if (nm->lights[0] != 0) {
    int k;
    int a;
    int lights[8];

    h = FUN_0042a5d0(nm->lights, 0, 0);
    c->lights = h;
    nl = FUN_004e6650(h);
    FUN_004b4010(c->lights, lights);
    for (k = 0; k < nl; k++) {
      a = lights[k];
      switch (*(unsigned char *)(a + 1)) {
      case 1:
        c->light1 = a;
        break;
      case 2:
        c->light2 = a;
        break;
      }
      *(char *)(a + 2) = 1;
      FUN_004e4810(c->world, a);
    }
    if (cd->amb.x > 0.0f || cd->amb.y > 0.0f || cd->amb.z > 0.0f) {
      if (c->light2 == 0) {
        a = FUN_004e4dd0(2);
        c->light2 = a;
        *(char *)(a + 2) = 1;
        FUN_004e4810(c->world, c->light2);
      }
      FUN_004e4900(c->light2, &cd->amb);
    }
  } else {
    v.x = 1.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    h = FUN_004e4dd0(2);
    c->light2 = h;
    FUN_004e4900(h, &DAT_006132dc);
    *(char *)(c->light2 + 2) = 1;
    FUN_004e4810(c->world, c->light2);
    h = FUN_004e4dd0(1);
    c->light1 = h;
    FUN_004e4900(h, &DAT_006132ec);
    *(char *)(c->light1 + 2) = 1;
    FUN_004c0740(c->light1, FUN_004c0b30());
    FUN_004c1520(*(int *)(c->light1 + 4), &v, 60.0f, 0);
    FUN_004e4810(c->world, c->light1);
  }
  for (i = 0; i < 16; i++) {
    if (nm->mts[i][0] != 0) {
      FUN_004987b0("LOADING MTS [%s] with index of [%d]\n", nm->mts[i], i);
      c->mts[i] = FUN_00478110(nm->mts[i], (int)c->mtsBuf[i]);
    }
  }
  for (i = 0; i < 4; i++) {
    if (nm->sky[i][0] != 0) {
      FUN_004987b0("Loading SkyDome [%s] into sky[%d]\n", nm->sky[i], i);
      h = FUN_0042a5d0(nm->sky[i], 0, 0);
      c->sky[i] = h;
      c->skyX[i] = FUN_00474fd0(h);
    }
  }
  h = FUN_00538a80(c->world);
  c->pvs = h;
  if (h != 0) {
    FUN_004987b0("PVS is available\n");
    c->usePvs = 1;
    FUN_004987b0("Using PVS\n");
  } else {
    c->usePvs = 0;
    FUN_004987b0("PVS not available\n");
  }
  for (i = 0; i < 16; i++) {
    if (nm->spline[i][0] != 0) {
      h = FUN_0042a7f0(nm->spline[i], 0, 0);
      if (h == 0) {
        FUN_004987b0("COURSES ERROR: Cannot load spline %s\n", nm->spline[i]);
        cd->nSplines = 0;
        return 0;
      }
      c->spline[i] = h;
      FUN_004987b0("Spline [%s] loaded into course spline %d, numsplines is %d\n", nm->spline[i], i,
                   cd->nSplines);
    }
  }
  FUN_004987b0("Loading Camera path [%s]\n", nm->camPath);
  h = FUN_0042a860(nm->camPath, 0, 0);
  c->camPath = h;
  if (h != 0) {
    FUN_004987b0("Camera path loaded\n");
  } else {
    FUN_004987b0("cant load camera path\n");
  }
  for (i = 0; i < 8; i++) {
    if (nm->anim[i][0] != 0) {
      FUN_004987b0("\nLoading general anim [%s]\n", nm->anim[i]);
      h = FUN_0042a860(nm->anim[i], 0, 0);
      c->anim[i] = h;
      if (h != 0) {
        FUN_004987b0("general anim %d loaded\n", i);
      } else {
        FUN_004987b0("cant load general anim %d\n", i);
      }
    }
  }
  for (i = 0; i < cd->nInst; i++) {
    CourseInst *in;
    int clump;
    int atomic;
    int a;
    int frame;
    int j;
    char *buf;
    struct { int count; int pad; int m[6]; } s; /* count + FUN_00485460 output share one block slot (0x34..0x54 with lights) */

    in = &cd->inst[i];
    clump = c->clump[in->clump];
    atomic = FUN_004b3f90(clump);
    s.count = c->mts[in->mts];
    buf = c->mtsBuf[in->mts];
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    FUN_004b4000(clump);
    FUN_004c1340(*(int *)(clump + 4), &v, 0);
    for (j = 0; j < s.count; j++, buf += 0x40) {
      a = atomic;
      frame = *(int *)(atomic + 4);
      if (j != 0) {
        a = FUN_004e69a0(atomic);
        frame = FUN_004c0b30();
        FUN_004e7e30(a, frame);
        FUN_004e6f80(clump, a);
        FUN_004c1040(*(int *)(clump + 4), frame);
      }
      FUN_004c1480(frame, (int)buf, 0);
      if (in->anim != -1) {
        FUN_00485460(a, s.m);
        FUN_004850e0(a, &v, s.m, in->anim);
      }
    }
  }
  for (i = 0; i < 64; i++) {
    if (c->clump[i] != 0) {
      if (cd->noWorld[i] == 0) {
        FUN_004e4450(c->world, c->clump[i]);
      }
      c->clumpX[i] = FUN_00474fd0(c->clump[i]);
    }
  }
  if (nm->tex1[0] != 0) {
    c->tex1 = FUN_004c5c00(c->texdic, nm->tex1);
  }
  if (nm->tex2[0] != 0) {
    c->tex2 = FUN_004c5c00(c->texdic, nm->tex2);
  }
  FUN_004790e0_CALL(c->world);
  type = cd->type;
  if (type != 0 && type != 0x1a && type != 0x24) {
    c->flag = 1;
  } else {
    c->flag = 0;
  }
  return 1;
}
