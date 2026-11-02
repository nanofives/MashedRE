// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044e190: this file holds four static functions of one TU, all register-passed (pilot finding 1):
   0x0044e190 (EBX = rec, handles on the stack) 73.2%, regs 98.2% (raw 44.4%)
   0x0044e240 (ESI = clump, EDI = b, a on the stack) 100%  (raw 17.5%)
   0x0044e2d0 (EAX = handle array, EBX = rec) 81.0%, regs 95.2% (raw 54.5%)
   0x0044e310 (EAX = handle, ESI = rec) 100% (raw 45.5%)
   Their only caller is FUN_0044f970 (0x004500d2/0x004500dc/0x004500e8, absent from pc_funcs.csv);
   caller_0044f970 is a stand-in for it (not matched). rec is the 0xf8-byte record of the 0x00890080 array.
   Residual (b) in 0x0044e190 and 0x0044e2d0: register allocation only (0x0044e2d0: ours loads rec->clump0 into
   ecx before push eax, the original reuses eax after the push; 0x0044e190: handles[i] in edx vs eax and the
   following registers rotate). Tried: a temp for the FUN_004c0b30 result, int/char* prototypes, K&R
   prototypes, sub-record pointer vs indexed form, the permuter on both (no gain). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { float x, y, z; } V3_0044e190;
typedef struct { V3_0044e190 pos; char *clump; char pad10[8]; } Sub_0044e190;
typedef struct {
  Sub_0044e190 sub[4];  /* 0x00 */
  char pad60[4];
  int count;            /* 0x64 */
  char pad68[0x38];
  char *clump0;         /* 0xa0 */
  char *clump1;         /* 0xa4 */
  int type;             /* 0xa8 */
  char padac[0x4c];
} Rec_0044e190;
MGLOBALA(int, DAT_008952b0, 4, 0x008952b0);
typedef V3_0044e190 Row_0044e190[4];
MGLOBALA(Row_0044e190, DAT_00895560, 4, 0x00895560);
int FUN_004c0b30(void);
int FUN_004c1040(int p0, int p1);
int FUN_004c1340(int p0, V3_0044e190 *p1, int p2);
char *FUN_004e69a0(int p0);
int FUN_004e7e30(char *p0, int p1);
int FUN_00557ec0(char *p0, int p1, int p2);
int FUN_00558030(char *p0, float p1);

// MATCH: 0x0044e190 FUN_0044e190
static void FUN_0044e190(Rec_0044e190 *rec, int *handles)
{
  int i;
  int frame;
  Sub_0044e190 *s;

  rec->count = DAT_008952b0[rec->type];
  for (i = 0; i < rec->count; i++) {
    s = &rec->sub[i];
    s->pos = DAT_00895560[rec->type][i];
    s->clump = FUN_004e69a0(handles[i]);
    FUN_004e7e30(s->clump, FUN_004c0b30());
    frame = *(int *)(s->clump + 4);
    FUN_004c1040(*(int *)(rec->clump0 + 4), frame);
    FUN_004c1340(frame, &DAT_00895560[rec->type][i], 0);
  }
}

// MATCH: 0x0044e240 FUN_0044e240
static void FUN_0044e240(char *c, char *a, char *b)
{
  FUN_00557ec0(c, 0, *(int *)(c + 0x18));
  FUN_00557ec0(c, 1, *(int *)(a + 0x18));
  FUN_00557ec0(c, 2, *(int *)(b + 0x18));
  FUN_00557ec0(c, 3, *(int *)(b + 0x18));
  FUN_00557ec0(c, 4, *(int *)(b + 0x18));
  FUN_00557ec0(c, 5, *(int *)(b + 0x18));
  FUN_00557ec0(c, 6, *(int *)(b + 0x18));
  FUN_00557ec0(c, 7, *(int *)(b + 0x18));
  FUN_00557ec0(c, 8, *(int *)(b + 0x18));
  FUN_00557ec0(c, 9, *(int *)(b + 0x18));
  FUN_00558030(c, 200.0f);
}

// MATCH: 0x0044e2d0 FUN_0044e2d0
static void FUN_0044e2d0(Rec_0044e190 *rec, char **h)
{
  rec->clump0 = FUN_004e69a0((int)h[0]);
  FUN_004e7e30(rec->clump0, FUN_004c0b30());
  FUN_0044e240(rec->clump0, h[1], h[2]);
}

// MATCH: 0x0044e310 FUN_0044e310
static void FUN_0044e310(Rec_0044e190 *rec, int h)
{
  int t;

  rec->clump1 = FUN_004e69a0(h);
  t = FUN_004c0b30();
  FUN_004e7e30(rec->clump1, t);
  FUN_004c1040(*(int *)(rec->clump0 + 4), t);
}

/* stand-in for the real caller FUN_0044f970 (0x004500d2..0x004500e8); not matched */
void caller_0044f970(Rec_0044e190 *rec, char **h3, int *h4, int h)
{
  for (; rec->type != 99; rec++) {
    FUN_0044e2d0(rec, h3);
    FUN_0044e190(rec, h4);
    FUN_0044e310(rec, h);
  }
}
