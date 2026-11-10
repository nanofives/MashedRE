def mk(decl, extra_at=None, extra=""):
    def f(t):
        i = t.index("// MATCH: 0x00416250"); a, b = t[:i], t[i:]
        b = b.replace("  volatile float steer;\n", decl)
        if extra_at: b = b.replace(extra_at, extra + extra_at, 1)
        return a + b
    return f
CALL = "  s = steer = FUN_00415e20"
V = [("void1", mk("  float steer;\n", CALL, "  (void)&steer;\n")),
     ("void0", mk("  float steer;\n", "  mode = FUN_0040e350();", "  (void)&steer;\n")),
     ("ptr", mk("  float steer;\n  float *ps;\n", CALL, "  ps = &steer;\n")),
     ("nov", mk("  float steer;\n")),
]
