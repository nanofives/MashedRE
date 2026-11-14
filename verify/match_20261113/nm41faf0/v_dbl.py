import itertools
T = {"x": "fr->m.at.x * b.at.x", "y": "fr->m.at.y * b.at.y", "z": "fr->m.at.z * b.at.z"}
D = "dot = fr->m.at.x * b.at.x + fr->m.at.y * b.at.y + fr->m.at.z * b.at.z;"
VARIANTS = []
for p in itertools.permutations("xyz"):
    a, b, c = (T[k] for k in p)
    n = "".join(p)
    VARIANTS.append((f"L{n}", [(D, f"dot = (float)((double){a} + (double){b} + (double){c});")]))
    VARIANTS.append((f"R{n}", [(D, f"dot = (float)((double){a} + ((double){b} + (double){c}));")]))
    VARIANTS.append((f"P{n}", [(D, f"dot = (float)((double)({a}) + (double)({b}) + (double)({c}));")]))
    VARIANTS.append((f"Q{n}", [(D, f"dot = (float)(((double)({a}) + ({b})) + ({c}));")]))
