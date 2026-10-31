// a3 matching pilot, 0x00430290 (987 B). Best: 100.0% (regs 100, ops 100) 300/300 ins.
// The converter's extraout_ECX / extraout_ECX_00 / extraout_EDX are real: the original keeps ECX
// (0x00430337 -> 0x0043033a, 0x00430409 -> 0x0043040c) and EDX (0x004305ce -> 0x004305d1) = idx*0x30
// live across `call 0x0042a920`, and does not re-read DAT_0067f17c. FUN_0042a920 (20 B at 0x0042a920)
// only touches EAX and two globals. An extern prototype gives 55.3% (cl reloads and recomputes after
// every call). Defining FUN_0042a920 in this TU BEFORE the caller, not inlined, gives 100%:
// with the plain definition under /O2 /GS /Ob1 (this file; a3 first used __declspec(noinline) under /O2 /GS, same bytes).
// Defined after the caller -> inlined, 57.0%. Static + noinline -> cl also switches the callee to a
// register argument (66.3%), so the callee had external linkage. Class (b): same-TU callee, intra-module
// register-usage knowledge of cl 13.00, not /GL (the 9176 compile reproduces it from a plain .obj).
// Unrolled loops (12-record count unrolled x6, 10-record count fully unrolled) came out of plain for loops.
// FLAGS: /O2 /GS /Ob1
typedef struct Rec30 {
    int f00, f04, f08, f0c, f10, f14, f18, f1c;
    int pad20[3];
    int f2c;
} Rec30;
extern int DAT_0067e9fc;
extern int DAT_0067f17c;
extern Rec30 DAT_007f0a40[13];
extern int DAT_007f0f2c;
int printf(const char *fmt, ...);
void FUN_004099a0(void);
unsigned int FUN_0040dd60(void);
extern int DAT_00898ab0; extern int DAT_00899140;
/* 0x0042a920: must be in this TU, before the caller, and not inlined (see header) */
void FUN_0042a920(int p0) { DAT_00898ab0 = 0x40; DAT_00899140 = p0; }
// MATCH: 0x00430290 _FUN_00430290
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

// MATCH: 0x0042a920 _FUN_0042a920
