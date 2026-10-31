// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412890 FUN_00412890
/* h2 0x00412890 (Ghidra 80 B, real 86 B: Ghidra's size drops `mov [0x0063bc54],eax / pop esi / ret`).
   Best: 100% BYTE-IDENTICAL (--bytes 86 B), first try. Start (g2s) 72.0%. 4096 six-short entries at
   0x0089d4a0 (quad -> two triangles: b, b+1, b+3, b, b+2, b+3 with b = i*4), stores written in index
   order (cl reorders them), then DAT_0063bc54 = FUN_0040bb30("headlight") (cdecl, string at 0x005cd040).
   Class: none (exact). */
typedef struct { short v[6]; } Idx6;
#ifndef MATCH_PORT
extern Idx6 DAT_0089d4a0[4096];
extern int DAT_0063bc54;
int FUN_0040bb30(const char *name);
#endif
void FUN_00412890(void)
{
  int i;
  short b;

  for (i = 0; i < 4096; i++) {
    b = (short)(i * 4);
    DAT_0089d4a0[i].v[0] = b;
    DAT_0089d4a0[i].v[1] = (short)(b + 1);
    DAT_0089d4a0[i].v[2] = (short)(b + 3);
    DAT_0089d4a0[i].v[3] = b;
    DAT_0089d4a0[i].v[4] = (short)(b + 2);
    DAT_0089d4a0[i].v[5] = (short)(b + 3);
  }
  DAT_0063bc54 = FUN_0040bb30("headlight");
}
