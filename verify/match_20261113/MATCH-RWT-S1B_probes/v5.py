exec(open("v1.py").read().split("LOOPS = {")[0])
PRE = r'''static __inline int s_cmp(const uint *a, const uint *b, uint n)
{
  while (n != 0) {
    if (*a != *b) return 1;
    a++;
    b++;
    n--;
  }
  return 0;
}
'''
LOOPS = {
 "s_qc": "      if (s_cmp((uint *)cache, (uint *)DAT_00618258.m, 16) == 0) return 1;",
 "s_cq": "      if (s_cmp((uint *)DAT_00618258.m, (uint *)cache, 16) == 0) return 1;",
 "s_qc64": "      if (s_cmp((uint *)cache, (uint *)DAT_00618258.m, sizeof(Mat16) >> 2) == 0) return 1;",
 "s_cq64": "      if (s_cmp((uint *)DAT_00618258.m, (uint *)cache, sizeof(Mat16) >> 2) == 0) return 1;",
 "s_qcn": "      if (!s_cmp((uint *)cache, (uint *)DAT_00618258.m, 16)) return 1;",
 "s_cqn": "      if (!s_cmp((uint *)DAT_00618258.m, (uint *)cache, 16)) return 1;",
}
VARIANTS = {k: T.replace("@@LOOP@@", v) for k, v in LOOPS.items()}
COMBOS = [(tc, "/O2 /Ob1 /G5 /Oy-") for tc in ("ddk", "rw9044")]
