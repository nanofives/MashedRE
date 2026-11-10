import itertools, os
blk = int(os.environ.get("BLK", "1"))
B = {1: ["  v.y = m->up.y;\n", "  v.z = m->up.z;\n", "  v.x = 0.0f;\n"],
     2: ["  v.x = m->at.x;\n", "  v.z = m->at.z;\n", "  v.y = 0.0f;\n"],
     3: ["  v.x = m->right.x;\n", "  v.y = m->right.y;\n", "  v.z = 0.0f;\n"]}
S = B[blk]
V = []
for perm in itertools.permutations(range(3)):
    if perm == (0, 1, 2): continue
    V.append((f"B{blk}_" + "".join(map(str, perm)), [("".join(S), "".join(S[i] for i in perm))]))
# combined declaration-style forms
if blk == 1:
    V.append(("B1_comma", [("".join(S), "  v.y = m->up.y, v.z = m->up.z, v.x = 0.0f;\n")]))
    V.append(("B1_chain", [("".join(S), "  v.y = m->up.y;\n  v.x = 0.0f, v.z = m->up.z;\n")]))
