// FLAGS: /O2 /GS /Ob1
/* a1 0x00475ef0. Best: 100% exact against the real 57-byte body (0x00475ef0..0x00475f28 `ret`).
   match.py reports 75.0% only because re/console/pc_funcs.csv gives Ghidra size 49, which cuts the
   function before `cmp esi,0x692418 / jl / pop esi / ret` (bytes at 0x00475f1f..0x00475f28, then NOP fill
   from 0x00475f29). Scored with size 57 via match.score_function(..., size=57): 16/16 ins, 100%.
   Residual: none in code. Class (c) tool limit: wrong Ghidra function boundary (size 49 vs 57).
   Loop: 0x10 records of 0xd8 B from 0x00691698 to 0x00692418, FUN_004b6520(rec, 0xd8) (cdecl, plain `ret`),
   then rec+0xc0 = DAT_00613254. */
extern unsigned int DAT_00613254;
extern unsigned char DAT_00691698[];
void FUN_004b6520(void *p, unsigned int n);

// MATCH: 0x00475ef0 _FUN_00475ef0
void FUN_00475ef0(void)
{
    unsigned char *p = DAT_00691698;
    do {
        FUN_004b6520(p, 0xd8);
        *(unsigned int *)(p + 0xc0) = DAT_00613254;
        p += 0xd8;
    } while ((int)p < 0x692418);
}
