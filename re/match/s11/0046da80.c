// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046da80 (helper h4): best 61.2% (regs 68.2%, ops 82.4%), class (b). File holds static callee 0x0046c5f0 (register args EAX/ECX/EDX/ESI, pilot finding 1; 100% here). Ghidra size 339 stops before the epilogue (real end 0x0046dbdd). Started from pilot a5 (50.6%). Moves: V3 initializer for the zero vector (immediate stores like the original), f30=0 first in the wheel loop. Remaining: our build keeps i in EBP across the function (original reloads it from the stack for FUN_00480d60, no EBP push), and the loop zero stores reuse EDX instead of immediates. Tried: float vs int zero globals, chained zeroing, pointer loop, no car/matrix locals (49%), permuter 300 s (66.3% only with in-place 19%, rejected). Literal 0.2778f = 0x005cea60. */
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
typedef struct { float x, y, z; } V3_0046da80;
typedef struct { float m[12]; V3_0046da80 pos; float pad; } Mtx_0046da80;           /* 0x40 */
typedef struct { float x; float y; float z; char p0[0x30 - 0xc]; float f30; char p1[0xc4 - 0x34]; } Wheel_0046da80;
typedef struct {
    char p0[0x16c];
    Wheel_0046da80 w[4];              /* +0x16c, stride 0xc4 */
    char p1[0x4a4 - 0x47c];
    float f4a4;                       /* +0x4a4 */
    char p2[0x928 - 0x4a8];
    Mtx_0046da80 mt[2];               /* +0x928 */
    int p3;                           /* +0x9a8 */
    int mi;                           /* +0x9ac */
    char p4[0x9c8 - 0x9b0];
    V3_0046da80 nrm;                  /* +0x9c8 */
    char p5[0xd04 - 0x9d4];
} Car_0046da80;
typedef struct { V3_0046da80 p; float r; int a, b; int n; } Query_0046da80;
MGLOBALA(Car_0046da80, DAT_008815a0, 16, 0x008815a0);
MGLOBALA(V3_0046da80, DAT_00881560, 4, 0x00881560);
MGLOBAL(int, DAT_0088e5e0, 0x0088e5e0);
MGLOBAL(int, DAT_0088e5e4, 0x0088e5e4);
MGLOBAL(int, DAT_0088e5e8, 0x0088e5e8);
MGLOBAL(int, DAT_0088e5ec, 0x0088e5ec);
MGLOBAL(V3_0046da80, DAT_0088e600, 0x0088e600);
MGLOBAL(float, DAT_0088e60c, 0x0088e60c);
MGLOBALA(V3_0046da80, DAT_0088e620, 4, 0x0088e620);
MGLOBAL(int, DAT_0088e654, 0x0088e654);
MGLOBAL(int, DAT_00828320, 0x00828320);
float FUN_004c3b30(float x);
void FUN_004c3d90(V3_0046da80 *out, V3_0046da80 *in, int n, Mtx_0046da80 *m);
int FUN_00468b80(void);
void FUN_00538c80(int w, Query_0046da80 *q, int (*cb)(void), int *res);
int FUN_0046d880(Car_0046da80 *c, int *res);
void FUN_00480d60(int i, int f);

// MATCH: 0x0046c5f0 FUN_0046c5f0
static void FUN_0046c5f0(float *a, float *c, float *b, float *out)
{
    float ax = a[0] - c[0], ay = a[1] - c[1], az = a[2] - c[2];
    float bx = a[0] - b[0], by = a[1] - b[1], bz = a[2] - b[2];
    float nx = bz * ay - by * az;
    float ny = bx * az - bz * ax;
    float nz = by * ax - bx * ay;
    float l = nx * nx + ny * ny + nz * nz;
    if (l >= 0.0f) {
        l = FUN_004c3b30(l);
        if (l != 0.0f) {
            l = 1.0f / l;
            out[0] = nx * l;
            out[1] = ny * l;
            out[2] = l * nz;
        }
    }
}

// MATCH: 0x0046da80 FUN_0046da80
int FUN_0046da80(int i, int p)
{
    Car_0046da80 *c = &DAT_008815a0[i];
    Mtx_0046da80 *m = &c->mt[c->mi];
    V3_0046da80 z = {0.0f, 0.0f, 0.0f};
    Query_0046da80 q;
    int j, r;
    DAT_0088e5e0 = 0;
    DAT_0088e5e4 = 0;
    DAT_0088e5e8 = 0;
    DAT_0088e5ec = 0;
    for (j = 0; j < 4; j++) {
        c->w[j].f30 = 0.0f;
        DAT_00881560[j].x = c->w[j].x * 0.2778f;
        DAT_00881560[j].y = 0.0f;
        DAT_00881560[j].z = c->w[j].z * 0.2778f;
    }
    FUN_004c3d90(&DAT_0088e600, &z, 1, m);
    FUN_004c3d90(DAT_0088e620, DAT_00881560, 4, m);
    FUN_0046c5f0(&DAT_0088e620[0].x, &DAT_0088e620[2].x, &DAT_0088e620[3].x, &c->nrm.x);
    q.n = 3;
    q.p = m->pos;
    q.r = c->f4a4;
    DAT_0088e60c = 0.0f;
    DAT_0088e654 = p;
    FUN_00538c80(*(int *)(p + 8), &q, FUN_00468b80, &DAT_00828320);
    r = FUN_0046d880(c, &DAT_00828320);
    FUN_00480d60(i, 8);
    return r;
}
