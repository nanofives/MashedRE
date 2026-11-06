// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048af70 (h7): 100%. Raw 76.0% (Ghidra's fastcall param was the push ecx local slot). What moved it: void(void), 0xa4-byte record array at 0x007201d0 (same records as 0x0048ae00, based 0xc lower), index loops with a per-record pointer rec = &DAT_007201d0[i] (direct indexing based the induction pointer at +0x28). Residual: none. */
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
typedef struct Rec007201d0 {
    int pad00[3];
    int a;              /* +0x0c */
    int b;              /* +0x10 */
    int pad14[5];
    int frame;          /* +0x28 */
    int atomic[10];     /* +0x2c */
    int pad54[10];
    int flag[10];       /* +0x7c */
} Rec007201d0;
MGLOBAL(char, DAT_00720040, 0x00720040);
MGLOBAL(char, DAT_00720080, 0x00720080);
MGLOBALA(Rec007201d0, DAT_007201d0, 8, 0x007201d0);
void FUN_004768c0(void *p0);
int FUN_004c0c20(int p0);
int FUN_004c1210(int p0);
int FUN_004e6920(int p0);
int FUN_004e7e30(int p0, int p1);

// MATCH: 0x0048af70 _FUN_0048af70
void FUN_0048af70(void)
{
    int i, j;
    int atomic;
    int frame;
    Rec007201d0 *rec;

    FUN_004768c0(&DAT_00720040);
    FUN_004768c0(&DAT_00720080);
    for (i = 0; i < 8; i++) {
        rec = &DAT_007201d0[i];
        for (j = 0; j < 10; j++) {
            atomic = rec->atomic[j];
            frame = *(int *)(atomic + 4);
            FUN_004e7e30(atomic, 0);
            FUN_004c1210(frame);
            FUN_004c0c20(frame);
            FUN_004e6920(atomic);
        }
        FUN_004c0c20(rec->frame);
    }
}
