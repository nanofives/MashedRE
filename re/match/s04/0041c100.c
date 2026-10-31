// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 h4 0x0041c100 (117 B): 100.0% exact (regs 100, ops 100), BYTE-IDENTICAL, by hand. Raw converter: 68.6%.
   The 0xff323232 store is a 4-byte RGBA local (50,50,50,255) built before the first call and copied as a struct
   into atom[1]->geometry(+0x18)->materials(+0x20)[0]->color(+4). Panel record at 0x0063cda4 (0x1c bytes:
   atom[4], clump, frame, on), clump store before the frame store. Residual: none. */
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
typedef struct { unsigned char r, g, b, a; } Rgba_0041c100;
typedef struct { int pad0; Rgba_0041c100 color; } Material_0041c100;
typedef struct { char pad[0x20]; Material_0041c100 **materials; } Geometry_0041c100;
typedef struct { char pad[0x18]; Geometry_0041c100 *geometry; } Atomic_0041c100;
typedef struct { int pad0; int frame; } Clump_0041c100;
typedef struct { Atomic_0041c100 *atom[4]; Clump_0041c100 *clump; int frame; int on; } Panel_0041c100;   /* 0x1c */
MGLOBAL(Panel_0041c100, DAT_0063cda4, 0x0063cda4);
MGLOBALA(char, s_KCPanel_dff_005cd1d4, 12, 0x005cd1d4);
Clump_0041c100 *FUN_0042a5d0(char *name, int a, int b);
int FUN_004b51d0(Clump_0041c100 *clump, Atomic_0041c100 **out, int n, int a, int b);
void FUN_004b6520(void *p, int n);

// MATCH: 0x0041c100 FUN_0041c100
void FUN_0041c100(void)
{
  Clump_0041c100 *c;
  Rgba_0041c100 col;
  col.r = 0x32;
  col.g = 0x32;
  col.b = 0x32;
  col.a = 0xff;
  c = FUN_0042a5d0(s_KCPanel_dff_005cd1d4, 0, 0);
  FUN_004b6520(&DAT_0063cda4, 0x1c);
  DAT_0063cda4.clump = c;
  DAT_0063cda4.frame = c->frame;
  DAT_0063cda4.on = 1;
  FUN_004b51d0(c, DAT_0063cda4.atom, 4, 0, 0);
  DAT_0063cda4.atom[1]->geometry->materials[0]->color = col;
}
