import re
src = open("re/match/s11/00469df0.c").read()
def arr(t):
    t = t.replace("    V3_00469df0 P0, P1, P2, P3;\n", "    V3_00469df0 P[4];\n")
    t = re.sub(r"\bP([0-3])\b", r"P[\1]", t)
    return t
V = [("parr", [(src, arr(src))])]
