// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414120 FUN_00414120
/* h1 MATCH-S03: BYTE-IDENTICAL over the real 81 B (Ghidra size 69 is short, ret at 0x00414170).
   Key: 0x0089a378..0x0089a4b3 must be ONE struct object. As separate globals (int or float, 2D, 1D,
   [][3], V3 struct rows, do-while) cl 13.00.9176 distributes the loop into rep stosd + rep movsd (6.2%).
   +0x4 is a float (stored as immediate 0, not via the edx zero). */
#ifndef MATCH_PORT
typedef struct G0089a378 {
    int a;             /* 0x0089a378 */
    float b;           /* 0x0089a37c */
    int c;             /* 0x0089a380 */
    int B[13][3];      /* 0x0089a384 */
    int A[13][3];      /* 0x0089a420 */
} G0089a378;
extern G0089a378 DAT_0089a378;
extern int DAT_005f2a70[13][3];
#endif

void FUN_00414120(void)
{
    int i;
    int j;
    DAT_0089a378.b = 0.0f;
    DAT_0089a378.a = 0;
    DAT_0089a378.c = 0;
    for (i = 0; i < 13; i++)
        for (j = 0; j < 3; j++) {
            DAT_0089a378.A[i][j] = 0;
            DAT_0089a378.B[i][j] = DAT_005f2a70[i][j];
        }
}
