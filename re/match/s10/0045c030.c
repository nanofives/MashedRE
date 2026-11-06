// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c030: 98.6% (regs 98.6%), 71/71 ins. Raw converter did not compile.
   Builds a matrix: right = normalize(a - b), up = normalize(cross), at = dir, then 0x004c45f0 and
   0x004c51a0(m, pos or a local zero vector, 2).
   Residual (a): the discarded x87 result of the first 0x004c39b0 call is popped (fstp st(0)) before the next
   `lea ecx` in the original, after it in ours. Tried: callee return float / double / long double / void,
   (V3 *)m vs &m->right, swapped call order, locals order; permuter 185 variants, no move. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct V3_0045c030 { float x, y, z; } V3_0045c030;
typedef struct Mat0045c030 {
    V3_0045c030 right; int flags;
    V3_0045c030 up; int pad1c;
    V3_0045c030 at; int pad2c;
    V3_0045c030 pos; int pad3c;
} Mat0045c030;
typedef struct Src0045c030 {
    V3_0045c030 dir;
    int pad0c[4];
    V3_0045c030 *a;
    V3_0045c030 *b;
} Src0045c030;
float FUN_004c39b0(V3_0045c030 *out, V3_0045c030 *in);
void FUN_004c45f0(Mat0045c030 *m);
void FUN_004c51a0(Mat0045c030 *m, V3_0045c030 *pos, int mode);

// MATCH: 0x0045c030 FUN_0045c030
void FUN_0045c030(Mat0045c030 *m, Src0045c030 *src, V3_0045c030 *pos)
{
    V3_0045c030 d;
    V3_0045c030 zero;
    V3_0045c030 c;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    if (pos == 0)
        pos = &zero;
    d.x = src->a->x - src->b->x;
    d.y = src->a->y - src->b->y;
    d.z = src->a->z - src->b->z;
    c.x = d.z * src->dir.y - d.y * src->dir.z;
    c.y = d.x * src->dir.z - d.z * src->dir.x;
    c.z = d.y * src->dir.x - d.x * src->dir.y;
    FUN_004c39b0(&m->right, &d);
    FUN_004c39b0(&m->up, &c);
    m->at = src->dir;
    FUN_004c45f0(m);
    FUN_004c51a0(m, pos, 2);
}
