// FLAGS: /O2 /GS /Ob1
/* a2 0x00403050 (125 B). Best: 100%.
   g2s 47.2%: /GS cookie dropped. The original carries a cookie with only a 4-byte frame, so the source had
   a char array (> 4 B, a 4-byte one gets no cookie) that the optimiser removed afterwards. RwGlobals +0x20
   (RenderStateSet) is a cdecl slot (caller clears, folded into the final add esp,0x24). */
typedef struct {
  void *s[8];
  int (*RenderStateSet)(int state, int value); /* +0x20, cdecl (caller clears) */
} RwGlobalsT;
extern int DAT_00771964;
extern RwGlobalsT *DAT_007d3ff8;
void FUN_00402fb0(void);
void FUN_00428760(int p0, float p1, float p2, float p3, float p4, int p5);

// MATCH: 0x00403050 FUN_00403050
void FUN_00403050(void)
{
  char buf[64]; /* unused: only makes /GS emit the cookie; any char array > 4 B works */
  (void)buf;
  if (DAT_00771964 != 0) {
    FUN_00428760(DAT_00771964, 320.0f, 200.0f, 480.0f, 240.0f, 2);
  }
  DAT_007d3ff8->RenderStateSet(6, 0);
  DAT_007d3ff8->RenderStateSet(8, 0);
  FUN_00402fb0();
  DAT_007d3ff8->RenderStateSet(6, 1);
  DAT_007d3ff8->RenderStateSet(8, 1);
}
