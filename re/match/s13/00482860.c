// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand: ring struct with 0x24-byte nodes (next at +0x1c), memset intrinsic of (count+2) nodes.
/* MATCH-S13 0x00482860: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct Node { char pad[0x1c]; struct Node *next; char pad2[4]; } Node;
typedef struct {
  char pad[0x14];
  Node *pool;   /* +0x14 */
  Node *head;   /* +0x18 */
  Node *cur;    /* +0x1c */
  Node *tail;   /* +0x20 */
  char pad2[0x168 - 0x24];
  int f168;
  int count;    /* +0x16c */
  int f170;
  char pad3[0x194 - 0x174];
  int f194;
  int f198;
} Ring;
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x00482860 FUN_00482860
void FUN_00482860(Ring *param_1)
{
  int i;

  param_1->head = param_1->pool;
  memset(param_1->pool, 0, (param_1->count + 2) * sizeof(Node));
  param_1->cur = param_1->head;
  param_1->cur->next = param_1->cur + 1;
  for (i = 0; i < param_1->count; i++) {
    param_1->cur->next = param_1->cur + 1;
    param_1->cur = param_1->cur->next;
  }
  param_1->tail = param_1->cur;
  param_1->cur->next = param_1->head;
  param_1->f168 = 0;
  param_1->f170 = 0;
  param_1->f194 = 0;
  param_1->f198 = 1;
  param_1->cur = param_1->head;
}
