// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456760 (h4): 100% (regs 100%), 85/85 ins, relocheck OK. Body from the a3 pilot file
   re/match/pilot/a3_00456760.c (100% there): array of 4 x 0x48-B records at 0x006886b8, the outer loop as
   a count-down `i = 4; while (i--)`, the strings as MGLOBALA char arrays.
   Fix over the pilot: the case blocks are laid out in source order, and the pilot's 0..6 order put the
   +0x3c and +0x24 stores in each other's blocks (byte-identical with relocations masked, relocheck BAD).
   The jump table at 0x00456878 maps 0..6 to +0x2c,+0x30,+0x34,+0x3c,+0x28,+0x38,+0x24 with the blocks in
   the order 0,1,2,6,4,5,3, so the cases are written in that order here. Residual: none. */
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
typedef struct S48_00456760 {
    void *clump;      /* +0x00 */
    int pad04[3];
    int f10;          /* +0x10 */
    int pad14[4];
    void *f24, *f28, *f2c, *f30, *f34, *f38, *f3c;
    int pad40[2];
} S48_00456760;       /* 0x48, 4 records at 0x006886b8 */
MGLOBALA(S48_00456760, DAT_006886b8, 4, 0x006886b8);
MGLOBALA(uchar, DAT_006887d0, 4, 0x006887d0);
MGLOBALA(char, s_flash_005ce4cc, 6, 0x005ce4cc);
MGLOBALA(char, s_GatlingGun_dff_005ce4bc, 15, 0x005ce4bc);
int FUN_0040bb30(const char *p0);
void *FUN_0042a5d0(const char *p0, int p1, int p2);
void FUN_00474d60(void *p0, int p1);
void FUN_00476cb0(void *p0, int p1, int p2);
void FUN_004770c0(void *p0, unsigned int p1, unsigned int p2, int p3);
int FUN_004b3fc0(void *p0, void **p1);
int FUN_004b5190(void *p0, int p1, int p2);
void FUN_004c5c80(int p0);
void *FUN_004e6ab0(void *p0);
int FUN_004e6e00(void *p0);
// MATCH: 0x00456760 FUN_00456760
void FUN_00456760(void)
{
    void *src;
    void *list[10];
    int n;
    int i;

    FUN_004770c0(DAT_006887d0, 0x817, 0x32, FUN_0040bb30(s_flash_005ce4cc));
    FUN_00476cb0(DAT_006887d0, 5, 6);
    FUN_004c5c80(0);
    src = FUN_0042a5d0(s_GatlingGun_dff_005ce4bc, 0, 0);
    i = 4;
    while (i--) {
        DAT_006886b8[i].f10 = 0;
        DAT_006886b8[i].clump = FUN_004e6ab0(src);
        n = FUN_004b3fc0(DAT_006886b8[i].clump, list);
        while (n) {
            n--;
            switch (FUN_004b5190(list[n], 0, 0)) {
            case 0: DAT_006886b8[i].f2c = list[n]; break;
            case 1: DAT_006886b8[i].f30 = list[n]; break;
            case 2: DAT_006886b8[i].f34 = list[n]; break;
            case 6: DAT_006886b8[i].f24 = list[n]; break;
            case 4: DAT_006886b8[i].f28 = list[n]; break;
            case 5: DAT_006886b8[i].f38 = list[n]; break;
            case 3: DAT_006886b8[i].f3c = list[n]; break;
            }
        }
        FUN_00474d60(src, 0);
    }
    FUN_004e6e00(src);
}
