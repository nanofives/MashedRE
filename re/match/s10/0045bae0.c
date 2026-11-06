// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045bae0 file. Holds five functions, all byte-identical to the original (whole obj symbol,
   relocations masked; relocheck OK):
   - 0x0045ba10 (static, 100%, 45/45 ins): slot record passed in ESI (pilot finding 1). Saves +0xb0, clears 0xb4
     bytes through 0x004b6520, writes two identity matrices. The zero stores had to be written in the original's
     store order (ux, rz, ry, ay, ax, uz, pz, py, px, then the ones az, uy, rx); natural row order gave 64.4%.
   - 0x0045bac0 (static, 100%, 8/8 ins): slot record in ESI; calls type->destroy(slot), clears +0xa8/+0xac.
   - 0x0045bae0 (96.1% instructions, bytes identical over 181 B): Ghidra's size (175) stops before the
     `add esp,4; jmp 0x004039c0` tail. The slot pointer is kept in a local so the +0xb0 store goes through ESI.
   - 0x0045bba0 (100%, 218/218 ins, 738 B): Ghidra's size (856) runs into 0x00453f60, which this function
     tail-calls. What moved it from 68.6%: the state==0 branch written first, the mode block written as
     `if (mode == 0) { edge test } else type->f08(slot, mode)` (the `if (mode != 0)` form duplicated the call
     and stopped the tail merge), `mode = 0` as the first statement of the loop body.
   - 0x0045bf30 (90.3% instructions, bytes identical over 98 B): Ghidra's size stops before pop/pop/ret.
     `(int)` pointer compare for the signed `jl`.
   0x0045be90 and 0x0045bed0 (raw converter files, 100%) also call 0x0045ba10 / 0x0045bac0 with the slot in ESI.
   Residual: none. */
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
typedef struct V3_0045ba10 { float x, y, z; } V3_0045ba10;
typedef struct Mat0045ba10 {
    float rx, ry, rz;
    unsigned int flags;
    float ux, uy, uz;
    int pad1c;
    float ax, ay, az;
    int pad2c;
    V3_0045ba10 pos;
    int pad3c;
} Mat0045ba10;
struct Slot0045ba10;
typedef struct PwType0045ba10 {
    int type;
    int (*activate)(struct Slot0045ba10 *s);
    void (*f08)(struct Slot0045ba10 *s, int mode);
    int (*f0c)(struct Slot0045ba10 *s);
    void (*destroy)(struct Slot0045ba10 *s);
    void (*init)(void);
    void *f18;
    void (*update)(void);
    void *f20;
    void (*reset)(void);
    void (*f28)(void);
    void (*f2c)(void);
    void (*clear)(void);
    void *f34;
    void (*f38)(void);
    void (*f3c)(void);
} PwType0045ba10;
typedef struct Slot0045ba10 {
    Mat0045ba10 m0;
    Mat0045ba10 m1;
    V3_0045ba10 v80;
    float f8c;
    V3_0045ba10 vel0;
    V3_0045ba10 vel1;
    PwType0045ba10 *type;
    int state;
    int index;
} Slot0045ba10;
MGLOBALA(Slot0045ba10, DAT_0088fbe0, 4, 0x0088fbe0);
MGLOBALA(Slot0045ba10, DAT_0068d200, 4, 0x0068d200);
MGLOBALA(int, DAT_0068d4d0, 4, 0x0068d4d0);
MGLOBAL(int, DAT_0068d4e0, 0x0068d4e0);
MGLOBAL(int, DAT_005f9bd8, 0x005f9bd8);
MGLOBALA(PwType0045ba10, DAT_005f9998, 8, 0x005f9998);
void FUN_004b6520(void *p, int n);
int FUN_0042a6b0(const char *name, int a, int b);
void FUN_00459290(void);
void FUN_004587a0(void);
void FUN_00453f30(void);
void FUN_0047b9b0(const char *name, void *out, void *fn);
void FUN_00440bc0(void);
void FUN_00411f30(void);
void FUN_004039c0(void);
typedef struct Pad0045bba0 {
    char b0, b1, b2, b3, b4;
    char pad05[0x14];
    char b19;
    char pad1a[0x32];
} Pad0045bba0;
typedef Pad0045bba0 PadBank0045bba0[16];
typedef struct Player0045bba0 {
    int pad;
    int pad04[3];
} Player0045bba0;
MGLOBALA(PadBank0045bba0, DAT_007f103c, 2, 0x007f103c);
MGLOBALA(Player0045bba0, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(int, DAT_0068d1f0, 4, 0x0068d1f0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
int FUN_0040e350(void);
void FUN_0041f060(int i, Mat0045ba10 *m);
void *FUN_0045bfe0(void);
int FUN_004b4b60(void *world, V3_0045ba10 *pos, int *hit);
int FUN_0045c350(int *hit, V3_0045ba10 *pos);
void FUN_00476880(V3_0045ba10 *pos, float *col, float a, float b);
void FUN_0045c010(int slot, int type);
int FUN_00426c00(void);
void FUN_0045a190(void);
void FUN_00459000(void);
void FUN_00453f60(void);

// MATCH: 0x0045ba10 FUN_0045ba10
static void FUN_0045ba10(Slot0045ba10 *s)
{
    int index = s->index;
    FUN_004b6520(s, 0xb4);
    s->m0.ux = 0.0f; s->m0.rz = 0.0f; s->m0.ry = 0.0f;
    s->m0.ay = 0.0f; s->m0.ax = 0.0f; s->m0.uz = 0.0f;
    s->m0.pos.z = 0.0f; s->m0.pos.y = 0.0f; s->m0.pos.x = 0.0f;
    s->m0.az = 1.0f; s->m0.uy = 1.0f; s->m0.rx = 1.0f;
    s->m0.flags |= 0x20003;
    s->m1.ux = 0.0f; s->m1.rz = 0.0f; s->m1.ry = 0.0f;
    s->m1.ay = 0.0f; s->m1.ax = 0.0f; s->m1.uz = 0.0f;
    s->m1.pos.z = 0.0f; s->m1.pos.y = 0.0f; s->m1.pos.x = 0.0f;
    s->m1.az = 1.0f; s->m1.uy = 1.0f; s->m1.rx = 1.0f;
    s->m1.flags |= 0x20003;
    s->f8c = 0.125f;
    s->index = index;
}

// MATCH: 0x0045bac0 FUN_0045bac0
static void FUN_0045bac0(Slot0045ba10 *s)
{
    s->type->destroy(s);
    s->type = 0;
    s->state = 0;
}

// MATCH: 0x0045bae0 FUN_0045bae0
void FUN_0045bae0(void)
{
    int i;
    int tmp;
    Slot0045ba10 *s;
    for (i = 0; i < 4; i++) {
        s = &DAT_0088fbe0[i];
        FUN_0045ba10(s);
        s->index = i;
        FUN_0045ba10(&DAT_0068d200[i]);
        DAT_0068d4d0[i] = 0;
    }
    DAT_0068d4e0 = FUN_0042a6b0("PowerUpModels.txd", 0, 0);
    for (i = 0; i < DAT_005f9bd8; i++) {
        DAT_005f9998[i].init();
    }
    FUN_00459290();
    FUN_004587a0();
    FUN_00453f30();
    FUN_0047b9b0("powerups_all.lua", &tmp, FUN_00440bc0);
    FUN_00411f30();
    FUN_004039c0();
}

// MATCH: 0x0045bf30 FUN_0045bf30
void FUN_0045bf30(void)
{
    int i;
    Slot0045ba10 *s;
    int *p = DAT_0068d4d0;
    for (s = DAT_0088fbe0; (int)s < (int)(DAT_0088fbe0 + 4); s++, p++) {
        if (s->type != 0)
            FUN_0045bac0(s);
        FUN_0045ba10(s);
        *p = 0;
    }
    for (i = 0; i < DAT_005f9bd8; i++) {
        DAT_005f9998[i].clear();
    }
}

// MATCH: 0x0045bba0 FUN_0045bba0
void FUN_0045bba0(void)
{
    int game;
    int i;
    int mode;
    int ctrl;
    int st;
    V3_0045ba10 old0;
    V3_0045ba10 old1;
    int hit[16];
    Slot0045ba10 *s;
    PwType0045ba10 *t;
    void (*fn)(void);

    game = FUN_0040e350();
    for (i = 0; i < 4; i++) {
        s = &DAT_0088fbe0[i];
        mode = 0;
        old0 = s->m0.pos;
        old1 = s->m1.pos;
        FUN_0041f060(i, &s->m0);
        FUN_0041f060(i, &s->m1);
        s->vel0.x = s->m0.pos.x - old0.x;
        s->vel0.y = s->m0.pos.y - old0.y;
        s->vel0.z = s->m0.pos.z - old0.z;
        s->vel1.x = s->m1.pos.x - old1.x;
        s->vel1.y = s->m1.pos.y - old1.y;
        s->vel1.z = s->m1.pos.z - old1.z;
        if (game != 6)
            continue;
        st = DAT_0068d1f0[i];
        if (st == 4)
            continue;
        if (st == 2) {
            DAT_0068d1f0[i] = 3;
            if (s->type != 0)
                FUN_0045bac0(s);
            continue;
        }
        if (st == 3)
            continue;
        if (s->state != 0) {
            s->v80 = s->m0.pos;
            if (FUN_004b4b60(FUN_0045bfe0(), &s->v80, hit) != 0) {
                if (FUN_0045c350(hit, &s->v80) == 0) {
                    FUN_0045bac0(s);
                    FUN_00476880(&s->v80, DAT_006146fc, 360.0f, 1.5f);
                }
            }
        }
        if (s->state == 0) {
            ctrl = DAT_007f1a14[i].pad;
            if (DAT_007f103c[0][ctrl].b19 && !DAT_007f103c[1][ctrl].b19)
                FUN_0045c010(i, 0x10);
        } else {
            if (s->type->f0c(s) != 0) {
                FUN_0045bac0(s);
            } else {
                ctrl = DAT_007f1a14[i].pad;
                if (DAT_007f103c[0][ctrl].b3)
                    mode = 3;
                if (DAT_007f103c[1][ctrl].b3 && !DAT_007f103c[0][ctrl].b3)
                    mode = 1;
                if (!DAT_007f103c[1][ctrl].b3 && DAT_007f103c[0][ctrl].b3)
                    mode = 2;
                if (mode == 0) {
                    if (DAT_007f103c[0][ctrl].b4 && !DAT_007f103c[1][ctrl].b4)
                        FUN_0045bac0(s);
                } else
                    s->type->f08(s, mode);
            }
        }
    }
    for (i = 0; i < DAT_005f9bd8; i++) {
        DAT_005f9998[i].update();
    }
    for (i = 0; i < DAT_005f9bd8; i++) {
        t = &DAT_005f9998[i];
        switch (FUN_00426c00()) {
        case 2:
            fn = t->f3c;
            break;
        case 0x21:
            fn = t->f38;
            break;
        default:
            fn = 0;
            break;
        }
        if (fn != 0)
            fn();
    }
    FUN_0045a190();
    FUN_00459000();
    FUN_00453f60();
}
