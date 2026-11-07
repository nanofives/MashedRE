// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b03e0 (199 B). Best 85.9% (regs 87.2%), residual class (b). ops 100%: nested list/array search; only stack frame differs (orig frame 0xc, spills the param-1 flag into param_2's own slot [esp+0x24]; ours 0x10). permuter swapcmp variant adopted. */
/* MATCH-S26 0x005b03e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs26_20261107/results.tsv. */
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
int FUN_005adf30(int p0, int p1);
// MATCH: 0x005b03e0 FUN_005b03e0
int FUN_005b03e0(int *param_1,unsigned int *param_2)
{
  int *n;
  for (n = (int *)param_1[1]; n != param_1; n = (int *)n[1]) {
    int obj = n[2];
    int *arr = *(int **)(obj + 0x54);
    unsigned int i;
    for (i = 0; i < (unsigned int)arr[2]; i++) {
      unsigned int *e = *(unsigned int **)(arr[1] + i * 4);
      unsigned int ef = *(unsigned char *)&e[4] & 1;
      unsigned int pf = *(unsigned char *)&param_2[6] & 1;
      if (FUN_005adf30(param_2[1], e[0]) == 0
          && *(char *)&param_2[3] == *(char *)&e[1]
          && pf == ef
          && *(char *)((int)param_2 + 0xd) == *(char *)((int)e + 5)
          && param_2[0] <= e[3]
          && param_2[0] >= e[2]) {
        return obj;
      }
    }
  }
  return 0;
}
