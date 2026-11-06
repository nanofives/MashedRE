// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c350: 100% (regs 100%), 77/77 ins, 303 B bytes identical. Raw converter 23.4%.
   What moved it: a switch on 0x00426c00() (case 2 / case 0x21) instead of if/else-if (66.2%), the packed
   RGBA compare, and the local identity matrix with the three 1.0f stores first (96.1% with them last).
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
typedef struct V3_0045c350 { float x, y, z; } V3_0045c350;
typedef struct Mat0045c350 {
    float rx, ry, rz;
    unsigned int flags;
    float ux, uy, uz;
    int pad1c;
    float ax, ay, az;
    int pad2c;
    float px, py, pz;
    int pad3c;
} Mat0045c350;
typedef struct Col0045c350 {
    int f00;
    unsigned char r, g, b, a;
} Col0045c350;
int FUN_0045c110(Col0045c350 *c);
int FUN_00426c00(void);
int FUN_0044d5e0(V3_0045c350 *pos);
void FUN_004c51a0(Mat0045c350 *m, V3_0045c350 *pos, int mode);
int FUN_00452140(Mat0045c350 *a, Mat0045c350 *b);

// MATCH: 0x0045c350 FUN_0045c350
int FUN_0045c350(int hit, V3_0045c350 *pos)
{
    Mat0045c350 m;
    Col0045c350 *c;
    unsigned int rgba;
    c = *(Col0045c350 **)(*(int *)(*(int *)(hit + 0x3c) + 0x10) +
        ((unsigned int)*(unsigned short *)(*(int *)(*(int *)(hit + 0x34) + 4) + 6 + *(int *)(hit + 0xc) * 8) +
         (unsigned int)*(unsigned short *)(*(int *)(hit + 0x34) + 0x80)) * 4);
    rgba = ((unsigned int)c->a << 24) | ((unsigned int)c->b << 16) | ((unsigned int)c->g << 8) | c->r;
    if (FUN_0045c110(c) != 0)
        return 1;
    switch (FUN_00426c00()) {
    case 2:
        if (FUN_0044d5e0(pos) != 0)
            return 1;
        break;
    case 0x21:
        if (rgba != 0xff804040)
            break;
        m.az = 1.0f; m.uy = 1.0f; m.rx = 1.0f;
        m.ux = 0.0f; m.rz = 0.0f; m.ry = 0.0f;
        m.ay = 0.0f; m.ax = 0.0f; m.uz = 0.0f;
        m.pz = 0.0f; m.py = 0.0f; m.px = 0.0f;
        m.flags |= 0x20003;
        FUN_004c51a0(&m, pos, 0);
        if (FUN_00452140(&m, &m) == 0)
            return 1;
        break;
    }
    return 0;
}
