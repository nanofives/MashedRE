// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-NEAR 0x0040ca00 (first hand source; raw g2s 46.9%): CLOSED, 908 B BYTE-IDENTICAL (Ghidra body
   905 B, symbol ends 0x0040cd8b, next entry 0x0040cd90), relocheck OK. Floats are __real@ literals (62500.0f
   0x005ccd20, 100.0f 0x005cc568, 90.0f 0x005ccad0, 0.3f 0x005cc99c, 0.4f 0x005ccac0, 3e-05f 0x005ccd1c,
   0.00015f 0x005ccd18, 5e-05f 0x005ccd14, 0.1f 0x005cc56c). Moved it: both loops as `i = 4; do { i--; ... }
   while (i != 0);` (cl strength-reduces to the 0x007f1a54 / 0x008856ac / 0x007f0a34 pointer walks and tests the
   pointer, 0x0040cc45 / 0x0040cd78; `for (i = 3; i >= 0; i--)` gives offset IVs, 44% to 57%); each parity branch
   carries its own `if (h != 0) FUN_0040c690(...)` and cl cross-jumps the tails (0x0040cb9f, 0x0040cd41; one call
   after the if/else gave 16 B -> 94%); the loop-2 V3 separate from the loop-1 V3 (frame 0x3c); the loop-2
   FUN_0046d320 out value in a block-scoped `c2` (cl packs it into the loop-1 pointer spill at [esp+0x14]; a
   function-scope c2 grows the frame to 0x40); `n` block-scoped in the loop-1 body so the loop-2 float f packs into
   its slot [esp+0x20] (2 B to 0). The 3-term sum is written z, x, y: cl emits y*y + x*x, then + z*z
   (0x0040cac4..0x0040cade), the order the original holds; the source order y, x, z emitted z first. */
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
typedef struct { int idx; int pad[3]; } Slot10;          /* 0x10 B, 4 at 0x007f1a14 */
typedef struct { uchar b0; uchar b1; uchar pad[0x4a]; } Rec4c;   /* 0x4c B at 0x007f103c */
typedef struct { int busy; int pad[0x340]; } Ply;        /* 0xd04 B, 4 at 0x0088229c */
typedef struct { int t; int mode; int pad; } Fx0c;       /* 0x0c B, 4 at 0x007f0a04 */
MGLOBALA(Slot10, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(Rec4c, DAT_007f103c, 1, 0x007f103c);
MGLOBALA(Ply, DAT_0088229c, 4, 0x0088229c);
MGLOBALA(Fx0c, DAT_007f0a04, 4, 0x007f0a04);
MGLOBAL(int, DAT_007f101c, 0x007f101c);
void  FUN_0040c690(int p0, int p1, V3 *p2, int p3, int p4, int p5);
int   FUN_0041efe0(int p0);
int   FUN_0041f290(int p0, int p1);
int   FUN_0045dd50(int p0);
void  FUN_0046cb30(V3 *p0, int p1);
void  FUN_0046cc10(int *p0, int p1);
void  FUN_0046d320(int *p0, int p1, int p2);
int   FUN_0046d570(float *p0, int p1);
void  FUN_0046d6a0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
void  FUN_00485e50(int p0, int p1, float p2, float p3);
float FUN_004c3ac0(V3 *p0);

// MATCH: 0x0040ca00 _FUN_0040ca00
void FUN_0040ca00(void)
{
    float sp;
    V3 v;
    float ang;
    int c;
    int h;
    int src;
    int hot;
    int i;
    V3 w;
    V3 v2;
    float len;
    float f;

    sp = 0.0f;
    i = 4;
    do {
        i--;
        if (DAT_007f1a14[i].idx > -1) {
            int n;
            FUN_0046d6a0(&sp, i);
            FUN_0046cb30(&v, i);
            src = FUN_0041f290(i, 0x13);
            FUN_0046cc10(&n, i);
            if (n > 1) {
                h = FUN_0045dd50(i);
                if (FUN_0041efe0(i) == 1 && h != 0) {
                    FUN_00485e50(i, 5, 0.2f, 0.1f);
                    FUN_00485e50(i, 6, 0.2f, 0.1f);
                }
                if (DAT_0088229c[i].busy == 0) {
                    if (v.z * v.z + v.x * v.x + v.y * v.y < 62500.0f)
                        continue;
                    hot = sp > 100.0f;
                    if (FUN_0046d570(&ang, i) != 0 && ang < 90.0f
                        && DAT_007f103c[DAT_007f1a14[i].idx].b1 > 0x40)
                        hot = 1;
                } else {
                    if (FUN_00472650(0.0f, 1.0f) < 0.3f)
                        continue;
                    hot = 1;
                }
                if (DAT_007f101c % 2 == 0) {
                    h = FUN_0041f290(i, 0x1d);
                    FUN_0046d320(&c, i, 0);
                    if (h != 0)
                        FUN_0040c690(src, h + 0x30, 0, c, hot, i);
                } else {
                    h = FUN_0041f290(i, 0x1e);
                    FUN_0046d320(&c, i, 2);
                    if (h != 0)
                        FUN_0040c690(src, h + 0x30, 0, c, hot, i);
                }
                if (hot != 0) {
                    h = FUN_0041f290(i, 0x20);
                    FUN_0046d320(&c, i, 1);
                    if (h != 0)
                        FUN_0040c690(src, h + 0x30, 0, c, hot, i);
                    h = FUN_0041f290(i, 0x1f);
                    FUN_0046d320(&c, i, 3);
                    if (h != 0)
                        FUN_0040c690(src, h + 0x30, 0, c, hot, i);
                }
            }
        }
    } while (i != 0);
    i = 4;
    do {
        i--;
        if (DAT_007f0a04[i].t != 0) {
            DAT_007f0a04[i].t = DAT_007f0a04[i].t - 0x18;
            if (DAT_007f0a04[i].t < 0)
                DAT_007f0a04[i].t = 0;
            if (DAT_007f0a04[i].mode == 2) {
                if (FUN_00472650(0.0f, 1.0f) > 0.4f) {
                    int c2;
                    src = FUN_0041f290(i, 0x13);
                    FUN_0046cb30(&v2, i);
                    len = FUN_004c3ac0(&v2);
                    w.x = 0.0f;
                    w.y = 3e-05f * len;
                    w.z = len * 0.00015f;
                    if (DAT_007f101c % 2 == 0) {
                        h = FUN_0041f290(i, 0x20);
                        FUN_0046d320(&c2, i, 1);
                        if (h != 0)
                            FUN_0040c690(src, h + 0x30, &w, c2, 1, i);
                    } else {
                        h = FUN_0041f290(i, 0x1f);
                        FUN_0046d320(&c2, i, 3);
                        if (h != 0)
                            FUN_0040c690(src, h + 0x30, &w, c2, 1, i);
                    }
                }
            } else {
                f = (float)DAT_007f0a04[i].t * 5e-05f + 0.1f;
                FUN_00485e50(i, 5, 0.8f, f);
                FUN_00485e50(i, 6, 0.8f, f);
            }
        }
    } while (i != 0);
}
