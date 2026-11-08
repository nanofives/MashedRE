// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0052df70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int undefined4;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(float, DAT_005d757c, 0x005d757c);
typedef struct Node { struct Node *next; float t; } Node;
typedef struct Src { int u0, u1, u2; float step; int u4; int hold; } Src;
typedef struct Ev Ev;
typedef int (*EvCb)(Ev *, int);
typedef void (*RunCb)(void *, Node *, Node *, float, int);
struct Ev {
  Src *src;
  float t;
  int flag;
  EvCb cb3;
  int arg4;
  float limit;
  EvCb cb6;
  int arg7;
  int p8;
  int stride;
  int p10;
  int count;
  int p12[5];
  RunCb run;
  int p18;
};
int FUN_0052de60(Ev *p0, Src *p1);
int FUN_0052e0e0(Ev *p0, float p1);
// MATCH: 0x0052df70 FUN_0052df70
int FUN_0052df70(Ev *e, float param_2)
{
  float fVar1;
  int bVar3;
  int bVar4;
  int iVar5;
  int *piVar6;
  Node *nd;

  bVar4 = 0;
  bVar3 = (e->t > e->limit && e->t - param_2 <= e->limit);
  fVar1 = e->t;
  e->t = fVar1 - param_2;
  if (fVar1 - param_2 >= DAT_005d757c) {
    piVar6 = (int *)e + 0x13;
    for (iVar5 = e->count; iVar5 != 0; iVar5 = iVar5 + -1) {
      if (e->t < ((Node *)*piVar6)->t) {
        do {
          piVar6[1] = *piVar6;
          nd = ((Node *)*piVar6)->next;
          *piVar6 = (int)nd;
        } while (e->t < nd->t);
      }
      e->run(piVar6,(Node *)*piVar6,(Node *)piVar6[1],e->t,e->src->hold);
      piVar6 = (int *)((int)piVar6 + e->stride);
    }
    e->flag = 0;
  }
  else {
    fVar1 = e->t;
    do {
      fVar1 = fVar1 + e->src->step;
    } while (fVar1 < DAT_005d757c);
    e->t = fVar1;
    FUN_0052de60(e,e->src);
    FUN_0052e0e0(e,fVar1);
    if (e->limit <= e->t) {
      bVar4 = 1;
    }
    else {
      bVar3 = 1;
      bVar4 = 1;
    }
  }
  if (bVar3 && e->cb3 != 0 && (iVar5 = e->cb3(e,e->arg4), iVar5 == 0)) {
    e->cb3 = 0;
  }
  if (bVar4 && e->cb6 != 0) {
    iVar5 = e->cb6(e,e->arg7);
    if (iVar5 == 0) {
      e->cb6 = 0;
      return 1;
    }
  }
  return 1;
}
