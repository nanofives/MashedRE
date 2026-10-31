// FLAGS: /O2 /GS /Ob1
/* 0x00405730 (80 B). Best 96.2%, residual (b): one store scheduled late (timer store at +0 lands after `mov ecx,eax; pop edi` in ours, before it in the original). Found: table 0x0063a220 is E12[5][10] {float timer; float reset; int handle} (stride 0xc, row 0x78), counts at 0x0063a478; float p4 (int typing gave 84.6%). Tried: chained/split/reordered timer/reset stores, count++ forms, 1-D index, pointer temp, handle typed void*; permuter 124 compiles no gain. */
typedef struct { float a; float b; int h; } E12;
extern E12 DAT_0063a220[][10];
extern int DAT_0063a478[];
int FUN_00458e00(float *p0, int p1);
// MATCH: 0x00405730 FUN_00405730
void FUN_00405730(int i, float *p2, int p3, float p4)
{
    int n = DAT_0063a478[i];
    DAT_0063a220[i][n].h = FUN_00458e00(p2, p3); DAT_0063a478[i] = n + 1; DAT_0063a220[i][n].a = p4; DAT_0063a220[i][n].b = p4;
}
