import re
def mk(decl, ref, sline=None):
    def f(t):
        i = t.index("// MATCH: 0x00416250"); a, b = t[:i], t[i:]
        b = b.replace("  volatile float steer;\n", "@@DECL@@")
        b = re.sub(r"\bsteer\b", ref, b)
        b = b.replace("@@DECL@@", decl)
        if sline: b = b.replace(sline[0], sline[1])
        return a + b
    return f
V = [("arr1", mk("  float steer[1];\n", "steer[0]")),
     ("arr2", mk("  float steer[2];\n", "steer[0]")),
     ("st1", mk("  struct { float v; } steer;\n", "steer.v")),
     ("arr1p", mk("  float steer[1];\n", "*steer")),
]
