/* 0x004077e0 (18 B). Best 100.0%. Float field +0x60 of the 0xec-byte record array at 0x00639d80. */
// FLAGS: /O2 /GS /Ob1
struct Rec { char pad0[0x60]; float f60; char pad64[0xec - 0x64]; };
extern struct Rec DAT_00639d80[];
extern int DAT_0063a5d8;

// MATCH: 0x004077e0 FUN_004077e0
float FUN_004077e0(void)
{
    return DAT_00639d80[DAT_0063a5d8].f60;
}
