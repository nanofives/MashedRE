// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045ae80 (h10): best 80.7% (regs 80.7%, ops 94.3%), 886 B; raw g2s 23.9%.
   The file holds the three register-arg callees as file-static functions defined before the caller (pilot
   finding 1), so the call sites get the original conventions: 0x0045a530 (vec3 in EAX + 1 stack arg) and
   0x0045a590 (record in EAX), bodies copied verbatim from h9's re/match/s09/0045a530.c / 0045a590.c (100% here
   too), and 0x0045ac40 (EAX/ECX + 1 stack arg, the body of re/match/s09/0045ac40.c, 61.5% here too).
   Moved it: typed 0x68-byte records at 0x0068bb60 and the 4x5x5 0x34-byte spark grid at 0x0068bd00; the hit test
   FUN_004b4cd0(FUN_0045bfe0(), &line, &hit) (0x0045bfe0 takes no args); start/end as one Line struct; float
   literals for the pooled .rdata constants (1.0, 0.02, 0.002, 1.1111112, 235, 255, 0.4, 0.25, 1.5, 0.3, 15;
   DAT_007f100c is a real global); static callees (58.7% -> 69.3%); the record loop as a pointer loop with c
   counted separately (79.2%); permuter reorders of independent stores (80.7%).
   Residual (b): the original keeps the record pointer unbiased in EBX plus a spilled copy at [esp+0x34] (used for
   the closing 0x00489890 call) and tests both record loops on the pointer (cmp esi,0x0068bb64 / jne and
   cmp ebx,0x0068bd00 / jl); 9176 turns both into down-counters and biases EBX by +0x10, and without the spilled
   copy the k / b / any slots sit 4 bytes off (frame size 0xc8 and the other slots match). Tried
   index loops (k = 3..0, c = 0..3 with rec = &DAT[c]), rec++ in the for header and at the end of the body, and
   DAT[c].m_10 vs rec->m_10 in the inner and closing calls. */
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
typedef struct V3_45ae80 { float x, y, z; } V3_45ae80;
typedef struct V2_45ae80 { float x, y; } V2_45ae80;
typedef struct Line45ae80 { V3_45ae80 start; V3_45ae80 end; } Line45ae80;
typedef struct Col45ae80 { unsigned char r, g, b, a; } Col45ae80;
typedef struct Mtx45ae80 { V3_45ae80 right; int f0; V3_45ae80 up; int f1; V3_45ae80 at; int f2; V3_45ae80 pos; int f3; } Mtx45ae80;
typedef struct Hit45ae80 { V3_45ae80 pos; int m_0c; int m_10[10]; float m_38; int m_3c; } Hit45ae80;
typedef struct Spark45ae80 {
    V3_45ae80 pos;  /* +0x00 */
    V3_45ae80 vel;  /* +0x0c */
    float m_18;     /* +0x18 */
    int m_1c;       /* +0x1c */
    float m_20;     /* +0x20 */
    int m_24;       /* +0x24 */
    float m_28;     /* +0x28 */
    int m_2c;       /* +0x2c */
    int m_active;   /* +0x30 */
} Spark45ae80;
typedef Spark45ae80 Grid45ae80[5][5];
typedef struct Sub20_45ae80 { int f0; int frame; } Sub20_45ae80;
typedef struct Rec45ae80 {
    int m_00;
    void *m_04;
    float m_08;
    int m_0c;
    int m_10;
    int m_14;
    int m_18;
    int m_1c;
    Sub20_45ae80 *m_20;
    int m_24;
    int m_28[16];
} Rec45ae80;
typedef struct {
  float x, y, z;
  float f0c;
  int i10;
  int i14;
  int i18;
  void (*cb)(int, void *);
  int pad[(0x8c - 0x20) / 4];
} Req530;
typedef struct { float x, y, z; } V3_590;
typedef struct { char pad[0xb0]; int fb0; } Sub04_590;
typedef struct { int f0; int frame; } Sub24_590;
typedef struct {
  int f00;
  Sub04_590 *p04;
  int f08;
  float f0c;
  int f10, f14, f18;
  int state;
  int f20;
  Sub24_590 *p24;
  float m28[16];
} Obj590;
typedef struct Ent45ac40 {
    float m_pos[3];
    int m_0c[3];
    float m_18;
    int m_1c[5];
    int m_active;
} Ent45ac40;
typedef Ent45ac40 Grid45ac40[5][5];

typedef struct Tex45ae80 { int a[2]; int b[2]; } Tex45ae80;
MGLOBALA(Rec45ae80, DAT_0068bb60, 4, 0x0068bb60);
MGLOBALA(Grid45ae80, DAT_0068bd00, 4, 0x0068bd00);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(Tex45ae80, DAT_005f9898, 16, 0x005f9898);

void FUN_00474d80(int a, int on);
int FUN_004c1480(int frame, void *m, int op);
void FUN_0045a950(Rec45ae80 *o);
int FUN_004671d0(int p0);
int FUN_0045bfe0(void);
int FUN_004b4cd0(int world, Line45ae80 *line, Hit45ae80 *hit);
void FUN_004b4650(V3_45ae80 *out, V3_45ae80 *a, V3_45ae80 *b, float c);
void FUN_004b5080(Mtx45ae80 *out, int *a, V3_45ae80 *b);
float FUN_00472650(float lo, float hi);
void FUN_00486610(Mtx45ae80 *m, float f);
void FUN_004893d0(int h, V3_45ae80 *v, Col45ae80 *c);
void FUN_00489450(int h, V2_45ae80 *v);
void FUN_00489480(int h, float f);
void FUN_004894a0(int h, int *a, int *b);
void FUN_004893b0(int h);
void FUN_00489890(int h);
void FUN_0045a420(int a, void *b);
int FUN_00484cf0(Req530 *r);
void FUN_00465f40(int p0, V3_590 *p1, int p2, float p3);
char *FUN_004c0ed0(int frame);
int FUN_004c13e0(int frame, V3_590 *v, int combine);

// MATCH: 0x0045a530 FUN_0045a530
static void FUN_0045a530(float *v, int param_1)
{
  Req530 r;
  r.x = v[0];
  r.y = v[1];
  r.z = v[2];
  r.f0c = 0.15f;
  r.i14 = 0;
  r.i18 = param_1;
  r.i10 = 0x10;
  r.cb = FUN_0045a420;
  FUN_00484cf0(&r);
}

// MATCH: 0x0045a590 FUN_0045a590
static void FUN_0045a590(Obj590 *o)
{
  int frame;
  float scale;
  V3_590 s;
  V3_590 pos;
  frame = o->p24->frame;
  s.x = 1.0f;
  s.y = 1.0f;
  s.z = 1.0f;
  scale = 1.0f;
  if (o->state == 1) {
    if (o->f0c < 5.0f) {
      o->f0c += 0.5f;
    }
    scale = 1.5f;
  } else if (o->f0c > 1.0f) {
    scale = 1.0f;
    o->f0c -= 0.5f;
  }
  pos = *(V3_590 *)(FUN_004c0ed0(o->p24->frame) + 0x30);
  FUN_00465f40(0x16, &pos, o->p04->fb0, o->f0c * 0.18181819f);
  scale += FUN_00472650(0.0f, 0.1f);
  o->f0c += FUN_00472650(0.0f, 0.3f);
  s.z = o->f0c;
  s.x = scale;
  s.y = scale;
  FUN_004c1480(frame, o->m28, 0);
  FUN_004c13e0(frame, &s, 1);
}

// MATCH: 0x0045ac40 FUN_0045ac40
static void FUN_0045ac40(int a, int b, float *p)
{
    int i;
    int j;
    Ent45ac40 *ei;
    Ent45ac40 *ej;
    Ent45ac40 tmp;
    float dx, dy, dz, d;

    ei = ((Grid45ac40 *)DAT_0068bd00)[a][b];
    for (i = 1; i < 6; i++, ei++) {
        if (ei->m_active != 0 && !(ei->m_18 >= 1.0f)) {
            dx = p[0] - ei->m_pos[0];
            dz = p[2] - ei->m_pos[2];
            dy = p[1] - ei->m_pos[1];
            d = dx * dx + dy * dy + dz * dz;
            j = i;
            if (j < 5) {
                ej = &((Grid45ac40 *)DAT_0068bd00)[a][b][j];
            top:
                if (ej->m_active != 0 && !(ej->m_18 >= 1.0f)) {
                    dx = p[0] - ej->m_pos[0];
                    dy = p[1] - ej->m_pos[1];
                    dz = p[2] - ej->m_pos[2];
                    if (dx * dx + dy * dy + dz * dz > d) {
                        tmp = *ej;
                        *ej = *ei;
                        *ei = tmp;
                    }
                }
                j++;
                ej++;
                if (j < 5) goto top;
            }
        }
    }
}


// MATCH: 0x0045ae80 FUN_0045ae80
void FUN_0045ae80(void)
{
    Rec45ae80 *rec;
    Spark45ae80 *sp;
    int c;
    int b;
    int k;
    int any;
    float f;
    int idx;
    Col45ae80 col;
    V3_45ae80 o;
    Line45ae80 ln;
    V2_45ae80 sz;
    Hit45ae80 hit;
    Mtx45ae80 m;

    col.r = 0xff;
    col.g = 0xff;
    col.b = 0xff;
    col.a = 0xff;
    rec = &DAT_0068bb60[4];
    do {
        rec--;
        FUN_00474d80(rec->m_24, 0);
        if (rec->m_00 != 0) {
            FUN_004c1480(rec->m_20->frame, rec->m_04, 0);
            rec->m_08 -= DAT_007f100c;
            FUN_0045a950(rec);
            FUN_0045a590((Obj590 *)rec);
            if (rec->m_14 >= 5) {
                rec->m_00 = 0;
                rec->m_14 = 5;
            }
        }
    } while (rec != DAT_0068bb60);
    c = 0;
    for (rec = DAT_0068bb60; rec < &DAT_0068bb60[4]; rec++) {
        for (b = 0; b < 5; b++) {
            any = 0;
            FUN_0045ac40(c, b, (float *)FUN_004671d0(0));
            for (k = 0; k < 5; k++) {
                sp = &DAT_0068bd00[c][b][k];
                if (sp->m_active != 0 && sp->m_18 < 1.0f) {
                    any = 1;
                    if (sp->m_2c == 0) {
                        ln.start = sp->pos;
                        ln.end.x = ln.start.x + sp->vel.x;
                        ln.end.y = ln.start.y + sp->vel.y;
                        ln.end.z = ln.start.z + sp->vel.z;
                        if (FUN_004b4cd0(FUN_0045bfe0(), &ln, &hit) != 0) {
                            FUN_004b4650(&o, &ln.start, &ln.end, hit.m_38);
                            o.z += hit.pos.z * 0.02f;
                            o.x += hit.pos.x * 0.02f;
                            o.y += hit.pos.y * 0.02f;
                            sp->vel.x = 0.0f;
                            sp->vel.z = 0.0f;
                            sp->vel.y = 0.0f;
                            sp->m_2c = 1;
                            FUN_004b5080(&m, hit.m_10, &o);
                            FUN_00486610(&m, FUN_00472650(0.3f, 1.3f));
                        } else {
                            sp->vel.y -= 0.002f;
                        }
                    }
                    sp->m_20 += DAT_007f100c;
                    sp->m_18 = sp->m_20 * 1.1111112f;
                    if (sp->m_18 > 1.0f) {
                        sp->m_18 = 1.0f;
                    }
                    col.a = (unsigned char)(255.0f - sp->m_18 * 235.0f);
                    if (FUN_00472650(0.0f, 1.0f) > 0.4f) {
                        FUN_0045a530(&sp->pos.x, c);
                    }
                    FUN_004893d0(rec->m_10, &sp->pos, &col);
                    f = sp->m_18 < 0.25f ? sp->m_18 : 0.25f;
                    sz.x = f * 1.5f + 0.3f;
                    sz.y = sz.x;
                    FUN_00489450(rec->m_10, &sz);
                    FUN_00489480(rec->m_10, sp->m_28);
                    idx = (int)(sp->m_18 * 15.0f);
                    FUN_004894a0(rec->m_10, DAT_005f9898[idx].a, DAT_005f9898[idx].b);
                    sp->pos.x += sp->vel.x;
                    sp->m_1c++;
                    sp->pos.y += sp->vel.y;
                    sp->pos.z += sp->vel.z;
                }
            }
            if (any != 0) {
                FUN_004893b0(rec->m_10);
            }
        }
        c++;
        FUN_00489890(rec->m_10);
    }
}
