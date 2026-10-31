// a2 0x0046d510 (84 B): best 100.0% (regs 100, ops 100). g2s start 92.9%. Change: struct array stride 0xd04
// based at 0x00881e80 (fields +0x48, +0xf4..+0xfc) instead of three separate int arrays, and >= 0x10 compare.
// FLAGS: /O2 /GS /Ob1
typedef struct { unsigned char pad0[0x48]; int b[0x2b]; int v[3]; unsigned char pad[0xd04 - 0x48 - 0xac - 12]; } Rec46d;
/* base 0x00881e80: +0x48 = 0x00881ec8, +0xf4 = 0x00881f74 */
extern Rec46d DAT_00881e80[];
extern unsigned char DAT_00614708;
int FUN_004c3df0(void *a, void *b, int c, void *d);

// MATCH: 0x0046d510 _FUN_0046d510
int FUN_0046d510(int *out, unsigned int i)
{
    if (i >= 0x10)
        return 0;
    FUN_004c3df0(DAT_00881e80[i].v, &DAT_00614708, 1, DAT_00881e80[i].b);
    out[0] = DAT_00881e80[i].v[0];
    out[1] = DAT_00881e80[i].v[1];
    out[2] = DAT_00881e80[i].v[2];
    return 1;
}
