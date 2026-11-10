import itertools
S1 = ["  v.y = m->up.y;\n", "  v.z = m->up.z;\n", "  v.x = 0.0f;\n"]
OLD1 = "".join(S1)
C1 = "  if (!(v.y < 0.0f)) d = -d;\n"
cmps = {"o": C1, "c": "  if (!((0, v.y) < 0.0f)) d = -d;\n", "n": "  if (v.y >= 0.0f) d = -d;\n"}
V = []
for perm in itertools.permutations(range(3)):
    for ck, cv in cmps.items():
        name = "".join(str(i) for i in perm) + ck
        V.append((name, [(OLD1, "".join(S1[i] for i in perm)), (C1, cv)] if ck != "o" else [(OLD1, "".join(S1[i] for i in perm))]))
V = [v for v in V if v[0] != "012o"]
