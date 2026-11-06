// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 h8 0x00477a10: 100% by whole-symbol bytes (290 B BYTE-IDENTICAL, relocations masked). The per-
   instruction score reads 98.0% (98/96) only because Ghidra's 287 B size stops before the epilogue's add esp,8 / ret.
   Raw converter 58.2%. Saves render states 10/11 through RwGlobals (DAT_007d3ff8, +0x24 get / +0x20 set, cdecl),
   sets 10=1, 11=6, 1=*DAT_00693190, walks the 256 0x2c0-byte records at 0x00693198 whose bit is set in the bitmap
   at 0x006bf198, frees a record (FUN_004778e0) when FUN_004c1b40(curCamera, &rec+0x2a0) is 0 and rec+0x2b4 != 0,
   else draws it (FUN_004cd070(rec, count*2, 0, 1), FUN_004cd2d0(4), FUN_004cd140) when rec+0x2ac != DAT_005d757c
   and count != 1, then restores 10/11.
   FUN_004778e0 takes the record index in ESI (pilot finding 1): this file holds it as a static callee with its own
   MATCH line (100%, same body as re/match/s12/004778e0.c). The record struct here names +0x2a0..+0x2ac as a sphere
   (the FUN_004c1b40 argument) where 004778e0.c spells the same bytes as the tail of Vtx v[19]: same layout. */
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
typedef struct Trail {
    undefined4 v[0xa8];
    float sphere[3];
    float radius;
    int count;
    int i2b4;
    float age;
    float life;
} Trail;
typedef struct RwG {
    void *curCamera;
    void *curWorld;
    undefined4 pad08[6];
    int (*setRS)(int, unsigned int);
    int (*getRS)(int, void *);
} RwG;
MGLOBAL(RwG *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(unsigned int *, DAT_00693190, 0x00693190);
MGLOBALA(Trail, DAT_00693198, 256, 0x00693198);
MGLOBALA(unsigned char, DAT_006bf198, 32, 0x006bf198);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_004b6520(void *p0, int p1);
int FUN_004c1b40(void *p0, float *p1);
int FUN_004cd070(void *p0, int p1, void *p2, unsigned int p3);
int FUN_004cd140(void);
int FUN_004cd2d0(int p0);

// MATCH: 0x004778e0 FUN_004778e0
static void FUN_004778e0(int i)
{
    FUN_004b6520(&DAT_00693198[i], 0x2c0);
    DAT_006bf198[i >> 3] &= ~(unsigned char)(1 << (i & 7));
}

// MATCH: 0x00477a10 FUN_00477a10
void FUN_00477a10(void)
{
    int i;
    Trail *t;
    unsigned int src;
    unsigned int dst;

    DAT_007d3ff8->getRS(10, &src);
    DAT_007d3ff8->getRS(0xb, &dst);
    DAT_007d3ff8->setRS(10, 1);
    DAT_007d3ff8->setRS(0xb, 6);
    DAT_007d3ff8->setRS(1, *DAT_00693190);
    for (i = 0; i < 256; i++) {
        t = &DAT_00693198[i];
        if ((DAT_006bf198[i >> 3] & (1 << (i & 7))) && t->count != 0) {
            if (FUN_004c1b40(DAT_007d3ff8->curCamera, t->sphere) == 0 && t->i2b4 != 0) {
                FUN_004778e0(i);
            } else if (t->radius != DAT_005d757c && t->count != 1) {
                if (FUN_004cd070(t, t->count * 2, 0, 1) != 0) {
                    FUN_004cd2d0(4);
                    FUN_004cd140();
                }
            }
        }
    }
    DAT_007d3ff8->setRS(10, src);
    DAT_007d3ff8->setRS(0xb, dst);
}
