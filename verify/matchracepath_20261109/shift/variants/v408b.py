GY = "static __inline float GY(int h, int n) { return FUN_00426d00(h, n)->y; }"
GZ = "static __inline float GZ(int h, int n) { return FUN_00426d00(h, n)->z; }"
ADD = GY + "\n" + GZ + "\nstatic __inline float CY(int h, int n) { return (0, FUN_00426d00(h, n)->y); }\nstatic __inline float CZ(int h, int n) { return (0, FUN_00426d00(h, n)->z); }"
OLD = "    d.y = GY(h, 0) - GY(h, 3);\n    d.z = GZ(h, 0) - GZ(h, 3);"
def v(new): return [(GY + "\n" + GZ, ADD), (OLD, new)]
V = [
 ("c_first", v("    d.y = CY(h, 0) - GY(h, 3);\n    d.z = CZ(h, 0) - GZ(h, 3);")),
 ("c_second", v("    d.y = GY(h, 0) - CY(h, 3);\n    d.z = GZ(h, 0) - CZ(h, 3);")),
 ("c_first_plain", v("    d.y = CY(h, 0) - FUN_00426d00(h, 3)->y;\n    d.z = CZ(h, 0) - FUN_00426d00(h, 3)->z;")),
 ("c_neg", v("    d.y = -CY(h, 3) + CY(h, 0);\n    d.z = -CZ(h, 3) + CZ(h, 0);")),
 ("c_both_paren", v("    d.y = (CY(h, 0)) - (0, CY(h, 3));\n    d.z = (CZ(h, 0)) - (0, CZ(h, 3));")),
]
