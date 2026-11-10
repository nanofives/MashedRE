import re
src = open("re/match/s11/00469df0.c").read()
def narr(t, m):
    t = t.replace("    V3_00469df0 n3;\n", "")
    t = t.replace("    V3_00469df0 n0, n1, n2, nn;\n", "    V3_00469df0 N[4];\n")
    t = re.sub(r"\bn([0-3])\b", lambda mm: f"N[{m[int(mm.group(1))]}]", t)
    return t
V = [("narr", [(src, narr(src, {0:0,1:1,2:3,3:2}))]), ("narr2", [(src, narr(src, {0:0,1:1,2:2,3:3}))])]
