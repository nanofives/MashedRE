// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d4bf0 (h4): 100% (regs 100%). Rewritten from odis with Sorter/Graph/Node structs (node 0x28 bytes, edges +8, numEdges +4, counter block +0x1c). Swaps the 32-dword edge arrays and the pointers, the 3-int counter blocks and the pointers, then the whole node by struct assignment. Remaps cur/idx in every edge list, then bumps the counters of the next node's targets and recurses when a == b. What moved the last 19%: the graph pointer reloaded in the for condition (g = s->graph, i < g->numNodes) and a Node* local in the tail loop. Residual: none. */
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
typedef unsigned int uint;
typedef struct Cnt { int a; int b; int c; } Cnt;
typedef struct Node {
  int f0;
  uint numEdges;
  int *edges;
  int pad[4];
  Cnt *cnt;
  int pad2[2];
} Node;
typedef struct Graph { int f0; uint numNodes; Node *nodes; } Graph;
typedef struct Sorter { Graph *graph; int next; } Sorter;
// MATCH: 0x004d4bf0 FUN_004d4bf0
void FUN_004d4bf0(Sorter *s, int idx)
{
  int cur = s->next;
  Node *n;
  uint j;
  if (cur != idx) {
    int *a, *b;
    int k;
    Cnt *x, *y, ct;
    Node t;
    uint i;
    Graph *g;
    a = s->graph->nodes[cur].edges;
    b = s->graph->nodes[idx].edges;
    for (k = 0; k < 32; k++) {
      int tmp = a[k];
      a[k] = b[k];
      b[k] = tmp;
    }
    s->graph->nodes[cur].edges = b;
    s->graph->nodes[idx].edges = a;
    x = s->graph->nodes[cur].cnt;
    y = s->graph->nodes[idx].cnt;
    ct = *x;
    *x = *y;
    *y = ct;
    s->graph->nodes[cur].cnt = y;
    s->graph->nodes[idx].cnt = x;
    t = s->graph->nodes[cur];
    s->graph->nodes[cur] = s->graph->nodes[idx];
    s->graph->nodes[idx] = t;
    for (i = 0; g = s->graph, i < g->numNodes; i++) {
      Node *m = &g->nodes[i];
      for (j = 0; j < m->numEdges; j++) {
        int v = m->edges[j];
        if (v == cur) m->edges[j] = idx;
        else if (v == idx) m->edges[j] = cur;
      }
    }
  }
  n = &s->graph->nodes[s->next++];
  if (n->numEdges) {
    for (j = 0; j < n->numEdges; j++) {
      int e = n->edges[j];
      if (e != -1) {
        Node *m = &s->graph->nodes[e];
        m->cnt->b++;
        if (m->cnt->a == m->cnt->b)
          FUN_004d4bf0(s, e);
      }
    }
  }
}
