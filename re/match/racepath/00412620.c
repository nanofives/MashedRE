// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH2 0x00412620 (first hand source; raw g2s 17.6%): NOT CLOSED, 49.4% insn (170 vs 167 insns).
   Constants are literals read from the anchored image: 1e-4f 0x005cd03c, 100.0f 0x005cc568, -100.0f 0x005cd038,
   4.0f 0x005cc35c, 0.001f 0x005cc558, 5.0 (double) 0x005cd030, 5.0f 0x005cc358, 0.5f 0x005cc32c, 0.05f 0x005cc9a0.
   DAT_005f2a40 is .data (writable), so it stays a global. The f20 test is `!(f20 <= 1e-4f)`, which gives the
   `jnp` at 0x004126b3 (`f20 > 1e-4f` gives `jne`). Residual, two structural: (1) the original keeps the counter i
   and n in memory ([esp+0] / [esp+4], i = 0 stored before the 0x0041f300 call at 0x00412625) and walks three
   pointer IVs, esi = slot, edi = &slot.p0, ebx = &slot.p1 (0x0041264a..0x00412654); ours drops i, counts n down
   and walks one pointer (base slot+0x28), deriving &p0 / &p1 with lea. Tried: index vs slot pointer vs p0/p1
   pointer locals, unsigned, unsized array, separate aliased p0/p1 arrays, while / do-while / for forms, a
   forceinline per-slot helper: no change in loop shape. (2) cl folds `dx * -100.0f` into a reuse of `dx * 100.0f`
   (fsub), where the original keeps dx / dy on the x87 stack and multiplies by the -100.0f literal (0x00412769).
   The frame is 0x3c against 0x44. */
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
typedef struct {
    int obj;       /* +0x00 */
    int active;    /* +0x04 */
    V3 p0;         /* +0x08 */
    V3 p1;         /* +0x14 */
    float f20;     /* +0x20 */
    int a24;       /* +0x24 */
    int a28;       /* +0x28 */
} Trail;           /* 0x2c B, array at 0x0063bb38 */
MGLOBALA(Trail, DAT_0063bb38, 1, 0x0063bb38);
MGLOBAL(V3, DAT_005f2a40, 0x005f2a40);
MGLOBAL(int, DAT_007f101c, 0x007f101c);
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
void FUN_0041f300(int a);
int FUN_00412130(void);
int FUN_0047ce00(int a);
int FUN_004671a0(int a);
float FUN_00474e70(V3 *a, V3 *b, V3 *c);
int FUN_00459400(int obj, int f);
int FUN_004c1340(int frame, V3 *v, int op);
void FUN_004b42c0(int frame, V3 *at, V3 *up);
int FUN_004c13e0(int frame, V3 *v, int op);

// MATCH: 0x00412620 FUN_00412620
void FUN_00412620(void)
{
    int i = 0;
    int n;
    FUN_0041f300(0);
    n = FUN_00412130();
    for (; i < n; i++) {
        V3 scale;
        V3 up;
        int frame;
        V3 a, b;
        float dz, d, ang;
        scale = DAT_005f2a40;
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        frame = *(int *)(DAT_0063bb38[i].obj + 4);
        if (DAT_0063bb38[i].active != 0 && !(DAT_0063bb38[i].f20 <= 0.0001f)) {
            if (FUN_0047ce00(DAT_0063bb38[i].a24) == 8 || FUN_0047ce00(DAT_0063bb38[i].a28) == 8) {
                DAT_0063bb38[i].active = 0;
            } else {
                float dx = DAT_0063bb38[i].p0.x - DAT_0063bb38[i].p1.x;
                float dy = DAT_0063bb38[i].p0.y - DAT_0063bb38[i].p1.y;
                dz = DAT_0063bb38[i].p0.z - DAT_0063bb38[i].p1.z;
                a = DAT_0063bb38[i].p0;
                a.x = dx * 100.0f + a.x;
                b = DAT_0063bb38[i].p1;
                a.y = 100.0f * dy + a.y;
                scale.x = 4.0f;
                scale.y = 4.0f;
                a.z = dz * 100.0f + a.z;
                b.x = dx * -100.0f + b.x;
                b.y = dy * -100.0f + b.y;
                b.z = dz * -100.0f + b.z;
                scale.z = DAT_0063bb38[i].f20 * 4.0f;
                d = FUN_00474e70(&a, &b, (V3 *)(*(int *)(FUN_004671a0(0) + 4) + 0x40));
                if (d < 0.001f)
                    d = 0.001f;
                if (d > 5.0)
                    d = 5.0f;
                d = d * 0.5f;
                scale.x = scale.x * d;
                scale.y = d * scale.y;
                FUN_00459400(DAT_0063bb38[i].obj, 0);
                ang = DAT_007f101c * 0.05f;
                up.x = (float)cos(ang);
                up.y = (float)sin(ang);
                FUN_004c1340(frame, &DAT_0063bb38[i].p0, 0);
                FUN_004b42c0(frame, &DAT_0063bb38[i].p1, &up);
                FUN_004c13e0(frame, &scale, 1);
            }
        }
    }
}
