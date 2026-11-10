import itertools, os
blk = int(os.environ.get("BLK", "2"))
B = {2: ["  v.x = m->at.x;\n", "  a[0] = (float)d;\n", "  v.z = m->at.z;\n", "  v.y = 0.0f;\n"],
     3: ["  v.x = m->right.x;\n", "  a[1] = (float)d;\n", "  v.y = m->right.y;\n", "  v.z = 0.0f;\n"],
     1: ["  v.y = m->up.y;\n", "  v.z = m->up.z;\n", "  v.x = 0.0f;\n"]}
S = B[blk]
V = []
for perm in itertools.permutations(range(len(S))):
    if perm == tuple(range(len(S))): continue
    V.append((f"B{blk}_" + "".join(map(str, perm)), [("".join(S), "".join(S[i] for i in perm))]))
