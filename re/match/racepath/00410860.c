// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH 0x00410860 (first hand source; raw g2s 70.4%): 592 B whole symbol BYTE-IDENTICAL
   (Ghidra body 591 B). The four end-of-round checks are one 4-step loop that cl unrolls (0x00410903..
   0x004109b9). Closed by the timer form `if (n == 0) DAT_005f29b8 = 400; DAT_005f29b8 += 2;` (esi = 400,
   conditional reload, add esi,2) and the goto `won` layout of the win tail (Ghidra's LAB_00410a49).
   Residual: none. */
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
typedef struct { int id; int pad[3]; } PlayerSlot;   /* 0x10 B, 4 at 0x007f1a14 */
typedef struct { char pad[0x13]; char flag; char pad2[0x4c - 0x14]; } CarRec; /* 0x4c B, base 0x007f102f */
MGLOBAL(int, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(int, DAT_007f1018, 0x007f1018);
MGLOBAL(int, DAT_007f1014, 0x007f1014);
MGLOBAL(int *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba80, 0x0063ba80);
MGLOBAL(int, DAT_005f29b8, 0x005f29b8);
MGLOBALA(PlayerSlot, DAT_007f1a14, 4, 0x007f1a14);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_008a94d0, 0x008a94d0);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBALA(char, DAT_007f1042, 0x4c * 16, 0x007f1042);
void FUN_00429b30(void);
int  FUN_0046c7b0(int i);
float FUN_00408a50(int i);
int  FUN_0040b6d0(int i);
int  FUN_004a2cbd(const char *fmt, ...);
int  FUN_0042f500(void);
int  FUN_0042f6a0(void);
int  FUN_00429860(void);
void FUN_0042aab0(void);
void FUN_0043aee0(void);
void FUN_00448700(int a, int b);
void FUN_0040e460(int a);
void FUN_00424b80(void);
void FUN_0041eda0(int i, int on);

// MATCH: 0x00410860 _FUN_00410860
void FUN_00410860(void)
{
    int i;
    int n;
    int end;
    int pts;
    int win;

    DAT_007f1014 = DAT_007f0ff4 > DAT_007f1018;
    FUN_00429b30();
    n = 0;
    for (i = 0; i < 4; i++)
        if (PTR_PTR_005f2770[0xd + i] != 0 && FUN_0046c7b0(i) == 1)
            n = i;
    DAT_0063ba80 = (int)FUN_00408a50(n);
    n = 0;
    for (i = 0; i < 4; i++)
        if (PTR_PTR_005f2770[0xd + i] != 0 && FUN_0046c7b0(i) == 1)
            n++;
    if (n == 0)
        DAT_005f29b8 = 400;
    DAT_005f29b8 += 2;
    end = 0;
    for (i = 0; i < 4; i++) {
        if (DAT_007f1a14[i].id != -1 && DAT_007f1042[DAT_007f1a14[i].id * 0x4c] != 0)
            end = 1;
        if (DAT_007f0fd0 != 1 && n == 0)
            end = 1;
    }
    if (DAT_005f29b8 >= 200 && end == 1) {
        FUN_0042aab0();
        win = 0;
        for (i = 0; i < 4; i++) {
            pts = FUN_0040b6d0(i);
            FUN_004a2cbd("player %d points : %d\n", i, pts);
            if ((DAT_008a94d0 == 2 || DAT_008a94d0 == 3 || FUN_0042f500() != 0) && pts == 8)
                win = i + 1;
            if (DAT_008a94d0 == 4 && pts >= 0xc)
                win = i + 1;
        }
        if (FUN_0042f6a0() == 2 && FUN_00429860() == 0xb) {
            win = 1;
            FUN_0043aee0();
            goto won;
        }
        if (win != 0) {
        won:
            FUN_00448700(1, win - 1);
            FUN_0040e460(1);
            DAT_0063ba8c = 9;
            return;
        }
        DAT_0063ba8c = 8;
        FUN_00448700(0, 0);
        FUN_00424b80();
        for (i = 0; i < 4; i++)
            if (DAT_007f1a14[i].id != -1)
                FUN_0041eda0(i, 1);
    }
}
