GY = "static __inline float GY(int h, int n) { return FUN_00426d00(h, n)->y; }"
GZ = "static __inline float GZ(int h, int n) { return FUN_00426d00(h, n)->z; }"
def both(fy, fz): return [(GY, fy), (GZ, fz)]
V = [
 ("comma", both("static __inline float GY(int h, int n) { return (0, FUN_00426d00(h, n)->y); }", "static __inline float GZ(int h, int n) { return (0, FUN_00426d00(h, n)->z); }")),
 ("dbl", both("static __inline double GY(int h, int n) { return FUN_00426d00(h, n)->y; }", "static __inline double GZ(int h, int n) { return FUN_00426d00(h, n)->z; }")),
 ("fdbl", both("static __inline float GY(int h, int n) { return (float)(double)FUN_00426d00(h, n)->y; }", "static __inline float GZ(int h, int n) { return (float)(double)FUN_00426d00(h, n)->z; }")),
 ("plaincomma", [("    d.y = GY(h, 0) - GY(h, 3);\n    d.z = GZ(h, 0) - GZ(h, 3);", "    d.y = (0, FUN_00426d00(h, 0)->y) - FUN_00426d00(h, 3)->y;\n    d.z = (0, FUN_00426d00(h, 0)->z) - FUN_00426d00(h, 3)->z;")]),
 ("plaindbl", [("    d.y = GY(h, 0) - GY(h, 3);\n    d.z = GZ(h, 0) - GZ(h, 3);", "    d.y = (float)((double)FUN_00426d00(h, 0)->y - FUN_00426d00(h, 3)->y);\n    d.z = (float)((double)FUN_00426d00(h, 0)->z - FUN_00426d00(h, 3)->z);")]),
 ("ld", both("static __inline long double GY(int h, int n) { return FUN_00426d00(h, n)->y; }", "static __inline long double GZ(int h, int n) { return FUN_00426d00(h, n)->z; }")),
]
