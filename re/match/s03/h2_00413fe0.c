// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413fe0 FUN_00413fe0
/* h2 0x00413fe0 (73 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 84.0%.
   for (i < 4) over the 0x74-stride table at 0x0089a4c0 (dword fields 11,12,13,1,2,3,4,18,23,24,25,26 zeroed in
   that order; cl strength-reduces to a pointer at +0x30) plus a 0x14-stride table at 0x008032d4 (+0 = 1000).
   Class: none (exact). */
#ifndef MATCH_PORT
typedef struct { int d[29]; } Rec74;
typedef struct { int v; int p[4]; } Ent20;
extern Rec74 DAT_0089a4c0[4];
extern Ent20 DAT_008032d4[4];
extern int DAT_0089a36c;
#endif
void FUN_00413fe0(void)
{
  int i;

  DAT_0089a36c = 0;
  for (i = 0; i < 4; i++) {
    DAT_0089a4c0[i].d[11] = 0;
    DAT_0089a4c0[i].d[12] = 0;
    DAT_0089a4c0[i].d[13] = 0;
    DAT_0089a4c0[i].d[1] = 0;
    DAT_0089a4c0[i].d[2] = 0;
    DAT_0089a4c0[i].d[3] = 0;
    DAT_0089a4c0[i].d[4] = 0;
    DAT_0089a4c0[i].d[18] = 0;
    DAT_0089a4c0[i].d[23] = 0;
    DAT_0089a4c0[i].d[24] = 0;
    DAT_0089a4c0[i].d[25] = 0;
    DAT_0089a4c0[i].d[26] = 0;
    DAT_008032d4[i].v = 1000;
  }
}
