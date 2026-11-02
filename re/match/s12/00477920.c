// FLAGS: /O2 /GS /Ob1
/* 0x00477920 (228 B): 93.3% (regs 93.3%, ops 96.0%); whole symbol 231 B, 9 differing bytes. Trail = 19 RwIm3DVertex
   (0x24 B) + float +0x2ac + count +0x2b0 + age +0x2b8 + life +0x2bc, 256 records at 0x00693198. What moved it: reading
   t->count twice (not a cached n) makes the induction pointer sit on +0x2b0 like the original; computing the colour
   inside the inner loop gives the original's j=0 / count>0 test before the hoisted _ftol (1.0f at 0x005cc320, 48.0f
   at 0x005cd784). Residue: the original stores the new age to the local first (fst [esp+0xc]; fstp [edi+8]), ours
   the other way. Tried a = t->age = x, t->age = a = x, +=, separate statements, local-first forms, permuter
   (140 compiles, no gain). Class (b). Tail (pop ebx/pop ecx/ret) lies past the Ghidra size.
   This file holds 0x00477920 and its static ESI-arg callee 0x004778e0 (100%). */
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
