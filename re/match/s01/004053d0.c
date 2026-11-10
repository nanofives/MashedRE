// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x004053d0 (45 B) 100%, 0x00405400 (23 B) 100%, 0x00405420 (11 B, g2s neighbour) 100%,
   0x00405430 (39 B) 100%. One state block at 0x00639d70..78.
   - DAT_00639d74 is a float: 0x00405400 stores it as an immediate 0 while 0x00639d78/70 reuse xor eax,eax.
     0x00405430 does fld on it.
   - 0x004053d0: store order d74, d78, d70 in source gives the original's d74, d70, d78 schedule (g2s 75%).
   - 0x00405430: two early `return 0;` (second on `>`), then `return 1;` (g2s `&&` form 69.2%). */
#ifdef MATCH_PORT
/* PORT-OT-GAMEPLAY: Match/PortTop_00405400.cpp compiles this file in the exe; globals are references to their
   original addresses (the exe maps the original data range). C under the DDK is unchanged. */
static float &DAT_00639d74 = *(float *)0x00639d74;
static int &DAT_00639d70 = *(int *)0x00639d70;
static int &DAT_00639d78 = *(int *)0x00639d78;
#else
extern float DAT_00639d74;
extern int DAT_00639d70;
extern int DAT_00639d78;
#endif
// MATCH: 0x004053d0 FUN_004053d0
void FUN_004053d0(int param_1, int param_2)
{
  if (param_1 != 0) {
    DAT_00639d74 = 0.0f;
    DAT_00639d78 = param_2;
    DAT_00639d70 = param_1;
    return;
  }
  DAT_00639d70 = 0;
  DAT_00639d78 = 0;
}
// MATCH: 0x00405400 FUN_00405400
void FUN_00405400(void)
{
  DAT_00639d74 = 0.0f;
  DAT_00639d78 = 0;
  DAT_00639d70 = 0;
}
// MATCH: 0x00405420 FUN_00405420
void FUN_00405420(void)
{
  DAT_00639d74 = 0.0f;
}
// MATCH: 0x00405430 FUN_00405430
int FUN_00405430(void)
{
  if (DAT_00639d78 == 0) return 0;
  if (DAT_00639d74 > *(float *)(DAT_00639d70 + 0xc)) return 0;
  return 1;
}
