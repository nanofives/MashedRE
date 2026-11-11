// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f4270 (h3, by hand): 93.5% (regs 93.5%) under /O2 /Ob1 /G5. Raw converter 54.8%.
   Linked-list unlink + RwFreeListFree through RwGlobals+0x11c. A cached `head` local fixed the register
   assignment (77.4% -> 93.5%). Residual: `push esi` is scheduled after the empty-list `je` instead of before it.
   Tried: while/break/return forms, Ghidra's nested-if form, for-loop, split declarations, void* key, permuter. */
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
typedef struct S22Node { int key; struct S22Node *next; } S22Node;
typedef void (*S22FreeFn)(void *fl, void *p);
MGLOBAL(S22Node *, DAT_007d7298, 0x007d7298);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(void *, DAT_007d729c, 0x007d729c);
// MATCH: 0x004f4270 FUN_004f4270
void FUN_004f4270(int key)
{
  S22Node *head = DAT_007d7298;
  S22Node *prev = 0;
  S22Node *cur = head;
  while (cur) {
    if (cur->key == key) {
      if (cur == head) DAT_007d7298 = cur->next;
      else prev->next = cur->next;
      cur->key = 0;
      (*(S22FreeFn *)(DAT_007d3ff8 + 0x11c))(DAT_007d729c, cur);
      return;
    }
    prev = cur;
    cur = cur->next;
  }
}
