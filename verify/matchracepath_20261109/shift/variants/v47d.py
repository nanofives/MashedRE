O = "  v.x *= DAT_005cc9dc;\n  v.y *= DAT_005cc9dc;\n  v.z *= DAT_005cc9dc;\n"
def s(fmt): return [(O, "".join(fmt.format(c=c) for c in "xyz"))]
V = [
 ("a", s("  v.{c} = (0, v.{c}) * DAT_005cc9dc;\n")),
 ("b", s("  v.{c} = v.{c} * DAT_005cc9dc;\n")),
 ("e", s("  v.{c} *= (0, DAT_005cc9dc);\n")),
 ("f", s("  v.{c} = DAT_005cc9dc * v.{c};\n")),
 ("vol", [("MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);", "MGLOBAL(volatile float, DAT_005cc9dc, 0x005cc9dc);")]),
 ("g", s("  v.{c} = (float)(v.{c} * (double)DAT_005cc9dc);\n")),
 ("h", s("  v.{c} = (0, v.{c} * DAT_005cc9dc);\n")),
]
