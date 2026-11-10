import itertools
O = "  v.x *= DAT_005cc9dc;\n  v.y *= DAT_005cc9dc;\n  v.z *= DAT_005cc9dc;\n"
F = {
 "g": "  v.{c} = (float)(v.{c} * (double)DAT_005cc9dc);\n",
 "o": "  v.{c} *= DAT_005cc9dc;\n",
 "c": "  v.{c} = (float)((0, v.{c}) * (double)DAT_005cc9dc);\n",
 "d": "  v.{c} = (float)((double)v.{c} * DAT_005cc9dc);\n",
}
V = []
for combo in itertools.product("gocd", repeat=3):
    if combo == ("g", "g", "g"): continue
    name = "".join(combo)
    V.append((name, [(O, "".join(F[k].format(c=c) for k, c in zip(combo, "xyz")))]))
