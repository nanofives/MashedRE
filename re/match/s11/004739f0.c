// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 h8 0x004739f0: 100% (regs 100%, ops 100%). Hand-written (raw converter 43.7%).
   Textured Im2D quad with a uv rectangle (u0, u1, v0, v1) and a scale mode: 0 = none, 2 = y/h only by
   H / 448.0f (0x005ceac4), else x/w by W / 640.0f and y/h by H / 480.0f. Same shape as 0x00473870.
   What moved it: the raster as `if (tex->raster) set(raster) else set(0)` (a `?:` folds to one push). */
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
typedef struct { uchar r, g, b, a; } RwRGBA;
typedef struct { void *raster; } RwTexture;
typedef struct { float x, y; } RwV2d;
typedef struct { float x, y, z, rhw; uint color; float u, v; } Im2DVtx;
typedef int (*RenderStateSetFn)(int state, void *value);
typedef int (*Im2DRenderPrimFn)(int prim, Im2DVtx *verts, int n);
typedef struct {
    uchar pad0[0x18];
    float nearZ;                   /* +0x18 */
    uchar pad1c[4];
    RenderStateSetFn stateSet;     /* +0x20 */
    uchar pad24[0xc];
    Im2DRenderPrimFn im2dPrim;     /* +0x30 */
} RwGlobalsT;
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVtx, DAT_00898a20, 4, 0x00898a20);

#define RGBALONG(c) ((uint)(((c).a << 24) | ((c).r << 16) | ((c).g << 8) | (c).b))

// MATCH: 0x004739f0 FUN_004739f0
void FUN_004739f0(RwTexture *tex, RwV2d p, RwV2d s, RwRGBA c, float u0, float u1, float v0, float v1, int mode,
                  int filter)
{
    int i;
    uint col;
    if (mode) {
        if (mode == 2) {
            p.y = p.y * (float)FUN_0042b8c0() / 448.0f;
            s.y = s.y * (float)FUN_0042b8c0() / 448.0f;
        } else {
            p.x = p.x * (float)FUN_0042b8b0() / 640.0f;
            p.y = p.y * (float)FUN_0042b8c0() / 480.0f;
            s.x = s.x * (float)FUN_0042b8b0() / 640.0f;
            s.y = s.y * (float)FUN_0042b8c0() / 480.0f;
        }
    }
    DAT_00898a20[0].x = p.x;
    DAT_00898a20[0].y = p.y;
    DAT_00898a20[0].u = u0;
    DAT_00898a20[0].v = v0;
    DAT_00898a20[1].x = p.x + s.x;
    DAT_00898a20[1].y = p.y;
    DAT_00898a20[1].u = u1;
    DAT_00898a20[1].v = v0;
    DAT_00898a20[2].x = p.x;
    DAT_00898a20[2].y = p.y + s.y;
    DAT_00898a20[2].u = u0;
    DAT_00898a20[2].v = v1;
    DAT_00898a20[3].x = p.x + s.x;
    DAT_00898a20[3].y = p.y + s.y;
    DAT_00898a20[3].u = u1;
    DAT_00898a20[3].v = v1;
    col = RGBALONG(c);
    for (i = 0; i < 4; i++) {
        DAT_00898a20[i].color = col;
        DAT_00898a20[i].z = DAT_007d3ff8->nearZ;
        DAT_00898a20[i].rhw = 1.0f;
    }
    if (tex) {
        if (tex->raster)
            DAT_007d3ff8->stateSet(1, tex->raster);
        else
            DAT_007d3ff8->stateSet(1, 0);
        if (filter)
            DAT_007d3ff8->stateSet(9, (void *)2);
        else
            DAT_007d3ff8->stateSet(9, (void *)1);
    } else {
        DAT_007d3ff8->stateSet(1, 0);
    }
    DAT_007d3ff8->stateSet(0xc, (void *)1);
    DAT_007d3ff8->stateSet(10, (void *)5);
    DAT_007d3ff8->stateSet(0xb, (void *)6);
    DAT_007d3ff8->im2dPrim(4, DAT_00898a20, 4);
}
