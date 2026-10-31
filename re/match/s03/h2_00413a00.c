// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413a00 FUN_00413a00
/* h2 0x00413a00 (64 B). Best: 100% BYTE-IDENTICAL (--bytes), 3rd try. Start (g2s) 62.5%.
   Ghidra lost both register args of the call to FUN_004128f0: the caller's stack param (ctx, [esp+4]) goes
   in EAX and &DAT_0089bea0[i] (0x2c stride) in EDI, the 1.6f (0x3fcccccd) on the stack, caller pops 4.
   FUN_004128f0 (1015 B, not matched here) is a `static` callee with its own register convention.
   The static FUN_004128f0 below is a STAND-IN callee (not the original body): its only job is to get that
   convention (ctx used only before its first call -> EAX, entry live across calls and another pointer
   live in ESI -> EDI). A first stand-in where ctx stayed live gave ctx=EDI / entry=ESI (50%).
   Port note: when #included, the real FUN_004128f0 port must be static in the same TU, defined first.
   Class: none (exact for 0x00413a00). */
#ifndef MATCH_PORT
typedef struct { int d[11]; } Ent2c;
extern Ent2c DAT_0089bea0[];
extern int DAT_0063bc58;
extern int DAT_008a94a0;
void Ext_004128f0(float *v, Ent2c *e, int n, char *p);
extern char DAT_00828320[];
#endif
/* stand-in callee (not matched): FUN_004128f0 is 1015 B; this static stand-in only reproduces its
   register convention (ctx in EAX, entry in EDI kept across calls, float on the stack). */
static void FUN_004128f0(char *ctx, Ent2c *e, float k)
{
  float v[4];
  char *t;

  e->d[4] = 1;
  t = DAT_00828320 + DAT_008a94a0 * 0x70;
  v[0] = *(float *)(ctx + 0x80);
  v[1] = *(float *)(ctx + 0x84);
  Ext_004128f0(v, e, 1, ctx + 0x20);
  *(float *)(t + 4) = *(float *)&e->d[3] * k;
  Ext_004128f0(v, e, 2, t);
}

void FUN_00413a00(char *ctx)
{
  int i;

  DAT_008a94a0 = 0;
  for (i = 0; i < DAT_0063bc58; i++) {
    FUN_004128f0(ctx, &DAT_0089bea0[i], 1.6f);
  }
}
