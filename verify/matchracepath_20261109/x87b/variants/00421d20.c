// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421d20: by hand, best 94.5% (regs 94.5%). This file holds the static callee 0x00421960 (object in
   EAX, 100%), defined before its caller.
   What moved it: the spawn position in a block scope and the cross vector inside the loop body (they share one
   frame slot, 68.2% -> 92.2%); the index as (set != 0 ? 0x5a : 0x50) + i; the 0.0f at 0x005d757c is a compiler
   literal (it sits inside an EH table, so the linker placed __real@00000000 there), y*0.0f order as written.
   Residual (b): cr.z = y*0 - x compiles to fsub [x] where the original loads x and does fsubr st(1) then
   fstp st(0) (the y*0 temporary kept live), plus two schedule moves (mov ecx,[esi+4] before the 0.25 scale, the
   push of the next call's args before vel.z's fadd) and the 2-byte alignment pad that follows. Tried: literal vs
   global 0.0f and 0.25f, 1.0f factors, negated forms, explicit temporaries, an __inline cross helper with a local
   up vector, *=, a frame temporary, permuter 418 variants (no gain). The tail past Ghidra's size is the epilogue. */
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
typedef struct V3d00421d20 { float x, y, z; } V3d00421d20;
typedef struct Mat00421d20 { V3d00421d20 right; int flags; V3d00421d20 up; int pad1; V3d00421d20 at; int pad2;
    V3d00421d20 pos; int pad3; } Mat00421d20;
typedef struct Ent00421d20 { V3d00421d20 v; int clump; int obj; int vis; } Ent00421d20;
struct Rec00421d20;
typedef struct Grp00421d20 { Ent00421d20 e[10]; unsigned char bits[4]; int count; int set; struct Rec00421d20 *rec; } Grp00421d20;
typedef struct Rec00421d20 { Grp00421d20 g[2]; int model; int idx; } Rec00421d20;
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(int, DAT_006146fc, 0x006146fc);
int FUN_0055dec0(int p0);
void FUN_00559c40(int p0, int p1, int p2, int p3);
void FUN_0041f260(int p0, Mat00421d20 *p1);
void FUN_0046cb30(V3d00421d20 *p0, int p1);
void FUN_00476880(V3d00421d20 *p0, int *p1, float p2, float p3);
int FUN_0041f1c0(int p0, int p1);
Mat00421d20 *FUN_0041f290(int p0, int p1);
void FUN_004c1480(int p0, Mat00421d20 *p1, int p2);
float FUN_004c39b0(V3d00421d20 *p0, V3d00421d20 *p1);
void FUN_004c1340(int p0, V3d00421d20 *p1, int p2);
void FUN_0055ae70(int p0, int p1, int p2);
float FUN_004b4510(float p0, float p1);
void FUN_00421750(int p0, V3d00421d20 *p1);
void FUN_00421800(int p0, V3d00421d20 *p1, int p2);
void FUN_00486460(void *p0, float p1);

// MATCH: 0x00421960 _FUN_00421960
static void FUN_00421960(int obj)
{
    FUN_00559c40(FUN_0055dec0(DAT_006ce274), obj, -1, 7);
}

// MATCH: 0x00421d20 _FUN_00421d20
void FUN_00421d20(Grp00421d20 *g)
{
    Rec00421d20 *rec = g->rec;
    Mat00421d20 mat;
    V3d00421d20 dir;
    V3d00421d20 vel;
    int i, n;
    vel.x = 0;
    vel.y = 0;
    vel.z = 0;
    FUN_0041f260(rec->idx, &mat);
    FUN_0046cb30(&dir, g->rec->idx);
    {
    V3d00421d20 pos;
    pos = mat.pos;
    pos.x = (g->set == 0 ? 0.4f : -0.4f) * mat.at.x + mat.pos.x;
    pos.y = (g->set == 0 ? 0.4f : -0.4f) * mat.at.y + pos.y;
    pos.z = (g->set == 0 ? 0.4f : -0.4f) * mat.at.z + pos.z;
    FUN_00476880(&pos, &DAT_006146fc, 30.0f, 0.5f);
    }
    for (i = 0; i < 10; i++) {
        Ent00421d20 *e = &g->e[i];
        if (e->clump != 0) {
            int k = (g->set != 0 ? 0x5a : 0x50) + i;
            Mat00421d20 *m;
            V3d00421d20 cr;
            if (FUN_0041f1c0(rec->idx, k) == 0)
                return;
            m = FUN_0041f290(rec->idx, k);
            FUN_004c1480(*(int *)(e->clump + 4), m, 0);
            e->v.x = m->pos.x - mat.pos.x;
            e->v.y = (float)((double)(m->pos.y) - mat.pos.y);
            e->v.z = (float)((double)(m->pos.z) - mat.pos.z);
            FUN_004c39b0(&e->v, &e->v);
            e->v.x = e->v.x * 0.25f;
            e->v.y = e->v.y * 0.25f;
            e->v.z = e->v.z * 0.25f;
            FUN_004c1340(*(int *)(e->clump + 4), &e->v, 2);
            FUN_00421960(e->obj);
            FUN_0055ae70(*(int *)(e->obj + 0x24), e->obj, 6);
            vel.x = FUN_004b4510(-5.0f, 5.0f);
            vel.z = FUN_004b4510(-5.0f, 5.0f);
            vel.y = FUN_004b4510(10.0f, 20.0f);
            vel.x = (float)(FUN_004b4510(0.01f, 0.02f) * dir.x + (double)(vel.x));
            vel.y = FUN_004b4510(0.01f, 0.02f) * dir.y + vel.y;
            vel.z = FUN_004b4510(0.01f, 0.02f) * dir.z + vel.z;
            cr.x = e->v.z - e->v.y * 0.0f;
            cr.y = e->v.x * 0.0f - e->v.z * 0.0f;
            cr.z = e->v.y * 0.0f - e->v.x;
            cr.x = (float)(FUN_004b4510(100.0f, 200.0f) * (double)(cr.x));
            cr.y = (float)(FUN_004b4510(100.0f, 200.0f) * (double)(cr.y));
            cr.z = (float)(FUN_004b4510(100.0f, 200.0f) * (double)(cr.z));
            FUN_00421750(e->clump, &cr);
            FUN_00421800(e->clump, &vel, 0);
            for (n = 0; n < 2; n++)
                FUN_00486460((void *)(*(int *)(e->clump + 4) + 0x40), FUN_004b4510(0.2f, 0.4f));
            for (n = 0; n < 5; n++)
                FUN_00486460(&m->pos, FUN_004b4510(0.5f, 0.7f));
            g->bits[i >> 3] |= 1 << (i & 7);
            e->vis = 0;
        }
    }
    g->count = 1;
}
