// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x00488e70 (first hand source; raw g2s 6.8%): 99.6%, 7 B differ, NOT CLOSED.
   0x00488320 is a same-TU static (cloud in EAX, &k in EDX at 0x00488fc0..0x00488fc7); the static below
   is a stand-in written from its body for the calling convention only (not scored, no MATCH line).
   Moved it: the 128-entry walk indexed DAT_0086ae20[j] (a pointer loop makes cl count down in EBP and
   push it); the mtx != 0 branch first; the four locks as separate locals with col[1] written in both
   branches (cl orders the frame by reference count: with one shared col[1] store the col lock drops
   behind rgba / uv, 67.2% to 92.0%); `(0, k * 0.5f)` for the i == 4 product; wp copied through comma
   rvalues `wp.x = (0, e->pos.x)` (fld/fstp copies at 0x00488fa3, plain assignment gives mov pairs,
   96.0% to 98.4%); `(0, e->mtx->at.x) * cam->at.x + ...` for the dot order at 0x00488f84. Residual:
   the `fstp len` after FUN_004c39b0 is placed after `mov ecx, e->count` (original: before, 0x0048902b).
   Tried: len as comma rvalue, double temp, block scope, (float) cast, double / long double return
   types, k and len as one array, for-init forms, while / do-while loops, match_permute (688 compiles,
   no gain). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0x20]; V3 at; char pad2[4]; V3 pos; } Frame;   /* at +0x20, pos +0x30 */
typedef struct { void *ptr; int stride; } Lock;
typedef struct { int v[4]; } UV4;                                          /* 0x10 B at 0x00614260 */
typedef struct {
    int type;           /* +0x00 */
    V3 pos;             /* +0x04 */
    int count;          /* +0x10 */
    int active;         /* +0x14 */
    float factor;       /* +0x18 */
    int pad1c;          /* +0x1c */
    float *tab;         /* +0x20 */
    int *idx;           /* +0x24 */
    unsigned char *rgba;/* +0x28 */
    Frame *mtx;         /* +0x2c */
    float speed;        /* +0x30 */
    float phase;        /* +0x34 */
} Cloud;                                                                  /* 0x38 B, 128 at 0x0086ae20 */
MGLOBALA(Cloud, DAT_0086ae20, 128, 0x0086ae20);
MGLOBALA(UV4, DAT_00614260, 1, 0x00614260);
MGLOBAL(int, DAT_007030b0, 0x007030b0);
MGLOBAL(int, DAT_007030b4, 0x007030b4);
MGLOBAL(int, DAT_007030a8, 0x007030a8);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
double sin(double);
int   FUN_004671a0(int p0);
int   FUN_00535700(int h, Lock *l, unsigned int n, unsigned int f);
void  FUN_00535910(int h);
float FUN_004c39b0(V3 *out, V3 *in);
void  FUN_004c3d90(V3 *out, V3 *in, int n, Frame *m);
void  FUN_004883b0(Frame *cam);

/* stand-in for 0x00488320 (same-TU static: cloud in EAX, scale out in EDX at 0x00488fc0..0x00488fc7) */
// MATCH: 0x00488320 _FUN_00488320
static void FUN_00488320(Cloud *c, float *s)
{
    switch (c->type) {
    case 0xc:
        *s = 1.0f;
        return;
    case 6:
        c->factor = (float)((sin(c->phase) + 1.0) * 0.5 + 0.25);
        c->phase = c->speed + c->phase;
        *s = 0.5f;
        return;
    case 4:
        c->factor = (float)((sin(c->phase) + 1.0) * 0.5 + 0.25);
        c->phase = c->speed + c->phase;
        *s = 1.0f;
        return;
    }
}

// MATCH: 0x00488e70 _FUN_00488e70
void FUN_00488e70(Frame *cam)
{
    float k;
    float len;
    Lock pos;
    Lock col;
    Lock rgba;
    Lock uv;
    V3 wp;
    V3 cp;
    V3 dir;
    Cloud *e;
    int j;
    int n;
    int i;
    float t;

    wp.x = 0.0f;
    wp.y = 0.0f;
    wp.z = 0.0f;
    FUN_004671a0(0);
    if (DAT_007030b0 != 0 && DAT_007030b4 == 0) {
        DAT_007030b4 = 1;
        FUN_00535700(DAT_007030a8, &pos, 1, 0x40000000);
        FUN_00535700(DAT_007030a8, &col, 4, 0x40000000);
        FUN_00535700(DAT_007030a8, &rgba, 2, 0x40000000);
        FUN_00535700(DAT_007030a8, &uv, 0x80, 0x40000000);
        n = 0;
        cp.x = cam->at.x * 3.0f + cam->pos.x;
        cp.y = cam->at.y * 3.0f + cam->pos.y;
        cp.z = cam->at.z * 3.0f + cam->pos.z;
        for (j = 0; j < 128; j++) {
            e = &DAT_0086ae20[j];
            if (e->active == 0)
                continue;
            if (e->mtx != 0) {
                FUN_004c3d90(&wp, &e->pos, 1, e->mtx);
                k = (0, e->mtx->at.x) * cam->at.x + e->mtx->at.y * cam->at.y;
                k += e->mtx->at.z * cam->at.z;
            } else {
                wp.x = (0, e->pos.x);
                k = 1.0f;
                wp.y = (0, e->pos.y);
                wp.z = (0, e->pos.z);
            }
            FUN_00488320(e, &k);
            k = k * e->factor;
            if (k < 0.0f)
                k = 0.0f;
            if (pos.ptr == 0)
                continue;
            dir.x = wp.x - cp.x;
            dir.y = wp.y - cp.y;
            dir.z = wp.z - cp.z;
            len = FUN_004c39b0(&dir, &dir);
            for (i = 0; i < e->count; i++) {
                ((float *)pos.ptr)[0] = len * dir.x + cp.x;
                ((float *)pos.ptr)[1] = len * dir.y + cp.y;
                ((float *)pos.ptr)[2] = len * dir.z + cp.z;
                pos.ptr = (char *)pos.ptr + pos.stride;
                if (i == 4) {
                    ((float *)col.ptr)[0] = (k * 10.0f + 2.0f) * e->tab[4] * k;
                    ((float *)col.ptr)[1] = (0, k * 0.5f) * e->tab[4] * k;
                } else {
                    t = k + 1.0f;
                    ((float *)col.ptr)[0] = t * e->tab[i] * k;
                    ((float *)col.ptr)[1] = t * e->tab[i] * k;
                }
                col.ptr = (char *)col.ptr + col.stride;
                ((unsigned char *)rgba.ptr)[0] = e->rgba[i * 4];
                ((unsigned char *)rgba.ptr)[1] = e->rgba[i * 4 + 1];
                ((unsigned char *)rgba.ptr)[2] = e->rgba[i * 4 + 2];
                ((unsigned char *)rgba.ptr)[3] = e->rgba[i * 4 + 3];
                rgba.ptr = (char *)rgba.ptr + rgba.stride;
                ((int *)uv.ptr)[0] = DAT_00614260[e->idx[i]].v[0];
                ((int *)uv.ptr)[1] = DAT_00614260[e->idx[i]].v[1];
                ((int *)uv.ptr)[2] = DAT_00614260[e->idx[i]].v[2];
                ((int *)uv.ptr)[3] = DAT_00614260[e->idx[i]].v[3];
                uv.ptr = (char *)uv.ptr + uv.stride;
                n++;
            }
        }
        FUN_00535910(DAT_007030a8);
        *(unsigned int *)(*(int *)(DAT_007dc57c + DAT_007030a8) + 0x40) |= 0x800000;
        *(int *)(*(int *)(DAT_007dc57c + DAT_007030a8) + 4) = n;
        FUN_004883b0(cam);
    }
}

