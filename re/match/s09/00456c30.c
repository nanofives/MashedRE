// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456c30: 100% by hand (h5), relocheck OK. Four 0x48-byte records at 0x006886b0 walked backwards.
   Raw converter 63%. A pointer started at &DAT_006886b0[4] became a down-counter (edi); starting it from the
   next global (DAT_006887d0) keeps the pointer, and comparing the +0x14 field address folds the end test into
   cmp esi, 0x006886c4 as the original has it. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Obj00456c30 {
  char pad[0x48];
  void (*fn)(struct Obj00456c30 *);
} Obj00456c30;
typedef struct Rec00456c30 {
  int pad0[5];
  int active;      /* +0x14 */
  int pad1[5];
  Obj00456c30 *obj; /* +0x2c */
  int pad2[6];
} Rec00456c30;
MGLOBALA(Rec00456c30, DAT_006886b0, 4, 0x006886b0);
MGLOBAL(int, DAT_006887d0, 0x006887d0);
void FUN_004770a0(int *p0);
// MATCH: 0x00456c30 FUN_00456c30
void FUN_00456c30(void)
{
  Rec00456c30 *p;

  FUN_004770a0(&DAT_006887d0);
  p = (Rec00456c30 *)&DAT_006887d0;
  do {
    p--;
    if (p->active != 0) {
      p->obj->fn(p->obj);
      p->active = 0;
    }
  } while (&p->active != &DAT_006886b0[0].active);
}
