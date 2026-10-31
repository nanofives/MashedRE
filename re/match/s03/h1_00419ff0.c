// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00419ff0 FUN_00419ff0
/* h1 MATCH-S03 r2: clears the 0x440-byte block at 0x0063c1c8, sets it up via 0x004770c0 with the "smoke"
   lookup (string at 0x005cca08), passes the local pair {0.35f, 0.35f} (0x3eb33333) to 0x00476ae0,
   0x00476c10(blk, 0), then tail-jumps 0x00418e50. */
#ifndef MATCH_PORT
extern char DAT_0063c1c8[0x440];
void FUN_004b6520(void *p, int n);
int  FUN_0040bb30(const char *name);
void FUN_004770c0(void *blk, unsigned int a, unsigned int b, int c);
void FUN_00476ae0(void *blk, float *v);
void FUN_00476c10(void *blk, void *p);
void FUN_00418e50(void);
#endif

void FUN_00419ff0(void)
{
    float v[2];
    v[0] = 0.35f;
    v[1] = 0.35f;
    FUN_004b6520(DAT_0063c1c8, 0x440);
    FUN_004770c0(DAT_0063c1c8, 0x895, 0x40, FUN_0040bb30("smoke"));
    FUN_00476ae0(DAT_0063c1c8, v);
    FUN_00476c10(DAT_0063c1c8, 0);
    FUN_00418e50();
}
