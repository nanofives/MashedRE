exec(open("v1.py").read().split("LOOPS = {")[0])
PRE = r'''static __inline int MatEqA(const int *a, const int *b)
{
  int n = 16;
  do { if (*a != *b) return 0; a++; b++; } while (--n);
  return 1;
}
static __inline int MatEqB(const int *a, const int *b)
{
  int i;
  for (i = 0; i < 16; i++) if (a[i] != b[i]) return 0;
  return 1;
}
static __inline int MatNeA(const int *a, const int *b)
{
  int n = 16;
  do { if (*a != *b) return 1; a++; b++; } while (--n);
  return 0;
}
'''
LOOPS = {
 "inA_qc": "      if (MatEqA(cache, DAT_00618258.m)) return 1;",
 "inA_cq": "      if (MatEqA(DAT_00618258.m, cache)) return 1;",
 "inB_qc": "      if (MatEqB(cache, DAT_00618258.m)) return 1;",
 "inB_cq": "      if (MatEqB(DAT_00618258.m, cache)) return 1;",
 "inN_qc": "      if (!MatNeA(cache, DAT_00618258.m)) return 1;",
 "inN_cq": "      if (!MatNeA(DAT_00618258.m, cache)) return 1;",
}
VARIANTS = {k: T.replace("@@LOOP@@", v) for k, v in LOOPS.items()}
COMBOS = [(tc, "/O2 /Ob1 /G5 /Oy-") for tc in ("ddk", "rw9044")]
