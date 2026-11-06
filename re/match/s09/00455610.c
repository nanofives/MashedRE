// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455610 (h4): 90.1% (regs 90.1%, ops 97.2%), 69/71 ins. Raw converter 48.8%. NOT 100%.
   The record pointer is ESI-passed (pilot finding 1): the file holds a stand-in caller (the record loop
   of 0x00455c90, unmatched); no MATCH line on it, built only outside MATCH_PORT.
   What moved it: sin() written twice (the compiler CSEs it and consumes the value in the second use,
   +3 pts over a `double s` local), 15.0 / 20.0 as MGLOBAL doubles (0x005ce4b0 / 0x005ce4a8; as literals the
   first sum's operand order flipped, 80.6%), the other constants as float literals.
   Residual (b): the velocity block. The original keeps the new +0x30 on the x87 stack and multiplies
   `fld [m+0x34]; fmul st(1)` for y and z, then drops it (`fstp st(0)`); ours consumes it in the z multiply,
   and adds px as `fld vx; fadd [eax]` instead of `fld [eax]; fadd vx`. Tried: a float local t (before /
   after the +0x30 store, in a block), operand order in every product and sum, a float* to px, and the
   permuter (91.5% only by swapping the vx / vy stores, not kept). */
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
typedef struct Mtx00455610 {
    float f00[4];
    float f10[4];       /* +0x10 */
    float f20[4];
    float f30, f34, f38;/* +0x30 */
} Mtx00455610;
typedef struct Obj00455610 { int f00; Mtx00455610 *frame; } Obj00455610;
typedef struct Rec00455610 {
    int f00, f04, f08;
    float px, py, pz;   /* +0x0c */
    float f18;          /* +0x18 */
    float vx, vy, vz;   /* +0x1c */
    float f28;          /* +0x28 */
    float f2c;          /* +0x2c */
    float f30;          /* +0x30 */
    int pad34[5];
    float f48;          /* +0x48 */
    float f4c;          /* +0x4c */
    int f50, f54, f58;  /* +0x50 */
    Obj00455610 *obj;   /* +0x5c */
    int f60;            /* +0x60 */
    int pad64[2];
} Rec00455610;          /* 0x6c, 5 records at 0x006883b0 */
MGLOBALA(Rec00455610, DAT_006883b0, 5, 0x006883b0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(double, DAT_005ce4b0, 0x005ce4b0);
MGLOBAL(double, DAT_005ce4a8, 0x005ce4a8);
#ifdef __cplusplus
extern "C" double sin(double x);
#else
double sin(double x);
#endif
#pragma intrinsic(sin)
void FUN_004c15c0(Mtx00455610 *p0);
int FUN_004c1520(Mtx00455610 *p0, float *p1, float p2, int p3);
int FUN_004c1340(Mtx00455610 *p0, float *p1, int p2);
// MATCH: 0x00455610 FUN_00455610
static void FUN_00455610(Rec00455610 *r)
{
    Mtx00455610 *m = r->obj->frame;
    float a = (float)(DAT_005ce4b0 * sin(r->f18 * 20.0f) + r->f48);
    float b = (float)(sin(r->f18 * 20.0f) * DAT_005ce4a8 + r->f4c);
    FUN_004c15c0(m);
    FUN_004c1520(m, DAT_006146fc, a, 0);
    FUN_004c1520(m, m->f10, b, 2);
    r->f2c = r->f2c + 0.0001f;
    if (r->f2c > 0.005f)
        r->f2c = 0.005f;
    r->f30 = r->f30 + r->f2c;
    r->vx = r->f30 * m->f30;
    r->vy = m->f34 * r->f30;
    r->vz = m->f38 * r->f30;
    r->vy = r->f28 + r->vy;
    r->px = r->px + r->vx;
    r->py = r->vy + r->py;
    r->pz = r->vz + r->pz;
    FUN_004c1340(m, &r->px, 2);
}

#ifndef MATCH_PORT
/* stand-in caller: the record loop of 0x00455c90 (not matched) */
void StandIn_00455610(void)
{
    Rec00455610 *r;
    for (r = DAT_006883b0; r < DAT_006883b0 + 5; r++)
        if (r->f54 == -1)
            FUN_00455610(r);
}
#endif
