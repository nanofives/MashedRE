# S1B attempt 2: CPU targets under the VC6 driver (rw9044 accepts /G3 /G4 /GB) and c2-only -G# through /d2.
exec(open("v1.py").read().split("COMBOS =")[0])
VARIANTS = {k: VARIANTS[k] for k in ("dn", "dn2", "idx")}
COMBOS = []
for tc in ("rw9044", "ddk"):
    for g in ("/G3", "/G4", "/G5", "/G6", "/GB", "/G5 /d2G3", "/G5 /d2G4", "/G5 /d2G6", "/G5 /d2G7", "/G5 /d2G0", "/G5 /d2G1"):
        COMBOS.append((tc, f"/O2 /Ob1 {g} /Oy-"))
