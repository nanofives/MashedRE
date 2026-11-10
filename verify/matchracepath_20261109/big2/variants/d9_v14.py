import re
src = open("re/match/s11/00469df0.c").read()
def pair(t, a, b, name):
    t = t.replace(f"{a}, ", "", 1) if f" {a}, " in t else t
    return t
def arr2(names, arrname):
    t = src
    decl = "    V3_00469df0 da, db, ra, ia, ib, ta, tb;\n"
    rest = [n for n in ["da","db","ra","ia","ib","ta","tb"] if n not in names]
    t = t.replace(decl, "    V3_00469df0 " + ", ".join(rest) + ";\n    V3_00469df0 " + arrname + "[%d];\n" % len(names))
    for i, n in enumerate(names):
        t = re.sub(rf"\b{n}\b", f"{arrname}[{i}]", t)
    return t
V = [("tarr", [(src, arr2(["ta","tb"], "tt"))]), ("iarr", [(src, arr2(["ia","ib"], "ii"))]), ("darr", [(src, arr2(["da","db"], "dd"))]), ("rarr", [(src, arr2(["ra"], "rr"))])]
