// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x0040c690 (first hand source; raw g2s 7.7%): 80.5%, 638 B differ, NOT CLOSED.
   Literals read from the anchored image (3.0/0.1/0.5/1.4/0.8/0.3/0.05/0.7/0.4). Moved it: the threshold as
   a 3-way switch with 0xff968080 falling into default when FUN_00426c00() != 0x1e (cmp chain at
   0x0040c6e6); the colour switch with 0xffff32ff as its own case (it is the BST pivot at 0x0040c849, `je`
   to the default body; merging it into `default:` drops the compare); the post-switch `if (type == 6)`
   (cl threads the constant-type cases past it, only the two rand cases keep `cmp esi,6` at 0x0040c8c6);
   v.x stored before vy in the copy cases; `x > 0.8f` / `x > 0.3f` for the rand tests (je at 0x0040c8b7);
   `v.x - (0, m->x)` for the field-first fsubr at 0x0040c911. Residual: `mov edi, big` is loaded once in
   each threshold successor where the original loads it before the jne in both predecessors
   (0x0040c6f4, 0x0040c716): one extra 4 B load, so the symbol is 878 B against 874 B and every later
   byte is shifted. Tried: ternaries, goto forms, if-chain, all 6 case orders, an __inline helper. */
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
typedef struct { char pad[0x30]; float x, y, z; } Mtx;
typedef struct { int v[3]; } Rec7f0a04;
MGLOBALA(Rec7f0a04, DAT_007f0a04, 4, 0x007f0a04);
MGLOBAL(int, DAT_00636adc, 0x00636adc);
float FUN_00472650(float lo, float hi);
float FUN_0041f100(int i);
int   FUN_00426c00(void);
int   FUN_0046d6a0(float *p0, int p1);
void  FUN_004c3d90(V3 *out, V3 *in, int n, Mtx *m);
void  FUN_0048cf90(V3 *vel, int pos, int type, float scale);

// MATCH: 0x0040c690 _FUN_0040c690
void FUN_0040c690(Mtx *m, int pos, float *vel, unsigned int col, int big, int car)
{
    float r;
    float vy;
    float scale;
    V3 v;
    float th;
    int type;

    vy = 0.0f;
    r = FUN_00472650(0.0f, 1.0f);
    scale = 1.0f;
    if (DAT_00636adc != 0)
        return;
    if (car != -1)
        scale = FUN_0041f100(car) * 3.0f;
    switch (col) {
    case 0xffc88080:
        if (big)
            th = 0.1f;
        else
            th = 0.05f;
        break;
    case 0xff968080:
        if (FUN_00426c00() == 0x1e) {
            if (big)
                th = 0.3f;
            else
                th = 0.1f;
            break;
        }
    default:
        if (big)
            th = 0.7f;
        else
            th = 0.4f;
        break;
    }
    if (r > th)
        return;
    v.x = 0.0f;
    v.y = 0.0f;
    r = r - th * 0.5f;
    v.z = 0.0f;
    switch (col) {
    case 0xffb48080:
        vy = 0.003f;
        v.x = r * 1.4f;
        type = 9;
        break;
    case 0xff80c800:
    case 0xff0064b4:
    case 0xff0080c8:
        vy = 0.006f;
        v.x = r * 1.4f;
        type = 0xe;
        break;
    case 0xff646464:
        type = 1;
        v.x = r;
        vy = 0.03f;
        break;
    case 0xff968080:
        v.x = r;
        vy = 0.03f;
        type = 6;
        break;
    case 0xffa08080:
        type = 7;
        v.x = r * 1.4f;
        break;
    case 0xffc88080:
        vy = 0.01f;
        v.x = r * 1.4f;
        type = 10;
        break;
    case 0xffc8c8c8:
        if (FUN_00472650(0.0f, 1.0f) > 0.8f)
            type = 0x10;
        else
            type = 0xf;
        v.x = r;
        vy = 0.03f;
        break;
    case 0xffdc8080:
        vy = 0.002f;
        v.x = r * 1.4f;
        type = 8;
        break;
    case 0xffff8040:
        type = 0xd;
        v.x = r;
        vy = 0.03f;
        break;
    case 0xffffffff:
        if (FUN_00472650(0.0f, 1.0f) > 0.3f)
            type = 0x10;
        else
            type = 0xf;
        v.x = r;
        vy = 0.03f;
        break;
    case 0xffff32ff:
        type = 0xb;
        break;
    default:
        type = 0xb;
        break;
    }
    if (type == 6) {
        FUN_0046d6a0(&r, car);
        if (big == 0 && DAT_007f0a04[car].v[0] == 0)
            return;
    }
    if (m != 0) {
        FUN_004c3d90(&v, &v, 1, m);
        v.x = v.x - (0, m->x);
        v.z = v.z - (0, m->z);
    }
    v.y = vy;
    if (vel != 0) {
        v.x = vel[0];
        v.y = vel[1];
        v.z = vel[2];
    }
    FUN_0048cf90(&v, pos, type, scale);
}
