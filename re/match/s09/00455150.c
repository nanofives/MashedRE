// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455150 (h4): 99.4% (regs 99.4%, ops 99.4%), 166/166 ins. NOT 100%. Raw converter 22.0%.
   What moved it: the two records as pointers into struct arrays (0x2c-B at 0x006885d0 in EDI, 0x6c-B at
   0x006883b0 in ESI), the shared `k` for +0x54 with the goto past it (Ghidra's shape), `count <= 0` (the zero
   then lives in EBP: 88.6% -> 98.8%), `(float)(acos(..) * 57.29..) - 90.0f` so 90 stays a float operand,
   0x0045bfd0 with no argument, 0x004a3384 declared as a plain `double (double)` (pilot finding 5).
   Residual (a): one scheduling slot. The original stores the angle local (`fstp [esp+0x2c]`) after the push
   of 0x006146f0, ours one push earlier (same slot). Tried: the angle as float / double local, `*=`, `-1.0f *`,
   a ternary into +0x48, assigning +0x48 directly (95.2%), the dot-product vector in a local, a third (float)
   parameter for the slot, and the permuter (300 s, no gain). */
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
typedef struct V3_00455150 { float x, y, z; } V3_00455150;
typedef struct Owner00455150 {
    char pad00[0x20];
    V3_00455150 dir;        /* +0x20 */
    char pad2c[0x90 - 0x2c];
    V3_00455150 vel;        /* +0x90 */
    char pad9c[0xb0 - 0x9c];
    int slot;               /* +0xb0 */
} Owner00455150;
typedef struct Obj00455150 { int f00; void *frame; } Obj00455150;
typedef struct A00455150 {
    void *owner;            /* +0x00 */
    int f04, count;         /* +0x04 */
    int link;               /* +0x0c */
    int f10;                /* +0x10 */
    float f14;              /* +0x14 */
    Obj00455150 *f18;       /* +0x18 */
    int f1c;                /* +0x1c */
    Obj00455150 *f20;       /* +0x20 */
    float f24, f28;         /* +0x24 */
} A00455150;                /* 0x2c, records at 0x006885d0 */
typedef struct B00455150 {
    int f00, f04, f08;
    V3_00455150 pos;        /* +0x0c */
    int f18, f1c, f20, f24, f28, f2c;   /* +0x18 */
    float f30;              /* +0x30 */
    int f34, f38;           /* +0x34 */
    int f3c;                /* +0x3c */
    int f40, f44;           /* +0x40 */
    float f48, f4c;         /* +0x48 */
    int f50, f54, f58;      /* +0x50 */
    Obj00455150 *obj;       /* +0x5c */
    int f60, f64;           /* +0x60 */
    int f68;                /* +0x68 */
} B00455150;                /* 0x6c, records at 0x006883b0 */
MGLOBALA(A00455150, DAT_006885d0, 5, 0x006885d0);
MGLOBALA(B00455150, DAT_006883b0, 5, 0x006883b0);
MGLOBALA(float, DAT_00614708, 3, 0x00614708);
MGLOBALA(float, DAT_006146f0, 3, 0x006146f0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
void FUN_0045a110(int p0);
int FUN_0045a0f0(int p0);
int FUN_0045a0d0(int p0);
int FUN_00407640(int p0);
char *FUN_004c0ed0(void *p0);
int FUN_004c1340(void *p0, V3_00455150 *p1, int p2);
float FUN_004c3ac0(V3_00455150 *p0);
float FUN_004726f0(float *p0, V3_00455150 *p1);
double FUN_004a3384(double x);
void FUN_00474d60(void *p0, int p1);
void *FUN_004e4800(void *p0);
void *FUN_004e45b0(void *p0, void *p1);
void *FUN_004e4450(void *p0, void *p1);
void *FUN_0045bfd0(void);
void FUN_004899c0(int p0, V3_00455150 *p1);
void FUN_00465e80(int p0, V3_00455150 *p1, int p2);
void FUN_00459540(int p0);
// MATCH: 0x00455150 FUN_00455150
void FUN_00455150(Owner00455150 *o, int ev)
{
    int i = o->slot;
    A00455150 *a = &DAT_006885d0[i];
    B00455150 *b;
    int k;
    float t;
    void *p;
    FUN_0045a110(i);
    if (ev != 2)
        return;
    if (a->count <= 0)
        return;
    a->count--;
    b = &DAT_006883b0[a->count + i];
    a->f1c = 1;
    b->f50 = 1;
    a->f28 = 0.5f;
    if (a->f10 == 0) {
        k = -1;
        b->f58 = -1;
    } else {
        k = FUN_0045a0f0(i);
        if (k == -1) {
            b->f58 = FUN_00407640(FUN_0045a0d0(i));
            b->f54 = -1;
            goto L;
        }
        b->f58 = -1;
    }
    b->f54 = k;
L:
    b->pos = *(V3_00455150 *)(FUN_004c0ed0(a->f20->frame) + 0x30);
    FUN_004c1340(b->obj->frame, &b->pos, 0);
    b->f18 = 0;
    b->f28 = 0;
    b->f24 = 0;
    b->f20 = 0;
    b->f1c = 0;
    b->f2c = 0;
    b->f34 = 0;
    b->f38 = 0;
    b->f30 = FUN_004c3ac0(&o->vel) + 0.075f;
    b->f3c = i;
    t = (float)(FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205);
    if (FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f)
        t = t * -1.0f;
    b->f48 = t;
    b->f4c = (float)(FUN_004a3384(FUN_004726f0(DAT_006146fc, &o->dir)) * 57.29577791868205) - 90.0f;
    b->f44 = 0;
    b->f40 = 0;
    b->f60 = 1;
    p = FUN_004e4800(b->obj);
    if (p != FUN_0045bfd0()) {
        if (p != 0)
            FUN_004e45b0(p, b->obj);
        FUN_004e4450(FUN_0045bfd0(), b->obj);
    }
    FUN_00474d60(b->obj, 1);
    FUN_004899c0(b->f68, &b->pos);
    FUN_00465e80(0x12, &b->pos, b->f3c);
    FUN_00465e80(0x14, &b->pos, b->f3c);
    FUN_00459540(o->slot);
}
