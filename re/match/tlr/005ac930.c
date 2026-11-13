// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* TL-MATCH-REST 0x005ac930 (no hooks.csv row before this slice): walks the list at param_2+0x20; records hold their link at +0x40. */
/* ---- MATCH-TLR preamble (same text in every re/match/tlr source; not part of any function body) ----
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
typedef struct Node_005ac930 { struct Node_005ac930 *next; } Node_005ac930;
typedef struct Rec_005ac930 { int key; int f[15]; Node_005ac930 link; } Rec_005ac930;
typedef struct Own_005ac930 { int pad[8]; Node_005ac930 head; } Own_005ac930;
typedef struct Q_005ac930 { int key; Rec_005ac930 *found; } Q_005ac930;
int FUN_005adf30(int a, int b);
// MATCH: 0x005ac930 FUN_005ac930
int FUN_005ac930(int param_1, Own_005ac930 *param_2, Q_005ac930 *q)
{
  Node_005ac930 *n;
  for (n = param_2->head.next; n != &param_2->head; n = n->next) {
    Rec_005ac930 *r = (Rec_005ac930 *)((char *)n - 0x40);
    if (FUN_005adf30(r->key, q->key) == 0) {
      q->found = r;
      return 1;
    }
  }
  return 0;
}
