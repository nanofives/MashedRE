// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x00418560 (first hand source; raw g2s 8.8%): 760 B whole symbol BYTE-IDENTICAL (the
   symbol includes the 4-entry jump table at 0x00418848). Closed by: the AI record indexed directly
   (DAT_0089a4c4[car].f, no pointer local: esi = car * 0x74 and [esi + 0x89a4d0] operands); the game-mode
   dispatch as an if / else-if chain (cmp chain, no sub/dec); the bank-select switch with `case 0` and
   `default` as two separate, identical bodies (table entry 0 = 0x00418754 = the default code; merging them
   into `case 0: default:` gives a dec chain and no table). The three callees are pushed 4 arguments
   (0x42c80000 = 100.0f at 0x00418776 / 0x004187be), so they are declared with 4 parameters here.
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
typedef struct { int id; int pad[3]; } PlayerSlot;                      /* 0x10 B, 4 at 0x007f1a14 */
typedef struct { unsigned char b[8]; char pad[0x4c - 8]; } CtrlRec;    /* 0x4c B at 0x007f1038 */
typedef struct { char data[0x200]; int count; } Bank;                   /* 0x204 B */
typedef struct { Bank bank[3]; } BankSet;                               /* 0x60c B, 4 at 0x00801aa0 */
typedef struct {
    char pad0[8];
    int mode;           /* +0x08 */
    int bank;           /* +0x0c */
    char pad1[0x34 - 0x10];
    int k34;            /* +0x34 */
    int timer;          /* +0x38 */
    char pad2[0x48 - 0x3c];
    int sv[4];          /* +0x48 */
    char pad3[0x74 - 0x58];
} AiVeh;                                                                 /* 0x74 B at 0x0089a4c4 */
MGLOBALA(PlayerSlot, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(CtrlRec, DAT_007f1038, 8, 0x007f1038);
MGLOBALA(BankSet, DAT_00801aa0, 4, 0x00801aa0);
MGLOBALA(AiVeh, DAT_0089a4c4, 8, 0x0089a4c4);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(int, DAT_007f1a50, 0x007f1a50);
MGLOBAL(int, DAT_007f1a68, 0x007f1a68);
MGLOBAL(int, DAT_007f1a6c, 0x007f1a6c);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f1008, 0x007f1008);
int  FUN_0040e350(void);
void FUN_00413fe0(void);
int  FUN_0040e4a0(void);
void FUN_00417180(int car);
void FUN_00416250(Bank *p1, int car, unsigned char *out, float k);
void FUN_00416a30(Bank *p1, int car, unsigned char *out, float k);
void FUN_00417da0(Bank *p1, int car, unsigned char *out, float k);
int  FUN_00426c00(void);
int  FUN_00407a40(int car);
void FUN_00417640(int idx, unsigned char *out);

// MATCH: 0x00418560 _FUN_00418560
void FUN_00418560(int car)
{
    unsigned char *r;
    Bank *bank;
    int t;
    int k;
    int v;

    r = DAT_007f1038[DAT_007f1a14[car].id].b;
    r[4] = 0;
    r[5] = 0;
    r[0] = 0;
    r[1] = 0;
    r[7] = 0;
    r[6] = 0;
    if (FUN_0040e350() == 5) {
        DAT_007f0ff8 = 0;
        FUN_00413fe0();
        t = FUN_0040e4a0();
        k = DAT_0089a4c4[car].k34;
        if (k >= 0) {
            if (t < (k + 0x40) * 100) {
                r[4] = 0xff;
                return;
            }
        } else if (t < (0x4a - k) * 100) {
            r[4] = 0xff;
            return;
        }
        return;
    }
    FUN_00417180(car);
    bank = &DAT_00801aa0[0].bank[0];
    if (DAT_0089a4c4[car].bank > 2)
        DAT_0089a4c4[car].bank = 0;
    if (DAT_0089a4c4[car].mode == 0) {
        if (DAT_00801aa0[0].bank[DAT_0089a4c4[car].bank].count < 4)
            DAT_0089a4c4[car].bank = 0;
        bank = &DAT_00801aa0[0].bank[DAT_0089a4c4[car].bank];
    }
    if (DAT_0089a4c4[car].mode == 2) {
        if (DAT_00801aa0[2].bank[DAT_0089a4c4[car].bank].count < 4)
            DAT_0089a4c4[car].bank = 0;
        if (DAT_00801aa0[2].bank[DAT_0089a4c4[car].bank].count < 4)
            bank = &DAT_00801aa0[0].bank[0];
        else
            bank = &DAT_00801aa0[2].bank[DAT_0089a4c4[car].bank];
    }
    if (DAT_0089a4c4[car].mode == 1) {
        if (DAT_00801aa0[1].bank[DAT_0089a4c4[car].bank].count < 4)
            DAT_0089a4c4[car].bank = 0;
        if (DAT_00801aa0[1].bank[DAT_0089a4c4[car].bank].count < 4)
            bank = &DAT_00801aa0[0].bank[0];
        else
            bank = &DAT_00801aa0[1].bank[DAT_0089a4c4[car].bank];
    }
    if (DAT_0089a4c4[car].mode == 3) {
        if (DAT_00801aa0[3].bank[DAT_0089a4c4[car].bank].count < 4)
            DAT_0089a4c4[car].bank = 0;
        if (DAT_00801aa0[3].bank[DAT_0089a4c4[car].bank].count < 4)
            bank = &DAT_00801aa0[0].bank[0];
        else
            bank = &DAT_00801aa0[3].bank[DAT_0089a4c4[car].bank];
    }
    if (DAT_007f1a50 == 1) {
        switch (DAT_007f1a68) {
        case 1: bank = &DAT_00801aa0[1].bank[DAT_007f1a6c]; break;
        case 2: bank = &DAT_00801aa0[2].bank[DAT_007f1a6c]; break;
        case 3: bank = &DAT_00801aa0[3].bank[DAT_007f1a6c]; break;
        case 0: bank = &DAT_00801aa0[0].bank[DAT_007f1a6c]; break;
        default: bank = &DAT_00801aa0[0].bank[DAT_007f1a6c]; break;
        }
    }
    if (DAT_007f0fd0 == 4 || DAT_007f0fd0 == 9) {
        FUN_00416a30(bank, car, r, 100.0f);
    } else if (DAT_007f0fd0 == 8) {
        FUN_00417da0(bank, car, r, 100.0f);
        if (FUN_00426c00() == 0x21) {
            v = FUN_00407a40(car);
            if (v < 0x44 && v > 0x37)
                r[5] = 0;
            if (v < 0x5f && v > 0x58)
                r[5] = 0;
        }
    } else {
        FUN_00416250(bank, car, r, 100.0f);
    }
    FUN_00417640(car, r);
    if (DAT_0089a4c4[car].timer != 0) {
        if ((DAT_0089a4c4[car].timer = DAT_0089a4c4[car].timer - DAT_007f1008) < 0)
            DAT_0089a4c4[car].timer = 0;
        r[0] = (unsigned char)DAT_0089a4c4[car].sv[0];
        r[1] = (unsigned char)DAT_0089a4c4[car].sv[1];
        r[4] = (unsigned char)DAT_0089a4c4[car].sv[2];
        r[5] = (unsigned char)DAT_0089a4c4[car].sv[3];
    }
    DAT_0089a4c4[car].sv[0] = r[0];
    DAT_0089a4c4[car].sv[1] = r[1];
    DAT_0089a4c4[car].sv[2] = r[4];
    DAT_0089a4c4[car].sv[3] = r[5];
}
