// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045b390 (h10): best 96.9% (regs 96.9%, ops 97.3%), 839 B; raw g2s 25.9%.
   Moved it: typed record (+0x04 two object pointers, +0x0c, +0x10, +0x14) and V3/V4/sphere/matrix locals;
   float literals instead of the pooled .rdata constants (1.5 @0x005cc348, 0.05 @0x005cc9a0, 40.0 @0x005cd274,
   1.25 @0x005cd074: a literal is loaded second, the extern global first); int buffer in the hit record (a char
   array put in a /GS cookie the original does not have); v = start then two scaled increments (frame layout,
   56.7% -> 76.8%); the copy chain sph.c = ltm->pos, end = sph.c, start = end (79.5%); the hit test is
   FUN_004b4b20(FUN_0045bfe0(), &line, &hit): 0x0045bfe0 takes no args (mov eax,[0x0080332c]; ret), which is what
   keeps the 0x00476860 pop separate (83.5%); start/end as one Line struct (frame layout, 95.5%); permuter
   reorders of independent stores (96.9%).
   Residual (b): x87/push scheduling in the v block (fld st(1) before push eax; col.b/col.a byte stores
   around the fstp) and the dir2.y load placement; permuter (about 1000 legal variants) did not move them. */
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
typedef struct V3_45b390 { float x, y, z; } V3_45b390;
typedef struct Line45b390 { V3_45b390 start; V3_45b390 end; } Line45b390;
typedef struct V4_45b390 { float x, y, z, w; } V4_45b390;
typedef struct Sph45b390 { V3_45b390 c; float r; } Sph45b390;
typedef struct Mtx45b390 { V3_45b390 right; int f0; V3_45b390 up; int f1; V3_45b390 at; int f2; V3_45b390 pos; int f3; } Mtx45b390;
typedef struct Hit45b390 { V3_45b390 pos; int m_0c; int m_10[10]; float m_38; int m_3c; } Hit45b390;
typedef struct Col45b390 { unsigned char r, g, b, a; } Col45b390;
typedef struct Obj45b390 { int m_00; char *m_04; } Obj45b390;
typedef struct Rec45b390 {
    int m_00;
    Obj45b390 *m_obj[2];
    int m_0c;
    int m_10;
    char *m_14;
} Rec45b390;
MGLOBAL(V3_45b390, DAT_006146fc, 0x006146fc);

void FUN_00465ca0(int id, char *p);
char *FUN_004c0ed0(char *frame);
float FUN_004c39b0(V3_45b390 *out, char *in);
void FUN_00476860(Sph45b390 *s, V3_45b390 *p, float a);
int FUN_0045bfe0(void);
int FUN_004b4b20(int world, Line45b390 *line, Hit45b390 *hit);
void FUN_004b5080(Mtx45b390 *out, int *a, V3_45b390 *b);
Mtx45b390 *FUN_004c5010(Mtx45b390 *m, V3_45b390 *s, int op);
void FUN_00475ab0(int a, Mtx45b390 *m, Col45b390 *c);
int FUN_0040e370(int car);
void FUN_0041f030(int car, V4_45b390 *out);
int FUN_00558b40(V3_45b390 *a, V4_45b390 *b, int *c);
int FUN_00480720(int car);
void FUN_004b4650(V3_45b390 *out, V3_45b390 *a, V3_45b390 *b, int c);
void FUN_00481750(int car, V3_45b390 *a, V3_45b390 *b);
void FUN_00421630(int car, float k, V3_45b390 *p);
void FUN_004922e0(int car, int a, int b, int c);

// MATCH: 0x0045b390 FUN_0045b390
void FUN_0045b390(Rec45b390 *rec)
{
    int i;
    char *ltm;
    float f;
    Col45b390 col;
    V3_45b390 v;
    V3_45b390 dir;
    Line45b390 ln;
    V3_45b390 dir2;
    int k;
    Sph45b390 sph;
    V3_45b390 scale;
    int res;
    V4_45b390 sph2;
    V3_45b390 out;
    Hit45b390 hit;
    Mtx45b390 m;

    FUN_00465ca0(0x15, rec->m_14 + 0x30);
    rec->m_10--;
    rec->m_0c = 8;
    sph.r = 0.25f;
    for (k = 0; k < 2; k++) {
        ltm = FUN_004c0ed0(rec->m_obj[k]->m_04);
        sph.c = *(V3_45b390 *)(ltm + 0x30);
        ln.end = sph.c;
        ln.start = ln.end;
        FUN_004c39b0(&dir, ltm + 0x20);
        sph.c.x += dir.x;
        sph.c.y += dir.y;
        sph.c.z += dir.z;
        dir2 = dir;
        ln.end.x += dir.x * 1.5f;
        ln.end.y += dir.y * 1.5f;
        ln.end.z += dir.z * 1.5f;
        FUN_00476860(&sph, &DAT_006146fc, 90.0f);
        if (FUN_004b4b20(FUN_0045bfe0(), &ln, &hit) != 0) {
            f = hit.m_38 * 1.5f;
            col.r = 0;
            col.b = 0;
            col.a = 0x80;
            col.g = 0;
            v = ln.start;
            v.x += dir.x * f;
            v.y += dir.y * f;
            v.z += dir.z * f;
            v.x += hit.pos.x * 0.05f;
            v.y += hit.pos.y * 0.05f;
            v.z += hit.pos.z * 0.05f;
            FUN_004b5080(&m, hit.m_10, &v);
            scale.y = 1.5f;
            scale.x = 1.5f;
            scale.z = 1.0f;
            FUN_004c5010(&m, &scale, 1);
            FUN_00475ab0(1, &m, &col);
        }
        dir2.x *= 40.0f;
        dir2.z *= 40.0f;
        dir2.y = -(dir2.y * 40.0f);
        for (i = 0; i < 4; i++) {
            if (FUN_0040e370(i) != 0 && *(int *)(rec->m_14 + 0xb0) != i) {
                FUN_0041f030(i, &sph2);
                sph2.w *= 1.25f;
                if (FUN_00558b40(&ln.start, &sph2, &res) != 0) {
                    FUN_00480720(i);
                    FUN_004b4650(&out, &ln.start, &ln.end, 0);
                    FUN_00481750(i, &dir2, &out);
                    FUN_00421630(i, 8.0f, &ln.start);
                    FUN_004922e0(i, 2, 0xf, 0xff);
                }
            }
        }
    }
}
