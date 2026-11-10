import re
src = open("re/match/s11/00469df0.c").read()
def nn(t, pairs):
    for a, b, nm in pairs:
        t = re.sub(rf"\b{a}\b", f"{nm}[0]", t); t = re.sub(rf"\b{b}\b", f"{nm}[1]", t)
    return t
D = "    V3_00469df0 n0, n1, n2, nn;\n"
V = [("n01", [(src, nn(src.replace(D, "    V3_00469df0 n2, nn;\n    V3_00469df0 n01[2];\n"), [("n0","n1","n01")]))]),
     ("n23", [(src, nn(src.replace(D, "    V3_00469df0 n0, n1, nn;\n    V3_00469df0 n23[2];\n").replace("    V3_00469df0 n3;\n",""), [("n3","n2","n23")]))])]
