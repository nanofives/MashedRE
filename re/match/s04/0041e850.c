// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041e850 (22 B): 100.0% (regs 100.0%), by hand. Raw g2s was 37.5%.
   This file also holds 0x0041e630 (134 B): 100.0% (regs 100.0%), raw g2s 56.2%.
   0x0041e630 takes the record at 0x0063d610 in EDI (Ghidra unaff_EDI): it is a `static` function defined before
   its only caller 0x0041e850, which does `mov edi, 0x0063d610`. That is cl 13.00.9176's static-callee register
   convention (pilot finding 1); with a stack argument 0x0041e850 pushes it instead.
   0x0041e630: needed a row pointer stepped by 11 entries (0x2c) beside the counter, both in the for header
   (`i++, row += 11` gives the original inc-before-add order and xor esi before mov ebx,edi), and the digit picked
   with an if/else (`i % 2 == 0` -> v / 10, else v % 10). Calls are RpAtomicRender-style `a->renderCallBack(a)`
   through +0x48. Struct offsets come from the disassembly: rows of 11 atomics at +0x000 (6 rows), +0x16c/+0x170/
   +0x174 atomics, int[3] at +0x188, flag at +0x1d0 (0x0063d7e0).
   Residual: none. Same record layout as re/match/s04/0041e6c0.c. */
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
typedef struct RpMaterial { void *texture; unsigned int color; } RpMaterial;
typedef struct RpGeometry { char pad0[0x20]; RpMaterial **materials; } RpGeometry;
typedef struct RpAtomic RpAtomic;
typedef RpAtomic *(*RpAtomicCallBackRender)(RpAtomic *);
struct RpAtomic { char pad0[0x18]; RpGeometry *geometry; char pad1c[0x48 - 0x1c]; RpAtomicCallBackRender renderCallBack; };
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct RwFrame RwFrame;
typedef struct Mgr {
  RpAtomic *digit[6][11];
  RpAtomic *a108[10];
  char pad130[0x16c - 0x130];
  RpAtomic *a16c;
  RpAtomic *a170;
  RpAtomic *a174;
  RpAtomic *a178;
  char pad17c[0x188 - 0x17c];
  int cur[3];
  int t194[3];
  char pad1a0[0x1b0 - 0x1a0];
  RwFrame *frame;
  int state;
  int laststate;
  int n1bc;
  int n1c0;
  int n1c4;
  int n1c8;
  int n1cc;
  int n1d0;
} Mgr;
MGLOBAL(Mgr, DAT_0063d610, 0x0063d610);

// MATCH: 0x0041e630 FUN_0041e630
static void FUN_0041e630(Mgr *m)
{
  int i;
  int v;
  RpAtomic **row;
  RpAtomic *a;
  m->a16c->renderCallBack(m->a16c);
  m->a170->renderCallBack(m->a170);
  m->a174->renderCallBack(m->a174);
  for (i = 0, row = m->digit[0]; i < 6; i++, row += 11) {
    v = m->cur[i / 2];
    if (v == -1) {
      a = row[10];
      a->renderCallBack(a);
    } else {
      if (i % 2 == 0)
        v = v / 10;
      else
        v = v % 10;
      a = row[v];
      a->renderCallBack(a);
    }
  }
}

// MATCH: 0x0041e850 FUN_0041e850
void FUN_0041e850(void)
{
  if (DAT_0063d610.n1d0 != 0)
    FUN_0041e630(&DAT_0063d610);
}
