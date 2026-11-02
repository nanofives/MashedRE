// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004881d0 (h4): 100%. Same shape as 0x004880a0 (local 4-float struct assigned whole) under a DAT_007030b0 once-guard, then calls 0x004880a0. Converter 72.4%. */
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
typedef struct V4 { float x, y, z, w; } V4;
MGLOBAL(int, DAT_007030a8, 0x007030a8);
MGLOBAL(int, DAT_007030b0, 0x007030b0);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
int FUN_004c0b30(void);
int FUN_00534b60(int p0, unsigned int p1, int p2);
int FUN_004e7e30(int p0, int p1);
int *FUN_004e8090(int p0, int p1);
void FUN_004880a0(int param_1);

#define OBJ(h) (*(char **)(DAT_007dc57c + (h)))

// MATCH: 0x004881d0 FUN_004881d0
void FUN_004881d0(int param_1)
{
    int r;
    V4 v;
    r = FUN_004c0b30();
    if (DAT_007030b0 == 0) {
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
        v.w = 10.0f;
        DAT_007030a8 = FUN_00534b60(0x200, 0x10000087, 0);
        *(int *)(OBJ(DAT_007030a8) + 0xa4) = 5;
        *(int *)(OBJ(DAT_007030a8) + 0xa8) = 2;
        *(unsigned int *)(OBJ(DAT_007030a8) + 0x40) |= 0x10000000;
        *(unsigned int *)(OBJ(DAT_007030a8) + 0x40) |= 0x800000;
        *(int *)(OBJ(DAT_007030a8) + 4) = 0x200;
        FUN_004e7e30(DAT_007030a8, r);
        *(V4 *)(*(int *)(*(int *)(DAT_007030a8 + 0x18) + 0x5c) + 4) = v;
        if (param_1 != 0) {
            *(char *)(param_1 + 0x50) = 2;
            FUN_004e8090(**(int **)(*(int *)(DAT_007030a8 + 0x18) + 0x20), param_1);
        }
        *(int *)(OBJ(DAT_007030a8) + 0xac) = 1;
        DAT_007030b0 = 1;
        FUN_004880a0(param_1);
    }
}
