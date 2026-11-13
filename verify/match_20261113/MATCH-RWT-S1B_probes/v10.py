# S1B attempt 2: /O letters taken apart (favor none / t / s, inline level, intrinsics, /Op), /Og kept, /Oy- /G5.
import itertools
exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "idx")}
COMBOS = []
for tc in ("rw9044", "ddk"):
    for fav, inl, oi, op in itertools.product(("", "/Ot", "/Os"), ("/Ob0", "/Ob1", "/Ob2"), ("", "/Oi"), ("", "/Op")):
        fl = " ".join(x for x in ("/Og", oi, fav, inl, op, "/Gs /GF /G5 /Oy-") if x)
        COMBOS.append((tc, fl))
