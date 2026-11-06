// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455030 (h4): 100% (regs 100%), 18/18 ins. Raw converter 27.8%. The file also holds its two
   callees 0x00454fa0 and 0x00454ff0 (each 100%, 21/21 ins) as `static` functions defined first: both take
   the record pointer in ESI (pilot finding 1). 0x00454ff0 has a second caller, 0x00455910.
   What moved 0x00455030: two pointer walks (5 records of 0x2c at 0x006885d0, 5 of 0x6c at 0x006883b0), `a`
   declared before `b`, and a signed (int) end compare (`jl`); the index loop swapped EBX/EDI (77.8%),
   an unsigned pointer compare gave `jb` (88.9%). Residual: none.
   The file also holds 0x00455910 (100%, 40/40 ins), the other caller of the static 0x00454ff0: it takes the
   record in EAX (pilot finding 1), so the file ends with a stand-in caller (the record loop of 0x00455c90,
   unmatched; no MATCH line on it, built only outside MATCH_PORT). Constants are float literals (0.2f, 360.0f, 1.0f, 1.9f, 40/200/250). */
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
typedef struct Rec00454fa0 {
    int f00, f04, f08;  /* +0x00 */
    int pad0c[3];
    void *frame;        /* +0x18 */
    int f1c;            /* +0x1c */
    int pad20[2];
    int f28;            /* +0x28 */
} Rec00454fa0;          /* 0x2c, 5 records at 0x006885d0 */
typedef struct V3_00454ff0 { float x, y, z; } V3_00454ff0;
typedef struct V4_00454ff0 { V3_00454ff0 p; float w; } V4_00454ff0;
typedef struct Rec00454ff0 {
    int pad00[3];
    V3_00454ff0 pos;    /* +0x0c */
    int pad18[9];
    int f3c;            /* +0x3c */
    int pad40[4];
    int f50, f54, f58;  /* +0x50 */
    void *frame;        /* +0x5c */
    int f60;            /* +0x60 */
    int pad64[2];
} Rec00454ff0;          /* 0x6c, 5 records at 0x006883b0 */
MGLOBALA(Rec00454ff0, DAT_006883b0, 5, 0x006883b0);
MGLOBALA(Rec00454fa0, DAT_006885d0, 5, 0x006885d0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
void FUN_00474d60(void *p0, int p1);
void *FUN_004e4800(void *p0);
void *FUN_004e45b0(void *p0, void *p1);
void FUN_00474de0(V3_00454ff0 *p0, V3_00454ff0 *p1, float p2);
void FUN_00476880(V3_00454ff0 *p0, float *p1, float p2, float p3);
void FUN_00453eb0(V4_00454ff0 *p0, float p1, float p2, float p3, int p4);
void FUN_004661a0(int p0, int p1);
// MATCH: 0x00454fa0 FUN_00454fa0
static void FUN_00454fa0(Rec00454fa0 *r)
{
    void *p;
    r->f04 = 0;
    r->f08 = 1;
    r->f00 = 0;
    r->f28 = 0;
    r->f1c = 0;
    FUN_00474d60(r->frame, 0);
    p = FUN_004e4800(r->frame);
    if (p != 0)
        FUN_004e45b0(p, r->frame);
}

// MATCH: 0x00454ff0 FUN_00454ff0
static void FUN_00454ff0(Rec00454ff0 *r)
{
    void *p;
    r->f54 = -1;
    r->f58 = -1;
    r->f50 = 0;
    r->f60 = 0;
    FUN_00474d60(r->frame, 0);
    p = FUN_004e4800(r->frame);
    if (p != 0)
        FUN_004e45b0(p, r->frame);
}

// MATCH: 0x00455030 FUN_00455030
void FUN_00455030(void)
{
    Rec00454fa0 *a = DAT_006885d0;
    Rec00454ff0 *b = DAT_006883b0;
    do {
        FUN_00454fa0(a);
        FUN_00454ff0(b);
        b++;
        a++;
    } while ((int)b < (int)(DAT_006883b0 + 5));
}

// MATCH: 0x00455910 FUN_00455910
static void FUN_00455910(Rec00454ff0 *r)
{
    V3_00454ff0 a;
    V4_00454ff0 b;
    FUN_00474de0(&r->pos, &a, 0.2f);
    FUN_00476880(&a, DAT_006146fc, 360.0f, 1.0f);
    FUN_00454ff0(r);
    b.p = r->pos;
    b.w = 1.9f;
    FUN_00453eb0(&b, 40.0f, 200.0f, 250.0f, 2);
    FUN_004661a0(0x14, r->f3c);
}

#ifndef MATCH_PORT
/* stand-in caller for 0x00455910 (EAX-passed record): the record loop of 0x00455c90 (not matched) */
void StandIn_00455910(void)
{
    Rec00454ff0 *r;
    for (r = DAT_006883b0; r < DAT_006883b0 + 5; r++)
        if (r->f60 != 0)
            FUN_00455910(r);
}
#endif
