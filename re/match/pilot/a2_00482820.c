// a2 0x00482820 (56 B): best 100.0% (regs 100, ops 100). g2s start 76.0%. Change: callbacks declared as
// functions (Ghidra had them as undefined1 data), plain int types.
// FLAGS: /O2 /GS /Ob1
void FUN_004827a0(void);
void FUN_004827c0(void);
void FUN_00482800(void);
int FUN_0055af80(int a, void *cb, int b);
int FUN_0055afc0(int a, void *cb, int b);
int FUN_0055de20(int a, void *cb, int b);
int FUN_0055dec0(int a);

// MATCH: 0x00482820 _FUN_00482820
void FUN_00482820(int p)
{
    int r = FUN_0055dec0(p);
    FUN_0055afc0(r, FUN_004827a0, r);
    FUN_0055af80(r, FUN_004827c0, r);
    FUN_0055de20(p, FUN_00482800, p);
}
