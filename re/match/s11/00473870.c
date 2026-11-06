// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 h8 0x00473870: 100% (regs 100%, ops 100%). Hand-written (raw converter 22.0%).
   Textured Im2D quad: v0 gets x, y, uv, z, rhw; the loop sets colour/z/rhw on all four; texture raster +
   filter mode 9 (2 if `filter`, else 1), or no raster when tex is NULL.
   What moved it: position and size as two RwV2d passed by value (separate float params always load x
   before w in `x + w`; the struct fields give the original's w-first order), and v0's u/v stored before
   its z/rhw. */
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
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVtx, DAT_00898a20, 4, 0x00898a20);

#define RGBALONG(c) ((uint)(((c).a << 24) | ((c).r << 16) | ((c).g << 8) | (c).b))

// MATCH: 0x00473870 FUN_00473870
void FUN_00473870(RwTexture *tex, RwV2d p, RwV2d s, RwRGBA c, int filter)
{
    int i;
    uint col;
    DAT_00898a20[0].x = p.x;
    DAT_00898a20[0].y = p.y;
    DAT_00898a20[0].u = 0.0f;
    DAT_00898a20[0].v = 0.0f;
    DAT_00898a20[0].z = DAT_007d3ff8->nearZ;
    DAT_00898a20[0].rhw = 1.0f;
    DAT_00898a20[1].x = p.x + s.x;
    DAT_00898a20[1].y = p.y;
    DAT_00898a20[1].u = 1.0f;
    DAT_00898a20[1].v = 0.0f;
    DAT_00898a20[2].x = p.x;
    DAT_00898a20[2].y = p.y + s.y;
    DAT_00898a20[2].u = 0.0f;
    DAT_00898a20[2].v = 1.0f;
    DAT_00898a20[3].x = p.x + s.x;
    DAT_00898a20[3].y = p.y + s.y;
    DAT_00898a20[3].u = 1.0f;
    DAT_00898a20[3].v = 1.0f;
    col = RGBALONG(c);
    for (i = 0; i < 4; i++) {
        DAT_00898a20[i].color = col;
        DAT_00898a20[i].z = DAT_007d3ff8->nearZ;
        DAT_00898a20[i].rhw = 1.0f;
    }
    if (tex) {
        DAT_007d3ff8->stateSet(1, tex->raster);
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
