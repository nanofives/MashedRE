// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00427ca0 (helper h6): 100% (regs 100%), relocheck OK. By hand.
   Holds a static copy of 0x00427880 (its own // MATCH line, 100%): the callee takes the file name in EAX, so it
   must be static and defined before the caller (pilot finding 1).
   What moved the score: the two names are char[11] filled by intrinsic strcpy in the order rwf, txd, declared
   rwf then txd (initialized arrays or the other order swap their frame slots: 77.8-84.4%); black.alpha = 255
   written first; the four globals at 0x0067d828..0x0067d834 cleared by one intrinsic memset of 16 bytes (gives
   the late xor edx, edx; four int stores CSE the zero into ebx for the whole function, four float stores use
   immediates). The 0x0067d828 block is therefore declared as float[4] here; 0x0067d830/0x0067d834 are the
   floats 0x00427680 reads. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
MGLOBALA(float, DAT_0067d828, 4, 0x0067d828);
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_0067d83c, 0x0067d83c);
MGLOBAL(int, DAT_0067d840, 0x0067d840);
MGLOBAL(int, DAT_0067d844, 0x0067d844);
MGLOBAL(int, DAT_0067d848, 0x0067d848);
int FUN_004cc230(int type, int access, void *name);
int FUN_004cc5e0(int stream, int type, int *length, int *version);
int FUN_00554390(int stream);
int FUN_004cc160(int stream, void *data);
void FUN_00552b60(int p);
int FUN_0042a6b0(char *name, int a, int b);
void FUN_00556cc0(int txd);
int FUN_00556d70(void);
int FUN_00556e90(int font, void *c0, void *c1, void *c2, void *c3);

#ifdef __cplusplus
extern "C" {
#endif
char *strcpy(char *, const char *);
void *memset(void *, int, unsigned int);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(strcpy)
#pragma intrinsic(memset)
#ifndef __cplusplus
// MATCH-HELD: 0x00427880 (static copy for the DDK register convention; port body: re/match/s06/00427880.c)
static int FUN_00427880(char *name)
{
    int font = 0;
    int stream = FUN_004cc230(2, 1, name);
    if (stream) {
        if (FUN_004cc5e0(stream, 0x199, 0, 0)) {
            font = FUN_00554390(stream);
            FUN_004cc160(stream, 0);
        }
    }
    return font;
}
#else
int FUN_00427880(char *name);   /* port: one body, bound by the wrapper's #define */
#endif

// MATCH: 0x00427ca0 FUN_00427ca0
void FUN_00427ca0(void)
{
    RwRGBA white;
    RwRGBA black;
    char rwf[11];
    char txd[11];
    black.alpha = 255;
    white.red = 255;
    white.green = 255;
    white.blue = 255;
    white.alpha = 255;
    black.red = 0;
    black.green = 0;
    black.blue = 0;
    strcpy(rwf, "fgdc20.rwf");
    strcpy(txd, "fgdc20.txd");
    FUN_00552b60(0);
    FUN_00556cc0(FUN_0042a6b0(txd, 0, 0));
    DAT_0067d838 = FUN_00427880(rwf);
    DAT_0067d844 = FUN_00556d70();
    FUN_00556e90(DAT_0067d844, &black, &black, &black, &black);
    DAT_0067d848 = FUN_00556d70();
    FUN_00556e90(DAT_0067d848, &white, &white, &white, &white);
    DAT_0067d83c = FUN_00556d70();
    DAT_0067d840 = FUN_00556d70();
    FUN_00556e90(DAT_0067d83c, &white, &white, &white, &white);
    memset(DAT_0067d828, 0, sizeof(DAT_0067d828));
}
