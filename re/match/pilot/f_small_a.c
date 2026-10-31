// FLAGS: /O2 /GS /Ob1
double sin(double);
#pragma intrinsic(sin)
extern float DAT_00644360;
extern float DAT_007f0f00;
extern unsigned int DAT_00881f90[];   /* stride 0x341 dwords (0xd04 bytes) */
extern unsigned int DAT_00881f94[];

// MATCH: 0x00426df0 _FUN_00426df0
float FUN_00426df0(void) { return DAT_00644360; }

// MATCH: 0x00431b20 _FUN_00431b20
/* 1.57 = the double at 0x005cd8f0 */
float FUN_00431b20(void) { return (float)sin(DAT_007f0f00 * 1.57); }

// MATCH: 0x0046cbe0 _FUN_0046cbe0
int FUN_0046cbe0(unsigned int i, unsigned int a, unsigned int b)
{
    if (i >= 0x10)
        return 0;
    DAT_00881f90[i * 0x341] = a;
    DAT_00881f94[i * 0x341] = b;
    return 1;
}
