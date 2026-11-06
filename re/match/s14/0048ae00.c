// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048ae00 (h7): 100% by bytes (match.py --bytes: 357 B whole symbol BYTE-IDENTICAL; the instruction view shows 98.3% only because Ghidra's size stops before add esp,0xc / ret). Raw 74.1%. What moved it: index loops over a 0xa4-byte record array at 0x007201dc (8 records, 10 atomics each), FUN_004c0b30 result taken before the two zero stores, the 0x007220f0 slot pointer advanced in the for-increment. Residual: none. */
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
typedef struct Rec0048ae00 {
    int a;              /* +0x00 */
    int b;              /* +0x04 */
    int pad08[5];
    int frame;          /* +0x1c */
    int atomic[10];     /* +0x20 */
    int pad48[10];
    int flag[10];       /* +0x70 */
    int pad98[3];
} Rec0048ae00;
MGLOBAL(char, DAT_00720040, 0x00720040);
MGLOBAL(char, DAT_00720080, 0x00720080);
MGLOBAL(char, DAT_007200c0, 0x007200c0);
MGLOBALA(int, DAT_007200d0, 0x40, 0x007200d0);
MGLOBALA(Rec0048ae00, DAT_007201dc, 8, 0x007201dc);
MGLOBALA(int, DAT_007220f0, 8, 0x007220f0);
int FUN_0040bb30(char *p0);
int FUN_0042a5d0(char *p0, int p1, int p2);
void FUN_004770c0(void *p0, int p1, int p2, int p3);
int FUN_004b3f90(int p0);
void FUN_004b52f0(int p0, int p1, int p2);
void FUN_004b6480(void *p0, int p1);
int FUN_004c0b30(void);
int FUN_004c1040(int p0, int p1);
int FUN_004e69a0(int p0);
int FUN_004e7e30(int p0, int p1);
int FUN_005449f0(int p0, int p1);
void FUN_00544a70(int p0, int p1, int p2, int p3, float p4);
int FUN_00544ad0(int p0, int p1);
int FUN_00544bf0(int p0, int p1);
int FUN_004e6e00(int p0);

// MATCH: 0x0048ae00 _FUN_0048ae00
void FUN_0048ae00(void)
{
    int clump;
    int geo;
    int mat;
    int *slot;
    Rec0048ae00 *rec;
    int *p;
    int i, j;
    int f, a;

    FUN_004770c0(&DAT_00720080, 0x817, 0x80, FUN_0040bb30("wfall"));
    FUN_004770c0(&DAT_00720040, 0x80c, 0x40, FUN_0040bb30("shockwave"));
    FUN_004b6480(&DAT_007200c0, 0x80);
    for (i = 0; i < 0x40; i++) DAT_007200d0[i] = 0;
    clump = FUN_0042a5d0("Splash.dff", 0, 0);
    geo = FUN_004b3f90(clump);
    mat = *(int *)(geo + 0x18);
    if (mat != 0) {
        FUN_005449f0(mat, 1);
        FUN_00544a70(mat, 0, 0, 1, 1.0f);
        FUN_00544ad0(geo, 0);
        FUN_00544bf0(geo, 0);
    }
    slot = DAT_007220f0;
    for (i = 0; i < 8; i++, slot++) {
        *slot = 0;
        f = FUN_004c0b30();
        DAT_007201dc[i].a = 0;
        DAT_007201dc[i].b = 0;
        DAT_007201dc[i].frame = f;
        for (j = 0; j < 10; j++) {
            f = FUN_004c0b30();
            a = FUN_004e69a0(geo);
            DAT_007201dc[i].atomic[j] = a;
            FUN_004e7e30(a, f);
            FUN_004c1040(DAT_007201dc[i].frame, f);
            FUN_004b52f0(DAT_007201dc[i].atomic[j], 0x40, 1);
            FUN_00544ad0(DAT_007201dc[i].atomic[j], 0);
            FUN_00544bf0(DAT_007201dc[i].atomic[j], 0);
            DAT_007201dc[i].flag[j] = 0;
        }
    }
    FUN_004e6e00(clump);
}
