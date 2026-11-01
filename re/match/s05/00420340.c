// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420340 by hand (h2): 100% (regs 100%). Raw g2s 29.7%. The PSlot record (0x0063d9e0, 0x2ac B)
   arrives in ESI (Ghidra unaff_ESI) and FUN_0041f360 keeps ECX live across its call (Ghidra extraout_ECX): both are
   file-`static` functions defined before their caller (pilot finding 1), so the file holds FUN_0041f360 (same body
   as re/match/s04/0041f360.c, 100% here too) and a stand-in caller Caller_00420870 (shaped like the real caller
   0x00420870, call site 0x00420a08; NOT matched, not the real TU). Bytes of 0x00420340 and 0x0041f360 identical. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0x20]; int **p20; } MObj;
typedef struct { char pad[0x18]; MObj *m; } EvObj;
typedef struct {
  EvObj *ev[0x96];    /* +0x000 */
  V3 pos;             /* +0x258 */
  int u264[2];        /* +0x264 */
  char *obj;          /* +0x26c */
  int u270;           /* +0x270 */
  float f274;         /* +0x274 */
  float f278;         /* +0x278 */
  float f27c;         /* +0x27c */
  int u280;           /* +0x280 */
  float f284;         /* +0x284 */
  int u288[2];        /* +0x288 */
  int slot;           /* +0x290 */
  unsigned int flags; /* +0x294 */
  int u298;           /* +0x298 */
  char *car;          /* +0x29c */
  int u2a0[3];        /* +0x2a0 */
} PSlot;
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
MGLOBAL(int, DAT_00636ad0, 0x00636ad0);
void FUN_00420290(int param_1, float param_2);
int FUN_00540080(int *p, float v);
int FUN_00540100(int *p, float v);
int FUN_00540160(int *p, float v);
// MATCH: 0x0041f360 FUN_0041f360
static int FUN_0041f360(int param_1)
{
  if (DAT_00636ad0 == 0) {
    switch (param_1) {
    case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf:
    case 0x25: case 0x26: case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30:
      return 1;
    }
  }
  return 0;
}
// MATCH: 0x00420340 FUN_00420340
static void FUN_00420340(PSlot *s)
{
  unsigned int flags;
  unsigned int b2, b4, b1;
  int i;
  int *p;
  FUN_00420290(s->slot, s->f284);
  flags = s->flags;
  b2 = flags & 2;
  b4 = flags & 4;
  b1 = flags & 1;
  for (i = 0; i < 0x80; i++) {
    if (s->ev[i] != 0 && FUN_0041f360(i) != 0) {
      p = *s->ev[i]->m->p20;
      if (b2) FUN_00540160(p, s->f274);
      if (b4) FUN_00540100(p, s->f27c);
      if (b1) FUN_00540080(p, s->f278);
    }
  }
  if (b2) s->flags &= ~2u;
  if (b4) s->flags &= ~4u;
  if (b1) s->flags &= ~1u;
}
/* stand-in caller shaped like 0x00420870 (not matched here) */
void Caller_00420870(int slot)
{
  PSlot *s = &DAT_0063d9e0[slot];
  FUN_00420340(s);
  s->u298 = slot;
}
