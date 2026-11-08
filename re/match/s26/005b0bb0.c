// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b0bb0 (h3): 58.8% exact, regs 80.0%. Residual class (b) typed struct form (58.8%): original keeps param_3 in ebp and zero-extends bytes through ebx; ours differs in allocation. Ghidra form was 39.5%. */
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
typedef unsigned char byte;
typedef struct Tn5b { char pad[0xd]; byte n; } Tn5b;
typedef struct En5b { char pad[0x14]; Tn5b *t; char pad2[0x10]; } En5b;
typedef struct Ob5b { char pad[0x14]; uint count; En5b *e; } Ob5b;
// MATCH: 0x005b0bb0 FUN_005b0bb0
int FUN_005b0bb0(uint *param_1,uint param_2,Ob5b *param_3)
{
  uint cnt;
  uint total;
  uint i;
  uint j;
  uint k;
  uint idx;
  uint *p;
  uint left;
  En5b *e;

  total = 0;
  cnt = param_3->count;
  if (cnt > 0) {
    e = param_3->e;
    left = cnt;
    do {
      total += e->t->n;
      e++;
      left--;
    } while (left != 0);
  }
  if (total * 4 > param_2) {
    return 0;
  }
  i = 0;
  idx = 0;
  if (i < cnt) {
    do {
      j = 0;
      if (param_3->e[i].t->n > 0) {
        p = param_1 + idx * 16 + 2;
        do {
          k = 0;
          do {
            p[0] = k;
            p[-2] = i;
            p[-1] = j;
            p[1] = 0x3f800000;
            k++;
            p += 4;
          } while (k < 4);
          idx++;
          j++;
        } while (j < param_3->e[i].t->n);
      }
      i++;
    } while (i < param_3->count);
  }
  return idx * 4;
}
