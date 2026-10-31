// a3 matching pilot, 0x00456760 (279 B). Best: 100.0% (regs 100, ops 100) 85/85 ins.
// Moved by: array of 4 x 0x48-byte records at 0x006886b8 (clump +0x0, +0x10 zeroed, seven atomics
// +0x24..+0x3c by FUN_004b5190's 0..6 result), and the outer loop written as a count-down
// `i = 4; while (i--)` (for (i = 3; i >= 0; i--) gave 94.1%: jns + different nop pad). Residual: none.
// Callee 0x004e6e00 is what the converter named RpClumpDestroy.
// FLAGS: /O2 /GS /Ob1
typedef struct S48 {
    void *clump;      /* +0x00 */
    int pad04[3];
    int f10;          /* +0x10 */
    int pad14[4];
    void *f24, *f28, *f2c, *f30, *f34, *f38, *f3c;
    int pad40[2];
} S48;
extern S48 DAT_006886b8[4];
extern unsigned char DAT_006887d0[];
int FUN_0040bb30(const char *p0);
void *FUN_0042a5d0(const char *p0, int p1, int p2);
void FUN_00474d60(void *p0, int p1);
void FUN_00476cb0(void *p0, int p1, int p2);
void FUN_004770c0(void *p0, unsigned int p1, unsigned int p2, int p3);
int FUN_004b3fc0(void *p0, void **p1);
int FUN_004b5190(void *p0, int p1, int p2);
void FUN_004c5c80(int p0);
void *FUN_004e6ab0(void *p0);
int FUN_004e6e00(void *p0);
// MATCH: 0x00456760 _FUN_00456760
void FUN_00456760(void)
{
    void *src;
    void *list[10];
    int n;
    int i;
    S48 *s;

    FUN_004770c0(DAT_006887d0, 0x817, 0x32, FUN_0040bb30("flash"));
    FUN_00476cb0(DAT_006887d0, 5, 6);
    FUN_004c5c80(0);
    src = FUN_0042a5d0("GatlingGun.dff", 0, 0);
    i = 4;
    while (i--) {
        DAT_006886b8[i].f10 = 0;
        DAT_006886b8[i].clump = FUN_004e6ab0(src);
        n = FUN_004b3fc0(DAT_006886b8[i].clump, list);
        while (n) {
            n--;
            switch (FUN_004b5190(list[n], 0, 0)) {
            case 0: DAT_006886b8[i].f2c = list[n]; break;
            case 1: DAT_006886b8[i].f30 = list[n]; break;
            case 2: DAT_006886b8[i].f34 = list[n]; break;
            case 3: DAT_006886b8[i].f3c = list[n]; break;
            case 4: DAT_006886b8[i].f28 = list[n]; break;
            case 5: DAT_006886b8[i].f38 = list[n]; break;
            case 6: DAT_006886b8[i].f24 = list[n]; break;
            }
        }
        FUN_00474d60(src, 0);
    }
    FUN_004e6e00(src);
}
