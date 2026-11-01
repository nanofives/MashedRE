// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00430290 (h9): Championship::Complete. 100% (regs 100%), 300/300 ins. Residual: none. Raw g2s 41.3% (extraout_ECX/EDX). Body taken from the
   MATCH-PILOT a3 file (re/match/pilot/a3_00430290.c, 100%), rewritten into the S07 form: globals via MGLOBAL /
   MGLOBALA, callees as FUN_<va8>. The original keeps ECX/EDX = DAT_0067f17c*0x30 live across `call 0x0042a920`
   (0x00430337, 0x00430409, 0x004305ce); that comes out only when FUN_0042a920 is defined in this file BEFORE the
   caller with external linkage and not inlined (pilot: extern prototype 55.3%, defined after 57.0%, static+noinline
   66.3%). So this file also holds FUN_0042a920 (20 B, its own MATCH line). Unrolled loops (12-record count x6,
   10-record count full) come from plain for loops. printf is the CRT entry 0x004a2cbd (FID_conflict:_wprintf). */
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
/* ---- end preamble ---- */
typedef struct Rec30 {
    int f00, f04, f08, f0c, f10, f14, f18, f1c;
    int pad20[3];
    int f2c;
} Rec30;
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(Rec30, DAT_007f0a40, 13, 0x007f0a40);
MGLOBAL(int, DAT_007f0f2c, 0x007f0f2c);
MGLOBAL(int, DAT_00898ab0, 0x00898ab0);
MGLOBAL(int, DAT_00899140, 0x00899140);
int printf(const char *fmt, ...);
void FUN_004099a0(void);
unsigned int FUN_0040dd60(void);
// HELD: 0x0042a920 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
void FUN_0042a920(int p0) { DAT_00898ab0 = 0x40; DAT_00899140 = p0; }
#else
void FUN_0042a920(int p0);
#endif
// MATCH: 0x00430290 FUN_00430290
void FUN_00430290(void)
{
    int i, n, snd, all;

    if ((DAT_0067e9fc == 3 || DAT_0067e9fc == 4 || DAT_0067e9fc == 5) && FUN_0040dd60()) {
        if (DAT_0067e9fc == 3) {
            if (DAT_007f0a40[DAT_0067f17c].f04 != 2) {
                snd = 0;
                n = 0;
                for (i = 0; i < 12; i++)
                    if (DAT_007f0a40[i].f04 == 2)
                        n++;
                if (n == 2)
                    snd = 0x264;
                else if (n == 9)
                    snd = 0x267;
                FUN_0042a920(snd);
            }
            DAT_007f0a40[DAT_0067f17c].f04 = 2;
            if (DAT_007f0a40[DAT_0067f17c].f08 != 2)
                DAT_007f0a40[DAT_0067f17c].f08 = 1;
            DAT_007f0a40[DAT_0067f17c].f10 = 2;
            DAT_007f0a40[DAT_0067f17c].f00 = 1;
            DAT_007f0a40[DAT_0067f17c + 1].f00 = 1;
            DAT_007f0a40[DAT_0067f17c].f2c = 1;
            DAT_007f0a40[DAT_0067f17c + 1].f2c = 1;
            n = 0;
            for (i = 0; i < 12; i++)
                if (DAT_007f0a40[i].f04 == 2)
                    n++;
            if (n) {
                for (i = 1; n; i++, n--) {
                    if (i < 11 && DAT_007f0a40[i].f04 != 2) {
                        DAT_007f0a40[i].f04 = 1;
                        DAT_007f0a40[i].f0c = 2;
                    }
                }
                FUN_004099a0();
                return;
            }
        } else {
            if (DAT_0067e9fc == 4) {
                if (DAT_007f0a40[DAT_0067f17c].f08 != 2)
                    FUN_0042a920(0x265);
                DAT_007f0a40[DAT_0067f17c].f08 = 2;
                if (DAT_007f0a40[DAT_0067f17c].f14 != 2)
                    DAT_007f0a40[DAT_0067f17c].f14 = 1;
                DAT_007f0a40[DAT_0067f17c].f18 = 2;
                n = 0;
                for (i = 0; i < 10; i++)
                    if (DAT_007f0a40[i].f08 == 2)
                        n++;
                printf("silver add=%d\n", n);
                printf("TOTALNUMTRACKS-3=%d\n", 10);
                if (n == 10) {
                    printf("HERE!!\n");
                    if (DAT_007f0a40[11].f04 != 2) {
                        DAT_007f0a40[11].f04 = 1;
                        DAT_007f0a40[11].f0c = 2;
                        FUN_0042a920(0x268);
                    }
                }
            }
            if (DAT_0067e9fc == 5) {
                if (DAT_007f0a40[DAT_0067f17c].f14 != 2) {
                    snd = 0x266;
                    DAT_007f0a40[DAT_0067f17c].f14 = 2;
                    all = 1;
                    for (i = 0; i < 12; i++) {
                        if (DAT_007f0a40[i].f04 != 2) all = 0;
                        if (DAT_007f0a40[i].f08 != 2) all = 0;
                        if (DAT_007f0a40[i].f14 != 2) all = 0;
                    }
                    if (all) {
                        snd = 0x26a;
                        DAT_007f0f2c = 1;
                        FUN_0042a920(snd);
                    }
                    FUN_0042a920(snd);
                }
                DAT_007f0a40[DAT_0067f17c].f14 = 2;
                DAT_007f0a40[DAT_0067f17c].f1c = 2;
                n = 0;
                for (i = 0; i < 10; i++)
                    if (DAT_007f0a40[i].f14 == 2)
                        n++;
                if (n == 10 && DAT_007f0a40[12].f04 != 2) {
                    DAT_007f0a40[12].f04 = 1;
                    DAT_007f0a40[12].f0c = 2;
                    FUN_0042a920(0x269);
                }
            }
        }
        FUN_004099a0();
    }
}
