// FLAGS: /O2 /GS /Ob1
/* 0x004778e0 (56 B): 100% (regs 100%). ESI-passed slot index (pilot finding 1): static callee, real caller 0x00477920
   in this file. Clears the 0x2c0-byte Trail record (FUN_004b6520) and its bit in the byte bitmap at 0x006bf198.
   This file holds 0x004778e0 and 0x00477920 (93.3%, see 00477920.c). */
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


typedef struct Vtx {
    float x, y, z;
    float nx, ny, nz;
    unsigned int color;
    float u, v;
} Vtx;
typedef struct Trail {
    Vtx v[19];
    float f2ac;
    int count;
    int i2b4;
    float age;
    float life;
} Trail;
MGLOBALA(Trail, DAT_00693198, 256, 0x00693198);
MGLOBALA(unsigned char, DAT_006bf198, 32, 0x006bf198);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
void FUN_004b6520(void *p0, int p1);

// MATCH: 0x004778e0 FUN_004778e0
static void FUN_004778e0(int i)
{
    FUN_004b6520(&DAT_00693198[i], 0x2c0);
    DAT_006bf198[i >> 3] &= ~(unsigned char)(1 << (i & 7));
}

// MATCH: 0x00477920 FUN_00477920
void FUN_00477920(void)
{
    int i;
    int n;
    int j;
    unsigned int c;
    float a;
    Trail *t;
    for (i = 0; i < 256; i++) {
        t = &DAT_00693198[i];
        a = t->age = DAT_007f100c + t->age;
        if (DAT_006bf198[i >> 3] & (1 << (i & 7))) {
            if (t->count == 0 && a > 1.0f) {
                FUN_004778e0(i);
            } else {
                a = a / t->life;
                if (a > 1.0f) {
                    FUN_004778e0(i);
                } else if (t->count != 0 && t->f2ac != 0.0f) {
                    for (j = 0; j < t->count; j++) {
                        c = ((unsigned char)(int)((1.0f - a) * 48.0f) << 24) | 0xffffff;
                        t->v[j * 2].color = c;
                        t->v[j * 2 + 1].color = c;
                    }
                }
            }
        }
    }
}
