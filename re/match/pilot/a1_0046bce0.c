// FLAGS: /O2 /GS /Ob1
/* a1 0x0046bce0 (54 B). Best: 100% exact.
   Start (g2s) 86.7%: only diff was `cmp eax,0xf / jbe` vs `cmp eax,0x10 / jb`.
   Fix: spell the bound as `i >= 0x10` (same as f_small_a.c 0x0046cbe0). No residual.
   Globals: three dword arrays, stride 0x341 dwords (0xd04 B), at 0x00882094/98/9c. */
extern unsigned int DAT_00882094[];
extern unsigned int DAT_00882098[];
extern unsigned int DAT_0088209c[];

// MATCH: 0x0046bce0 _FUN_0046bce0
int FUN_0046bce0(unsigned int *out, unsigned int i)
{
    if (i >= 0x10)
        return 0;
    out[0] = DAT_00882094[i * 0x341];
    out[1] = DAT_00882098[i * 0x341];
    out[2] = DAT_0088209c[i * 0x341];
    return 1;
}
