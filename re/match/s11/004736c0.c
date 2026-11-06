// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 h8 0x004736c0: 100% byte-identical (match.py --bytes: 424 B whole symbol, 0 differing). The scored
   view says 97.4% only because Ghidra's size (417) stops before the last `call [eax+0x30]; add esp,0x30; ret`.
   Hand-written (raw converter 18.3%). Im2D quad, one rgb with a per-vertex alpha (a0..a3 as uchar params:
   int/uint alpha params drop to 62.9%). Same shape as 0x00473540 (RwV2d pos/size by value, per-vertex
   statement order, `/ 640.0f` literals). */
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
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);

#define RGBA4(r, g, b, a) ((uint)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RGBALONG(c) ((uint)(((c).a << 24) | ((c).r << 16) | ((c).g << 8) | (c).b))

/* port (MATCH-S11 swap): `* (1.0f / 640.0f)` / `* (1.0f / 480.0f)` is the multiply by 0x5cd5a8 / 0x5cc560 that 9176
   emits for `/ 640.0f` / `/ 480.0f` (bytes unchanged); VS2022 /fp:precise keeps a real divide, which rounds differently. */
// MATCH: 0x004736c0 FUN_004736c0
void FUN_004736c0(RwV2d p, RwV2d s, RwRGBA c, uchar a0, uchar a1, uchar a2, uchar a3)
{
    int i;
    p.x = p.x * (float)FUN_0042b8b0() * (1.0f / 640.0f);
    p.y = p.y * (float)FUN_0042b8c0() * (1.0f / 480.0f);
    s.x = s.x * (float)FUN_0042b8b0() * (1.0f / 640.0f);
    s.y = s.y * (float)FUN_0042b8c0() * (1.0f / 480.0f);
    DAT_00898a20[0].x = p.x;
    DAT_00898a20[0].y = p.y;
    DAT_00898a20[0].color = RGBA4(c.r, c.g, c.b, a0);
    DAT_00898a20[1].x = p.x + s.x;
    DAT_00898a20[1].y = p.y;
    DAT_00898a20[1].color = RGBA4(c.r, c.g, c.b, a1);
    DAT_00898a20[2].x = p.x;
    DAT_00898a20[2].y = p.y + s.y;
    DAT_00898a20[2].color = RGBA4(c.r, c.g, c.b, a2);
    DAT_00898a20[3].x = p.x + s.x;
    DAT_00898a20[3].y = p.y + s.y;
    DAT_00898a20[3].color = RGBA4(c.r, c.g, c.b, a3);
    for (i = 0; i < 4; i++) {
        DAT_00898a20[i].z = DAT_007d3ff8->nearZ;
        DAT_00898a20[i].rhw = 1.0f;
    }
    DAT_007d3ff8->stateSet(1, 0);
    DAT_007d3ff8->stateSet(0xc, (void *)1);
    DAT_007d3ff8->stateSet(10, (void *)5);
    DAT_007d3ff8->stateSet(0xb, (void *)6);
    DAT_007d3ff8->im2dPrim(4, DAT_00898a20, 4);
}
