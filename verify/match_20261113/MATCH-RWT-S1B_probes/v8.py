# S1B attempt 2: c2 switches that take an argument, passed through /d2 (cl hands c2 "-<rest>").
exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "idx")}
X = [""]
X += [f"/d2off{n}" for n in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 24, 28, 32, 64)]
X += [f"/d2Loop{n}" for n in (0, 1, 2, 3, 4, 8)]
X += [f"/d2vol{n}" for n in (0, 1, 2, 3)]
X += [f"/d2db{n}" for n in (0, 1, 2, 3)]
X += [f"/d2ef{n}" for n in (0, 1, 2)]
X += ["/d2noehopt", "/d2nolock", "/d2optref", "/d2NoLUR", "/d2Zvc6", "/d2QIWMTemu", "/d2QIftol2", "/d2cef"]
COMBOS = [("ddk", ("/O2 /Ob1 /G5 /Oy- " + x).strip()) for x in X]
