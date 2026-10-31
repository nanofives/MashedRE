// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x00410510: s02 form of re/match/pilot/a5_00410510.c (body unchanged, globals moved to MGLOBAL with
   their real addresses). Matched by the whole-symbol byte rule: match.py --bytes = 848 B whole symbol,
   0 differing unmasked bytes (BYTE-IDENTICAL); the instruction score is 98.4% only because Ghidra's 812 B
   body stops before the epilogue (class (c), jump table at 0x00410844). Pilot header follows. */
/* a5 matching pilot, 0x00410510 (812 B per Ghidra). Best: 98.4% exact (regs 98.4, ops 98.4), 246/242 ins.
   All 242 original instructions match; the only diff is 4 trailing instructions (pop esi / pop ebp /
   add esp,0xc / ret) at 0x0041083c-0x00410841 that the original HAS but Ghidra's function body (812 B)
   stops before: class (c), function-boundary tool limit (jump table at 0x00410844 follows).
   Path: raw Ghidra 37.6% -> 70.9% (one result variable r in ESI, switch with break, r==0 check after it)
   -> 91.9% (case bodies in source order 4,9,8,5,7,10, which the compiler lays out verbatim, and the
   common block written `if (r != -1) {...} else {...}`, case 10's last test as `<= 0.0f`, case 10
   ending in break instead of return) -> 98.4% (case 9 written `if (FUN_00417740(0) == -1) {...}` with the
   r=-1 tail after the block: the compiler then cross-jumps that tail into case 10's identical tail at
   0x00410801, exactly as the original; the other three identical tails stay duplicated in both).
   Case 8 `if (r != 1 && r == 0)` reproduces the original's redundant cmp esi,1 / je 0x00410622.
   Constants: 0x005cc31c = 3.0f, 0x005cc9c8 = 0.9f, 0x005cc574 = 2.0f, 0x005d757c = 0.0f,
   0x442f0000 = 700.0f. DAT_005f2770 is a pointer (value 0x005f2728), read as int[13 + i]. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063b90c, 0x0063b90c);
MGLOBAL(int, DAT_008a94d0, 0x008a94d0);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int *, DAT_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(float, DAT_0063b910, 0x0063b910);
MGLOBAL(float, DAT_0063b918, 0x0063b918);
MGLOBAL(float, DAT_0063b91c, 0x0063b91c);
MGLOBAL(int, DAT_0063b914, 0x0063b914);
MGLOBAL(int, DAT_007f0fcc, 0x007f0fcc);
MGLOBAL(float, DAT_007f0fe4, 0x007f0fe4);
int FUN_00405890(void);
int FUN_0040b6d0(int i);
int FUN_0040e340(void);
float FUN_00417730(int i);
int FUN_00417740(int i);
int FUN_00426c00(void);
int FUN_0042f500(void);
int FUN_0042f6a0(void);
void FUN_00448700(int a, int b);
void FUN_0044df80(int a);
void FUN_00458f80(int a);
void FUN_0045bed0(void);
void FUN_0045bf30(void);
int FUN_0046c7b0(int i);
void FUN_0046cbb0(int i, int *a, int *b);

// MATCH: 0x00410510 _FUN_00410510
int FUN_00410510(void)
{
    int r = 0;
    int i, t, ok, dummy;
    float f;
    DAT_0063b90c = 0;
    if (FUN_0042f6a0() == 2)
        return 0;
    for (i = 0; i < 4; i++) {
        t = FUN_0040b6d0(i);
        if ((DAT_008a94d0 == 2 || DAT_008a94d0 == 3 || FUN_0042f500()) && t == 8)
            r = i + 1;
        if (DAT_007f0fd0 == 2 && t == 8)
            r = i + 1;
        if (DAT_008a94d0 == 4 && t >= 12)
            r = i + 1;
    }
    if (r != 0) {
        for (i = 0; i < 4; i++) {
            if (DAT_005f2770[i + 13] != 0 && FUN_0046c7b0(i) == 1) {
                t = FUN_0040b6d0(i);
                if ((DAT_008a94d0 == 2 || DAT_008a94d0 == 3 || FUN_0042f500()) && t == 8)
                    r = i + 1;
                if (DAT_008a94d0 == 4 && t >= 12)
                    r = i + 1;
            }
        }
    }
    switch (DAT_007f0fd0) {
    case 4:
        r = FUN_00417740(0) + 1;
        break;
    case 9:
        if (FUN_00417740(0) == -1) {
            FUN_0046cbb0(0, &ok, &dummy);
            if (ok != 0) {
                r = -1;
                DAT_007f0fcc = 0;
                break;
            }
            f = FUN_00417730(1);
            if (f - FUN_00417730(0) >= 0.9f) {
                r = -1;
                DAT_007f0fcc = 0;
            } else {
                r = 1;
                DAT_007f0fcc = 1;
            }
            break;
        }
        r = -1;
        DAT_007f0fcc = 0;
        break;
    case 8:
        r = FUN_00417740(0) + 1;
        if (r != 1 && r == 0)
            r = -1;
        break;
    case 5:
        if (FUN_0046c7b0(0) == 0)
            r = -1;
        else
            r = FUN_00405890();
        break;
    case 7:
        for (i = 0; i < FUN_0040e340(); i++) {
            if (FUN_00417730(i) > 3.0f) {
                r = i + 1;
                break;
            }
        }
        break;
    case 10:
        r = 0;
        FUN_0046cbb0(0, &ok, &dummy);
        if (ok != 0) {
            r = -1;
            DAT_007f0fcc = 0;
            break;
        }
        if (FUN_00417730(0) >= 2.0f) {
            r = 1;
            DAT_007f0fcc = 1;
            break;
        }
        if (DAT_007f0fe4 <= 0.0f) {
            r = -1;
            DAT_007f0fcc = 0;
        }
        break;
    }
    if (r != 0) {
        FUN_0045bed0();
        FUN_0045bf30();
        FUN_00458f80(0);
        DAT_0063ba8c = 0xb;
        DAT_0063b90c = 1;
        DAT_0063b910 = 700.0f;
        DAT_0063b918 = 0.0f;
        DAT_0063b91c = 0.0f;
        if (r != -1) {
            t = r - 1;
            FUN_00448700(1, t);
            DAT_0063b914 = t;
            DAT_007f0fcc = (t == 0);
        } else {
            FUN_00448700(1, 0);
            DAT_0063b914 = 1;
            DAT_007f0fcc = 0;
        }
        if (FUN_00426c00() == 0x26)
            FUN_0044df80(0);
    }
    return r;
}
