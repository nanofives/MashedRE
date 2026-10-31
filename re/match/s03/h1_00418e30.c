// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418e30 FUN_00418e30
// MATCH: 0x00418de0 FUN_00418de0
/* h1 MATCH-S03: walks the 4 records (stride 0x64) at 0x0063bda0..0x0063bf30 calling 0x00418de0 with
   EAX = record. The register arg needs FUN_00418de0 as a static callee defined before the caller, so the
   file carries a transcription of 0x00418de0's body. Not claimed (not on h1's list), but with a MATCH
   line it is also BYTE-IDENTICAL (73 B, add esp 0x2c confirms FUN_00413c70 takes 3 args).
   Signed jl on the pointer = an int index loop, as in 0x0041b520. */
#ifndef MATCH_PORT
typedef struct Rec418de0 {
    char pad[0x44];
    int  f44;   /* +0x44 */
    int  f48;   /* +0x48 */
    int  f4c;   /* +0x4c */
    int  f50;   /* +0x50 */
    int  f54;   /* +0x54 */
    int  f58;   /* +0x58 */
    int  f5c;   /* +0x5c */
    char pad60[0x64 - 0x60];
} Rec418de0;
extern Rec418de0 DAT_0063bda0[4];
void FUN_00413c70(int a, int b, int c);
void FUN_004661a0(int a, int b);
#endif

static void FUN_00418de0(Rec418de0 *rec)
{
    rec->f44 = 0;
    rec->f48 = 0;
    rec->f58 = 0;
    rec->f5c = 0;
    rec->f50 = 0;
    rec->f54 = 0;
    FUN_00413c70(rec->f4c, 2, 0);
    FUN_004661a0(0x14, 0);
    FUN_004661a0(0x14, 1);
    FUN_004661a0(0x14, 2);
    FUN_004661a0(0x14, 3);
}

void FUN_00418e30(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00418de0(&DAT_0063bda0[i]);
}
