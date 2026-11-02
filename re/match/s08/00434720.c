// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00434720 (8369 B, menu, top-down map row T11, called from MenuDrawLoop 0x0043c5b0), by hand (h2).
   Best: 57.1% exact (regs 70.5%, ops 97.1%), 2198/2194 ins. cxx: ok (VS2022 /TP, no error lines).
   The raw converter output did not compile (Ghidra typed floats/ints/pointers through shared stack slots).
   Rewritten by hand from the disassembly. What moved the score:
   - 0x0042f8d0 takes its alpha in AL and two 8-byte V2 structs by value (pos, size): static callee in this file,
     defined before the caller (pilot finding 1); V2 by value also explains the store-then-reload of every
     constant x/y/w/h in both functions (struct members are not constant-propagated). This file holds
     0x0042f8d0 (100%), 0x00430b30 (EAX/ECX register args, 100%), 0x0042b8b0 and 0x0042b8c0 (100%).
     0x0042b8b0/0x0042b8c0 defined in the file let the compiler keep the cos() value in st(0) across the
     calls (0x00435ed8..0x00435f16), as the original does.
   - FUN_00472c60 / 004736c0 / 00473870 / 004739f0 take (V2 pos, V2 size, RGBA color, ...).
   - double dr assigned before the do-while and again after the sin line gives the 8-aligned frame
     (and esp, -8) and the qword temp at [esp+0x140].
   - if/else polarity from the block layout: DAT_0067ecdc != 0 first, DAT_0067ed74 >= 359 first,
     scroll clamp scr > -1500 first; scroll tail scr = FUN_004282a0(..) + scr + 8 shared by three branches.
   - 0x00434f1f: the original tests (!DAT_0067e9fc) == 2 (never true), kept as is (C++ warning C4806 only).
   Residual class (b) source shape: stack-slot packing and register choice. Locals are laid out by use count
   with lifetime-based slot sharing (checked with /FAs on a test file); ours puts several scalars at other
   offsets (e.g. cW 0x88 vs 0x78, f94 shares 0x30) and swaps ebx/edi for the zero constant vs n (2) from the
   first block on. Tried: declaration order (no effect), block-scoped nstars/dr (no effect), array order and
   sizes (no effect or worse), separate scroll id (49.6%, worse), zero-store order (no effect). Permuter run
   (--jobs 2) found no gain over 19 min before it was stopped. Ran out of budget (150 min) on the frame layout. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { uchar r, g, b, a; } RGBA;
typedef struct { float x, y; } V2;
typedef struct { int id; int grp; int icon; int pad; } SlotT;
typedef struct { float frac; int sec; int min; } LapT;
typedef struct { char pad[0x20]; int (*renderStateSet)(int, int); } RwGlobalsT;

MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067ea6c, 0x0067ea6c);
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067ed68, 0x0067ed68);
MGLOBAL(uchar, DAT_0067e7cc, 0x0067e7cc);
MGLOBAL(int, DAT_0067ecdc, 0x0067ecdc);
MGLOBAL(int, DAT_0067ed6c, 0x0067ed6c);
MGLOBAL(int, DAT_0067ebcc, 0x0067ebcc);
MGLOBAL(int, DAT_0067ebc8, 0x0067ebc8);
MGLOBAL(int, DAT_0067ea08, 0x0067ea08);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_0067f0d4, 0x0067f0d4);
MGLOBAL(int, DAT_0067ed2c, 0x0067ed2c);
MGLOBAL(int, DAT_0067ecd4, 0x0067ecd4);
MGLOBAL(int, DAT_0067eccc, 0x0067eccc);
MGLOBAL(int, DAT_0067f0cc, 0x0067f0cc);
MGLOBAL(int, DAT_0067ecd8, 0x0067ecd8);
MGLOBAL(int, DAT_0067ecd0, 0x0067ecd0);
MGLOBAL(int, DAT_0067f0d0, 0x0067f0d0);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(float, DAT_0067ea04, 0x0067ea04);
MGLOBAL(float, DAT_0067ed74, 0x0067ed74);
MGLOBAL(int, DAT_005f76c4, 0x005f76c4);
MGLOBAL(int, DAT_0067ed4c, 0x0067ed4c);
MGLOBAL(int, DAT_005f76c8, 0x005f76c8);
MGLOBAL(int, DAT_007f0fcc, 0x007f0fcc);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
MGLOBALA(int, DAT_0067ecbc, 4, 0x0067ecbc);
MGLOBALA(int, DAT_0067ebd0, 16, 0x0067ebd0);
MGLOBALA(SlotT, DAT_007f1a14, 8, 0x007f1a14);
MGLOBALA(LapT, DAT_008989e0, 8, 0x008989e0);
MGLOBALA(char, s_d_005cd794, 3, 0x005cd794);
MGLOBALA(char, s_sep_005cd9c4, 4, 0x005cd9c4);
MGLOBALA(char, s_2nd_005cd9d0, 4, 0x005cd9d0);
MGLOBALA(char, s_minus_005cd99c, 3, 0x005cd99c);
MGLOBALA(char, s_plus_005cd9b0, 3, 0x005cd9b0);
MGLOBALA(char, s_slash_005cc4d8, 2, 0x005cc4d8);
MGLOBALA(char, s_Star_005cd970, 5, 0x005cd970);
MGLOBALA(char, s_time_005cd9b8, 11, 0x005cd9b8);
MGLOBALA(char, s_stime_005cd9a0, 13, 0x005cd9a0);

double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
int sprintf(char *, const char *, ...);
void FUN_0040b460(int *p);
void FUN_0040b540(int *p);
void FUN_0040b620(int *p);
int FUN_0040b6b0(int p);
int FUN_0040b6c0(int p);
int FUN_0040b6d0(int p);
int FUN_0040b7a0(void);
int FUN_0040b7b0(int p, int k);
void *FUN_0040bb50(const char *name);
int FUN_0040e340(void);
void FUN_0040e3a0(int p, RGBA *c);
short *FUN_00427780(int id);
void FUN_00427e00(int id, float x, float y, RGBA c, float scale, int align);
void FUN_00427f00(char *s, float x, float y, RGBA c, float scale, int align);
float FUN_004282a0(int id, float scale);
float FUN_00428320(char *s, float scale);
int FUN_00429870(void);
void FUN_00429a30(void);
float FUN_00429a70(int p);
int FUN_00429a80(int p);
int FUN_00429a90(int p);
void FUN_0042d290(int sec, float frac, char *a, char *b);
void FUN_0042d300(int t0, int t1, int *type, int *val, int *sec, float *frac);
void *FUN_0042fab0(int id);
void FUN_00472c60(V2 pos, V2 size, RGBA c);
void FUN_004736c0(V2 pos, V2 size, RGBA c, uchar a0, uchar a1, uchar a2, uchar a3);
void FUN_00473870(void *tex, V2 pos, V2 size, RGBA c, int n);
void FUN_004739f0(void *tex, V2 pos, V2 size, RGBA c, float u0, float v0, float u1, float v1,
                  int p10, int p11);
float FUN_00474e60(float deg);

// MATCH: 0x0042b8b0 FUN_0042b8b0
static short FUN_0042b8b0(void)
{
    return DAT_0067ea54;
}

// MATCH: 0x0042b8c0 FUN_0042b8c0
static short FUN_0042b8c0(void)
{
    return DAT_0067ea56;
}

// MATCH: 0x0042f8d0 FUN_0042f8d0
static void FUN_0042f8d0(uchar a, V2 pos, V2 size)
{
    RGBA c;
    V2 os;

    os = size;
    c.r = 0xf0;
    c.g = 0x6e;
    c.b = 0x14;
    c.a = a >> 1;
    FUN_00472c60(pos, size, c);
    c.r = 0xb4;
    c.g = 0x50;
    c.b = 0x10;
    c.a = a;
    pos.x -= 2.0f;
    size.x = 2.0f;
    FUN_00472c60(pos, size, c);
    size.x = os.x + 4.0f;
    size.y = 2.0f;
    FUN_00472c60(pos, size, c);
    pos.y += os.y - 2.0f;
    FUN_00472c60(pos, size, c);
    pos.x = os.x + pos.x + 2.0f;
    size.x = 2.0f;
    size.y = os.y;
    pos.y -= os.y - 2.0f;
    FUN_00472c60(pos, size, c);
}

// MATCH: 0x00430b30 FUN_00430b30
static void FUN_00430b30(int b, int a, float *frac, int *sec, int *min)
{
    *frac = DAT_008989e0[a + b * 2].frac;
    *min = DAT_008989e0[a + b * 2].min;
    *sec = DAT_008989e0[a + b * 2].sec;
}

// MATCH: 0x00434720 FUN_00434720
void FUN_00434720(void)
{
    RGBA cA;
    RGBA cW;
    RGBA cB;
    RGBA cC;
    RGBA cE;
    uchar a48;
    int show;
    int n;
    int v90;
    int v60;
    int va8;
    int t;
    int cnt;
    V2 P;
    V2 S;
    V2 P2;
    V2 S2;
    float fx;
    float ya;
    float yb;
    int i;
    int e;
    int grp;
    int lastId;
    V2 IS;
    float off;
    void *icon;
    int id;
    char num[4];
    char bufB8[32];
    char bufD8[32];
    char bufF8[68];
    char buf148[128];
    char buf1c8[56];
    float tx;
    float ty;
    float f94;
    int i7c;
    int i88;
    float f98;
    int i80;
    int i8c;
    float t0;
    int type;
    int val;
    int sec;
    float frac;
    int v;
    float tx2;
    float w1;
    float off50;
    int flag;
    int nstars;
    float rad;
    double dr;
    int j;
    V2 SP;
    float ang;
    int lim;
    int k;
    int col;
    float scr;
    float end;
    short *p;
    short *q;
    float clip;
    float acc;
    int m;
    short sn;
    float w71;
    float wB;

    cA.r = 0xff;
    cA.a = 0xff;
    cW.r = 0xff;
    cW.g = 0xff;
    cW.b = 0xff;
    cW.a = 0xff;
    cB.r = 0;
    cB.g = 0;
    cB.b = 0;
    cA.g = 0;
    cA.b = 0x30;
    if (DAT_007f0fd0 == 2)
        DAT_0067ea64 = 0;
    FUN_0040e3a0(0, &cA);
    show = 1;
    if ((DAT_0067e9fc == 3 || DAT_0067e9fc == 4 || DAT_0067e9fc == 5) &&
        (DAT_007f0fd0 == 10 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 8 || DAT_007f0fd0 == 7 ||
         DAT_007f0fd0 == 5))
        show = 0;
    switch (DAT_0067ea6c) {
    case 0:
    case 1:
    case 2:
        FUN_0040b540(DAT_0067ecbc);
        cA.a = (uchar)(int)DAT_0067f0d4;
        break;
    case 3:
    case 4:
        if (DAT_0067ecdc == 2)
            FUN_0040b620(DAT_0067ecbc);
        else
            FUN_0040b460(DAT_0067ecbc);
    case 5:
        if (DAT_0067ea6c == 5)
            cA.a = DAT_0067e7cc;
        else
            cA.a = (uchar)(int)DAT_0067f0d4;
        break;
    }
    DAT_007d3ff8->renderStateSet(6, 0);
    DAT_007d3ff8->renderStateSet(8, 0);
    n = FUN_0040e340();
    v90 = 3;
    v60 = 2;
    va8 = 1;
    if (n == 3) {
        v90 = 2;
        v60 = 1;
        va8 = 0;
    } else if (n == 2) {
        v90 = 1;
        v60 = 0;
    }
    if (DAT_0067ea64) {
        v90 = 1;
        v60 = 0;
    }
    t = DAT_0067ed68;
    if (t < 60)
        t = 60;
    P.x = (float)t;
    cnt = 600 - t;
    P.y = 100.0f;
    S.x = (float)cnt;
    S.y = 26.0f;
    cC.r = 0xf0;
    cC.g = 0x6e;
    cC.b = 0x14;
    cC.a = DAT_0067e7cc;
    a48 = DAT_0067e7cc;
    cB.a = DAT_0067e7cc;
    if (DAT_0067e9fc != 2)
        FUN_0042f8d0(a48, P, S);
    if (DAT_0067ea6c < 3) {
        if (DAT_0067ea64)
            FUN_00427e00(0x6b, P.x + 10.0f, 113.0f, cB, 0.7f, 0);
        else
            FUN_00427e00(0xa1, P.x + 10.0f, 113.0f, cB, 0.7f, 0);
    } else if (DAT_0067e9fc == 6 || DAT_0067e9fc == 2) {
        if (DAT_0067ea64)
            FUN_00427e00(0x6b, P.x + 10.0f, 113.0f, cB, 0.7f, 0);
        else if (DAT_0067e9fc != 2)
            FUN_00427e00(0xa1, P.x + 10.0f, 113.0f, cB, 0.7f, 0);
        if (DAT_0067e9fc != 2) {
            fx = P.x + 155.0f;
            FUN_00427e00(0x3d, fx, 107.0f, cB, 0.5f, 2);
            FUN_00427e00(0x72, fx, 119.0f, cB, 0.5f, 2);
            FUN_00427e00(0x6d, P.x + 225.0f, 113.0f, cB, 0.7f, 2);
            FUN_00427e00(0x6e, P.x + 295.0f, 113.0f, cB, 0.7f, 2);
            if (!DAT_0067ea64 && n > 2)
                FUN_00427e00(0x6f, P.x + 365.0f, 113.0f, cB, 0.7f, 2);
            if (!DAT_0067ea64 && n > 3)
                FUN_00427e00(0x70, P.x + 435.0f, 113.0f, cB, 0.7f, 2);
        } else {
            S2.y = 26.0f;
            P2.y = 100.0f;
            S2.x = S.x;
            P2.x = P.x;
            FUN_0042f8d0(a48, P2, S2);
            FUN_00427e00(0x14c, P.x + 10.0f, 113.0f, cB, 0.5f, 0);
            P2.y = 132.0f;
            FUN_0042f8d0(a48, P2, S2);
            P2.y = 164.0f;
            FUN_0042f8d0(a48, P2, S2);
            P2.y = 196.0f;
            FUN_0042f8d0(a48, P2, S2);
            P2.y = 100.0f;
        }
    }
    if (DAT_0067ecdc == 0 || DAT_0067ea6c < 3) {
        if (DAT_0067e9fc == 2) {
            ya = P2.y + 7.0f;
            FUN_00427e00(0xbe, 270.0f, ya, cB, 0.5f, 2);
            yb = P2.y + 19.0f;
            FUN_00427e00(0x14d, 270.0f, yb, cB, 0.5f, 2);
            FUN_00427e00(0xa1, 400.0f, ya, cB, 0.5f, 2);
            FUN_00427e00(0x14d, 400.0f, yb, cB, 0.5f, 2);
            FUN_00427e00(0x14e, 590.0f, P2.y + 12.0f, cB, 0.5f, 1);
        } else if (DAT_0067e9fc == 6) {
            FUN_00427e00(0x6c, 590.0f, 113.0f, cB, 0.7f, 1);
        } else {
            FUN_00427e00(0x24e, 590.0f, 113.0f, cB, 0.7f, 1);
        }
    } else {
        if (DAT_0067ecdc == 1) {
            FUN_00427e00(0xa6, 595.0f, 107.0f, cB, 0.5f, 1);
            FUN_00427e00(0xa8, 595.0f, 119.0f, cB, 0.5f, 1);
        }
        if (DAT_0067ecdc == 2) {
            FUN_00427e00(0xa7, 595.0f, 107.0f, cB, 0.5f, 1);
            FUN_00427e00(0xa8, 595.0f, 119.0f, cB, 0.5f, 1);
        }
    }
    grp = 1;
    cnt = 4;
    lastId = -1;
    P.y = 148.0f;
    IS.y = 32.0f;
    IS.x = 32.0f;
    if (DAT_007f0fd0 == 8)
        cnt = 1;
    for (i = 0; i < cnt; i++) {
        if (i != 0 && DAT_0067ed6c != 0)
            continue;
        e = DAT_0067ecbc[i];
        if (DAT_007f1a14[e].id <= -1)
            continue;
        if (DAT_0067ea64) {
            if (DAT_007f1a14[e].grp == lastId) {
                P.y -= 50.0f;
                icon = FUN_0042fab0(DAT_007f1a14[e].icon);
                off = (float)(grp * 36 + 8);
                P.x = off + P.x;
                FUN_004739f0(icon, P, IS, cW, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
                P.x = P.x - off;
                grp++;
                P.y += 50.0f;
                continue;
            }
            lastId = DAT_007f1a14[e].grp;
            grp = 1;
        }
        S.y = 26.0f;
        cC.r = 0x2e;
        cC.g = 0x31;
        cC.b = 0x92;
        cC.a = DAT_0067e7cc;
        if (DAT_0067e9fc != 2)
            FUN_0042f8d0(a48, P, S);
        P.y += 2.0f;
        S.y = 4.0f;
        /* original: (!DAT_0067e9fc) == 2, never true (sete cl; cmp ecx, 2 at 0x00434f1f) */
        if (!DAT_0067ea64 && !DAT_0067e9fc == 2)
            FUN_0042f8d0(a48, P, S);
        icon = FUN_0042fab0(DAT_007f1a14[DAT_0067ecbc[i]].icon);
        if (DAT_007f0fd0 == 8)
            icon = FUN_0042fab0(DAT_007f1a14[0].icon);
        P.x += 8.0f;
        P.y -= 4.0f;
        if (DAT_0067e9fc != 2 && DAT_0067ecbc[i] != -1)
            FUN_004739f0(icon, P, IS, cW, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
        P.x -= 8.0f;
        P.y += 2.0f;
        switch (DAT_0067ea6c) {
        case 0:
        case 1:
            if (i == 0) {
                id = 0x6d;
                sprintf(num, s_d_005cd794, v90);
            } else if (i == 1) {
                id = 0x6e;
                sprintf(num, s_d_005cd794, v60);
            } else if (i == 2) {
                if (FUN_0040b6d0(DAT_0067ecbc[1]) == FUN_0040b6d0(DAT_0067ecbc[2])) {
                    id = 0x6e;
                    sprintf(num, s_d_005cd794, v60);
                } else {
                    id = 0x6f;
                    sprintf(num, s_d_005cd794, va8);
                }
            } else if (i == 3) {
                if (FUN_0040b6d0(DAT_0067ecbc[3]) == FUN_0040b6d0(DAT_0067ecbc[1])) {
                    id = 0x6e;
                    sprintf(num, s_d_005cd794, v60);
                } else if (FUN_0040b6d0(DAT_0067ecbc[3]) == FUN_0040b6d0(DAT_0067ecbc[2])) {
                    id = 0x6f;
                    sprintf(num, s_d_005cd794, va8);
                } else {
                    id = 0x70;
                    sprintf(num, s_d_005cd794, 0);
                }
            }
            if (DAT_007f0fd0 == 4 || DAT_007f0fd0 == 7)
                id = i + 0x6d;
            if (DAT_0067ea64 && i) {
                sprintf(bufF8, s_2nd_005cd9d0);
                id = 0x6e;
                sprintf(num, s_d_005cd794, v60);
            }
            if (DAT_0067e9fc == 6) {
                ty = P.y + 15.0f;
                FUN_00427e00(id, P.x + 140.0f, ty, cB, 0.6f, 1);
                FUN_00427f00(num, P.x + 220.0f, ty, cB, 0.6f, 2);
            } else {
                if (DAT_007f0fd0 == 10 || DAT_007f0fd0 == 8 || DAT_007f0fd0 == 5)
                    id = DAT_007f0fcc ? 0x6d : 0x221;
                FUN_00427e00(id, P.x + 220.0f, P.y + 15.0f, cB, 0.6f, 1);
            }
            break;
        case 3:
        case 4:
        case 5:
            if (DAT_0067e9fc == 2) {
                DAT_0067ed2c = 0;
                if (FUN_00429870() && !DAT_0067ed6c) {
                    FUN_00429a30();
                    DAT_0067ed2c = 1;
                }
                DAT_0067ecd4 = FUN_00429a80(0);
                DAT_0067eccc = FUN_00429a90(0);
                DAT_0067f0cc = (int)(FUN_00429a70(0) * 100.0f);
                DAT_0067ecd8 = FUN_00429a80(1);
                DAT_0067ecd0 = FUN_00429a90(1);
                DAT_0067f0d0 = (int)(FUN_00429a70(1) * 100.0f);
            }
            if (DAT_0067e9fc == 2) {
                sprintf(bufF8, s_d_005cd794, 1);
                tx = P2.x + 10.0f;
                FUN_00427f00(bufF8, tx, P2.y + 45.0f, cB, 0.7f, 0);
                FUN_00430b30(0, 1, &f94, &i7c, &i88);
                FUN_0042d290(i7c, f94 * 100.0f, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, i88, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                ty = P2.y + 47.0f;
                FUN_00427f00(bufF8, 270.0f, ty, cB, 0.6f, 2);
                FUN_00430b30(0, 0, &f98, &i80, &i8c);
                t0 = f98 * 100.0f;
                FUN_0042d290(i80, t0, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, i8c, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 400.0f, ty, cB, 0.6f, 2);
                FUN_0042d300((int)((i88 * 60 + i7c) * 100 + f94 * 100.0),
                             (int)((float)((i8c * 60 + i80) * 100) + t0), &type, &val, &sec, &frac);
                FUN_0042d290(sec, frac, bufB8, bufD8);
                if (type == 2)
                    sprintf(bufF8, s_time_005cd9b8, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                else if (type == 1)
                    sprintf(bufF8, s_stime_005cd9a0, s_plus_005cd9b0, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4,
                            bufD8);
                else
                    sprintf(bufF8, s_stime_005cd9a0, s_minus_005cd99c, val, s_sep_005cd9c4, bufB8,
                            s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 590.0f, ty, cB, 0.6f, 1);

                sprintf(bufF8, s_d_005cd794, 2);
                FUN_00427f00(bufF8, tx, P2.y + 77.0f, cB, 0.7f, 0);
                FUN_00430b30(1, 1, &f94, &i7c, &i88);
                FUN_0042d290(i7c, f94 * 100.0f, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, i88, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                ty = P2.y + 79.0f;
                FUN_00427f00(bufF8, 270.0f, ty, cB, 0.6f, 2);
                FUN_00430b30(1, 0, &f98, &i80, &i8c);
                t0 = f98 * 100.0f;
                FUN_0042d290(i80, t0, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, i8c, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 400.0f, ty, cB, 0.6f, 2);
                FUN_0042d300((int)((i88 * 60 + i7c) * 100 + f94 * 100.0),
                             (int)((float)((i8c * 60 + i80) * 100) + t0), &type, &val, &sec, &frac);
                FUN_0042d290(sec, frac, bufB8, bufD8);
                if (type == 2)
                    sprintf(bufF8, s_time_005cd9b8, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                else if (type == 1)
                    sprintf(bufF8, s_stime_005cd9a0, s_plus_005cd9b0, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4,
                            bufD8);
                else
                    sprintf(bufF8, s_stime_005cd9a0, s_minus_005cd99c, val, s_sep_005cd9c4, bufB8,
                            s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 590.0f, ty, cB, 0.6f, 1);

                FUN_00427e00(0x14f, tx, P2.y + 109.0f, cB, 0.7f, 0);
                FUN_0042d290(DAT_0067ecd0, (float)DAT_0067f0d0, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, DAT_0067ecd8, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                ty = P2.y + 111.0f;
                FUN_00427f00(bufF8, 270.0f, ty, cB, 0.6f, 2);
                FUN_0042d290(DAT_0067eccc, (float)DAT_0067f0cc, bufB8, bufD8);
                sprintf(bufF8, s_time_005cd9b8, DAT_0067ecd4, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 400.0f, ty, cB, 0.6f, 2);
                FUN_0042d300((DAT_0067ecd8 * 60 + DAT_0067ecd0) * 100 + DAT_0067f0d0,
                             (DAT_0067ecd4 * 60 + DAT_0067eccc) * 100 + DAT_0067f0cc, &type, &val, &sec, &frac);
                FUN_0042d290(sec, frac, bufB8, bufD8);
                if (type == 2)
                    sprintf(bufF8, s_time_005cd9b8, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4, bufD8);
                else if (type == 1)
                    sprintf(bufF8, s_stime_005cd9a0, s_plus_005cd9b0, val, s_sep_005cd9c4, bufB8, s_sep_005cd9c4,
                            bufD8);
                else
                    sprintf(bufF8, s_stime_005cd9a0, s_minus_005cd99c, val, s_sep_005cd9c4, bufB8,
                            s_sep_005cd9c4, bufD8);
                FUN_00427f00(bufF8, 590.0f, ty, cB, 0.6f, 1);
                if (DAT_0067ed2c == 0)
                    break;
                off50 = FUN_00428320(bufF8, 0.6f) * 0.5f + 8.0f;
                flag = 1;
                goto stars;
            }
            if (DAT_0067e9fc == 6 || DAT_0067e9fc == 2) {
                if (DAT_0067ecdc == 2)
                    v = FUN_0040b6b0(DAT_0067ecbc[i]);
                else
                    v = FUN_0040b6c0(DAT_0067ecbc[i]);
                sprintf(bufF8, s_d_005cd794, v);
                if (DAT_0067ecdc != 0) {
                    if (DAT_0067ecdc == 1)
                        v = DAT_005f76c4;
                    else if (DAT_0067ecdc == 2)
                        v = DAT_005f76c8;
                    sprintf(buf1c8, s_d_005cd794, v);
                    tx2 = (S.x + P.x) - 5.0f;
                    FUN_00427f00(buf1c8, tx2, P.y + 19.0f, cB, 0.5f, 1);
                    w1 = FUN_00428320(buf1c8, 0.5f);
                    off50 = w1;
                    FUN_00427f00(s_slash_005cc4d8, tx2 - w1, P.y + 13.0f, cB, 0.5f, 1);
                    w1 = FUN_00428320(s_slash_005cc4d8, 0.5f) + w1;
                    off50 = FUN_00428320(s_slash_005cc4d8, 0.5f) * 0.5f + off50;
                    FUN_00427f00(bufF8, (tx2 - w1) - 2.0f, P.y + 12.0f, cB, 0.5f, 1);
                    flag = 0;
                    if (DAT_0067ed4c == DAT_0067ecbc[i] + 1) {
stars:
                        DAT_0067ea04 = DAT_007f1004 + DAT_0067ea04;
                        if (DAT_0067ea04 > 0.075f) {
                            if (DAT_0067ed74 >= 359.0f) {
                                DAT_0067ed74 = 359.0f;
                                if (DAT_0067ea04 > 0.75f) {
                                    DAT_0067ed74 = 0.0f;
                                    DAT_0067ea04 = 0.0f;
                                }
                            } else {
                                DAT_0067ed74 = DAT_0067ed74 + 45.0f;
                                DAT_0067ea04 = 0.0f;
                            }
                        }
                        nstars = (int)(DAT_0067ed74 * 0.022222223f);
                        cC.r = 0xf7;
                        cC.g = 0x94;
                        cC.b = 0x1d;
                        rad = 38.0f;
                        if (!flag)
                            rad = 26.0f;
                        j = 0;
                        if (nstars >= 0) {
                            dr = rad;
                            do {
                                if (DAT_0067e9fc == 2) {
                                    SP.x = 392.0f;
                                    SP.y = ((P2.y + 13.0f) - 7.0f) + 96.0f;
                                } else {
                                    SP.x = (((S.x + P.x) - 5.0f) - off50) - 7.0f;
                                    SP.y = (P.y + 13.0f) - 7.0f;
                                }
                                ang = (float)j * 45.0f;
                                SP.x = (float)(sin(FUN_00474e60(ang)) * dr * 1.3333333333333333 + SP.x);
                                dr = rad;
                                SP.y = (float)(cos(FUN_00474e60(ang)) * dr + SP.y);
                                SP.x = (float)FUN_0042b8b0() * SP.x * 0.0015625f;
                                SP.y = (float)FUN_0042b8c0() * SP.y * 0.0020833334f;
                                S2.x = (float)FUN_0042b8b0() * 0.025f;
                                S2.y = (float)FUN_0042b8c0() * 0.033333335f;
                                cC.r = 0;
                                cC.g = 0;
                                cC.b = 0;
                                FUN_00473870(FUN_0040bb50(s_Star_005cd970), SP, S2, cC, 1);
                                SP.x -= 2.0f;
                                cC.r = 0xf7;
                                cC.g = 0x94;
                                cC.b = 0x1d;
                                SP.y -= 2.0f;
                                FUN_00473870(FUN_0040bb50(s_Star_005cd970), SP, S2, cC, 1);
                                j++;
                            } while (j <= nstars);
                        }
                        if (flag)
                            break;
                    }
                } else {
                    FUN_00427f00(bufF8, (S.x + P.x) - 14.0f, P.y + 15.0f, cB, 0.6f, 1);
                }
                sprintf(bufF8, s_d_005cd794, FUN_0040b7a0());
                ty = P.y + 15.0f;
                FUN_00427f00(bufF8, P.x + 155.0f, ty, cB, 0.6f, 2);
                if (DAT_0067ea64)
                    lim = 2;
                else
                    lim = n;
                for (k = 0, col = 0; col < 280; col += 70, k++) {
                    if (k < lim) {
                        sprintf(bufF8, s_d_005cd794, FUN_0040b7b0(DAT_0067ecbc[i], k));
                        FUN_00427f00(bufF8, (float)col + P.x + 225.0f, ty, cB, 0.6f, 2);
                    }
                }
            }
            break;
        }
        P.y += 48.0f;
    }
    if (show) {
        S.y = 60.0f;
        S.x = 520.0f;
        cC.a = DAT_0067e7cc;
        P.y = 340.0f;
        P.x = 60.0f;
        cC.r = 0xf0;
        cC.g = 0x6e;
        cC.b = 0x14;
        FUN_0042f8d0(a48, P, S);
        scr = ((float)DAT_0067ebcc - 640.0f) - 1000.0f;
        if (scr < 100.0f) {
            if (scr > -1500.0f)
                scr = 100.0f;
            else
                scr += 1600.0f;
        }
        for (k = 0; k < DAT_0067ebc8; k++) {
            id = DAT_0067ebd0[k];
            end = FUN_004282a0(id, 0.6f) + scr;
            if (end < 60.0f) {
                scr = FUN_004282a0(id, 0.6f) + scr + 8.0f;
            } else {
                if (scr < 60.0f) {
                    p = FUN_00427780(id);
                    clip = 60.0f - scr;
                    buf148[0] = (char)p[1];
                    buf148[1] = 0;
                    acc = 0.0f;
                    j = 1;
                    if (DAT_005d757c < clip) {
                        q = p + 2;
                        do {
                            acc = FUN_00428320(buf148, 0.6f) + acc;
                            buf148[0] = (char)*q;
                            q++;
                            j++;
                        } while (acc < clip);
                    }
                    for (m = 0; j < *p + 1; m++, j++)
                        buf148[m] = (char)p[j];
                    buf148[m] = 0;
                    FUN_00427f00(buf148, (scr - 60.0f) + acc + 60.0f, 376.0f, cB, 0.6f, 0);
                } else if (scr <= 580.0f) {
                    if (end > 580.0f) {
                        p = FUN_00427780(id);
                        sn = *p;
                        buf148[1] = 0;
                        if (end > 580.0f) {
                            do {
                                if (sn == 0)
                                    break;
                                buf148[0] = (char)p[sn];
                                sn--;
                                end = end - FUN_00428320(buf148, 0.6f);
                            } while (end > 580.0f);
                        }
                        for (m = 0; m < sn; m++)
                            buf148[m] = (char)p[m + 1];
                        buf148[m] = 0;
                        FUN_00427f00(buf148, scr, 376.0f, cB, 0.6f, 0);
                    } else {
                        FUN_00427e00(id, scr, 376.0f, cB, 0.6f, 0);
                    }
                } else {
                    continue;
                }
                scr = FUN_004282a0(id, 0.6f) + scr + 8.0f;
            }
        }
        S.x = 10.0f;
        P.x = 60.0f;
        FUN_00472c60(P, S, cC);
        S.x = 30.0f;
        P.x = 70.0f;
        FUN_004736c0(P, S, cC, cC.a, 0, cC.a, 0);
        P.x = 540.0f;
        FUN_004736c0(P, S, cC, 0, cC.a, 0, cC.a);
        S.x = 10.0f;
        P.x = 570.0f;
        FUN_00472c60(P, S, cC);
        S.x = 20.0f;
        P.x = 40.0f;
        FUN_004736c0(P, S, cC, 0, cC.a, 0, cC.a);
        P.x = 580.0f;
        FUN_004736c0(P, S, cC, cC.a, 0, cC.a, 0);
        S.y = 2.0f;
        S.x = 640.0f;
        P.y = 344.0f;
        P.x = 0.0f;
        FUN_00472c60(P, S, cB);
        P.x = 80.0f;
        P.y = 344.0f;
        w71 = FUN_004282a0(0x71, 0.6f);
        wB = FUN_004282a0(DAT_0067ea08 + 0xaa, 0.6f);
        if (DAT_0067e9fc == 2) {
            w71 = 0.0f;
            wB = FUN_004282a0(0xc1, 0.6f);
        }
        S.y = 16.0f;
        S.x = wB + w71 + 4.0f;
        FUN_0042f8d0(a48, P, S);
        if (DAT_0067e9fc == 2) {
            FUN_00427e00(0xc1, 82.0f, 351.0f, cB, 0.6f, 0);
        } else {
            FUN_00427e00(0x71, 82.0f, 351.0f, cB, 0.6f, 0);
            FUN_00427e00(DAT_0067ea08 + 0xaa, w71 + 82.0f, 351.0f, cB, 0.6f, 0);
        }
    }
    DAT_007d3ff8->renderStateSet(8, 1);
    DAT_007d3ff8->renderStateSet(6, 1);
    if (DAT_0067ea6c <= 3) {
        t = DAT_0067ed68 - 0x127;
        S.x = 256.0f;
        S.y = 220.0f;
        cE.g = 0;
        cE.b = 0;
        cE.r = 0;
        a48 = DAT_0067e7cc;
        if (a48 > 0xa0)
            a48 = 0xa0;
        P.x = (float)t + 8.0f;
        P.y = 108.0f;
        FUN_004736c0(P, S, cE, a48, a48, a48, a48);
        P.x = P.x - 8.0f;
        P.y = 100.0f;
        FUN_00472c60(P, S, cC);
    }
}
