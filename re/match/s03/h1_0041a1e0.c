// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041a1e0 FUN_0041a1e0
/* h1 MATCH-S03 r2: loads "BTBPanel.dff" (0x005cd16c) via 0x0042a5d0, clears the 0x1c-byte block at
   0x0063c610, stores the clump and its +4 at +0x10/+0x14, sets +0x18 = 1, fills the 4 slots via
   0x004b51d0, then for slot index 1 only calls 0x004b52f0(slot[1], 0x40, 1). Real size 111 B. */
typedef struct Panel63c610 {
    int  slot[4];      /* 0x0063c610 */
    int  clump;        /* 0x0063c620 */
    int  clump4;       /* 0x0063c624 */
    int  active;       /* 0x0063c628 */
} Panel63c610;
#ifndef MATCH_PORT
extern Panel63c610 DAT_0063c610;
int  FUN_0042a5d0(const char *name, int a, int b);
void FUN_004b6520(void *p, int n);
int  FUN_004b51d0(int clump, int *out, int n, int a, int b);
void FUN_004b52f0(int obj, unsigned int a, int b);
#endif

void FUN_0041a1e0(void)
{
    int clump;
    int i;
    clump = FUN_0042a5d0("BTBPanel.dff", 0, 0);
    FUN_004b6520(&DAT_0063c610, 0x1c);
    DAT_0063c610.clump = clump;
    DAT_0063c610.clump4 = *(int *)(clump + 4);
    DAT_0063c610.active = 1;
    FUN_004b51d0(clump, DAT_0063c610.slot, 4, 0, 0);
    for (i = 0; i < 4; i++) {
        if (i == 1)
            FUN_004b52f0(DAT_0063c610.slot[1], 0x40, i);
    }
}
