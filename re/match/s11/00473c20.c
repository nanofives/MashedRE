// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 h8 0x00473c20: best 99.0% (regs 99.0%, ops 100.0%; --bytes: 2 differing bytes of 697).
   Hand-written (raw converter 49.3%). Textured Im2D quad, colour (alpha << 24) | 0xffffff, uv max from
   FUN_00493f80(&a, &b) as a / b, then a raster path (raster > 1 given, or the 0x00771a50/54 rasters,
   or untextured black when FUN_00494ee0(0) == 0). Ghidra's 4-arg signature misses the stack args: the
   body reads arg 9 (raster, +0x24) and arg 10 (alpha byte, +0x28). Args 5..8 (f5..f8) are never read:
   their types are [UNCERTAIN] (declared float only to keep the stack offsets).
   What moved it: a and b in an inner block that ends after uu/vv are computed (frame 0x18 -> 0x10, the
   int temps and spills reuse their slots: 42.9% -> 91.6%), x2/y2 locals for p.x + s.x / p.y + s.y
   (91.6% -> 99.0%), RwV2d pos/size and `/ 640.0f` literals as in 0x00473220.
   Residual (b): the y2 spill (fst + mov edx for v3.y) goes to [esp+8] (a.x, shared with the int temps)
   where the original uses [esp+0x14] (b.y). Tried: declaration order of all 5 locals (120 orders),
   b before a, separate declarations, RwV2d[2] / float[4] (frame grows), x2/y2 block-scoped, in the
   a/b block, computed before the call, permuter (2 runs). */
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
int FUN_00494ef0(int);
int FUN_00494ee0(int);
void FUN_00493f80(RwV2d *a, RwV2d *b);
void FUN_0045b350(int);
MGLOBAL(void *, DAT_00771a50, 0x00771a50);
MGLOBAL(void *, DAT_00771a54, 0x00771a54);
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVtx, DAT_00898a20, 4, 0x00898a20);


// MATCH: 0x00473c20 FUN_00473c20
void FUN_00473c20(RwV2d p, RwV2d s, float f5, float f6, float f7, float f8, int raster, uchar alpha)
{
    int i;
    float uu, vv;
    float x2, y2;
    DAT_007d3ff8->stateSet(0x14, (void *)1);
    if (FUN_00494ef0(0)) {
        p.x = p.x * (float)FUN_0042b8b0() / 640.0f;
        p.y = p.y * (float)FUN_0042b8c0() / 480.0f;
        s.x = s.x * (float)FUN_0042b8b0() / 640.0f;
        s.y = s.y * (float)FUN_0042b8c0() / 480.0f;
        {
            RwV2d a, b;
            FUN_00493f80(&a, &b);
            uu = a.x / b.x;
            vv = a.y / b.y;
        }
        x2 = p.x + s.x;
        y2 = p.y + s.y;
        DAT_00898a20[0].x = p.x;
        DAT_00898a20[0].y = p.y;
        DAT_00898a20[0].u = 0.0f;
        DAT_00898a20[0].v = vv;
        DAT_00898a20[1].x = x2;
        DAT_00898a20[1].y = p.y;
        DAT_00898a20[1].u = uu;
        DAT_00898a20[1].v = vv;
        DAT_00898a20[2].x = p.x;
        DAT_00898a20[2].y = y2;
        DAT_00898a20[2].u = 0.0f;
        DAT_00898a20[2].v = 0.0f;
        DAT_00898a20[3].x = x2;
        DAT_00898a20[3].y = y2;
        DAT_00898a20[3].u = uu;
        DAT_00898a20[3].v = 0.0f;
        for (i = 0; i < 4; i++) {
            DAT_00898a20[i].color = ((uint)alpha << 24) | 0xffffff;
            DAT_00898a20[i].z = DAT_007d3ff8->nearZ;
            DAT_00898a20[i].rhw = 1.0f;
        }
        DAT_007d3ff8->stateSet(9, (void *)2);
        DAT_007d3ff8->stateSet(0xc, (void *)1);
        DAT_007d3ff8->stateSet(10, (void *)5);
        DAT_007d3ff8->stateSet(0xb, (void *)6);
        if (raster != 0 && raster != 1) {
            DAT_007d3ff8->stateSet(1, (void *)raster);
            DAT_007d3ff8->stateSet(2, (void *)3);
            DAT_007d3ff8->im2dPrim(4, DAT_00898a20, 4);
            DAT_007d3ff8->stateSet(2, (void *)1);
            return;
        }
        if (FUN_00494ee0(0) == 0) {
            for (i = 0; i < 4; i++)
                DAT_00898a20[i].color = 0xff000000;
            DAT_007d3ff8->stateSet(1, 0);
            FUN_0045b350(0);
        } else {
            if (raster == 1)
                DAT_007d3ff8->stateSet(1, DAT_00771a54);
            else
                DAT_007d3ff8->stateSet(1, DAT_00771a50);
            FUN_0045b350(1);
        }
        DAT_007d3ff8->stateSet(2, (void *)3);
        DAT_007d3ff8->im2dPrim(4, DAT_00898a20, 4);
        DAT_007d3ff8->stateSet(2, (void *)1);
        FUN_0045b350(0);
    }
}
