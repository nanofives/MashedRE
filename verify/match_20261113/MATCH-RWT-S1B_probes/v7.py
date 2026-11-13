exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "idx")}
X = ["", "/Op", "/Oa", "/Ow", "/GX", "/GR", "/Zi", "/Z7", "/Gh", "/Ge", "/Gf", "/GF", "/Gi", "/QIfdiv", "/QI0f",
     "/QIfist", "/J", "/Za", "/Oi-", "/Ob0", "/Ob2", "/Gm", "/GT", "/Gs", "/GA", "/GD", "/Gy-", "/Zp1", "/Zp2",
     "/Zl", "/TP", "/d2nodlp", "/d2loopopt", "/d2datasort", "/d2bzalign", "/Gs0", "/Ox"]
COMBOS = [(tc, ("/O2 /Ob1 /G5 /Oy- " + x).strip()) for tc in ("rw9044", "ddk") for x in X]
