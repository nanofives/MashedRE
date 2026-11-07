// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8c40 (h3): best 69.4% (regs 80.6%). Raw 27.0%. Two-list splice (lists at RwGlobals+[0x00911ad8]+0x20/+0x24)
   with a link struct; the permuter swapped the last two stores of the else branch (equivalent) for 27 -> 69.4.
   Residual: ours loads the two globals into esi/edi (extra push edi) where the original uses edx/eax with no
   push, and the branch-1 tail is cross-jumped with the else branch in ours. Tried load order, a struct view of
   the globals block, no-temp branch forms, permuter 240 s (286 compiles). Class (b): source shape not found. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct Lnk8c40 { struct Lnk8c40 *next; struct Lnk8c40 *prev; } Lnk8c40;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ad8, 0x00911ad8);
// MATCH: 0x004d8c40 FUN_004d8c40
void FUN_004d8c40(void)
{
  Lnk8c40 *a;
  Lnk8c40 *b;
  Lnk8c40 *n;
  Lnk8c40 *p;
  a = *(Lnk8c40 **)(DAT_007d3ff8 + DAT_00911ad8 + 0x24);
  b = *(Lnk8c40 **)(DAT_007d3ff8 + DAT_00911ad8 + 0x20);
  n = b->next;
  if (n != b) {
    if (a->next == a) {
      a->next = n;
      n->prev = a;
      n = b->prev;
      a->prev = n;
      n->next = a;
    } else {
      p = a->prev;
      p->next = n;
      n->prev = p;
      a->prev = n;
      n = b->prev;
      n->next = a;
    }
    b->next = b;
    b->prev = b;
  }
  *(Lnk8c40 **)(DAT_007d3ff8 + DAT_00911ad8 + 0x24) = b;
  *(Lnk8c40 **)(DAT_007d3ff8 + DAT_00911ad8 + 0x20) = a;
  *(int *)(DAT_007d3ff8 + DAT_00911ad8 + 8) = 0;
}
