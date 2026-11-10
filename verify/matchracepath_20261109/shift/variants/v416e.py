import re
def mk(decl, helper, extra_at=None, extra="", subs=()):
    def f(t):
        i = t.index("// MATCH: 0x00416250"); a, b = t[:i], t[i:]
        b = b.replace("  volatile float steer;\n", decl)
        if extra_at: b = b.replace(extra_at, extra + extra_at, 1)
        for o, n in subs: b = b.replace(o, n)
        return a + helper + b
    return f
CALL = "  s = steer = FUN_00415e20(car, tgt[0], tgt[1]);\n"
H0 = "static __inline void keep(float *p) { }\n"
H1 = "static __inline float rd(float *p) { return *p; }\n"
H2 = "static __inline void st(float *p, float v) { *p = v; }\n"
V = [("keep_after", mk("  float steer;\n", H0, "  delta = DAT_005d757c;", "  keep(&steer);\n")),
     ("keep_top", mk("  float steer;\n", H0, "  mode = FUN_0040e350();", "  keep(&steer);\n")),
     ("rd", mk("  float steer;\n", H1, subs=[("< steer)", "< rd(&steer))"), ("> steer)", "> rd(&steer))")])),
     ("st", mk("  float steer;\n", H2, subs=[(CALL, "  s = FUN_00415e20(car, tgt[0], tgt[1]);\n  st(&steer, s);\n")])),
     ("st2", mk("  float steer;\n", H2, subs=[(CALL, "  st(&steer, s = FUN_00415e20(car, tgt[0], tgt[1]));\n")])),
]
