# S1B attempt 2: VC6 c1 switch table (strings in tools/vc6pp/bin/c1.dll) passed through /d1, rw9044 only.
exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "dn2", "idx")}
X = ["", "/d1Ov", "/d1Oq", "/d1Ow", "/d1Oa", "/d1Bdlp", "/d1BnoWorkList", "/d1noexpinl", "/d1GM", "/d1int2enum",
     "/d1GI", "/d1Gi", "/d1Ov /d1Oq", "/d1Oy"]
COMBOS = [("rw9044", ("/O2 /Ob1 /G5 /Oy- " + x).strip()) for x in X]
