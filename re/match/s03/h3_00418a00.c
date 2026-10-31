// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418a00 _FUN_00418a00
// MATCH: 0x00418a30 _FUN_00418a30
// MATCH: 0x00418a70 _FUN_00418a70
// MATCH: 0x00419090 _FUN_00419090
// MATCH: 0x004190f0 _FUN_004190f0
// MATCH: 0x004194f0 FUN_004194f0
/* Static group for the 0x6c-byte records at 0x0063c018 (4) and the 0x54-byte records at 0x0063bf30 (2).
   All six are byte-identical (--bytes, relocations masked), /O2 /GS /Ob1, plain cl 13.00.9176/9178:
   0x00418a00 (37 B)  ESI = rec. Converter 15.4%. static, memset-like FUN_004b6520(rec, 0x6c) then fields.
   0x00418a30 (50 B)  EAX = rec. Converter 66.7%. The 8-byte copy from DAT_005f32b0[rec->f50] is a struct
                      assignment (int[2] or two int arrays give shl ecx,3 + base reads, not [ecx*8+disp]).
   0x00418a70 (40 B)  EAX = rec. Converter 70.6%. FUN_005592f0 reads 3 stack args (callee reads [esp+0x1c],
                      [esp+0x24] after sub esp,0x18, nothing deeper), not 4. The original keeps a dead
                      4-byte stack copy of rec+0x10 (push ecx / mov [esp],ecx / pop ecx). cl 13.00 drops
                      a plain dead local of any type tried (RGBA, int, float, struct-in-struct, array,
                      pointer-to-it, (void)&col). It survives only when its address is taken somewhere:
                      `if (0) FUN_004b6520((void *)&col, 4)` (stands for a compiled-out use) or `volatile`.
                      Source shape (b): the real use of the copy is unknown.
   0x004190f0 (9 B)   EAX = rec. Not in the list, needed so 0x004194f0 keeps EAX live across the call.
   0x00419090 (96 B)  pilot re/match/pilot/a6_00419090.c reused unchanged, re-checked with /Ob1: 100%.
   0x004194f0 (51 B)  Converter 69.2%. Index loops `for (i...) f(&arr[i])` give the original's jl; pointer
                      loops give jb.
   Register args come from cl's own convention for static, address-not-taken callees defined before
   their caller (no /GL). caller_0041a060 and caller_00419530 are stand-ins for the real callers
   0x0041a060 / 0x00419530 (not matched, only there to give the statics their real call sites).
   Port: Rec6c / Rec54 / RGBA / Pair8 field names are mechanical (offsets), no semantics claimed. */
typedef struct Pair8 { int a; int b; } Pair8;
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
typedef struct Rec6c {
  Pair8 p0;
  float f8;
  float fc;
  RGBA c10;
  float v14[2];
  float v1c[6];
  int f34;
  int f38;
  int f3c;
  int f40;
  int f44;
  int f48;
  int f4c;
  int f50;
  char pad54[0x10];
  int f64;
  int f68;
} Rec6c;
typedef struct Rec54 {
  char pad[0x20];
  int  slot[8];
  int  obj;
  int  obj4;
  int  idx;
  int  f4c;
  int  f50;
} Rec54;
#ifndef MATCH_PORT
extern Pair8 DAT_005f32b0[];
extern Rec6c DAT_0063c018[4];
extern Rec54 DAT_0063bf30[2];
extern int  *DAT_007d3ff8;
extern char  DAT_0063bda0[];
void FUN_00418e30(void);
void FUN_004b6520(void *p, int n);
void FUN_005592f0(int a, float *b, float *c);
void FUN_004b3fc0(int obj, int *out);
int  FUN_004b5190(int a, int b, int c);
void FUN_004b52c0(int a, unsigned int b, int c);
int  FUN_0042a5d0(const char *s, int a, int b);
void FUN_00418e50(void);
#endif

static void FUN_00418a00(Rec6c *r)
{
  FUN_004b6520((void *)r, 0x6c);
  r->f8 = 0.05f;
  r->fc = 0.05f;
  r->c10.r = 0xff;
  r->c10.g = 0xff;
  r->c10.b = 0xff;
  r->c10.a = 0xff;
}

static void FUN_00418a30(Rec6c *r)
{
  r->p0 = DAT_005f32b0[r->f50];
  r->f34 = 0;
  r->f64 = -1;
  r->f38 = 0;
  r->f3c = 0;
  r->f40 = 0;
  r->f44 = 0;
  r->f4c = 0;
}

static void FUN_00418a70(Rec6c *r)
{
  RGBA col;

  col = r->c10;
  if (0) FUN_004b6520((void *)&col, 4);
  if (r->f34 == 2)
    FUN_005592f0(*DAT_007d3ff8, r->v1c, r->v14);
}

static void FUN_00419090(Rec54 *rec, int obj, int idx)
{
  int list[6];
  int i;
  FUN_004b3fc0(obj, list);
  rec->idx = idx;
  rec->obj = obj;
  rec->obj4 = *(int *)(obj + 4);
  FUN_004b6520((void *)rec->slot, 0x20);
  i = 0;
  do {
    int a = list[i];
    rec->slot[FUN_004b5190(a, 0, 0)] = a;
    FUN_004b52c0(*(int *)(a + 0x18), 0x40, 1);
    i++;
  } while (i < 6);
}

static void FUN_004190f0(Rec54 *r)
{
  r->f4c = 0;
  r->f50 = 0;
}

void FUN_004194f0(void)
{
  int i;

  for (i = 0; i < 4; i++)
    FUN_00418a30(&DAT_0063c018[i]);
  FUN_00418e30();
  FUN_00418e50();
  for (i = 0; i < 2; i++)
    FUN_004190f0(&DAT_0063bf30[i]);
}

/* stand-in for the 0x0041a060 loops (not matched) */
void caller_0041a060(void)
{
  int i;
  Rec6c *r;
  char *e;
  for (i = 0; i < 2; i++)
    FUN_00419090(&DAT_0063bf30[i], FUN_0042a5d0("x", 0, 0), i);
  for (i = 0, r = DAT_0063c018, e = DAT_0063bda0; r < DAT_0063c018 + 4; r++, i++, e += 0x64) {
    FUN_00418a00(r);
    FUN_00418a30(r);
    r->f50 = i;
    r->f68 = (int)e;
  }
}

/* stand-in for the 0x00419530 loop (not matched) */
void caller_00419530(void)
{
  Rec6c *r;
  for (r = DAT_0063c018; r < DAT_0063c018 + 4; r++)
    if (r->f34 != 0)
      FUN_00418a70(r);
}
