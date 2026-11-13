D = "dot = fr->m.at.x * b.at.x + fr->m.at.y * b.at.y + fr->m.at.z * b.at.z;"
X, Y, Z = "fr->m.at.x * b.at.x", "fr->m.at.y * b.at.y", "fr->m.at.z * b.at.z"
VARIANTS = [("base", [])]
for nm, e in [
    ("z+(x+y)", f"{Z} + ({X} + {Y})"),
    ("z+(y+x)", f"{Z} + ({Y} + {X})"),
    ("x+(y+z)", f"{X} + ({Y} + {Z})"),
    ("y+(x+z)", f"{Y} + ({X} + {Z})"),
    ("(y+x)+z", f"({Y} + {X}) + {Z}"),
    ("(x+z)+y", f"({X} + {Z}) + {Y}"),
    ("x+(z+y)", f"{X} + ({Z} + {Y})"),
    ("y+(z+x)", f"{Y} + ({Z} + {X})"),
]:
    VARIANTS.append((nm, [(D, f"dot = {e};")]))
# temps / pointer forms
VARIANTS.append(("ptrs", [(D, "{ V3 *p = &fr->m.at; V3 *q = &b.at; dot = p->x * q->x + p->y * q->y + p->z * q->z; }")]))
VARIANTS.append(("ptrs-zxy", [(D, "{ V3 *p = &fr->m.at; V3 *q = &b.at; dot = p->z * q->z + (p->x * q->x + p->y * q->y); }")]))
VARIANTS.append(("stepwise", [(D, f"dot = {Y}; dot += {X}; dot += {Z};")]))
VARIANTS.append(("stepwise2", [(D, f"dot = {Y} + {X}; dot += {Z};")]))
VARIANTS.append(("dbl", [(D, f"dot = (float)((double){X} + (double){Y} + (double){Z});")]))
VARIANTS.append(("b-first", [(D, "dot = b.at.x * fr->m.at.x + b.at.y * fr->m.at.y + b.at.z * fr->m.at.z;")]))
