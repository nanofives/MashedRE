// FLAGS: /O2 /GS /Ob1
/* a6 0x00409790 (248 B): 100% BYTES-OK. Lua-style switch on FUN_0055dec0(L)-0xc. One float local shared
   by all cases (case 0xc reads it as int via *(int*)&f): two separate locals made a bigger frame.
   0x0063a5f0 = array of V3 (stride 0xc) indexed by DAT_0063b830. Case 0x10 body first in source. */
typedef struct { float x, y, z; } V3;
extern int DAT_0063b830;
extern V3 DAT_0063a5f0[];
extern char DAT_0066d728[];
int FUN_004096a0(void);
void FUN_004292c0(void *L, int n);
void FUN_004292d0(void *L, void *out);
int FUN_0055dec0(void *L);
// MATCH: 0x00409790 FUN_00409790
void FUN_00409790(void)
{

    float f;
    switch (FUN_0055dec0(DAT_0066d728)) {
    case 0x10:
        FUN_004292c0(DAT_0066d728, 0);
        FUN_004096a0();
        break;
    case 0xc:
        FUN_004292d0(DAT_0066d728, &f);
        FUN_004292c0(DAT_0066d728, 0);
        DAT_0063b830 = *(int *)&f;
        break;
    case 0xd:
        FUN_004292d0(DAT_0066d728, &f);
        FUN_004292c0(DAT_0066d728, 0);
        DAT_0063a5f0[DAT_0063b830].x = f;
        break;
    case 0xe:
        FUN_004292d0(DAT_0066d728, &f);
        FUN_004292c0(DAT_0066d728, 0);
        DAT_0063a5f0[DAT_0063b830].y = f;
        break;
    case 0xf:
        FUN_004292d0(DAT_0066d728, &f);
        FUN_004292c0(DAT_0066d728, 0);
        DAT_0063a5f0[DAT_0063b830].z = f;
        break;
    }
}
