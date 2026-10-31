// FLAGS: /O2 /GS /Ob1
/* a4 / 0x0045d250 (220 B): best 100.0% (regs 100, ops 100), first try.
 * Ghidra raw 65.0%: it had folded the known -1 of `b` into the first FUN_0045cb20 call
 * (the original pushes ebx, i.e. the variable) and its raw types did not compile to the same
 * shape. Clean C with if/else-if in Ghidra order, `for` loop, int returns: exact. Residual: none. */
int FUN_0040e350(void);
int FUN_0040e4b0(void);
int FUN_0040e500(void);
void FUN_0045ca30(void);
int FUN_0045cab0(int);
void FUN_0045cb20(int, int, int);
void FUN_0045d080(void);
extern int DAT_007f0fd0;
extern int DAT_0088fbc0;

// MATCH: 0x0045d250 _FUN_0045d250
void FUN_0045d250(void)
{
    int mode = FUN_0040e350();
    int sub = DAT_007f0fd0;
    int a = FUN_0040e4b0();
    int b = FUN_0040e500();
    int i;

    if (a != -1) {
        if (DAT_0088fbc0 == mode)
            return;
        if (mode == 7 && b == -1) {
            FUN_0045d080();
            FUN_0045cb20(1, a, b);
        }
    } else if (b == -1 && mode != 0xb) {
        for (i = 0; i < 4; i++) {
            if (FUN_0045cab0(i))
                FUN_0045cb20(0x27, i, -1);
        }
    }
    if (DAT_0088fbc0 != mode) {
        if (mode == 0xb && b != -1) {
            FUN_0045d080();
            FUN_0045cb20(0, b, -1);
            FUN_0045ca30();
        }
        if (DAT_0088fbc0 != mode && mode == 0xb && sub == 4) {
            FUN_0045d080();
            FUN_0045cb20(0, 0, -1);
            FUN_0045ca30();
        }
    }
}
