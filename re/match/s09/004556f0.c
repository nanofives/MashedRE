// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004556f0 (h4): 98.8% (regs 100%, ops 100%) over the real body, 170/170 ins; whole obj symbol
   541 B with 2 differing unmasked bytes. NOT 100%. Ghidra's size (98 B, pc_funcs.csv) stops at 0x00455752
   mid-function: the real end is the `ret` at 0x0045590c (541 B), so plain match.py scores this file against
   98 B only; the score above is match.py with the size overridden to 0x21d.
   The record pointer is ESI-passed (pilot finding 1): the file holds a stand-in caller (the record loop of
   0x00455c90, unmatched); no MATCH line on it, built only outside MATCH_PORT. Raw converter 8.6% (on 98 B).
   What moved it: locals in the original frame order (src ptr, k, v, n, pt, 64-B matrix), the +0x30 vector
   copied as a struct then .y adjusted, 4.878e-05 as the MGLOBAL float 0x005ce474 (as a literal the multiply
   operands flipped), `r->f34 = K * k; r->f38 = r->f34 - r->f30;` (that spelling also fixed the operand order
   of the later `+0x30 += +0x2c`), FUN_004c39b0 / FUN_004c3ac0 declared as returning float (fstp st(0)).
   Residual (b): `r->vx = m->f30 * r->f30` loads m->f30 first, the original loads r->f30 first (vy/vz match).
   Tried: both operand orders, the +0x30 update as +=, via a local, clamp spellings, and two permuter runs
   (300 + 400 s, no gain). */
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
typedef struct V3_004556f0 { float x, y, z; } V3_004556f0;
typedef struct Mtx004556f0 {
    float f00[4];
    float f10[4];       /* +0x10 */
    float f20[4];
    float f30, f34, f38;/* +0x30 */
    float f3c;
} Mtx004556f0;
typedef struct Src004556f0 { float f00[12]; V3_004556f0 pos; } Src004556f0;   /* pos at +0x30 */
typedef struct Obj004556f0 { int f00; Mtx004556f0 *frame; } Obj004556f0;
typedef struct Rec004556f0 {
    int f00, f04, f08;
    float px, py, pz;   /* +0x0c */
    float f18;          /* +0x18 */
    float vx, vy, vz;   /* +0x1c */
    float f28;          /* +0x28 */
    float f2c;          /* +0x2c */
    float f30;          /* +0x30 */
    float f34;          /* +0x34 */
    float f38;          /* +0x38 */
    float f3c;
    float f40;          /* +0x40 */
    float f44;          /* +0x44 */
    float f48;          /* +0x48 */
    float f4c;          /* +0x4c */
    int f50, f54, f58;  /* +0x50 */
    Obj004556f0 *obj;   /* +0x5c */
    int f60;            /* +0x60 */
    int pad64[2];
} Rec004556f0;          /* 0x6c, 5 records at 0x006883b0 */
MGLOBALA(Rec004556f0, DAT_006883b0, 5, 0x006883b0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(float, DAT_005ce474, 0x005ce474);
float *FUN_004c4dc0(Mtx004556f0 *out, float *in);
void FUN_00407620(Src004556f0 **out, int id);
int FUN_0046d4a0(Src004556f0 **out, int id);
void FUN_004c3d90(V3_004556f0 *out, V3_004556f0 *in, int n, Mtx004556f0 *m);
float FUN_004c39b0(V3_004556f0 *out, V3_004556f0 *in);
float FUN_004c3ac0(V3_004556f0 *v);
void FUN_004c15c0(Mtx004556f0 *p0);
int FUN_004c1520(Mtx004556f0 *p0, float *p1, float p2, int p3);
int FUN_004c1340(Mtx004556f0 *p0, float *p1, int p2);
void FUN_0046d6d0(float *out, int id);
// MATCH: 0x004556f0 FUN_004556f0
static void FUN_004556f0(Rec004556f0 *r)
{
    Src004556f0 *src;
    float k;
    V3_004556f0 v;
    V3_004556f0 n;
    V3_004556f0 pt;
    Mtx004556f0 inv;
    Mtx004556f0 *m;
    float t;

    k = 0.0f;
    m = r->obj->frame;
    FUN_004c4dc0(&inv, m->f10);
    if (r->f54 != -1)
        FUN_0046d4a0(&src, r->f54);
    else
        FUN_00407620(&src, r->f58);
    v = src->pos;
    v.y = v.y + 0.2f;
    FUN_004c3d90(&pt, &v, 1, &inv);
    FUN_004c39b0(&n, &pt);
    if (n.y > 0.2f)
        r->f40 = -20.0f;
    else if (n.y < -0.2f)
        r->f40 = 20.0f;
    else
        r->f40 = 0.0f;
    if (n.x > 0.2f)
        r->f44 = 2.0f;
    else if (n.x < -0.2f)
        r->f44 = -2.0f;
    else
        r->f44 = 0.0f;
    FUN_004c3ac0(&pt);
    r->f48 = r->f44 + r->f48;
    r->f4c = r->f4c + r->f40;
    FUN_004c15c0(m);
    FUN_004c1520(m, DAT_006146fc, r->f48, 0);
    FUN_004c1520(m, m->f10, r->f4c, 2);
    FUN_0046d6d0(&k, r->f54);
    r->f34 = DAT_005ce474 * k;
    r->f38 = r->f34 - r->f30;
    if (r->f38 < 0.02f)
        r->f2c = r->f2c + 0.0001f;
    else
        r->f2c = r->f2c - 0.01f;
    if (r->f2c > 0.005f)
        r->f2c = 0.005f;
    r->f30 += r->f2c;
    if (r->f30 < 0.075f)
        r->f30 = 0.075f;
    r->vx = m->f30 * r->f30;
    r->vy = m->f34 * r->f30;
    r->vz = m->f38 * r->f30;
    r->vy = r->f28 + r->vy;
    r->px = r->vx + r->px;
    r->py = r->vy + r->py;
    r->pz = r->vz + r->pz;
    FUN_004c1340(m, &r->px, 2);
}

#ifndef MATCH_PORT
/* stand-in caller: the record loop of 0x00455c90 (not matched) */
void StandIn_004556f0(void)
{
    Rec004556f0 *r;
    for (r = DAT_006883b0; r < DAT_006883b0 + 5; r++)
        if (r->f54 != -1)
            FUN_004556f0(r);
}
#endif
