// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b690 (136 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 44.2%.
   Two register args: clump in EAX, record in EBX. `static (clump, rec)`, defined before its only caller
   FUN_0041bec0 (call site 0x0041bef7). Local colour {0x32, 0x32, 0x32, 0xff} is a 4-byte struct, list[29].
   This file also holds the caller FUN_0041bec0 (100%). Its loop needs `e = &DAT_0063cab8[i]` in a local inside
   an index loop (the pointer compare `cmp ebx, 0x63cd90; jl`): a pointer-increment loop gives `cmp esi, 2`
   (92.3%), indexing `DAT_0063cab8[i].idx` directly gives a different IV (58.6%).
   Record (stride 0x16c at 0x0063cab8): +0xb0 atom[40], +0x150 float[3], +0x15c clump, +0x160 clump->+4,
   +0x164 idx, +0x168 mask. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { unsigned char r, g, b, a; } Rgba_0041b690;
typedef struct { int pad0; int frame; } Clump_0041b690;
typedef struct { char pad[0xb0]; int atom[40]; float col[3]; Clump_0041b690 *clump; int frame; int idx; unsigned int mask; } Emit0041b690;   /* stride 0x16c */
MGLOBALA(Emit0041b690, DAT_0063cab8, 2, 0x0063cab8);
MGLOBALA(char, s_team0panel_005cd1c8, 11, 0x005cd1c8);
MGLOBALA(char, s_team1panel_005cd1bc, 11, 0x005cd1bc);
void FUN_004b3fc0(Clump_0041b690 *c, int *out);
void FUN_004b6520(void *p, int n);
int FUN_004b5190(int a, int b, int c);
void FUN_004b5260(int atom, Rgba_0041b690 *col);
void FUN_004c5c80(int a);
Clump_0041b690 *FUN_0042a5d0(char *name, int a, int b);

// MATCH: 0x0041b690 FUN_0041b690
static void FUN_0041b690(Clump_0041b690 *c, Emit0041b690 *e)
{
  int i;
  int a;
  Rgba_0041b690 col;
  int list[29];
  col.r = 0x32;
  col.g = 0x32;
  col.b = 0x32;
  col.a = 0xff;
  FUN_004b3fc0(c, list);
  e->clump = c;
  e->frame = c->frame;
  FUN_004b6520(e->atom, 0xa0);
  for (i = 0; i < 29; i++) {
    a = list[i];
    e->atom[FUN_004b5190(a, 0, 0)] = a;
  }
  FUN_004b5260(e->atom[2], &col);
}

// MATCH: 0x0041bec0 FUN_0041bec0
void FUN_0041bec0(void)
{
  int i;
  Emit0041b690 *e;
  char *names[2];
  names[0] = s_team0panel_005cd1c8;
  names[1] = s_team1panel_005cd1bc;
  FUN_004c5c80(0);
  for (i = 0; i < 2; i++) {
    e = &DAT_0063cab8[i];
    FUN_0041b690(FUN_0042a5d0(names[i], 0, 0), e);
    e->idx = i;
  }
}
