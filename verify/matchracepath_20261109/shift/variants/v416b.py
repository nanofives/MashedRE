import re
def body(t):
    i = t.index("// MATCH: 0x00416250"); return t[:i], t[i:]
def mk(slot, keep_s=True):
    def f(t):
        a, b = body(t)
        b = b.replace("  volatile float steer;\n", "")
        b = re.sub(r"\bsteer\b", slot, b)
        return a + b
    return f
V = [("tgt0", mk("tgt[0]")), ("tgt1", mk("tgt[1]")), ("pos0", mk("pos[0]")), ("pos1", mk("pos[1]")), ("ang", mk("ang")), ("speedv", mk("speed")) ]
