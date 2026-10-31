// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041ae20 FUN_0041ae20
// MATCH: 0x0041b520 FUN_0041b520
/* h1 MATCH-S03: 0x0041ae20 takes the record in ECX (static callee, register arg), copies the 12-byte
   vector DAT_005f3304[rec+0x64] into rec+0x50, zeroes rec+0x70, doubles rec+0x50. 0x0041b520 walks
   the 4 records (stride 0x74) at 0x0063c8d0..0x0063caa0. */
typedef struct Vec3_41ae20 { float x, y, z; } Vec3_41ae20;
typedef struct Rec41ae20 {
    char pad0[0x50];
    Vec3_41ae20 v50;   /* +0x50 */
    char pad5c[0x64 - 0x5c];
    int idx64;         /* +0x64 */
    char pad68[0x70 - 0x68];
    int i70;           /* +0x70 */
} Rec41ae20;           /* 0x74 */
#ifndef MATCH_PORT
extern Vec3_41ae20 DAT_005f3304[];
extern Rec41ae20 DAT_0063c8d0[4];
#endif

static void FUN_0041ae20(Rec41ae20 *rec)
{
    rec->v50 = DAT_005f3304[rec->idx64];
    rec->i70 = 0;
    rec->v50.x = rec->v50.x + rec->v50.x;
}

void FUN_0041b520(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_0041ae20(&DAT_0063c8d0[i]);
}
